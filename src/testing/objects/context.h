static testing_property_t* context_properties = NULL;

#if REPLAY
static void* get_data_prop_context_depth_buf(uint64_t index, const void* rev_, size_t* size) {
    //Assume the viewport is (0, 0, drawable_width, drawable_height)
    const trc_gl_context_rev_t* rev = rev_;
    const float* viewport = trc_map_data(rev->state_float_GL_VIEWPORT, TRC_MAP_READ);
    *size = (size_t)viewport[2] * (size_t)viewport[3] * 2;
    void* data = malloc(*size*2);
    trc_read_chunked_data_t rinfo =
        {.data=rev->back_depth_buffer, .start=0, .size=rev->back_depth_buffer.size, .dest=data};
    trc_read_chunked_data(rinfo);
    
    for (size_t i = 0; i < viewport[2]*viewport[3]; i++)
        ((uint16_t*)data)[i] = ((float*)data)[i] * 65535.0f;
    
    return data;
}
#endif

static void begin_get_context_fb(void* ctx, GLint temp[11], GLenum buf) {
    F(glGetIntegerv)(GL_PACK_SWAP_BYTES, &temp[0]);
    F(glGetIntegerv)(GL_PACK_LSB_FIRST, &temp[1]);
    F(glGetIntegerv)(GL_PACK_ROW_LENGTH, &temp[2]);
    F(glGetIntegerv)(GL_PACK_IMAGE_HEIGHT, &temp[3]);
    F(glGetIntegerv)(GL_PACK_SKIP_ROWS, &temp[4]);
    F(glGetIntegerv)(GL_PACK_SKIP_PIXELS, &temp[5]);
    F(glGetIntegerv)(GL_PACK_SKIP_IMAGES, &temp[6]);
    F(glGetIntegerv)(GL_PACK_ALIGNMENT, &temp[7]);
    F(glGetIntegerv)(GL_READ_BUFFER, &temp[8]);
    F(glGetIntegerv)(GL_PIXEL_PACK_BUFFER_BINDING, &temp[9]);
    F(glGetIntegerv)(GL_READ_FRAMEBUFFER_BINDING, &temp[10]);
    F(glPixelStorei)(GL_PACK_SWAP_BYTES, 0);
    F(glPixelStorei)(GL_PACK_LSB_FIRST, 0);
    F(glPixelStorei)(GL_PACK_ROW_LENGTH, 0);
    F(glPixelStorei)(GL_PACK_IMAGE_HEIGHT, 0);
    F(glPixelStorei)(GL_PACK_SKIP_ROWS, 0);
    F(glPixelStorei)(GL_PACK_SKIP_PIXELS, 0);
    F(glPixelStorei)(GL_PACK_SKIP_IMAGES, 0);
    F(glPixelStorei)(GL_PACK_ALIGNMENT, 1);
    F(glReadBuffer)(buf);
    F(glBindBuffer)(GL_PIXEL_PACK_BUFFER, 0);
    F(glBindFramebuffer)(GL_READ_FRAMEBUFFER, 0);
}

static void end_get_context_fb(void* ctx, GLint temp[11]) {
    F(glPixelStorei)(GL_PACK_SWAP_BYTES, temp[0]);
    F(glPixelStorei)(GL_PACK_LSB_FIRST, temp[1]);
    F(glPixelStorei)(GL_PACK_ROW_LENGTH, temp[2]);
    F(glPixelStorei)(GL_PACK_IMAGE_HEIGHT, temp[3]);
    F(glPixelStorei)(GL_PACK_SKIP_ROWS, temp[4]);
    F(glPixelStorei)(GL_PACK_SKIP_PIXELS, temp[5]);
    F(glPixelStorei)(GL_PACK_SKIP_IMAGES, temp[6]);
    F(glPixelStorei)(GL_PACK_ALIGNMENT, temp[7]);
    F(glReadBuffer)(temp[8]);
    F(glBindBuffer)(GL_PIXEL_PACK_BUFFER, temp[9]);
    F(glBindFramebuffer)(GL_READ_FRAMEBUFFER, temp[10]);
}

static void* get_data_prop_context_depth_buf_gl(uint64_t index, void* ctx, const void* rev, GLuint64 real, size_t* size) {
    //Assume the viewport is (0, 0, drawable_width, drawable_height)
    GLint viewport[4];
    F(glGetIntegerv)(GL_VIEWPORT, viewport);
    
    GLint temp[11];
    begin_get_context_fb(ctx, temp, GL_BACK);
    *size = (size_t)viewport[2]*(size_t)viewport[3]*2;
    void* data = malloc(*size);
    F(glReadPixels)(0, 0, viewport[2], viewport[3], GL_DEPTH_COMPONENT, GL_UNSIGNED_SHORT, data);
    end_get_context_fb(ctx, temp);
    
    return data;
}

