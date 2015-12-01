#ifndef LIB_INSPECT_H
#define LIB_INSPECT_H
#include "libtrace.h"
#include <stdbool.h>

typedef struct inspector_t inspector_t;

typedef enum {
    AttachType_Info,
    AttachType_Warning,
    AttachType_Error
} attachment_type_t;

typedef enum {
    InspectAction_GenTexture,
    InspectAction_DelTexture,
    InspectAction_TexParams,
    InspectAction_TexData,
    InspectAction_GenBuffer,
    InspectAction_DelBuffer,
    InspectAction_BufferData,
    InspectAction_BufferSubData,
    InspectAction_NewShader,
    InspectAction_DelShader,
    InspectAction_ShaderSource,
    InspectAction_NewProgram,
    InspectAction_DelProgram,
    InspectAction_UpdateProgramInfoLog,
    InspectAction_AttachShader,
    InspectAction_DetachShader,
    InspectAction_UpdateShdrInfoLog
} inspect_action_type_t;

typedef struct inspect_attachment_t {
    attachment_type_t type;
    char* message;
    struct inspect_attachment_t* next;
} inspect_attachment_t;

typedef struct inspect_gl_state_entry_t {
    const char* name; //this must not be freed.
    trace_value_t val;
} inspect_gl_state_entry_t;

typedef struct {
    uint32_t width;
    uint32_t height;
    uint8_t* data;
} inspect_image_t;

typedef struct {
    unsigned int texture;
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
    unsigned int texture;
    size_t mipmap;
    size_t data_size;
    void* data;
} inspect_gl_tex_data_t;

typedef struct {
    unsigned int buffer;
    uint32_t size;
    void* data;
    unsigned int usage;
} inspect_gl_buffer_data_t;

typedef struct {
    unsigned int buffer;
    uint32_t offset;
    uint32_t size;
    void* data;
} inspect_gl_buffer_sub_data_t;

typedef struct {
    unsigned int shader;
    unsigned int type;
} inspect_gl_create_shader_t;

typedef struct {
    unsigned int shader;
    size_t count;
    char** sources;
} inspect_gl_shader_source_t;

typedef struct {
    unsigned int program;
    unsigned int shader;
} inspect_gl_prog_shdr_t;

typedef struct {
    unsigned int obj;
    char* str;
} inspect_gl_info_log_t;

typedef struct {
    inspect_action_type_t type;
    union {
        unsigned int texture; //GenTexture and DelTexture
        inspect_gl_tex_params_t tex_params; //TexParams
        inspect_gl_tex_data_t tex_data; //TexData
        unsigned int buffer; //GenBuffer and DelBuffer
        inspect_gl_buffer_data_t buf_data; //BufferData
        inspect_gl_buffer_sub_data_t buf_sub_data; //BufferSubData
        inspect_gl_create_shader_t new_shader; //NewShader
        unsigned int del_shader; //DelShader
        inspect_gl_shader_source_t shader_source; //ShaderSource
        unsigned int program; //NewProgram, DelProgram
        inspect_gl_prog_shdr_t prog_shdr; //AttachShader, DetachShader
        inspect_gl_info_log_t info_log; //UpdateProgramInfoLog, UpdateShdrInfoLog
    };
} inspect_action_t;

typedef struct {
    vec_t entries; //inspect_gl_state_entry_t
    inspect_image_t back; //data is NULL if it did not change. RGBA
    inspect_image_t front; //data is NULL if it did not change. RGBA
    inspect_image_t depth; //data is NULL if it did not change. uint32_t
    vec_t actions; //vec_t of inspect_action_t
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

inspector_t* create_inspector(inspection_t* inspection);
void free_inspector(inspector_t* inspector);
void seek_inspector(inspector_t* inspector, size_t frame, size_t cmd);
size_t inspect_get_tex_count(inspector_t* inspector);
size_t inspect_get_buf_count(inspector_t* inspector);
size_t inspect_get_shdr_count(inspector_t* inspector);
size_t inspect_get_prog_count(inspector_t* inspector);
//0 on failure
unsigned int inspect_get_tex(inspector_t* inspector, size_t index);
//Negative if it could not be found
int inspect_find_tex(inspector_t* inspector, unsigned int tex);
//True if it succeeded.
bool inspect_get_tex_params(inspector_t* inspector, size_t index, inspect_gl_tex_params_t* dest);
bool inspect_get_tex_data(inspector_t* inspector, size_t index, size_t level, void** data);
//0 on failure
unsigned int inspect_get_buf(inspector_t* inspector, size_t index);
//Negative if it could not be found
int inspect_find_buf(inspector_t* inspector, unsigned int buf);
//Negative on failure
int inspect_get_buf_size(inspector_t* inspector, size_t index);
//True if it succeeded
bool inspect_get_buf_data(inspector_t* inspector, size_t index, void** data);
//0 on failure
unsigned int inspect_get_shdr(inspector_t* inspector, size_t index);
//Negative on failure
int inspect_find_shdr(inspector_t* inspector, unsigned int shdr);
//Negative on failure
int inspect_get_shdr_type(inspector_t* inspector, size_t index);
//True if it succeeded. The result may be null.
bool inspect_get_shdr_source(inspector_t* inspector, size_t index, char** source);
//True if it succeeded. The result may be null.
bool inspect_get_shdr_info_log(inspector_t* inspector, size_t index, char** info_log);
//0 on failure
unsigned int inspect_get_prog(inspector_t* inspector, size_t index);
//Negative on failure
int inspect_find_prog(inspector_t* inspector, unsigned int prog);
//True if it succeeded. shaders is a vec_t of unsigned int.
bool inspect_get_prog_shaders(inspector_t* inspector, size_t index, vec_t* shaders);
//True if it succeeded. The result may be null.
bool inspect_get_prog_info_log(inspector_t* inspector, size_t index, char** info_log);
#endif
