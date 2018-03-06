#ifndef TRACE_FILE_H
#define TRACE_FILE_H

#include <stddef.h>
#include <pthread.h>

#include "shared/types.h"
#include "shared/replay_config.h"

#define TRC_GL_OBJ_HEAD trc_obj_rev_head_t head;\
uint64_t real;

#define TRC_DATA_NO_ZERO (1<<0)
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

typedef struct trc_attachment_t {
    trc_attachment_type_t type;
    char* message;
    struct trc_attachment_t* next;
} trc_attachment_t;

typedef enum trc_compression_t {
    TrcCompression_None,
    TrcCompression_Zlib,
    TrcCompression_LZ4,
    TrcCompression_Zstd
} trc_compression_t;

typedef enum trc_data_storage_type_t {
    TrcDataStorage_Independent,
    TrcDataStorage_Container,
    TrcDataStorage_Tiny
} trc_data_storage_type_t;

typedef struct trc_indep_storage_t {
    uint64_t compressed_size:60;
    trc_compression_t compression:4;
    uint64_t last_accessed;
    void* data;
} trc_indep_storage_t;

typedef struct trc_data_container_t {
    pthread_rwlock_t lock; //write lock->compress, read lock->don't compress
    struct trc_data_container_t* prev;
    struct trc_data_container_t* next;
    
    uint64_t last_accessed; //in milliseconds
    size_t data_usage;
    
    trc_compression_t compression;
    size_t compressed_size;
    uint8_t* data;
} trc_data_container_t;

typedef struct trc_data_t {
    struct trc_data_t* next;
    uint64_t size:34;
    trc_data_storage_type_t storage:2;
    uint32_t storage_data:28; //container offset for container storage
    //trc_data_container_t* or trc_indep_storage_t is stored after
} trc_data_t;

typedef struct trc_chunked_data_t {
    //chunk_count = chunks->size/sizeof(trc_data_t*)
    //chunk_size = ceil(size/chunk_count)
    size_t size;
    trc_data_t* chunks; //array of trc_data_t*
} trc_chunked_data_t;

typedef struct {
    trace_type_t type;
    uint32_t count;
    bool is_array;
    union {
        uint64_t u64;
        int64_t i64;
        double dbl;
        bool bl;
        char* str;
        uint64_t ptr;
        trc_data_t* data;
        uint64_t* u64_array;
        int64_t* i64_array;
        double* dbl_array;
        bool* bl_array;
        char** str_array;
        uint64_t* ptr_array;
        trc_data_t** data_array;
    };
    int32_t group_index; //Negative if there is no group
} trace_value_t;

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

typedef struct trc_obj_t trc_obj_t;
typedef struct trc_namespace_t trc_namespace_t;

typedef struct trc_obj_rev_head_t {
    trc_obj_t* obj;
    uint64_t revision;
    uint ref_count;
    bool has_name;
    bool has_had_name;
    uint64_t name;
    uint64_t old_name;
    trc_namespace_t* namespace_;
} trc_obj_rev_head_t;

typedef struct trace_t trace_t;

struct trc_obj_t {
    trace_t* trace;
    bool name_table;
    trc_obj_type_t type;
    size_t revision_count;
    void** revisions; //sorted from lowest revision to highest
};

typedef struct trc_obj_ref_t {
    trc_obj_t* obj;
} trc_obj_ref_t;

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
    
    uint data_usage;
    trc_chunked_data_t data;
    
    bool mapped;
    uint64_t map_offset;
    uint64_t map_length;
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
    double border_color[4];
    uint compare_mode;
    uint compare_func;
    float lod_bias;
    float max_anisotropy;
} trc_gl_sample_params_t;

typedef struct trc_gl_sampler_rev_t {
    TRC_GL_OBJ_HEAD
    //Although a sampler name can have no object, one is created when it is used
    //so there is no need to keep track of whether a revision has an object
    //bool has_object;
    trc_gl_sample_params_t params;
} trc_gl_sampler_rev_t;

typedef enum trc_image_format_t {
    TrcImageFormat_Red_U32,
    TrcImageFormat_RedGreen_U32,
    TrcImageFormat_RGB_U32,
    TrcImageFormat_RGBA_U32,
    TrcImageFormat_Red_I32,
    TrcImageFormat_RedGreen_I32,
    TrcImageFormat_RGB_I32,
    TrcImageFormat_RGBA_I32,
    TrcImageFormat_Red_F32,
    TrcImageFormat_RedGreen_F32,
    TrcImageFormat_RGB_F32,
    TrcImageFormat_RGBA_F32,
    TrcImageFormat_SRGB_U8,
    TrcImageFormat_SRGBA_U8,
    TrcImageFormat_F32_U24_U8,
    TrcImageFormat_RGBA_U8
} trc_image_format_t;

