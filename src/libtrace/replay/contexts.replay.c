#include "libtrace/replay/utils.h"

#define REPLAY_CONFIG_FUNCS
#define RC_F F
#include "shared/replay_config.h"
#undef RC_F
#undef REPLAY_CONFIG_FUNCS

static void create_context_buffers(trc_gl_context_rev_t* rev) {
    size_t size = rev->drawable_width * rev->drawable_height * 4;
    rev->front_color_buffer = trc_create_chunked_data(ctx->trace, size, NULL);
    rev->back_color_buffer = rev->front_color_buffer;
    rev->back_depth_buffer = rev->back_color_buffer;
    rev->back_stencil_buffer = rev->back_depth_buffer;
}

static void begin_get_fb0_data(GLint prev[11]) {
    F(glGetIntegerv)(GL_PACK_SWAP_BYTES, &prev[0]);
    F(glGetIntegerv)(GL_PACK_LSB_FIRST, &prev[1]);
    F(glGetIntegerv)(GL_PACK_ROW_LENGTH, &prev[2]);
    F(glGetIntegerv)(GL_PACK_IMAGE_HEIGHT, &prev[3]);
    F(glGetIntegerv)(GL_PACK_SKIP_PIXELS, &prev[4]);
    F(glGetIntegerv)(GL_PACK_SKIP_ROWS, &prev[5]);
    F(glGetIntegerv)(GL_PACK_SKIP_IMAGES, &prev[6]);
    F(glGetIntegerv)(GL_PACK_ALIGNMENT, &prev[7]);
    F(glGetIntegerv)(GL_READ_BUFFER, &prev[8]);
    F(glGetIntegerv)(GL_READ_FRAMEBUFFER_BINDING, &prev[9]);
    F(glGetIntegerv)(GL_PIXEL_PACK_BUFFER_BINDING, &prev[10]);
    
    F(glPixelStorei)(GL_PACK_SWAP_BYTES, GL_FALSE);
    F(glPixelStorei)(GL_PACK_LSB_FIRST, GL_FALSE);
    F(glPixelStorei)(GL_PACK_ROW_LENGTH, 0);
    F(glPixelStorei)(GL_PACK_IMAGE_HEIGHT, 0);
    F(glPixelStorei)(GL_PACK_SKIP_PIXELS, 0);
    F(glPixelStorei)(GL_PACK_SKIP_ROWS, 0);
    F(glPixelStorei)(GL_PACK_SKIP_IMAGES, 0);
    F(glPixelStorei)(GL_PACK_ALIGNMENT, 1);
    F(glBindFramebuffer)(GL_READ_FRAMEBUFFER, 0);
    F(glBindBuffer)(GL_PIXEL_PACK_BUFFER, 0);
}

static void end_get_fb0_data(const GLint prev[11]) {
    F(glBindBuffer)(GL_PIXEL_PACK_BUFFER, prev[10]);
    F(glBindFramebuffer)(GL_READ_FRAMEBUFFER, prev[9]);
    F(glReadBuffer)(prev[8]);
    F(glPixelStorei)(GL_PACK_ALIGNMENT, prev[7]);
    F(glPixelStorei)(GL_PACK_SKIP_IMAGES, prev[6]);
    F(glPixelStorei)(GL_PACK_SKIP_ROWS, prev[5]);
    F(glPixelStorei)(GL_PACK_SKIP_PIXELS, prev[4]);
    F(glPixelStorei)(GL_PACK_IMAGE_HEIGHT, prev[3]);
    F(glPixelStorei)(GL_PACK_ROW_LENGTH, prev[2]);
    F(glPixelStorei)(GL_PACK_LSB_FIRST, prev[1]);
    F(glPixelStorei)(GL_PACK_SWAP_BYTES, prev[0]);
}

static void get_fb0_buffer(trc_chunked_data_t* data, trc_gl_context_rev_t* state, GLenum buffer, GLenum format, GLenum type) {
    F(glReadBuffer)(buffer);
    
    size_t data_size = state->drawable_width * state->drawable_height * 4;
    void* buf = malloc(data_size);
    F(glReadPixels)(0, 0, state->drawable_width, state->drawable_height, format, type, buf);
    
    if (data_size != data->size) {
        *data = trc_create_chunked_data(ctx->trace, data_size, buf);
    } else {
        trc_chunked_data_mod_t mod = {.next=NULL, .start=0, .size=data_size, .data=buf};
        trc_modify_chunked_data_t minfo = {.base=*data, .mods=&mod};
        *data = trc_modify_chunked_data(ctx->trace, minfo);
    }
    
    free(buf);
}

static void store_and_bind_fb(GLint* prev, GLuint fb) {
    F(glGetIntegerv)(GL_DRAW_FRAMEBUFFER_BINDING, prev);
    F(glBindFramebuffer)(GL_DRAW_FRAMEBUFFER, fb);
}

