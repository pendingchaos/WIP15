#ifndef TRACE_FILE_H
#define TRACE_FILE_H

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include <pthread.h>

#include "shared/vec.h"
#include "shared/uint.h"

#define TRC_GL_OBJ_HEAD uint64_t revision;\
uint64_t fake_context;\
uint64_t real;\
uint ref_count;\

#define TRC_DATA_IMMUTABLE (1<<0)
#define TRC_DATA_NO_COMPRESS (1<<1)
#define TRC_DATA_NO_ZERO (1<<2) //Used for trc_create_data
#define TRC_MAP_READ (1<<0)
#define TRC_MAP_WRITE (1<<1)
#define TRC_MAP_MODIFY (TRC_MAP_READ|TRC_MAP_WRITE)
#define TRC_MAP_REPLACE TRC_MAP_WRITE

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

typedef enum trc_storage_type_t {
    TrcStorage_Plain,
    TrcStorage_External
} trc_storage_type_t;

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
        struct {
            size_t size;
            void* ptr;
        } data;
        uint64_t* u64_array;
        int64_t* i64_array;
        double* dbl_array;
        bool* bl_array;
        char** str_array;
        uint64_t* ptr_array;
        struct {
            size_t* sizes;
            void** ptrs;
        } data_array;
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

typedef struct trc_data_external_storage_t {
    trc_compression_t compression:32;
    uint32_t size;
    void* data;
} trc_data_external_storage_t;

