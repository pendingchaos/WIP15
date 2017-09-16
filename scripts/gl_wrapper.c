#ifdef ZLIB_ENABLED
#include <zlib.h>
#endif
#ifdef LZ4_ENABLED
#include <lz4.h>
#endif
#ifdef ZSTD_ENABLED
#include <zstd.h>
#endif

#include "shared/replay_config.h"

#if __WORDSIZE == 64 //TODO: Why is this needed?
#elif __WORDSIZE == 32
#else
#error "Unsupported word size."
#endif

#define OP_DECL_FUNC 0
#define OP_DECL_GROUP 1
#define OP_CALL 2

#define BASE_VOID 0
#define BASE_UNSIGNED_INT 1
#define BASE_INT 2
#define BASE_PTR 3
#define BASE_BOOL 4
#define BASE_FLOAT 5
#define BASE_DOUBLE 6
#define BASE_STRING 7
#define BASE_DATA 8
#define BASE_FUNC_PTR 9

#define COMPRESSION_AUTO -1
#define COMPRESSION_NONE 0
#define COMPRESSION_ZLIB 1
#define COMPRESSION_LZ4 2
#define COMPRESSION_ZSTD 3

typedef enum opengl_version_t {
    OpenGLVersion_32,
    OpenGLVersion_33,
    OpenGLVersion_40,
    OpenGLVersion_41,
    OpenGLVersion_42,
    OpenGLVersion_43,
    OpenGLVersion_44,
    OpenGLVersion_45,
    OpenGLVersion_46,
    OpenGLVersion_Count
} opengl_version_t;

typedef struct config_entry_t {
    struct config_entry_t* next;
    char name[64];
    int value;
} config_entry_t;

typedef struct config_t {
    config_entry_t* entries;
} config_t;

static FILE *trace_file = NULL;
static void *lib_gl = NULL;
static config_t configs[OpenGLVersion_Count];
static trc_replay_config_t current_config; //TODO: Make this thread local
static GLsizei drawable_width = -1;
static GLsizei drawable_height = -1;
static unsigned int compression_level = 0; //0-100
static int compression_method = COMPRESSION_AUTO;

static void gl_write_b(uint8_t v) {
    fwrite(&v, 1, 1, trace_file);
}

static void gl_write_uint32(uint32_t i) {
    i = htole32(i);
    fwrite(&i, 4, 1, trace_file);
}

static void gl_write_bool(uint64_t b) {
    gl_write_b(b ? 1 : 0);
}

static void gl_write_float(float f) {
    fwrite(&f, 4, 1, trace_file);
}

static void gl_write_double(double d) {
    fwrite(&d, 8, 1, trace_file);
}

static void gl_write_uleb128(uint64_t v) {
    do {
        uint8_t b = v & 0x7f;
        v >>= 7;
        if (v) b |= 0x80;
        fwrite(&b, 1, 1, trace_file);
    } while (v);
}

static void gl_write_sleb128(int64_t v) {
    if (v<0) gl_write_uleb128(1 | (uint64_t)(-v)<<1);
    else gl_write_uleb128((uint64_t)v << 1);
}

static void gl_write_ptr(const void* v) {
    gl_write_uleb128((size_t)v);
}

static void gl_write_str(const char* s) {
    uint32_t length = s == NULL ? 0 : strlen(s);
    gl_write_uint32(length);
    fwrite(s, length, 1, trace_file);
}

static void gl_write_type(uint8_t base, bool has_group, bool is_array) {
    gl_write_b(base);
    gl_write_bool(has_group);
    gl_write_bool(is_array);
}

static void gl_write_data(ptrdiff_t size, const void* data) {
    if (size < 0) size = 0;
    
    int method = compression_method;
    if (method == COMPRESSION_AUTO) {
    #if defined(ZSTD_ENABLED) && defined(ZLIB_ENABLED)
        method = size > 2*1024*1024 ? COMPRESSION_ZSTD : COMPRESSION_ZLIB;
    #elif defined(ZLIB_ENABLED)
        method = COMPRESSION_ZLIB;
    #elif defined(LZ4_ENABLED)
        method = COMPRESSION_LZ4;
    #else
        method = COMPRESSION_NONE;
    #endif
    }
    
    #ifdef ZSTD_ENABLED
    if (method == COMPRESSION_ZSTD) {
        unsigned int level = (float)compression_level / 100.0 * 19.0; //don't use ultra levels (20+)
        if (level == 0) goto none;
        void* compressed = malloc(size);
        size_t written = ZSTD_compress(compressed, size, data, size, level);
        if (ZSTD_isError(written) || written>size) {
            free(compressed);
            goto none;
        } else {
            gl_write_b(COMPRESSION_ZSTD);
            gl_write_uint32(size);
            gl_write_uint32(written);
            fwrite(compressed, written, 1, trace_file);
            free(compressed);
            return;
        }
    }
    #endif
    #ifdef ZLIB_ENABLED
    if (method == COMPRESSION_ZLIB) {
        unsigned int level = (float)compression_level / 100.0 * 9.0;
        if (level == 0) goto none;
        void* compressed = malloc(size);
        uLongf compressed_size = size;
        if (compress2(compressed, &compressed_size, data, size, level) != Z_OK) {
            free(compressed);
            goto none;
        } else {
            gl_write_b(COMPRESSION_ZLIB);
            gl_write_uint32(size);
            gl_write_uint32(compressed_size);
            fwrite(compressed, compressed_size, 1, trace_file);
            free(compressed);
            return;
        }
    }
    #endif
    #ifdef LZ4_ENABLED
    if (method == COMPRESSION_LZ4) {
        void* compressed = malloc(size);
        int compressed_size;
        if (!(compressed_size=LZ4_compress_default(data, compressed, size, size))) {
            free(compressed);
            goto none;
        } else {
            gl_write_b(COMPRESSION_LZ4);
            gl_write_uint32(size);
            gl_write_uint32(compressed_size);
            fwrite(compressed, compressed_size, 1, trace_file);
            free(compressed);
            return;
        }
    }
    #endif
    
    //#if to remove warnings
    #if defined(ZSTD_ENABLED) || defined(ZLIB_ENABLED) || defined(LZ4_ENABLED)
    none:
    #endif
        gl_write_b(COMPRESSION_NONE);
        gl_write_uint32(size);
        gl_write_uint32(size);
        fwrite(data, size, 1, trace_file);
}

static void gl_start_func_decl(uint32_t func_id, const char* name) {
    gl_write_b(OP_DECL_FUNC);
    gl_write_uint32(func_id);
    gl_write_str(name);
}

static void gl_end_func_decl() {}

static void gl_start_func_decl_args(uint32_t count) {
    gl_write_uint32(count);
}

static void gl_end_func_decl_args() {}

typedef struct {
    const char* name;
    size_t size;
} extra_t;

static size_t extra_count;
static extra_t** extras;

static void gl_start_call(uint32_t func_id) {
    gl_write_b(OP_CALL);
    gl_write_uint32(func_id);
    extra_count = 0;
    extras = NULL;
}