void update_fb0_buffers(bool backcolor, bool frontcolor, bool depth, bool stencil) {
    F(glFinish)();
    
    GLint prevfb;
    store_and_bind_fb(&prevfb, 0);
    GLint depth_size, stencil_size;
    F(glGetFramebufferAttachmentParameteriv)(GL_DRAW_FRAMEBUFFER, GL_DEPTH,
                                             GL_FRAMEBUFFER_ATTACHMENT_DEPTH_SIZE, &depth_size);
    F(glGetFramebufferAttachmentParameteriv)(GL_DRAW_FRAMEBUFFER, GL_STENCIL,
                                             GL_FRAMEBUFFER_ATTACHMENT_STENCIL_SIZE, &stencil_size);
    if (depth_size == 0) depth = false;
    if (stencil_size == 0) stencil = false;
    F(glBindFramebuffer)(GL_DRAW_FRAMEBUFFER, prevfb);
    
    GLint prev[11];
    begin_get_fb0_data(prev);
    trc_gl_context_rev_t state = *trc_get_context(ctx->trace);
    if (backcolor)
        get_fb0_buffer(&state.back_color_buffer, &state, GL_BACK, GL_RGBA, GL_UNSIGNED_BYTE);
    if (frontcolor)
        get_fb0_buffer(&state.front_color_buffer, &state, GL_FRONT, GL_RGBA, GL_UNSIGNED_BYTE);
    if (depth)
        get_fb0_buffer(&state.back_depth_buffer, &state, GL_BACK, GL_DEPTH_COMPONENT, GL_FLOAT);
    if (stencil)
        get_fb0_buffer(&state.back_stencil_buffer, &state, GL_BACK, GL_STENCIL_INDEX, GL_UNSIGNED_INT);
    trc_set_context(ctx->trace, &state);
    end_get_fb0_data(prev);
}

