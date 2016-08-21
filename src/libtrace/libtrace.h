#ifndef TRACE_FILE_H
#define TRACE_FILE_H

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

#include "shared/vec.h"
#include "shared/uint.h"

#define TRC_GL_OBJ_HEAD uint64_t revision;\
uint64_t fake_context;\
uint64_t real;\
uint ref_count;\

typedef enum {
    Type_Void,
    Type_UInt,
    Type_Int,
    Type_Double,
    Type_Boolean,
    Type_Str,
    Type_FunctionPtr,
    Type_Ptr,
    Type_Data
} trace_type_t;

typedef enum {
    TraceError_None,
    TraceError_UnableToOpen,
    TraceError_Invalid
} trace_error_t;

typedef enum trc_attachment_type_t {
    TrcAttachType_Info,
    TrcAttachType_Warning,
    TrcAttachType_Error
} trc_attachment_type_t;

typedef struct trc_attachment_t {
    trc_attachment_type_t type;
    char* message;
    struct trc_attachment_t* next;
} trc_attachment_t;

typedef struct {
    trace_type_t type;
    uint32_t count;
    union {
        uint64_t u64;
        int64_t i64;
        double dbl;
        bool bl;
        char* str;
        uint64_t ptr;
        void* data; //TODO: The size is not stored
        uint64_t* u64_array;
        int64_t* i64_array;
        double* dbl_array;
        bool* bl_array;
        char** str_array;
        uint64_t* ptr_array;
        void** data_array; //TODO: The size is not stored
    };
    int32_t group_index; //Negative if there is no group
} trace_value_t;
TYPED_VEC(trace_value_t, trace_val)

typedef enum trc_compression_t {
    TrcCompression_None,
    TrcCompression_Zlib,
    TrcCompression_LZ4
} trc_compression_t;

typedef enum trc_gl_obj_type_t {
    TrcGLObj_Buffer,
    TrcGLObj_Sampler,
    TrcGLObj_Texture,
    TrcGLObj_Query,
    TrcGLObj_Framebuffer,
    TrcGLObj_Renderbuffer,
    TrcGLObj_Sync,
    TrcGLObj_Program,
    TrcGLObj_ProgramPipeline,
    TrcGLObj_Shader,
    TrcGLObj_VAO,
    TrcGLObj_TransformFeedback,
    TrcGLObj_Max
} trc_gl_obj_type_t;

typedef struct trc_data_t {
    size_t uncompressed_size;
    size_t compressed_size;
    trc_compression_t compression;
    void* compressed_data;
} trc_data_t;

typedef struct trc_gl_obj_rev_t {
    TRC_GL_OBJ_HEAD
} trc_gl_obj_rev_t;

typedef struct trc_gl_buffer_rev_t {
    TRC_GL_OBJ_HEAD
    bool has_data;
    uint data_usage;
    trc_data_t* data;
    
    bool mapped;
    size_t map_offset;
    size_t map_length;
    uint map_access; //Bitfield of GL_MAP_*_BIT
} trc_gl_buffer_rev_t;

typedef struct trc_gl_sample_params_t {
    uint min_filter;
    uint mag_filter;
    float min_lod;
    float max_lod;
    uint wrap_s;
    uint wrap_t;
    uint wrap_r;
    float border_color[4];
    uint compare_mode;
    uint compare_func;
} trc_gl_sample_params_t;

typedef struct trc_gl_sampler_rev_t {
    TRC_GL_OBJ_HEAD
} trc_gl_sampler_rev_t;

typedef struct trc_gl_texture_rev_t {
    TRC_GL_OBJ_HEAD
    trc_gl_sample_params_t sample_params;
    uint depth_stencil_mode;
    uint base_level;
    uint max_level;
    float lod_bias;
    uint swizzle[4];
} trc_gl_texture_rev_t;

typedef struct trc_gl_query_rev_t {
    TRC_GL_OBJ_HEAD
    uint type;
    int64_t result;
} trc_gl_query_rev_t;