//name must be available at gl_end_call.
static void gl_add_extra(const char* name, size_t size, const void* data) {
    extras = realloc(extras, (extra_count+1)*sizeof(extra_t*));
    extra_t** extra = extras + extra_count++;
    *extra = malloc(sizeof(extra_t)+size);
    (*extra)->name = name;
    (*extra)->size = size;
    memcpy((*extra)+1, data, size);
}

static void gl_end_call() {
    gl_write_uint32(extra_count);
    
    for (size_t i = 0; i < extra_count; i++) {
        gl_write_str(extras[i]->name);
        gl_write_data(extras[i]->size, extras[i]+1);
        free(extras[i]);
    }
    free(extras);
}

static void* actual_func(const char* name) {
    return dlsym(RTLD_NEXT, name);
}

static void* actual_dlopen(const char* filename, int flags) {
    return ((void* (*)(const char*, int))actual_func("dlopen"))(filename, flags);
}

//TODO: Make get_func_real() and get_func() thread safe
static func_t get_func_real(const char* name) {
    return gl_glXGetProcAddress(name);
}

static bool gl_get(GLenum pname, GLdouble* dval, GLint64* ival) {
    switch (pname) {
    #define VALF(name) \
        {GLdouble v = current_config.name; if (dval) *dval = (v); if(ival) *ival = (v); break;}
    #define VALI(name) \
        {GLint64 v = current_config.name; if (dval) *dval = (v); if(ival) *ival = (v); break;}
    case GL_MAJOR_VERSION: VALI(major_version)
    case GL_MINOR_VERSION: VALI(minor_version)
    case GL_MAX_VERTEX_STREAMS: VALI(max_vertex_streams)
    case GL_MAX_CLIP_DISTANCES: VALI(max_clip_distances)
    case GL_MAX_DRAW_BUFFERS: VALI(max_draw_buffers)
    case GL_MAX_VIEWPORTS: VALI(max_viewports)
    case GL_MAX_VERTEX_ATTRIBS: VALI(max_vertex_attribs)
    case GL_MAX_COLOR_ATTACHMENTS: VALI(max_color_attachments)
    case GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS: VALI(max_combined_texture_units)
    case GL_MAX_PATCH_VERTICES: VALI(max_patch_vertices)
    case GL_MAX_RENDERBUFFER_SIZE: VALI(max_renderbuffer_size)
    case GL_MAX_TEXTURE_SIZE: VALI(max_texture_size)
    case GL_MAX_TRANSFORM_FEEDBACK_BUFFERS: VALI(max_xfb_buffers)
    case GL_MAX_UNIFORM_BUFFER_BINDINGS: VALI(max_ubo_bindings)
    case GL_MAX_ATOMIC_COUNTER_BUFFER_BINDINGS: VALI(max_atomic_counter_buffer_bindings)
    case GL_MAX_SHADER_STORAGE_BUFFER_BINDINGS: VALI(max_ssbo_bindings)
    case GL_MAX_SAMPLE_MASK_WORDS: VALI(max_sample_mask_words)
    #undef VALI
    #undef VALF
    default:
        return false;
    }
    return true;
}

static void glGetBooleanv_impl(GLenum pname, GLboolean* data) {
    REALF(glGetBooleanv)(pname, data);
    GLint64 ival;
    if (gl_get(pname, NULL, &ival)) *data = ival;
}

static void glGetDoublev_impl(GLenum pname, GLdouble* data) {
    REALF(glGetDoublev)(pname, data);
    gl_get(pname, data, NULL);
}

static void glGetFloatv_impl(GLenum pname, GLfloat* data) {
    REALF(glGetFloatv)(pname, data);
    GLdouble dval;
    if (gl_get(pname, &dval, NULL)) *data = dval;
}

static void glGetIntegerv_impl(GLenum pname, GLint* data) {
    REALF(glGetIntegerv)(pname, data);
    GLint64 ival;
    if (gl_get(pname, NULL, &ival)) *data = ival;
}

static void glGetInteger64v_impl(GLenum pname, GLint64* data) {
    REALF(glGetInteger64v)(pname, data);
    gl_get(pname, NULL, data);
}

static func_t get_func(func_t* f, const char* name) {
    if (*f) return *f;
    
    struct {
        const char* name;
        func_t func;
    } table[] = {
        {"glGetBooleanv", &glGetBooleanv_impl},
        {"glGetDoublev", &glGetDoublev_impl},
        {"glGetFloatv", &glGetFloatv_impl},
        {"glGetIntegerv", &glGetIntegerv_impl},
        {"glGetInteger64v", &glGetInteger64v_impl}
    };
    size_t table_size = sizeof(table) / sizeof(table[0]);
    for (size_t i = 0; i < table_size; i++) {
        if (strcmp(name, table[i].name) == 0)
            return *f = table[i].func;
    }
    return *f = get_func_real(name);
}

void* dlopen(const char* filename, int flags) {
    if (!filename)
        return actual_dlopen(filename, flags);
    
    if (strcmp(filename, "libGL.so") == 0 ||
        strcmp(filename, "libGL.so.1") == 0)
        return actual_dlopen(NULL, flags);
    
    return actual_dlopen(filename, flags);
}

static bool set_config_value_int(config_t* config, const char* name, int value) {
    config_entry_t entry;
    entry.next = config->entries;
    entry.name[sizeof(entry.name)-1] = 0;
    strncpy(entry.name, name, sizeof(entry.name)-1);
    entry.value = value;
    
    for (config_entry_t* cur = config->entries; cur; cur = cur->next) {
        if (strncmp(cur->name, name, sizeof(cur->name)-1)==0) {
            cur->value = entry.value;
            return true;
        }
    }
    
    config_entry_t* entryp = malloc(sizeof(config_entry_t));
    *entryp = entry;
    config->entries = entryp;
    return true;
}

static bool set_config_value(config_t* config, const char* name, const char* value) {
    int ival = 0;
    for (const char* c = value; *c; c++) {
        if (*c<'0' || *c>'9') return false;
        ival = ival*10 + (*c-'0');
    }
    return set_config_value_int(config, name, ival);
}

static void get_major_minor(opengl_version_t ver, int* major, int* minor) {
    switch (ver) {
    case OpenGLVersion_32:
    case OpenGLVersion_33: *major = 3; break;
    case OpenGLVersion_40:
    case OpenGLVersion_41:
    case OpenGLVersion_42:
    case OpenGLVersion_43:
    case OpenGLVersion_44:
    case OpenGLVersion_45:
    case OpenGLVersion_46: *major = 4; break;
    case OpenGLVersion_Count: break;
    }
    
    switch (ver) {
    case OpenGLVersion_32: *minor = 2; break;
    case OpenGLVersion_33: *minor = 3; break;
    case OpenGLVersion_40: *minor = 0; break;
    case OpenGLVersion_41: *minor = 1; break;
    case OpenGLVersion_42: *minor = 2; break;
    case OpenGLVersion_43: *minor = 3; break;
    case OpenGLVersion_44: *minor = 4; break;
    case OpenGLVersion_45: *minor = 5; break;
    case OpenGLVersion_46: *minor = 6; break;
    case OpenGLVersion_Count: break;
    }
}