static void init_context() {
    trace_t* trace = ctx->trace;
    
    trc_replay_config_t cfg = trc_get_context(trace)->trace_cfg;
    
    int w, h;
    SDL_GL_GetDrawableSize(ctx->window, &w, &h);
    gls_set_drawable_width(w);
    gls_set_drawable_height(h);
    gls_bound_buffer_init(GL_ARRAY_BUFFER, (trc_obj_ref_t){NULL});
    gls_bound_buffer_init(GL_ELEMENT_ARRAY_BUFFER, (trc_obj_ref_t){NULL});
    gls_bound_buffer_init(GL_ATOMIC_COUNTER_BUFFER, (trc_obj_ref_t){NULL});
    gls_bound_buffer_init(GL_COPY_READ_BUFFER, (trc_obj_ref_t){NULL});
    gls_bound_buffer_init(GL_COPY_WRITE_BUFFER, (trc_obj_ref_t){NULL});
    gls_bound_buffer_init(GL_DISPATCH_INDIRECT_BUFFER, (trc_obj_ref_t){NULL});
    gls_bound_buffer_init(GL_DRAW_INDIRECT_BUFFER, (trc_obj_ref_t){NULL});
    gls_bound_buffer_init(GL_PIXEL_PACK_BUFFER, (trc_obj_ref_t){NULL});
    gls_bound_buffer_init(GL_PIXEL_UNPACK_BUFFER, (trc_obj_ref_t){NULL});
    gls_bound_buffer_init(GL_QUERY_BUFFER, (trc_obj_ref_t){NULL});
    gls_bound_buffer_init(GL_SHADER_STORAGE_BUFFER, (trc_obj_ref_t){NULL});
    gls_bound_buffer_init(GL_TEXTURE_BUFFER, (trc_obj_ref_t){NULL});
    gls_bound_buffer_init(GL_TRANSFORM_FEEDBACK_BUFFER, (trc_obj_ref_t){NULL});
    gls_bound_buffer_init(GL_UNIFORM_BUFFER, (trc_obj_ref_t){NULL});
    gls_bound_program_init((trc_obj_ref_t){NULL});
    gls_bound_pipeline_init((trc_obj_ref_t){NULL});
    gls_bound_vao_init((trc_obj_ref_t){NULL});
    gls_bound_renderbuffer_init((trc_obj_ref_t){NULL});
    gls_read_framebuffer_init((trc_obj_ref_t){NULL});
    gls_draw_framebuffer_init((trc_obj_ref_t){NULL});
    gls_set_active_texture_unit(0);
    
    gls_bound_queries_init(GL_SAMPLES_PASSED, 1, NULL);
    gls_bound_queries_init(GL_ANY_SAMPLES_PASSED, 1, NULL);
    gls_bound_queries_init(GL_ANY_SAMPLES_PASSED_CONSERVATIVE, 1, NULL);
    gls_bound_queries_init(GL_PRIMITIVES_GENERATED, cfg.max_vertex_streams, NULL);
    gls_bound_queries_init(GL_TRANSFORM_FEEDBACK_PRIMITIVES_WRITTEN, cfg.max_vertex_streams, NULL);
    gls_bound_queries_init(GL_TIME_ELAPSED, 1, NULL);
    
    gls_state_int_init1(GL_MAX_VERTEX_STREAMS, cfg.max_vertex_streams);
    gls_state_int_init1(GL_MAX_CLIP_DISTANCES, cfg.max_clip_distances);
    gls_state_int_init1(GL_MAX_DRAW_BUFFERS, cfg.max_draw_buffers);
    gls_state_int_init1(GL_MAX_VIEWPORTS, cfg.max_viewports);
    gls_state_int_init1(GL_MAX_VERTEX_ATTRIBS, cfg.max_vertex_attribs);
    gls_state_int_init1(GL_MAX_VERTEX_ATTRIB_STRIDE, cfg.max_vertex_attrib_stride);
    gls_state_int_init1(GL_MAX_VERTEX_ATTRIB_RELATIVE_OFFSET, cfg.max_vertex_attrib_relative_offset);
    gls_state_int_init1(GL_MAX_VERTEX_ATTRIB_BINDINGS, cfg.max_vertex_attrib_bindings);
    gls_state_int_init1(GL_MAX_COLOR_ATTACHMENTS, cfg.max_color_attachments);
    gls_state_int_init1(GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS, cfg.max_combined_texture_units);
    gls_state_int_init1(GL_MAX_TRANSFORM_FEEDBACK_BUFFERS, cfg.max_xfb_buffers);
    gls_state_int_init1(GL_MAX_UNIFORM_BUFFER_BINDINGS, cfg.max_ubo_bindings);
    gls_state_int_init1(GL_MAX_PATCH_VERTICES, cfg.max_patch_vertices);
    gls_state_int_init1(GL_MAX_RENDERBUFFER_SIZE, cfg.max_renderbuffer_size);
    gls_state_int_init1(GL_MAX_TEXTURE_SIZE, cfg.max_texture_size);
    gls_state_int_init1(GL_MAX_ATOMIC_COUNTER_BUFFER_BINDINGS, cfg.max_atomic_counter_buffer_bindings);
    gls_state_int_init1(GL_MAX_SHADER_STORAGE_BUFFER_BINDINGS, cfg.max_ssbo_bindings);
    gls_state_int_init1(GL_MAX_SAMPLE_MASK_WORDS, cfg.max_sample_mask_words);
    gls_state_int_init1(GL_MAJOR_VERSION, cfg.version/100);
    gls_state_int_init1(GL_MINOR_VERSION, cfg.version%100/10);
    gls_set_ver(cfg.version);
    
    gls_bound_buffer_indexed_init(GL_UNIFORM_BUFFER, cfg.max_ubo_bindings, NULL);
    gls_bound_buffer_indexed_init(GL_ATOMIC_COUNTER_BUFFER, cfg.max_atomic_counter_buffer_bindings, NULL);
    gls_bound_buffer_indexed_init(GL_SHADER_STORAGE_BUFFER, cfg.max_ssbo_bindings, NULL);
    
    trc_gl_context_rev_t rev = *trc_get_context(trace);
    
    trc_gl_transform_feedback_rev_t default_tf;
    default_tf.has_object = true;
    default_tf.real = 0;
    size_t size = cfg.max_xfb_buffers * sizeof(trc_gl_buffer_binding_point_t);
    default_tf.bindings = trc_create_data(trace, size, NULL, 0);
    default_tf.active = false;
    default_tf.paused = false;
    default_tf.active_not_paused = false;
    trc_obj_t* default_tf_obj = trc_create_named_obj(rev.priv_ns, TrcTransformFeedback, 0, &default_tf);
    
    rev.bound_buffer_indexed_GL_TRANSFORM_FEEDBACK_BUFFER = default_tf.bindings;
    trc_set_context(trace, &rev);
    
    gls_set_tf_primitive(0);
    
    gls_default_tf_init((trc_obj_ref_t){default_tf_obj});
    trc_grab_obj(default_tf_obj); //gls_default_tf_init does not increase the reference count
    gls_current_tf_init((trc_obj_ref_t){default_tf_obj});
    trc_grab_obj(default_tf_obj); //gls_current_tf_init does not increase the reference count
    
    gls_bound_textures_init(GL_TEXTURE_1D, cfg.max_combined_texture_units, NULL);
    gls_bound_textures_init(GL_TEXTURE_2D, cfg.max_combined_texture_units, NULL);
    gls_bound_textures_init(GL_TEXTURE_3D, cfg.max_combined_texture_units, NULL);
    gls_bound_textures_init(GL_TEXTURE_1D_ARRAY, cfg.max_combined_texture_units, NULL);
    gls_bound_textures_init(GL_TEXTURE_2D_ARRAY, cfg.max_combined_texture_units, NULL);
    gls_bound_textures_init(GL_TEXTURE_RECTANGLE, cfg.max_combined_texture_units, NULL);
    gls_bound_textures_init(GL_TEXTURE_CUBE_MAP, cfg.max_combined_texture_units, NULL);
    gls_bound_textures_init(GL_TEXTURE_CUBE_MAP_ARRAY, cfg.max_combined_texture_units, NULL);
    gls_bound_textures_init(GL_TEXTURE_BUFFER, cfg.max_combined_texture_units, NULL);
    gls_bound_textures_init(GL_TEXTURE_2D_MULTISAMPLE, cfg.max_combined_texture_units, NULL);
    gls_bound_textures_init(GL_TEXTURE_2D_MULTISAMPLE_ARRAY, cfg.max_combined_texture_units, NULL);
    
    gls_bound_samplers_init(cfg.max_combined_texture_units, NULL);
    
    gls_enabled_init(GL_BLEND, cfg.max_draw_buffers, NULL);
    gls_enabled_init(GL_CLIP_DISTANCE0, cfg.max_clip_distances, NULL);
    gls_enabled_init1(GL_COLOR_LOGIC_OP, false);
    gls_enabled_init1(GL_CULL_FACE, false);
    gls_enabled_init1(GL_DEBUG_OUTPUT, false); //TODO: This is somes enabled depending on how the context is created
    gls_enabled_init1(GL_DEBUG_OUTPUT_SYNCHRONOUS, false);
    gls_enabled_init1(GL_DEPTH_CLAMP, false);
    gls_enabled_init1(GL_DEPTH_TEST, false);
    gls_enabled_init1(GL_DITHER, true);
    gls_enabled_init1(GL_FRAMEBUFFER_SRGB, false);
    gls_enabled_init1(GL_LINE_SMOOTH, false);
    gls_enabled_init1(GL_MULTISAMPLE, true);
    gls_enabled_init1(GL_POLYGON_OFFSET_FILL, false);
    gls_enabled_init1(GL_POLYGON_OFFSET_LINE, false);
    gls_enabled_init1(GL_POLYGON_OFFSET_POINT, false);
    gls_enabled_init1(GL_POLYGON_SMOOTH, false);
    gls_enabled_init1(GL_PRIMITIVE_RESTART, false);
    gls_enabled_init1(GL_PRIMITIVE_RESTART_FIXED_INDEX, false);
    gls_enabled_init1(GL_RASTERIZER_DISCARD, false);
    gls_enabled_init1(GL_SAMPLE_ALPHA_TO_COVERAGE, false);
    gls_enabled_init1(GL_SAMPLE_ALPHA_TO_ONE, false);
    gls_enabled_init1(GL_SAMPLE_COVERAGE, false);
    gls_enabled_init1(GL_SAMPLE_SHADING, false);
    gls_enabled_init1(GL_SAMPLE_MASK, false);
    gls_enabled_init(GL_SCISSOR_TEST, cfg.max_viewports, NULL);
    gls_enabled_init1(GL_STENCIL_TEST, false);
    gls_enabled_init1(GL_TEXTURE_CUBE_MAP_SEAMLESS, false);
    gls_enabled_init1(GL_PROGRAM_POINT_SIZE, false);
    
    gls_state_bool_init1(GL_DEPTH_WRITEMASK, GL_TRUE);
    bool color_mask[cfg.max_draw_buffers*4];
    for (size_t i = 0; i < cfg.max_draw_buffers*4; i++) color_mask[i] = GL_TRUE;
    gls_state_bool_init(GL_COLOR_WRITEMASK, cfg.max_draw_buffers*4, color_mask);
    gls_state_int_init1(GL_STENCIL_WRITEMASK, 0xffffffff);
    gls_state_int_init1(GL_STENCIL_BACK_WRITEMASK, 0xffffffff);
    
    gls_state_bool_init1(GL_PACK_SWAP_BYTES, GL_FALSE);
    gls_state_bool_init1(GL_PACK_LSB_FIRST, GL_FALSE);
    gls_state_bool_init1(GL_UNPACK_SWAP_BYTES, GL_FALSE);
    gls_state_bool_init1(GL_UNPACK_LSB_FIRST, GL_FALSE);
    
    gls_state_int_init1(GL_PACK_ROW_LENGTH, 0);
    gls_state_int_init1(GL_PACK_IMAGE_HEIGHT, 0);
    gls_state_int_init1(GL_PACK_SKIP_ROWS, 0);
    gls_state_int_init1(GL_PACK_SKIP_PIXELS, 0);
    gls_state_int_init1(GL_PACK_SKIP_IMAGES, 0);
    gls_state_int_init1(GL_PACK_ALIGNMENT, 4);
    gls_state_int_init1(GL_UNPACK_ROW_LENGTH, 0);
    gls_state_int_init1(GL_UNPACK_IMAGE_HEIGHT, 0);
    gls_state_int_init1(GL_UNPACK_SKIP_ROWS, 0);
    gls_state_int_init1(GL_UNPACK_SKIP_PIXELS, 0);
    gls_state_int_init1(GL_UNPACK_SKIP_IMAGES, 0);
    gls_state_int_init1(GL_UNPACK_ALIGNMENT, 4);
    
    gls_state_int_init1(GL_STENCIL_CLEAR_VALUE, 0);
    gls_state_float_init1(GL_DEPTH_CLEAR_VALUE, 1);
    float color_clear[4] = {0.0f, 0.0f, 0.0f, 0.0f};
    gls_state_float_init(GL_COLOR_CLEAR_VALUE, 4, color_clear);
    
    gls_state_float_init1(GL_POINT_SIZE, 1.0f);
    gls_state_float_init1(GL_LINE_WIDTH, 1.0f);
    
    gls_state_float_init1(GL_POLYGON_OFFSET_UNITS, 0.0f);
    gls_state_float_init1(GL_POLYGON_OFFSET_FACTOR, 0.0f);
    gls_state_float_init1(GL_POLYGON_OFFSET_CLAMP, 0.0f);
    
    gls_state_float_init1(GL_SAMPLE_COVERAGE_VALUE, 1.0f);
    gls_state_bool_init1(GL_SAMPLE_COVERAGE_INVERT, false);
    
    gls_state_enum_init1(GL_STENCIL_FUNC, GL_ALWAYS);
    gls_state_int_init1(GL_STENCIL_REF, 0);
    gls_state_int_init1(GL_STENCIL_VALUE_MASK, 0xffffffff);
    gls_state_enum_init1(GL_STENCIL_BACK_FUNC, GL_ALWAYS);
    gls_state_int_init1(GL_STENCIL_BACK_REF, 0);
    gls_state_int_init1(GL_STENCIL_BACK_VALUE_MASK, 0xffffffff);
    gls_state_enum_init1(GL_STENCIL_FAIL, GL_KEEP);
    gls_state_enum_init1(GL_STENCIL_PASS_DEPTH_PASS, GL_KEEP);
    gls_state_enum_init1(GL_STENCIL_PASS_DEPTH_FAIL, GL_KEEP);
    gls_state_enum_init1(GL_STENCIL_BACK_FAIL, GL_KEEP);
    gls_state_enum_init1(GL_STENCIL_BACK_PASS_DEPTH_PASS, GL_KEEP);
    gls_state_enum_init1(GL_STENCIL_BACK_PASS_DEPTH_FAIL, GL_KEEP);
    
    GLenum blenddata[cfg.max_draw_buffers];
    for (size_t i = 0; i < cfg.max_draw_buffers; i++) blenddata[i] = GL_ONE;
    gls_state_enum_init(GL_BLEND_SRC_RGB, cfg.max_draw_buffers, blenddata);
    gls_state_enum_init(GL_BLEND_SRC_ALPHA, cfg.max_draw_buffers, blenddata);
    
    for (size_t i = 0; i < cfg.max_draw_buffers; i++) blenddata[i] = GL_ZERO;
    gls_state_enum_init(GL_BLEND_DST_RGB, cfg.max_draw_buffers, blenddata);
    gls_state_enum_init(GL_BLEND_DST_ALPHA, cfg.max_draw_buffers, blenddata);
    
    float blend_color[4] = {0.0f, 0.0f, 0.0f, 0.0f};
    gls_state_float_init(GL_BLEND_COLOR, 4, blend_color);
    
    for (size_t i = 0; i < cfg.max_draw_buffers; i++) blenddata[i] = GL_FUNC_ADD;
    gls_state_enum_init(GL_BLEND_EQUATION_RGB, cfg.max_draw_buffers, blenddata);
    gls_state_enum_init(GL_BLEND_EQUATION_ALPHA, cfg.max_draw_buffers, blenddata);
    
    gls_set_hints(GL_FRAGMENT_SHADER_DERIVATIVE_HINT, GL_DONT_CARE);
    gls_set_hints(GL_LINE_SMOOTH_HINT, GL_DONT_CARE);
    gls_set_hints(GL_POLYGON_SMOOTH_HINT, GL_DONT_CARE);
    gls_set_hints(GL_TEXTURE_COMPRESSION_HINT, GL_DONT_CARE);
    
    float zerof[cfg.max_viewports*4];
    for (size_t i = 0; i < cfg.max_viewports*4; i++) zerof[i] = 0.0f;
    gls_state_float_init(GL_VIEWPORT, cfg.max_viewports*4, zerof);
    gls_state_int_init(GL_SCISSOR_BOX, cfg.max_viewports*4, NULL);
    float depth_range[cfg.max_viewports*2];
    for (int i = 0; i < cfg.max_viewports*2; i++) depth_range[i] = (float[]){0.0f, 1.0f}[i%2];
    gls_state_float_init(GL_DEPTH_RANGE, cfg.max_viewports*2, depth_range);
    
    gls_state_enum_init1(GL_PROVOKING_VERTEX, GL_LAST_VERTEX_CONVENTION);
    gls_state_enum_init1(GL_LOGIC_OP_MODE, GL_COPY);
    gls_state_int_init1(GL_PRIMITIVE_RESTART_INDEX, 0);
    gls_state_enum_init1(GL_POLYGON_MODE, GL_FILL);
    gls_state_enum_init1(GL_CULL_FACE_MODE, GL_BACK);
    gls_state_enum_init1(GL_FRONT_FACE, GL_CCW);
    gls_state_enum_init1(GL_DEPTH_FUNC, GL_LESS);
    gls_state_enum_init1(GL_LOGIC_OP_MODE, GL_COPY);
    gls_state_enum_init1(GL_CLIP_ORIGIN, GL_LOWER_LEFT);
    gls_state_enum_init1(GL_CLIP_DEPTH_MODE, GL_NEGATIVE_ONE_TO_ONE);
    gls_state_float_init1(GL_POINT_FADE_THRESHOLD_SIZE, 1.0);
    gls_state_enum_init1(GL_POINT_SPRITE_COORD_ORIGIN, GL_UPPER_LEFT);
    gls_state_float_init1(GL_MIN_SAMPLE_SHADING_VALUE, 0.0f);
    
    gls_state_int_init1(GL_PATCH_VERTICES, 3);
    float one4[4] = {1.0f, 1.0f, 1.0f, 1.0f};
    gls_state_float_init(GL_PATCH_DEFAULT_OUTER_LEVEL, 4, one4);
    gls_state_float_init(GL_PATCH_DEFAULT_INNER_LEVEL, 2, one4);
    
    double* va = malloc(cfg.max_vertex_attribs*4*sizeof(double));
    for (size_t i = 0; i < cfg.max_vertex_attribs*4; i++) va[i] = i%4==3 ? 1 : 0;
    gls_current_vertex_attrib_init(cfg.max_vertex_attribs*4, va);
    free(va);
    
    uint* types = malloc(cfg.max_vertex_attribs*sizeof(uint));
    for (int i = 0; i < cfg.max_vertex_attribs; i++) types[i] = GL_FLOAT;
    gls_current_vertex_attrib_types_init(cfg.max_vertex_attribs, types);
    free(types);
    
    GLenum draw_buffers[1] = {GL_BACK};
    gls_state_enum_init(GL_DRAW_BUFFER, 1, draw_buffers);
    
    int sample_mask_value[cfg.max_sample_mask_words];
    memset(sample_mask_value, 0xff, cfg.max_sample_mask_words*sizeof(int));
    gls_state_int_init(GL_SAMPLE_MASK_VALUE, cfg.max_sample_mask_words, sample_mask_value);
    
    uint draw_vao;
    F(glGenVertexArrays)(1, &draw_vao);
    F(glBindVertexArray)(draw_vao);
    gls_set_draw_vao(draw_vao);
    
    rev = *trc_get_context(trace);
    create_context_buffers(&rev);
    trc_set_context(ctx->trace, &rev);
    update_fb0_buffers(true, true, true, true);
    
    if (cfg.version >= 430) {
        F(glEnable)(GL_DEBUG_OUTPUT);
        F(glEnable)(GL_DEBUG_OUTPUT_SYNCHRONOUS);
        F(glDebugMessageCallback)(replay_debug_callback, ctx);
        F(glDebugMessageControl)(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, 0, GL_TRUE);
    }
}