//State TODO list
/*map array buffer_binding_point bound_buffer_indexed
    GL_TRANSFORM_FEEDBACK_BUFFER
    GL_UNIFORM_BUFFER
    GL_SHADER_STORAGE_BUFFER
    GL_ATOMIC_COUNTER_BUFFER

transform_feedback current_tf

uint tf_primitive

map array query bound_queries
    GL_SAMPLES_PASSED
    GL_ANY_SAMPLES_PASSED
    GL_ANY_SAMPLES_PASSED_CONSERVATIVE
    GL_PRIMITIVES_GENERATED
    GL_TRANSFORM_FEEDBACK_PRIMITIVES_WRITTEN
    GL_TIME_ELAPSED

uint active_texture_unit
map array texture bound_textures
    GL_TEXTURE_1D
    GL_TEXTURE_2D
    GL_TEXTURE_3D
    GL_TEXTURE_1D_ARRAY
    GL_TEXTURE_2D_ARRAY
    GL_TEXTURE_RECTANGLE
    GL_TEXTURE_CUBE_MAP
    GL_TEXTURE_CUBE_MAP_ARRAY
    GL_TEXTURE_BUFFER
    GL_TEXTURE_2D_MULTISAMPLE
    GL_TEXTURE_2D_MULTISAMPLE_ARRAY

array sampler bound_samplers

chunked array uint8 front_color_buffer
chunked array uint8 back_color_buffer
chunked array uint8 back_stencil_buffer

array double current_vertex_attrib
array enum current_vertex_attrib_types

map array char state_ascii_string
//    GL_VENDOR
//    GL_RENDERER
//    GL_VERSION
//    GL_SHADING_LANGUAGE_VERSION*/

//Create trc_data_t indexing utilities
#if REPLAY
#define INDEX_DATA_FUNC(t, d)\
static t index_data_##t(trc_data_t* data, size_t index) {\
    if (index >= data->size/sizeof(t)) return d;\
    t* mapped = trc_map_data(data, TRC_MAP_READ);\
    t val = mapped[index];\
    trc_unmap_data(mapped);\
    return val;\
}
INDEX_DATA_FUNC(bool, false);
INDEX_DATA_FUNC(int, 0);
INDEX_DATA_FUNC(uint, 0);
INDEX_DATA_FUNC(float, 0.0f);
INDEX_DATA_FUNC(double, 0.0);
#undef INDEX_DATA_FUNC
#endif

