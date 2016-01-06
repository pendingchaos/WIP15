#ifndef LIB_INSPECT_H
#define LIB_INSPECT_H
#include "libtrace/libtrace.h"
#include "shared/uint.h"
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
} inspect_gl_state_entry_t;
TYPED_VEC(inspect_gl_state_entry_t, inspect_gl_state)

typedef struct {
    uint32_t width;
    uint32_t height;
    uint8_t* data;
} inspect_image_t;

typedef struct {
    int texture;
    int type;
    int depth_stencil_mode;
    int base_level;
    int compare_func;
    int compare_mode;
    int max_level;
    int wrap[3];
    int min_filter;
    int mag_filter;
    float border_color[4];
    float min_lod;
    float max_lod;
    float lod_bias;
    int swizzle[4];
    int width;
    int height;
    int depth;
    int internal_format;
} inspect_gl_tex_params_t;

typedef struct {
    uint fake;
    inspect_gl_tex_params_t params;
    size_t mipmap_count;
    void** mipmaps;
} inspect_texture_t;
TYPED_VEC(inspect_texture_t, inspect_tex)

typedef struct {
    uint fake;
    uint usage;
    size_t size;
    void* data;
} inspect_buffer_t;
TYPED_VEC(inspect_buffer_t, inspect_buf)

typedef struct {
    uint fake;
    uint type;
    char* source;
    char* info_log;
} inspect_shader_t;
TYPED_VEC(inspect_shader_t, inspect_shdr)

typedef struct {
    uint fake;
    vec_t shaders; //vec_t of unsigned int
    char* info_log;
} inspect_program_t;
TYPED_VEC(inspect_program_t, inspect_prog)

typedef struct {
    unsigned int index;
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
} inspect_vertex_attrib_t;

typedef struct {
    unsigned int fake;
    size_t attrib_count;
    inspect_vertex_attrib_t* attribs;
} inspect_vao_t;
TYPED_VEC(inspect_vao_t, inspect_vao)

typedef struct {
    unsigned int attachment;
    unsigned int tex;
    unsigned int level;
} inspect_fb_attach_t;
TYPED_VEC(inspect_fb_attach_t, inspect_fb_attach)

typedef struct {
    unsigned int fake;
    inspect_fb_attach_vec_t color;
    //depth.attachment, stencil.attachment and depth_stencil.attachment are ignored
    inspect_fb_attach_t depth;
    inspect_fb_attach_t stencil;
    inspect_fb_attach_t depth_stencil;
} inspect_fb_t;
TYPED_VEC(inspect_fb_t, inspect_fb)

typedef struct {
    unsigned int fake;
    size_t width;
    size_t height;
    unsigned int internal_format;
    size_t sample_count;
    size_t red_size;
    size_t green_size;
    size_t blue_size;
    size_t alpha_size;
    size_t depth_size;
    size_t stencil_size;
} inspect_rb_t;
TYPED_VEC(inspect_rb_t, inspect_rb)

typedef struct inspect_sync_t {
    uint64_t fake;
    uint type;
    uint condition;
    uint flags;
} inspect_sync_t;
TYPED_VEC(inspect_sync_t, inspect_sync)

struct inspection_t;
typedef struct {
    struct inspection_t* inspection;
    inspect_tex_vec_t textures;
    inspect_buf_vec_t buffers;
    inspect_shdr_vec_t shaders;
    inspect_prog_vec_t programs;
    inspect_vao_vec_t vaos;
    inspect_fb_vec_t framebuffers;
    inspect_rb_vec_t renderbuffers;
    inspect_sync_vec_t syncs;
    inspect_image_t* back_buf;
    inspect_image_t* front_buf;
    inspect_image_t* depth_buf;
} inspector_t;

struct inspect_action_t;
typedef struct inspect_action_t {
    void (*apply_func)(inspector_t*, struct inspect_action_t*);
    void (*free_func)(struct inspect_action_t*);
    union {
        void* data;
        uint64_t obj;
    };
} inspect_action_t;
TYPED_VEC(inspect_action_t, inspect_act)

typedef struct {
    inspect_gl_state_vec_t entries;
    inspect_image_t back; //data is NULL if it did not change. RGBA
    inspect_image_t front; //data is NULL if it did not change. RGBA
    inspect_image_t depth; //data is NULL if it did not change. uint32_t
    inspect_act_vec_t actions;
} inspect_gl_state_t;

typedef struct inspect_command_t {
    trace_command_t* trace_cmd;
    inspect_attachment_t* attachments;
    uint64_t gl_context;
    char *name;
    inspect_gl_state_t state;
} inspect_command_t;

typedef struct {
    size_t command_count;
    inspect_command_t* commands;
    trace_frame_t* trace_frame;
} inspect_frame_t;

typedef struct inspection_t {
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

//Negative if it could not be found
int inspect_find_tex(inspector_t* inspector, uint tex);
int inspect_find_buf(inspector_t* inspector, uint buf);
int inspect_find_shdr(inspector_t* inspector, uint shdr);
int inspect_find_prog(inspector_t* inspector, uint prog);
int inspect_find_vao(inspector_t* inspector, uint vao);
int inspect_find_fb(inspector_t* inspector, uint fb);
int inspect_find_rb(inspector_t* inspector, uint rb);
int inspect_find_sync(inspector_t* inspector, uint64_t sync);

//NULL if it could not be found
inspect_texture_t* inspect_find_tex_ptr(inspector_t* inspector, uint fake);
inspect_buffer_t* inspect_find_buf_ptr(inspector_t* inspector, uint fake);
inspect_shader_t* inspect_find_shdr_ptr(inspector_t* inspector, uint fake);
inspect_program_t* inspect_find_prog_ptr(inspector_t* inspector, uint fake);
inspect_vao_t* inspect_find_vao_ptr(inspector_t* inspector, uint fake);
inspect_fb_t* inspect_find_fb_ptr(inspector_t* inspector, uint fake);
inspect_rb_t* inspect_find_rb_ptr(inspector_t* inspector, uint fake);
inspect_sync_t* inspect_find_sync_ptr(inspector_t* inspector, uint64_t fake);
#endif
