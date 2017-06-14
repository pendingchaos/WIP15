#ifndef TRACE_FILE_H
#define TRACE_FILE_H

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include <pthread.h>

#include "shared/uint.h"

#define TRC_GL_OBJ_HEAD trc_obj_rev_head_t head;\
uint64_t fake_context;\
uint64_t real;\
uint64_t fake;\

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

typedef enum trc_compression_t {
    TrcCompression_None,
    TrcCompression_Zlib,
    TrcCompression_LZ4
} trc_compression_t;

typedef enum trc_obj_type_t {
    TrcBuffer,
    TrcSampler,
    TrcTexture,
    TrcQuery,
    TrcFramebuffer,
    TrcRenderbuffer,
    TrcSync,
    TrcProgram,
    TrcProgramPipeline,
    TrcShader,
    TrcVAO,
    TrcTransformFeedback,
    TrcContext,
    Trc_ObjMax
} trc_obj_type_t;

typedef struct trc_data_external_storage_t {
    trc_compression_t compression:32;
    uint32_t size;
    void* data;
} trc_data_external_storage_t;

typedef struct trc_data_t {
    pthread_rwlock_t lock;
    uint8_t flags;
    trc_storage_type_t storage_type:16;
    uint32_t size;
    union {
        void* plain;
        trc_data_external_storage_t* external; //only for immutable data
    };
    struct trc_data_t* queue_next; //this is not guarded by trc_data_t::mutex
} trc_data_t;

typedef struct trc_obj_t trc_obj_t;

typedef struct trc_obj_rev_head_t {
    trc_obj_t* obj;
    uint64_t revision;
    uint ref_count;
} trc_obj_rev_head_t;

typedef struct trace_t trace_t;

struct trc_obj_t {
    trace_t* trace;
    bool name_table;
    trc_obj_type_t type;
    size_t revision_count;
    void** revisions; //sorted from lowest revision to highest
};

typedef struct trc_name_table_rev_t {
    trc_obj_rev_head_t head;
    size_t entry_count;
    trc_data_t* names; //array of uint64_t
    trc_data_t* objects; //array of trc_obj_t*
} trc_name_table_rev_t;

typedef struct trc_gl_buffer_rev_t {
    TRC_GL_OBJ_HEAD
    bool has_object;
    
    uint tf_binding_count;
    
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
    bool has_object;
} trc_gl_sampler_rev_t;

typedef struct trc_gl_texture_image_t {
    uint face; //0 if the texture is not a non-array cubemap
    uint level;
    uint internal_format;
    
    //for buffer textures
    uint buffer;
    uint buffer_start;
    uint buffer_size;
    
    //for image textures
    uint width;
    uint height;
    uint depth;
    trc_data_t* data; //array of uint32_t, int32_t, float or float+uint32_t depending on the internal format
} trc_gl_texture_image_t;

typedef struct trc_gl_texture_rev_t {
    TRC_GL_OBJ_HEAD
    bool has_object;
    uint type;
    trc_gl_sample_params_t sample_params;
    uint depth_stencil_mode;
    uint base_level;
    uint max_level;
    float lod_bias;
    uint swizzle[4];
    //array of trc_gl_texture_image_t
    trc_data_t* images;
} trc_gl_texture_rev_t;

typedef struct trc_gl_query_rev_t {
    TRC_GL_OBJ_HEAD
    bool has_object;
    int active_index;
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
    bool has_object;
    trc_data_t* attachments; //array of trc_gl_framebuffer_attachment_t
    trc_data_t* draw_buffers; //array of GLenum
} trc_gl_framebuffer_rev_t;

typedef struct trc_gl_renderbuffer_rev_t {
    TRC_GL_OBJ_HEAD
    bool has_object;
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
    
    //index 0=vertex, 1=tess control, 2=tess eval, 3=geometry, 4=fragment, 5=compute
    trc_data_t* subroutines[6]; //{int[]{real0, fake0, real1, fake1, ...}, ...}
    trc_data_t* subroutine_uniforms[6]; //{int[]{real0, fake0, real1, fake1, ...}, ...}
    
    trc_data_t* shaders; //array of trc_gl_program_shader_t
    trc_data_t* info_log; //null-terminated ascii
    int binary_retrievable_hint; //-1=unset, 0=false, 1=true
    bool separable;
} trc_gl_program_rev_t;

typedef struct trc_gl_program_pipeline_rev_t {
    TRC_GL_OBJ_HEAD
    bool has_object;
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
    bool has_object;
    trc_data_t* attribs; //array of trc_gl_vao_attrib_t
} trc_gl_vao_rev_t;

typedef struct trc_gl_transform_feedback_rev_t {
    TRC_GL_OBJ_HEAD
    bool has_object;
} trc_gl_transform_feedback_rev_t;