static void test_host_config(const trc_replay_config_t* host, const trc_replay_config_t* trace) {
    typedef struct cap_info_t {
        const char* name;
        size_t offset;
    } cap_info_t;
    cap_info_t caps[] = {
        {"version", offsetof(trc_replay_config_t, version)},
        {"max_vertex_streams", offsetof(trc_replay_config_t, max_vertex_streams)},
        {"max_clip_distances", offsetof(trc_replay_config_t, max_clip_distances)},
        {"max_draw_buffers", offsetof(trc_replay_config_t, max_draw_buffers)},
        {"max_viewports", offsetof(trc_replay_config_t, max_viewports)},
        {"max_vertex_attribs", offsetof(trc_replay_config_t, max_vertex_attribs)},
        {"max_vertex_attrib_stride", offsetof(trc_replay_config_t, max_vertex_attrib_stride)},
        {"max_vertex_attrib_relative_offset", offsetof(trc_replay_config_t, max_vertex_attrib_relative_offset)},
        {"max_vertex_attrib_bindings", offsetof(trc_replay_config_t, max_vertex_attrib_bindings)},
        {"max_color_attachments", offsetof(trc_replay_config_t, max_color_attachments)},
        {"max_combined_texture_units", offsetof(trc_replay_config_t, max_combined_texture_units)},
        {"max_patch_vertices", offsetof(trc_replay_config_t, max_patch_vertices)},
        {"max_renderbuffer_size", offsetof(trc_replay_config_t, max_renderbuffer_size)},
        {"max_texture_size", offsetof(trc_replay_config_t, max_texture_size)},
        {"max_xfb_buffers", offsetof(trc_replay_config_t, max_xfb_buffers)},
        {"max_ubo_bindings", offsetof(trc_replay_config_t, max_ubo_bindings)},
        {"max_atomic_counter_buffer_bindings", offsetof(trc_replay_config_t, max_atomic_counter_buffer_bindings)},
        {"max_ssbo_bindings", offsetof(trc_replay_config_t, max_ssbo_bindings)}};
    size_t cap_count = sizeof(caps) / sizeof(caps[0]);
    
    for (size_t i = 0; i < cap_count; i++) {
        const char* name = caps[i].name;
        int host_val = *(const int*)((const uint8_t*)host+caps[i].offset);
        int trace_val = *(const int*)((const uint8_t*)trace+caps[i].offset);
        if (host_val < trace_val) {
            trc_add_warning(cmd, "Host value for capability '%s' is lower than that of trace: %d < %d",
                            name, host_val, trace_val);
        }
    }
}

