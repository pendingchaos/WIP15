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
    trace_value_t val;
    struct inspect_gl_state_entry_t* next;
} inspect_gl_state_entry_t;

typedef struct {
    uint32_t width;
    uint32_t height;
    uint8_t* data;
} inspect_image_t;

typedef struct {
    unsigned int fake_texture;
    unsigned int type;
    unsigned int min_filter;
    unsigned int mag_filter;
    float min_lod;
    float max_lod;
    int base_level;
    int max_level;
    unsigned int wrap_s;
    unsigned int wrap_t;
    unsigned int wrap_r;
    float priority;
    unsigned int compare_mode;
    unsigned int compare_func;
    unsigned int depth_texture_mode;
    bool generate_mipmap;
    unsigned int depth_stencil_mode;
    float lod_bias;
    unsigned int swizzle[4];
    float border_color[4];
    unsigned int width;
    unsigned int height;
    unsigned int depth;
    unsigned int internal_format;
} inspect_gl_tex_params_t;

typedef struct {
    unsigned int fake_texture;
    size_t mipmap;
    size_t data_size;
    void* data;
} inspect_gl_tex_data_t;

typedef struct {
    vec_t entries; //inspect_gl_state_entry_t
    inspect_image_t color; //data is NULL if it did not change. RGBA
    inspect_image_t depth; //data is NULL if it did not change. uint32_t
    vec_t texture_params; //vec_t of inspect_gl_tex_params_t.
    vec_t texture_data; //vec_t of inspect_gl_tex_data_t.
} inspect_gl_state_t;

typedef struct inspect_command_t {
    trace_command_t* trace_cmd;
    inspect_attachment_t* attachments;
    uint64_t gl_context;
    char *name;
    uint64_t cpu_duration; //Nanoseconds
    uint64_t gpu_duration; //Nanoseconds
    inspect_gl_state_t state;
} inspect_command_t;

typedef struct {
    size_t command_count;
    inspect_command_t* commands;
    trace_frame_t* trace_frame;
} inspect_frame_t;

typedef struct {
    const trace_t *trace;
    size_t frame_count;
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
