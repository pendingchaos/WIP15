#if __WORDSIZE == 64
#elif __WORDSIZE == 32
#else
#error "Unsupported word size."
#endif

#define WIP15_END 0
#define WIP15_U32_ARRAY 1
#define WIP15_STR 2
#define WIP15_PTR32 3
#define WIP15_PTR64 4
#define WIP15_FUNC_PTR 5
#define WIP15_U8 6
#define WIP15_S8 7
#define WIP15_U16 8
#define WIP15_S16 9
#define WIP15_U32 10
#define WIP15_S32 11
#define WIP15_U64 12
#define WIP15_S64 13
#define WIP15_BOOLEAN 14
#define WIP15_BITFIELD 15
#define WIP15_FLOAT 16
#define WIP15_DOUBLE 17
#define WIP15_RESULT 18
#define WIP15_STR_ARRAY 19
#define WIP15_DATA 20
#define WIP15_DOUBLE_ARRAY 21
#define WIP15_S32_ARRAY 22

typedef struct {
    bool exists;
    int32_t val;
} ival_t;

typedef struct {
    bool exists;
    float val;
} fval_t;

typedef struct {
    bool exists;
    char* str;
} sval_t;

typedef struct {
    bool exists;
    size_t count;
    char** vals;
} sarrval_t;

typedef struct {
    bool exists;
    size_t count;
    int32_t* vals;
} iarrval_t;

typedef struct {
    sval_t shading_language_version;
    sval_t version;
    sval_t renderer;
    sval_t vendor;
    ival_t max_lights;
    ival_t max_clip_planes;
    ival_t max_color_matrix_stack_depth;
    ival_t max_modelview_stack_depth;
    ival_t max_projection_stack_depth;
    ival_t max_texture_stack_depth;
    ival_t subpixel_bits;
    ival_t max_3d_texture_size;
    ival_t max_texture_size;
    ival_t max_texture_lod_bias;
    ival_t max_cube_map_texture_size;
    ival_t max_pixel_map_table;
    ival_t max_name_stack_depth;
    ival_t max_list_nesting;
    ival_t max_eval_order;
    ival_t max_attrib_stack_depth;
    ival_t max_client_attrib_stack_depth;
    iarrval_t max_color_table_size;
    ival_t max_histogram_table_size;
    ival_t aux_buffers;
    ival_t rgba_mode_supported;
    ival_t index_mode_supported;
    ival_t doublebuffer_supported;
    ival_t stereo_supported;
    iarrval_t aliased_point_size_range;
    iarrval_t smooth_point_size_range;
    fval_t smooth_point_size_granularity;
    iarrval_t aliased_line_width_range;
    iarrval_t smooth_line_width_range;
    fval_t smooth_line_width_granularity;
    iarrval_t max_convolution_width;
    iarrval_t max_convolution_height;
    ival_t max_elements_indices;
    ival_t max_elements_vertices;
    ival_t sample_buffers;
    ival_t samples;
    iarrval_t compressed_texture_formats;
    ival_t query_counter_bits;
    sarrval_t extensions;
    ival_t max_texture_units;
    ival_t max_vertex_attribs;
    ival_t max_vertex_uniform_components;
    ival_t max_varying_floats;
    ival_t max_combined_texture_image_units;
    ival_t max_vertex_texture_image_units;
    ival_t max_texture_image_units;
    ival_t max_texture_coords;
    ival_t max_fragment_uniform_components;
    ival_t max_draw_buffers;
    iarrval_t red_bits_supported;
    iarrval_t green_bits_supported;
    iarrval_t blue_bits_supported;
    iarrval_t alpha_bits_supported;
    iarrval_t index_bits_supported;
    iarrval_t depth_bits_supported;
    iarrval_t stencil_bits_supported;
    iarrval_t accum_red_bits_supported;
    iarrval_t accum_green_bits_supported;
    iarrval_t accum_blue_bits_supported;
    iarrval_t accum_alpha_bits_supported;
    char* extensions_str;
} limits_t;

static FILE *trace_file;
static void *lib_gl;
static limits_t gl10_limits;
static limits_t gl11_limits;
static limits_t gl12_limits;
static limits_t gl13_limits;
static limits_t gl14_limits;
static limits_t gl15_limits;
static limits_t gl20_limits;
static limits_t gl21_limits;
static limits_t* current_limits;

static void gl_write_b(uint8_t v) {
    fwrite(&v, 1, 1, trace_file);
}

static void gl_write_u32(uint32_t v) {
    uint32_t le = htole32(v);
    fwrite(&le, 4, 1, trace_file);
}

static void gl_write_str(const char* s) {
    uint32_t length = s == NULL ? 0 : strlen(s);
    uint32_t len_le = htole32(length);
    fwrite(&len_le, 4, 1, trace_file);
    fwrite(s, length, 1, trace_file);
}

static void gl_start(uint32_t func_id) {
    uint32_t func_id_le = htole32(func_id);
    fwrite(&func_id_le, 4, 1, trace_file);
}

static void gl_end() {
    gl_write_b(WIP15_END);
}

static void gl_write_int32(int32_t i) {
    i = htole32(i);
    fwrite(&i, 4, 1, trace_file);
}

static void gl_param_GLuint_array(size_t count, const GLuint* data) {
    gl_write_b(WIP15_U32_ARRAY);
    
    uint32_t count_le = htole32(count);
    fwrite(&count_le, 4, 1, trace_file);
    
    for (size_t i = 0; i < count; ++i)
    {
        uint32_t item = htole32(data[i]);
        fwrite(&item, 4, 1, trace_file);
    }
}

static void gl_param_GLint_array(size_t count, const GLint* data) {
    gl_write_b(WIP15_S32_ARRAY);
    
    uint32_t count_le = htole32(count);
    fwrite(&count_le, 4, 1, trace_file);
    
    for (size_t i = 0; i < count; ++i)
    {
        int32_t item = htole32(data[i]);
        fwrite(&item, 4, 1, trace_file);
    }
}

static void gl_param_GLfloat_array(size_t count, const GLfloat* data) {
    gl_write_b(WIP15_DOUBLE_ARRAY);
    
    uint32_t count_le = htole32(count);
    fwrite(&count_le, 4, 1, trace_file);
    
    for (size_t i = 0; i < count; ++i)
    {
        double val = data[i];
        fwrite(&val, 8, 1, trace_file);
    }
}

static void gl_param_double_array(size_t count, const double* data) {
    gl_write_b(WIP15_DOUBLE_ARRAY);
    
    uint32_t count_le = htole32(count);
    fwrite(&count_le, 4, 1, trace_file);
    
    for (size_t i = 0; i < count; ++i)
    {
        fwrite(data+i, 8, 1, trace_file);
    }
}

static void gl_param_string_array(const GLchar*const* data, size_t count) {
    gl_write_b(WIP15_STR_ARRAY);
    
    uint32_t count_le = htole32(count);
    fwrite(&count_le, 4, 1, trace_file);
    
    for (size_t i = 0; i < count; ++i)
    {
        gl_write_str(data[i]);
    }
}

static void gl_param_string(const char *value) {
    gl_write_b(WIP15_STR);
    gl_write_str(value);
}

static void gl_param_data(size_t size, const void* data) {
    gl_write_b(WIP15_DATA);
    uint32_t size_le = htole32(size);
    fwrite(&size_le, 4, 1, trace_file);
    fwrite(data, size, 1, trace_file);
}

static void gl_param_pointer(const void *value) {
#if __WORDSIZE == 64
    gl_write_b(WIP15_PTR64);
    uint64_t v = htole64((uint64_t)value);
    fwrite(&v, 8, 1, trace_file);
#elif __WORDSIZE == 32
    gl_write_b(WIP15_PTR32);
    uint32_t v = htole32((uint32_t)value);
    fwrite(&v, 4, 1, trace_file);
#endif
}

