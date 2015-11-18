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

static void free_value(trace_value_t value) {
    switch (value.type) {
    case Type_UInt:
    case Type_Int:
    case Type_Double:
    case Type_Boolean:
    case Type_Bitfield:
    case Type_Ptr:
        free(value.u64);
        break;
    case Type_Str:
        for (size_t i = 0; i < value.count; ++i) {
            free(value.str[i]);
        }
        free(value.str);
        break;
    case Type_Void:
    case Type_FunctionPtr:
        break;
    }
    
    free(value.group);
}

static void free_command(trace_command_t* command) {
    vec_t args = command->args;
    for (size_t i = 0; i < get_vec_size(args)/sizeof(trace_arg_t); ++i) {
        trace_arg_t* arg = ((trace_arg_t*)get_vec_data(args)) + i;
        free_value(arg->val);
    }
    free_vec(args);
    
    free_value(command->ret);
    free(command);
}

static void free_frame(trace_frame_t* frame) {
    size_t count = get_vec_size(frame->commands) / sizeof(trace_command_t);
    for (size_t i = 0; i < count; ++i) {
        free_command(trace_get_cmd(frame, i));
    }
    
    free(frame);
}

static void set_int(trace_value_t *val, int64_t i) {
    val->type = Type_Int;
    val->i64 = malloc(sizeof(int64_t));
    *val->i64 = i;
    val->count = 1;
}

static void set_uint(trace_value_t *val, uint64_t i) {
    val->type = Type_UInt;
    val->u64 = malloc(sizeof(uint64_t));
    *val->u64 = i;
    val->count = 1;
}

