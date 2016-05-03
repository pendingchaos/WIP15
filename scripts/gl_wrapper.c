#ifdef ZLIB_ENABLED
#include <zlib.h>
#endif

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

#define COMPRESSION_NONE 0
#define COMPRESSION_ZLIB 1
#define COMPRESSION_LZ4 2

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

static FILE *trace_file = NULL;
static void *lib_gl = NULL;
static limits_t gl30_limits;
static limits_t gl31_limits;
static limits_t gl32_limits;
static limits_t gl33_limits;
static limits_t gl40_limits;
static limits_t gl41_limits;
static limits_t gl42_limits;
static limits_t gl43_limits;
static limits_t gl44_limits;
static limits_t gl45_limits;
static limits_t* current_limits = NULL;
static bool test_mode = false;
static GLsizei drawable_width = -1;
static GLsizei drawable_height = -1;
static unsigned int compression_level = 0; //0-100
static unsigned int compression_method = COMPRESSION_ZLIB;

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

#ifdef LZ4_ENABLED
int LZ4_compress_default(const char* source, char* dest, int sourceSize, int maxDestSize);
#endif

static void gl_write_data(size_t size, const void* data) {
    #ifdef ZLIB_ENABLED
    if (compression_method == COMPRESSION_ZLIB) {
        void* compressed = malloc(size);
        
        unsigned int level = (float)compression_level / 100.0 * 9.0;
        uLongf compressed_size = size;
        if (level && compress2(compressed, &compressed_size, data, size, level) != Z_OK) {
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
    if (compression_method == COMPRESSION_LZ4) {
        void* compressed = malloc(size);
        
        int compressed_size;
        if (!(compressed_size = LZ4_compress_default(data, compressed, size, size))) {
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
    
    #if defined(ZLIB_ENABLED) || defined(LZ4_ENABLED)
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
    
    if (!file) {
        fprintf(stderr, "Unable to find limits file.\n");
        exit(1);
    }
    
    init_limits(&gl30_limits);
    init_limits(&gl31_limits);
    init_limits(&gl32_limits);
    init_limits(&gl33_limits);
    init_limits(&gl40_limits);
    init_limits(&gl41_limits);
    init_limits(&gl42_limits);
    init_limits(&gl43_limits);
    init_limits(&gl44_limits);
    init_limits(&gl45_limits);
    
    bool gl30 = false;
    bool gl31 = false;
    bool gl32 = false;
    bool gl33 = false;
    bool gl40 = false;
    bool gl41 = false;
    bool gl42 = false;
    bool gl43 = false;
    bool gl44 = false;
    bool gl45 = false;
    
    while (true) {
        int c = fgetc(file);
        if (c == EOF) {
            break;
        } else if (c == '@') {
            gl30 = false;
            gl31 = false;
            gl32 = false;
            gl33 = false;
            gl40 = false;
            gl41 = false;
            gl42 = false;
            gl43 = false;
            gl44 = false;
            gl45 = false;
            
            #define UPDATE_VERSION int major, minor;\
            if (sscanf(version, "%d.%d", &major, &minor) != 2) {\
                fprintf(stderr, "Invalid version \"%s\".\n", version);\
                exit(1);\
            }\
            if ((major == 3) && (minor == 0))\
                gl30 = true;\
            else if ((major == 3) && (minor == 1))\
                gl31 = true;\
            else if ((major == 3) && (minor == 2))\
                gl32 = true;\
            else if ((major == 3) && (minor == 3))\
                gl33 = true;\
            else if ((major == 4) && (minor == 0))\
                gl40 = true;\
            else if ((major == 4) && (minor == 1))\
                gl41 = true;\
            else if ((major == 4) && (minor == 2))\
                gl42 = true;\
            else if ((major == 4) && (minor == 3))\
                gl43 = true;\
            else if ((major == 4) && (minor == 4))\
                gl44 = true;\
            else if ((major == 4) && (minor == 5))\
                gl45 = true;\
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
            
            if (gl30) set_limit_value(&gl30_limits, name, value);
            if (gl31) set_limit_value(&gl31_limits, name, value);
            if (gl32) set_limit_value(&gl32_limits, name, value);
            if (gl33) set_limit_value(&gl33_limits, name, value);
            if (gl40) set_limit_value(&gl40_limits, name, value);
            if (gl41) set_limit_value(&gl41_limits, name, value);
            if (gl42) set_limit_value(&gl42_limits, name, value);
            if (gl43) set_limit_value(&gl43_limits, name, value);
            if (gl44) set_limit_value(&gl44_limits, name, value);
            if (gl45) set_limit_value(&gl45_limits, name, value);
        }
    }
    
    fclose(file);
    
    create_extensions_str(&gl30_limits);
    create_extensions_str(&gl31_limits);
    create_extensions_str(&gl32_limits);
    create_extensions_str(&gl33_limits);
    create_extensions_str(&gl40_limits);
    create_extensions_str(&gl41_limits);
    create_extensions_str(&gl42_limits);
    create_extensions_str(&gl43_limits);
    create_extensions_str(&gl44_limits);
    create_extensions_str(&gl45_limits);
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

void __attribute__ ((destructor)) wip15_gl_deinit() {
    fclose(trace_file);
    dlclose(lib_gl);
    
    free_limits(&gl30_limits);
    free_limits(&gl31_limits);
    free_limits(&gl32_limits);
    free_limits(&gl33_limits);
    free_limits(&gl40_limits);
    free_limits(&gl41_limits);
    free_limits(&gl42_limits);
    free_limits(&gl43_limits);
    free_limits(&gl44_limits);
    free_limits(&gl45_limits);
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
        //TODO
        break;
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
    
    GLint alignment;
    F(glGetIntegerv)(GL_UNPACK_ALIGNMENT, &alignment);
    
    if (final_size) {
        size_t base = final_size / alignment * alignment;
        size_t extra = final_size - base;
        final_size = base + (extra?alignment:0);
    }
    
    return final_size;
}

void glTestFBWIP15(const GLchar* name, const GLvoid* color, const GLvoid* depth);
void glDrawableSizeWIP15(GLsizei width, GLsizei height);

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
    
    if (w!=drawable_width || h!=drawable_height) {
        drawable_width = w;
        drawable_height = h;
        glDrawableSizeWIP15(w, h);
    }
}

static void test_fb(const char* name) {
    if (test_mode) {
        update_drawable_size();
        
        F(glFinish)();
        
        GLint last_buf;
        F(glGetIntegerv)(GL_READ_BUFFER, &last_buf);
        
        F(glReadBuffer)(GL_BACK);
        void* back = malloc(drawable_width*drawable_height*4);
        F(glReadPixels)(0, 0, drawable_width, drawable_height, GL_RGBA, GL_UNSIGNED_BYTE, back);
        
        void* depth = malloc(drawable_width*drawable_height*4);
        F(glReadPixels)(0, 0, drawable_width, drawable_height, GL_DEPTH_COMPONENT, GL_UNSIGNED_INT, depth);
        
        F(glReadBuffer)(last_buf);
        
        glTestFBWIP15(name, back, depth);
        
        free(back);
        free(depth);
    }
}

size_t glx_attrib_int_count(const int* attribs) {
    size_t count = 0;
    while (attribs[count]) count += 2;
    return count + 1;
}

static void add_uniform(const char* name, uint32_t loc) {
    uint8_t data[strlen(name)+8];
    loc = htole32(loc);
    uint32_t len = htole32(strlen(name));
    memcpy(data, &loc, 4);
    memcpy(data+4, &len, 4);
    memcpy(data+8, name, strlen(name));
    gl_add_extra("replay/program/uniform", strlen(name)+8, data);
}

static void add_vertex_attrib(const char* name, uint32_t idx) {
    uint8_t data[strlen(name)+8];
    idx = htole32(idx);
    uint32_t len = htole32(strlen(name));
    memcpy(data, &idx, 4);
    memcpy(data+4, &len, 4);
    memcpy(data+8, name, strlen(name));
    gl_add_extra("replay/program/vertex_attrib", strlen(name)+8, data);
}

static void link_program_extras(GLuint program) {
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
        
        if (!strncmp(name, "_main_0_gp5vp", 13)) continue; //TODO: This is a hack
        if (!strncmp(name, "gl_", 3)) continue;
        
        if (size == 1) {
            add_uniform(name, F(glGetUniformLocation)(program, name));
        } else {
            for (size_t j = 0; j < size; j++) {
                GLchar new_name[maxNameLen+1];
                memset(new_name, 0, maxNameLen+1);
                snprintf(new_name, maxNameLen+1, "%s[%zu]", name, j);
                add_uniform(new_name, F(glGetUniformLocation)(program, name));
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
            add_vertex_attrib(name, F(glGetAttribLocation)(program, name));
        } else {
            for (size_t j = 0; j < size; j++) {
                GLchar new_name[maxNameLen+1];
                memset(new_name, 0, maxNameLen+1);
                snprintf(new_name, maxNameLen+1, "%s[%zu]", name, j);
                add_vertex_attrib(new_name, F(glGetAttribLocation)(program, name));
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
