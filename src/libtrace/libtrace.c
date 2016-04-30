#include "libtrace/libtrace.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <endian.h>
#include <zlib.h>

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

void trace_free_value(trace_value_t value) {
    if ((value.count == 1) && (value.type == Type_Str))
        free(value.str);
    else if ((value.count == 1) && (value.type == Type_Data))
        free(value.data);
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
            for (size_t i = 0; i < value.count; ++i) {
                free(value.str_array[i]);
            }
            free(value.str_array);
            break;
        case Type_Data:
            for (size_t i = 0; i < value.count; ++i) {
                free(value.data_array[i]);
            }
            free(value.data_array);
            break;
        case Type_Void:
        case Type_FunctionPtr:
            break;
        }
}

static void free_command(trace_command_t* command) {
    trace_val_vec_t args = command->args;
    for (trace_value_t* arg = args->data; !vec_end(args, arg); arg++)
        trace_free_value(*arg);
    free_trace_val_vec(args);
    
    trace_free_value(command->ret);
    
    for (size_t i = 0; i < command->extra_count; i++) {
        free(command->extras[i].name);
        free(command->extras[i].data);
    }
}

static void free_frame(trace_frame_t* frame) {
    trace_cmd_vec_t commands = frame->commands;
    for (trace_command_t* cmd = commands->data; !vec_end(commands, cmd); cmd++)
        free_command(cmd);
    free_trace_cmd_vec(commands);
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
    } else {
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
                val->data = read_data(file, NULL);
                if (!val->data)
                    return false;
            } else {
                val->data_array = malloc(sizeof(void*)*val->count);
                
                for (size_t i = 0; i < val->count; i++) {
                    val->data_array[i] = read_data(file, NULL);
                    if (!val->data_array[i]) {
                        for (size_t j = 0; j < i; j++)
                            free(val->data_array[i]);
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
    }
    
    if (type->has_group) {
        if (!readf(&val->group_index, 4, 1, file)) {
            trace_free_value(*val);
            return false;
        }
        val->group_index = le32toh(val->group_index);
        
        if ((val->group_index>=trace->group_name_count) || (val->group_index<0)) {
            trace_free_value(*val);
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
    trace->frames = NULL;
    
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
    
    char version[17];
    version[16] = 0;
    if (!readf(version, 16, 1, file)) {
        trace_error = TraceError_Invalid;
        trace_error_desc = "Unable to read version";
        free_trace(trace);
        return NULL;
    }
    
    if (strcmp(version, "0.0a            ")) {
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
    
    trace->frames = alloc_trace_frame_vec(1);
    trace_frame_t *frame = get_trace_frame_vec(trace->frames, 0);
    frame->commands = alloc_trace_cmd_vec(0);
    
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
            
            append_trace_cmd_vec(frame->commands, &command);
            
            if (strcmp(trace->func_names[command.func_index], "glXSwapBuffers") == 0) {
                size_t index = get_trace_frame_vec_count(trace->frames);
                resize_trace_frame_vec(trace->frames, index+1);
                
                frame = get_trace_frame_vec(trace->frames, index);
                frame->commands = alloc_trace_cmd_vec(0);
            }
            break;
        }
        }
    }
    
    for (size_t i = 0; i < trace->func_name_count; i++)
        free(func_decls[i].args);
    free(func_decls);
    
    fclose(file);
    
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
    for (size_t i = 0; i < trace->func_name_count; ++i)
        free(trace->func_names[i]);
    
    for (size_t i = 0; i < trace->group_name_count; ++i)
        free(trace->group_names[i]);
    
    free(trace->func_names);
    free(trace->group_names);
    
    trace_frame_vec_t frames = trace->frames;
    for (trace_frame_t* frame = frames->data; !vec_end(frames, frame); frame++)
        free_frame(frame);
    free_trace_frame_vec(trace->frames);
    
    free(trace);
}

trace_value_t* trace_get_arg(trace_command_t* command, size_t i) {
    return get_trace_val_vec(command->args, i);
}

trace_command_t* trace_get_cmd(trace_frame_t* frame, size_t i) {
    return get_trace_cmd_vec(frame->commands, i);
}

trace_error_t get_trace_error() {
    trace_error_t error = trace_error;
    trace_error = TraceError_None;
    return error;
}

const char *get_trace_error_desc() {
    const char *desc = trace_error_desc;
    trace_error_desc = "";
    return desc;
}

trace_value_t trace_create_uint(uint32_t count, uint64_t* vals) {
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

trace_value_t trace_create_int(uint32_t count, int64_t* vals) {
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

trace_value_t trace_create_double(uint32_t count, double* vals) {
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

trace_value_t trace_create_bool(uint32_t count, bool* vals) {
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

trace_value_t trace_create_ptr(uint32_t count, uint64_t* vals) {
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

trace_value_t trace_create_str(uint32_t count, const char*const* vals) {
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

uint64_t* trace_get_uint(trace_value_t* val) {
    return val->count==1 ? &val->u64 : val->u64_array;
}

int64_t* trace_get_int(trace_value_t* val) {
    return val->count==1 ? &val->i64 : val->i64_array;
}

double* trace_get_double(trace_value_t* val) {
    return val->count==1 ? &val->dbl : val->dbl_array;
}

bool* trace_get_bool(trace_value_t* val) {
    return val->count==1 ? &val->bl : val->bl_array;
}

uint64_t* trace_get_ptr(trace_value_t* val) {
    return val->count==1 ? &val->ptr : val->ptr_array;
}

char** trace_get_str(trace_value_t* val) {
    return val->count==1 ? &val->str : val->str_array;
}

void** trace_get_data(trace_value_t* val) {
    return val->count==1 ? &val->data : val->data_array;
}

trace_extra_t* trace_get_extra(trace_command_t* cmd, const char* name) {
    return trace_get_extrai(cmd, name, 0);
}

trace_extra_t* trace_get_extrai(trace_command_t* cmd, const char* name, size_t index) {
    size_t num_left = index + 1;
    for (size_t i = 0; i < cmd->extra_count; i++)
        if (!strcmp(cmd->extras[i].name, name) && !--num_left)
            return cmd->extras + i;
    return NULL;
}