static void handle_new_context_config(trc_gl_context_rev_t* rev, trace_extra_t* extra) {
    init_host_config(ctx, &rev->host_cfg);
    rev->trace_cfg = rev->host_cfg;
    trc_replay_config_t* cfg = &rev->trace_cfg;
    
    if (extra) {
        typedef struct option_info_t {
            const char* name;
            int* ptr;
        } option_info_t;
        
        //TODO: Use the array in test_host_config()
        const option_info_t options[] = {
            {"version", &cfg->version},
            {"max_vertex_streams", &cfg->max_vertex_streams},
            {"max_clip_distances", &cfg->max_clip_distances},
            {"max_draw_buffers", &cfg->max_draw_buffers},
            {"max_viewports", &cfg->max_viewports},
            {"max_vertex_attribs", &cfg->max_vertex_attribs},
            {"max_vertex_attrib_stride", &cfg->max_vertex_attrib_stride},
            {"max_vertex_attrib_relative_offset", &cfg->max_vertex_attrib_relative_offset},
            {"max_vertex_attrib_bindings", &cfg->max_vertex_attrib_bindings},
            {"max_color_attachments", &cfg->max_color_attachments},
            {"max_combined_texture_units", &cfg->max_combined_texture_units},
            {"max_patch_vertices", &cfg->max_patch_vertices},
            {"max_renderbuffer_size", &cfg->max_renderbuffer_size},
            {"max_texture_size", &cfg->max_texture_size},
            {"max_xfb_buffers", &cfg->max_xfb_buffers},
            {"max_ubo_bindings", &cfg->max_ubo_bindings},
            {"max_atomic_counter_buffer_bindings", &cfg->max_atomic_counter_buffer_bindings},
            {"max_ssbo_bindings", &cfg->max_ssbo_bindings},
            {"nvidia_xfb_object_bindings_bug", &cfg->nvidia_xfb_object_bindings_bug}};
        size_t option_count = sizeof(options) / sizeof(options[0]);
        
        data_reader_t dr = dr_new(extra->size, extra->data);
        
        uint32_t count;
        if (!dr_read_le(&dr, 4, &count, -1))
            ERROR2(, "Invalid %s extra: End of data", extra->name);
        
        for (uint32_t i = 0; i < count; i++) {
            uint32_t name_len;
            if (!dr_read_le(&dr, 4, &name_len, -1))
                ERROR2(, "Invalid %s extra: End of data", extra->name);
            char* name = calloc(name_len+1, 1);
            if (!dr_read(&dr, name_len, name)) {
                free(name);
                ERROR2(, "Invalid %s extra: End of data", extra->name);
            }
            
            int32_t value;
            if (!dr_read_le(&dr, 4, &value, -1))
                ERROR2(, "Invalid %s extra: End of data", extra->name);
            
            for (size_t j = 0; j < option_count; j++) {
                if (strcmp(options[j].name, name)) continue;
                *options[j].ptr = value;
                goto done;
            }
            trc_add_error(cmd, "Unknown target option '%s'", ctx->target_option_names[i]);
            done: ;
            
            free(name);
        }
    }
    
    test_host_config(&rev->host_cfg, &rev->trace_cfg);
}

