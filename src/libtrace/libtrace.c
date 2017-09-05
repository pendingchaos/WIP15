#include "libtrace/libtrace.h"

#include <assert.h>
#include <malloc.h>
#include <stdatomic.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <endian.h>
#include <dlfcn.h>
#include <unistd.h>
#include <sys/wait.h>
#include <SDL2/SDL.h>
#if ZLIB_ENABLED
#include <zlib.h>
#endif
#if LZ4_ENABLED
#include <lz4.h>
#endif

typedef enum {
    BaseType_Void = 0,
    BaseType_UnsignedInt = 1,
    BaseType_Int = 2,
    BaseType_Ptr = 3,
    BaseType_Bool = 4,
    BaseType_Float = 5,
    BaseType_Double = 6,
    BaseType_String = 7,
    BaseType_Data = 8,
    BaseType_FunctionPtr = 9,
    BaseType_Variant = 10
} base_type_t;

typedef enum {
    Op_DeclFunc = 0,
    Op_DeclGroup = 1,
    Op_Call = 2
} opcode_t;

typedef struct {
    base_type_t base;
    bool has_group;
    bool is_array;
} type_t;

typedef struct {
    char* name;
    type_t result;
    uint32_t arg_count;
    type_t* args;
} func_decl_t;

static trace_error_t trace_error = TraceError_None;
static const char *trace_error_desc = "";

#define MAX_THREADS 1024

#define MAX_MAPPINGS 1024
typedef struct data_mapping_t {
    trc_data_t* data;
    uint32_t flags;
    bool free_ptr;
    void* ptr;
} data_mapping_t;
static pthread_mutex_t mapping_mutex = PTHREAD_MUTEX_INITIALIZER;
static data_mapping_t mappings[MAX_MAPPINGS];

static void* compress_thread(void* userdata);

bool trace_program(int* exitcode, size_t count, ...) {
    const char** arguments = NULL;
    const char* output_filename = NULL;
    const char* limits = NULL;
    int compression = 60;
    const char* lib_path = NULL;
    
    va_list list;
    va_start(list, count);
    for (size_t i = 0; i < count; i++) {
        switch (va_arg(list, trc_trace_program_arg_t)) {
        case TrcProgramArguments:
            arguments = va_arg(list, const char**);
            break;
        case TrcOutputFilename:
            output_filename = va_arg(list, const char*);
            break;
        case TrcLimitsFilename:
            limits = va_arg(list, const char*);
            break;
        case TrcCompression:
            compression = va_arg(list, int);
            break;
        case TrcLibGL:
            lib_path = va_arg(list, const char*);
            break;
        }
    }
    va_end(list);
    
    bool failure = compression<0 || compression>100;
    failure = failure || !arguments || !output_filename || !limits || !lib_path;
    if (failure) return false;
    
    //TODO: More error checking?
    pid_t pid;
    if (!(pid=fork())) {
        setenv("WIP15_LIMITS", limits, 1);
        setenv("WIP15_OUTPUT", output_filename, 1);
        char buf[16];
        sprintf(buf, "%d", compression);
        setenv("WIP15_COMPRESSION_LEVEL", buf, 1);
        setenv("SDL_OPEN_LIBRARY", lib_path, 1);
        setenv("LD_PRELOAD", lib_path, 1);
        
        execv(arguments[0], (char*const*)arguments);
    } else {
        int wstatus;
        waitpid(pid, &wstatus, 0);
        *exitcode = WEXITSTATUS(wstatus);
    }
    
    return true;
}

static size_t readf(void* ptr, size_t size, size_t count, FILE* stream) {
    size_t res = fread(ptr, size, count, stream);
    return size*count == 0 ? count : res;
}

void trc_free_value(trace_value_t value) {
    if ((value.count == 1) && (value.type == Type_Str))
        free(value.str);
    else if (value.count != 1)
        switch (value.type) {
        case Type_UInt:
        case Type_Int:
        case Type_Double:
        case Type_Boolean:
        case Type_Ptr:
            free(value.u64_array);
            break;
        case Type_Str:
            for (size_t i = 0; i < value.count; ++i)
                free(value.str_array[i]);
            free(value.str_array);
            break;
        case Type_Data:
            free(value.data_array);
            break;
        case Type_Void:
        case Type_FunctionPtr:
            break;
        }
}

static void free_command(trace_command_t* command) {
    for (size_t i = 0; i < command->arg_count; i++)
        trc_free_value(command->args[i]);
    free(command->args);
    trc_free_value(command->ret);
    
    for (size_t i = 0; i < command->extra_count; i++) {
        free(command->extras[i].name);
        free(command->extras[i].data);
    }
    free(command->extras);
    
    trc_attachment_t* attach = command->attachments;
    while (attach) {
        trc_attachment_t* next_attach = attach->next;
        free(attach->message);
        free(attach);
        attach = next_attach;
    }
}

static void free_frame(trace_frame_t* frame) {
    for (size_t i = 0; i < frame->command_count; i++) free_command(&frame->commands[i]);
    free(frame->commands);
}

static char* read_str(FILE* file) {
    uint32_t length;
    if (!readf(&length, 4, 1, file))
        return NULL;
    length = le32toh(length);
    
    char* str = (char*)malloc(length+1);
    if (!readf(str, length, 1, file)) {
        free(str);
        return NULL;
    }
    str[length] = 0;
    
    return str;
}

static void* read_data(FILE* file, size_t* res_size) {
    if (res_size) *res_size = 0;
    
    uint8_t compression_method;
    if (!readf(&compression_method, 1, 1, file)) return NULL;
    
    uint32_t size;
    if (!readf(&size, 4, 1, file)) return NULL;
    size = le32toh(size);
    
    uint32_t compressed_size;
    if (!readf(&compressed_size, 4, 1, file)) return NULL;
    compressed_size = le32toh(compressed_size);
    
    void* compressed_data = malloc(compressed_size);
    if (!readf(compressed_data, compressed_size, 1, file)) {
        free(compressed_data);
        return NULL;
    }
    
    if (compression_method == 0) {
        if (res_size) *res_size = size;
        return compressed_data;
    }
    #if ZLIB_ENABLED
    if (compression_method == 1) {
        void* data = malloc(size);
        
        uLongf dest_len = size;
        if (uncompress(data, &dest_len, compressed_data, compressed_size) != Z_OK) {
            free(compressed_data);
            free(data);
            return NULL;
        }
        
        free(compressed_data);
        
        if (res_size) *res_size = size;
        return data;
    }
    #endif
    #if LZ4_ENABLED
    if (compression_method == 2) {
        void* data = malloc(size);
        
        if (LZ4_decompress_safe(compressed_data, data, compressed_size, size) < 0) {
            free(compressed_data);
            free(data);
            return NULL;
        }
        
        free(compressed_data);
        
        if (res_size) *res_size = size;
        return data;
    }
    #endif
    free(compressed_data);
    return NULL;
}

