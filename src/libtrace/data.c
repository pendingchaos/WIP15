#include "libtrace.h"

#include <stdatomic.h>
#include <malloc.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>
#if ZLIB_ENABLED
#include <zlib.h>
#endif
#if LZ4_ENABLED
#include <lz4.h>
#endif
#if ZSTD_ENABLED
#include <zstd.h>
#endif

#define DATA_CONTAINER_SIZE (8*1024*1024) //Max: 2^28
#define DATA_CONTAINER_THRESHOLD (2*1024*1024) //should be <= DATA_CONTAINER_SIZE
#define MIN_DATA_LOCK_LIST_CAPACITY 256
#define QUEUE_CAPACITY 1024
//milliseconds since last accessed until a container can be compressed
#define COMPRESS_CONTAINER_THRESHOLD 2000
//milliseconds since last accessed until a independent data can be compessed
//anything that makes any signifigant performance changes always comes with great memory usage
//so it's zero
#define COMPRESS_DATA_THRESHOLD 0
#define MAX_MAPPINGS 1024
#define MAX_DATA_LOCKS 2048
#define MAX_THREADS 1024

typedef struct data_state_t {
    trc_data_t* first_data;
    pthread_mutex_t first_container_lock;
    trc_data_container_t* first_container;
} data_state_t;

typedef struct data_mapping_t {
    trc_data_t* data;
    bool free_ptr;
    uint32_t flags;
    void* ptr;
    int line;
    const char* file;
    const char* func;
} data_mapping_t;

typedef struct data_lock_t {
    struct data_lock_t* next;
    int ref_count;
    trc_data_t* data;
    pthread_rwlock_t lock;
} data_lock_t;

typedef struct queue_entry_t {
    trc_data_container_t* container;
    trc_data_t* data;
} queue_entry_t;

static pthread_mutex_t data_locks_mutex = PTHREAD_MUTEX_INITIALIZER;
static bool data_locks_initialized = false;
static data_lock_t data_locks[MAX_DATA_LOCKS];
static data_lock_t* next_data_lock = NULL;

static pthread_mutex_t queue_mutex = PTHREAD_MUTEX_INITIALIZER;
static size_t queue_start = 0;
static size_t queue_size = 0;
static queue_entry_t queue[QUEUE_CAPACITY];

static bool threads_running;
static bool pause_threads;
static size_t threads_paused;
static size_t thread_count;
static pthread_t* threads;

static data_mapping_t mappings[MAX_MAPPINGS];

static uint64_t get_milliseconds() {
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC_RAW, &ts);
    return (uint64_t)ts.tv_sec*1000 + ts.tv_nsec/1000000;
}

static trc_data_container_t** get_data_container(trc_data_t* data) {
    return (trc_data_container_t**)(data+1);
}

static trc_indep_storage_t* get_data_indep(trc_data_t* data) {
    return (trc_indep_storage_t*)(data+1);
}

static bool lock_rwlock(pthread_rwlock_t* lock, bool write, bool try_lock) {
    if (try_lock && write)
        return pthread_rwlock_trywrlock(lock) == 0;
    else if (try_lock)
        return pthread_rwlock_tryrdlock(lock) == 0;
    else if (write)
        pthread_rwlock_wrlock(lock);
    else
        pthread_rwlock_rdlock(lock);
    return true;
}

//FNV hash
static uint32_t hash(size_t count, const uint8_t* data) {
    uint32_t hash = 2166136261;
    for (size_t i = 0; i < count; i++)
        hash = (hash*16777619) ^ data[i];
    return hash;
}

static void dec_lock_ref_count(data_lock_t* lock) {
    if (--lock->ref_count == 0) {
        lock->data = NULL;
        lock->next = next_data_lock;
        next_data_lock = lock;
    }
}