//-1 == Error
//0 = End
//1 = Arg
//2 = Result
//TODO: Some of this code assumes little endian.
static int read_val(FILE* file, trace_value_t *val) {
    uint8_t type;
    if (!readf(&type, 1, 1, file)) {
        trace_error_desc = "Unable to read type";
        return -1;
    }
    
    val->type = Type_Void;
    val->group = NULL;
    
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
        
        val->type = Type_Int;
        val->i64 = malloc(sizeof(int64_t)*val->count);
        
        for (size_t i = 0; i < val->count; ++i) {
            uint32_t v;
            if (!readf(&v, 4, 1, file)) {
                free(val->i64);
                trace_error_desc = "Unable to read uint32_t array element";
                return -1;
            }
            val->i64[i] = le32toh(v);
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
        val->str = malloc(sizeof(char *));
        *val->str = malloc(length+1);
        if (!readf(*val->str, length, 1, file)) {
            free(*val->str);
            free(val->str);
            trace_error_desc = "Unable to read string data";
            return -1;
        }
        (*val->str)[length] = 0;
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
        val->str = malloc(sizeof(uint64_t *));
        val->ptr[0] = le32toh(v);
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
        val->str = malloc(sizeof(uint64_t *));
        val->ptr[0] = le64toh(v);
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
        set_uint(val, v);
        group = true;
        break;
    }
    case 9: { //int16_t
        int16_t v;
        if (!readf(&v, 2, 1, file)) {
            trace_error_desc = "Unable to read 16-bit signed integer";
            return -1;
        }
        set_int(val, v);
        group = true;
        break;
    }
    case 10: { //uint32_t
        uint32_t v;
        if (!readf(&v, 4, 1, file)) {
            trace_error_desc = "Unable to read 32-bit unsigned integer";
            return -1;
        }
        set_uint(val, v);
        group = true;
        break;
    }
    case 11: { //int32_t
        int32_t v;
        if (!readf(&v, 4, 1, file)) {
            trace_error_desc = "Unable to read 32-bit signed integer";
            return -1;
        }
        set_int(val, v);
        group = true;
        break;
    }
    case 12: { //uint64_t
        uint64_t v;
        if (!readf(&v, 8, 1, file)) {
            trace_error_desc = "Unable to read 64-bit unsigned integer";
            return -1;
        }
        set_uint(val, v);
        group = true;
        break;
    }
    case 13: { //int64_t
        int64_t v;
        if (!readf(&v, 8, 1, file)) {
            trace_error_desc = "Unable to read 64-bit signed integer";
            return -1;
        }
        set_int(val, v);
        group = true;
        break;
    }
    case 14: { //boolean
        int8_t v;
        if (!readf(&v, 1, 1, file)) {
            trace_error_desc = "Unable to read boolean";
            return -1;
        }
        val->type = Type_Boolean;
        val->count = 1;
        val->bl = malloc(sizeof(bool));
        *val->bl = v;
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
        val->bitfield = malloc(sizeof(uint32_t));
        *val->bitfield = v;
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
        val->dbl = malloc(sizeof(double));
        *val->dbl = v;
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
        val->dbl = malloc(sizeof(double));
        *val->dbl = v;
        group = true;
        break;
    }
    case 18: { //result
        int res = read_val(file, val);
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
        val->str = malloc(sizeof(char *)*val->count);
        
        for (size_t i = 0; i < val->count; ++i) val->str[i] = NULL;
        
        for (size_t i = 0; i < val->count; ++i) {
            uint32_t length;
            if (!readf(&length, 4, 1, file)) {
                free_value(*val);
                trace_error_desc = "Unable to read length for string array element";
                return -1;
            }
            length = le32toh(length);
            
            char *str = malloc(length+1);
            if (!readf(val->str, length, 1, file)) {
                free(str);
                free_value(*val);
                trace_error_desc = "Unable to read data for string array element";
                return -1;
            }
            str[length] = 0;
            
            val->str[i] = str;
        }
        break;
    }
    default: {
        trace_error_desc = "Invalid value type";
        return -1;
    }
    }
    
    if (group) {
        uint32_t length;
        if (!readf(&length, 4, 1, file)) {
            free_value(*val);
            trace_error_desc = "Unable to read group length";
            return -1;
        }
        length = le32toh(length);
        
        val->group = malloc(length+1);
        if (!readf(val->group, length, 1, file)) {
            free_value(*val);
            trace_error_desc = "Unable to read group data";
            return -1;
        }
        val->group[length] = 0;
    } else {
        val->group = malloc(1);
        val->group[0] = 0;
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
    trace->frames = NULL;
    
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
    
    trace_frame_t* frame = malloc(sizeof(trace_frame_t));
    frame->commands = NULL;
    frame->next = NULL;
    trace->frames = frame;
    
    while (true) {
        if (fgetc(file) == EOF) {
            break;
        }
        
        fseek(file, -1, SEEK_CUR);
        
        trace_command_t command;
        command.func_index = 0;
        command.args = NULL;
        command.ret.type = Type_Void;
        command.ret.group = malloc(1);
        command.ret.group[0] = 0;
        command.next = NULL;
        
        if (!readf(&command.func_index, 4, 1, file)) {
            trace_error = TraceError_Invalid;
            trace_error_desc = "Unable to read function index";
            free_trace(trace);
            return NULL;
        }
        command.func_index = le32toh(command.func_index);
        
        if (command.func_index > trace->func_name_count) {
            trace_error = TraceError_Invalid;
            trace_error_desc = "Invalid function index";
            free_trace(trace);
            return NULL;
        }
        
        int res;
        trace_value_t val;
        while ((res = read_val(file, &val)) != 0) {
            if (res == -1) {
                free_trace(trace);
                trace_error = TraceError_Invalid;
                return NULL;
            } else if (res == 1) {
                trace_arg_t arg;
                arg.val = val;
                arg.next = NULL;
                append_vec(command.args, sizeof(trace_arg_t), &arg);
            } else if (res == 2) {
                free_value(command.ret);
                command.ret = val;
            }
        }
        
        append_vec(frame->commands, sizeof(trace_command_t), &command);
        
        if (strcmp(trace->func_names[command.func_index], "glXSwapBuffers") == 0) {
            trace_frame_t *old_frame = frame;
            
            frame = malloc(sizeof(trace_frame_t));
            frame->commands = NULL;
            frame->next = NULL;
            old_frame->next = frame;
        }
    }
    
    fclose(file);
    
    return trace;
}

void free_trace(trace_t* trace) {
    for (size_t i = 0; i < trace->func_name_count; ++i) {
        free(trace->func_names[i]);
    }
    
    free(trace->func_names);
    
    trace_frame_t *frame = trace->frames;
    while (frame) {
        trace_frame_t* next_frame = frame->next;
        free_frame(frame);
        frame = next_frame;
    }
    
    free(trace);
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
