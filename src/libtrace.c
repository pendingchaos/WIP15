#include "libtrace.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <endian.h>

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
        case Type_Bitfield:
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
    for (size_t i = 0; i < get_trace_val_vec_count(args); ++i) {
        trace_free_value(*get_trace_val_vec(args, i));
    }
    free_trace_val_vec(args);
    
    trace_free_value(command->ret);
}

static void free_frame(trace_frame_t* frame) {
    size_t count = get_trace_cmd_vec_count(frame->commands);
    for (size_t i = 0; i < count; ++i) {
        free_command(trace_get_cmd(frame, i));
    }
    free_trace_cmd_vec(frame->commands);
}

static void set_int(trace_value_t* val, int64_t i) {
    val->type = Type_Int;
    val->i64 = i;
    val->count = 1;
}

static void set_uint(trace_value_t* val, uint64_t i) {
    val->type = Type_UInt;
    val->u64 = i;
    val->count = 1;
}

//-1 == Error
//0 = End
//1 = Arg
//2 = Result
static int read_val(FILE* file, trace_value_t* val, trace_t* trace) {
    uint8_t type;
    if (!readf(&type, 1, 1, file)) {
        trace_error_desc = "Unable to read type";
        return -1;
    }
    
    val->type = Type_Void;
    val->group_index = -1;
    
    bool group = false;
    
    switch (type) {
    case 0: { //end
        return 0;
    }
    case 1: {//uint32_t array
        if (!readf(&val->count, 4, 1, file)) {
            trace_error_desc = "Unable to read uint32_t array count";
            return -1;
        }
        val->count = le32toh(val->count);
        val->type = Type_UInt;
        if (val->count == 1) {
            uint32_t v;
            if (!readf(&v, 4, 1, file)) {
                trace_error_desc = "Unable to read uint32_t array element";
                return -1;
            }
            val->u64 = le32toh(v);
        } else {
            val->u64_array = malloc(sizeof(uint64_t)*val->count);
            
            for (size_t i = 0; i < val->count; ++i) {
                uint32_t v;
                if (!readf(&v, 4, 1, file)) {
                    free(val->u64_array);
                    trace_error_desc = "Unable to read uint32_t array element";
                    return -1;
                }
                val->u64_array[i] = le32toh(v);
            }
        }
        break;
    }
    case 2: { //string
        uint32_t length;
        if (!readf(&length, 4, 1, file)) {
            trace_error_desc = "Unable to read string length";
            return -1;
        }
        length = le32toh(length);
        
        val->type = Type_Str;
        val->count = 1;
        val->str = malloc(length+1);
        if (!readf(val->str, length, 1, file)) {
            free(val->str);
            trace_error_desc = "Unable to read string data";
            return -1;
        }
        val->str[length] = 0;
        break;
    }
    case 3: { //32-bit pointer
        uint32_t v;
        if (!readf(&v, 4, 1, file)) {
            trace_error_desc = "Unable to read 32-bit pointer";
            return -1;
        }
        val->type = Type_Ptr;
        val->count = 1;
        val->ptr = le32toh(v);
        break;
    }
    case 4: { //64-bit pointer
        uint64_t v;
        if (!readf(&v, 8, 1, file)) {
            trace_error_desc = "Unable to read 64-bit pointer";
            return -1;
        }
        val->type = Type_Ptr;
        val->count = 1;
        val->ptr = le64toh(v);
        break;
    }
    case 5: { //function pointer
        break;
    }
    case 6: { //uint8_t
        uint8_t v;
        if (!readf(&v, 1, 1, file)) {
            trace_error_desc = "Unable to read 8-bit unsigned integer";
            return -1;
        }
        set_uint(val, v);
        group = true;
        break;
    }
    case 7: { //int8_t
        int8_t v;
        if (!readf(&v, 1, 1, file)) {
            trace_error_desc = "Unable to read 8-bit signed integer";
            return -1;
        }
        set_int(val, v);
        group = true;
        break;
    }
    case 8: { //uint16_t
        uint16_t v;
        if (!readf(&v, 2, 1, file)) {
            trace_error_desc = "Unable to read 16-bit unsigned integer";
            return -1;
        }
        set_uint(val, le16toh(v));
        group = true;
        break;
    }
    case 9: { //int16_t
        int16_t v;
        if (!readf(&v, 2, 1, file)) {
            trace_error_desc = "Unable to read 16-bit signed integer";
            return -1;
        }
        set_int(val, le16toh(v));
        group = true;
        break;
    }
    case 10: { //uint32_t
        uint32_t v;
        if (!readf(&v, 4, 1, file)) {
            trace_error_desc = "Unable to read 32-bit unsigned integer";
            return -1;
        }
        set_uint(val, le32toh(v));
        group = true;
        break;
    }
    case 11: { //int32_t
        int32_t v;
        if (!readf(&v, 4, 1, file)) {
            trace_error_desc = "Unable to read 32-bit signed integer";
            return -1;
        }
        set_int(val, le16toh(v));
        group = true;
        break;
    }
    case 12: { //uint64_t
        uint64_t v;
        if (!readf(&v, 8, 1, file)) {
            trace_error_desc = "Unable to read 64-bit unsigned integer";
            return -1;
        }
        set_uint(val, le64toh(v));
        group = true;
        break;
    }
    case 13: { //int64_t
        int64_t v;
        if (!readf(&v, 8, 1, file)) {
            trace_error_desc = "Unable to read 64-bit signed integer";
            return -1;
        }
        set_int(val, le64toh(v));
        group = true;
        break;
    }
    case 14: { //boolean
        uint8_t v;
        if (!readf(&v, 1, 1, file)) {
            trace_error_desc = "Unable to read boolean";
            return -1;
        }
        val->type = Type_Boolean;
        val->count = 1;
        val->bl = v;
        group = true;
        break;
    }
    case 15: { //bitfield
        uint32_t v;
        if (!readf(&v, 4, 1, file)) {
            trace_error_desc = "Unable to read bitfield";
            return -1;
        }
        val->type = Type_Bitfield;
        val->count = 1;
        val->bitfield = v;
        group = true;
        break;
    }
    case 16: { //float
        //TODO: Make this more portable.
        float v;
        if (!readf(&v, 4, 1, file)) {
            trace_error_desc = "Unable to read float";
            return -1;
        }
        val->type = Type_Double;
        val->count = 1;
        val->dbl = v;
        group = true;
        break;
    }
    case 17: { //double
        //TODO: Make this more portable.
        double v;
        if (!readf(&v, 8, 1, file)) {
            trace_error_desc = "Unable to read double";
            return -1;
        }
        val->type = Type_Double;
        val->count = 1;
        val->dbl = v;
        group = true;
        break;
    }
    case 18: { //result
        int res = read_val(file, val, trace);
        if (res == -1) {
            return -1;
        } else if (res == 0) {
            trace_error_desc = "Result value is End";
            return -1;
        } else if (res == 2) {
            trace_error_desc = "Result value is a result";
            return -1;
        }
        return 2;
    }
    case 19: { //string array
        if (!readf(&val->count, 4, 1, file)) {
            trace_error_desc = "Unable to read count for string array";
            return -1;
        }
        val->count = le32toh(val->count);
        val->type = Type_Str;
        
        if (val->count == 1) {
            uint32_t length;
            if (!readf(&length, 4, 1, file)) {
                trace_free_value(*val);
                trace_error_desc = "Unable to read length for string array element";
                return -1;
            }
            length = le32toh(length);
            
            val->str = malloc(length+1);
            if (!readf(val->str, length, 1, file)) {
                trace_free_value(*val);
                trace_error_desc = "Unable to read data for string array element";
                return -1;
            }
            val->str[length] = 0;
        } else {
            val->str_array = malloc(sizeof(char *)*val->count);
            
            for (size_t i = 0; i < val->count; ++i) val->str_array[i] = NULL;
            
            for (size_t i = 0; i < val->count; ++i) {
                uint32_t length;
                if (!readf(&length, 4, 1, file)) {
                    trace_free_value(*val);
                    trace_error_desc = "Unable to read length for string array element";
                    return -1;
                }
                length = le32toh(length);
                
                char *str = malloc(length+1);
                if (!readf(str, length, 1, file)) {
                    trace_free_value(*val);
                    trace_error_desc = "Unable to read data for string array element";
                    return -1;
                }
                str[length] = 0;
                
                val->str_array[i] = str;
            }
        }
        break;
    }
    case 20: { //data
        uint32_t size;
        if (!readf(&size, 4, 1, file)) {
            trace_error_desc = "Unable to read data size";
            return -1;
        }
        size = le32toh(size);
        
        val->type = Type_Data;
        val->count = 1;
        val->data = malloc(size);
        if (!readf(val->data, size, 1, file)) {
            free(val->data);
            trace_error_desc = "Unable to read data";
            return -1;
        }
        break;
    }
    case 21: {//double array
        if (!readf(&val->count, 4, 1, file)) {
            trace_error_desc = "Unable to read double array count";
            return -1;
        }
        val->count = le32toh(val->count);
        val->type = Type_Double;
        if (val->count == 1) {
            if (!readf(&val->dbl, 8, 1, file)) {
                trace_error_desc = "Unable to read double array element";
                return -1;
            }
        } else {
            val->dbl_array = malloc(sizeof(double)*val->count);
            for (size_t i = 0; i < val->count; ++i) {
                if (!readf(val->dbl_array+i, 4, 1, file)) {
                    free(val->dbl_array);
                    trace_error_desc = "Unable to read double array element";
                    return -1;
                }
            }
        }
        break;
    }
    case 22: {//int32_t array
        if (!readf(&val->count, 4, 1, file)) {
            trace_error_desc = "Unable to read double array count";
            return -1;
        }
        val->count = le32toh(val->count);
        val->type = Type_Int;
        if (val->count == 1) {
            int32_t i;
            if (!readf(&i, 4, 1, file)) {
                trace_error_desc = "Unable to read double array element";
                return -1;
            }
            val->i64 = le32toh(i);
        } else {
            val->i64_array = malloc(sizeof(int64_t)*val->count);
            for (size_t i = 0; i < val->count; ++i) {
                int32_t iv;
                if (!readf(&iv, 4, 1, file)) {
                    free(val->i64_array);
                    trace_error_desc = "Unable to read double array element";
                    return -1;
                }
                val->i64_array[i] = le32toh(iv);
            }
        }
        break;
    }
    default: {
        trace_error_desc = "Invalid value type";
        return -1;
    }
    }
    
    if (group) {
        if (!readf(&val->group_index, 4, 1, file)) {
            trace_error = TraceError_Invalid;
            trace_error_desc = "Unable to read group index";
            trace_free_value(*val);
            return -1;
        }
        val->group_index = le32toh(val->group_index);
        
        if (val->group_index >= trace->group_name_count && !(val->group_index<0)) {
            trace_error = TraceError_Invalid;
            trace_error_desc = "Invalid group index";
            trace_free_value(*val);
            return -1;
        }
    } else {
        val->group_index = -1;
    }
    
    return 1;
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
    
    uint8_t little_endian;
    if (!readf(&little_endian, 1, 1, file)) {
        trace_error = TraceError_Invalid;
        trace_error_desc = "Unable to read endian";
        free_trace(trace);
        return NULL;
    }
    trace->little_endian = little_endian;
    
    if (!readf(&trace->func_name_count, 4, 1, file)) {
        trace_error = TraceError_Invalid;
        trace_error_desc = "Unable to read function name count";
        free_trace(trace);
        return NULL;
    }
    trace->func_name_count = le32toh(trace->func_name_count);
    trace->func_names = malloc(sizeof(char *)*trace->func_name_count);
    
    for (size_t i = 0; i < trace->func_name_count; ++i) {
        uint32_t length;
        if (!readf(&length, 4, 1, file)) {
            trace_error = TraceError_Invalid;
            trace_error_desc = "Unable to read function name length";
            free(trace->func_names);
            trace->func_name_count = 0;
            free_trace(trace);
            return NULL;
        }
        length = le32toh(length);
        
        char *name = malloc(length+1);
        if (!readf(name, length, 1, file)) {
            trace_error = TraceError_Invalid;
            trace_error_desc = "Unable to read function name length";
            free(name);
            free(trace->func_names);
            trace->func_name_count = 0;
            free_trace(trace);
            return NULL;
        }
        name[length] = 0;
        
        trace->func_names[i] = name;
    }
    
    if (!readf(&trace->group_name_count, 4, 1, file)) {
        trace_error = TraceError_Invalid;
        trace_error_desc = "Unable to read function name count";
        free_trace(trace);
        return NULL;
    }
    trace->group_name_count = le32toh(trace->group_name_count);
    trace->group_names = malloc(sizeof(char *)*trace->group_name_count);
    
    for (size_t i = 0; i < trace->group_name_count; ++i) {
        uint32_t length;
        if (!readf(&length, 4, 1, file)) {
            trace_error = TraceError_Invalid;
            trace_error_desc = "Unable to read function name length";
            free(trace->func_names);
            trace->func_name_count = 0;
            free_trace(trace);
            return NULL;
        }
        length = le32toh(length);
        
        char *name = malloc(length+1);
        if (!readf(name, length, 1, file)) {
            trace_error = TraceError_Invalid;
            trace_error_desc = "Unable to read function name length";
            free(name);
            free(trace->group_names);
            trace->group_name_count = 0;
            free_trace(trace);
            return NULL;
        }
        name[length] = 0;
        
        trace->group_names[i] = name;
    }
    
    trace->frames = alloc_trace_frame_vec(1);
    trace_frame_t *frame = get_trace_frame_vec(trace->frames, 0);
    frame->commands = alloc_trace_cmd_vec(0);
    
    while (true) {
        if (fgetc(file) == EOF) {
            break;
        }
        
        fseek(file, -1, SEEK_CUR);
        
        trace_command_t command;
        command.func_index = 0;
        command.args = NULL;
        command.ret.type = Type_Void;
        command.ret.group_index = -1;
        command.ret.count = 0;
        command.args = alloc_vec(0);
        
        if (!readf(&command.func_index, 4, 1, file)) {
            trace_error = TraceError_Invalid;
            trace_error_desc = "Unable to read function index";
            free_vec(command.args);
            free_trace(trace);
            return NULL;
        }
        command.func_index = le32toh(command.func_index);
        
        if (command.func_index >= trace->func_name_count) {
            trace_error = TraceError_Invalid;
            trace_error_desc = "Invalid function index";
            free_vec(command.args);
            free_trace(trace);
            return NULL;
        }
        
        int res;
        trace_value_t val;
        while ((res = read_val(file, &val, trace)) != 0) {
            if (res == -1) {
                free_vec(command.args);
                free_trace(trace);
                trace_error = TraceError_Invalid;
                return NULL;
            } else if (res == 1) {
                append_vec(command.args, sizeof(trace_value_t), &val);
            } else if (res == 2) {
                trace_free_value(command.ret);
                command.ret = val;
            }
        }
        
        append_trace_cmd_vec(frame->commands, &command);
        
        if (strcmp(trace->func_names[command.func_index], "glXSwapBuffers") == 0) {
            size_t index = get_trace_frame_vec_count(trace->frames);
            resize_trace_frame_vec(trace->frames, index+1);
            
            frame = get_trace_frame_vec(trace->frames, index);
            frame->commands = alloc_trace_cmd_vec(0);
        }
    }
    
    fclose(file);
    
    return trace;
}