static bool parse_major_minor(int major, int minor, opengl_version_t* ver) {
    for (size_t i = 0; i < OpenGLVersion_Count; i++) {
        int major2, minor2;
        get_major_minor(i, &major2, &minor2);
        if (major2==major && minor2==minor) {
            *ver = i;
            return true;
        }
    }
    return false;
}

static bool parse_version(const char* src, opengl_version_t* ver) {
    if (!src[0] || src[0]<'0' || src[0]>'9') return false;
    int major = src[0] - '0';
    if (src[1] != '.') return false;
    if (!src[2] || src[2]<'0' || src[2]>'9') return false;
    int minor = src[2] - '0';
    
    return parse_major_minor(major, minor, ver);
}

static bool add_version_spec(bool* mask, const char* spec) {
    opengl_version_t base_ver;
    if (!parse_version(spec, &base_ver)) return false;
    spec += 3;
    
    if (spec[0] == 0) {
        mask[base_ver] = true;
    } else if (spec[0]=='+' && spec[1]==0) {
        for (size_t i = base_ver; i < OpenGLVersion_Count; i++)
            mask[i] = true;
    } else if (spec[0]=='-') {
        opengl_version_t end_ver;
        if (!parse_version(spec+1, &end_ver)) return false;
        for (size_t i = base_ver; i <= end_ver; i++)
            mask[i] = true;
    } else {
        return false;
    }
    
    return true;
}

static void handle_config() {
    char *filename = getenv("WIP15_CONFIG");
    if (!filename) {
        fprintf(stderr, "WIP15_CONFIG environment variable (the filename of the limits file) has not been set.\n");
        exit(1);
    }
    
    FILE* file = fopen(filename, "r");
    if (!file) {
        fprintf(stderr, "Unable to open config file '%s'.\n", filename);
        exit(1);
    }
    
    for (size_t i = 0; i < OpenGLVersion_Count; i++) {
        configs[i].entries = NULL;
        int major, minor;
        get_major_minor(i, &major, &minor);
        set_config_value_int(&configs[i], "major_version", major);
        set_config_value_int(&configs[i], "minor_version", minor);
    }
    bool version_mask[OpenGLVersion_Count] = {0};
    
    while (true) {
        int c = fgetc(file);
        if (c == EOF) {
            break;
        } else if (c == '@') {
            for (size_t i = 0; i < OpenGLVersion_Count; i++) version_mask[i] = false;
            
            //TODO: handle errors in add_version_spec
            char version_spec[16] = {0};
            while (true) {
                c = fgetc(file);
                if (c==EOF || c=='\n') {
                    break;
                } else if (c == ',') {
                    add_version_spec(version_mask, version_spec);
                    memset(version_spec, 0, sizeof(version_spec));
                } else if (c==' ' || c=='\t' || c=='\r') {
                    //ignore whitespace
                } else {
                    if (strlen(version_spec) < 15)
                        version_spec[strlen(version_spec)] = c;
                }
            }
            
            if (strlen(version_spec))
                add_version_spec(version_mask, version_spec);
        } else if (c == '#') {
            while (true) {
                c = fgetc(file);
                if (c == EOF)
                    break;
                else if (c == '\n')
                    break;
            }
        } else if (c == '\n') {
        } else {
            char name[256];
            memset(name, 0, 256);
            name[0] = c;
            size_t i = 1;
            while (i < 255) {
                c = fgetc(file);
                if (c == EOF)
                    break;
                else if (c == '\n')
                    break;
                else if (c == ':')
                    break;
                else
                    name[i++] = c;
            }
            
            char value[1024];
            memset(value, 0, 1024);
            i = 0;
            while (i < 1024) {
                c = fgetc(file);
                if (c == EOF)
                    break;
                else if (c == '\n')
                    break;
                else if (c == ' ' && i == 0)
                    ;
                else
                    value[i++] = c;
            }
            
            //TODO: Handle errors in set_config_value
            for (size_t i = 0; i < OpenGLVersion_Count; i++) {
                if (version_mask[i])
                    set_config_value(&configs[i], name, value);
            }
        }
    }
    
    fclose(file);
}

static void free_config(config_t* config) {
    for (config_entry_t* entry = config->entries; entry;) {
        config_entry_t* next = entry->next;
        free(entry);
        entry = next;
    }
}

void __attribute__ ((destructor)) wip15_gl_deinit() {
    fclose(trace_file);
    dlclose(lib_gl);
    
    for (size_t i = 0; i < OpenGLVersion_Count; i++)
        free_config(&configs[i]);
}

void wip15TestFB(const GLchar* name, const GLvoid* color, const GLvoid* depth);
void wip15DrawableSize(GLsizei width, GLsizei height);

static void update_drawable_size() {
    Display* dpy = F(glXGetCurrentDisplay)();
    GLXDrawable drawable = F(glXGetCurrentDrawable)();
    
    if (!F(glXGetCurrentContext)())
        return;
    
    int w, h;
    if (dpy && drawable!=None) {
        uint w_, h_;
        F(glXQueryDrawable)(dpy, drawable, GLX_WIDTH, &w_);
        F(glXQueryDrawable)(dpy, drawable, GLX_HEIGHT, &h_);
        
        w = w_;
        h = h_;
    } else {
        w = -1;
        h = -1;
    }
    
    if (w!=drawable_width || h!=drawable_height)
        wip15DrawableSize(w, h);
}

static config_t* get_current_config() {
    GLint major, minor;
    REALF(glGetIntegerv)(GL_MAJOR_VERSION, &major);
    REALF(glGetIntegerv)(GL_MINOR_VERSION, &minor);
    opengl_version_t ver;
    if (!parse_major_minor(major, minor, &ver))
        ver = OpenGLVersion_Count - 1; //Just use the latest
    return &configs[ver];
}

//TODO: from test_host_config() in nontrivial_func_impls.c
typedef struct cap_info_t {
    const char* name;
    size_t offset;
} cap_info_t;
static const cap_info_t caps[] = {
    {"max_vertex_streams", offsetof(trc_replay_config_t, max_vertex_streams)},
    {"max_clip_distances", offsetof(trc_replay_config_t, max_clip_distances)},
    {"max_draw_buffers", offsetof(trc_replay_config_t, max_draw_buffers)},
    {"max_viewports", offsetof(trc_replay_config_t, max_viewports)},
    {"max_vertex_attribs", offsetof(trc_replay_config_t, max_vertex_attribs)},
    {"max_color_attachments", offsetof(trc_replay_config_t, max_color_attachments)},
    {"max_combined_texture_units", offsetof(trc_replay_config_t, max_combined_texture_units)},
    {"max_patch_vertices", offsetof(trc_replay_config_t, max_patch_vertices)},
    {"max_renderbuffer_size", offsetof(trc_replay_config_t, max_renderbuffer_size)},
    {"max_texture_size", offsetof(trc_replay_config_t, max_texture_size)},
    {"max_xfb_buffers", offsetof(trc_replay_config_t, max_xfb_buffers)},
    {"max_ubo_bindings", offsetof(trc_replay_config_t, max_ubo_bindings)},
    {"max_atomic_counter_buffer_bindings", offsetof(trc_replay_config_t, max_atomic_counter_buffer_bindings)},
    {"max_ssbo_bindings", offsetof(trc_replay_config_t, max_ssbo_bindings)}};