static trc_data_t* read_data_compressed(trace_t* trace, FILE* file) {
    uint8_t compression_method;
    if (!readf(&compression_method, 1, 1, file)) return NULL;
    
    uint32_t size;
    if (!readf(&size, 4, 1, file)) return NULL;
    size = le32toh(size);
    
    uint32_t compressed_size;
    if (!readf(&compressed_size, 4, 1, file)) return NULL;
    compressed_size = le32toh(compressed_size);
    
    trc_compressed_data_t data;
    data.compressed_data = malloc(compressed_size);
    if (!readf(data.compressed_data, compressed_size, 1, file)) {
        free(data.compressed_data);
        return NULL;
    }
    
    data.size = size;
    data.compressed_size = compressed_size;
    switch (compression_method) {
    case 0:
        data.compression = TrcCompression_None;
        break;
    #if ZLIB_ENABLED
    case 1:
        data.compression = TrcCompression_Zlib;
        break;
    #endif
    #if LZ4_ENABLED
    case 2:
        data.compression = TrcCompression_LZ4;
        break;
    #endif
    default:
        free(data.compressed_data);
        return NULL;
    }
    
    return trc_create_compressed_data_no_copy(trace, data);
}

static uint64_t read_uleb128(FILE* file, bool* error) {
    uint64_t v = 0;
    uint shift = 0;
    uint8_t b;
    do {
        if (!readf(&b, 1, 1, file)) {
            *error = true;
            return 0;
        }
        v |= (uint64_t)(b&0x7f) << shift;
        shift += 7;
    } while (b & 0x80);
    *error = false;
    return v;
}

static int64_t read_sleb128(FILE* file, bool* error) {
    uint64_t i = read_uleb128(file, error);
    bool sign = i&1;
    i >>= 1;
    return sign ? -(int64_t)i : i;
}

//Returns true on success
static bool read_val(FILE* file, trace_value_t* val, type_t* type, trace_t* trace) {
    if (type->is_array) {
        if (!readf(&val->count, 4, 1, file))
            return false;
        val->count = le32toh(val->count);
    } else {
        val->count = 1;
    }
    
    base_type_t base = type->base;
    while (true) {
        switch (base) {
        case BaseType_Void: {
            val->type = Type_Void;
            break;
        }
        case BaseType_UnsignedInt: {
            val->type = Type_UInt;
            bool error;
            if (val->count == 1) {
                val->u64 = read_uleb128(file, &error);
                if (error) return false;
            } else {
                val->u64_array = malloc(sizeof(uint64_t)*val->count);
                
                for (size_t i = 0; i < val->count; ++i) {
                    val->u64_array[i] = read_uleb128(file, &error);
                    if (error) {
                        free(val->u64_array);
                        return false;
                    }
                }
            }
            break;
        }
        case BaseType_Int: {
            val->type = Type_Int;
            bool error;
            if (val->count == 1) {
                val->i64 = read_sleb128(file, &error);
                if (error) return false;
            } else {
                val->i64_array = malloc(sizeof(int64_t)*val->count);
                
                for (size_t i = 0; i < val->count; i++) {
                    val->i64_array[i] = read_sleb128(file, &error);
                    if (error) {
                        free(val->i64_array);
                        return false;
                    }
                }
            }
            break;
        }
        case BaseType_Ptr: {
            val->type = Type_Ptr;
            bool error;
            if (val->count == 1) {
                val->ptr = read_uleb128(file, &error);
                if (error) return false;
            } else {
                val->ptr_array = malloc(sizeof(uint64_t)*val->count);
                
                for (size_t i = 0; i < val->count; i++) {
                    val->ptr_array[i] = read_uleb128(file, &error);
                    if (error) {
                        free(val->ptr_array);
                        return false;
                    }
                }
            }
            break;
        }
        case BaseType_Bool: {
            val->type = Type_Boolean;
            if (val->count == 1) {
                uint8_t v;
                if (!readf(&v, 1, 1, file)) return false;
                val->bl = v;
            } else {
                val->bl_array = malloc(val->count);
                
                for (size_t i = 0; i < val->count; i++) {
                    uint8_t v;
                    if (!readf(&v, 1, 1, file)) {
                        free(val->bl_array);
                        return false;
                    }
                    val->bl_array[i] = v;
                }
            }
            break;
        }
        case BaseType_Float: { //TODO: Make this more portable.
            val->type = Type_Double;
            if (val->count == 1) {
                float v;
                if (!readf(&v, 4, 1, file)) return false;
                val->dbl = v;
            } else {
                val->dbl_array = malloc(sizeof(double)*val->count);
                
                for (size_t i = 0; i < val->count; i++) {
                    float v;
                    if (!readf(&v, 4, 1, file)) {
                        free(val->dbl_array);
                        return false;
                    }
                    val->dbl_array[i] = v;
                }
            }
            break;
        }
        case BaseType_Double: { //TODO: Make this more portable.
            val->type = Type_Double;
            if (val->count == 1) {
                if (!readf(&val->dbl, 8, 1, file)) return false;
            } else {
                val->dbl_array = malloc(sizeof(double)*val->count);
                
                for (size_t i = 0; i < val->count; i++)
                    if (!readf(val->dbl_array+i, 8, 1, file))
                        return false;
            }
            break;
        }
        case BaseType_String: {
            val->type = Type_Str;
            if (val->count == 1) {
                val->str = read_str(file);
                if (!val->str) return false;
            } else {
                val->str_array = malloc(sizeof(char*)*val->count);
                
                for (size_t i = 0; i < val->count; i++) {
                    val->str_array[i] = read_str(file);
                    if (!val->str_array[i]) {
                        for (size_t j = 0; j < i; j++)
                            free(val->str_array[i]);
                        free(val->str_array);
                        return false;
                    }
                }
            }
            break;
        }
        case BaseType_Data: {
            val->type = Type_Data;
            if (val->count == 1) {
                val->data = read_data_compressed(trace, file);
                if (!val->data) return false;
            } else {
                val->data_array = malloc(sizeof(trc_data_t*)*val->count);
                
                for (size_t i = 0; i < val->count; i++) {
                    val->data_array[i] = read_data_compressed(trace, file);
                    if (!val->data_array) {
                        free(val->data_array);
                        return false;
                    }
                }
            }
            break;
        }
        case BaseType_FunctionPtr: {
            val->type = Type_FunctionPtr;
            break;
        }
        case BaseType_Variant: {
            uint8_t v;
            if (!readf(&v, 1, 1, file)) return false;
            base = v;
            break;
        }
        }
        if (base != BaseType_Variant) break;
    }
    
    if (type->has_group) {
        if (!readf(&val->group_index, 4, 1, file)) {
            trc_free_value(*val);
            return false;
        }
        val->group_index = le32toh(val->group_index);
        
        if ((val->group_index>=trace->group_name_count) || (val->group_index<0)) {
            trc_free_value(*val);
            return false;
        }
    } else {
        val->group_index = -1;
    }
    
    return true;
}