static bool lock_data(trc_data_t* data, bool write, bool try_lock) {
    pthread_mutex_lock(&data_locks_mutex);
    
    if (!data_locks_initialized) {
        for (size_t i = 0; i < MAX_DATA_LOCKS; i++) {
            data_locks[i].next = i==MAX_DATA_LOCKS-1 ? NULL : &data_locks[i+1];
            data_locks[i].ref_count = 0;
            data_locks[i].data = NULL;
            data_locks[i].lock = (pthread_rwlock_t)PTHREAD_RWLOCK_INITIALIZER;
        }
        next_data_lock = &data_locks[0];
        data_locks_initialized = true;
    }
    
    uint32_t index = hash(sizeof(trc_data_t*), (const void*)&data) % MAX_DATA_LOCKS;
    data_lock_t* lock = NULL;
    for (size_t i = 0; i < MAX_DATA_LOCKS; i++) {
        if (data_locks[index].data == NULL) {
            lock = &data_locks[index];
            break;
        } else if (data_locks[index].data == data) {
            data_locks[index].ref_count++;
            pthread_mutex_unlock(&data_locks_mutex);
            bool success = lock_rwlock(&data_locks[index].lock, write, try_lock);
            if (!success) {
                pthread_mutex_lock(&data_locks_mutex);
                dec_lock_ref_count(&data_locks[index]);
                pthread_mutex_unlock(&data_locks_mutex);
            }
            return success;
        }
        index = (index+1) % MAX_DATA_LOCKS;
    }
    assert(lock);
    
    lock->ref_count = 1;
    lock->data = data;
    next_data_lock = lock->next;
    
    pthread_mutex_unlock(&data_locks_mutex);
    
    bool success = lock_rwlock(&lock->lock, write, try_lock);
    if (!success) {
        pthread_mutex_lock(&data_locks_mutex);
        dec_lock_ref_count(lock);
        pthread_mutex_unlock(&data_locks_mutex);
    }
    
    return success;
}

static void unlock_data(trc_data_t* data) {
    pthread_mutex_lock(&data_locks_mutex);
    uint32_t index = hash(sizeof(trc_data_t*), (const void*)&data) % MAX_DATA_LOCKS;
    for (size_t i = 0; i < MAX_DATA_LOCKS; i++) {
        if (data_locks[index].data == data) {
            pthread_rwlock_unlock(&data_locks[index].lock);
            dec_lock_ref_count(&data_locks[index]);
            break;
        }
        index = (index+1) % MAX_DATA_LOCKS;
    }
    pthread_mutex_unlock(&data_locks_mutex);
}

static void enqueue_for_compression(queue_entry_t entry) {
    while (true) {
        pthread_mutex_lock(&queue_mutex);
        if (queue_size == QUEUE_CAPACITY) {
            pthread_mutex_unlock(&queue_mutex);
            goto wait;
        }
        
        queue[(queue_start+queue_size)%QUEUE_CAPACITY] = entry;
        queue_size++;
        
        pthread_mutex_unlock(&queue_mutex);
        break;
        
        wait:;
        struct timespec sleep_time = {.tv_sec=0, .tv_nsec=1000};
        nanosleep(&sleep_time, NULL);
    }
}

static trc_data_container_t* find_container(trace_t* trace, size_t data_size) {
    data_state_t* state = trace->data_state;
    
    pthread_mutex_lock(&state->first_container_lock);
    
    trc_data_container_t* cur = NULL;
    for (trc_data_container_t* c = state->first_container; c; c = c->next) {
        bool usable = DATA_CONTAINER_SIZE-c->data_usage >= data_size;
        if (!usable) continue;
        if (!cur) {
            cur = c;
        } else if (c->compression==TrcCompression_None && cur->compression!=TrcCompression_None) {
            cur = c; //Always prefer uncompressed containers over compressed ones
        } else if (cur && c->last_accessed > cur->last_accessed) {
            if (c->compression!=TrcCompression_None && cur->compression==TrcCompression_None)
                continue;
            cur = c; //Prefer more recently accessed containers
        }
    }
    if (!cur) { //No container found
        cur = malloc(sizeof(trc_data_container_t));
        cur->lock = (pthread_rwlock_t)PTHREAD_RWLOCK_INITIALIZER;
        
        cur->last_accessed = get_milliseconds();
        cur->data_usage = 0;
        cur->compression = TrcCompression_None;
        cur->compressed_size = DATA_CONTAINER_SIZE;
        cur->data = calloc(1, DATA_CONTAINER_SIZE);
        
        cur->prev = NULL;
        cur->next = state->first_container;
        if (state->first_container) state->first_container->prev = cur;
        state->first_container = cur;
        
        pthread_rwlock_rdlock(&cur->lock);
        
        enqueue_for_compression((queue_entry_t){.data=NULL, .container=cur});
    } else {
        pthread_rwlock_rdlock(&cur->lock);
    }
    
    pthread_mutex_unlock(&state->first_container_lock);
    
    return cur;
}