#define REPLAY_CONFIG_FUNCS
#define RC_F REALF
#include "shared/replay_config.h"
#undef RC_F
#undef REPLAY_CONFIG_FUNCS

static trc_replay_config_t create_replay_config() {
    trc_replay_config_t cfg;
    init_host_config(NULL, &cfg);
    
    config_t* src_cfg = get_current_config();
    
    size_t cap_count = sizeof(caps) / sizeof(caps[0]);
    
    for (config_entry_t* entry = src_cfg->entries; entry; entry = entry->next) {
        for (size_t i = 0; i < cap_count; i++) {
            if (strcmp(caps[i].name, entry->name) == 0) {
                *(int*)(((uint8_t*)&cfg) + caps[i].offset) = entry->value;
            }
        }
    }
    
    return cfg;
}

static void glx_make_current_epilogue() {
    if (F(glXGetCurrentContext)())
        current_config = create_replay_config();
    reset_gl_funcs();
    update_drawable_size();
}

void wip15SetTargetOptions(GLsizeiptr count, const char*const* names, const char*const* values);

static void set_target_options(GLXContext ctx) {
    trc_replay_config_t cfg = create_replay_config();
    
    size_t cap_count = sizeof(caps) / sizeof(caps[0]);
    const char** names = malloc(cap_count*sizeof(const char*));
    char** values = malloc(cap_count*sizeof(char*));
    
    for (size_t i = 0; i < cap_count; i++) {
        names[i] = caps[i].name;
        values[i] = malloc(16);
        sprintf(values[i], "%d", *(int*)(((uint8_t*)&cfg)+caps[i].offset));
    }
    wip15SetTargetOptions(cap_count, names, (const char*const*)values);
    
    for (size_t i = 0; i < cap_count; i++) free(values[i]);
    free(values);
    free(names);
}

#define BEGIN_PROLOGUE(fb_config) Display* prev_dpy = F(glXGetCurrentDisplay)();\
GLXDrawable prev_draw_drawable = F(glXGetCurrentDrawable)();\
GLXDrawable prev_read_drawable = F(glXGetCurrentReadDrawable)();\
GLXContext prev_ctx = F(glXGetCurrentContext)();\
GLXPixmap pbuf = F(glXCreatePbuffer)(dpy, fb_config, (int[]){None});\

#define END_PROLOGUE if (ctx) {\
    F(glXMakeCurrent)(dpy, pbuf, ctx);\
    reset_gl_funcs();\
    set_target_options(ctx);\
    F(glXDestroyContext)(dpy, ctx);\
}\
F(glXDestroyPbuffer)(dpy, pbuf);\
F(glXMakeContextCurrent)(prev_dpy, prev_draw_drawable, prev_read_drawable, prev_ctx);\
reset_gl_funcs();

static void glx_create_context_attribs_prologue(Display* dpy, GLXFBConfig config, GLXContext share_context, Bool direct, const int* attrib_list) {
    BEGIN_PROLOGUE(config)
    GLXContext ctx = F(glXCreateContextAttribsARB)(dpy, config, share_context, direct, (int*)attrib_list);
    END_PROLOGUE
}

static void glx_create_context_prologue(Display* dpy, XVisualInfo* vis, GLXContext shareList, Bool direct) {
    int _;
    GLXFBConfig* fb_configs = (GLXFBConfig*)F(glXChooseFBConfig)(dpy, vis->screen, (int[]){None}, &_);
    BEGIN_PROLOGUE(fb_configs[0])
    XFree(fb_configs);
    GLXContext ctx = F(glXCreateContext)(dpy, vis, shareList, direct);
    END_PROLOGUE
}

static size_t get_texel_size(GLenum format, GLenum type) {
    size_t components = 0;
    switch (format) {
    case GL_RED:
    case GL_RED_INTEGER:
    case GL_STENCIL_INDEX:
    case GL_DEPTH_COMPONENT:
        components = 1;
        break;
    case GL_DEPTH_STENCIL:
        return type == GL_UNSIGNED_INT_24_8_EXT ? 4 : 0;
    case GL_RG:
    case GL_RG_INTEGER:
        components = 2;
        break;
    case GL_RGB:
    case GL_RGB_INTEGER:
    case GL_BGR_INTEGER:
    case GL_BGR:
        components = 3;
        break;
    case GL_RGBA:
    case GL_RGBA_INTEGER:
    case GL_BGRA_INTEGER:
    case GL_BGRA:
        components = 4;
        break;
    }

    size_t final_size = 0;
    switch (type) {
    case GL_UNSIGNED_BYTE:
    case GL_BYTE:
        final_size = components;
        break;
    case GL_UNSIGNED_SHORT:
    case GL_SHORT:
        final_size = components * 2;
        break;
    case GL_UNSIGNED_INT:
    case GL_INT:
    case GL_FLOAT:
        final_size = components * 4;
        break;
    case GL_UNSIGNED_BYTE_3_3_2:
    case GL_UNSIGNED_BYTE_2_3_3_REV:
        final_size = 1;
        break;
    case GL_UNSIGNED_SHORT_5_6_5:
    case GL_UNSIGNED_SHORT_5_6_5_REV:
    case GL_UNSIGNED_SHORT_5_5_5_1:
    case GL_UNSIGNED_SHORT_1_5_5_5_REV:
    case GL_UNSIGNED_SHORT_4_4_4_4:
    case GL_UNSIGNED_SHORT_4_4_4_4_REV:
        final_size = 2;
        break;
    case GL_UNSIGNED_INT_8_8_8_8:
    case GL_UNSIGNED_INT_8_8_8_8_REV:
    case GL_UNSIGNED_INT_10_10_10_2:
    case GL_UNSIGNED_INT_2_10_10_10_REV:
        final_size = 4;
        break;
    }
    
    return final_size;
}

size_t glx_attrib_int_count(const int* attribs) {
    size_t count = 0;
    while (attribs[count]) count += 2;
    return count + 1;
}

static void add_program_extra(const char* type, const char* name, GLenum stage, uint32_t val) {
    uint8_t data[strlen(name)+12];
    val = htole32(val);
    stage = htole32(stage);
    uint32_t len = htole32(strlen(name));
    memcpy(data, &val, 4);
    memcpy(data+4, &stage, 4);
    memcpy(data+8, &len, 4);
    memcpy(data+12, name, strlen(name));
    gl_add_extra(type, strlen(name)+12, data);
}

