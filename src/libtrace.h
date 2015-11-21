#ifndef TRACE_FILE_H
#define TRACE_FILE_H

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

#include "vec.h"

typedef enum {
    Type_Void,
    Type_UInt,
    Type_Int,
    Type_Double,
    Type_Boolean,
    Type_Str,
    Type_Bitfield,
    Type_FunctionPtr,
    Type_Ptr,
    Type_Data
} type_t;

typedef enum {
    TraceError_None,
    TraceError_UnableToOpen,
    TraceError_Invalid
} trace_error_t;

typedef struct {
    type_t type;
    uint32_t count;
    union {
        uint64_t u64;
        int64_t i64;
        double dbl;
        bool bl;
        char* str;
        uint32_t bitfield;
        uint64_t ptr;
        void* data;
        uint64_t* u64_array;
        int64_t* i64_array;
        double* dbl_array;
        bool* bl_array;
        char** str_array;
        uint32_t* bitfield_array;
        uint64_t* ptr_array;
        void** data_array;
    };
    int32_t group_index; //Negative if there is no group
} trace_value_t;

typedef struct trace_arg_t {
    trace_value_t val;
} trace_arg_t;

typedef struct trace_command_t {
    uint32_t func_index;
    vec_t args; //trace_value_t
    trace_value_t ret;
} trace_command_t;

typedef struct trace_frame_t {
    vec_t commands; //trace_command_t
    struct trace_frame_t* next;
} trace_frame_t;

typedef struct {
    uint32_t func_name_count;
    char** func_names;
    
    uint32_t group_name_count;
    char** group_names;
    
    trace_frame_t *frames;
} trace_t;

trace_t* load_trace(const char* filename);
void free_trace(trace_t* trace);
void trace_free_value(trace_value_t value);
trace_arg_t* trace_get_arg(trace_command_t* command, size_t i);
trace_command_t* trace_get_cmd(trace_frame_t* frame, size_t i);
trace_error_t get_trace_error();
const char *get_trace_error_desc();
trace_value_t trace_create_uint(uint32_t count, uint64_t* vals);
trace_value_t trace_create_int(uint32_t count, int64_t* vals);
trace_value_t trace_create_double(uint32_t count, double* vals);
trace_value_t trace_create_bool(uint32_t count, bool* vals);
trace_value_t trace_create_bitfield(uint32_t count, uint32_t* vals);
trace_value_t trace_create_ptr(uint32_t count, uint64_t* vals);
trace_value_t trace_create_str(uint32_t count, const char*const* vals);
uint64_t* trace_get_uint(trace_value_t* val);
int64_t* trace_get_int(trace_value_t* val);
double* trace_get_double(trace_value_t* val);
bool* trace_get_bool(trace_value_t* val);
uint32_t* trace_get_bitfield(trace_value_t* val);
uint64_t* trace_get_ptr(trace_value_t* val);
char** trace_get_str(trace_value_t* val);
void** trace_get_data(trace_value_t* val);
#endif