typedef struct trc_gl_framebuffer_rev_t {
    TRC_GL_OBJ_HEAD
} trc_gl_framebuffer_rev_t;

typedef struct trc_gl_renderbuffer_rev_t {
    TRC_GL_OBJ_HEAD
} trc_gl_renderbuffer_rev_t;

typedef struct trc_gl_sync_rev_t {
    TRC_GL_OBJ_HEAD
    uint type;
    uint condition;
    uint flags;
} trc_gl_sync_rev_t;

typedef struct trc_gl_program_rev_t {
    TRC_GL_OBJ_HEAD
    size_t uniform_count;
    size_t vertex_attrib_count;
    //TODO: Move these arrays into trc_data_t(s)
    int* uniforms; //[real0, fake0, real1, fake1, ...]
    int* vertex_attribs; //[real0, fake0, real1, fake1, ...]
    
    size_t shader_count;
    uint* shaders;
    uint* shader_revisions;
    
    char* info_log;
} trc_gl_program_rev_t;

typedef struct trc_gl_program_pipeline_rev_t {
    TRC_GL_OBJ_HEAD
} trc_gl_program_pipeline_rev_t;

typedef struct trc_gl_shader_rev_t {
    TRC_GL_OBJ_HEAD
    uint type;
    size_t source_count;
    size_t* source_lengths;
    char** sources;
    char* info_log;
} trc_gl_shader_rev_t;

typedef struct trc_gl_vao_attrib_t {
    bool enabled;
    bool normalized;
    bool integer;
    uint size;
    uint stride;
    uint offset;
    uint type;
    uint divisor;
    uint buffer;
    double value[4];
} trc_gl_vao_attrib_t;

typedef struct trc_gl_vao_rev_t {
    TRC_GL_OBJ_HEAD
    size_t attrib_count;
    trc_gl_vao_attrib_t* attribs;
} trc_gl_vao_rev_t;

typedef struct trc_gl_transform_feedback_rev_t {
    TRC_GL_OBJ_HEAD
} trc_gl_transform_feedback_rev_t;

typedef struct trc_gl_obj_history_t {
    uint64_t fake;
    size_t revision_count;
    void* revisions; //Sorted from lowest revision to highest
} trc_gl_obj_history_t;

typedef struct trc_gl_context_rev_t {
    uint64_t revision;
    void* real;
    
    //TODO: Change these names to names like buf_array and buf_atomic_counter
    uint array_buffer;
    uint atomic_counter_buffer;
    uint copy_read_buffer;
    uint copy_write_buffer;
    uint dispatch_indirect_buffer;
    uint draw_indirect_buffer;
    uint element_array_buffer;
    uint pixel_pack_buffer;
    uint pixel_unpack_buffer;
    uint query_buffer;
    uint shader_storage_buffer;
    uint texture_buffer;
    uint transform_feedback_buffer;
    uint uniform_buffer;
    
    uint bound_program;
    uint bound_vao;
    
    uint read_framebuffer;
    uint draw_framebuffer;
    //TODO: Add a GL_FRAMEBUFFER field?
    
    uint samples_passed_query;
    uint any_samples_passed_query;
    uint any_samples_passed_conservative_query;
    uint primitives_generated_query;
    uint transform_feedback_primitives_written_query;
    uint time_elapsed_query;
    uint timestamp_query;
    
    uint active_texture_unit;
    
    trc_data_t* tex_1d;
    trc_data_t* tex_2d;
    trc_data_t* tex_3d;
    trc_data_t* tex_1d_array;
    trc_data_t* tex_2d_array;
    trc_data_t* tex_rectangle;
    trc_data_t* tex_cube_map;
    trc_data_t* tex_cube_map_array;
    trc_data_t* tex_buffer;
    trc_data_t* tex_2d_multisample;
    trc_data_t* tex_2d_multisample_array;
} trc_gl_context_rev_t;

