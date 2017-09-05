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

static void gl_write_data(ptrdiff_t size, const void* data) {
    if (size < 0) size = 0;
    
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
            
            #undef UPDATE_VERSION
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