trace_t *load_trace(const char* filename) {
    FILE* file = fopen(filename, "rb");
    if (!file) {
        trace_error = TraceError_UnableToOpen;
        trace_error_desc = "Unable to open trace file";
        return NULL;
    }
    
    trace_t *trace = calloc(1, sizeof(trace_t));
    
    trace->frame_count = 1;
    trace->frames = calloc(1, sizeof(trace_frame_t));
    trace_frame_t *frame = trace->frames;
    frame->commands = NULL;
    
    char magic[6];
    magic[5] = 0;
    if (!readf(magic, 5, 1, file)) {
        trace_error = TraceError_Invalid;
        trace_error_desc = "Unable to read magic";
        free_trace(trace);
        return NULL;
    }
    
    if (strcmp(magic, "WIP15")) {
        trace_error = TraceError_Invalid;
        trace_error_desc = "Invalid magic";
        free_trace(trace);
        return NULL;
    }
    
    char endian;
    if (!readf(&endian, 1, 1, file)) {
        trace_error = TraceError_Invalid;
        trace_error_desc = "Unable to read endian";
        free_trace(trace);
        return NULL;
    }
    trace->little_endian = endian == '_';
    if (trace->little_endian != (SDL_BYTEORDER==SDL_LIL_ENDIAN)) {
        trace_error = TraceError_Invalid;
        trace_error_desc = "Trace-platform endianness mismatch";
        free_trace(trace);
        return NULL;
    }
    
    uint8_t version[2];
    if (!readf(version, 2, 1, file)) {
        trace_error = TraceError_Invalid;
        trace_error_desc = "Unable to read version";
        free_trace(trace);
        return NULL;
    }
    
    if ((version[0]!=0) || (version[1]!=0)) {
        trace_error = TraceError_Invalid;
        trace_error_desc = "Unknown version";
        free_trace(trace);
        return NULL;
    }
    
    if (!readf(&trace->func_name_count, 4, 1, file)) {
        trace_error = TraceError_Invalid;
        trace_error_desc = "Unable to read function name count";
        free_trace(trace);
        return NULL;
    }
    trace->func_name_count = le32toh(trace->func_name_count);
    trace->func_names = calloc(1, sizeof(char*)*trace->func_name_count);
    
    if (!readf(&trace->group_name_count, 4, 1, file)) {
        trace_error = TraceError_Invalid;
        trace_error_desc = "Unable to read group name count";
        free_trace(trace);
        return NULL;
    }
    trace->group_name_count = le32toh(trace->group_name_count);
    trace->group_names = calloc(1, sizeof(char *)*trace->group_name_count);
    
    func_decl_t* func_decls = calloc(1, sizeof(func_decl_t)*trace->func_name_count);
    memset(func_decls, 0, trace->func_name_count*sizeof(func_decl_t));
    
    #define ERROR(desc) do {\
    trace_error = TraceError_Invalid;\
    trace_error_desc = desc;\
    goto error;\
    } while (0)
    
    #define READ_TYPE(dest_) do {\
        type_t* dest = &(dest_);\
        uint8_t base, has_group, is_array;\
        if (!readf(&base, 1, 1, file))\
            ERROR("Unable to read base type");\
        if (!readf(&has_group, 1, 1, file))\
            ERROR("Unable to read whenever a type has a group or not");\
        if (!readf(&is_array, 1, 1, file))\
            ERROR("Unable to read whenever a type is an array or not");\
        dest->base = base;\
        dest->has_group = has_group;\
        dest->is_array = is_array;\
    } while (0)
    
    while (true) {
        uint8_t op_;
        if (!readf(&op_, 1, 1, file))
            break;
        opcode_t op = (opcode_t)op_;
        
        switch (op) {
        case Op_DeclFunc: {
            uint32_t index;
            if (!readf(&index, 4, 1, file))
                ERROR("Unable to read function index");
            index = le32toh(index);
            
            if (index >= trace->func_name_count)
                ERROR("Invalid function index");
            
            trace->func_names[index] = read_str(file);
            if (!trace->func_names[index])
                ERROR("Unable to read function name");
            
            func_decl_t* decl = func_decls+index;
            
            free(decl->args);
            decl->args = NULL;
            
            decl->name = trace->func_names[index];
            READ_TYPE(decl->result);
            
            if (!readf(&decl->arg_count, 4, 1, file))
                ERROR("Unable to read argument count");
            decl->arg_count = le32toh(decl->arg_count);
            
            decl->args = malloc(sizeof(type_t)*decl->arg_count);
            for (uint32_t i = 0; i < decl->arg_count; i++)
                READ_TYPE(decl->args[i]);
            break;
        }
        case Op_DeclGroup: {
            uint8_t group_type; //TODO: The group type is unused
            if (!readf(&group_type, 1, 1, file))
                ERROR("Failed to read group type");
            
            uint32_t index;
            if (!readf(&index, 4, 1, file))
                ERROR("Unable to read group index");
            index = le32toh(index);
            
            trace->group_names[index] = read_str(file);
            if (!trace->group_names[index])
                ERROR("Unable to read group name");
            break;
        }
        case Op_Call: {
            trace_command_t command;
            memset(&command, 0, sizeof(trace_command_t));
            
            if (!readf(&command.func_index, 4, 1, file))
                ERROR("Unable to read function index");
            command.func_index = le32toh(command.func_index);
            
            if (command.func_index >= trace->func_name_count)
                ERROR("Invalid function index");
            
            func_decl_t* decl = func_decls + command.func_index;
            
            if (!decl->name)
                ERROR("Undeclared function used");
            
            command.arg_count = decl->arg_count;
            command.args = malloc(decl->arg_count*sizeof(trace_value_t));
            for (size_t i = 0; i < decl->arg_count; i++) {
                memset(&command.args[i], 0, sizeof(trace_value_t));
                command.args[i].type = Type_Void;
            }
            memset(&command.ret.type, 0, sizeof(trace_value_t));
            command.ret.type = Type_Void;
            
            for (size_t i = 0; i < decl->arg_count; i++) {
                if (!read_val(file, &command.args[i], decl->args+i, trace)) {
                    free_command(&command);
                    ERROR("Failed to read argument");
                }
            }
            
            if (!read_val(file, &command.ret, &decl->result, trace)) {
                free_command(&command);
                ERROR("Failed to read command result");
            }
            
            if (!readf(&command.extra_count, 4, 1, file))
                ERROR("Unable to read function extra count");
            command.extra_count = le32toh(command.extra_count);
            
            command.extras = malloc(command.extra_count * sizeof(trace_extra_t));
            for (size_t i = 0; i < command.extra_count; i++) {
                command.extras[i].name = read_str(file);
                command.extras[i].data = read_data(file, &command.extras[i].size);
            }
            
            frame->commands = realloc(frame->commands, ++frame->command_count*sizeof(trace_command_t));
            frame->commands[frame->command_count-1] = command;
            
            if (strcmp(trace->func_names[command.func_index], "glXSwapBuffers") == 0) {
                trace->frames = realloc(trace->frames, ++trace->frame_count*sizeof(trace_frame_t));
                frame = &trace->frames[trace->frame_count-1];
                frame->command_count = 0;
                frame->commands = NULL;
            }
            break;
        }
        }
    }
    
    for (size_t i = 0; i < trace->func_name_count; i++)
        free(func_decls[i].args);
    free(func_decls);
    
    fclose(file);
    
    trace->data_queue_mutex = (pthread_mutex_t)PTHREAD_MUTEX_INITIALIZER;
    trace->data_queue_start = NULL;
    trace->data_queue_end = NULL;
    
    trace->thread_count = sysconf(_SC_NPROCESSORS_ONLN);
    if (trace->thread_count > MAX_THREADS) trace->thread_count = MAX_THREADS;
    
    atomic_store(&trace->threads_running, true);
    trace->threads = malloc(sizeof(pthread_t)*trace->thread_count);
    for (size_t i = 0; i < trace->thread_count; i++)
        pthread_create(&trace->threads[i], NULL, &compress_thread, trace);
    
    trace->inspection.cur_revision = 0;
    trace->inspection.data_count = 0;
    trace->inspection.data = NULL;
    trace->inspection.global_namespace.trace = trace;
    for (size_t i = 0; i < Trc_ObjMax; i++) {
        trace->inspection.object_count[i] = 0;
        trace->inspection.objects[i] = NULL;
        trace->inspection.global_namespace.name_tables[i] = trc_create_obj(trace, true, i, NULL);
    }
    trace->inspection.namespace_count = 0;
    trace->inspection.namespaces = NULL;
    trace->inspection.cur_ctx_revision_count = 1;
    trace->inspection.cur_ctx_revisions = malloc(sizeof(trc_cur_context_rev_t));
    trace->inspection.cur_ctx_revisions[0].context.obj = NULL;
    trace->inspection.cur_ctx_revisions[0].revision = trace->inspection.cur_revision;
    
    return trace;
    
    error:
        for (size_t i = 0; i < trace->func_name_count; i++)
            free(func_decls[i].args);
        free(func_decls);
        free_trace(trace);
        fclose(file);
        return NULL;
}