static void gl_param_GLDEBUGPROC(GLDEBUGPROC proc, int32_t group) {
    gl_write_b(WIP15_FUNC_PTR);
}

static void gl_param_GLsizei(GLsizei value, int32_t group) {
    gl_write_b(WIP15_S32);
    int32_t v = htole32(value);
    fwrite(&v, 4, 1, trace_file);
    gl_write_int32(group);
}

static void gl_param_GLint64EXT(GLint64EXT value, int32_t group) {
    gl_write_b(WIP15_S64);
    int64_t v = htole64(value);
    fwrite(&v, 8, 1, trace_file);
    gl_write_int32(group);
}

static void gl_param_GLshort(GLshort value, int32_t group) {
    gl_write_b(WIP15_S16);
    int16_t v = htole16(value);
    fwrite(&v, 2, 1, trace_file);
    gl_write_int32(group);
}

static void gl_param_int64_t(GLshort value, int32_t group) {
    gl_write_b(WIP15_S64);
    int64_t v = htole64(value);
    fwrite(&v, 8, 1, trace_file);
    gl_write_int32(group);
}

static void gl_param_GLubyte(GLubyte value, int32_t group) {
    gl_write_b(WIP15_U8);
    gl_write_b(value);
    gl_write_int32(group);
}

static void gl_param_GLDEBUGPROCARB(GLDEBUGPROCARB proc, int32_t group) {
    gl_write_b(WIP15_FUNC_PTR);
}

static void gl_param_GLboolean(GLboolean value, int32_t group) {
    gl_write_b(WIP15_BOOLEAN);
    gl_write_b(value ? 1 : 0);
    gl_write_int32(group);
}

static void gl_param_Bool(Bool value, int32_t group) {
    gl_write_b(WIP15_BOOLEAN);
    gl_write_b(value ? 1 : 0);
    gl_write_int32(group);
}

static void gl_param_GLbitfield(GLbitfield value, int32_t group) {
    gl_write_b(WIP15_BITFIELD);
    fwrite(&value, 4, 1, trace_file);
    gl_write_int32(group);
}

static void gl_param_GLsync(GLsync value, int32_t group) {
#if __WORDSIZE == 64
    gl_write_b(WIP15_U64);
    uint64_t v = htole64(value);
    fwrite(&v, 8, 1, trace_file);
#elif __WORDSIZE == 32
    gl_write_b(WIP15_U32);
    uint32_t v = htole32(value);
    fwrite(&v, 4, 1, trace_file);
#endif
    gl_write_int32(group);
}

static void gl_param_GLuint(GLuint value, int32_t group) {
    gl_write_b(WIP15_U32);
    uint32_t v = htole32(value);
    fwrite(&v, 4, 1, trace_file);
    gl_write_int32(group);
}

static void gl_param_GLint64(GLint64 value, int32_t group) {
    gl_write_b(WIP15_S64);
    int64_t v = htole64(value);
    fwrite(&v, 8, 1, trace_file);
    gl_write_int32(group);
}

static void gl_param_int(int value, int32_t group) {
    gl_write_b(WIP15_S32);
    int v = htole32(value);
    fwrite(&v, 4, 1, trace_file);
    gl_write_int32(group);
}

static void gl_param_GLeglImageOES(GLeglImageOES value, int32_t group) {
#if __WORDSIZE == 64
    gl_write_b(WIP15_U64);
    uint64_t v = htole64(value);
    fwrite(&v, 8, 1, trace_file);
#elif __WORDSIZE == 32
    gl_write_b(WIP15_U32);
    uint32_t v = htole32(value);
    fwrite(&v, 4, 1, trace_file);
#endif
    gl_write_int32(group);
}

static void gl_param_GLfixed(GLfixed value, int32_t group) {
    gl_write_b(WIP15_FLOAT);
    float v = value / 65546.0f;
    fwrite(&v, 4, 1, trace_file);
    gl_write_int32(group);
}

static void gl_param_GLclampf(GLclampf value, int32_t group) {
    gl_write_b(WIP15_FLOAT);
    fwrite(&value, 4, 1, trace_file);
    gl_write_int32(group);
}

static void gl_param_float(float value, int32_t group) {
    gl_write_b(WIP15_FLOAT);
    fwrite(&value, 4, 1, trace_file);
    gl_write_int32(group);
}

static void gl_param_GLhalfNV(GLhalfNV value, int32_t group) { //TODO
    gl_write_b(WIP15_U16);
    fwrite(&value, 2, 1, trace_file);
    gl_write_int32(group);
}

static void gl_param_GLintptr(GLintptr value, int32_t group) {
#if __WORDSIZE == 64
    gl_write_b(WIP15_U64);
    uint64_t v = htole64(value);
    fwrite(&v, 8, 1, trace_file);
#elif __WORDSIZE == 32
    gl_write_b(WIP15_U32);
    uint32_t v = htole32(value);
    fwrite(&v, 4, 1, trace_file);
#endif
    gl_write_int32(group);
}

static void gl_param_GLushort(GLushort value, int32_t group) {
    gl_write_b(WIP15_U16);
    uint16_t v = htole16(value);
    fwrite(&v, 2, 1, trace_file);
    gl_write_int32(group);
}

static void gl_param_GLenum(GLenum value, int32_t group) {
    gl_write_b(WIP15_U32);
    uint32_t v = htole32(value);
    fwrite(&v, 4, 1, trace_file);
    gl_write_int32(group);
}

static void gl_param_unsigned_int(unsigned int value, int32_t group) {
    gl_write_b(WIP15_U32);
    uint32_t v = htole32(value);
    fwrite(&v, 4, 1, trace_file);
    gl_write_int32(group);
}

static void gl_param_GLfloat(GLfloat value, int32_t group) {
    gl_write_b(WIP15_FLOAT);
    fwrite(&value, 4, 1, trace_file);
    gl_write_int32(group);
}

static void gl_param_GLuint64(GLuint64 value, int32_t group) {
    gl_write_b(WIP15_U64);
    uint64_t v = htole64(value);
    fwrite(&v, 8, 1, trace_file);
    gl_write_int32(group);
}

static void gl_param_GLdouble(GLdouble value, int32_t group) {
    gl_write_b(WIP15_DOUBLE);
    fwrite(&value, 8, 1, trace_file);
    gl_write_int32(group);
}

static void gl_param_GLhandleARB(GLhandleARB value, int32_t group) {
#ifdef __APPLE__
#if __WORDSIZE == 64
    gl_write_b(WIP15_U64);
    uint64_t v = htole64(value);
    fwrite(&v, 8, 1, trace_file);
#elif __WORDSIZE == 32
    gl_write_b(WIP15_U32);
    uint32_t v = htole32(value);
    fwrite(&v, 4, 1, trace_file);
#endif
#else
    gl_write_b(WIP15_U32);
    uint32_t v = htole32(value);
    fwrite(&v, 4, 1, trace_file);
#endif
    gl_write_int32(group);
}

static void gl_param_GLintptrARB(GLintptrARB value, int32_t group) {
#if __WORDSIZE == 64
    gl_write_b(WIP15_S64);
    uint64_t v = htole64(value);
    fwrite(&v, 8, 1, trace_file);
#elif __WORDSIZE == 32
    gl_write_b(WIP15_S32);
    uint32_t v = htole32(value);
    fwrite(&v, 4, 1, trace_file);
#endif
    gl_write_int32(group);
}

static void gl_param_GLsizeiptr(GLsizeiptr value, int32_t group)
{
    gl_param_GLintptrARB(value, group);
}