static void decompress_data_ip(trc_compression_t compression, size_t size,
                               size_t compressed_size, const void* compressed, void* dest) {
    switch (compression) {
    case TrcCompression_None:
        memcpy(dest, compressed, compressed_size);
        break;
    #if LZ4_ENABLED
    case TrcCompression_LZ4: {
        if (LZ4_decompress_fast(compressed, dest, size) < 0)
            assert(false);
        break;
    }
    #endif
    #if ZLIB_ENABLED
    case TrcCompression_Zlib: {
        uLongf _ = size;
        if (uncompress(dest, &_, compressed, compressed_size) != Z_OK)
            assert(false);
        break;
    }
    #endif
    #if ZSTD_ENABLED
    case TrcCompression_Zstd: {
        size_t written =
            ZSTD_decompress(dest, size, compressed, compressed_size);
        if (ZSTD_isError(written) || written!=size)
            assert(false);
        break;
    }
    #endif
    default:
        assert(false);
        break;
    }
}

static void* decompress_data(trc_compression_t compression, size_t size,
                             size_t compressed_size, const void* compressed) {
    void* res = malloc(size);
    decompress_data_ip(compression, size, compressed_size, compressed, res);
    return res;
}

//Assumes a lock is held on the container
static void decompress_container(trc_data_container_t* container) {
    if (container->compression == TrcCompression_None)
        return;
    
    void* data = decompress_data(container->compression, DATA_CONTAINER_SIZE,
                                 container->compressed_size, container->data);
    free(container->data);
    container->data = data;
    container->compressed_size = DATA_CONTAINER_SIZE;
    container->compression = TrcCompression_None;
    
    enqueue_for_compression((queue_entry_t){.data=NULL, .container=container});
}

static void compress_data(size_t src_size, void* src, trc_compression_t* dst_compression,
                          size_t* dst_size, void** dest) {
    void* compressed = malloc(src_size-1);
    trc_compression_t compression = TrcCompression_Zstd;
    size_t compressed_size;
    
    #if ZSTD_ENABLED
    //Try Zstandard
    if (src_size > 2*1024*1024) { //Zstandard works best with large amounts of data
        compressed_size = ZSTD_compress(compressed, src_size-1, src, src_size, 15);
        if (!ZSTD_isError(compressed_size)) goto compression_done;
    }
    #endif
    
    #if ZLIB_ENABLED
    //Try Zlib
    compression = TrcCompression_Zlib;
    uLongf zlib_compressed_size = src_size - 1;
    if (compress2(compressed, &zlib_compressed_size, src, src_size, 9) == Z_OK) {
        compressed_size = zlib_compressed_size;
        goto compression_done;
    }
    #endif
    
    #if LZ4_ENABLED
    //Try LZ4
    compression = TrcCompression_LZ4;
    if ((compressed_size=LZ4_compress_default(src, compressed, src_size, src_size-1)))
        goto compression_done;
    #endif
    
    //Compression failed
    free(compressed);
    malloc_trim(0);
    *dest = src;
    *dst_compression = TrcCompression_None;
    *dst_size = src_size;
    return;
    
    compression_done:
    free(src);
    compressed = realloc(compressed, compressed_size);
    malloc_trim(0);
    *dest = compressed;
    *dst_compression = compression;
    *dst_size = compressed_size;
}