static void free_obj(trc_obj_t* obj) {
    for (size_t i = 0; i < obj->revision_count; i++)
        free(obj->revisions[i]);
    free(obj->revisions);
    free(obj);
}

void free_trace(trace_t* trace) {
    if (!trace) return;
    
    atomic_store(&trace->threads_running, false);
    for (size_t i = 0; i < trace->thread_count; i++)
        pthread_join(trace->threads[i], NULL);
    free(trace->threads);
    
    for (size_t i = 0; i < trace->func_name_count; ++i) free(trace->func_names[i]);
    for (size_t i = 0; i < trace->group_name_count; ++i) free(trace->group_names[i]);
    
    free(trace->func_names);
    free(trace->group_names);
    
    for (size_t i = 0; i < trace->frame_count; i++) free_frame(&trace->frames[i]);
    free(trace->frames);
    
    trc_gl_inspection_t* ti = &trace->inspection;
    
    for (size_t i = 0; i < Trc_ObjMax; i++) {
        for (size_t j = 0; j < ti->object_count[i]; j++) {
            trc_obj_t* obj = ti->objects[i][j];
            free_obj(obj);
        }
        free(ti->objects[i]);
        if (ti->global_namespace.name_tables[i])
            free_obj(ti->global_namespace.name_tables[i]);
        for (size_t j = 0; j < ti->namespace_count; j++) {
            if (ti->namespaces[j]->name_tables[i])
                free_obj(ti->namespaces[j]->name_tables[i]);
        }
    }
    free(ti->namespaces);
    free(ti->cur_ctx_revisions);
    
    pthread_mutex_lock(&trace->data_queue_mutex);
    for (size_t i = 0; i < ti->data_count; i++) {
        trc_data_t* data = ti->data[i];
        switch (data->storage_type) {
        case TrcStorage_Plain: free(data->plain); break;
        case TrcStorage_External: free(data->external->data); free(data->external); break;
        }
        free(data);
    }
    pthread_mutex_unlock(&trace->data_queue_mutex);
    free(ti->data);
    
    free(trace);
}

trace_error_t trc_get_error() {
    trace_error_t error = trace_error;
    trace_error = TraceError_None;
    return error;
}

const char *trc_get_error_desc() {
    const char *desc = trace_error_desc;
    trace_error_desc = "";
    return desc;
}

trace_value_t trc_create_uint(uint32_t count, uint64_t* vals) {
    trace_value_t val;
    memset(&val, 0, sizeof(val));
    val.type = Type_UInt;
    val.count = count;
    val.group_index = -1;
    if (count == 1) {
        val.u64 = *vals;
    } else {
        val.u64_array = malloc(count*sizeof(uint64_t));
        memcpy(val.u64_array, vals, count*sizeof(uint64_t));
    }
    return val;
}

trace_value_t trc_create_int(uint32_t count, int64_t* vals) {
    trace_value_t val;
    memset(&val, 0, sizeof(val));
    val.type = Type_Int;
    val.count = count;
    val.group_index = -1;
    if (count == 1) {
        val.i64 = *vals;
    } else {
        val.i64_array = malloc(count*sizeof(int64_t));
        memcpy(val.i64_array, vals, count*sizeof(int64_t));
    }
    return val;
}

trace_value_t trc_create_double(uint32_t count, double* vals) {
    trace_value_t val;
    memset(&val, 0, sizeof(val));
    val.type = Type_Double;
    val.count = count;
    val.group_index = -1;
    if (count == 1) {
        val.dbl = *vals;
    } else {
        val.dbl_array = malloc(count*sizeof(double));
        memcpy(val.dbl_array, vals, count*sizeof(double));
    }
    return val;
}

trace_value_t trc_create_bool(uint32_t count, bool* vals) {
    trace_value_t val;
    memset(&val, 0, sizeof(val));
    val.type = Type_Boolean;
    val.count = count;
    val.group_index = -1;
    if (count == 1) {
        val.bl = *vals;
    } else {
        val.bl_array = malloc(count*sizeof(bool));
        memcpy(val.bl_array, vals, count*sizeof(bool));
    }
    return val;
}

trace_value_t trc_create_ptr(uint32_t count, uint64_t* vals) {
    trace_value_t val;
    memset(&val, 0, sizeof(val));
    val.type = Type_Ptr;
    val.count = count;
    val.group_index = -1;
    if (count == 1) {
        val.ptr = *vals;
    } else {
        val.ptr_array = malloc(count*sizeof(uint64_t));
        memcpy(val.ptr_array, vals, count*sizeof(uint64_t));
    }
    return val;
}

trace_value_t trc_create_str(uint32_t count, const char*const* vals) {
    trace_value_t val;
    memset(&val, 0, sizeof(val));
    val.type = Type_Str;
    val.count = count;
    val.group_index = -1;
    if (count == 1) {
        size_t len = *vals ? strlen(*vals) : 0;
        val.str = malloc(len+1);
        memcpy(val.str, *vals, len);
        val.str[len] = 0;
    } else {
        val.str_array = malloc(sizeof(char*)*count);
        for (size_t i = 0; i < count; ++i) {
            size_t len = vals[i] ? strlen(vals[i]) : 0;
            val.str_array[i] = malloc(len+1);
            memcpy(val.str_array[i], vals[i], len);
            val.str_array[i][len] = 0;
        }
    }
    return val;
}

const uint64_t* trc_get_uint(const trace_value_t* val) {
    return val->count==1 ? &val->u64 : val->u64_array;
}

const int64_t* trc_get_int(const trace_value_t* val) {
    return val->count==1 ? &val->i64 : val->i64_array;
}

const double* trc_get_double(const trace_value_t* val) {
    return val->count==1 ? &val->dbl : val->dbl_array;
}

const bool* trc_get_bool(const trace_value_t* val) {
    return val->count==1 ? &val->bl : val->bl_array;
}

const uint64_t* trc_get_ptr(const trace_value_t* val) {
    return val->count==1 ? &val->ptr : val->ptr_array;
}

const char*const* trc_get_str(const trace_value_t* val) {
    return val->count==1 ? (const char*const*)&val->str : (const char*const*)val->str_array;
}

const trc_data_t*const* trc_get_data(const trace_value_t* val) {
    if (val->count==1) return (const trc_data_t*const*)&val->data;
    else return (const trc_data_t*const*)val->data_array;
}

trace_extra_t* trc_get_extra(trace_command_t* cmd, const char* name) {
    return trc_get_extrai(cmd, name, 0);
}

trace_extra_t* trc_get_extrai(trace_command_t* cmd, const char* name, size_t index) {
    size_t num_left = index + 1;
    for (size_t i = 0; i < cmd->extra_count; i++)
        if (!strcmp(cmd->extras[i].name, name) && !--num_left)
            return cmd->extras + i;
    return NULL;
}