static void gl_param_GLint(GLint value, int32_t group)
{
    gl_write_b(WIP15_S32);
    int32_t v = htole32(value);
    fwrite(&v, 4, 1, trace_file);
    gl_write_int32(group);
}

static void gl_param_GLclampx(GLclampx value, int32_t group) {
    gl_write_b(WIP15_S32);
    int32_t v = htole32(value);
    fwrite(&v, 4, 1, trace_file);
    gl_write_int32(group);
}

static void gl_param_GLsizeiptrARB(GLsizeiptrARB value, int32_t group) {
    gl_param_GLsizeiptr(value, group);
}

static void gl_param_GLuint64EXT(GLuint64EXT value, int32_t group) {
    gl_write_b(WIP15_U64);
    uint64_t v = htole64(value);
    fwrite(&v, 8, 1, trace_file);
    gl_write_int32(group);
}

static void gl_param_GLvdpauSurfaceNV(GLvdpauSurfaceNV value, int32_t group) {
    gl_param_GLintptrARB(value, group);
}

static void gl_param_GLbyte(GLbyte value, int32_t group) {
    gl_write_b(WIP15_S8);
    fwrite(&value, 1, 1, trace_file);
    gl_write_int32(group);
}

static void gl_param_GLclampd(GLclampd value, int32_t group) {
    gl_write_b(WIP15_DOUBLE);
    fwrite(&value, 8, 1, trace_file);
    gl_write_int32(group);
}

static void gl_param_GLDEBUGPROCKHR(GLDEBUGPROCKHR value, int32_t group) {
    gl_write_b(WIP15_FUNC_PTR);
    gl_write_int32(group);
}

static void gl_param_GLDEBUGPROCAMD(GLDEBUGPROCAMD value, int32_t group) {
    gl_write_b(WIP15_FUNC_PTR);
    gl_write_int32(group);
}

static void gl_param_GLXPixmap(GLXPixmap value, int32_t group) {
    gl_write_b(WIP15_U32);
    uint32_t v = htole32(value);
    fwrite(&v, 4, 1, trace_file);
    gl_write_int32(group);
}

static void gl_param_GLXWindow(GLXWindow value, int32_t group) {
    gl_write_b(WIP15_U32);
    uint32_t v = htole32(value);
    fwrite(&v, 4, 1, trace_file);
    gl_write_int32(group);
}

static void gl_param_GLXPbuffer(GLXPbuffer value, int32_t group) {
    gl_write_b(WIP15_U32);
    uint32_t v = htole32(value);
    fwrite(&v, 4, 1, trace_file);
    gl_write_int32(group);
}

static void gl_param_GLXDrawable(GLXDrawable value, int32_t group) {
    gl_write_b(WIP15_U32);
    uint32_t v = htole32(value);
    fwrite(&v, 4, 1, trace_file);
    gl_write_int32(group);
}

static void gl_param_GLXVideoDeviceNV(GLXVideoDeviceNV value, int32_t group) {
    gl_write_b(WIP15_U32);
    uint32_t v = htole32(value);
    fwrite(&v, 4, 1, trace_file);
    gl_write_int32(group);
}

static void gl_param_Pixmap(Pixmap value, int32_t group) {
    gl_write_b(WIP15_U32);
    uint32_t v = htole32(value);
    fwrite(&v, 4, 1, trace_file);
    gl_write_int32(group);
}

static void gl_param_Window(Window value, int32_t group) {
    gl_write_b(WIP15_U32);
    uint32_t v = htole32(value);
    fwrite(&v, 4, 1, trace_file);
    gl_write_int32(group);
}

static void gl_param_Font(Font value, int32_t group) {
    gl_write_b(WIP15_U32);
    uint32_t v = htole32(value);
    fwrite(&v, 4, 1, trace_file);
    gl_write_int32(group);
}

static void gl_param_Colormap(Colormap value, int32_t group) {
    gl_write_b(WIP15_U32);
    uint32_t v = htole32(value);
    fwrite(&v, 4, 1, trace_file);
    gl_write_int32(group);
}

static void gl_param_GLXContextID(GLXContextID value, int32_t group) {
    gl_write_b(WIP15_U32);
    uint32_t v = htole32(value);
    fwrite(&v, 4, 1, trace_file);
    gl_write_int32(group);
}

static void gl_param_GLXFBConfig(GLXFBConfig value, int32_t group) {
#if __WORDSIZE == 64
    gl_write_b(WIP15_U64);
    uint64_t v = htole64(value);
    fwrite(&v, 8, 1, trace_file);
#elif __WORDSIZE == 32
    gl_write_b(WIP15_U32);
    uint32_t v = htole32(value);
    fwrite(&v, 4, 1, trace_file);
#endif
    gl_write_int32(group);
}

static void gl_param_GLXVideoCaptureDeviceNV(GLXVideoCaptureDeviceNV value, int32_t group) {
    gl_write_b(WIP15_U32);
    uint32_t v = htole32(value);
    fwrite(&v, 4, 1, trace_file);
    gl_write_int32(group);
}

static void gl_param_GLXFBConfigSGIX(GLXFBConfig value, int32_t group) {
#if __WORDSIZE == 64
    gl_write_b(WIP15_U64);
    uint64_t v = htole64(value);
    fwrite(&v, 8, 1, trace_file);
#elif __WORDSIZE == 32
    gl_write_b(WIP15_U32);
    uint32_t v = htole32(value);
    fwrite(&v, 4, 1, trace_file);
#endif
    gl_write_int32(group);
}

static void gl_param_GLXPbufferSGIX(GLXPbufferSGIX value, int32_t group) {
    gl_write_b(WIP15_U32);
    uint32_t v = htole32(value);
    fwrite(&v, 4, 1, trace_file);
    gl_write_int32(group);
}

static void gl_param_GLXVideoSourceSGIX(GLXVideoSourceSGIX value, int32_t group) {
    gl_write_b(WIP15_U32);
    uint32_t v = htole32(value);
    fwrite(&v, 4, 1, trace_file);
    gl_write_int32(group);
}

static void gl_param_GLXContext(GLXContext value, int32_t group) {
#if __WORDSIZE == 64
    gl_write_b(WIP15_U64);
    uint64_t v = htole64(value);
    fwrite(&v, 8, 1, trace_file);
#elif __WORDSIZE == 32
    gl_write_b(WIP15_U32);
    uint32_t v = htole32(value);
    fwrite(&v, 4, 1, trace_file);
#endif
    gl_write_int32(group);
}

static void gl_result_GLenum(GLenum value) {
    gl_write_b(WIP15_RESULT);
    gl_write_b(WIP15_U32);
    uint32_t v = htole32(value);
    fwrite(&v, 4, 1, trace_file);
    gl_write_str(NULL);
}

static void gl_result_GLintptr(GLintptr value) {
    gl_write_b(WIP15_RESULT);
#if __WORDSIZE == 64
    gl_write_b(WIP15_S64);
    uint64_t v = htole64(value);
    fwrite(&v, 8, 1, trace_file);
#elif __WORDSIZE == 32
    gl_write_b(WIP15_S32);
    uint32_t v = htole32(value);
    fwrite(&v, 4, 1, trace_file);
#endif
    gl_write_str(NULL);
}

static void gl_result_GLint(GLint value) {
    gl_write_b(WIP15_RESULT);
    gl_write_b(WIP15_S32);
    uint32_t v = htole32(value);
    fwrite(&v, 4, 1, trace_file);
    gl_write_str(NULL);
}

static void gl_result_GLboolean(GLboolean value) {
    gl_write_b(WIP15_RESULT);
    gl_write_b(WIP15_BOOLEAN);
    fwrite(&value, 1, 1, trace_file);
    gl_write_str(NULL);
}

