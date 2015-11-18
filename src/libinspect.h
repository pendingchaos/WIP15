#ifndef LIB_INSPECT_H
#define LIB_INSPECT_H
#include "libtrace.h"
#include <stdbool.h>

typedef enum {
    AttachType_Info,
    AttachType_Warning,
    AttachType_Error
} attachment_type_t;

typedef struct inspect_attachment_t {
    attachment_type_t type;
    char* message;
    struct inspect_attachment_t* next;
} inspect_attachment_t;

typedef struct inspect_gl_state_entry_t {
    const char* name; //this must not be freed.
    trace_value_t val; //group is always NULL
    struct inspect_gl_state_entry_t* next;
} inspect_gl_state_entry_t;

typedef struct {
    inspect_gl_state_entry_t* entries;
} inspect_gl_state_t;

typedef struct inspect_command_t {
    trace_command_t* trace_cmd;
    inspect_attachment_t* attachments;
    uint64_t gl_context;
    char *name;
    uint64_t cpu_duration; //Nanoseconds
    uint64_t gpu_duration; //Nanoseconds
    inspect_gl_state_t state;
    struct inspect_command_t* next;
} inspect_command_t;

typedef struct inspect_frame_t {
    inspect_command_t* commands;
    trace_frame_t* trace_frame;
    struct inspect_frame_t* next;
} inspect_frame_t;

typedef struct {
    const trace_t *trace;
    inspect_frame_t* frames;
} inspection_t;

inspection_t* create_inspection(const trace_t* trace);
void free_inspection(inspection_t* inspection);
void inspect(inspection_t* inspection);
void inspect_add_info(inspect_command_t* command, const char* format, ...);
void inspect_add_warning(inspect_command_t* command, const char* format, ...);
void inspect_add_error(inspect_command_t* command, const char* format, ...);
void inspect_add_attachment(inspect_command_t* command, inspect_attachment_t* attach);
#endif