#define WIP15_STATE_GEN_DECL
#include "libtrace_glstate.h"
#undef WIP15_STATE_GEN_DECL

typedef struct trc_cur_context_rev_t {
    uint64_t revision;
    uint64_t context;
} trc_cur_context_rev_t;

typedef struct trc_gl_inspection_t {
    uint64_t cur_revision;
    
    size_t data_count;
    trc_data_t** data;
    
    size_t object_count[Trc_ObjMax];
    trc_obj_t** objects[Trc_ObjMax];
    trc_obj_t* name_tables[Trc_ObjMax];
    
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
    uint32_t arg_count;
    trace_value_t* args;
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

struct trace_t {
    bool little_endian;
    
    uint32_t func_name_count;
    char** func_names;
    
    uint32_t group_name_count;
    char** group_names;
    
    size_t frame_count;
    trace_frame_t* frames;
    
    trc_gl_inspection_t inspection;
    
    pthread_mutex_t data_queue_mutex;
    trc_data_t* data_queue_start;
    trc_data_t* data_queue_end;
    
    bool threads_running;
    size_t thread_count;
    pthread_t* threads;
};

typedef struct trc_replay_context_t {
    void* _replay_gl;
    trace_t* trace;
    struct SDL_Window* window;
    const char* current_test_name;
} trc_replay_context_t;

trace_t* load_trace(const char* filename);
void free_trace(trace_t* trace);
void trc_free_value(trace_value_t value);
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

//Object
trc_obj_t* trc_create_obj(trace_t* trace, bool name_table, trc_obj_type_t type, const void* rev);

void* trc_obj_get_rev(trc_obj_t* obj, uint64_t rev);
void trc_obj_set_rev(trc_obj_t* obj, const void* rev);
void trc_grab_obj(trc_obj_t* obj);
void trc_drop_obj(trc_obj_t* obj);

void trc_set_name(trace_t* trace, trc_obj_type_t type, uint64_t name, trc_obj_t* obj);
void trc_unset_name(trace_t* trace, trc_obj_type_t type, uint64_t name);
trc_obj_t* trc_lookup_name(trace_t* trace, trc_obj_type_t type, uint64_t name, uint64_t rev);

void trc_set_obj(trace_t* trace, trc_obj_type_t type, uint64_t name, void* rev);
void* trc_get_obj(trace_t* trace, trc_obj_type_t type, uint64_t name);

#define TRC_ITER_OBJECTS_BEGIN(type, revt) for (size_t i = 0; i < trace->inspection.object_count[type]; i++) {\
    trc_obj_t* obj = trace->inspection.objects[type][i];\
    const revt* rev = trc_obj_get_rev(obj, revision);\
    if (!rev || rev->head.ref_count==0) continue;
#define TRC_ITER_OBJECTS_END }

//Helpers
static inline const trc_gl_buffer_rev_t* trc_get_gl_buffer(trace_t* trace, uint fake) {
    return trc_get_obj(trace, TrcBuffer, fake);
}

static inline void trc_set_gl_buffer(trace_t* trace, uint fake, const trc_gl_buffer_rev_t* rev) {
    trc_obj_t* obj = trc_lookup_name(trace, TrcBuffer, fake, -1);
    if (!obj) trc_set_name(trace, TrcBuffer, fake, trc_create_obj(trace, false, TrcBuffer, rev));
    else trc_obj_set_rev(obj, rev);
}

static inline uint trc_get_real_gl_buffer(trace_t* trace, uint fake) {
    return trc_get_gl_buffer(trace, fake)->real;
}

static inline const trc_gl_sampler_rev_t* trc_get_gl_sampler(trace_t* trace, uint fake) {
    return trc_get_obj(trace, TrcSampler, fake);
}

static inline void trc_set_gl_sampler(trace_t* trace, uint fake, const trc_gl_sampler_rev_t* rev) {
    trc_obj_t* obj = trc_lookup_name(trace, TrcSampler, fake, -1);
    if (!obj) trc_set_name(trace, TrcSampler, fake, trc_create_obj(trace, false, TrcSampler, rev));
    else trc_obj_set_rev(obj, rev);
}

static inline uint trc_get_real_gl_sampler(trace_t* trace, uint fake) {
    return trc_get_gl_sampler(trace, fake)->real;
}

static inline const trc_gl_texture_rev_t* trc_get_gl_texture(trace_t* trace, uint fake) {
    return trc_get_obj(trace, TrcTexture, fake);
}

static inline void trc_set_gl_texture(trace_t* trace, uint fake, const trc_gl_texture_rev_t* rev) {
    trc_obj_t* obj = trc_lookup_name(trace, TrcTexture, fake, -1);
    if (!obj) trc_set_name(trace, TrcTexture, fake, trc_create_obj(trace, false, TrcTexture, rev));
    else trc_obj_set_rev(obj, rev);
}

static inline uint trc_get_real_gl_texture(trace_t* trace, uint fake) {
    return trc_get_gl_texture(trace, fake)->real;
}

static inline const trc_gl_query_rev_t* trc_get_gl_query(trace_t* trace, uint fake) {
    return trc_get_obj(trace, TrcQuery, fake);
}

static inline void trc_set_gl_query(trace_t* trace, uint fake, const trc_gl_query_rev_t* rev) {
    trc_obj_t* obj = trc_lookup_name(trace, TrcQuery, fake, -1);
    if (!obj) trc_set_name(trace, TrcQuery, fake, trc_create_obj(trace, false, TrcQuery, rev));
    else trc_obj_set_rev(obj, rev);
}

static inline uint trc_get_real_gl_query(trace_t* trace, uint fake) {
    return trc_get_gl_query(trace, fake)->real;
}

static inline const trc_gl_framebuffer_rev_t* trc_get_gl_framebuffer(trace_t* trace, uint fake) {
    return trc_get_obj(trace, TrcFramebuffer, fake);
}

static inline void trc_set_gl_framebuffer(trace_t* trace, uint fake, const trc_gl_framebuffer_rev_t* rev) {
    trc_obj_t* obj = trc_lookup_name(trace, TrcFramebuffer, fake, -1);
    if (!obj) trc_set_name(trace, TrcFramebuffer, fake, trc_create_obj(trace, false, TrcFramebuffer, rev));
    else trc_obj_set_rev(obj, rev);
}

static inline uint trc_get_real_gl_framebuffer(trace_t* trace, uint fake) {
    return trc_get_gl_framebuffer(trace, fake)->real;
}

static inline const trc_gl_renderbuffer_rev_t* trc_get_gl_renderbuffer(trace_t* trace, uint fake) {
    return trc_get_obj(trace, TrcRenderbuffer, fake);
}

static inline void trc_set_gl_renderbuffer(trace_t* trace, uint fake, const trc_gl_renderbuffer_rev_t* rev) {
    trc_obj_t* obj = trc_lookup_name(trace, TrcRenderbuffer, fake, -1);
    if (!obj) trc_set_name(trace, TrcRenderbuffer, fake, trc_create_obj(trace, false, TrcRenderbuffer, rev));
    else trc_obj_set_rev(obj, rev);
}

static inline uint trc_get_real_gl_renderbuffer(trace_t* trace, uint fake) {
    return trc_get_gl_renderbuffer(trace, fake)->real;
}

static inline const trc_gl_sync_rev_t* trc_get_gl_sync(trace_t* trace, uint64_t fake) {
    return trc_get_obj(trace, TrcSync, fake);
}

static inline void trc_set_gl_sync(trace_t* trace, uint64_t fake, const trc_gl_sync_rev_t* rev) {
    trc_obj_t* obj = trc_lookup_name(trace, TrcSync, fake, -1);
    if (!obj) trc_set_name(trace, TrcSync, fake, trc_create_obj(trace, false, TrcSync, rev));
    else trc_obj_set_rev(obj, rev);
}

static inline uint64_t trc_get_real_gl_sync(trace_t* trace, uint64_t fake) {
    return trc_get_gl_sync(trace, fake)->real;
}

static inline const trc_gl_program_rev_t* trc_get_gl_program(trace_t* trace, uint fake) {
    return trc_get_obj(trace, TrcProgram, fake);
}

static inline void trc_set_gl_program(trace_t* trace, uint fake, const trc_gl_program_rev_t* rev) {
    trc_obj_t* obj = trc_lookup_name(trace, TrcProgram, fake, -1);
    if (!obj) trc_set_name(trace, TrcProgram, fake, trc_create_obj(trace, false, TrcProgram, rev));
    else trc_obj_set_rev(obj, rev);
}

static inline uint trc_get_real_gl_program(trace_t* trace, uint fake) {
    return trc_get_gl_program(trace, fake)->real;
}

static inline const trc_gl_program_pipeline_rev_t* trc_get_gl_program_pipeline(trace_t* trace, uint fake) {
    return trc_get_obj(trace, TrcProgramPipeline, fake);
}

static inline void trc_set_gl_program_pipeline(trace_t* trace, uint fake, const trc_gl_program_pipeline_rev_t* rev) {
    trc_obj_t* obj = trc_lookup_name(trace, TrcProgramPipeline, fake, -1);
    if (!obj) trc_set_name(trace, TrcProgramPipeline, fake, trc_create_obj(trace, false, TrcProgramPipeline, rev));
    else trc_obj_set_rev(obj, rev);
}

static inline uint trc_get_real_gl_program_pipeline(trace_t* trace, uint fake) {
    return trc_get_gl_program_pipeline(trace, fake)->real;
}

static inline const trc_gl_shader_rev_t* trc_get_gl_shader(trace_t* trace, uint fake) {
    return trc_get_obj(trace, TrcShader, fake);
}

static inline void trc_set_gl_shader(trace_t* trace, uint fake, const trc_gl_shader_rev_t* rev) {
    trc_obj_t* obj = trc_lookup_name(trace, TrcShader, fake, -1);
    if (!obj) trc_set_name(trace, TrcShader, fake, trc_create_obj(trace, false, TrcShader, rev));
    else trc_obj_set_rev(obj, rev);
}

static inline uint trc_get_real_gl_shader(trace_t* trace, uint fake) {
    return trc_get_gl_shader(trace, fake)->real;
}

static inline const trc_gl_vao_rev_t* trc_get_gl_vao(trace_t* trace, uint fake) {
    return trc_get_obj(trace, TrcVAO, fake);
}

static inline void trc_set_gl_vao(trace_t* trace, uint fake, const trc_gl_vao_rev_t* rev) {
    trc_obj_t* obj = trc_lookup_name(trace, TrcVAO, fake, -1);
    if (!obj) trc_set_name(trace, TrcVAO, fake, trc_create_obj(trace, false, TrcVAO, rev));
    else trc_obj_set_rev(obj, rev);
}

static inline uint trc_get_real_gl_vao(trace_t* trace, uint fake) {
    return trc_get_gl_vao(trace, fake)->real;
}

static inline const trc_gl_transform_feedback_rev_t* trc_get_gl_transform_feedback(trace_t* trace, uint fake) {
    return trc_get_obj(trace, TrcTransformFeedback, fake);
}

static inline void trc_set_gl_transform_feedback(trace_t* trace, uint fake, const trc_gl_transform_feedback_rev_t* rev) {
    trc_obj_t* obj = trc_lookup_name(trace, TrcTransformFeedback, fake, -1);
    if (!obj) trc_set_name(trace, TrcTransformFeedback, fake, trc_create_obj(trace, false, TrcTransformFeedback, rev));
    else trc_obj_set_rev(obj, rev);
}

static inline uint trc_get_real_gl_transform_feedback(trace_t* trace, uint fake) {
    return trc_get_gl_transform_feedback(trace, fake)->real;
}

static inline void trc_grab_gl_obj(trace_t* trace, uint64_t fake, trc_obj_type_t type) {
    if (!fake) return;
    trc_grab_obj(trc_lookup_name(trace, type, fake, -1));
}

static inline void trc_rel_gl_obj(trace_t* trace, uint64_t fake, trc_obj_type_t type) {
    if (!fake) return;
    trc_drop_obj(trc_lookup_name(trace, type, fake, -1));
    if (((trc_obj_rev_head_t*)trc_get_obj(trace, type, fake))->ref_count == 0)
        trc_unset_name(trace, type, fake);
}

static inline const void* trc_lookup_gl_obj(trace_t* trace, uint64_t revision, uint64_t fake, trc_obj_type_t type) {
    return trc_lookup_name(trace, type, fake, revision);
}

uint64_t trc_lookup_current_fake_gl_context(trace_t* trace, uint64_t revision);
uint64_t trc_get_current_fake_gl_context(trace_t* trace);
void trc_set_current_fake_gl_context(trace_t* trace, uint64_t fake);

static inline const trc_gl_context_rev_t* trc_get_gl_context(trace_t* trace, uint64_t fake) {
    if (!fake) fake = trc_get_current_fake_gl_context(trace);
    return trc_get_obj(trace, TrcContext, fake);
}

static inline void trc_set_gl_context(trace_t* trace, uint64_t fake, const trc_gl_context_rev_t* rev) {
    if (!fake) fake = trc_get_current_fake_gl_context(trace);
    trc_obj_t* obj = trc_lookup_name(trace, TrcContext, fake, -1);
    if (!obj) trc_set_name(trace, TrcContext, fake, trc_create_obj(trace, false, TrcContext, rev));
    else trc_obj_set_rev(obj, rev);
}

static inline void* trc_get_real_gl_context(trace_t* trace, uint64_t fake) {
    if (!fake) fake = trc_get_current_fake_gl_context(trace);
    return trc_get_gl_context(trace, fake)->real;
}

static inline const trc_gl_context_rev_t* trc_lookup_gl_context(trace_t* trace, uint64_t revision, uint64_t fake) {
    trc_obj_t* obj = trc_lookup_name(trace, TrcContext, fake, revision);
    if (!obj) return NULL;
    return trc_obj_get_rev(obj, revision);
}

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