static void gl_result_GLbitfield(GLbitfield value) {
    gl_write_b(WIP15_RESULT);
    gl_write_b(WIP15_BITFIELD);
    fwrite(&value, 4, 1, trace_file);
    gl_write_str(NULL);
}

static void gl_result_GLsync(GLsync value) {
    gl_write_b(WIP15_RESULT);
#if __WORDSIZE == 64
    gl_write_b(WIP15_U64);
    uint64_t v = htole64(value);
    fwrite(&v, 8, 1, trace_file);
#elif __WORDSIZE == 32
    gl_write_b(WIP15_U32);
    uint32_t v = htole32(value);
    fwrite(&v, 4, 1, trace_file);
#endif
    gl_write_str(NULL);
}

static void gl_result_GLuint64(GLuint64 value) {
    gl_write_b(WIP15_RESULT);
    gl_write_b(WIP15_U64);
    uint64_t v = htole64(value);
    fwrite(&v, 8, 1, trace_file);
    gl_write_str(NULL);
}

static void gl_result_GLvdpauSurfaceNV(GLvdpauSurfaceNV value) {
    gl_write_b(WIP15_RESULT);
#if __WORDSIZE == 64
    gl_write_b(WIP15_S64);
    uint64_t v = htole64(value);
    fwrite(&v, 8, 1, trace_file);
#elif __WORDSIZE == 32
    gl_write_b(WIP15_S32);
    uint32_t v = htole32(value);
    fwrite(&v, 4, 1, trace_file);
#endif
    gl_write_str(NULL);
}

static void gl_result_GLfloat(GLfloat value) {
    gl_write_b(WIP15_RESULT);
    gl_write_b(WIP15_FLOAT);
    fwrite(&value, 4, 1, trace_file);
    gl_write_str(NULL);
}

static void gl_result_GLuint(GLuint value) {
    gl_write_b(WIP15_RESULT);
    gl_write_b(WIP15_U32);
    uint32_t v = htole32(value);
    fwrite(&v, 4, 1, trace_file);
    gl_write_str(NULL);
}

static void gl_result_int64_t(GLuint value) {
    gl_write_b(WIP15_RESULT);
    gl_write_b(WIP15_S64);
    int64_t v = htole64(value);
    fwrite(&v, 8, 1, trace_file);
    gl_write_str(NULL);
}

static void gl_result_unsigned_int(unsigned int value) {
    gl_write_b(WIP15_RESULT);
    gl_write_b(WIP15_U32);
    uint32_t v = htole32(value);
    fwrite(&v, 4, 1, trace_file);
    gl_write_str(NULL);
}

static void gl_result_GLhandleARB(GLhandleARB value) {
    gl_write_b(WIP15_RESULT);
#ifdef __APPLE__
#if __WORDSIZE == 64
    gl_write_b(WIP15_U64);
    uint64_t v = htole64(value);
    fwrite(&v, 8, 1, trace_file);
#elif __WORDSIZE == 32
    gl_write_b(WIP15_U32);
    uint32_t v = htole32(value);
    fwrite(&v, 4, 1, trace_file);
#endif
#else
    gl_write_b(WIP15_U32);
    uint32_t v = htole32(value);
    fwrite(&v, 4, 1, trace_file);
#endif
    gl_write_str(NULL);
}

static void gl_result_GLushort(GLushort value) {
    gl_write_b(WIP15_RESULT);
    gl_write_b(WIP15_U16);
    uint16_t v = htole16(value);
    fwrite(&v, 2, 1, trace_file);
    gl_write_str(NULL);
}

static void gl_result_int(int value) {
    gl_write_b(WIP15_RESULT);
    gl_write_b(WIP15_S32);
    int v = htole32(value);
    fwrite(&v, 4, 1, trace_file);
    gl_write_str(NULL);
}

static void gl_result_Bool(int value) {
    gl_write_b(WIP15_RESULT);
    gl_write_b(WIP15_BOOLEAN);
    uint8_t v = value ? 1 : 0;
    fwrite(&v, 1, 1, trace_file);
    gl_write_str(NULL);
}

static void gl_result_pointer(void *value) {
    gl_write_b(WIP15_RESULT);
#if __WORDSIZE == 64
    gl_write_b(WIP15_PTR64);
    uint64_t v = htole64((uint64_t)value);
    fwrite(&v, 8, 1, trace_file);
#elif __WORDSIZE == 32
    gl_write_b(WIP15_PTR32);
    uint32_t v = htole32((uint32_t)value);
    fwrite(&v, 4, 1, trace_file);
#endif
}

static void gl_result_GLXContext(GLXContext value) {
    gl_write_b(WIP15_RESULT);
#if __WORDSIZE == 64
    gl_write_b(WIP15_S64);
    uint64_t v = htole64(value);
    fwrite(&v, 8, 1, trace_file);
#elif __WORDSIZE == 32
    gl_write_b(WIP15_S32);
    uint32_t v = htole32(value);
    fwrite(&v, 4, 1, trace_file);
#endif
    gl_write_str(NULL);
}

static void gl_result_GLXPixmap(GLXPixmap value) {
    gl_write_b(WIP15_RESULT);
    gl_write_b(WIP15_U32);
    uint32_t v = htole32(value);
    fwrite(&v, 4, 1, trace_file);
    gl_write_str(NULL);
}

static void gl_result_GLXWindow(GLXWindow value) {
    gl_write_b(WIP15_RESULT);
    gl_write_b(WIP15_U32);
    uint32_t v = htole32(value);
    fwrite(&v, 4, 1, trace_file);
    gl_write_str(NULL);
}

static void gl_result_GLXPbuffer(GLXPbuffer value) {
    gl_write_b(WIP15_RESULT);
    gl_write_b(WIP15_U32);
    uint32_t v = htole32(value);
    fwrite(&v, 4, 1, trace_file);
    gl_write_str(NULL);
}

static void gl_result_GLXDrawable(GLXDrawable value) {
    gl_write_b(WIP15_RESULT);
    gl_write_b(WIP15_U32);
    uint32_t v = htole32(value);
    fwrite(&v, 4, 1, trace_file);
    gl_write_str(NULL);
}

static void gl_result_GLXContextID(GLXContextID value) {
    gl_write_b(WIP15_RESULT);
    gl_write_b(WIP15_U32);
    uint32_t v = htole32(value);
    fwrite(&v, 4, 1, trace_file);
    gl_write_str(NULL);
}

static void gl_result___GLXextFuncPtr(__GLXextFuncPtr value) {
    gl_write_b(WIP15_RESULT);
    gl_write_b(WIP15_FUNC_PTR);
    gl_write_str(NULL);
}

static void gl_result_Status(Status value) {
    gl_write_b(WIP15_RESULT);
    gl_write_b(WIP15_S32);
    uint32_t v = htole32(value);
    fwrite(&v, 4, 1, trace_file);
    gl_write_str(NULL);
}

static void gl_result_GLXFBConfigSGIX(GLXFBConfigSGIX value) {
    gl_write_b(WIP15_RESULT);
#if __WORDSIZE == 64
    gl_write_b(WIP15_S64);
    uint64_t v = htole64(value);
    fwrite(&v, 8, 1, trace_file);
#elif __WORDSIZE == 32
    gl_write_b(WIP15_S32);
    uint32_t v = htole32(value);
    fwrite(&v, 4, 1, trace_file);
#endif
    gl_write_str(NULL);
}

static void gl_result_GLXVideoCaptureDeviceNV(GLXVideoCaptureDeviceNV value) {
    gl_write_b(WIP15_RESULT);
    gl_write_b(WIP15_U32);
    uint32_t v = htole32(value);
    fwrite(&v, 4, 1, trace_file);
    gl_write_str(NULL);
}