void free_trace(trace_t* trace) {
    for (size_t i = 0; i < trace->func_name_count; ++i) {
        free(trace->func_names[i]);
    }
    
    for (size_t i = 0; i < trace->group_name_count; ++i) {
        free(trace->group_names[i]);
    }
    
    free(trace->func_names);
    free(trace->group_names);
    
    for (size_t i = 0; i < get_trace_frame_vec_count(trace->frames); i++) {
        free_frame(get_trace_frame_vec(trace->frames, i));
    }
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

trace_value_t trace_create_bitfield(uint32_t count, uint32_t* vals) {
    trace_value_t val;
    val.type = Type_Bitfield;
    val.count = count;
    val.group_index = -1;
    if (count == 1) {
        val.bitfield = *vals;
    } else {
        val.bitfield_array = malloc(count*sizeof(uint32_t));
        memcpy(val.bitfield_array, vals, count*sizeof(uint32_t));
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
        size_t len = strlen(*vals);
        val.str = malloc(len+1);
        memcpy(val.str, *vals, len);
        val.str[len] = 0;
    } else {
        val.str_array = malloc(sizeof(char*)*count);
        for (size_t i = 0; i < count; ++i) {
            size_t len = strlen(vals[i]);
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

uint32_t* trace_get_bitfield(trace_value_t* val) {
    return val->count==1 ? &val->bitfield : val->bitfield_array;
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