glXMakeCurrent: //Display* p_dpy, GLXDrawable p_drawable, GLXContext p_ctx
    SDL_GLContext glctx = NULL;
    trc_namespace_t* global_ns = &ctx->trace->inspection.global_namespace;
    if (p_ctx) {
        if (!(glctx=((trc_gl_context_rev_t*)trc_get_obj(global_ns, TrcContext, p_ctx))->real))
            ERROR("Invalid GLX context");
    }
    
    if (SDL_GL_MakeCurrent(ctx->window, glctx) < 0)
        ERROR("Unable to make a context current");
    
    if (glctx) {
        reload_gl_funcs();
        trc_set_current_gl_context(ctx->trace, trc_lookup_name(global_ns, TrcContext, p_ctx, -1));
        if (!trc_get_context(ctx->trace)->made_current_before) {
            trc_gl_context_rev_t rev = *trc_get_context(ctx->trace);
            trace_extra_t* config_extra = trc_get_extrai(cmd, "replay/glXMakeCurrent/config", 0);
            //config_extra may be NULL but handle_new_context_config handles that
            handle_new_context_config(&rev, config_extra);
            rev.made_current_before = true;
            trc_set_context(ctx->trace, &rev);
            
            init_context();
            
            trace_extra_t* extra = trc_get_extra(cmd, "replay/glXMakeCurrent/drawable_size");
            if (!extra) ERROR("replay/glXMakeCurrent/drawable_size extra not found");
            if (extra->size != 8) ERROR("replay/glXMakeCurrent/drawable_size is not 8 bytes");
            int32_t width = ((int32_t*)extra->data)[0];
            int32_t height = ((int32_t*)extra->data)[1];
            if (width>=0 && height>=0) { //TODO: Move this into init_context()
                gls_set_drawable_width(width);
                gls_set_drawable_height(height);
                SDL_SetWindowSize(ctx->window, width, height);
                for (size_t i = 0; i < gls_get_state_int(GL_MAX_VIEWPORTS, 0); i++) {
                    gls_set_state_float(GL_VIEWPORT, i*4+2, width);
                    gls_set_state_float(GL_VIEWPORT, i*4+3, height);
                    gls_set_state_int(GL_SCISSOR_BOX, i*4+2, width);
                    gls_set_state_int(GL_SCISSOR_BOX, i*4+3, height);
                    if (i == 0) {
                        F(glViewport)(0, 0, width, height);
                        F(glScissor)(0, 0, width, height);
                    } else {
                        F(glViewportIndexedf)(i, 0, 0, width, height);
                        F(glScissorIndexed)(i, 0, 0, width, height);
                    }
                }
            }
        }
    } else {
        reset_gl_funcs();
        trc_set_current_gl_context(ctx->trace, NULL);
    }
    
    //Seems to be messing up the front buffer.
    //But the front buffer is still sometimes black when it should not be.
    /*SDL_GL_SetSwapInterval(0)*/