typedef struct trc_gl_texture_buffer_t {
    uint internal_format;
    uint buffer; //TODO: trc_obj_ref_t?
    size_t offset;
    size_t size;
} trc_gl_texture_buffer_t;

typedef struct trc_gl_texture_image_t {
    uint face; //0 if the texture is not a non-array cubemap
    uint level;
    uint internal_format;
    bool compressed_internal_format;
    uint width;
    uint height;
    uint depth;
    trc_image_format_t data_format;
    trc_chunked_data_t data;
} trc_gl_texture_image_t;

typedef struct trc_gl_texture_rev_t {
    TRC_GL_OBJ_HEAD
    bool has_object;
    uint type;
    trc_gl_sample_params_t sample_params;
    uint depth_stencil_mode;
    uint base_level;
    uint max_level;
    uint swizzle[4];
    bool fixed_sample_locations;
    uint samples;
    bool immutable;
    //array of trc_gl_texture_image_t
    trc_data_t* images;
    trc_gl_texture_buffer_t buffer;
    int complete_status; //-1=unknown, 0=incomplete, 1=complete
    const char* incompleteness_reason;
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
    trc_obj_ref_t renderbuffer;
    //when has_renderbuffer == false
    trc_obj_ref_t texture;
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
    bool multisample;
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

typedef enum trc_gl_uniform_base_dtype_t {
    TrcUniformBaseType_Float = 0,
    TrcUniformBaseType_Double = 1,
    TrcUniformBaseType_Uint = 2,
    TrcUniformBaseType_Int = 3,
    TrcUniformBaseType_Uint64 = 4,
    TrcUniformBaseType_Int64 = 5,
    TrcUniformBaseType_Bool = 6,
    TrcUniformBaseType_Sampler = 7,
    TrcUniformBaseType_Image = 8,
    TrcUniformBaseType_AtomicCounter = 9,
    TrcUniformBaseType_Struct = 10,
    TrcUniformBaseType_Array = 11
} trc_gl_uniform_base_dtype_t;

typedef enum trc_gl_uniform_tex_type_t {
    TrcUniformTexType_1D = 0,
    TrcUniformTexType_2D = 1,
    TrcUniformTexType_3D = 2,
    TrcUniformTexType_Cube = 3,
    TrcUniformTexType_Rect = 4,
    TrcUniformTexType_Buffer = 5
} trc_gl_uniform_tex_type_t;

typedef struct trc_gl_uniform_dtype_t {
    trc_gl_uniform_base_dtype_t base;
    uint dim[2];
    
    trc_gl_uniform_tex_type_t tex_type;
    bool tex_shadow;
    bool tex_array;
    bool tex_multisample;
    trc_gl_uniform_base_dtype_t tex_dtype; //Float, Uint or Int
} trc_gl_uniform_dtype_t;

typedef struct trc_gl_uniform_t {
    trc_data_t* name; //null terminated - empty string in the case of array elements
    trc_gl_uniform_dtype_t dtype;
    uint parent; //can be 0xffffffff for root uniforms
    uint next; //next member or array element, or 0xffffffff if none
    union {
    struct {
        int real_loc;
        uint fake_loc;
        uint data_offset;
    };
    //For Array(s), all element types must be the same
    //Unordered in the case of Struct(s)
    uint first_child;
    };
} trc_gl_uniform_t;

typedef struct trc_gl_program_uniform_block_t {
    trc_data_t* name; //null terminated
    uint real; //real index
    uint fake; //fake index
    uint binding;
} trc_gl_program_uniform_block_t;

typedef struct trc_gl_program_vertex_attrib_t {
    trc_data_t* name;
    uint type;
    uint locations_used; //the number of columns for matrices, otherwise 1
    uint index;
} trc_gl_program_vertex_attrib_t;

typedef struct trc_gl_program_rev_t {
    TRC_GL_OBJ_HEAD
    size_t root_uniform_count;
    //may be larger than root_uniform_count*sizeof(trc_gl_uniform_t)
    trc_data_t* uniforms; //array of trc_gl_uniform_t
    trc_data_t* uniform_data;
    trc_data_t* vertex_attribs; //array of trc_gl_program_vertex_attrib_t
    trc_data_t* uniform_blocks; //array of trc_gl_program_uniform_block_t
    
    //index 0=vertex, 1=tess control, 2=tess eval, 3=geometry, 4=fragment, 5=compute
    trc_data_t* subroutines[6]; //{int[]{real0, fake0, real1, fake1, ...}, ...}
    trc_data_t* subroutine_uniforms[6]; //{int[]{real0, fake0, real1, fake1, ...}, ...}
    
    trc_data_t* shaders; //array of trc_obj_ref_t
    trc_data_t* linked; //array of trc_obj_t*
    int64_t link_revision;
    trc_data_t* info_log; //null-terminated ascii
    int binary_retrievable_hint; //-1=unset, 0=false, 1=true
    bool separable;
    
    int link_status; //-1=notdone, 0=failed, 1=succeeded
    int validation_status; //-1=notdone, 0=failed, 1=succeeded
} trc_gl_program_rev_t;

typedef struct trc_gl_program_pipeline_rev_t {
    TRC_GL_OBJ_HEAD
    bool has_object;
    trc_obj_ref_t active_program;
    trc_obj_ref_t vertex_program;
    trc_obj_ref_t fragment_program;
    trc_obj_ref_t geometry_program;
    trc_obj_ref_t tess_control_program;
    trc_obj_ref_t tess_eval_program;
    trc_obj_ref_t compute_program;
    bool validation_status; //-1=notdone, 0=failed, 1=succeeded
} trc_gl_program_pipeline_rev_t;

typedef struct trc_gl_shader_rev_t {
    TRC_GL_OBJ_HEAD
    uint type;
    trc_data_t* sources; //concatenated null-terminated ascii
    trc_data_t* info_log; //null-terminated ascii
} trc_gl_shader_rev_t;

typedef struct trc_gl_vao_buffer_t {
    uint64_t offset;
    uint stride;
    uint divisor;
    trc_obj_ref_t buffer;
} trc_gl_vao_buffer_t;

typedef struct trc_gl_vao_attrib_t {
    bool enabled;
    bool normalized;
    bool integer;
    //TODO: bool doubles;
    uint size; //1, 2, 3, 4 or GL_BGRA
    uint64_t offset;
    uint type;
    uint buffer_index;
} trc_gl_vao_attrib_t;

typedef struct trc_gl_vao_rev_t {
    TRC_GL_OBJ_HEAD
    bool has_object;
    trc_data_t* attribs; //array of trc_gl_vao_attrib_t
    trc_data_t* buffers; //array of trc_gl_vao_buffer_t
    trc_obj_ref_t element_buffer;
} trc_gl_vao_rev_t;

typedef struct trc_gl_transform_feedback_rev_t {
    TRC_GL_OBJ_HEAD
    bool has_object;
    trc_data_t* bindings; //array of trc_gl_buffer_binding_point_t
    bool active;
    bool paused;
    bool active_not_paused;
} trc_gl_transform_feedback_rev_t;

struct trc_namespace_t {
    trace_t* trace;
    trc_obj_t* name_tables[Trc_ObjMax];
};

#define WIP15_STATE_GEN_DECL
#include "libtrace_glstate.h"
#undef WIP15_STATE_GEN_DECL

typedef struct trc_cur_context_rev_t {
    uint64_t revision;
    trc_obj_ref_t context;
} trc_cur_context_rev_t;

typedef struct trc_gl_inspection_t {
    uint64_t cur_revision;
    
    size_t frame_index;
    size_t cmd_index;
    
    size_t object_count[Trc_ObjMax];
    trc_obj_t** objects[Trc_ObjMax];
    trc_namespace_t global_namespace; //Should only have names to contexts
    size_t namespace_count;
    trc_namespace_t** namespaces; //Should only have names for objects other than contexts
    
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
    
    uint64_t revision; //Revision after the command
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
    void* data_state;
};

typedef struct trc_replay_test_failure_t {
    char error_message[1024];
    struct trc_replay_test_failure_t* next;
} trc_replay_test_failure_t;

typedef struct trc_replay_test_t {
    char name[1024];
    size_t successes;
    trc_replay_test_failure_t* failures;
    struct trc_replay_test_t* next;
} trc_replay_test_t;

typedef struct trc_replay_context_t {
    void* _replay_gl;
    trace_t* trace;
    trace_command_t* current_command;
    struct SDL_Window* window;
    trc_replay_test_t* tests;
    trc_replay_test_t* current_test;
    trc_namespace_t* ns; //current context's namespace
    trc_namespace_t* priv_ns; //current context's private namespace (for container objects)
    size_t target_option_count;
    const char*const* target_option_names;
    const char*const* target_option_values;
} trc_replay_context_t;

typedef enum trc_trace_program_arg_t {
    TrcProgramArguments, //const char**
    TrcOutputFilename, //const char*
    TrcConfigFilename, //const char*
    TrcCompression, //int, 0-100, default 60
    TrcLibGL, //const char*
    TrcCurrentWorkingDirectory //const char*
} trc_trace_program_arg_t;

#pragma GCC visibility push(default)
bool trc_compression_available(trc_compression_t compression);

//Traces
bool trace_program(int* exitcode, size_t count, ...);
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
const trc_data_t*const* trc_get_data(const trace_value_t* val);
trace_extra_t* trc_get_extra(trace_command_t* cmd, const char* name);
trace_extra_t* trc_get_extrai(trace_command_t* cmd, const char* name, size_t index);
void trc_add_info(trace_command_t* command, const char* format, ...);
void trc_add_warning(trace_command_t* command, const char* format, ...);
void trc_add_error(trace_command_t* command, const char* format, ...);
void trc_run_inspection(trace_t* trace);

//Objects
trc_obj_t* trc_create_obj(trace_t* trace, bool name_table, trc_obj_type_t type, const void* rev);

const void* trc_obj_get_rev(trc_obj_t* obj, uint64_t rev);
void trc_obj_set_rev(trc_obj_t* obj, const void* rev);
void trc_grab_obj(trc_obj_t* obj);
void trc_drop_obj(trc_obj_t* obj);

trc_namespace_t* trc_create_namespace(trace_t* trace);
bool trc_set_name(trc_namespace_t* ns, trc_obj_type_t type, uint64_t name, trc_obj_t* obj);
void trc_free_name(trc_namespace_t* ns, trc_obj_type_t type, uint64_t name);
trc_obj_t* trc_lookup_name(trc_namespace_t* ns, trc_obj_type_t type, uint64_t name, uint64_t rev);

trc_obj_t* trc_create_named_obj(trc_namespace_t* ns, trc_obj_type_t type, uint64_t name, const void* rev);
void trc_set_obj(trc_namespace_t* ns, trc_obj_type_t type, uint64_t name, const void* rev);
const void* trc_get_obj(trc_namespace_t* ns, trc_obj_type_t type, uint64_t name);

const trc_gl_context_rev_t* trc_get_context(trace_t* trace);
void trc_set_context(trace_t* trace, trc_gl_context_rev_t* rev);

bool trc_iter_objects(trace_t* trace, trc_obj_type_t type, size_t* index, uint64_t revision, const void** rev);

void trc_del_obj_ref(trc_obj_ref_t ref);
void trc_set_obj_ref(trc_obj_ref_t* ref, trc_obj_t* obj);

trc_obj_t* trc_get_current_gl_context(trace_t* trace, uint64_t revision);
void trc_set_current_gl_context(trace_t* trace, trc_obj_t* obj);

//Data
typedef struct trc_data_settings_t {
    int thread_count; //0 = auto heavy, -1 = auto light
    
    bool zlib_enabled;
    bool zstd_enabled;
    bool lz4_enabled;
    
    int zlib_level;
    int lz4_acceleration;
    int zstd_level;
} trc_data_settings_t;

typedef struct trc_compressed_data_t {
    trc_compression_t compression;
    size_t size;
    size_t compressed_size;
    void* compressed_data;
} trc_compressed_data_t;

bool trc_data_init(trc_data_settings_t* settings_);
trc_data_t* trc_create_data(trace_t* trace, size_t size, const void* data, uint32_t flags);
trc_data_t* trc_create_data_no_copy(trace_t* trace, size_t size, void* data, uint32_t flags);
trc_data_t* trc_create_compressed_data_no_copy(trace_t* trace, trc_compressed_data_t data);
trc_data_t* trc_copy_data(trace_t* trace, trc_data_t* src);
//void* trc_map_data(trc_data_t* data, uint32_t flags);
void* _trc_map_data(trc_data_t* data, uint32_t flags, int line, const char* file, const char* func);
#define trc_map_data(data, flags) _trc_map_data(data, flags, __LINE__, __FILE__, __func__)
void trc_unmap_data(const void* mapped_ptr);

//Chunked data
typedef struct trc_chunked_data_mod_t {
    struct trc_chunked_data_mod_t* next;
    size_t start;
    size_t size;
    const uint8_t* data;
} trc_chunked_data_mod_t;

typedef struct trc_modify_chunked_data_t {
    trc_chunked_data_t base;
    trc_chunked_data_mod_t* mods;
} trc_modify_chunked_data_t;

typedef struct trc_read_chunked_data_t {
    trc_chunked_data_t data;
    size_t start;
    size_t size;
    uint8_t* dest;
} trc_read_chunked_data_t;

trc_chunked_data_t trc_create_chunked_data(trace_t* trace, size_t size, const void* data);
trc_chunked_data_t trc_modify_chunked_data(trace_t* trace, trc_modify_chunked_data_t info);
void trc_read_chunked_data(trc_read_chunked_data_t info);
#pragma GCC visibility pop
#endif