//ENABLED*()
#define ENABLED(minver, cap) int64_t get_int_prop_context_##cap(uint64_t index, const void* rev) {\
    return SWITCH_REPLAY(index_data_bool(((const trc_gl_context_rev_t*)rev)->enabled_GL_##cap, 0), 0);\
}\
int64_t get_int_prop_context_##cap##_gl(uint64_t index, void* ctx, const void* rev, GLuint64 real) {\
    return F(glIsEnabled)(GL_##cap);\
}\
REGISTER_PROPERTY_INT(context, minver, cap, get_int_prop_context_##cap, get_int_prop_context_##cap##_gl)

#define ENABLED_INDEXED(minver, cap) int64_t get_int_prop_context_##cap(uint64_t index, const void* rev) {\
    return SWITCH_REPLAY(index_data_bool(((const trc_gl_context_rev_t*)rev)->enabled_GL_##cap, index), 0);\
}\
int64_t get_int_prop_context_##cap##_gl(uint64_t index, void* ctx, const void* rev, GLuint64 real) {\
    return F(glIsEnabledi)(GL_##cap, index);\
}\
REGISTER_PROPERTY_INT(context, minver, cap, get_int_prop_context_##cap, get_int_prop_context_##cap##_gl)

//STATE_BOOL
#define STATE_BOOL(minver, param) int64_t get_int_prop_context_##param(uint64_t index, const void* rev) {\
    return SWITCH_REPLAY(index_data_bool(((const trc_gl_context_rev_t*)rev)->state_bool_GL_##param, 0), 0);\
}\
int64_t get_int_prop_context_##param##_gl(uint64_t index, void* ctx, const void* rev, GLuint64 real) {\
    GLboolean v;\
    F(glGetBooleanv)(GL_##param, &v);\
    return v;\
}\
REGISTER_PROPERTY_INT(context, minver, param, get_int_prop_context_##param, get_int_prop_context_##param##_gl)

//HINT
#define HINT(minver, hint) int64_t get_int_prop_context_##hint(uint64_t index, const void* rev) {\
    return SWITCH_REPLAY(((const trc_gl_context_rev_t*)rev)->hints_GL_##hint, 0);\
}\
int64_t get_int_prop_context_##hint##_gl(uint64_t index, void* ctx, const void* rev, GLuint64 real) {\
    GLint v;\
    F(glGetIntegerv)(GL_##hint, &v);\
    return v;\
}\
REGISTER_PROPERTY_INT(context, minver, hint, get_int_prop_context_##hint, get_int_prop_context_##hint##_gl)

//STATE_INT
#define STATE_INT(minver, param) int64_t get_int_prop_context_##param(uint64_t index, const void* rev) {\
    return SWITCH_REPLAY(index_data_int(((const trc_gl_context_rev_t*)rev)->state_int_GL_##param, 0), 0);\
}\
int64_t get_int_prop_context_##param##_gl(uint64_t index, void* ctx, const void* rev, GLuint64 real) {\
    GLint v;\
    F(glGetIntegerv)(GL_##param, &v);\
    return v;\
}\
REGISTER_PROPERTY_INT(context, minver, param, get_int_prop_context_##param, get_int_prop_context_##param##_gl)

//STATE_FLOAT
#define STATE_FLOAT(minver, param) double get_double_prop_context_##param(uint64_t index, const void* rev) {\
    return SWITCH_REPLAY(index_data_float(((const trc_gl_context_rev_t*)rev)->state_float_GL_##param, 0), 0);\
}\
double get_double_prop_context_##param##_gl(uint64_t index, void* ctx, const void* rev, GLuint64 real) {\
    GLdouble v;\
    F(glGetDoublev)(GL_##param, &v);\
    return v;\
}\
REGISTER_PROPERTY_DOUBLE(context, minver, param, get_double_prop_context_##param, get_double_prop_context_##param##_gl)

//STATE_ENUM
#define STATE_ENUM(minver, param) int64_t get_int_prop_context_##param(uint64_t index, const void* rev) {\
    return SWITCH_REPLAY(index_data_uint(((const trc_gl_context_rev_t*)rev)->state_enum_GL_##param, 0), 0);\
}\
int64_t get_int_prop_context_##param##_gl(uint64_t index, void* ctx, const void* rev, GLuint64 real) {\
    GLint v;\
    F(glGetIntegerv)(GL_##param, &v);\
    return v;\
}\
REGISTER_PROPERTY_INT(context, minver, param, get_int_prop_context_##param, get_int_prop_context_##param##_gl)

//OBJ()
//TODO: get_int_prop_context_##propname##_gl for replay
#if REPLAY
#define OBJ(minver, getparam, get_code)\
int64_t get_int_prop_context_##getparam(uint64_t index, const void* rev_) {\
    const trc_gl_context_rev_t* rev = rev_;\
    trc_obj_t* obj = (get_code).obj;\
    return obj ? ((const trc_obj_rev_head_t*)trc_obj_get_rev(obj, -1))->name : 0;\
}\
REGISTER_PROPERTY_INT(context, minver, getparam, &get_int_prop_context_##getparam, NULL)
#else
#define OBJ(minver, getparam, get_code)\
int64_t get_int_prop_context_##getparam##_gl(uint64_t index, void* ctx, const void* rev, GLuint64 real) {\
    GLint v;\
    F(glGetIntegerv)(GL_##getparam, &v);\
    return v;\
}\
REGISTER_PROPERTY_INT(context, minver, getparam, NULL, &get_int_prop_context_##getparam##_gl)
#endif

#define BUFFER(minver, prop) OBJ(minver, prop##_BUFFER_BINDING, rev->bound_buffer_GL_##prop##_BUFFER)

BUFFER(320, ARRAY)
BUFFER(320, ATOMIC_COUNTER)
BUFFER(320, COPY_READ)
BUFFER(320, COPY_WRITE)
BUFFER(430, DISPATCH_INDIRECT)
BUFFER(400, DRAW_INDIRECT)
BUFFER(320, ELEMENT_ARRAY)
BUFFER(320, PIXEL_PACK)
BUFFER(320, PIXEL_UNPACK)
BUFFER(440, QUERY)
BUFFER(430, SHADER_STORAGE)
BUFFER(320, TEXTURE)
BUFFER(320, TRANSFORM_FEEDBACK)
BUFFER(320, UNIFORM)

OBJ(320, CURRENT_PROGRAM, rev->bound_program)
OBJ(410, PROGRAM_PIPELINE_BINDING, rev->bound_pipeline)
OBJ(320, VERTEX_ARRAY_BINDING, rev->bound_vao)
OBJ(320, RENDERBUFFER_BINDING, rev->bound_renderbuffer)
OBJ(320, READ_FRAMEBUFFER_BINDING, rev->read_framebuffer)
OBJ(320, DRAW_FRAMEBUFFER_BINDING, rev->draw_framebuffer)

ENABLED_INDEXED(320, BLEND)
ENABLED(320, COLOR_LOGIC_OP)
ENABLED(320, CULL_FACE)
//ENABLED(DEBUG_OUTPUT)
//ENABLED(DEBUG_OUTPUT_SYNCHRONOUS)
ENABLED(320, DEPTH_CLAMP)
ENABLED(320, DEPTH_TEST)
ENABLED(320, DITHER)
ENABLED(320, FRAMEBUFFER_SRGB)
ENABLED(320, LINE_SMOOTH)
ENABLED(320, MULTISAMPLE)
ENABLED(320, POLYGON_OFFSET_FILL)
ENABLED(320, POLYGON_OFFSET_LINE)
ENABLED(320, POLYGON_OFFSET_POINT)
ENABLED(320, POLYGON_SMOOTH)
ENABLED(320, PRIMITIVE_RESTART)
ENABLED(430, PRIMITIVE_RESTART_FIXED_INDEX)
ENABLED(320, RASTERIZER_DISCARD)
ENABLED(320, SAMPLE_ALPHA_TO_COVERAGE)
ENABLED(320, SAMPLE_ALPHA_TO_ONE)
ENABLED(320, SAMPLE_COVERAGE)
ENABLED(400, SAMPLE_SHADING)
ENABLED(320, SAMPLE_MASK)
ENABLED_INDEXED(320, SCISSOR_TEST)
ENABLED(320, STENCIL_TEST)
ENABLED(320, TEXTURE_CUBE_MAP_SEAMLESS)
ENABLED(320, PROGRAM_POINT_SIZE)

STATE_BOOL(320, DEPTH_WRITEMASK)
//STATE_BOOL(COLOR_WRITEMASK)
STATE_BOOL(320, PACK_SWAP_BYTES)
STATE_BOOL(320, PACK_LSB_FIRST)
STATE_BOOL(320, UNPACK_SWAP_BYTES)
STATE_BOOL(320, UNPACK_LSB_FIRST)
STATE_BOOL(320, SAMPLE_COVERAGE_INVERT)

HINT(320, FRAGMENT_SHADER_DERIVATIVE_HINT)
HINT(320, LINE_SMOOTH_HINT)
HINT(320, POLYGON_SMOOTH_HINT)
HINT(320, TEXTURE_COMPRESSION_HINT)

STATE_INT(320, PACK_ROW_LENGTH)
STATE_INT(320, PACK_IMAGE_HEIGHT)
STATE_INT(320, PACK_SKIP_ROWS)
STATE_INT(320, PACK_SKIP_PIXELS)
STATE_INT(320, PACK_SKIP_IMAGES)
STATE_INT(320, PACK_ALIGNMENT)
STATE_INT(320, UNPACK_ROW_LENGTH)
STATE_INT(320, UNPACK_IMAGE_HEIGHT)
STATE_INT(320, UNPACK_SKIP_ROWS)
STATE_INT(320, UNPACK_SKIP_PIXELS)
STATE_INT(320, UNPACK_SKIP_IMAGES)
STATE_INT(320, UNPACK_ALIGNMENT)
STATE_INT(320, STENCIL_CLEAR_VALUE)
STATE_INT(320, STENCIL_VALUE_MASK)
STATE_INT(320, STENCIL_REF)
STATE_INT(320, STENCIL_BACK_VALUE_MASK)
STATE_INT(320, STENCIL_BACK_REF)
STATE_INT(320, STENCIL_WRITEMASK)
STATE_INT(320, STENCIL_BACK_WRITEMASK)
STATE_INT(320, PRIMITIVE_RESTART_INDEX)
//TODO: SCISSOR_BOX
//STATE_INT(SAMPLE_ALPHA_TO_COVERAGE)
//STATE_INT(SAMPLE_ALPHA_TO_ONE)
//STATE_INT(SAMPLE_COVERAGE)
//STATE_INT(SAMPLE_BUFFFERS)
//STATE_INT(SAMPLES)
//STATE_INT(UNPACK_COMPRESSED_BLOCK_WIDTH)
//STATE_INT(UNPACK_COMPRESSED_BLOCK_HEIGHT)
//STATE_INT(UNPACK_COMPRESSED_BLOCK_DEPTH)
//STATE_INT(UNPACK_COMPRESSED_BLOCK_SIZE)
//STATE_INT(PACK_COMPRESSED_BLOCK_WIDTH)
//STATE_INT(PACK_COMPRESSED_BLOCK_HEIGHT)
//STATE_INT(PACK_COMPRESSED_BLOCK_DEPTH)
//STATE_INT(PACK_COMPRESSED_BLOCK_SIZE)

STATE_ENUM(320, POLYGON_MODE)
STATE_ENUM(320, CULL_FACE_MODE)
STATE_ENUM(320, FRONT_FACE)
STATE_ENUM(320, DEPTH_FUNC)
STATE_ENUM(320, STENCIL_FUNC)
STATE_ENUM(320, STENCIL_FAIL)
STATE_ENUM(320, STENCIL_PASS_DEPTH_FAIL)
STATE_ENUM(320, STENCIL_PASS_DEPTH_PASS)
STATE_ENUM(320, STENCIL_BACK_FUNC)
STATE_ENUM(320, STENCIL_BACK_FAIL)
STATE_ENUM(320, STENCIL_BACK_PASS_DEPTH_FAIL)
STATE_ENUM(320, STENCIL_BACK_PASS_DEPTH_PASS)
//STATE_ENUM(BLEND_DST_RGB)
//STATE_ENUM(BLEND_DST_ALPHA)
//STATE_ENUM(BLEND_SRC_RGB)
//STATE_ENUM(BLEND_SRC_ALPHA)
//STATE_ENUM(BLEND_EQUATION_RGB)
//STATE_ENUM(BLEND_EQUATION_ALPHA)
STATE_ENUM(320, LOGIC_OP_MODE)
STATE_ENUM(320, DRAW_BUFFER)
//STATE_ENUM(READ_BUFFER)
//STATE_ENUM(LAYER_PROVOKING_VERTEX)
//STATE_ENUM(VIEWPORT_INDEX_PROVOKING_VERTEX)
//STATE_ENUM(CLAMP_READ_COLOR)
STATE_ENUM(320, POINT_SPRITE_COORD_ORIGIN)
STATE_ENUM(320, PROVOKING_VERTEX)
STATE_ENUM(450, CLIP_ORIGIN)
STATE_ENUM(450, CLIP_DEPTH_MODE)

//TODO: GL_VIEWPORT
STATE_FLOAT(320, DEPTH_CLEAR_VALUE)
//TODO: GL_COLOR_CLEAR_VALUE
//TODO: GL_DEPTH_RANGE
STATE_FLOAT(320, POINT_SIZE)
STATE_FLOAT(320, LINE_WIDTH)
STATE_FLOAT(320, POLYGON_OFFSET_UNITS)
STATE_FLOAT(320, POLYGON_OFFSET_FACTOR)
STATE_FLOAT(320, POLYGON_OFFSET_CLAMP)
STATE_FLOAT(320, SAMPLE_COVERAGE_VALUE)
//TODO: GL_BLEND_COLOR
STATE_FLOAT(320, POINT_FADE_THRESHOLD_SIZE)
STATE_FLOAT(400, MIN_SAMPLE_SHADING_VALUE)

int64_t get_int_prop_context_CLIP_DISTANCE(uint64_t index, const void* rev) {
    return SWITCH_REPLAY(index_data_bool(((const trc_gl_context_rev_t*)rev)->enabled_GL_CLIP_DISTANCE0, index), 0);
}
int64_t get_int_prop_context_CLIP_DISTANCE_gl(uint64_t index, void* ctx, const void* rev, GLuint64 real) {
    return F(glIsEnabled)(GL_CLIP_DISTANCE0+index);
}
REGISTER_PROPERTY_INT(context, 320, CLIP_DISTANCE, get_int_prop_context_CLIP_DISTANCE, get_int_prop_context_CLIP_DISTANCE_gl)

REGISTER_PROPERTY_DATA(context, 320, depth_buf, SWITCH_REPLAY(get_data_prop_context_depth_buf, NULL), get_data_prop_context_depth_buf_gl)

#undef HINT
#undef STATE_FLOAT
#undef STATE_BOOL
#undef STATE_ENUM
#undef STATE_INT
#undef BUFFER
#undef OBJ
#undef ENABLED_INDEXED
#undef ENABLED