typedef enum uniform_type_t {
    UniformType_Variable,
    UniformType_Struct,
    UniformType_Array,
    UniformType_Unspecified
} uniform_type_t;

typedef struct uniform_t {
    union {
        char* name;
        size_t index;
    };
    uniform_type_t type;
    struct uniform_t* prev;
    struct uniform_t* next;
    union {
        struct {
            GLenum dtype;
            GLuint location;
        };
        struct {
            size_t child_count;
            struct uniform_t* first_child;
        };
    };
} uniform_t;

//TODO: Make this smaller
static bool add_uniform_variable(GLuint program, uniform_t** uniforms, GLchar* name, GLint size,
                                 GLenum type, GLint loc, bool array) {
    GLchar name_temp[strlen(name)+1];
    strcpy(name_temp, name);
    GLchar* name_ptr = name_temp;
    uniform_t* cur = NULL;
    
    char* name_end = name_ptr;
    for (; *name_end!=0 && *name_end!='.' && *name_end!='['; name_end++) ;
    size_t name_len = name_end - name_ptr;
    
    for (cur = *uniforms; cur; cur = cur->next) {
        if (strlen(cur->name)==name_len && strncmp(cur->name, name_ptr, name_len)==0)
            goto cont;
    }
    
    cur = malloc(sizeof(uniform_t));
    cur->name = calloc(name_len+1, 1);
    memcpy(cur->name, name_ptr, name_len);
    cur->type = UniformType_Unspecified;
    cur->prev = NULL;
    cur->next = *uniforms;
    *uniforms = cur;
    
    cont:
    name_ptr = name_end;
    
    while (*name_ptr != 0) {
        if (*name_ptr == '.') {
            if (cur->type == UniformType_Unspecified) {
                cur->type = UniformType_Struct;
                cur->child_count = 0;
                cur->first_child = NULL;
            }
            if (cur->type != UniformType_Struct) return false;
            name_ptr++;
            
            char* name_end = name_ptr;
            for (; *name_end!=0 && *name_end!='.' && *name_end!='['; name_end++) ;
            size_t name_len = name_end - name_ptr;
            
            uniform_t* member;
            for (member = cur->first_child; member; member = member->next) {
                if (strlen(member->name)==name_len && strncmp(member->name, name_ptr, name_len)==0)
                    goto cont2;
            }
            
            member = calloc(sizeof(uniform_t), 1);
            member->name = calloc(name_len+1, 1);
            memcpy(member->name, name_ptr, name_len);
            member->type = UniformType_Unspecified;
            member->next = cur->first_child;
            cur->first_child = member;
            cur->child_count++;
            
            cont2:
            cur = member;
            name_ptr = name_end;
        } else if (*name_ptr == '[') {
            if (cur->type == UniformType_Unspecified) {
                cur->type = UniformType_Array;
                cur->child_count = 0;
                cur->first_child = NULL;
            }
            if (cur->type != UniformType_Array) return false;
            
            name_ptr++;
            unsigned long long int index = strtoull(name_ptr, &name_ptr, 10);
            if (*name_ptr != ']') return false;
            name_ptr++;
            
            uniform_t* element;
            for (element = cur->first_child; element; element = element->next) {
                if (element->index == index)
                    goto cont3;
            }
            
            element = calloc(sizeof(uniform_t), 1);
            element->index = index;
            element->type = UniformType_Unspecified;
            element->next = cur->first_child;
            cur->first_child = element;
            cur->child_count++;
            
            cont3:
            cur = element;
        } else {
            return false;
        }
    }
    
    if (cur->type != UniformType_Unspecified) return false;
    
    if (array) {
        cur->type = UniformType_Array;
        cur->first_child = NULL;
        cur->child_count = size;
        for (size_t j = 0; j < size; j++) {
            uniform_t* element = malloc(sizeof(uniform_t));
            element->index = j;
            element->type = UniformType_Variable;
            element->prev = NULL;
            element->next = cur->first_child;
            cur->first_child = element;
            element->dtype = type;
            GLchar element_name[strlen(name)+8];
            sprintf(element_name, "%s[%zu]", name, j);
            element->location = F(glGetUniformLocation)(program, element_name);
        }
    } else {
        cur->type = UniformType_Variable;
        cur->dtype = type;
        cur->location = loc;
    }
    
    return true;
}

static size_t get_uniforms_extra_max_size(uniform_t* uniform, bool array_element) {
    switch (uniform->type) {
    case UniformType_Variable: {
        return array_element ? 14 : 14+strlen(uniform->name);
    }
    case UniformType_Struct:
    case UniformType_Array: {
        size_t size = array_element ? 9+uniform->child_count*4
                                    : 9+uniform->child_count*4+strlen(uniform->name);
        for (uniform_t* child = uniform->first_child; child; child = child->next)
            size += get_uniforms_extra_max_size(child, uniform->type==UniformType_Array);
        return size;
    }
    case UniformType_Unspecified: {
        return 0;
    }
    }
    return 0;
}

static size_t get_uniforms_spec_count(uniform_t* uniform) {
    switch (uniform->type) {
    case UniformType_Variable: {
        return 1;
    }
    case UniformType_Struct:
    case UniformType_Array: {
        size_t count = 1;
        for (uniform_t* child = uniform->first_child; child; child = child->next)
            count += get_uniforms_spec_count(child);
        return count;
    }
    case UniformType_Unspecified: {
        return 0;
    }
    }
    return 0;
}

typedef struct uniform_dtype_info_t {
    GLenum type;
    uint8_t base_type;
    uint8_t dim[2];
    uint8_t tex_type;
    bool tex_shadow;
    bool tex_array;
    bool tex_multisample;
    uint8_t tex_dtype;
} uniform_dtype_info_t;