static char *format_str(const char *format, va_list list) {
    va_list list2;
    va_copy(list2, list);
    char dummy_buf[1];
    int length = vsnprintf(dummy_buf, 0, format, list2);
    va_end(list2);
    
    if (length < 0) {
        char *result = malloc(1);
        result[0] = 0;
        return result;
    } else {
        char *result = malloc(length+1);
        vsnprintf(result, length+1, format, list);
        result[length] = 0;
        return result;
    }
}

void trc_add_attachment(trace_command_t* command, trc_attachment_t* attach) {
    if (!command->attachments) {
        command->attachments = attach;
    } else {
        trc_attachment_t* current = command->attachments;
        while (current->next) current = current->next;
        current->next = attach;
    }
}

void trc_add_info(trace_command_t* command, const char* format, ...) {
    trc_attachment_t* attach = malloc(sizeof(trc_attachment_t));
    attach->type = TrcAttachType_Info;
    attach->next = NULL;
    
    va_list list;
    va_start(list, format);
    attach->message = format_str(format, list);
    va_end(list);
    
    trc_add_attachment(command, attach);
}

void trc_add_warning(trace_command_t* command, const char* format, ...) {
    trc_attachment_t* attach = malloc(sizeof(trc_attachment_t));
    attach->type = TrcAttachType_Warning;
    attach->next = NULL;
    
    va_list list;
    va_start(list, format);
    attach->message = format_str(format, list);
    va_end(list);
    
    trc_add_attachment(command, attach);
}

void trc_add_error(trace_command_t* command, const char* format, ...) {
    trc_attachment_t* attach = malloc(sizeof(trc_attachment_t));
    attach->type = TrcAttachType_Error;
    attach->next = NULL;
    
    va_list list;
    va_start(list, format);
    attach->message = format_str(format, list);
    va_end(list);
    
    trc_add_attachment(command, attach);
}

typedef void (*replay_func_t)(trc_replay_context_t*, trace_command_t*);

void init_replay_gl(trc_replay_context_t* ctx);
void deinit_replay_gl(trc_replay_context_t* ctx);

void trc_run_inspection(trace_t* trace) {
    replay_func_t* funcs = malloc(trace->func_name_count * sizeof(replay_func_t));
    
    for (size_t i = 0; i < trace->func_name_count; ++i) {
        if (!trace->func_names[i] || !strlen(trace->func_names[i]))
            continue;
        
        char name[strlen(trace->func_names[i])+8];
        strcpy(name, "replay_");
        strcat(name, trace->func_names[i]);
        funcs[i] = dlsym(RTLD_DEFAULT, name);
        if (!funcs[i]) {
            fprintf(stderr, "Unable to find \"%s\".\n", name); //TODO: Handle
            fflush(stderr);
        }
    }
    
    trc_replay_context_t ctx;
    memset(&ctx, 0, sizeof(ctx));
    ctx.trace = trace;
    
    bool sdl_was_init = SDL_WasInit(SDL_INIT_VIDEO);
    if (!sdl_was_init)
        SDL_Init(SDL_INIT_VIDEO);
    ctx.window = SDL_CreateWindow("",
                                  SDL_WINDOWPOS_UNDEFINED,
                                  SDL_WINDOWPOS_UNDEFINED,
                                  100,
                                  100,
                                  SDL_WINDOW_SHOWN | //TODO: For some reason SDL_WINDOW_HIDDEN messes up the viewport and framebuffer display
                                  SDL_WINDOW_OPENGL);
    if (!ctx.window) {
        fprintf(stderr, "Unable to create a window: %sn", SDL_GetError()); //TODO: Handle
        fflush(stderr);
    }
    
    ctx.current_test = NULL;
    ctx.tests = NULL;
    
    init_replay_gl(&ctx);
    
    for (size_t i = 0; i < trace->frame_count; i++) {
        trace_frame_t* frame = &trace->frames[i];
        trace->inspection.frame_index = i;
        for (size_t j = 0; j < frame->command_count; j++) {
            trace_command_t* cmd = &frame->commands[j];
            ctx.current_command = cmd;
            trace->inspection.cmd_index = j;
            funcs[cmd->func_index](&ctx, cmd);
            cmd->revision = trace->inspection.cur_revision++;
        }
    }
    
    deinit_replay_gl(&ctx);
    
    for (trc_replay_test_t* test = ctx.tests; test;) {
        for (trc_replay_test_failure_t* e = test->failures; e;) {
            trc_replay_test_failure_t* next = e->next;
            free(e);
            e = next;
        }
        trc_replay_test_t* next = test->next;
        free(test);
        test = next;
    }
    ctx.tests = NULL;
    
    if (ctx.current_test) {
        for (trc_replay_test_failure_t* e = ctx.current_test->failures; e;) {
            trc_replay_test_failure_t* next = e->next;
            free(e);
            e = next;
        }
        free(ctx.current_test);
        ctx.current_test = NULL;
    }
    
    SDL_DestroyWindow(ctx.window);
    if (!sdl_was_init)
        SDL_Quit(); //TODO: Only quit the video subsystem
    
    free(funcs);
}

trc_obj_t* trc_get_current_gl_context(trace_t* trace, uint64_t revision) {
    for (ptrdiff_t i = trace->inspection.cur_ctx_revision_count-1; i >= 0; i--) {
        if (trace->inspection.cur_ctx_revisions[i].revision <= revision)
            return trace->inspection.cur_ctx_revisions[i].context.obj;
    }
    return NULL;
}

void trc_set_current_gl_context(trace_t* trace, trc_obj_t* obj) {
    trc_gl_inspection_t* i = &trace->inspection;
    size_t size = (i->cur_ctx_revision_count+1)*sizeof(trc_cur_context_rev_t);
    i->cur_ctx_revisions = realloc(i->cur_ctx_revisions, size);
    
    trc_cur_context_rev_t* rev = &i->cur_ctx_revisions[i->cur_ctx_revision_count++];
    *rev = i->cur_ctx_revisions[i->cur_ctx_revision_count-2];
    rev->revision = i->cur_revision;
    trc_set_obj_ref(&rev->context, obj);
}

#define WIP15_STATE_GEN_IMPL
#include "libtrace_glstate.h"
#undef WIP15_STATE_GEN_IMPL

static size_t obj_sizes[Trc_ObjMax] = {
    [TrcBuffer] = sizeof(trc_gl_buffer_rev_t),
    [TrcSampler] = sizeof(trc_gl_sampler_rev_t),
    [TrcTexture] = sizeof(trc_gl_texture_rev_t),
    [TrcQuery] = sizeof(trc_gl_query_rev_t),
    [TrcFramebuffer] = sizeof(trc_gl_framebuffer_rev_t),
    [TrcRenderbuffer] = sizeof(trc_gl_renderbuffer_rev_t),
    [TrcSync] = sizeof(trc_gl_sync_rev_t),
    [TrcProgram] = sizeof(trc_gl_program_rev_t),
    [TrcProgramPipeline] = sizeof(trc_gl_program_pipeline_rev_t),
    [TrcShader] = sizeof(trc_gl_shader_rev_t),
    [TrcVAO] = sizeof(trc_gl_vao_rev_t),
    [TrcTransformFeedback] = sizeof(trc_gl_transform_feedback_rev_t),
    [TrcContext] = sizeof(trc_gl_context_rev_t)
};