glXCreateContext: //Display* p_dpy, XVisualInfo* p_vis, GLXContext p_shareList, Bool p_direct
    trc_namespace_t* global_ns = &ctx->trace->inspection.global_namespace;
    
    const trc_gl_context_rev_t* shareList = NULL;
    if (p_shareList) {
        shareList = trc_get_obj(global_ns, TrcContext, (uint64_t)p_shareList);
        if (!shareList) ERROR("Invalid share context name");
    }
    
    SDL_GLContext last_ctx = SDL_GL_GetCurrentContext();
    if (shareList) {
        SDL_GL_MakeCurrent(ctx->window, shareList->real);
        SDL_GL_SetAttribute(SDL_GL_SHARE_WITH_CURRENT_CONTEXT, 1);
    } else {
        SDL_GL_SetAttribute(SDL_GL_SHARE_WITH_CURRENT_CONTEXT, 0);
    }
    
    SDL_GLContext res = SDL_GL_CreateContext(ctx->window);
    if (!res) {
        trc_add_error(cmd, "Unable to create context: %s", SDL_GetError());
        SDL_GL_MakeCurrent(ctx->window, last_ctx);
        RETURN;
    }
    reload_gl_funcs();
    
    trc_gl_context_rev_t rev;
    rev.real = res;
    if (shareList) rev.namespace = shareList->namespace;
    else rev.namespace = trc_create_namespace(ctx->trace);
    rev.priv_ns = trc_create_namespace(ctx->trace);
    rev.made_current_before = false;
    
    uint64_t fake = trc_get_ptr(&cmd->ret)[0];
    trc_obj_t* cur_ctx = trc_create_named_obj(global_ns, TrcContext, fake, &rev);
    
    SDL_GL_MakeCurrent(ctx->window, last_ctx);
    reload_gl_funcs();