static void gl_result_GLXPbufferSGIX(GLXPbufferSGIX value) {
    gl_write_b(WIP15_RESULT);
    gl_write_b(WIP15_U32);
    uint32_t v = htole32(value);
    fwrite(&v, 4, 1, trace_file);
    gl_write_str(NULL);
}

static void* actual_dlopen(const char* filename, int flags) {
    return ((void* (*)(const char*, int))dlsym(RTLD_NEXT, "dlopen"))(filename, flags);
}

void* dlopen(const char* filename, int flags) {
    if (!filename)
        return actual_dlopen(filename, flags);
    
    if (strcmp(filename, "libGL.so") == 0 ||
        strcmp(filename, "libGL.so.1") == 0)
        return actual_dlopen(NULL, flags);
    
    return actual_dlopen(filename, flags);
}

static void init_limits(limits_t* limits) {
    memset(limits, 0, sizeof(limits_t));
}

static void set_int_limit(ival_t* dest, const char* str) {
    dest->exists = true;
    sscanf(str, "%"PRIi32, &dest->val);
}

static void set_float_limit(fval_t* dest, const char* str) {
    dest->exists = true;
    sscanf(str, "%f", &dest->val);
}

static void set_str_limit(sval_t* dest, const char* str) {
    dest->exists = true;
    
    while ((*str == ' ') && *str)
        str++;
    
    const char* c = str;
    const char* end = c;
    while (*c) {
        if (*c != ' ')
            end = c;
        c++;
    }
    
    size_t len = end - str + 1;
    
    dest->str = malloc(len+1);
    memcpy(dest->str, str, len);
    dest->str[len] = 0;
}

static void set_str_array_limit(sarrval_t* dest, const char* str) {
    dest->exists = true;
    
    bool empty = true;
    const char* c = str;
    while (*c) {
        empty = empty && (*c == ' ');
        c++;
    }
    
    if (empty) {
        dest->count = 0;
    } else {
        dest->count = 1;
        c = str;
        while (*c) dest->count += (*c++) == ',' ? 1 : 0;
    }
    
    dest->vals = malloc(sizeof(char*)*dest->count);
    char** cs = dest->vals;
    char element_str[256];
    memset(element_str, 0, 256);
    char* cur = element_str;
    while (*str) {
        if (*str == ' ') {
        } else if (*str == ',') {
            size_t len = strlen(element_str);
            *cs = malloc(len+1);
            memcpy(*cs, element_str, len);
            cs[len] = 0;
            cs++;
            
            memset(element_str, 0, 256);
            cur = element_str;
        } else {
            *cur = *str;
            cur++;
        }
        str++;
    }
    
    size_t len = strlen(element_str);
    *cs = malloc(len+1);
    memcpy(*cs, element_str, len);
    cs[len] = 0;
}

static void set_int_array_limit(iarrval_t* dest, const char* str) {
    dest->exists = true;
    
    bool empty = true;
    const char* c = str;
    while (*c) {
        empty = empty && (*c == ' ');
        c++;
    }
    
    if (empty) {
        dest->count = 0;
    } else {
        dest->count = 1;
        c = str;
        while (*c) dest->count += (*c++) == ',' ? 1 : 0;
    }
    
    dest->vals = malloc(sizeof(int32_t)*dest->count);
    int32_t* ci = dest->vals;
    char element_str[256];
    memset(element_str, 0, 256);
    char* cur = element_str;
    while (*str) {
        if (*str == ' ') {
        } else if (*str == ',') {
            sscanf(element_str, "%"PRIi32, ci);
            ci++;
            memset(element_str, 0, 256);
            cur = element_str;
        } else {
            *cur = *str;
            cur++;
        }
        str++;
    }
    
    sscanf(element_str, "%"PRIi32, ci);
}

static void set_limit_value(limits_t* limits, const char* name, const char* value) {
    #define INT_LIMIT(limit) else if (strcmp(name, #limit) == 0) set_int_limit(&limits->limit, value);
    #define INT_ARR_LIMIT(limit) else if (strcmp(name, #limit) == 0) set_int_array_limit(&limits->limit, value);
    #define FLOAT_LIMIT(limit) else if (strcmp(name, #limit) == 0) set_float_limit(&limits->limit, value);
    
    if (strcmp(name, "shading_language_version") == 0)
        set_str_limit(&limits->shading_language_version, value);
    else if (strcmp(name, "version") == 0)
        set_str_limit(&limits->version, value);
    else if (strcmp(name, "renderer") == 0)
        set_str_limit(&limits->renderer, value);
    else if (strcmp(name, "vendor") == 0)
        set_str_limit(&limits->vendor, value);
    else if (strcmp(name, "extensions") == 0)
        set_str_array_limit(&limits->extensions, value);
    INT_LIMIT(max_lights)
    INT_LIMIT(max_clip_planes)
    INT_LIMIT(max_color_matrix_stack_depth)
    INT_LIMIT(max_modelview_stack_depth)
    INT_LIMIT(max_projection_stack_depth)
    INT_LIMIT(max_texture_stack_depth)
    INT_LIMIT(subpixel_bits)
    INT_LIMIT(max_3d_texture_size)
    INT_LIMIT(max_texture_size)
    INT_LIMIT(max_texture_lod_bias)
    INT_LIMIT(max_cube_map_texture_size)
    INT_LIMIT(max_pixel_map_table)
    INT_LIMIT(max_name_stack_depth)
    INT_LIMIT(max_list_nesting)
    INT_LIMIT(max_eval_order)
    INT_LIMIT(max_attrib_stack_depth)
    INT_LIMIT(max_client_attrib_stack_depth)
    INT_ARR_LIMIT(max_color_table_size)
    INT_LIMIT(max_histogram_table_size)
    INT_LIMIT(aux_buffers)
    INT_LIMIT(rgba_mode_supported)
    INT_LIMIT(index_mode_supported)
    INT_LIMIT(doublebuffer_supported)
    INT_LIMIT(stereo_supported)
    INT_ARR_LIMIT(aliased_point_size_range)
    INT_ARR_LIMIT(smooth_point_size_range)
    FLOAT_LIMIT(smooth_point_size_granularity)
    INT_ARR_LIMIT(aliased_line_width_range)
    INT_ARR_LIMIT(smooth_line_width_range)
    FLOAT_LIMIT(smooth_line_width_granularity)
    INT_ARR_LIMIT(max_convolution_width)
    INT_ARR_LIMIT(max_convolution_height)
    INT_LIMIT(max_elements_indices)
    INT_LIMIT(max_elements_vertices)
    INT_LIMIT(sample_buffers)
    INT_LIMIT(samples)
    INT_ARR_LIMIT(compressed_texture_formats)
    INT_LIMIT(query_counter_bits)
    INT_LIMIT(max_texture_units)
    INT_LIMIT(max_vertex_attribs)
    INT_LIMIT(max_vertex_uniform_components)
    INT_LIMIT(max_varying_floats)
    INT_LIMIT(max_combined_texture_image_units)
    INT_LIMIT(max_vertex_texture_image_units)
    INT_LIMIT(max_texture_image_units)
    INT_LIMIT(max_texture_coords)
    INT_LIMIT(max_fragment_uniform_components)
    INT_LIMIT(max_draw_buffers)
    INT_ARR_LIMIT(red_bits_supported)
    INT_ARR_LIMIT(green_bits_supported)
    INT_ARR_LIMIT(blue_bits_supported)
    INT_ARR_LIMIT(alpha_bits_supported)
    INT_ARR_LIMIT(index_bits_supported)
    INT_ARR_LIMIT(depth_bits_supported)
    INT_ARR_LIMIT(stencil_bits_supported)
    INT_ARR_LIMIT(accum_red_bits_supported)
    INT_ARR_LIMIT(accum_green_bits_supported)
    INT_ARR_LIMIT(accum_blue_bits_supported)
    INT_ARR_LIMIT(accum_alpha_bits_supported)
    else
        fprintf(stderr, "Unknown limit \"%s\".\n", name);
}

