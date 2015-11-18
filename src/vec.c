#include "vec.h"

#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#define POOL_SIZE 65536
#define POOL_INDEX(p) ((size_t)((pool_item_t*)(p)-pool)/sizeof(pool_item_t))

//The pool is a bit faster with lots of alloc_vec and free_vec calls.
//Though it can be a bit slower.
//#define USE_POOL

#ifdef USE_POOL
typedef struct {
    void *ptr;
    int next;
} pool_item_t;

static pool_item_t pool[POOL_SIZE];
static int next = -72;
#endif

static void* _alloc_vec(size_t amount) {
    void* vec = malloc(amount+sizeof(size_t));
    *((size_t*)vec) = amount;
    return vec;
}

static void _free_vec(void* vec) {
    free(vec);
}

static size_t _get_vec_size(void* vec) {
    return *(size_t*)vec;
}

static void* _resize_vec(void* vec, size_t new_size) {
    void* data = realloc(vec, new_size+sizeof(size_t));
    *(size_t*)data = new_size;
    return data;
}

static void* _append_vec(void* vec, size_t amount, void* data) {
    size_t old_size = _get_vec_size(vec);
    vec = _resize_vec(vec, old_size+amount);
    memcpy((uint8_t*)vec+sizeof(size_t)+old_size, data, amount);
    return vec;
}

#ifdef USE_POOL
static void _init_pool() {
    if (next == -72) {
        for (size_t i = 0; i < POOL_SIZE-1; ++i) {
            pool[i].ptr = NULL;
            pool[i].next = i+1;
        }
        pool[POOL_SIZE-1].ptr = NULL;
        pool[POOL_SIZE-1].next = -1;
        
        next = 0;
    }
}
#endif

vec_t alloc_vec(size_t amount) {
    #ifdef USE_POOL
    _init_pool();
    #endif
    
    void* ptr = _alloc_vec(amount);
    #ifdef USE_POOL
    if (next == -1) {
    #endif
        void** result = malloc(sizeof(void*));
        *result = ptr;
        return result;
    #ifdef USE_POOL
    }
    
    int pool_index = next;
    next = pool[pool_index].next;
    pool[pool_index].ptr = ptr;
    
    return pool+pool_index;
    #endif
}

void free_vec(vec_t vec) {
    #ifdef USE_POOL
    if ((vec < (void*)pool) || (vec >= (void*)(pool+POOL_SIZE))) {
    #endif
        free(*(void**)vec);
        free(vec);
    #ifdef USE_POOL
    } else {
        int pool_index = POOL_INDEX(vec);
        
        free((size_t*)pool[pool_index].ptr);
        pool[pool_index].ptr = NULL;
        
        pool[pool_index].next = next;
        next = pool_index;
    }
    #endif
}

static void _set_vec_data(vec_t vec, void* data) {
    #ifdef USE_POOL
    if ((vec < (void*)pool) || (vec >= (void*)(pool+POOL_SIZE))) {
    #endif
        *(void**)vec = data;
    #ifdef USE_POOL
    } else {
        pool[POOL_INDEX(vec)].ptr = data;
    }
    #endif
}

void* _get_vec_data(vec_t vec) {
    #ifdef USE_POOL
    if ((vec < (void*)pool) || (vec >= (void*)(pool+POOL_SIZE))) {
    #endif
        return *(void**)vec;
    #ifdef USE_POOL
    } else {
        return pool[POOL_INDEX(vec)].ptr;
    }
    #endif
}

void append_vec(vec_t vec, size_t amount, void* data) {
    _set_vec_data(vec, _append_vec(_get_vec_data(vec), amount, data));
}

void resize_vec(vec_t vec, size_t new_size) {
    _set_vec_data(vec, _resize_vec(_get_vec_data(vec), new_size));
}

size_t get_vec_size(vec_t vec) {
    return _get_vec_size(_get_vec_data(vec));
}

void* get_vec_data(vec_t vec) {
    return (size_t*)_get_vec_data(vec)+1;
}
