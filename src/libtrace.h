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
    Type_Ptr
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
        uint64_t* u64;
        int64_t* i64;
        double* dbl;
        bool* bl;
        char** str;
        uint32_t* bitfield;
        uint64_t *ptr;
    };
    char *group;
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
    char **func_names;
    
    trace_frame_t *frames;
} trace_t;

trace_t* load_trace(const char* filename);
void free_trace(trace_t* trace);
trace_arg_t* trace_get_arg(trace_command_t* command, size_t i);
trace_command_t* trace_get_cmd(trace_frame_t* frame, size_t i);
trace_error_t get_trace_error();
const char *get_trace_error_desc();
#endif