trc_obj_t* trc_create_obj(trace_t* trace, bool name_table, trc_obj_type_t type, const void* revdata) {
    trc_gl_inspection_t* in = &trace->inspection;
    
    trc_obj_t* obj = malloc(sizeof(trc_obj_t));
    obj->trace = trace;
    obj->name_table = name_table;
    obj->type = type;
    obj->revision_count = 1;
    trc_obj_rev_head_t* rev = calloc(1, name_table?sizeof(trc_name_table_rev_t):obj_sizes[type]);
    obj->revisions = malloc(sizeof(void*));
    obj->revisions[0] = rev;
    
    rev->obj = obj;
    rev->revision = in->cur_revision;
    rev->ref_count = 1;
    rev->has_name = false;
    rev->name = 0;
    rev->namespace_ = NULL;
    rev->has_had_name = false;
    rev->old_name = 0;
    if (name_table) {
        trc_name_table_rev_t* table = (trc_name_table_rev_t*)rev;
        table->entry_count = 0;
        table->names = table->objects = trc_create_data(trace, 0, NULL, TRC_DATA_IMMUTABLE);
    } else {
        memcpy(rev+1, (trc_obj_rev_head_t*)revdata+1, obj_sizes[type]-sizeof(trc_obj_rev_head_t));
        in->objects[type] = realloc(in->objects[type], (in->object_count[type]+1)*sizeof(trc_obj_t*));
        in->objects[type][in->object_count[type]++] = obj;
    }
    
    return obj;
}

const void* trc_obj_get_rev(trc_obj_t* obj, uint64_t rev) {
    if (!obj) return NULL;
    for (size_t i = obj->revision_count-1; ; i--) {
        trc_obj_rev_head_t* head = obj->revisions[i];
        if (head->revision <= rev)
            return obj->revisions[i];
        if (i == 0) break;
    }
    return NULL;
}

void trc_obj_set_rev(trc_obj_t* obj, const void* rev) {
    trc_gl_inspection_t* in = &obj->trace->inspection;
    size_t rev_size = obj->name_table ? sizeof(trc_name_table_rev_t) : obj_sizes[obj->type];
    for (size_t i = 0; i < obj->revision_count; i++) {
        trc_obj_rev_head_t* head = obj->revisions[i];
        if (head->revision == in->cur_revision) {
            memcpy(obj->revisions[i], rev, rev_size);
            return;
        }
    }
    
    trc_obj_rev_head_t* newrev = malloc(rev_size);
    memcpy(newrev, rev, rev_size);
    newrev->revision = in->cur_revision;
    
    obj->revisions = realloc(obj->revisions, (obj->revision_count+1)*sizeof(void*));
    obj->revisions[obj->revision_count] = newrev;
    obj->revision_count++;
}

void trc_grab_obj(trc_obj_t* obj) {
    if (!obj) return;
    
    const trc_obj_rev_head_t* head = trc_obj_get_rev(obj, -1);
    if (!head) return; //TODO: How to handle this
    
    //TODO: unnecessary memory allocation
    size_t rev_size = obj->name_table ? sizeof(trc_name_table_rev_t) : obj_sizes[obj->type];
    trc_obj_rev_head_t* newrev = malloc(rev_size);
    memcpy(newrev, head, rev_size);
    
    newrev->ref_count++;
    trc_obj_set_rev(obj, newrev);
    
    free(newrev);
}

#define WIP15_STATE_GEN_DESTRUCTOR
#include "libtrace_glstate.h"
#undef WIP15_STATE_GEN_DESTRUCTOR

static void framebuffer_destructor(const trc_gl_framebuffer_rev_t* rev) {
    size_t count = rev->attachments->size / sizeof(trc_gl_framebuffer_attachment_t);
    const trc_gl_framebuffer_attachment_t* attachments = trc_map_data(rev->attachments, TRC_MAP_READ);
    for (size_t i = 0; i < count; i++) {
        trc_del_obj_ref(attachments[i].renderbuffer);
        trc_del_obj_ref(attachments[i].texture);
    }
    trc_unmap_data(rev->attachments);
}

static void vertex_array_destructor(const trc_gl_vao_rev_t* rev) {
    size_t count = rev->attribs->size / sizeof(trc_gl_vao_attrib_t);
    trc_gl_vao_attrib_t* attribs = trc_map_data(rev->attribs, TRC_MAP_READ);
    for (size_t i = 0; i < count; i++)
        trc_del_obj_ref(attribs[i].buffer);
    trc_unmap_data(rev->attribs);
}

static void program_pipeline_destructor(const trc_gl_program_pipeline_rev_t* rev) {
    trc_del_obj_ref(rev->active_program);
    trc_del_obj_ref(rev->vertex_program);
    trc_del_obj_ref(rev->fragment_program);
    trc_del_obj_ref(rev->geometry_program);
    trc_del_obj_ref(rev->tess_control_program);
    trc_del_obj_ref(rev->tess_eval_program);
    trc_del_obj_ref(rev->compute_program);
}

static void transform_feedback_destructor(const trc_gl_transform_feedback_rev_t* rev) {
    size_t count = rev->bindings->size / sizeof(trc_gl_buffer_binding_point_t);
    const trc_gl_buffer_binding_point_t* bindings = trc_map_data(rev->bindings, TRC_MAP_READ);
    for (size_t i = 0; i < count; i++)
        trc_del_obj_ref(bindings[i].buf);
    trc_unmap_data(rev->bindings);
}

void trc_drop_obj(trc_obj_t* obj) {
    if (!obj) return;
    
    const trc_obj_rev_head_t* head = trc_obj_get_rev(obj, -1);
    if (!head) return; //TODO: How to handle this
    
    //TODO: unnecessary memory allocation
    size_t rev_size = obj->name_table ? sizeof(trc_name_table_rev_t) : obj_sizes[obj->type];
    trc_obj_rev_head_t* newrev = malloc(rev_size);
    memcpy(newrev, head, rev_size);
    
    newrev->ref_count--;
    trc_obj_set_rev(obj, newrev);
    
    if (newrev->ref_count==0) {
        switch (obj->type) {
        case TrcFramebuffer:
            framebuffer_destructor((trc_gl_framebuffer_rev_t*)newrev);
            break;
        case TrcVAO:
            vertex_array_destructor((trc_gl_vao_rev_t*)newrev);
            break;
        case TrcProgramPipeline:
            program_pipeline_destructor((trc_gl_program_pipeline_rev_t*)newrev);
            break;
        case TrcTransformFeedback:
            transform_feedback_destructor((trc_gl_transform_feedback_rev_t*)newrev);
            break;
        case TrcContext:
            gl_context_destructor((trc_gl_context_rev_t*)newrev);
            break;
        default:
            break;
        }
    }
    
    free(newrev);
}

static void set_obj_name(trc_obj_t* obj, trc_namespace_t* ns, bool has_name, uint64_t name) {
    const trc_obj_rev_head_t* head = trc_obj_get_rev(obj, -1);
    if (head) {
        //TODO: unnecessary memory allocation
        size_t rev_size = obj->name_table ? sizeof(trc_name_table_rev_t) : obj_sizes[obj->type];
        trc_obj_rev_head_t* newrev = malloc(rev_size);
        memcpy(newrev, head, rev_size);
        
        if (!has_name && newrev->has_name) {
            newrev->has_had_name = true;
            newrev->old_name = newrev->name;
        }
        
        newrev->has_name = has_name;
        newrev->name = name;
        newrev->namespace_ = ns;
        trc_obj_set_rev(obj, newrev);
        
        free(newrev);
    }
}