typedef struct trc_gl_context_history_t {
    uint64_t fake;
    size_t revision_count;
    trc_gl_context_rev_t* revisions; //Sorted from lowest revision to highest
} trc_gl_context_history_t;

typedef struct trc_gl_inspection_t {
    uint cur_revision;
    
    size_t data_count;
    trc_data_t** data;
    
    size_t gl_obj_history_count[TrcGLObj_Max];
    trc_gl_obj_history_t* gl_obj_history[TrcGLObj_Max];
    
    size_t gl_context_history_count;
    trc_gl_context_history_t* gl_context_history;
    
    uint64_t cur_fake_context;
} trc_gl_inspection_t;

typedef struct {
    char* name;
    size_t size;
    void* data;
} trace_extra_t;

typedef struct trace_command_t {
    uint32_t func_index;
    trace_val_vec_t args;
    trace_value_t ret;
    
    uint32_t extra_count;
    trace_extra_t* extras;
    
    uint revision; //Revision after the command
    trc_attachment_t* attachments;
} trace_command_t;

typedef struct trace_frame_t {
    size_t command_count;
    trace_command_t* commands;
} trace_frame_t;

typedef struct trace_t {
    bool little_endian;
    
    uint32_t func_name_count;
    char** func_names;
    
    uint32_t group_name_count;
    char** group_names;
    
    size_t frame_count;
    trace_frame_t* frames;
    
    trc_gl_inspection_t inspection;
} trace_t;

typedef struct trc_replay_context_t {
    void* _replay_gl;
    trace_t* trace;
    struct SDL_Window* window;
    const char* current_test_name;
} trc_replay_context_t;

trace_t* load_trace(const char* filename);
void free_trace(trace_t* trace);
void trc_free_value(trace_value_t value);
trace_value_t* trc_get_arg(trace_command_t* command, size_t i);
trace_command_t* trc_get_cmd(trace_frame_t* frame, size_t i);
trace_error_t trc_get_error();
const char *trc_get_error_desc();
trace_value_t trc_create_uint(uint32_t count, uint64_t* vals);
trace_value_t trc_create_int(uint32_t count, int64_t* vals);
trace_value_t trc_create_double(uint32_t count, double* vals);
trace_value_t trc_create_bool(uint32_t count, bool* vals);
trace_value_t trc_create_ptr(uint32_t count, uint64_t* vals);
trace_value_t trc_create_str(uint32_t count, const char*const* vals);
uint64_t* trc_get_uint(trace_value_t* val);
int64_t* trc_get_int(trace_value_t* val);
double* trc_get_double(trace_value_t* val);
bool* trc_get_bool(trace_value_t* val);
uint64_t* trc_get_ptr(trace_value_t* val);
char** trc_get_str(trace_value_t* val);
void** trc_get_data(trace_value_t* val);
trace_extra_t* trc_get_extra(trace_command_t* cmd, const char* name);
trace_extra_t* trc_get_extrai(trace_command_t* cmd, const char* name, size_t index);
void trc_add_info(trace_command_t* command, const char* format, ...);
void trc_add_warning(trace_command_t* command, const char* format, ...);
void trc_add_error(trace_command_t* command, const char* format, ...);
void trc_run_inspection(trace_t* trace);

const trc_gl_buffer_rev_t* trc_get_gl_buffer(trace_t* trace, uint fake);
void trc_set_gl_buffer(trace_t* trace, uint fake, const trc_gl_buffer_rev_t* rev);
uint trc_get_real_gl_buffer(trace_t* trace, uint fake);

const trc_gl_sampler_rev_t* trc_get_gl_sampler(trace_t* trace, uint fake);
void trc_set_gl_sampler(trace_t* trace, uint fake, const trc_gl_sampler_rev_t* rev);
uint trc_get_real_gl_sampler(trace_t* trace, uint fake);