static uniform_dtype_info_t uniform_dtype_info[] = {
    {GL_FLOAT, 0, {1, 1}, 0, false, false, false, 0},
    {GL_FLOAT_VEC2, 0, {2, 1}, 0, false, false, false, 0},
    {GL_FLOAT_VEC3, 0, {3, 1}, 0, false, false, false, 0},
    {GL_FLOAT_VEC4, 0, {4, 1}, 0, false, false, false, 0},
    {GL_DOUBLE, 1, {1, 1}, 0, false, false, false, 0},
    {GL_DOUBLE_VEC2, 1, {2, 1}, 0, false, false, false, 0},
    {GL_DOUBLE_VEC3, 1, {3, 1}, 0, false, false, false, 0},
    {GL_DOUBLE_VEC4, 1, {4, 1}, 0, false, false, false, 0},
    {GL_INT, 3, {1, 1}, 0, false, false, false, 0},
    {GL_INT_VEC2, 3, {2, 1}, 0, false, false, false, 0},
    {GL_INT_VEC3, 3, {3, 1}, 0, false, false, false, 0},
    {GL_INT_VEC4, 3, {4, 1}, 0, false, false, false, 0},
    {GL_UNSIGNED_INT, 2, {1, 1}, 0, false, false, false, 0},
    {GL_UNSIGNED_INT_VEC2, 2, {2, 1}, 0, false, false, false, 0},
    {GL_UNSIGNED_INT_VEC3, 2, {3, 1}, 0, false, false, false, 0},
    {GL_UNSIGNED_INT_VEC4, 2, {4, 1}, 0, false, false, false, 0},
    {GL_BOOL, 6, {1, 1}, 0, false, false, false, 0},
    {GL_BOOL_VEC2, 6, {2, 1}, 0, false, false, false, 0},
    {GL_BOOL_VEC3, 6, {3, 1}, 0, false, false, false, 0},
    {GL_BOOL_VEC4, 6, {4, 1}, 0, false, false, false, 0},
    {GL_FLOAT_MAT2, 0, {2, 2}, 0, false, false, false, 0},
    {GL_FLOAT_MAT3, 0, {3, 3}, 0, false, false, false, 0},
    {GL_FLOAT_MAT4, 0, {4, 4}, 0, false, false, false, 0},
    {GL_FLOAT_MAT2x3, 0, {2, 3}, 0, false, false, false, 0},
    {GL_FLOAT_MAT2x4, 0, {2, 4}, 0, false, false, false, 0},
    {GL_FLOAT_MAT3x2, 0, {3, 2}, 0, false, false, false, 0},
    {GL_FLOAT_MAT3x4, 0, {3, 4}, 0, false, false, false, 0},
    {GL_FLOAT_MAT4x2, 0, {4, 2}, 0, false, false, false, 0},
    {GL_FLOAT_MAT4x3, 0, {4, 3}, 0, false, false, false, 0},
    {GL_DOUBLE_MAT2, 1, {2, 2}, 0, false, false, false, 0},
    {GL_DOUBLE_MAT3, 1, {3, 3}, 0, false, false, false, 0},
    {GL_DOUBLE_MAT4, 1, {4, 4}, 0, false, false, false, 0},
    {GL_DOUBLE_MAT2x3, 1, {2, 3}, 0, false, false, false, 0},
    {GL_DOUBLE_MAT2x4, 1, {2, 4}, 0, false, false, false, 0},
    {GL_DOUBLE_MAT3x2, 1, {3, 2}, 0, false, false, false, 0},
    {GL_DOUBLE_MAT3x4, 1, {3, 4}, 0, false, false, false, 0},
    {GL_DOUBLE_MAT4x2, 1, {4, 2}, 0, false, false, false, 0},
    {GL_DOUBLE_MAT4x3, 1, {4, 3}, 0, false, false, false, 0},
    {GL_SAMPLER_1D, 7, {0, 0}, 0, False, False, False, 0},
    {GL_SAMPLER_2D, 7, {0, 0}, 1, False, False, False, 0},
    {GL_SAMPLER_3D, 7, {0, 0}, 2, False, False, False, 0},
    {GL_SAMPLER_CUBE, 7, {0, 0}, 3, False, False, False, 0},
    {GL_SAMPLER_1D_SHADOW, 7, {0, 0}, 0, True, False, False, 0},
    {GL_SAMPLER_2D_SHADOW, 7, {0, 0}, 1, True, False, False, 0},
    {GL_SAMPLER_1D_ARRAY, 7, {0, 0}, 0, False, True, False, 0},
    {GL_SAMPLER_2D_ARRAY, 7, {0, 0}, 1, False, True, False, 0},
    {GL_SAMPLER_CUBE_MAP_ARRAY, 7, {0, 0}, 3, False, True, False, 0},
    {GL_SAMPLER_1D_ARRAY_SHADOW, 7, {0, 0}, 0, True, True, False, 0},
    {GL_SAMPLER_2D_ARRAY_SHADOW, 7, {0, 0}, 1, True, True, False, 0},
    {GL_SAMPLER_2D_MULTISAMPLE, 7, {0, 0}, 1, False, False, True, 0},
    {GL_SAMPLER_2D_MULTISAMPLE_ARRAY, 7, {0, 0}, 1, False, True, True, 0},
    {GL_SAMPLER_CUBE_SHADOW, 7, {0, 0}, 3, True, False, False, 0},
    {GL_SAMPLER_CUBE_MAP_ARRAY_SHADOW, 7, {0, 0}, 3, True, True, False, 0},
    {GL_SAMPLER_BUFFER, 7, {0, 0}, 5, False, False, False, 0},
    {GL_SAMPLER_2D_RECT, 7, {0, 0}, 4, False, False, False, 0},
    {GL_SAMPLER_2D_RECT_SHADOW, 7, {0, 0}, 4, True, False, False, 0},
    {GL_INT_SAMPLER_1D, 7, {0, 0}, 0, False, False, False, 3},
    {GL_INT_SAMPLER_2D, 7, {0, 0}, 1, False, False, False, 3},
    {GL_INT_SAMPLER_3D, 7, {0, 0}, 2, False, False, False, 3},
    {GL_INT_SAMPLER_CUBE, 7, {0, 0}, 3, False, False, False, 3},
    {GL_INT_SAMPLER_1D_ARRAY, 7, {0, 0}, 0, False, True, False, 3},
    {GL_INT_SAMPLER_2D_ARRAY, 7, {0, 0}, 1, False, True, False, 3},
    {GL_INT_SAMPLER_CUBE_MAP_ARRAY, 7, {0, 0}, 3, False, True, False, 3},
    {GL_INT_SAMPLER_2D_MULTISAMPLE, 7, {0, 0}, 1, False, False, True, 3},
    {GL_INT_SAMPLER_2D_MULTISAMPLE_ARRAY, 7, {0, 0}, 1, False, True, True, 3},
    {GL_INT_SAMPLER_BUFFER, 7, {0, 0}, 5, False, False, False, 3},
    {GL_INT_SAMPLER_2D_RECT, 7, {0, 0}, 4, False, False, False, 3},
    {GL_UNSIGNED_INT_SAMPLER_1D, 7, {0, 0}, 0, False, False, False, 2},
    {GL_UNSIGNED_INT_SAMPLER_2D, 7, {0, 0}, 1, False, False, False, 2},
    {GL_UNSIGNED_INT_SAMPLER_3D, 7, {0, 0}, 2, False, False, False, 2},
    {GL_UNSIGNED_INT_SAMPLER_CUBE, 7, {0, 0}, 3, False, False, False, 2},
    {GL_UNSIGNED_INT_SAMPLER_1D_ARRAY, 7, {0, 0}, 0, False, True, False, 2},
    {GL_UNSIGNED_INT_SAMPLER_2D_ARRAY, 7, {0, 0}, 1, False, True, False, 2},
    {GL_UNSIGNED_INT_SAMPLER_CUBE_MAP_ARRAY, 7, {0, 0}, 3, False, True, False, 2},
    {GL_UNSIGNED_INT_SAMPLER_2D_MULTISAMPLE, 7, {0, 0}, 1, False, False, True, 2},
    {GL_UNSIGNED_INT_SAMPLER_2D_MULTISAMPLE_ARRAY, 7, {0, 0}, 1, False, True, True, 2},
    {GL_UNSIGNED_INT_SAMPLER_BUFFER, 7, {0, 0}, 5, False, False, False, 2},
    {GL_UNSIGNED_INT_SAMPLER_2D_RECT, 7, {0, 0}, 4, False, False, False, 2},
    {GL_IMAGE_1D, 8, {0, 0}, 0, False, False, False, 0},
    {GL_IMAGE_2D, 8, {0, 0}, 1, False, False, False, 0},
    {GL_IMAGE_3D, 8, {0, 0}, 2, False, False, False, 0},
    {GL_IMAGE_2D_RECT, 8, {0, 0}, 4, False, False, False, 0},
    {GL_IMAGE_CUBE, 8, {0, 0}, 3, False, False, False, 0},
    {GL_IMAGE_BUFFER, 8, {0, 0}, 5, False, False, False, 0},
    {GL_IMAGE_1D_ARRAY, 8, {0, 0}, 0, False, True, False, 0},
    {GL_IMAGE_2D_ARRAY, 8, {0, 0}, 1, False, True, False, 0},
    {GL_IMAGE_CUBE_MAP_ARRAY, 8, {0, 0}, 3, False, True, False, 0},
    {GL_IMAGE_2D_MULTISAMPLE, 8, {0, 0}, 1, False, False, True, 0},
    {GL_IMAGE_2D_MULTISAMPLE_ARRAY, 8, {0, 0}, 1, False, True, True, 0},
    {GL_INT_IMAGE_1D, 8, {0, 0}, 0, False, False, False, 3},
    {GL_INT_IMAGE_2D, 8, {0, 0}, 1, False, False, False, 3},
    {GL_INT_IMAGE_3D, 8, {0, 0}, 2, False, False, False, 3},
    {GL_INT_IMAGE_2D_RECT, 8, {0, 0}, 4, False, False, False, 3},
    {GL_INT_IMAGE_CUBE, 8, {0, 0}, 3, False, False, False, 3},
    {GL_INT_IMAGE_BUFFER, 8, {0, 0}, 5, False, False, False, 3},
    {GL_INT_IMAGE_1D_ARRAY, 8, {0, 0}, 0, False, True, False, 3},
    {GL_INT_IMAGE_2D_ARRAY, 8, {0, 0}, 1, False, True, False, 3},
    {GL_INT_IMAGE_CUBE_MAP_ARRAY, 8, {0, 0}, 3, False, True, False, 3},
    {GL_INT_IMAGE_2D_MULTISAMPLE, 8, {0, 0}, 1, False, False, True, 3},
    {GL_INT_IMAGE_2D_MULTISAMPLE_ARRAY, 8, {0, 0}, 1, False, True, True, 3},
    {GL_UNSIGNED_INT_IMAGE_1D, 8, {0, 0}, 0, False, False, False, 2},
    {GL_UNSIGNED_INT_IMAGE_2D, 8, {0, 0}, 1, False, False, False, 2},
    {GL_UNSIGNED_INT_IMAGE_3D, 8, {0, 0}, 2, False, False, False, 2},
    {GL_UNSIGNED_INT_IMAGE_2D_RECT, 8, {0, 0}, 4, False, False, False, 2},
    {GL_UNSIGNED_INT_IMAGE_CUBE, 8, {0, 0}, 3, False, False, False, 2},
    {GL_UNSIGNED_INT_IMAGE_BUFFER, 8, {0, 0}, 5, False, False, False, 2},
    {GL_UNSIGNED_INT_IMAGE_1D_ARRAY, 8, {0, 0}, 0, False, True, False, 2},
    {GL_UNSIGNED_INT_IMAGE_2D_ARRAY, 8, {0, 0}, 1, False, True, False, 2},
    {GL_UNSIGNED_INT_IMAGE_CUBE_MAP_ARRAY, 8, {0, 0}, 3, False, True, False, 2},
    {GL_UNSIGNED_INT_IMAGE_2D_MULTISAMPLE, 8, {0, 0}, 1, False, False, True, 2},
    {GL_UNSIGNED_INT_IMAGE_2D_MULTISAMPLE_ARRAY, 8, {0, 0}, 1, False, True, True, 2},
    {GL_UNSIGNED_INT_ATOMIC_COUNTER, 9, {0, 0}, 0, false, false, false, 0},
};