void* compress_thread(void* _) {
    while (atomic_load(&threads_running)) {
        if (atomic_load(&pause_threads)) {
            atomic_fetch_add(&threads_paused, 1);
            while (atomic_load(&pause_threads)) ;
            atomic_fetch_sub(&threads_paused, 1);
        }
        
        pthread_mutex_lock(&queue_mutex);
        if (!queue_size) {
            pthread_mutex_unlock(&queue_mutex);
            goto wait;
        }
        queue_entry_t entry = queue[queue_start];
        queue_start = (queue_start+1) % QUEUE_CAPACITY;
        queue_size--;
        pthread_mutex_unlock(&queue_mutex);
        
        if (entry.data) {
            trc_data_t* data = entry.data;
            if (!lock_data(data, true, true)) goto add_to_queue;
            
            if (data->storage!=TrcDataStorage_Independent ||
                get_data_indep(data)->compression!=TrcCompression_None) {
                unlock_data(data);
                continue;
            }
            if (get_milliseconds()-get_data_indep(data)->last_accessed < COMPRESS_DATA_THRESHOLD) {
                unlock_data(data);
                goto add_to_queue;
            }
            
            trc_indep_storage_t* indep = get_data_indep(data);
            
            trc_compression_t compression;
            size_t size;
            void* new_data;
            compress_data(data->size, indep->data, &compression, &size, &new_data);
            
            indep->compressed_size = size;
            indep->compression = compression;
            indep->data = new_data;
            
            unlock_data(data);
        } else if (entry.container) {
            trc_data_container_t* c = entry.container;
            
            pthread_rwlock_wrlock(&c->lock);
            if (c->compression != TrcCompression_None) {
                pthread_rwlock_unlock(&c->lock);
                continue;
            }
            if (get_milliseconds()-c->last_accessed < COMPRESS_CONTAINER_THRESHOLD) {
                pthread_rwlock_unlock(&c->lock);
                goto add_to_queue;
            }
            
            compress_data(DATA_CONTAINER_SIZE, c->data, &c->compression,
                          &c->compressed_size, (void**)&c->data);
            
            c->last_accessed = get_milliseconds();
            
            pthread_rwlock_unlock(&c->lock);
        }
        
        continue;
        add_to_queue:
        enqueue_for_compression(entry);
        wait:;
        struct timespec sleep_time = {.tv_sec=0, .tv_nsec=1000};
        nanosleep(&sleep_time, NULL);
    }
    
    return NULL;
}

static trc_data_t* create_data(trace_t* trace, size_t size, trc_compression_t compression,
                               size_t compressed_size, void* data, uint32_t flags, bool copy) {
    trc_data_t* res;
    if (size < 4096) {
        res = malloc(sizeof(trc_data_t)+size);
        res->size = size;
        res->storage = TrcDataStorage_Tiny;
        
        if (data) decompress_data_ip(compression, size, compressed_size, data, res+1);
        else memset(res+1, 0, size);
        
        if (!copy) free(data);
    } else if (size <= DATA_CONTAINER_THRESHOLD) {
        res = malloc(sizeof(trc_data_t)+sizeof(trc_data_container_t*));
        res->size = size;
        res->storage = TrcDataStorage_Container;
        
        trc_data_container_t* container = find_container(trace, size);
        *get_data_container(res) = container;
        container->last_accessed = get_milliseconds();
        
        decompress_container(container);
        
        res->storage_data = container->data_usage;
        uint8_t* dest = container->data + container->data_usage;
        if (data) decompress_data_ip(compression, size, compressed_size, data, dest);
        else ; //no need to zero out memory - it was calloc'd
        container->data_usage += size;
        
        if (!copy) free(data);
        
        pthread_rwlock_unlock(&container->lock);
    } else {
        res = malloc(sizeof(trc_data_t)+sizeof(trc_indep_storage_t));
        res->size = size;
        
        trc_indep_storage_t* indep = get_data_indep(res);
        
        res->storage = TrcDataStorage_Independent;
        res->storage_data = 0;
        indep->last_accessed = get_milliseconds();
        indep->compressed_size = compressed_size;
        indep->compression = compression;
        
        if (data) {
            if (copy) {
                indep->data = malloc(compressed_size);
                memcpy(indep->data, data, compressed_size);
            } else {
                indep->data = data;
            }
        } else { //This bit only really makes sense for uncompressed data
            if (flags & TRC_DATA_NO_ZERO)
                indep->data = malloc(compressed_size);
            else
                indep->data = calloc(compressed_size, 1);
        }
        
        if (compression == TrcCompression_None)
            enqueue_for_compression((queue_entry_t){.data=res, .container=NULL});
    }
    
    data_state_t* state = trace->data_state;
    res->next = state->first_data;
    state->first_data = res;
    
    return res;
}