static void create_extensions_str(limits_t* limits) {
    if (!limits->extensions.exists)
        return;
    
    size_t len = 0;
    for (size_t i = 0; i < limits->extensions.count; i++)
        len += strlen(limits->extensions.vals[i]) + 1;
    limits->extensions_str = malloc(len);
    len -= 1;
    
    char* c = limits->extensions_str;
    for (size_t i = 0; i < limits->extensions.count; i++) {
        char* ext = limits->extensions.vals[i];
        size_t ext_len = strlen(ext);
        memcpy(c, ext, ext_len);
        c += ext_len;
        
        if (i != limits->extensions.count-1)
            *(c++) = ' ';
    }
    c[len] = 0;
}

static void handle_limits() {
    char *filename = getenv("WIP15_LIMITS");
    if (!filename) {
        fprintf(stderr, "WIP15_LIMITS environment variable (the filename of the limits file) has not been set.\n");
        exit(1);
    }
    
    FILE* file = fopen(filename, "r");
    
    init_limits(&gl10_limits);
    init_limits(&gl11_limits);
    init_limits(&gl12_limits);
    init_limits(&gl13_limits);
    init_limits(&gl14_limits);
    init_limits(&gl15_limits);
    init_limits(&gl20_limits);
    init_limits(&gl21_limits);
    
    bool gl10 = false;
    bool gl11 = false;
    bool gl12 = false;
    bool gl13 = false;
    bool gl14 = false;
    bool gl15 = false;
    bool gl20 = false;
    bool gl21 = false;
    
    while (true) {
        int c = fgetc(file);
        if (c == EOF) {
            break;
        } else if (c == '@') {
            gl10 = false;
            gl11 = false;
            gl12 = false;
            gl13 = false;
            gl14 = false;
            gl15 = false;
            gl20 = false;
            gl21 = false;
            
            #define UPDATE_VERSION int major, minor;\
            if (sscanf(version, "%d.%d", &major, &minor) != 2) {\
                fprintf(stderr, "Invalid version \"%s\".\n", version);\
                exit(1);\
            }\
            if ((major == 1) && (minor == 0))\
                gl10 = true;\
            else if ((major == 1) && (minor == 1))\
                gl11 = true;\
            else if ((major == 1) && (minor == 2))\
                gl12 = true;\
            else if ((major == 1) && (minor == 3))\
                gl13 = true;\
            else if ((major == 1) && (minor == 4))\
                gl14 = true;\
            else if ((major == 1) && (minor == 5))\
                gl15 = true;\
            else if ((major == 2) && (minor == 0))\
                gl20 = true;\
            else if ((major == 2) && (minor == 1))\
                gl21 = true;\
            else\
                fprintf(stderr, "Unknown version \"%s\".\n", version);
            
            char version[256];
            memset(version, 0, 256);
            size_t i = 0;
            while (i < 255) {
                c = fgetc(file);
                if (c == EOF) {
                    break;
                } else if (c == '\n') {
                    break;
                } else if (c == ',') {
                    UPDATE_VERSION
                    memset(version, 0, 256);
                    i = 0;
                } else if (c != ' ') {
                    version[i++] = c;
                }
            }
            
            UPDATE_VERSION
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
            
            if (gl10) set_limit_value(&gl10_limits, name, value);
            if (gl11) set_limit_value(&gl11_limits, name, value);
            if (gl12) set_limit_value(&gl12_limits, name, value);
            if (gl13) set_limit_value(&gl13_limits, name, value);
            if (gl14) set_limit_value(&gl14_limits, name, value);
            if (gl15) set_limit_value(&gl15_limits, name, value);
            if (gl20) set_limit_value(&gl20_limits, name, value);
            if (gl21) set_limit_value(&gl21_limits, name, value);
        }
    }
    
    fclose(file);
    
    create_extensions_str(&gl10_limits);
    create_extensions_str(&gl11_limits);
    create_extensions_str(&gl12_limits);
    create_extensions_str(&gl13_limits);
    create_extensions_str(&gl14_limits);
    create_extensions_str(&gl15_limits);
    create_extensions_str(&gl20_limits);
    create_extensions_str(&gl21_limits);
}

static void free_limits(limits_t* limits) {
    free(limits->shading_language_version.str);
    free(limits->version.str);
    free(limits->renderer.str);
    free(limits->vendor.str);
    free(limits->max_color_table_size.vals);
    free(limits->aliased_point_size_range.vals);
    free(limits->smooth_point_size_range.vals);
    free(limits->aliased_line_width_range.vals);
    free(limits->smooth_line_width_range.vals);
    free(limits->max_convolution_width.vals);
    free(limits->max_convolution_height.vals);
    free(limits->compressed_texture_formats.vals);
    
    for (size_t i = 0; i < limits->extensions.count; i++)
        free(limits->extensions.vals[i]);
    free(limits->extensions.vals);
    
    free(limits->red_bits_supported.vals);
    free(limits->green_bits_supported.vals);
    free(limits->blue_bits_supported.vals);
    free(limits->alpha_bits_supported.vals);
    free(limits->index_bits_supported.vals);
    free(limits->depth_bits_supported.vals);
    free(limits->stencil_bits_supported.vals);
    free(limits->accum_red_bits_supported.vals);
    free(limits->accum_green_bits_supported.vals);
    free(limits->accum_blue_bits_supported.vals);
    free(limits->accum_alpha_bits_supported.vals);
    free(limits->extensions_str);
}

#define GL_GETI(get, pname, name) case pname: {\
    if (current_limits->name.exists)\
        *data = current_limits->name.val;\
    else\
        F(get)(pname, data);\
    break;\
}