static uint8_t* write_uniform_extra(uint8_t* data, uniform_t* uniform, bool array_element) {
    uint32_t name_len = htole32(array_element?0:strlen(uniform->name));
    memcpy(data, &name_len, 4);
    memcpy(data+4, uniform->name, name_len);
    data += 4 + name_len;
    switch (uniform->type) {
    case UniformType_Variable: {
        uniform_dtype_info_t info;
        for (size_t i = 0; i < sizeof(uniform_dtype_info)/sizeof(uniform_dtype_info[0]); i++) {
            if (uniform_dtype_info[i].type == uniform->dtype) {
                info = uniform_dtype_info[i];
                goto found;
            }
        }
        //TODO: Handle
        found:
        *data++ = info.base_type;
        uint32_t loc = htole32(uniform->location);
        memcpy(data, &loc, 4);
        data += 4;
        if (info.dim[0]==0 && info.dim[0]==0) {
            *data++ = info.tex_type;
            *data++ = info.tex_shadow;
            *data++ = info.tex_array;
            *data++ = info.tex_multisample;
            *data++ = info.tex_dtype;
        } else {
            *data++ = info.dim[0];
            *data++ = info.dim[1];
        }
        break;
    }
    case UniformType_Struct: {
        *data++ = 10;
        uint32_t member_count = htole32(uniform->child_count);
        memcpy(data, &member_count, 4);
        data += 4;
        for (uniform_t* member = uniform->first_child; member; member = member->next)
            data = write_uniform_extra(data, member, false);
        break;
    }
    case UniformType_Array: {
        *data++ = 11;
        uint32_t element_count = htole32(uniform->child_count);
        memcpy(data, &element_count, 4);
        data += 4;
        for (size_t i = 0; i < uniform->child_count; i++) {
            uniform_t* element = uniform->first_child;
            for (; element&&element->index!=i; element = element->next)
            if (!element) ; //TODO: This should never happen because of previous validation
            data = write_uniform_extra(data, element, true);
        }
        break;
    }
    case UniformType_Unspecified: {
        break;
    }
    }
    return data;
}

static void write_uniforms_extra(uniform_t* uniforms) {
    size_t size = 8;
    for (uniform_t* uniform = uniforms; uniform; uniform = uniform->next)
        size += get_uniforms_extra_max_size(uniform, false);
    uint8_t* data = malloc(size);
    
    uint32_t spec_count = 0;
    for (uniform_t* uniform = uniforms; uniform; uniform = uniform->next)
        spec_count += get_uniforms_spec_count(uniform);
    spec_count = htole32(spec_count);
    memcpy(data, &spec_count, 4);
    
    uint32_t root_spec_count = 0;
    for (uniform_t* uniform = uniforms; uniform; uniform = uniform->next) root_spec_count++;
    root_spec_count = htole32(root_spec_count);
    memcpy(data+4, &root_spec_count, 4);
    
    uint8_t* data_ptr = data + 8;
    for (uniform_t* uniform = uniforms; uniform; uniform = uniform->next)
        data_ptr = write_uniform_extra(data_ptr, uniform, false);
    
    gl_add_extra("replay/program/uniforms", size, data);
    
    free(data);
}