typedef struct trc_data_t {
    uint8_t mutex;
    uint8_t flags;
    trc_storage_type_t storage_type:16;
    uint32_t size;
    union {
        void* plain;
        trc_data_external_storage_t* external; //only for immutable data
    };
    struct trc_data_t* queue_next; //this is not guarded by trc_data_t::mutex
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

typedef struct trc_gl_texture_image_t {
    uint face; //0 if the texture is not a non-array cubemap
    uint level;
    uint internal_format;
    uint width;
    uint height;
    uint depth;
    trc_data_t* data; //array of uint32_t, int32_t, float or float+uint32_t depending on the internal format
} trc_gl_texture_image_t;

typedef struct trc_gl_texture_rev_t {
    TRC_GL_OBJ_HEAD
    trc_gl_sample_params_t sample_params;
    uint depth_stencil_mode;
    uint base_level;
    uint max_level;
    float lod_bias;
    uint swizzle[4];
    
    bool created;
    uint type;
    //array of trc_gl_texture_image_t
    trc_data_t* images;
} trc_gl_texture_rev_t;

typedef struct trc_gl_query_rev_t {
    TRC_GL_OBJ_HEAD
    uint type;
    int64_t result;
} trc_gl_query_rev_t;

typedef struct trc_gl_framebuffer_attachment_t {
    uint attachment;
    bool has_renderbuffer;
    //when has_renderbuffer == true
    uint fake_renderbuffer;
    //when has_renderbuffer == false
    uint fake_texture;
    uint level;
    uint layer;
    uint face; //0 for non-cubemap or non-cubemap-array textures
} trc_gl_framebuffer_attachment_t;

typedef struct trc_gl_framebuffer_rev_t {
    TRC_GL_OBJ_HEAD
    trc_data_t* attachments; //array of trc_gl_framebuffer_attachment_t
    trc_data_t* draw_buffers; //array of GLenum
} trc_gl_framebuffer_rev_t;

typedef struct trc_gl_renderbuffer_rev_t {
    TRC_GL_OBJ_HEAD
    bool has_storage;
    uint width;
    uint height;
    uint internal_format;
    uint sample_count;
    uint rgba_bits[4];
    uint depth_bits;
    uint stencil_bits;
} trc_gl_renderbuffer_rev_t;

typedef struct trc_gl_sync_rev_t {
    TRC_GL_OBJ_HEAD
    uint type;
    uint condition;
    uint flags;
} trc_gl_sync_rev_t;

typedef struct trc_gl_program_shader_t {
    uint fake_shader;
    uint shader_revision;
} trc_gl_program_shader_t;

typedef struct trc_gl_program_uniform_t {
    uint real; //real location
    uint fake; //fake location
    uint dim[2];
    uint count; //for non-matrices: 0 -> non-array
    trc_data_t* value; //array of double
} trc_gl_program_uniform_t;

typedef struct trc_gl_program_uniform_block_t {
    uint real; //real index
    uint fake; //fake index
    uint binding;
} trc_gl_program_uniform_block_t;

typedef struct trc_gl_program_rev_t {
    TRC_GL_OBJ_HEAD
    trc_data_t* uniforms; //array of trc_gl_program_uniform_t
    trc_data_t* vertex_attribs; //int[]{real0, fake0, real1, fake1, ...}
    trc_data_t* uniform_blocks; //array of trc_gl_program_uniform_block_t
    
    trc_data_t* shaders; //array of trc_gl_program_shader_t
    
    trc_data_t* info_log; //null-terminated ascii
} trc_gl_program_rev_t;

typedef struct trc_gl_program_pipeline_rev_t {
    TRC_GL_OBJ_HEAD
} trc_gl_program_pipeline_rev_t;

typedef struct trc_gl_shader_rev_t {
    TRC_GL_OBJ_HEAD
    uint type;
    trc_data_t* sources; //concatenated null-terminated ascii
    trc_data_t* info_log; //null-terminated ascii
} trc_gl_shader_rev_t;

typedef struct trc_gl_vao_attrib_t {
    bool enabled;
    bool normalized;
    bool integer;
    uint size;
    uint stride;
    uint64_t offset;
    uint type;
    uint divisor;
    uint buffer;
} trc_gl_vao_attrib_t;

typedef struct trc_gl_vao_rev_t {
    TRC_GL_OBJ_HEAD
    trc_data_t* attribs; //array of trc_gl_vao_attrib_t
} trc_gl_vao_rev_t;

typedef struct trc_gl_transform_feedback_rev_t {
    TRC_GL_OBJ_HEAD
} trc_gl_transform_feedback_rev_t;

typedef struct trc_gl_obj_history_t {
    uint64_t fake;
    size_t revision_count;
    void* revisions; //Sorted from lowest revision to highest
} trc_gl_obj_history_t;

#define WIP15_STATE_GEN_DECL
#include "libtrace_glstate.h"
#undef WIP15_STATE_GEN_DECL

typedef struct trc_gl_context_history_t {
    uint64_t fake;
    size_t revision_count;
    trc_gl_context_rev_t* revisions; //Sorted from lowest revision to highest
} trc_gl_context_history_t;

typedef struct trc_cur_context_rev_t {
    uint64_t revision;
    uint64_t context;
} trc_cur_context_rev_t;

typedef struct trc_gl_inspection_t {
    uint cur_revision;
    
    size_t data_count;
    trc_data_t** data;
    
    size_t gl_obj_history_count[TrcGLObj_Max];
    trc_gl_obj_history_t* gl_obj_history[TrcGLObj_Max];
    
    size_t gl_context_history_count;
    trc_gl_context_history_t* gl_context_history;
    
    size_t cur_ctx_revision_count;
    trc_cur_context_rev_t* cur_ctx_revisions;
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
    
    uint8_t data_queue_mutex;
    trc_data_t* data_queue_start;
    trc_data_t* data_queue_end;
    
    bool threads_running;
    size_t thread_count;
    pthread_t* threads;
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
const uint64_t* trc_get_uint(const trace_value_t* val);
const int64_t* trc_get_int(const trace_value_t* val);
const double* trc_get_double(const trace_value_t* val);
const bool* trc_get_bool(const trace_value_t* val);
const uint64_t* trc_get_ptr(const trace_value_t* val);
const char*const* trc_get_str(const trace_value_t* val);
const size_t* trc_get_data_sizes(const trace_value_t* val);
const void*const* trc_get_data(const trace_value_t* val);
trace_extra_t* trc_get_extra(trace_command_t* cmd, const char* name);
trace_extra_t* trc_get_extrai(trace_command_t* cmd, const char* name, size_t index);
void trc_add_info(trace_command_t* command, const char* format, ...);
void trc_add_warning(trace_command_t* command, const char* format, ...);
void trc_add_error(trace_command_t* command, const char* format, ...);
void trc_run_inspection(trace_t* trace);

//The pointers from trc_get_gl_* are invalidated by trc_set_gl_*
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
const trc_gl_obj_rev_t* trc_lookup_gl_obj(trace_t* trace, uint revision, uint64_t fake, trc_gl_obj_type_t type);

uint64_t trc_lookup_current_fake_gl_context(trace_t* trace, uint revision);
uint64_t trc_get_current_fake_gl_context(trace_t* trace);
void trc_set_current_fake_gl_context(trace_t* trace, uint64_t fake);

const trc_gl_context_rev_t* trc_get_gl_context(trace_t* trace, uint64_t fake);
void trc_set_gl_context(trace_t* trace, uint64_t fake, const trc_gl_context_rev_t* rev);
void* trc_get_real_gl_context(trace_t* trace, uint64_t fake);
const trc_gl_context_rev_t* trc_lookup_gl_context(trace_t* trace, uint revision, uint64_t fake);

#define WIP15_STATE_GEN_FUNC_DECL
#include "libtrace_glstate.h"
#undef WIP15_STATE_GEN_FUNC_DECL

trc_data_t* trc_create_data(trace_t* trace, size_t size, const void* data, uint32_t flags);
trc_data_t* trc_create_data_no_copy(trace_t* trace, size_t size, void* data, uint32_t flags);
void* trc_map_data(trc_data_t* data, uint32_t flags);
void trc_unmap_data(trc_data_t* data);
void trc_freeze_data(trace_t* trace, trc_data_t* data);
void trc_unmap_freeze_data(trace_t* trace, trc_data_t* data);
#endif
