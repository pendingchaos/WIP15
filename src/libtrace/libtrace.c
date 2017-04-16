#include "libtrace/libtrace.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <endian.h>
#include <dlfcn.h>
#include <SDL2/SDL.h>
#ifdef ZLIB_ENABLED
#include <zlib.h>
#endif
#ifdef LZ4_ENABLED
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
    BaseType_FunctionPtr = 9
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

static size_t readf(void* ptr, size_t size, size_t count, FILE* stream) {
    size_t res = fread(ptr, size, count, stream);
    return size*count == 0 ? count : res;
}

void trc_free_value(trace_value_t value) {
    if ((value.count == 1) && (value.type == Type_Str))
        free(value.str);
    else if ((value.count == 1) && (value.type == Type_Data))
        free(value.data.ptr);
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
            for (size_t i = 0; i < value.count; ++i)
                free(value.data_array.ptrs[i]);
            free(value.data_array.sizes);
            free(value.data_array.ptrs);
            break;
        case Type_Void:
        case Type_FunctionPtr:
            break;
        }
}

static void free_command(trace_command_t* command) {
    trace_val_vec_t args = command->args;
    for (trace_value_t* arg = args->data; !vec_end(args, arg); arg++) trc_free_value(*arg);
    free_trace_val_vec(args);
    
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
    #ifdef ZLIB_ENABLED
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
    #ifdef LZ4_ENABLED
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
    return NULL;
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
    
    switch (type->base) {
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
                if (!readf(&v, 1, 1, file))
                    return false;
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
                if (!readf(&v, 4, 1, file))
                    return false;
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
                if (!readf(&val->dbl, 8, 1, file))
                    return false;
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
                if (!val->str)
                    return false;
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
                val->data.ptr = read_data(file, &val->data.size);
                if (!val->data.ptr)
                    return false;
            } else {
                val->data_array.sizes = malloc(sizeof(size_t)*val->count);
                val->data_array.ptrs = malloc(sizeof(void*)*val->count);
                
                for (size_t i = 0; i < val->count; i++) {
                    val->data_array.ptrs[i] = read_data(file, &val->data_array.sizes[i]);
                    if (!val->data_array.ptrs[i]) {
                        for (size_t j = 0; j < i; j++)
                            free(val->data_array.ptrs[i]);
                        free(val->data_array.sizes);
                        free(val->data_array.ptrs);
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
    
    trace_t *trace = malloc(sizeof(trace_t));
    
    trace->func_name_count = 0;
    trace->group_name_count = 0;
    trace->func_names = NULL;
    trace->group_names = NULL;
    trace->frame_count = 1;
    trace->frames = malloc(sizeof(trace_frame_t));
    trace_frame_t *frame = trace->frames;
    frame->command_count = 0;
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
            command.revision = 0;
            command.attachments = NULL;
            
            if (!readf(&command.func_index, 4, 1, file))
                ERROR("Unable to read function index");
            command.func_index = le32toh(command.func_index);
            
            if (command.func_index >= trace->func_name_count)
                ERROR("Invalid function index");
            
            func_decl_t* decl = func_decls + command.func_index;
            
            if (!decl->name)
                ERROR("Undeclared function used");
            
            command.args = alloc_trace_val_vec(decl->arg_count);
            for (trace_value_t* a = command.args->data; !vec_end(command.args, a); a++)
                a->type = Type_Void;
            command.ret.type = Type_Void;
            
            for (size_t i = 0; i < decl->arg_count; i++)
                if (!read_val(file, get_trace_val_vec(command.args, i), decl->args+i, trace)) {
                    free_command(&command);
                    goto error;
                }
            
            if (!read_val(file, &command.ret, &decl->result, trace)) {
                free_command(&command);
                goto error;
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
    
    trace->inspection.cur_revision = 0;
    trace->inspection.data_count = 0;
    trace->inspection.data = NULL;
    for (size_t i = 0; i < TrcGLObj_Max; i++) {
        trace->inspection.gl_obj_history_count[i] = 0;
        trace->inspection.gl_obj_history[i] = NULL;
    }
    trace->inspection.gl_context_history_count = 0;
    trace->inspection.gl_context_history = NULL;
    trace->inspection.cur_ctx_revision_count = 1;
    trace->inspection.cur_ctx_revisions = malloc(sizeof(trc_cur_context_rev_t));
    trace->inspection.cur_ctx_revisions[0].context = 0;
    trace->inspection.cur_ctx_revisions[0].revision = trace->inspection.cur_revision;
    
    trace->inspection.cur_revision++;
    
    return trace;
    
    error:
        for (size_t i = 0; i < trace->func_name_count; i++)
            free(func_decls[i].args);
        free(func_decls);
        free_trace(trace);
        fclose(file);
        return NULL;
}

void free_trace(trace_t* trace) {
    for (size_t i = 0; i < trace->func_name_count; ++i) free(trace->func_names[i]);
    
    for (size_t i = 0; i < trace->group_name_count; ++i) free(trace->group_names[i]);
    
    free(trace->func_names);
    free(trace->group_names);
    
    for (size_t i = 0; i < trace->frame_count; i++) free_frame(&trace->frames[i]);
    free(trace->frames);
    
    trc_gl_inspection_t* ti = &trace->inspection;
    
    free(ti->cur_ctx_revisions);
    for (size_t i = 0; i < ti->data_count; i++) trc_destroy_data(ti->data[i]);
    free(ti->data);
    
    for (size_t i = 0; i < TrcGLObj_Max; i++) {
        for (size_t j = 0; j < ti->gl_obj_history_count[i]; j++)
            free(ti->gl_obj_history[i][j].revisions);
        free(ti->gl_obj_history[i]);
    }
    
    for (size_t i = 0; i < ti->gl_context_history_count; i++)
        free(ti->gl_context_history[i].revisions);
    free(ti->gl_context_history);
    
    free(trace);
}

trace_value_t* trc_get_arg(trace_command_t* command, size_t i) {
    return get_trace_val_vec(command->args, i);
}

trace_command_t* trc_get_cmd(trace_frame_t* frame, size_t i) {
    return &frame->commands[i];
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

const size_t* trc_get_data_sizes(const trace_value_t* val) {
    return val->count==1 ? &val->data.size : val->data_array.sizes;
}

const void*const* trc_get_data(const trace_value_t* val) {
    return val->count==1 ? (const void*const*)&val->data.ptr : (const void*const*)val->data_array.ptrs;
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
    
    ctx.current_test_name = "Unnamed";
    
    init_replay_gl(&ctx);
    
    for (size_t i = 0; i < trace->frame_count; i++) {
        trace_frame_t* frame = &trace->frames[i];
        for (size_t j = 0; j < frame->command_count; j++) {
            trace_command_t* cmd = &frame->commands[j];
            funcs[cmd->func_index](&ctx, cmd);
            cmd->revision = trace->inspection.cur_revision++;
        }
    }
    
    deinit_replay_gl(&ctx);
    
    SDL_DestroyWindow(ctx.window);
    if (!sdl_was_init)
        SDL_Quit();
    
    free(funcs);
}

trc_gl_obj_history_t* find_gl_obj_history(trace_t* trace, trc_gl_obj_type_t type, uint fake) {
    for (size_t i = 0; i < trace->inspection.gl_obj_history_count[type]; i++) {
        if (trace->inspection.gl_obj_history[type][i].fake == fake)
            return &trace->inspection.gl_obj_history[type][i];
    }
    return NULL;
}

static size_t gl_obj_sizes[] = {
    [TrcGLObj_Buffer] = sizeof(trc_gl_buffer_rev_t),
    [TrcGLObj_Sampler] = sizeof(trc_gl_sampler_rev_t),
    [TrcGLObj_Texture] = sizeof(trc_gl_texture_rev_t),
    [TrcGLObj_Query] = sizeof(trc_gl_query_rev_t),
    [TrcGLObj_Framebuffer] = sizeof(trc_gl_framebuffer_rev_t),
    [TrcGLObj_Renderbuffer] = sizeof(trc_gl_renderbuffer_rev_t),
    [TrcGLObj_Sync] = sizeof(trc_gl_sync_rev_t),
    [TrcGLObj_Program] = sizeof(trc_gl_program_rev_t),
    [TrcGLObj_ProgramPipeline] = sizeof(trc_gl_program_pipeline_rev_t),
    [TrcGLObj_Shader] = sizeof(trc_gl_shader_rev_t),
    [TrcGLObj_VAO] = sizeof(trc_gl_vao_rev_t),
    [TrcGLObj_TransformFeedback] = sizeof(trc_gl_transform_feedback_rev_t)
};

static trc_gl_obj_rev_t* get_gl_obj_rev(trc_gl_obj_history_t* h, trc_gl_obj_type_t type, size_t index) {
    return (trc_gl_obj_rev_t*)((uint8_t*)h->revisions+index*gl_obj_sizes[type]);
}

static void* get_gl_obj(trace_t* trace, trc_gl_obj_type_t type, uint64_t fake) {
    trc_gl_obj_history_t* h = find_gl_obj_history(trace, type, fake);
    if (!h) return NULL;
    if (!h->revision_count) return NULL;
    trc_gl_obj_rev_t* rev = get_gl_obj_rev(h, type, h->revision_count-1);
    return (void*)rev;
}

static void set_gl_obj(trace_t* trace, trc_gl_obj_type_t type, uint64_t fake, const trc_gl_obj_rev_t* rev) {
    size_t size = gl_obj_sizes[type];
    
    trc_gl_obj_history_t* h = find_gl_obj_history(trace, type, fake);
    if (!h) {
        trc_gl_inspection_t* i = &trace->inspection;
        i->gl_obj_history[type] = realloc(i->gl_obj_history[type],
                                          (i->gl_obj_history_count[type]+1) *
                                          sizeof(trc_gl_obj_history_t));
        h = &i->gl_obj_history[type][i->gl_obj_history_count[type]++];
        h->fake = fake;
        h->revision_count = 1;
        h->revisions = malloc(size);
        memcpy(h->revisions, rev, size);
        ((trc_gl_obj_rev_t*)h->revisions)->revision = trace->inspection.cur_revision;
        return;
    }
    
    trc_gl_obj_rev_t* dest = get_gl_obj_rev(h, type, h->revision_count-1);
    if (dest->revision == trace->inspection.cur_revision) {
        memcpy(dest, rev, size);
        dest->revision = trace->inspection.cur_revision;
    } else {
        h->revisions = realloc(h->revisions, ++h->revision_count*size);
        memcpy(get_gl_obj_rev(h, type, h->revision_count-1), rev, size);
        get_gl_obj_rev(h, type, h->revision_count-1)->revision = trace->inspection.cur_revision;
    }
}

const trc_gl_buffer_rev_t* trc_get_gl_buffer(trace_t* trace, uint fake) {
    return get_gl_obj(trace, TrcGLObj_Buffer, fake);
}

void trc_set_gl_buffer(trace_t* trace, uint fake, const trc_gl_buffer_rev_t* rev) {
    set_gl_obj(trace, TrcGLObj_Buffer, fake, (trc_gl_obj_rev_t*)rev);
}

uint trc_get_real_gl_buffer(trace_t* trace, uint fake) {
    const trc_gl_buffer_rev_t* rev = trc_get_gl_buffer(trace, fake);
    if (!rev) return 0;
    return rev->real;
}

const trc_gl_sampler_rev_t* trc_get_gl_sampler(trace_t* trace, uint fake) {
    return get_gl_obj(trace, TrcGLObj_Sampler, fake);
}

void trc_set_gl_sampler(trace_t* trace, uint fake, const trc_gl_sampler_rev_t* rev) {
    set_gl_obj(trace, TrcGLObj_Sampler, fake, (trc_gl_obj_rev_t*)rev);
}

uint trc_get_real_gl_sampler(trace_t* trace, uint fake) {
    const trc_gl_sampler_rev_t* rev = trc_get_gl_sampler(trace, fake);
    if (!rev) return 0;
    return rev->real;
}

const trc_gl_texture_rev_t* trc_get_gl_texture(trace_t* trace, uint fake) {
    return get_gl_obj(trace, TrcGLObj_Texture, fake);
}

void trc_set_gl_texture(trace_t* trace, uint fake, const trc_gl_texture_rev_t* rev) {
    set_gl_obj(trace, TrcGLObj_Texture, fake, (trc_gl_obj_rev_t*)rev);
}

uint trc_get_real_gl_texture(trace_t* trace, uint fake) {
    const trc_gl_texture_rev_t* rev = trc_get_gl_texture(trace, fake);
    if (!rev) return 0;
    return rev->real;
}

const trc_gl_query_rev_t* trc_get_gl_query(trace_t* trace, uint fake) {
    return get_gl_obj(trace, TrcGLObj_Query, fake);
}

void trc_set_gl_query(trace_t* trace, uint fake, const trc_gl_query_rev_t* rev) {
    set_gl_obj(trace, TrcGLObj_Query, fake, (trc_gl_obj_rev_t*)rev);
}

uint trc_get_real_gl_query(trace_t* trace, uint fake) {
    const trc_gl_query_rev_t* rev = trc_get_gl_query(trace, fake);
    if (!rev) return 0;
    return rev->real;
}

const trc_gl_framebuffer_rev_t* trc_get_gl_framebuffer(trace_t* trace, uint fake) {
    return get_gl_obj(trace, TrcGLObj_Framebuffer, fake);
}

void trc_set_gl_framebuffer(trace_t* trace, uint fake, const trc_gl_framebuffer_rev_t* rev) {
    set_gl_obj(trace, TrcGLObj_Framebuffer, fake, (trc_gl_obj_rev_t*)rev);
}

uint trc_get_real_gl_framebuffer(trace_t* trace, uint fake) {
    const trc_gl_framebuffer_rev_t* rev = trc_get_gl_framebuffer(trace, fake);
    if (!rev) return 0;
    return rev->real;
}

const trc_gl_renderbuffer_rev_t* trc_get_gl_renderbuffer(trace_t* trace, uint fake) {
    return get_gl_obj(trace, TrcGLObj_Renderbuffer, fake);
}

void trc_set_gl_renderbuffer(trace_t* trace, uint fake, const trc_gl_renderbuffer_rev_t* rev) {
    set_gl_obj(trace, TrcGLObj_Renderbuffer, fake, (trc_gl_obj_rev_t*)rev);
}

uint trc_get_real_gl_renderbuffer(trace_t* trace, uint fake) {
    const trc_gl_renderbuffer_rev_t* rev = trc_get_gl_renderbuffer(trace, fake);
    if (!rev) return 0;
    return rev->real;
}

const trc_gl_sync_rev_t* trc_get_gl_sync(trace_t* trace, uint64_t fake) {
    return get_gl_obj(trace, TrcGLObj_Sync, fake);
}

void trc_set_gl_sync(trace_t* trace, uint64_t fake, const trc_gl_sync_rev_t* rev) {
    set_gl_obj(trace, TrcGLObj_Sync, fake, (trc_gl_obj_rev_t*)rev);
}

uint64_t trc_get_real_gl_sync(trace_t* trace, uint64_t fake) {
    const trc_gl_sync_rev_t* rev = trc_get_gl_sync(trace, fake);
    if (!rev) return 0;
    return rev->real;
}

const trc_gl_program_rev_t* trc_get_gl_program(trace_t* trace, uint fake) {
    return get_gl_obj(trace, TrcGLObj_Program, fake);
}

void trc_set_gl_program(trace_t* trace, uint fake, const trc_gl_program_rev_t* rev) {
    set_gl_obj(trace, TrcGLObj_Program, fake, (trc_gl_obj_rev_t*)rev);
}

uint trc_get_real_gl_program(trace_t* trace, uint fake) {
    const trc_gl_program_rev_t* rev = trc_get_gl_program(trace, fake);
    if (!rev) return 0;
    return rev->real;
}

const trc_gl_program_pipeline_rev_t* trc_get_gl_program_pipeline(trace_t* trace, uint fake) {
    return get_gl_obj(trace, TrcGLObj_ProgramPipeline, fake);
}

void trc_set_gl_program_pipeline(trace_t* trace, uint fake, const trc_gl_program_pipeline_rev_t* rev) {
    set_gl_obj(trace, TrcGLObj_ProgramPipeline, fake, (trc_gl_obj_rev_t*)rev);
}

uint trc_get_real_gl_program_pipeline(trace_t* trace, uint fake) {
    const trc_gl_program_pipeline_rev_t* rev = trc_get_gl_program_pipeline(trace, fake);
    if (!rev) return 0;
    return rev->real;
}

const trc_gl_shader_rev_t* trc_get_gl_shader(trace_t* trace, uint fake) {
    return get_gl_obj(trace, TrcGLObj_Shader, fake);
}

void trc_set_gl_shader(trace_t* trace, uint fake, const trc_gl_shader_rev_t* rev) {
    set_gl_obj(trace, TrcGLObj_Shader, fake, (trc_gl_obj_rev_t*)rev);
}

uint trc_get_real_gl_shader(trace_t* trace, uint fake) {
    const trc_gl_shader_rev_t* rev = trc_get_gl_shader(trace, fake);
    if (!rev) return 0;
    return rev->real;
}

const trc_gl_vao_rev_t* trc_get_gl_vao(trace_t* trace, uint fake) {
    return get_gl_obj(trace, TrcGLObj_VAO, fake);
}

void trc_set_gl_vao(trace_t* trace, uint fake, const trc_gl_vao_rev_t* rev) {
    set_gl_obj(trace, TrcGLObj_VAO, fake, (trc_gl_obj_rev_t*)rev);
}

uint trc_get_real_gl_vao(trace_t* trace, uint fake) {
    const trc_gl_vao_rev_t* rev = trc_get_gl_vao(trace, fake);
    if (!rev) return 0;
    return rev->real;
}

const trc_gl_transform_feedback_rev_t* trc_get_gl_transform_feedback(trace_t* trace, uint fake) {
    return get_gl_obj(trace, TrcGLObj_TransformFeedback, fake);
}

void trc_set_gl_transform_feedback(trace_t* trace, uint fake, const trc_gl_transform_feedback_rev_t* rev) {
    set_gl_obj(trace, TrcGLObj_TransformFeedback, fake, (trc_gl_obj_rev_t*)rev);
}

uint trc_get_real_gl_transform_feedback(trace_t* trace, uint fake) {
    const trc_gl_transform_feedback_rev_t* rev = trc_get_gl_transform_feedback(trace, fake);
    if (!rev) return 0;
    return rev->real;
}

void trc_grab_gl_obj(trace_t* trace, uint64_t fake, trc_gl_obj_type_t type) {
    size_t size = gl_obj_sizes[type];
    const trc_gl_obj_rev_t* rev = get_gl_obj(trace, type, fake);
    if (!rev) return;
    trc_gl_obj_rev_t* new_rev = malloc(size);
    memcpy(new_rev, rev, size);
    new_rev->ref_count++;
    set_gl_obj(trace, type, fake, new_rev);
    free(new_rev);
}

void trc_rel_gl_obj(trace_t* trace, uint64_t fake, trc_gl_obj_type_t type) {
    size_t size = gl_obj_sizes[type];
    const trc_gl_obj_rev_t* rev = get_gl_obj(trace, type, fake);
    if (!rev) return;
    trc_gl_obj_rev_t* new_rev = malloc(size);
    memcpy(new_rev, rev, size);
    new_rev->ref_count--;
    set_gl_obj(trace, type, fake, new_rev);
    free(new_rev);
}

const trc_gl_obj_rev_t* trc_lookup_gl_obj(trace_t* trace, uint revision, uint64_t fake, trc_gl_obj_type_t type) {
    for (size_t i = 0; i < trace->inspection.gl_obj_history_count[type]; i++) {
        if (trace->inspection.gl_obj_history[type][i].fake != fake) continue;
        trc_gl_obj_history_t* history = &trace->inspection.gl_obj_history[type][i];
        for (ptrdiff_t j = history->revision_count-1; j >= 0; j--) {
            if (get_gl_obj_rev(history, type, j)->revision <= revision)
                return get_gl_obj_rev(history, type, j);
        }
    }
    return NULL;
}

static trc_gl_context_history_t* find_ctx_history(trace_t* trace, uint64_t fake) {
    for (size_t i = 0; i < trace->inspection.gl_context_history_count; i++) {
        if (trace->inspection.gl_context_history[i].fake == fake)
            return &trace->inspection.gl_context_history[i];
    }
    return NULL;
}

uint64_t trc_lookup_current_fake_gl_context(trace_t* trace, uint revision) {
    for (ptrdiff_t i = trace->inspection.cur_ctx_revision_count-1; i >= 0; i--) {
        if (trace->inspection.cur_ctx_revisions[i].revision <= revision)
            return trace->inspection.cur_ctx_revisions[i].context;
    }
    return 0;
}

uint64_t trc_get_current_fake_gl_context(trace_t* trace) {
    return trace->inspection.cur_ctx_revisions[trace->inspection.cur_ctx_revision_count-1].context;
}

void trc_set_current_fake_gl_context(trace_t* trace, uint64_t fake) {
    trc_gl_inspection_t* i = &trace->inspection;
    size_t size = (i->cur_ctx_revision_count+1)*sizeof(trc_cur_context_rev_t);
    i->cur_ctx_revisions = realloc(i->cur_ctx_revisions, size);
    i->cur_ctx_revisions[i->cur_ctx_revision_count].revision = ++i->cur_revision;
    i->cur_ctx_revisions[i->cur_ctx_revision_count++].context = fake;
}

const trc_gl_context_rev_t* trc_get_gl_context(trace_t* trace, uint64_t fake) {
    if (!fake) fake = trc_get_current_fake_gl_context(trace);
    trc_gl_context_history_t* h = find_ctx_history(trace, fake);
    if (!h) return NULL;
    if (!h->revision_count) return NULL;
    return &h->revisions[h->revision_count-1];
}

void trc_set_gl_context(trace_t* trace, uint64_t fake, const trc_gl_context_rev_t* rev) {
    if (!fake) fake = trc_get_current_fake_gl_context(trace);
    trc_gl_context_history_t* h = find_ctx_history(trace, fake);
    if (!h) {
        trc_gl_inspection_t* i = &trace->inspection;
        i->gl_context_history = realloc(i->gl_context_history,
                                       (i->gl_context_history_count+1) *
                                       sizeof(trc_gl_context_history_t));
        h = &i->gl_context_history[i->gl_context_history_count++];
        h->fake = fake;
        h->revision_count = 1;
        h->revisions = malloc(sizeof(trc_gl_context_rev_t));
        *h->revisions = *rev;
        h->revisions->revision = trace->inspection.cur_revision;
        return;
    }
    
    trc_gl_context_rev_t* dest = &h->revisions[h->revision_count-1];
    if (dest->revision == trace->inspection.cur_revision) {
        *dest = *rev;
        dest->revision = trace->inspection.cur_revision;
    } else {
        h->revisions = realloc(h->revisions, ++h->revision_count*sizeof(trc_gl_context_rev_t));
        h->revisions[h->revision_count-1] = *rev;
        h->revisions[h->revision_count-1].revision = trace->inspection.cur_revision;
    }
}

void* trc_get_real_gl_context(trace_t* trace, uint64_t fake) {
    const trc_gl_context_rev_t* rev = trc_get_gl_context(trace, fake);
    if (!rev) return NULL;
    return rev->real;
}

const trc_gl_context_rev_t* trc_lookup_gl_context(trace_t* trace, uint revision, uint64_t fake) {
    for (size_t i = 0; i < trace->inspection.gl_context_history_count; i++) {
        if (trace->inspection.gl_context_history[i].fake != fake) continue;
        trc_gl_context_history_t* history = &trace->inspection.gl_context_history[i];
        for (ptrdiff_t j = history->revision_count-1; j >= 0; j--) {
            if (history->revisions[j].revision <= revision)
                return &history->revisions[j];
        }
    }
    return NULL;
}

#define WIP15_STATE_GEN_IMPL
#include "libtrace_glstate.h"
#undef WIP15_STATE_GEN_IMPL

static void set_data(trc_data_t* data, void* src, bool can_own, bool* owns_data) {
    void* dest = malloc(data->uncompressed_size);
    int res = LZ4_compress_default(src, dest, data->uncompressed_size, data->uncompressed_size);
    if (res==0 || res==data->uncompressed_size) {
        data->compression = TrcCompression_None;
        data->compressed_size = data->uncompressed_size;
        if (can_own) {
            free(dest);
            data->compressed_data = src;
            if (owns_data) *owns_data = true;
        } else {
            data->compressed_data = dest;
            memcpy(dest, src, data->uncompressed_size);
            if (owns_data) *owns_data = false;
        }
    } else {
        data->compression = TrcCompression_LZ4;
        data->compressed_data = malloc(res);
        memcpy(data->compressed_data, dest, res);
        free(dest);
        data->compressed_size = res;
        if (owns_data) *owns_data = false;
    }
}

trc_data_t* trc_create_data(trace_t* trace, size_t size, const void* data) {
    trc_data_t* res = malloc(sizeof(trc_data_t));
    res->uncompressed_size = size;
    if (data == NULL) {
        void* zdata = calloc(size, 1);
        set_data(res, zdata, false, NULL);
        free(zdata);
    } else {
        set_data(res, (void*)data, false, NULL);
    }
    
    return res;
}

trc_data_t* trc_create_inspection_data(trace_t* trace, size_t size, const void* data) {
    trc_data_t* res = trc_create_data(trace, size, data);
    
    trc_gl_inspection_t* ti = &trace->inspection;
    ti->data = realloc(ti->data, ++ti->data_count*sizeof(trc_data_t*));
    ti->data[ti->data_count-1] = res;
    
    return res;
}

void trc_destroy_data(trc_data_t* data) {
    free(data->compressed_data);
    free(data);
}

void* trc_lock_data(trc_data_t* data, bool read, bool write) {
    data->lock_write = write;
    switch (data->compression) {
    case TrcCompression_None: {
        data->uncompressed_data = data->compressed_data;
        return data->compressed_data;
    }
    #if LZ4_ENABLED
    case TrcCompression_LZ4: {
        data->uncompressed_data = malloc(data->uncompressed_size);
        if (LZ4_decompress_fast(data->compressed_data, data->uncompressed_data, data->uncompressed_size) < 0)
            assert(false);
        return data->uncompressed_data;
    }
    #endif
    default: assert(false);
    }
}

void trc_unlock_data(trc_data_t* data) {
    if (data->lock_write) {
        bool owns_data;
        set_data(data, data->uncompressed_data, true, &owns_data);
        if (owns_data) data->uncompressed_data = NULL;
    }
    
    switch (data->compression) {
    case TrcCompression_None: break;
    case TrcCompression_LZ4: free(data->uncompressed_data); break;
    default: assert(false); break;
    }
    data->uncompressed_data = NULL;
}