const trc_gl_texture_rev_t* trc_get_gl_texture(trace_t* trace, uint fake);
void trc_set_gl_texture(trace_t* trace, uint fake, const trc_gl_texture_rev_t* rev);
uint trc_get_real_gl_texture(trace_t* trace, uint fake);

const trc_gl_query_rev_t* trc_get_gl_query(trace_t* trace, uint fake);
void trc_set_gl_query(trace_t* trace, uint fake, const trc_gl_query_rev_t* rev);
uint trc_get_real_gl_query(trace_t* trace, uint fake);

const trc_gl_framebuffer_rev_t* trc_get_gl_framebuffer(trace_t* trace, uint fake);
void trc_set_gl_framebuffer(trace_t* trace, uint fake, const trc_gl_framebuffer_rev_t* rev);
uint trc_get_real_gl_framebuffer(trace_t* trace, uint fake);

const trc_gl_renderbuffer_rev_t* trc_get_gl_renderbuffer(trace_t* trace, uint fake);
void trc_set_gl_renderbuffer(trace_t* trace, uint fake, const trc_gl_renderbuffer_rev_t* rev);
uint trc_get_real_gl_renderbuffer(trace_t* trace, uint fake);

const trc_gl_sync_rev_t* trc_get_gl_sync(trace_t* trace, uint64_t fake);
void trc_set_gl_sync(trace_t* trace, uint64_t fake, const trc_gl_sync_rev_t* rev);
uint64_t trc_get_real_gl_sync(trace_t* trace, uint64_t fake);

const trc_gl_program_rev_t* trc_get_gl_program(trace_t* trace, uint fake);
void trc_set_gl_program(trace_t* trace, uint fake, const trc_gl_program_rev_t* rev);
uint trc_get_real_gl_program(trace_t* trace, uint fake);

const trc_gl_program_pipeline_rev_t* trc_get_gl_program_pipeline(trace_t* trace, uint fake);
void trc_set_gl_program_pipeline(trace_t* trace, uint fake, const trc_gl_program_pipeline_rev_t* rev);
uint trc_get_real_gl_program_pipeline(trace_t* trace, uint fake);

const trc_gl_shader_rev_t* trc_get_gl_shader(trace_t* trace, uint fake);
void trc_set_gl_shader(trace_t* trace, uint fake, const trc_gl_shader_rev_t* rev);
uint trc_get_real_gl_shader(trace_t* trace, uint fake);

const trc_gl_vao_rev_t* trc_get_gl_vao(trace_t* trace, uint fake);
void trc_set_gl_vao(trace_t* trace, uint fake, const trc_gl_vao_rev_t* rev);
uint trc_get_real_gl_vao(trace_t* trace, uint fake);

const trc_gl_transform_feedback_rev_t* trc_get_gl_transform_feedback(trace_t* trace, uint fake);
void trc_set_gl_transform_feedback(trace_t* trace, uint fake, const trc_gl_transform_feedback_rev_t* rev);
uint trc_get_real_gl_transform_feedback(trace_t* trace, uint fake);

void trc_grab_gl_obj(trace_t* trace, uint64_t fake, trc_gl_obj_type_t type); //Increase the reference count
void trc_rel_gl_obj(trace_t* trace, uint64_t fake, trc_gl_obj_type_t type); //Decrease the reference count
trc_gl_obj_rev_t* trc_lookup_gl_obj(trace_t* trace, uint revision, uint64_t fake, trc_gl_obj_type_t type);

const trc_gl_context_rev_t* trc_get_gl_context(trace_t* trace, uint64_t fake);
void trc_set_gl_context(trace_t* trace, uint64_t fake, const trc_gl_context_rev_t* rev);
void* trc_get_real_gl_context(trace_t* trace, uint64_t fake);

trc_data_t* trc_create_data(trace_t* trace, size_t size, const void* data);
void* trc_lock_data(trc_data_t* data, bool read, bool write);
void trc_unlock_data(trc_data_t* data);
#endif