trc_data_t* trc_create_data(trace_t* trace, size_t size, const void* data, uint32_t flags) {
    return create_data(trace, size, TrcCompression_None, size, (void*)data, flags, true);
}

trc_data_t* trc_create_data_no_copy(trace_t* trace, size_t size, void* data, uint32_t flags) {
    return create_data(trace, size, TrcCompression_None, size, data, flags, false);
}

trc_data_t* trc_create_compressed_data_no_copy(trace_t* trace, trc_compressed_data_t data) {
    return create_data(trace, data.size, data.compression, data.compressed_size,
                       data.compressed_data, 0, false);
}

//void* trc_map_data(trc_data_t* data, uint32_t flags) {
void* _trc_map_data(trc_data_t* data, uint32_t flags, int line, const char* file, const char* func) {
    if (data->storage != TrcDataStorage_Tiny) lock_data(data, flags&TRC_MAP_WRITE, false);
    
    data_mapping_t mapping;
    mapping.data = data;
    mapping.free_ptr = false;
    mapping.flags = flags;
    mapping.line = line;
    mapping.file = file;
    mapping.func = func;
    switch (data->storage) {
    case TrcDataStorage_Container: {
        trc_data_container_t* c = *get_data_container(data);
        pthread_rwlock_rdlock(&c->lock);
        decompress_container(c);
        c->last_accessed = get_milliseconds();
        mapping.ptr = c->data + data->storage_data;
        break;
    }
    case TrcDataStorage_Independent: {
        trc_indep_storage_t* indep = get_data_indep(data);
        indep->last_accessed = get_milliseconds();
        if (indep->compression == TrcCompression_None) {
            mapping.ptr = indep->data;
            mapping.free_ptr = false;
        } else {
            mapping.ptr = decompress_data(indep->compression, data->size,
                                          indep->compressed_size, indep->data);
            mapping.free_ptr = !(flags&TRC_MAP_WRITE);
        }
        break;
    }
    case TrcDataStorage_Tiny: {
        mapping.ptr = data + 1;
        mapping.free_ptr = false;
        break;
    }
    }
    
    for (size_t i = 0; i < MAX_MAPPINGS; i++) {
        if (mappings[i].data == NULL) {
            mappings[i] = mapping;
            return mapping.ptr;
        }
    }
    assert(false);
    return NULL;
}

void trc_unmap_data(const void* mapped_ptr) {
    if (!mapped_ptr) return;
    
    data_mapping_t* mapping = NULL;
    for (size_t i = 0; i < MAX_MAPPINGS; i++) {
        if (mappings[i].ptr == mapped_ptr) {
            mapping = &mappings[i];
            break;
        }
    }
    assert(mapping);
    
    trc_data_t* data = mapping->data;
    
    switch (data->storage) {
    case TrcDataStorage_Container:
        pthread_rwlock_unlock(&(*get_data_container(data))->lock);
        break;
    case TrcDataStorage_Independent: {
        trc_indep_storage_t* indep = get_data_indep(data);
        if (indep->compression!=TrcCompression_None)
            enqueue_for_compression((queue_entry_t){.data=data, .container=NULL});
        if (mapping->flags&TRC_MAP_WRITE && indep->compression!=TrcCompression_None) {
            free(indep->data);
            indep->compression = TrcCompression_None;
            indep->compressed_size = data->size;
            indep->data = mapping->ptr;
        }
        indep->last_accessed = get_milliseconds();
        break;
    }
    case TrcDataStorage_Tiny: {
        break;
    }
    }
    
    free(mapping->free_ptr ? mapping->ptr : NULL);
    mapping->data = NULL;
    mapping->ptr = NULL;
    
    if (data->storage != TrcDataStorage_Tiny) unlock_data(data);
}