static void link_program_extras(GLuint program) {
    GLint count;
    F(glGetProgramiv)(program, GL_ACTIVE_UNIFORMS, &count);
    GLint maxNameLen;
    F(glGetProgramiv)(program, GL_ACTIVE_UNIFORM_MAX_LENGTH, &maxNameLen);
    
    uniform_t* uniforms = NULL;
    for (size_t i = 0; i < count; i++) {
        GLchar name[maxNameLen+1];
        memset(name, 0, maxNameLen+1);
        GLint size;
        GLenum type;
        F(glGetActiveUniform)(program, i, maxNameLen, NULL, &size, &type, name);
        GLint loc = F(glGetUniformLocation)(program, name);
        if (loc < 0) continue; //Probably part of a uniform buffer block or a builtin variable
        
        bool array = strlen(name)>3 && strcmp(name+strlen(name)-3, "[0]")==0;
        array = array || size!=1;
        
        if (strlen(name)>3 && strcmp(name+strlen(name)-3, "[0]")==0)
            name[strlen(name)-3] = 0;
        
        if (!add_uniform_variable(program, &uniforms, name, size, type, loc, array)) {
            //TODO: Free uniforms and don't write the extra
        }
    }
    
    //TODO: Validate uniforms
    
    write_uniforms_extra(uniforms);
    
    F(glGetProgramiv)(program, GL_ACTIVE_ATTRIBUTES, &count);
    F(glGetProgramiv)(program, GL_ACTIVE_ATTRIBUTE_MAX_LENGTH, &maxNameLen);
    for (size_t i = 0; i < count; i++) {
        GLchar name[maxNameLen+1];
        memset(name, 0, maxNameLen+1);
        GLint size;
        GLenum type;
        F(glGetActiveAttrib)(program, i, maxNameLen, NULL, &size, &type, name);
        
        size_t name_len = strlen(name);
        bool array = name_len>=3 && strcmp(name+name_len-3, "[0]")==0;
        if (array) {
            memset(name+name_len-3, 0, 3);
            for (GLint i = 0; i < size; i++) {
                GLchar newname[maxNameLen+16];
                sprintf(newname, "%s[%u]", name, i);
                GLint loc = F(glGetAttribLocation)(program, newname);
                if (loc == -1) continue; //This seems to happen with gl_VertexID
                add_program_extra("replay/program/vertex_attrib", newname, 0, loc);
            }
        } else {
            GLint loc = F(glGetAttribLocation)(program, name);
            if (loc == -1) continue; //This seems to happen with gl_VertexID
            add_program_extra("replay/program/vertex_attrib", name, 0, loc);
        }
    }
    
    F(glGetProgramiv)(program, GL_ACTIVE_UNIFORM_BLOCKS, &count);
    F(glGetProgramiv)(program, GL_ACTIVE_UNIFORM_BLOCK_MAX_NAME_LENGTH, &maxNameLen);
    for (size_t i = 0; i < count; i++) {
        GLchar name[maxNameLen+1];
        memset(name, 0, maxNameLen+1);
        F(glGetActiveUniformBlockName)(program, i, maxNameLen+1, NULL, name);
        add_program_extra("replay/program/uniform_block", name, 0, F(glGetUniformBlockIndex)(program, name));
    }
    
    GLint major, minor;
    F(glGetIntegerv)(GL_MAJOR_VERSION, &major);
    F(glGetIntegerv)(GL_MINOR_VERSION, &minor);
    uint ver = major*100 + minor*10;
    
    bool subroutines_supported = ver>=400; //TODO: Test for the extension's support
    if (subroutines_supported) {
        size_t stage_count = 3;
        GLenum stages[6] = {GL_VERTEX_SHADER, GL_GEOMETRY_SHADER, GL_FRAGMENT_SHADER, 0, 0};
        
        if (ver>=400) { //TODO: Test for the extension's support
            stages[stage_count++] = GL_TESS_CONTROL_SHADER;
            stages[stage_count++] = GL_TESS_EVALUATION_SHADER;
        }
        if (ver>=430) stages[stage_count++] = GL_COMPUTE_SHADER; //TODO: Test for the extension's support
        
        for (size_t i = 0; i < stage_count; i++) {
            GLenum stage = stages[i];
            
            F(glGetProgramStageiv)(program, stage, GL_ACTIVE_SUBROUTINES, &count);
            for (GLint j = 0; j < count; j++) {
                GLchar name[maxNameLen+1];
                memset(name, 0, maxNameLen+1);
                F(glGetActiveSubroutineName)(program, stage, j, maxNameLen+1, NULL, name);
                add_program_extra("replay/program/subroutine", name, stage, j);
            }
            
            F(glGetProgramStageiv)(program, stage, GL_ACTIVE_SUBROUTINE_UNIFORM_LOCATIONS, &count);
            for (GLint j = 0; j < count; j++) {
                F(glGetActiveSubroutineUniformiv)(program, stage, j, GL_UNIFORM_NAME_LENGTH, &maxNameLen);
                GLchar name[maxNameLen+1];
                memset(name, 0, maxNameLen+1);
                F(glGetActiveSubroutineUniformName)(program, stage, j, maxNameLen+1, NULL, name);
                add_program_extra("replay/program/subroutine_uniform", name, stage, j);
            }
        }
    }
}

static size_t tex_param_count(GLenum param) {
    switch (param) {
    case GL_DEPTH_STENCIL_TEXTURE_MODE:
    case GL_TEXTURE_COMPARE_FUNC:
    case GL_TEXTURE_COMPARE_MODE:
    case GL_TEXTURE_LOD_BIAS:
    case GL_TEXTURE_MIN_FILTER:
    case GL_TEXTURE_MAG_FILTER:
    case GL_TEXTURE_MIN_LOD:
    case GL_TEXTURE_MAX_LOD:
    case GL_TEXTURE_MAX_LEVEL:
    case GL_TEXTURE_SWIZZLE_R:
    case GL_TEXTURE_SWIZZLE_G:
    case GL_TEXTURE_SWIZZLE_B:
    case GL_TEXTURE_SWIZZLE_A:
    case GL_TEXTURE_WRAP_S:
    case GL_TEXTURE_WRAP_T:
    case GL_TEXTURE_WRAP_R:
    case GL_TEXTURE_BASE_LEVEL: return 1;
    case GL_TEXTURE_SWIZZLE_RGBA:
    case GL_TEXTURE_BORDER_COLOR: return 4;
    default: return 0;
    }
}