glXCreateContextAttribsARB: //Display* p_dpy, GLXFBConfig p_config, GLXContext p_share_context, Bool p_direct, const int* p_attrib_list
    int last_major, last_minor, last_flags, last_profile;
    SDL_GL_GetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, &last_major);
    SDL_GL_GetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, &last_minor);
    SDL_GL_GetAttribute(SDL_GL_CONTEXT_FLAGS, &last_flags);
    SDL_GL_GetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, &last_profile);
    
    int major = last_major;
    int minor = last_minor;
    int flags = 0;
    int profile = 0;
    
    const int* attribs = p_attrib_list;
    while (*attribs) {
        int attr = *(attribs++);
        if (attr == GLX_CONTEXT_MAJOR_VERSION_ARB) {
            major = *(attribs++);
        } else if (attr == GLX_CONTEXT_MINOR_VERSION_ARB) {
            minor = *(attribs++);
        } else if (attr == GLX_CONTEXT_FLAGS_ARB) {
            int glx_flags = *(attribs++);
            flags = 0;
            if (glx_flags & GLX_CONTEXT_DEBUG_BIT_ARB)
                flags |= SDL_GL_CONTEXT_DEBUG_FLAG;
            if (glx_flags & GLX_CONTEXT_FORWARD_COMPATIBLE_BIT_ARB)
                flags |= SDL_GL_CONTEXT_FORWARD_COMPATIBLE_FLAG;
        } else if (attr == GLX_CONTEXT_PROFILE_MASK_ARB) {
            int mask = *(attribs++);
            profile = 0;
            if (mask & GLX_CONTEXT_CORE_PROFILE_BIT_ARB)
                profile = SDL_GL_CONTEXT_PROFILE_CORE;
            if (mask & GLX_CONTEXT_COMPATIBILITY_PROFILE_BIT_ARB)
                profile = SDL_GL_CONTEXT_PROFILE_COMPATIBILITY;
        } else {
            trc_add_warning(cmd, "Unnamed attribute: %d", attr);
            attribs++;
        }
    }
    
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, major);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, minor);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, flags);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, profile);
    
    trc_namespace_t* global_ns = &ctx->trace->inspection.global_namespace;
    
    const trc_gl_context_rev_t* share_ctx = NULL;
    if (p_share_context) {
        share_ctx = trc_get_obj(global_ns, TrcContext, (uint64_t)p_share_context);
        if (!share_ctx) ERROR("Invalid share context name");
    }
    
    SDL_GLContext last_ctx = SDL_GL_GetCurrentContext();
    if (share_ctx) {
        SDL_GL_MakeCurrent(ctx->window, share_ctx->real);
        SDL_GL_SetAttribute(SDL_GL_SHARE_WITH_CURRENT_CONTEXT, 1);
    } else {
        SDL_GL_SetAttribute(SDL_GL_SHARE_WITH_CURRENT_CONTEXT, 0);
    }
    
    SDL_GLContext res = SDL_GL_CreateContext(ctx->window);
    if (!res) {
        trc_add_error(cmd, "Unable to create context: %s", SDL_GetError());
        SDL_GL_MakeCurrent(ctx->window, last_ctx);
        RETURN;
    }
    reload_gl_funcs();
    
    trc_gl_context_rev_t rev;
    rev.real = res;
    if (share_ctx) rev.namespace = share_ctx->namespace;
    else rev.namespace = trc_create_namespace(ctx->trace);
    rev.priv_ns = trc_create_namespace(ctx->trace);
    rev.made_current_before = false;
    
    uint64_t fake = trc_get_ptr(&cmd->ret)[0];
    trc_obj_t* cur_ctx = trc_create_named_obj(global_ns, TrcContext, fake, &rev);
    
    SDL_GL_MakeCurrent(ctx->window, last_ctx);
    reload_gl_funcs();
    
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, last_major);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, last_minor);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, last_flags);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, last_profile);

glXDestroyContext: //Display* p_dpy, GLXContext p_ctx
    trc_namespace_t* global_ns = &ctx->trace->inspection.global_namespace;
    SDL_GLContext glctx = ((trc_gl_context_rev_t*)trc_get_obj(global_ns, TrcContext, (uint64_t)p_ctx))->real;
    if (!glctx) ERROR("Invalid context name");
    
    SDL_GL_DeleteContext(glctx);
    
    delete_obj(p_ctx, TrcContext);

glXSwapBuffers: //Display* p_dpy, GLXDrawable p_drawable
    if (!trc_get_current_gl_context(ctx->trace, -1)) ERROR("No current OpenGL context");
    SDL_GL_SwapWindow(ctx->window);
    update_fb0_buffers(false, true, false, false);

wip15DrawableSize: //GLsizei p_width, GLsizei p_height
    if (p_width < 0) p_width = 100;
    if (p_height < 0) p_height = 100;
    
    SDL_SetWindowSize(ctx->window, p_width, p_height);
    
    trc_gl_context_rev_t state = *trc_get_context(ctx->trace);
    if (state.drawable_width==p_width && state.drawable_height==p_height) return;
    state.drawable_width = p_width;
    state.drawable_height = p_height;
    trc_set_context(ctx->trace, &state);
    update_fb0_buffers(true, true, true, true);

glXSwapIntervalEXT: //Display* p_dpy, GLXDrawable p_drawable, int p_interval
    ;

glXSwapIntervalSGI: //int p_interval
    ;

glXGetProcAddressARB: //const GLubyte* p_procName
    ;

glXGetProcAddress: //const GLubyte* p_procName
    ;

glXQueryExtension: //Display* p_dpy, int * p_errorb, int * p_event
    ;

glXQueryVersion: //Display* p_dpy, int * p_maj, int * p_min
    ;

glXQueryDrawable: //Display* p_dpy, GLXDrawable p_draw, int p_attribute, unsigned int * p_value
    ;

glXChooseVisual: //Display* p_dpy, int p_screen, int * p_attribList
    ;

glXChooseFBConfig: //Display* p_dpy, int p_screen, const int * p_attrib_list, int * p_nelements
    ;

glXGetFBConfigs: //Display* p_dpy, int p_screen, int * p_nelements
    ;

glXGetFBConfigAttrib: //Display* p_dpy, GLXFBConfig p_config, int p_attribute, int * p_value
    ;

glXGetVisualFromFBConfig: //Display* p_dpy, GLXFBConfig p_config
    ;

glXChooseFBConfigSGIX: //Display* p_dpy, int p_screen, int * p_attrib_list, int * p_nelements
    ;

glXGetFBConfigAttribSGIX: //Display* p_dpy, GLXFBConfigSGIX p_config, int p_attribute, int * p_value
    ;

glXGetVisualFromFBConfigSGIX: //Display* p_dpy, GLXFBConfigSGIX p_config
    ;

glXGetClientString: //Display* p_dpy, int p_name
    ;

glXGetCurrentContext:
    ;

glXGetCurrentDisplay:
    ;

glXGetCurrentDrawable:
    ;

glXGetCurrentReadDrawable:
    ;

glXQueryExtensionsString: //Display* p_dpy, int p_screen
    ;