trc_namespace_t* trc_create_namespace(trace_t* trace) {
    trc_namespace_t* ns = malloc(sizeof(trc_namespace_t));
    ns->trace = trace;
    for (size_t i = 0; i < Trc_ObjMax; i++)
        ns->name_tables[i] = trc_create_obj(trace, true, i, NULL);
    
    trace->inspection.namespaces = realloc(
        trace->inspection.namespaces, (trace->inspection.namespace_count+1)*sizeof(trc_namespace_t*));
    trace->inspection.namespaces[trace->inspection.namespace_count++] = ns;
    
    return ns;
}

bool trc_set_name(trc_namespace_t* ns, trc_obj_type_t type, uint64_t name, trc_obj_t* obj) {
    const trc_obj_rev_head_t* obj_head = trc_obj_get_rev(obj, -1);
    if (obj_head && obj_head->has_name) return false; //Already named
    
    trc_obj_t* table_obj = ns->name_tables[type];
    trc_name_table_rev_t table = *(const trc_name_table_rev_t*)trc_obj_get_rev(table_obj, -1);
    
    uint64_t* names = trc_map_data(table.names, TRC_MAP_READ);
    trc_obj_t** objects = trc_map_data(table.objects, TRC_MAP_READ);
    size_t index = 0;
    
    trc_obj_t* prevobj = NULL;
    
    for (; index < table.entry_count; index++) {
        if (names[index] == name) {
            size_t objects_size = table.entry_count*sizeof(trc_obj_t*);
            trc_obj_t** newobjects = malloc(objects_size);
            
            prevobj = objects[index];
            
            memcpy(newobjects, objects, objects_size);
            newobjects[index] = obj;
            
            trc_name_table_rev_t newtable = table;
            newtable.objects = trc_create_data_no_copy(ns->trace, objects_size,
                                                       newobjects, TRC_DATA_IMMUTABLE);
            
            trc_obj_set_rev(table_obj, &newtable);
            break;
        }
    }
    
    if (index == table.entry_count) { //Create a new entry
        uint64_t* newnames = malloc((table.entry_count+1)*sizeof(uint64_t));
        trc_obj_t** newobjects = malloc((table.entry_count+1)*sizeof(trc_obj_t*));
        
        memcpy(newnames, names, table.entry_count*sizeof(uint64_t));
        memcpy(newobjects, objects, table.entry_count*sizeof(trc_obj_t*));
        
        newnames[table.entry_count] = name;
        newobjects[table.entry_count] = obj;
        
        trc_name_table_rev_t newtable = table;
        newtable.entry_count++;
        newtable.names = trc_create_data_no_copy(ns->trace, newtable.entry_count*sizeof(uint64_t),
                                                 newnames, TRC_DATA_IMMUTABLE);
        newtable.objects = trc_create_data_no_copy(ns->trace, newtable.entry_count*sizeof(trc_obj_t*),
                                                   newobjects, TRC_DATA_IMMUTABLE);
        
        trc_obj_set_rev(table_obj, &newtable);
    }
    
    trc_unmap_data(table.objects);
    trc_unmap_data(table.names);
    
    if (prevobj) set_obj_name(prevobj, ns, false, 0);
    if (obj) set_obj_name(obj, ns, true, name);
    
    return true;
}

void trc_free_name(trc_namespace_t* ns, trc_obj_type_t type, uint64_t name) {
    trc_set_name(ns, type, name, NULL);
}

trc_obj_t* trc_lookup_name(trc_namespace_t* ns, trc_obj_type_t type, uint64_t name, uint64_t rev) {
    trc_obj_t* table_obj = ns->name_tables[type];
    const trc_name_table_rev_t* table = trc_obj_get_rev(table_obj, rev);
    if (!table) return NULL;
    
    uint64_t* names = trc_map_data(table->names, TRC_MAP_READ);
    size_t index = 0;
    for (; index < table->entry_count; index++) {
        if (names[index] == name) break;
    }
    trc_unmap_data(table->names);
    
    trc_obj_t* res = NULL;
    if (index != table->entry_count) {
        res = ((trc_obj_t**)trc_map_data(table->objects, TRC_MAP_READ))[index];
        trc_unmap_data(table->objects);
    }
    
    return res;
}

trc_obj_t* trc_create_named_obj(trc_namespace_t* ns, trc_obj_type_t type, uint64_t name, const void* rev) {
    trc_obj_t* obj = trc_create_obj(ns->trace, false, type, rev);
    trc_set_name(ns, type, name, obj);
    return obj;
}

void trc_set_obj(trc_namespace_t* ns, trc_obj_type_t type, uint64_t name, const void* rev) {
    trc_obj_t* obj = trc_lookup_name(ns, type, name, -1);
    if (!obj) return;
    trc_obj_set_rev(obj, rev);
}

const void* trc_get_obj(trc_namespace_t* ns, trc_obj_type_t type, uint64_t name) {
    trc_obj_t* obj = trc_lookup_name(ns, type, name, -1);
    return trc_obj_get_rev(obj, -1);
}

const trc_gl_context_rev_t* trc_get_context(trace_t* trace) {
    return trc_obj_get_rev(trc_get_current_gl_context(trace, -1), -1);
}

void trc_set_context(trace_t* trace, trc_gl_context_rev_t* rev) {
    trc_obj_t* obj = trc_get_current_gl_context(trace, -1);
    assert(obj);
    trc_obj_set_rev(obj, rev);
}

bool trc_iter_objects(trace_t* trace, trc_obj_type_t type, size_t* index, uint64_t revision, const void** rev) {
    const trc_obj_rev_head_t* head = NULL;
    while (true) {
        if (*index >= trace->inspection.object_count[type]) return false;
        
        trc_obj_t* obj = trace->inspection.objects[type][*index];
        (*index)++;
        
        head = trc_obj_get_rev(obj, revision);
        if (head && head->ref_count>0) break;
    }
    
    *rev = head;
    return true;
}

void trc_del_obj_ref(trc_obj_ref_t ref) {
    if (ref.obj) trc_drop_obj(ref.obj);
}

void trc_set_obj_ref(trc_obj_ref_t* ref, trc_obj_t* obj) {
    if (obj) trc_grab_obj(obj);
    if (ref->obj) trc_drop_obj(ref->obj);
    ref->obj = obj;
}

static void queue_push_to_back(trace_t* trace, trc_data_t* data) {
    pthread_mutex_lock(&trace->data_queue_mutex);
    if (trace->data_queue_end != NULL)
        trace->data_queue_end->queue_next = data;
    trace->data_queue_end = data;
    if (trace->data_queue_start == NULL)
        trace->data_queue_start = trace->data_queue_end;
    pthread_mutex_unlock(&trace->data_queue_mutex);
}

static trc_data_t* queue_pop_from_front(trace_t* trace) {
    pthread_mutex_lock(&trace->data_queue_mutex);
    trc_data_t* data = NULL;
    if (trace->data_queue_start != NULL) {
        data = trace->data_queue_start;
        trace->data_queue_start = trace->data_queue_start->queue_next;
        if (trace->data_queue_start == NULL) trace->data_queue_end = NULL;
        data->queue_next = NULL;
    }
    pthread_mutex_unlock(&trace->data_queue_mutex);
    return data;
}