//TODO: GL_ALIASED_POINT_SIZE_RANGE
//TODO: GL_SMOOTH_POINT_SIZE_RANGE
//TODO: GL_SMOOTH_POINT_SIZE_GRANULARITY
//TODO: GL_ALIASED_LINE_WIDTH_RANGE
//TODO: GL_SMOOTH_LINE_WIDTH_RANGE
//TODO: GL_SMOOTH_LINE_WIDTH_GRANULARITY
//TODO: GL_MAX_CONVOLUTION_WIDTH
//TODO: GL_MAX_CONVOLUTION_HEIGHT
//TODO: GL_MAX_COLOR_TABLE_SIZE
//TODO: GL_MAX_HISTOGRAM_TABLE_SIZE
//TODO: GL_COMPRESSED_TEXTURE_FORMATS and GL_NUM_COMPRESSED_TEXTURE_FORMATS
#define GL_GET(get) switch (pname) {\
    GL_GETI(get, GL_MAX_LIGHTS, max_lights)\
    GL_GETI(get, GL_MAX_CLIP_PLANES, max_clip_planes)\
    GL_GETI(get, GL_MAX_COLOR_MATRIX_STACK_DEPTH, max_color_matrix_stack_depth)\
    GL_GETI(get, GL_MAX_MODELVIEW_STACK_DEPTH, max_modelview_stack_depth)\
    GL_GETI(get, GL_MAX_PROJECTION_STACK_DEPTH, max_projection_stack_depth)\
    GL_GETI(get, GL_MAX_TEXTURE_STACK_DEPTH, max_texture_stack_depth)\
    GL_GETI(get, GL_SUBPIXEL_BITS, subpixel_bits)\
    GL_GETI(get, GL_MAX_3D_TEXTURE_SIZE, max_3d_texture_size)\
    GL_GETI(get, GL_MAX_TEXTURE_SIZE, max_texture_size)\
    GL_GETI(get, GL_MAX_TEXTURE_LOD_BIAS, max_texture_lod_bias)\
    GL_GETI(get, GL_MAX_CUBE_MAP_TEXTURE_SIZE, max_cube_map_texture_size)\
    GL_GETI(get, GL_MAX_PIXEL_MAP_TABLE, max_pixel_map_table)\
    GL_GETI(get, GL_MAX_NAME_STACK_DEPTH, max_name_stack_depth)\
    GL_GETI(get, GL_MAX_LIST_NESTING, max_list_nesting)\
    GL_GETI(get, GL_MAX_EVAL_ORDER, max_eval_order)\
    GL_GETI(get, GL_MAX_ATTRIB_STACK_DEPTH, max_attrib_stack_depth)\
    GL_GETI(get, GL_MAX_CLIENT_ATTRIB_STACK_DEPTH, max_client_attrib_stack_depth)\
    GL_GETI(get, GL_MAX_ELEMENTS_INDICES, max_elements_indices)\
    GL_GETI(get, GL_MAX_ELEMENTS_VERTICES, max_elements_vertices)\
    GL_GETI(get, GL_QUERY_COUNTER_BITS, query_counter_bits)\
    GL_GETI(get, GL_MAX_TEXTURE_UNITS, max_texture_units)\
    GL_GETI(get, GL_MAX_VERTEX_ATTRIBS, max_vertex_attribs)\
    GL_GETI(get, GL_MAX_VERTEX_UNIFORM_COMPONENTS, max_vertex_uniform_components)\
    GL_GETI(get, GL_MAX_VARYING_FLOATS, max_varying_floats)\
    GL_GETI(get, GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS, max_combined_texture_image_units)\
    GL_GETI(get, GL_MAX_VERTEX_TEXTURE_IMAGE_UNITS, max_vertex_texture_image_units)\
    GL_GETI(get, GL_MAX_TEXTURE_IMAGE_UNITS, max_texture_image_units)\
    GL_GETI(get, GL_MAX_TEXTURE_COORDS, max_texture_coords)\
    GL_GETI(get, GL_MAX_FRAGMENT_UNIFORM_COMPONENTS, max_fragment_uniform_components)\
    GL_GETI(get, GL_MAX_DRAW_BUFFERS, max_draw_buffers)\
    default: {\
        F(get)(pname, data);\
        break;\
    }\
    }

GLint* gl_geti(GLenum pname) {
    static GLint data[16];
    GL_GET(glGetIntegerv);
    return data;
}

GLfloat* gl_getf(GLenum pname) {
    static GLfloat data[16];
    GL_GET(glGetFloatv);
    return data;
}

GLboolean* gl_getb(GLenum pname) {
    static GLboolean data[16];
    GL_GET(glGetBooleanv);
    return data;
}

void __attribute__ ((destructor)) gl_deinit() {
    fclose(trace_file);
    dlclose(lib_gl);
    
    free_limits(&gl10_limits);
    free_limits(&gl11_limits);
    free_limits(&gl12_limits);
    free_limits(&gl13_limits);
    free_limits(&gl14_limits);
    free_limits(&gl15_limits);
    free_limits(&gl20_limits);
    free_limits(&gl21_limits);
}

void glSetContextCapsWIP15();
void glMappedBufferDataWIP15(GLenum target, GLsizei size, const GLvoid* data);
void glProgramUniformWIP15(GLuint program, const GLchar* name, GLuint location);
void glProgramAttribWIP15(GLuint program, const GLchar* name, GLuint index);
void glBeginClientArrayDataWIP15(GLsizei primcount);
void glClientVertexDataWIP15(GLuint prim, GLsizei size, const GLvoid* data);
void glClientColorDataWIP15(GLuint prim, GLsizei size, const GLvoid* data);
void glClientEdgeFlagDataWIP15(GLuint prim, GLsizei size, const GLvoid* data);
void glClientFogCoordDataWIP15(GLuint prim, GLsizei size, const GLvoid* data);
void glClientIndexDataWIP15(GLuint prim, GLsizei size, const GLvoid* data);
void glClientNormalDataWIP15(GLuint prim, GLsizei size, const GLvoid* data);
void glClientSecondaryColorDataWIP15(GLuint prim, GLsizei size, const GLvoid* data);
void glClientTexCoordDataWIP15(GLuint prim, GLsizei size, const GLvoid* data);

GLboolean glUnmapBuffer(GLenum target) {
    GLint access;
    F(glGetBufferParameteriv)(target, GL_BUFFER_ACCESS, &access);
    
    GLboolean result = F(glUnmapBuffer)(target);
    
    if (access != GL_READ_ONLY) {
        GLint size;
        F(glGetBufferParameteriv)(target, GL_BUFFER_SIZE, &size);
        
        void* data = malloc(size);
        F(glGetBufferSubData)(target, 0, size, data);
        glMappedBufferDataWIP15(target, size, data);
        free(data);
    }
    
    gl_start(FUNC_glUnmapBuffer);
    gl_param_GLenum(target, -1);
    gl_result_GLboolean(result);
    gl_end();
    
    return result;
}

void glLinkProgram(GLuint program) {
    gl_start(FUNC_glLinkProgram);
    gl_param_GLuint(program, -1);
    F(glLinkProgram)(program);
    gl_end();
    
    GLint count;
    F(glGetProgramiv)(program, GL_ACTIVE_UNIFORMS, &count);
    
    GLint maxNameLen;
    F(glGetProgramiv)(program, GL_ACTIVE_UNIFORM_MAX_LENGTH, &maxNameLen);
    
    for (size_t i = 0; i < count; i++) {
        GLchar name[maxNameLen+1];
        memset(name, 0, maxNameLen+1);
        
        GLint size;
        GLenum type;
        
        F(glGetActiveUniform)(program, i, maxNameLen, NULL, &size, &type, name);
        
        if (!strncmp(name, "gl_", 3))
            continue;
        
        if (size == 1) {
            glProgramUniformWIP15(program, name, F(glGetUniformLocation)(program, name));
        } else {
            for (size_t j = 0; j < size; j++) {
                GLchar new_name[maxNameLen+1];
                memset(new_name, 0, maxNameLen+1);
                snprintf(new_name, maxNameLen+1, "%s[%zu]", name, j);
                
                glProgramUniformWIP15(program, new_name, F(glGetUniformLocation)(program, name));
            }
        }
    }
    
    F(glGetProgramiv)(program, GL_ACTIVE_ATTRIBUTES, &count);
    F(glGetProgramiv)(program, GL_ACTIVE_ATTRIBUTE_MAX_LENGTH, &maxNameLen);
    
    for (size_t i = 0; i < count; i++) {
        GLchar name[maxNameLen+1];
        memset(name, 0, maxNameLen+1);
        
        GLint size;
        GLenum type;
        
        F(glGetActiveAttrib)(program, i, maxNameLen, NULL, &size, &type, name);
        
        if (!strncmp(name, "gl_", 3))
            continue;
        
        if (size == 1) {
            glProgramAttribWIP15(program, name, F(glGetAttribLocation)(program, name));
        } else {
            for (size_t j = 0; j < size; j++) {
                GLchar new_name[maxNameLen+1];
                memset(new_name, 0, maxNameLen+1);
                snprintf(new_name, maxNameLen+1, "%s[%zu]", name, j);
                
                glProgramAttribWIP15(program, new_name, F(glGetAttribLocation)(program, name));
            }
        }
    }
}