void __attribute__((constructor)) init() {
    thread_count = sysconf(_SC_NPROCESSORS_ONLN) * 0.375;
    if (thread_count < 1) thread_count = 1;
    if (thread_count > MAX_THREADS) thread_count = MAX_THREADS;
    
    atomic_store(&threads_running, true);
    threads = malloc(sizeof(pthread_t)*thread_count);
    for (size_t i = 0; i < thread_count; i++)
        pthread_create(&threads[i], NULL, &compress_thread, NULL);
}

void __attribute__((destructor)) deinit() {
    atomic_store(&threads_running, false);
    for (size_t i = 0; i < thread_count; i++)
        pthread_join(threads[i], NULL);
    free(threads);
}

void _trc_data_init(trace_t* trace) {
    data_state_t* state = malloc(sizeof(data_state_t));
    state->first_data = NULL;
    state->first_container_lock = (pthread_mutex_t)PTHREAD_MUTEX_INITIALIZER;
    state->first_container = NULL;
    trace->data_state = state;
}

static bool data_belongs_to_trace(data_state_t* state, trc_data_t* data) {
    if (!data) return false;
    for (trc_data_t* cur = state->first_data; cur; cur=cur->next) {
        if (cur == data) return true;
    }
    return false;
}

static bool container_belongs_to_trace(data_state_t* state, trc_data_container_t* container) {
    if (!container) return false;
    for (trc_data_container_t* cur = state->first_container; cur; cur=cur->next) {
        if (cur == container) return true;
    }
    return false;
}

void _trc_data_deinit(trace_t* trace) {
    data_state_t* state = trace->data_state;
    
    //Check for mapping leaks
    for (size_t i = 0; i < MAX_MAPPINGS; i++) {
        data_mapping_t m = mappings[i];
        if (data_belongs_to_trace(state, m.data)) {
            printf("trc_data_t %p was leaked at line %d of the file '%s' (in the function '%s')\n",
                   m.data, m.line, m.file, m.func);
        }
    }
    
    //Pause threads so they hold no references to any data objects or containers
    atomic_store(&pause_threads, true);
    while (atomic_load(&threads_paused) != thread_count) ;
    
    //Remove data objects and containers from queue
    pthread_mutex_lock(&queue_mutex);
    for (size_t i = 0; i < queue_size;) {
        size_t index = (i+queue_start) % QUEUE_CAPACITY;
        queue_entry_t e = queue[index];
        if (data_belongs_to_trace(state, e.data) || container_belongs_to_trace(state, e.container)) {
            //Remove entry
            for (size_t j = i; j < queue_size-1; j++)
                queue[(queue_start+j)%QUEUE_CAPACITY] = queue[(queue_start+j+1)%QUEUE_CAPACITY];
            queue_size--;
        } else {
            i++;
        }
    }
    pthread_mutex_unlock(&queue_mutex);
    
    //Unpause
    atomic_store(&pause_threads, false);
    
    //Free data objects
    for (trc_data_t* cur = state->first_data; cur;) {
        trc_data_t* next = cur->next;
        
        if (cur->storage == TrcDataStorage_Independent)
            free(get_data_indep(cur));
        free(cur);
        
        cur = next;
    }
    
    //Free container objects
    for (trc_data_container_t* cur = state->first_container; cur;) {
        trc_data_container_t* next = cur->next;
        free(cur->data);
        free(cur);
        cur = next;
    }
    
    free(state);
}