static void compress_data(trc_data_t* data) {
    void* compressed = malloc(data->size);
    trc_compression_t compression = TrcCompression_Zlib;
    #if ZLIB_ENABLED
    uLongf compressed_size = data->size;
    if (compress2(compressed, &compressed_size, data->plain, data->size, 9) != Z_OK) {
    #else
    int compressed_size;
    #endif
    #if LZ4_ENABLED
        compression = TrcCompression_LZ4;
        if (!(compressed_size=LZ4_compress_default(data->plain, compressed, data->size, data->size))) {
    #endif
            free(compressed);
            data->flags |= TRC_DATA_NO_COMPRESS;
            return;
    #if LZ4_ENABLED
        }
    #endif
    #if ZLIB_ENABLED
    }
    #endif
    
    trc_data_external_storage_t* storage = malloc(sizeof(trc_data_external_storage_t));
    storage->compression = compression;
    storage->size = compressed_size;
    storage->data = malloc(compressed_size);
    memcpy(storage->data, compressed, compressed_size);
    free(compressed);
    
    free(data->plain);
    
    if (data->size > 16384) malloc_trim(0);
    
    data->storage_type = TrcStorage_External;
    data->external = storage;
}

static void* compress_thread(void* userdata) {
    trace_t* trace = userdata;
    while (atomic_load(&trace->threads_running)) {
        trc_data_t* data = queue_pop_from_front(trace);
        if (!data) goto wait;
        pthread_rwlock_wrlock(&data->lock);
        
        assert(data->storage_type == TrcStorage_Plain);
        
        compress_data(data);
        
        pthread_rwlock_unlock(&data->lock);
        continue;
        wait:;
            struct timespec sleep_time;
            sleep_time.tv_sec = 0;
            sleep_time.tv_nsec = 1000;
            nanosleep(&sleep_time, NULL);
    }
    
    return NULL;
}

static trc_data_t* create_data(trace_t* trace, size_t size, void* data, uint32_t flags, bool copy) {
    trc_data_t* res = malloc(sizeof(trc_data_t));
    res->lock = (pthread_rwlock_t)PTHREAD_RWLOCK_INITIALIZER;
    res->flags = flags;
    res->storage_type = TrcStorage_Plain;
    res->size = size;
    if (data) {
        if (copy) {
            res->plain = malloc(size);
            memcpy(res->plain, data, size);
        } else {
            res->plain = data;
        }
    } else {
        if (flags & TRC_DATA_NO_ZERO)
            res->plain = malloc(size);
        else
            res->plain = calloc(size, 1);
    }
    res->queue_next = NULL;
    
    trc_gl_inspection_t* ti = &trace->inspection;
    ti->data = realloc(ti->data, ++ti->data_count*sizeof(trc_data_t*));
    ti->data[ti->data_count-1] = res;
    
    if (res->flags&TRC_DATA_IMMUTABLE && !(res->flags&TRC_DATA_NO_COMPRESS)) {
        if (trace->thread_count == 0) compress_data(res);
        else queue_push_to_back(trace, res);
    }
    
    return res;
}

trc_data_t* trc_create_data(trace_t* trace, size_t size, const void* data, uint32_t flags) {
    return create_data(trace, size, (void*)data, flags, true);
}

trc_data_t* trc_create_data_no_copy(trace_t* trace, size_t size, void* data, uint32_t flags) {
    return create_data(trace, size, data, flags, false);
}

trc_data_t* trc_create_compressed_data_no_copy(trace_t* trace, trc_compressed_data_t data) {
    trc_data_t* res = malloc(sizeof(trc_data_t));
    res->lock = (pthread_rwlock_t)PTHREAD_RWLOCK_INITIALIZER;
    res->flags = 0;
    res->storage_type = TrcStorage_External;
    res->size = data.size;
    res->external = malloc(sizeof(trc_data_external_storage_t));
    res->external->compression = data.compression;
    res->external->size = data.compressed_size;
    res->external->data = data.compressed_data;
    
    res->queue_next = NULL;
    
    trc_gl_inspection_t* ti = &trace->inspection;
    ti->data = realloc(ti->data, ++ti->data_count*sizeof(trc_data_t*));
    ti->data[ti->data_count-1] = res;
    
    return res;
}

trc_data_t* trc_copy_data(trace_t* trace, trc_data_t* src, uint32_t flags) {
    void* src_data = trc_map_data(src, TRC_MAP_READ);
    trc_data_t* data = trc_create_data(trace, src->size, src_data, flags);
    trc_unmap_data(src);
    return data;
}

void* trc_map_data(trc_data_t* data, uint32_t flags) {
    if (flags & TRC_MAP_WRITE) pthread_rwlock_wrlock(&data->lock);
    else pthread_rwlock_rdlock(&data->lock);
    if ((flags&TRC_MAP_WRITE) && data->flags&TRC_DATA_IMMUTABLE)
        assert(false);
    data_mapping_t mapping;
    mapping.data = data;
    mapping.flags = flags;
    mapping.free_ptr = true;
    switch (data->storage_type) {
    case TrcStorage_Plain: {
        mapping.free_ptr = false;
        mapping.ptr = data->plain;
        break;
    }
    case TrcStorage_External: {
        switch (data->external->compression) {
        case TrcCompression_None: {
            mapping.free_ptr = false;
            mapping.ptr = data->external->data;
            break;
        }
        #if LZ4_ENABLED
        case TrcCompression_LZ4: {
            mapping.ptr = malloc(data->size);
            if (flags & TRC_MAP_READ) {
                if (LZ4_decompress_fast(data->external->data, mapping.ptr, data->size) < 0)
                    assert(false);
            }
            break;
        }
        #endif
        #if ZLIB_ENABLED
        case TrcCompression_Zlib: {
            mapping.ptr = malloc(data->size);
            if (flags & TRC_MAP_READ) {
                uLongf _ = data->size;
                if (uncompress(mapping.ptr, &_, data->external->data, data->external->size) != Z_OK)
                    assert(false);
            }
            break;
        }
        #endif
        default: {
            assert(false);
            break;
        }
        }
        break;
    }
    default: {
        assert(false);
        return NULL;
    }
    }
    
    pthread_mutex_lock(&mapping_mutex);
    for (size_t i = 0; i < MAX_MAPPINGS; i++) {
        if (mappings[i].data == NULL) {
            mappings[i] = mapping;
            goto success;
        }
    }
    assert(false);
    success:
    pthread_mutex_unlock(&mapping_mutex);
    return mapping.ptr;
}

void trc_unmap_data(trc_data_t* data) {
    pthread_mutex_lock(&mapping_mutex);
    
    data_mapping_t* mapping = NULL;
    for (size_t i = 0; i < MAX_MAPPINGS; i++) {
        if (mappings[i].data == data) {
            mapping = &mappings[i];
            break;
        }
    }
    if (mapping) {
        free(mapping->free_ptr ? mapping->ptr : NULL);
        mapping->data = NULL;
    }
    
    pthread_mutex_unlock(&mapping_mutex);
    if (mapping->flags & TRC_MAP_WRITE) pthread_rwlock_unlock(&data->lock);
    else pthread_rwlock_unlock(&data->lock);
}

void trc_freeze_data(trace_t* trace, trc_data_t* data) {
    pthread_rwlock_wrlock(&data->lock);
    data->flags |= TRC_DATA_IMMUTABLE;
    bool compress = !(data->flags&TRC_DATA_NO_COMPRESS);
    pthread_rwlock_unlock(&data->lock);
    if (compress) {
        if (trace->thread_count == 0) compress_data(data);
        else queue_push_to_back(trace, data);
    }
}

void trc_unmap_freeze_data(trace_t* trace, trc_data_t* data) {
    trc_unmap_data(data);
    trc_freeze_data(trace, data);
}