static GLint get_vertex_size(GLint count, GLint type, GLint stride) {
    if (stride)
        return stride;
    else
        switch (type) {
        case GL_BYTE:
        case GL_UNSIGNED_BYTE:
            return count;
        case GL_SHORT:
        case GL_UNSIGNED_SHORT:
            return count * 2;
        case GL_INT:
        case GL_UNSIGNED_INT:
        case GL_FLOAT:
            return count * 4;
        case GL_DOUBLE:
            return count * 8;
        }
}

void do_arrays(GLuint prim, GLint vertex_count) {
    GLint buf;
    F(glGetIntegerv)(GL_VERTEX_ARRAY_BUFFER_BINDING, &buf);
    if (!buf && F(glIsEnabled)(GL_VERTEX_ARRAY)) {
        GLint count;
        GLint type;
        GLint stride;
        F(glGetIntegerv)(GL_VERTEX_ARRAY_SIZE, &count);
        F(glGetIntegerv)(GL_VERTEX_ARRAY_TYPE, &type);
        F(glGetIntegerv)(GL_VERTEX_ARRAY_STRIDE, &stride);
        
        GLint size = get_vertex_size(count, type, stride) * vertex_count;
        GLvoid* data;
        F(glGetPointerv)(GL_VERTEX_ARRAY_POINTER, &data);
        
        glClientVertexDataWIP15(prim, size, data);
    }
    
    F(glGetIntegerv)(GL_COLOR_ARRAY_BUFFER_BINDING, &buf);
    if (!buf && F(glIsEnabled)(GL_COLOR_ARRAY)) {
        GLint count;
        GLint type;
        GLint stride;
        F(glGetIntegerv)(GL_COLOR_ARRAY_SIZE, &count);
        F(glGetIntegerv)(GL_COLOR_ARRAY_TYPE, &type);
        F(glGetIntegerv)(GL_COLOR_ARRAY_STRIDE, &stride);
        
        GLint size = get_vertex_size(count, type, stride) * vertex_count;
        GLvoid* data;
        F(glGetPointerv)(GL_COLOR_ARRAY_POINTER, &data);
        
        glClientColorDataWIP15(prim, size, data);
    }
    
    F(glGetIntegerv)(GL_EDGE_FLAG_ARRAY_BUFFER_BINDING, &buf);
    if (!buf && F(glIsEnabled)(GL_EDGE_FLAG_ARRAY)) {
        GLint stride;
        F(glGetIntegerv)(GL_EDGE_FLAG_ARRAY_STRIDE, &stride);
        
        GLint size = sizeof(GLboolean) * vertex_count;
        GLvoid* data;
        F(glGetPointerv)(GL_EDGE_FLAG_ARRAY_POINTER, &data);
        
        glClientEdgeFlagDataWIP15(prim, size, data);
    }
    
    F(glGetIntegerv)(GL_FOG_COORD_ARRAY_BUFFER_BINDING, &buf);
    if (!buf && F(glIsEnabled)(GL_FOG_COORD_ARRAY)) {
        GLint type;
        GLint stride;
        F(glGetIntegerv)(GL_FOG_COORD_ARRAY_TYPE, &type);
        F(glGetIntegerv)(GL_FOG_COORD_ARRAY_STRIDE, &stride);
        
        GLint size = get_vertex_size(1, type, stride) * vertex_count;
        GLvoid* data;
        F(glGetPointerv)(GL_FOG_COORD_ARRAY_POINTER, &data);
        
        glClientFogCoordDataWIP15(prim, size, data);
    }
    
    F(glGetIntegerv)(GL_INDEX_ARRAY_BUFFER_BINDING, &buf);
    if (!buf && F(glIsEnabled)(GL_INDEX_ARRAY)) {
        GLint type;
        GLint stride;
        F(glGetIntegerv)(GL_INDEX_ARRAY_TYPE, &type);
        F(glGetIntegerv)(GL_INDEX_ARRAY_STRIDE, &stride);
        
        GLint size = get_vertex_size(1, type, stride) * vertex_count;
        GLvoid* data;
        F(glGetPointerv)(GL_INDEX_ARRAY_POINTER, &data);
        
        glClientIndexDataWIP15(prim, size, data);
    }
    
    F(glGetIntegerv)(GL_NORMAL_ARRAY_BUFFER_BINDING, &buf);
    if (!buf && F(glIsEnabled)(GL_NORMAL_ARRAY)) {
        GLint type;
        GLint stride;
        F(glGetIntegerv)(GL_NORMAL_ARRAY_TYPE, &type);
        F(glGetIntegerv)(GL_NORMAL_ARRAY_STRIDE, &stride);
        
        GLint size = get_vertex_size(3, type, stride) * vertex_count;
        GLvoid* data;
        F(glGetPointerv)(GL_NORMAL_ARRAY_POINTER, &data);
        
        glClientNormalDataWIP15(prim, size, data);
    }
    
    F(glGetIntegerv)(GL_SECONDARY_COLOR_ARRAY_BUFFER_BINDING, &buf);
    if (!buf && F(glIsEnabled)(GL_SECONDARY_COLOR_ARRAY)) {
        GLint count;
        GLint type;
        GLint stride;
        F(glGetIntegerv)(GL_SECONDARY_COLOR_ARRAY_SIZE, &count);
        F(glGetIntegerv)(GL_SECONDARY_COLOR_ARRAY_TYPE, &type);
        F(glGetIntegerv)(GL_SECONDARY_COLOR_ARRAY_STRIDE, &stride);
        
        GLint size = get_vertex_size(count, type, stride) * vertex_count;
        GLvoid* data;
        F(glGetPointerv)(GL_SECONDARY_COLOR_ARRAY_POINTER, &data);
        
        glClientSecondaryColorDataWIP15(prim, size, data);
    }
    
    F(glGetIntegerv)(GL_TEXTURE_COORD_ARRAY_BUFFER_BINDING, &buf);
    if (!buf && F(glIsEnabled)(GL_TEXTURE_COORD_ARRAY)) {
        GLint count;
        GLint type;
        GLint stride;
        F(glGetIntegerv)(GL_TEXTURE_COORD_ARRAY_SIZE, &count);
        F(glGetIntegerv)(GL_TEXTURE_COORD_ARRAY_TYPE, &type);
        F(glGetIntegerv)(GL_TEXTURE_COORD_ARRAY_STRIDE, &stride);
        
        GLint size = get_vertex_size(count, type, stride) * vertex_count;
        GLvoid* data;
        F(glGetPointerv)(GL_TEXTURE_COORD_ARRAY_POINTER, &data);
        
        glClientTexCoordDataWIP15(prim, size, data);
    }
}

void glDrawArrays(GLenum mode, GLint first, GLsizei count) {
    glBeginClientArrayDataWIP15(1);
    do_arrays(0, count);
    
    gl_start(FUNC_glDrawArrays);
    gl_param_GLenum(mode, GROUP_PrimitiveType);
    gl_param_GLint(first, -1);
    gl_param_GLsizei(count, -1);
    F(glDrawArrays)(mode, first, count);
    gl_end();
}

void glMultiDrawArrays(GLenum mode, GLint* first, GLsizei* count, GLsizei primcount) {
    glBeginClientArrayDataWIP15(primcount);
    for (GLsizei i = 0; i < primcount; i++)
        do_arrays(i, count[i]);
    
    gl_start(FUNC_glMultiDrawArrays);
    gl_param_GLenum(mode, GROUP_PrimitiveType);
    gl_param_GLint_array(primcount, first);
    gl_param_GLint_array(primcount, count);
    gl_param_GLsizei(primcount, -1);
    F(glMultiDrawArrays)(mode, first, count, primcount);
    gl_end();
}
