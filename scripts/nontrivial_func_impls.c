static bool sample_param_double(trace_command_t* cmd, trc_gl_sample_params_t* params,
                                GLenum param, uint32_t count, const double* val) {
    switch (param) {
    case GL_TEXTURE_MIN_FILTER:
    case GL_TEXTURE_MAG_FILTER:
    case GL_TEXTURE_MIN_LOD:
    case GL_TEXTURE_MAX_LOD:
    case GL_TEXTURE_WRAP_S:
    case GL_TEXTURE_WRAP_T:
    case GL_TEXTURE_WRAP_R:
    case GL_TEXTURE_COMPARE_MODE:
    case GL_TEXTURE_COMPARE_FUNC:
        if (count != 1)
            ERROR2(true, "Expected 1 value. Got %u.", count);
        break;
    case GL_TEXTURE_BORDER_COLOR:
        if (count != 4)
            ERROR2(true, "Expected 4 values. Got %u.", count);
        break;
    }
    
    switch (param) {
    case GL_TEXTURE_MIN_FILTER:
        if (val[0]!=GL_LINEAR && val[0]!=GL_NEAREST && val[0]!=GL_NEAREST_MIPMAP_NEAREST &&
            val[0]!=GL_LINEAR_MIPMAP_NEAREST && val[0]!=GL_NEAREST_MIPMAP_LINEAR &&
            val[0]!=GL_LINEAR_MIPMAP_LINEAR)
            ERROR2(true, "Invalid minification filter.");
        break;
    case GL_TEXTURE_MAG_FILTER:
        if (val[0]!=GL_LINEAR && val[0]!=GL_NEAREST)
            ERROR2(true, "Invalid magnification filter.");
        break;
    case GL_TEXTURE_WRAP_S:
    case GL_TEXTURE_WRAP_T:
    case GL_TEXTURE_WRAP_R:
        if (val[0]!=GL_CLAMP_TO_EDGE && val[0]!=GL_CLAMP_TO_BORDER && val[0]!=GL_MIRRORED_REPEAT &&
            val[0]!=GL_REPEAT && val[0]!=GL_MIRROR_CLAMP_TO_EDGE && val[0]!=GL_CLAMP_TO_EDGE)
            ERROR2(true, "Invalid wrap mode.");
        break;
    case GL_TEXTURE_COMPARE_MODE:
        if (val[0]!=GL_COMPARE_REF_TO_TEXTURE && val[0]!=GL_NONE)
            ERROR2(true, "Invalid compare mode.");
        break;
    case GL_TEXTURE_COMPARE_FUNC:
        if (val[0]!=GL_LEQUAL && val[0]!=GL_GEQUAL && val[0]!=GL_LESS && val[0]!=GL_GREATER &&
            val[0]!=GL_EQUAL && val[0]!=GL_NOTEQUAL && val[0]!=GL_ALWAYS && val[0]!=GL_NEVER)
            ERROR2(true, "Invalid compare function.");
        break;
    }
    
    switch (param) {
    case GL_TEXTURE_MIN_FILTER: params->min_filter = val[0]; break;
    case GL_TEXTURE_MAG_FILTER: params->mag_filter = val[0]; break;
    case GL_TEXTURE_MIN_LOD: params->min_lod = val[0]; break;
    case GL_TEXTURE_MAX_LOD: params->max_lod = val[0]; break;
    case GL_TEXTURE_WRAP_S: params->wrap_s = val[0]; break;
    case GL_TEXTURE_WRAP_T: params->wrap_t = val[0]; break;
    case GL_TEXTURE_WRAP_R: params->wrap_r = val[0]; break;
    case GL_TEXTURE_BORDER_COLOR:
        for (uint i = 0; i < 4; i++) params->border_color[i] = val[i];
        break;
    case GL_TEXTURE_COMPARE_MODE: params->compare_mode = val[0]; break;
    case GL_TEXTURE_COMPARE_FUNC: params->compare_func = val[0]; break;
    }
    
    return false;
}

static void replay_create_context_buffers(trace_t* trace, trc_gl_context_rev_t* rev) {
    size_t size = rev->drawable_width * rev->drawable_height * 4;
    rev->front_color_buffer = trc_create_data(trace, size, NULL, TRC_DATA_NO_ZERO);
    rev->back_color_buffer = trc_create_data(trace, size, NULL, TRC_DATA_NO_ZERO);
    rev->back_depth_buffer = trc_create_data(trace, size, NULL, TRC_DATA_NO_ZERO);
    rev->back_stencil_buffer = trc_create_data(trace, size, NULL, TRC_DATA_NO_ZERO);
    
    void* data = trc_map_data(rev->front_color_buffer, TRC_MAP_REPLACE);
    memset(data, 0, size);
    trc_unmap_freeze_data(trace, rev->front_color_buffer);
    
    data = trc_map_data(rev->back_color_buffer, TRC_MAP_REPLACE);
    memset(data, 0, size);
    trc_unmap_freeze_data(trace, rev->back_color_buffer);
    
    data = trc_map_data(rev->back_depth_buffer, TRC_MAP_REPLACE);
    memset(data, 0, size);
    trc_unmap_freeze_data(trace, rev->back_depth_buffer);
    
    data = trc_map_data(rev->back_stencil_buffer, TRC_MAP_REPLACE);
    memset(data, 0, size);
    trc_unmap_freeze_data(trace, rev->back_stencil_buffer);
}

static void replay_update_fb0_buffers(trc_replay_context_t* ctx, bool backcolor, bool frontcolor, bool depth, bool stencil);

static void init_context(trc_replay_context_t* ctx) {
    trace_t* trace = ctx->trace;
    
    trc_gl_state_set_made_current_before(trace, false);
    
    GLint major, minor;
    F(glGetIntegerv)(GL_MAJOR_VERSION, &major);
    F(glGetIntegerv)(GL_MINOR_VERSION, &minor);
    uint ver = major*100 + minor*10;
    
    int w, h;
    SDL_GL_GetDrawableSize(ctx->window, &w, &h);
    trc_gl_state_set_drawable_width(trace, w);
    trc_gl_state_set_drawable_height(trace, h);
    trc_gl_state_set_bound_buffer(trace, GL_ARRAY_BUFFER, 0);
    trc_gl_state_set_bound_buffer(trace, GL_ATOMIC_COUNTER_BUFFER, 0);
    trc_gl_state_set_bound_buffer(trace, GL_COPY_READ_BUFFER, 0);
    trc_gl_state_set_bound_buffer(trace, GL_COPY_WRITE_BUFFER, 0);
    trc_gl_state_set_bound_buffer(trace, GL_DISPATCH_INDIRECT_BUFFER, 0);
    trc_gl_state_set_bound_buffer(trace, GL_DRAW_INDIRECT_BUFFER, 0);
    trc_gl_state_set_bound_buffer(trace, GL_ELEMENT_ARRAY_BUFFER, 0);
    trc_gl_state_set_bound_buffer(trace, GL_PIXEL_PACK_BUFFER, 0);
    trc_gl_state_set_bound_buffer(trace, GL_PIXEL_UNPACK_BUFFER, 0);
    trc_gl_state_set_bound_buffer(trace, GL_QUERY_BUFFER, 0);
    trc_gl_state_set_bound_buffer(trace, GL_SHADER_STORAGE_BUFFER, 0);
    trc_gl_state_set_bound_buffer(trace, GL_TEXTURE_BUFFER, 0);
    trc_gl_state_set_bound_buffer(trace, GL_TRANSFORM_FEEDBACK_BUFFER, 0);
    trc_gl_state_set_bound_buffer(trace, GL_UNIFORM_BUFFER, 0);
    trc_gl_state_set_bound_program(trace, 0);
    trc_gl_state_set_bound_vao(trace, 0);
    trc_gl_state_set_bound_renderbuffer(trace, 0);
    trc_gl_state_set_read_framebuffer(trace, 0);
    trc_gl_state_set_draw_framebuffer(trace, 0);
    trc_gl_state_set_active_texture_unit(trace, 0);
    
    GLint max_query_bindings = 64; //TODO
    trc_gl_state_bound_queries_init(trace, GL_SAMPLES_PASSED, max_query_bindings, NULL);
    trc_gl_state_bound_queries_init(trace, GL_ANY_SAMPLES_PASSED, max_query_bindings, NULL);
    trc_gl_state_bound_queries_init(trace, GL_ANY_SAMPLES_PASSED_CONSERVATIVE, max_query_bindings, NULL);
    trc_gl_state_bound_queries_init(trace, GL_PRIMITIVES_GENERATED, max_query_bindings, NULL);
    trc_gl_state_bound_queries_init(trace, GL_TRANSFORM_FEEDBACK_PRIMITIVES_WRITTEN, max_query_bindings, NULL);
    trc_gl_state_bound_queries_init(trace, GL_TIME_ELAPSED, max_query_bindings, NULL);
    
    GLint max_clip_distances, max_draw_buffers, max_viewports;
    GLint max_vertex_attribs, max_color_attachments, max_tex_units;
    GLint max_uniform_buffer_bindings, max_patch_vertices, max_renderbuffer_size;
    GLint max_texture_size;
    F(glGetIntegerv)(GL_MAX_CLIP_DISTANCES, &max_clip_distances);
    F(glGetIntegerv)(GL_MAX_DRAW_BUFFERS, &max_draw_buffers);
    if (ver>=410) F(glGetIntegerv)(GL_MAX_VIEWPORTS, &max_viewports);
    else max_viewports = 1;
    F(glGetIntegerv)(GL_MAX_VERTEX_ATTRIBS, &max_vertex_attribs);
    F(glGetIntegerv)(GL_MAX_COLOR_ATTACHMENTS, &max_color_attachments);
    F(glGetIntegerv)(GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS, &max_tex_units);
    F(glGetIntegerv)(GL_MAX_UNIFORM_BUFFER_BINDINGS, &max_uniform_buffer_bindings);
    F(glGetIntegerv)(GL_MAX_PATCH_VERTICES, &max_patch_vertices);
    F(glGetIntegerv)(GL_MAX_RENDERBUFFER_SIZE, &max_renderbuffer_size);
    F(glGetIntegerv)(GL_MAX_TEXTURE_SIZE, &max_texture_size);
    
    trc_gl_state_state_int_init1(trace, GL_MAX_CLIP_DISTANCES, max_clip_distances);
    trc_gl_state_state_int_init1(trace, GL_MAX_DRAW_BUFFERS, max_draw_buffers);
    trc_gl_state_state_int_init1(trace, GL_MAX_VIEWPORTS, max_viewports);
    trc_gl_state_state_int_init1(trace, GL_MAX_VERTEX_ATTRIBS, max_vertex_attribs);
    trc_gl_state_state_int_init1(trace, GL_MAX_COLOR_ATTACHMENTS, max_color_attachments);
    trc_gl_state_state_int_init1(trace, GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS, max_tex_units);
    trc_gl_state_state_int_init1(trace, GL_MAX_UNIFORM_BUFFER_BINDINGS, max_uniform_buffer_bindings);
    trc_gl_state_state_int_init1(trace, GL_MAX_PATCH_VERTICES, max_patch_vertices);
    trc_gl_state_state_int_init1(trace, GL_MAX_RENDERBUFFER_SIZE, max_renderbuffer_size);
    trc_gl_state_state_int_init1(trace, GL_MAX_TEXTURE_SIZE, max_texture_size);
    trc_gl_state_state_int_init1(trace, GL_MAJOR_VERSION, major);
    trc_gl_state_state_int_init1(trace, GL_MINOR_VERSION, minor);
    trc_gl_state_set_ver(trace, ver);
    
    trc_gl_state_bound_textures_init(trace, GL_TEXTURE_1D, max_tex_units, NULL);
    trc_gl_state_bound_textures_init(trace, GL_TEXTURE_2D, max_tex_units, NULL);
    trc_gl_state_bound_textures_init(trace, GL_TEXTURE_3D, max_tex_units, NULL);
    trc_gl_state_bound_textures_init(trace, GL_TEXTURE_1D_ARRAY, max_tex_units, NULL);
    trc_gl_state_bound_textures_init(trace, GL_TEXTURE_2D_ARRAY, max_tex_units, NULL);
    trc_gl_state_bound_textures_init(trace, GL_TEXTURE_RECTANGLE, max_tex_units, NULL);
    trc_gl_state_bound_textures_init(trace, GL_TEXTURE_CUBE_MAP, max_tex_units, NULL);
    trc_gl_state_bound_textures_init(trace, GL_TEXTURE_CUBE_MAP_ARRAY, max_tex_units, NULL);
    trc_gl_state_bound_textures_init(trace, GL_TEXTURE_BUFFER, max_tex_units, NULL);
    trc_gl_state_bound_textures_init(trace, GL_TEXTURE_2D_MULTISAMPLE, max_tex_units, NULL);
    trc_gl_state_bound_textures_init(trace, GL_TEXTURE_2D_MULTISAMPLE_ARRAY, max_tex_units, NULL);
    
    trc_gl_state_enabled_init(trace, GL_BLEND, max_draw_buffers, NULL);
    trc_gl_state_enabled_init(trace, GL_CLIP_DISTANCE0, max_clip_distances, NULL);
    trc_gl_state_enabled_init1(trace, GL_COLOR_LOGIC_OP, false);
    trc_gl_state_enabled_init1(trace, GL_CULL_FACE, false);
    trc_gl_state_enabled_init1(trace, GL_DEBUG_OUTPUT, false);
    trc_gl_state_enabled_init1(trace, GL_DEBUG_OUTPUT_SYNCHRONOUS, false);
    trc_gl_state_enabled_init1(trace, GL_DEPTH_CLAMP, false);
    trc_gl_state_enabled_init1(trace, GL_DEPTH_TEST, false);
    trc_gl_state_enabled_init1(trace, GL_DITHER, true);
    trc_gl_state_enabled_init1(trace, GL_FRAMEBUFFER_SRGB, false);
    trc_gl_state_enabled_init1(trace, GL_LINE_SMOOTH, false);
    trc_gl_state_enabled_init1(trace, GL_MULTISAMPLE, false);
    trc_gl_state_enabled_init1(trace, GL_POLYGON_OFFSET_FILL, false);
    trc_gl_state_enabled_init1(trace, GL_POLYGON_OFFSET_LINE, false);
    trc_gl_state_enabled_init1(trace, GL_POLYGON_OFFSET_POINT, false);
    trc_gl_state_enabled_init1(trace, GL_POLYGON_SMOOTH, false);
    trc_gl_state_enabled_init1(trace, GL_PRIMITIVE_RESTART, false);
    trc_gl_state_enabled_init1(trace, GL_PRIMITIVE_RESTART_FIXED_INDEX, false);
    trc_gl_state_enabled_init1(trace, GL_RASTERIZER_DISCARD, false);
    trc_gl_state_enabled_init1(trace, GL_SAMPLE_ALPHA_TO_COVERAGE, false);
    trc_gl_state_enabled_init1(trace, GL_SAMPLE_ALPHA_TO_ONE, false);
    trc_gl_state_enabled_init1(trace, GL_SAMPLE_COVERAGE, false);
    trc_gl_state_enabled_init1(trace, GL_SAMPLE_SHADING, false);
    trc_gl_state_enabled_init1(trace, GL_SAMPLE_MASK, false);
    trc_gl_state_enabled_init(trace, GL_SCISSOR_TEST, max_viewports, NULL);
    trc_gl_state_enabled_init1(trace, GL_STENCIL_TEST, false);
    trc_gl_state_enabled_init1(trace, GL_TEXTURE_CUBE_MAP_SEAMLESS, false);
    trc_gl_state_enabled_init1(trace, GL_PROGRAM_POINT_SIZE, false);
    
    trc_gl_state_state_bool_init1(trace, GL_DEPTH_WRITEMASK, GL_TRUE);
    bool color_mask[max_draw_buffers*4];
    for (size_t i = 0; i < max_draw_buffers*4; i++) color_mask[i] = GL_TRUE;
    trc_gl_state_state_bool_init(trace, GL_COLOR_WRITEMASK, max_draw_buffers*4, color_mask);
    trc_gl_state_state_int_init1(trace, GL_STENCIL_WRITEMASK, 0xffffffff);
    trc_gl_state_state_int_init1(trace, GL_STENCIL_BACK_WRITEMASK, 0xffffffff);
    
    trc_gl_state_state_bool_init1(trace, GL_PACK_SWAP_BYTES, GL_FALSE);
    trc_gl_state_state_bool_init1(trace, GL_PACK_LSB_FIRST, GL_FALSE);
    trc_gl_state_state_bool_init1(trace, GL_UNPACK_SWAP_BYTES, GL_FALSE);
    trc_gl_state_state_bool_init1(trace, GL_UNPACK_LSB_FIRST, GL_FALSE);
    
    trc_gl_state_state_int_init1(trace, GL_PACK_ROW_LENGTH, 0);
    trc_gl_state_state_int_init1(trace, GL_PACK_IMAGE_HEIGHT, 0);
    trc_gl_state_state_int_init1(trace, GL_PACK_SKIP_ROWS, 0);
    trc_gl_state_state_int_init1(trace, GL_PACK_SKIP_PIXELS, 0);
    trc_gl_state_state_int_init1(trace, GL_PACK_SKIP_IMAGES, 0);
    trc_gl_state_state_int_init1(trace, GL_PACK_ALIGNMENT, 4);
    trc_gl_state_state_int_init1(trace, GL_UNPACK_ROW_LENGTH, 0);
    trc_gl_state_state_int_init1(trace, GL_UNPACK_IMAGE_HEIGHT, 0);
    trc_gl_state_state_int_init1(trace, GL_UNPACK_SKIP_ROWS, 0);
    trc_gl_state_state_int_init1(trace, GL_UNPACK_SKIP_PIXELS, 0);
    trc_gl_state_state_int_init1(trace, GL_UNPACK_SKIP_IMAGES, 0);
    trc_gl_state_state_int_init1(trace, GL_UNPACK_ALIGNMENT, 4);
    
    trc_gl_state_state_int_init1(trace, GL_STENCIL_CLEAR_VALUE, 0);
    trc_gl_state_state_float_init1(trace, GL_DEPTH_CLEAR_VALUE, 0);
    float color_clear[4] = {0.0f, 0.0f, 0.0f, 0.0f};
    trc_gl_state_state_float_init(trace, GL_COLOR_CLEAR_VALUE, 4, color_clear);
    
    trc_gl_state_state_float_init1(trace, GL_POINT_SIZE, 1.0f);
    trc_gl_state_state_float_init1(trace, GL_LINE_WIDTH, 1.0f);
    
    trc_gl_state_state_float_init1(trace, GL_POLYGON_OFFSET_UNITS, 0.0f);
    trc_gl_state_state_float_init1(trace, GL_POLYGON_OFFSET_FACTOR, 0.0f);
    
    trc_gl_state_state_float_init1(trace, GL_SAMPLE_COVERAGE_VALUE, 1.0f);
    trc_gl_state_state_bool_init1(trace, GL_SAMPLE_COVERAGE_INVERT, false);
    
    trc_gl_state_state_enum_init1(trace, GL_STENCIL_FUNC, GL_ALWAYS);
    trc_gl_state_state_int_init1(trace, GL_STENCIL_REF, 0);
    trc_gl_state_state_int_init1(trace, GL_STENCIL_VALUE_MASK, 0xffffffff);
    trc_gl_state_state_enum_init1(trace, GL_STENCIL_BACK_FUNC, GL_ALWAYS);
    trc_gl_state_state_int_init1(trace, GL_STENCIL_BACK_REF, 0);
    trc_gl_state_state_int_init1(trace, GL_STENCIL_BACK_VALUE_MASK, 0xffffffff);
    trc_gl_state_state_enum_init1(trace, GL_STENCIL_FAIL, GL_KEEP);
    trc_gl_state_state_enum_init1(trace, GL_STENCIL_PASS_DEPTH_PASS, GL_KEEP);
    trc_gl_state_state_enum_init1(trace, GL_STENCIL_PASS_DEPTH_FAIL, GL_KEEP);
    trc_gl_state_state_enum_init1(trace, GL_STENCIL_BACK_FAIL, GL_KEEP);
    trc_gl_state_state_enum_init1(trace, GL_STENCIL_BACK_PASS_DEPTH_PASS, GL_KEEP);
    trc_gl_state_state_enum_init1(trace, GL_STENCIL_BACK_PASS_DEPTH_FAIL, GL_KEEP);
    
    GLenum blenddata[max_draw_buffers];
    for (size_t i = 0; i < max_draw_buffers; i++) blenddata[i] = GL_ONE;
    trc_gl_state_state_enum_init(trace, GL_BLEND_SRC_RGB, max_draw_buffers, blenddata);
    trc_gl_state_state_enum_init(trace, GL_BLEND_SRC_ALPHA, max_draw_buffers, blenddata);
    
    for (size_t i = 0; i < max_draw_buffers; i++) blenddata[i] = GL_ZERO;
    trc_gl_state_state_enum_init(trace, GL_BLEND_DST_RGB, max_draw_buffers, blenddata);
    trc_gl_state_state_enum_init(trace, GL_BLEND_DST_ALPHA, max_draw_buffers, blenddata);
    
    float blend_color[4] = {0.0f, 0.0f, 0.0f, 0.0f};
    trc_gl_state_state_float_init(trace, GL_BLEND_COLOR, 4, blend_color);
    
    for (size_t i = 0; i < max_draw_buffers; i++) blenddata[i] = GL_FUNC_ADD;
    trc_gl_state_state_enum_init(trace, GL_BLEND_EQUATION_RGB, max_draw_buffers, blenddata);
    trc_gl_state_state_enum_init(trace, GL_BLEND_EQUATION_ALPHA, max_draw_buffers, blenddata);
    
    trc_gl_state_set_hints(trace, GL_FRAGMENT_SHADER_DERIVATIVE_HINT, GL_DONT_CARE);
    trc_gl_state_set_hints(trace, GL_LINE_SMOOTH_HINT, GL_DONT_CARE);
    trc_gl_state_set_hints(trace, GL_POLYGON_SMOOTH_HINT, GL_DONT_CARE);
    trc_gl_state_set_hints(trace, GL_TEXTURE_COMPRESSION_HINT, GL_DONT_CARE);
    
    float zerof[max_viewports*4];
    for (size_t i = 0; i < max_viewports*4; i++) zerof[i] = 0.0f;
    trc_gl_state_state_float_init(trace, GL_VIEWPORT, max_viewports*4, zerof);
    trc_gl_state_state_int_init(trace, GL_SCISSOR_BOX, max_viewports*4, NULL);
    float depth_range[max_viewports*2];
    for (int i = 0; i < max_viewports; i++) depth_range[i] = (float[]){0.0f, 1.0f}[i%2];
    trc_gl_state_state_float_init(trace, GL_DEPTH_RANGE, max_viewports*2, depth_range);
    
    trc_gl_state_state_enum_init1(trace, GL_PROVOKING_VERTEX, GL_LAST_VERTEX_CONVENTION);
    trc_gl_state_state_enum_init1(trace, GL_LOGIC_OP_MODE, GL_COPY);
    trc_gl_state_state_int_init1(trace, GL_PRIMITIVE_RESTART_INDEX, 0);
    trc_gl_state_state_enum_init1(trace, GL_POLYGON_MODE, GL_FILL);
    trc_gl_state_state_enum_init1(trace, GL_CULL_FACE_MODE, GL_BACK);
    trc_gl_state_state_enum_init1(trace, GL_FRONT_FACE, GL_CCW);
    trc_gl_state_state_enum_init1(trace, GL_DEPTH_FUNC, GL_LESS);
    trc_gl_state_state_float_init1(trace, GL_POINT_FADE_THRESHOLD_SIZE, GL_UPPER_LEFT);
    trc_gl_state_state_enum_init1(trace, GL_POINT_SPRITE_COORD_ORIGIN, GL_UPPER_LEFT);
    trc_gl_state_state_float_init1(trace, GL_MIN_SAMPLE_SHADING_VALUE, 0.0f);
    
    trc_gl_state_state_int_init1(trace, GL_PATCH_VERTICES, 3);
    float one4[4] = {1.0f, 1.0f, 1.0f, 1.0f};
    trc_gl_state_state_float_init(trace, GL_PATCH_DEFAULT_OUTER_LEVEL, 4, one4);
    trc_gl_state_state_float_init(trace, GL_PATCH_DEFAULT_INNER_LEVEL, 2, one4);
    
    double* va = malloc((max_vertex_attribs-1)*4*sizeof(double));
    for (size_t i = 0; i < (max_vertex_attribs-1)*4; i++) va[i] = i%4==3 ? 1 : 0;
    trc_gl_state_state_double_init(trace, GL_CURRENT_VERTEX_ATTRIB, (max_vertex_attribs-1)*4, va);
    free(va);
    
    GLenum draw_buffers[1] = {GL_BACK};
    trc_gl_state_state_enum_init(trace, GL_DRAW_BUFFER, 1, draw_buffers);
    
    uint draw_vao;
    F(glGenVertexArrays)(1, &draw_vao);
    F(glBindVertexArray)(draw_vao);
    trc_gl_state_set_draw_vao(trace, draw_vao);
    
    //replay_create_context_buffers(ctx->trace, &rev);
    replay_update_fb0_buffers(ctx, true, true, true, true);
}

static void replay_pixel_store(trc_replay_context_t* ctx, trace_command_t* cmd, GLenum pname, GLint param) {
    switch (pname) {
    case GL_PACK_SWAP_BYTES:
    case GL_PACK_LSB_FIRST:
    case GL_UNPACK_SWAP_BYTES:
    case GL_UNPACK_LSB_FIRST: trc_gl_state_set_state_bool(ctx->trace, pname, 0, param!=0); break;
    case GL_PACK_ROW_LENGTH:
    case GL_PACK_IMAGE_HEIGHT:
    case GL_PACK_SKIP_ROWS:
    case GL_PACK_SKIP_PIXELS:
    case GL_PACK_SKIP_IMAGES:
    case GL_UNPACK_ROW_LENGTH:
    case GL_UNPACK_IMAGE_HEIGHT:
    case GL_UNPACK_SKIP_ROWS:
    case GL_UNPACK_SKIP_PIXELS:
    case GL_UNPACK_SKIP_IMAGES:
        if (param < 0) ERROR2(, "param is negative");
        trc_gl_state_set_state_int(ctx->trace, pname, 0, param);
        break;
    case GL_PACK_ALIGNMENT:
    case GL_UNPACK_ALIGNMENT:
        if (param!=1 && param!=2 && param!=4 && param!=8)
            ERROR2(, "param is not 1, 2, 4, or 8");
        trc_gl_state_set_state_int(ctx->trace, pname, 0, param);
        break;
    default:
        ERROR2(, "Invalid parameter");
    }
    F(glPixelStorei)(pname, param);
}

static void replay_set_texture_image(trace_t* trace, uint fake, const trc_gl_texture_rev_t* rev, uint level, uint face,
                                     uint internal_format, uint width, uint height, uint depth, trc_data_t* data) {
    trc_gl_texture_image_t img;
    memset(&img, 0, sizeof(img)); //Fill in padding to fix use of uninitialized memory errors because of compression
    img.face = face;
    img.level = level;
    img.internal_format = internal_format;
    img.width = width;
    img.height = height;
    img.depth = depth;
    img.data = data;
    
    size_t img_count = rev->images->size / sizeof(trc_gl_texture_image_t);
    trc_gl_texture_image_t* newimages = malloc((img_count+1)*sizeof(trc_gl_texture_image_t));
    
    trc_gl_texture_image_t* images = trc_map_data(rev->images, TRC_MAP_READ);
    bool replaced = false;
    for (size_t i = 0; i < img_count; i++) {
        if (images[i].face==img.face && images[i].level==img.level) {
            newimages[i] = img;
            replaced = true;
        } else {
            newimages[i] = images[i];
        }
    }
    trc_unmap_data(rev->images);
    
    trc_gl_texture_rev_t newrev = *rev;
    if (!replaced) newimages[img_count++] = img;
    
    size_t size = img_count * sizeof(trc_gl_texture_image_t);
    newrev.images = trc_create_data_no_copy(trace, size, newimages, TRC_DATA_IMMUTABLE);
    
    trc_set_gl_texture(trace, fake, &newrev);
}

static void replay_update_tex_image(trc_replay_context_t* ctx, const trc_gl_texture_rev_t* tex,
                                    uint fake, uint level, uint face) {
    GLenum prevget;
    switch (tex->type) {
    case GL_TEXTURE_1D: prevget = GL_TEXTURE_BINDING_1D; break;
    case GL_TEXTURE_2D: prevget = GL_TEXTURE_BINDING_2D; break;
    case GL_TEXTURE_3D: prevget = GL_TEXTURE_BINDING_3D; break;
    case GL_TEXTURE_1D_ARRAY: prevget = GL_TEXTURE_BINDING_1D_ARRAY; break;
    case GL_TEXTURE_2D_ARRAY: prevget = GL_TEXTURE_BINDING_2D_ARRAY; break;
    case GL_TEXTURE_RECTANGLE: prevget = GL_TEXTURE_BINDING_RECTANGLE; break;
    case GL_TEXTURE_CUBE_MAP: prevget = GL_TEXTURE_BINDING_CUBE_MAP; break;
    case GL_TEXTURE_CUBE_MAP_ARRAY: prevget = GL_TEXTURE_BINDING_CUBE_MAP; break;
    case GL_TEXTURE_BUFFER: return; //TODO: Error
    case GL_TEXTURE_2D_MULTISAMPLE: return; //TODO: Handle
    case GL_TEXTURE_2D_MULTISAMPLE_ARRAY: return; //TODO: Handle
    }
    GLint prev;
    F(glGetIntegerv)(prevget, &prev);
    F(glBindTexture)(tex->type, tex->real);
    
    GLint width, height, depth, internal_format;
    F(glGetTexLevelParameteriv)(tex->type, level, GL_TEXTURE_WIDTH, &width);
    F(glGetTexLevelParameteriv)(tex->type, level, GL_TEXTURE_HEIGHT, &height);
    F(glGetTexLevelParameteriv)(tex->type, level, GL_TEXTURE_DEPTH, &depth);
    F(glGetTexLevelParameteriv)(tex->type, level, GL_TEXTURE_INTERNAL_FORMAT, &internal_format);
    if (!width) width = 1;
    if (!height) height = 1;
    if (!depth) depth = 1;
    
    uint dtype = 0; //0=uint32_t 1=int32_t 2=float 3=float+unused24+uint8
    uint ftype = 0; //0=normal 1=depth 2=stencil 3=depthstencil
    uint components = 0;
    switch (internal_format) {
    case GL_DEPTH_COMPONENT: dtype = 2; ftype = 1; components = 1; break;
    case GL_DEPTH_COMPONENT16: dtype = 2; ftype = 1; components = 1; break;
    case GL_DEPTH_COMPONENT24: dtype = 2; ftype = 1; components = 1; break;
    case GL_DEPTH_COMPONENT32: dtype = 2; ftype = 1; components = 1; break;
    case GL_DEPTH_COMPONENT32F: dtype = 2; ftype = 1; components = 1; break;
    case GL_DEPTH_STENCIL: dtype = 3; ftype = 3; components = 2; break;
    case GL_DEPTH24_STENCIL8: dtype = 3; ftype = 3; components = 2; break;
    case GL_DEPTH32F_STENCIL8: dtype = 3; ftype = 3; components = 2; break;
    case GL_STENCIL_INDEX: dtype = 0; ftype = 2; components = 1; break;
    case GL_STENCIL_INDEX1: dtype = 0; ftype = 2; components = 1; break;
    case GL_STENCIL_INDEX4: dtype = 0; ftype = 2; components = 1; break;
    case GL_STENCIL_INDEX8: dtype = 0; ftype = 2; components = 1; break;
    case GL_STENCIL_INDEX16: dtype = 0; ftype = 2; components = 1; break;
    case GL_RED: dtype = 2; components = 1; break;
    case GL_RG: dtype = 2; components = 2; break;
    case GL_RGB: dtype = 2; components = 3; break;
    case GL_RGBA: dtype = 2; components = 4; break;
    case GL_R8: dtype = 2; components = 1; break;
    case GL_R8_SNORM: dtype = 2; components = 1; break;
    case GL_R16: dtype = 2; components = 1; break;
    case GL_R16_SNORM: dtype = 2; components = 1; break;
    case GL_RG8: dtype = 2; components = 2; break;
    case GL_RG8_SNORM: dtype = 2; components = 2; break;
    case GL_RG16: dtype = 2; components = 2; break;
    case GL_RG16_SNORM: dtype = 2; components = 2; break;
    case GL_R3_G3_B2: dtype = 2; components = 3; break;
    case GL_RGB4: dtype = 2; components = 3; break;
    case GL_RGB5: dtype = 2; components = 3; break;
    case GL_RGB8: dtype = 2; components = 3; break;
    case GL_RGB8_SNORM: dtype = 2; components = 3; break;
    case GL_RGB10: dtype = 2; components = 3; break;
    case GL_RGB12: dtype = 2; components = 3; break;
    case GL_RGB16_SNORM: dtype = 2; components = 3; break;
    case GL_RGBA2: dtype = 2; components = 4; break;
    case GL_RGBA4: dtype = 2; components = 4; break;
    case GL_RGB5_A1: dtype = 2; components = 4; break;
    case GL_RGBA8: dtype = 2; components = 4; break;
    case GL_RGBA8_SNORM: dtype = 2; components = 4; break;
    case GL_RGB10_A2: dtype = 2; components = 4; break;
    case GL_RGB10_A2UI: dtype = 0; components = 4; break;
    case GL_RGBA12: dtype = 2; components = 4; break;
    case GL_RGBA16: dtype = 2; components = 4; break;
    case GL_SRGB8: dtype = 2; components = 3; break;
    case GL_SRGB8_ALPHA8: dtype = 2; components = 4; break;
    case GL_R16F: dtype = 2; components = 1; break;
    case GL_RG16F: dtype = 2; components = 2; break;
    case GL_RGB16F: dtype = 2; components = 3; break;
    case GL_RGBA16F: dtype = 2; components = 4; break;
    case GL_R32F: dtype = 2; components = 1; break;
    case GL_RG32F: dtype = 2; components = 2; break;
    case GL_RGB32F: dtype = 2; components = 3; break;
    case GL_RGBA32F: dtype = 2; components = 4; break;
    case GL_R11F_G11F_B10F: dtype = 2; components = 3; break;
    case GL_RGB9_E5: dtype = 2; components = 3; break;
    case GL_R8I: dtype = 1; components = 1; break;
    case GL_R8UI: dtype = 0; components = 1; break;
    case GL_R16I: dtype = 1; components = 1; break;
    case GL_R16UI: dtype = 0; components = 1; break;
    case GL_R32I: dtype = 1; components = 1; break;
    case GL_R32UI: dtype = 0; components = 1; break;
    case GL_RG8I: dtype = 1; components = 2; break;
    case GL_RG8UI: dtype = 0; components = 2; break;
    case GL_RG16I: dtype = 1; components = 2; break;
    case GL_RG16UI: dtype = 0; components = 2; break;
    case GL_RG32I: dtype = 1; components = 2; break;
    case GL_RG32UI: dtype = 0; components = 2; break;
    case GL_RGB8I: dtype = 1; components = 3; break;
    case GL_RGB8UI: dtype = 0; components = 3; break;
    case GL_RGB16I: dtype = 1; components = 3; break;
    case GL_RGB16UI: dtype = 0; components = 3; break;
    case GL_RGB32I: dtype = 1; components = 3; break;
    case GL_RGB32UI: dtype = 0; components = 3; break;
    case GL_RGBA8I: dtype = 1; components = 4; break;
    case GL_RGBA8UI: dtype = 0; components = 4; break;
    case GL_RGBA16I: dtype = 1; components = 4; break;
    case GL_RGBA16UI: dtype = 0; components = 4; break;
    case GL_RGBA32I: dtype = 1; components = 4; break;
    case GL_RGBA32UI: dtype = 0; components = 4; break;
    case GL_COMPRESSED_RED: dtype = 2; components = 1; break;
    case GL_COMPRESSED_RG: dtype = 2; components = 2; break;
    case GL_COMPRESSED_RGB: dtype = 2; components = 3; break;
    case GL_COMPRESSED_RGBA: dtype = 2; components = 4; break;
    case GL_COMPRESSED_SRGB: dtype = 2; components = 3; break;
    case GL_COMPRESSED_SRGB_ALPHA: dtype = 2; components = 4; break;
    case GL_COMPRESSED_RED_RGTC1: dtype = 2; components = 1; break;
    case GL_COMPRESSED_SIGNED_RED_RGTC1: dtype = 2; components = 1; break;
    case GL_COMPRESSED_RG_RGTC2: dtype = 2; components = 2; break;
    case GL_COMPRESSED_SIGNED_RG_RGTC2: dtype = 2; components = 2; break;
    case GL_COMPRESSED_RGBA_BPTC_UNORM: dtype = 2; components = 4; break;
    case GL_COMPRESSED_SRGB_ALPHA_BPTC_UNORM: dtype = 2; components = 4; break;
    case GL_COMPRESSED_RGB_BPTC_SIGNED_FLOAT: dtype = 2; components = 3; break;
    case GL_COMPRESSED_RGB_BPTC_UNSIGNED_FLOAT: dtype = 2; components = 3; break;
    default: assert(false);
    }
    
    size_t data_size = width * height * depth * components * (dtype==3?8:4);
    trc_data_t* data = trc_create_data(ctx->trace, data_size, NULL, TRC_DATA_NO_ZERO);
    void* dest = trc_map_data(data, TRC_MAP_REPLACE);
    
    GLenum format;
    switch (ftype) {
    case 0: //normal
        format = (GLenum[]){GL_RED, GL_RG, GL_RGB, GL_RGBA}[components-1];
        break;
    case 1: //depth
        format = GL_DEPTH_COMPONENT;
        break;
    case 2: //stencil
        format = GL_STENCIL_INDEX;
        break;
    case 3: //depth stencil
        format = GL_DEPTH_STENCIL;
        break;
    }
    GLenum type;
    switch (dtype) {
    case 0: //uint32_t
        type = GL_UNSIGNED_INT;
        break;
    case 1: //int32_t
        type = GL_INT;
        break;
    case 2: //float
        type = GL_FLOAT;
        break;
    case 3: //float+unused24+uint8
        type = GL_FLOAT_32_UNSIGNED_INT_24_8_REV;
        break;
    }
    uint target = tex->type;
    if (target==GL_TEXTURE_CUBE_MAP) target = GL_TEXTURE_CUBE_MAP_POSITIVE_X + face;
    F(glGetTexImage)(target, level, format, type, dest);
    F(glBindTexture)(tex->type, prev);
    
    trc_unmap_freeze_data(ctx->trace, data);
    
    replay_set_texture_image(ctx->trace, fake, tex, level, face, internal_format, width, height, depth, data);
}

static const trc_gl_texture_rev_t* replay_get_bound_tex(trc_replay_context_t* ctx, uint target, uint* fake) {
    uint unit = trc_gl_state_get_active_texture_unit(ctx->trace);
    *fake = trc_gl_state_get_bound_textures(ctx->trace, target, unit);
    const trc_gl_texture_rev_t* rev = trc_get_gl_texture(ctx->trace, *fake);
    return rev;
}

static void replay_update_bound_tex_image(trc_replay_context_t* ctx, trace_command_t* cmd, uint target, uint level) {
    uint fake;
    const trc_gl_texture_rev_t* rev = replay_get_bound_tex(ctx, target, &fake);
    if (!rev) ERROR2(, "No texture bound to target");
    
    uint face = 0;
    if (target>=GL_TEXTURE_CUBE_MAP_POSITIVE_X && target<=GL_TEXTURE_CUBE_MAP_NEGATIVE_Z)
        face = target - GL_TEXTURE_CUBE_MAP_POSITIVE_X;
    replay_update_tex_image(ctx, rev, fake, level, face);
}

static bool tex_buffer(trc_replay_context_t* ctx, trace_command_t* cmd, GLuint tex_or_target, bool dsa,
                       GLenum internalformat, GLuint buffer, GLintptr offset, GLsizeiptr size) {
    const trc_gl_texture_rev_t* rev = NULL;
    if (!dsa)
        rev = replay_get_bound_tex(ctx, tex_or_target, &tex_or_target);
    else
        rev = trc_get_gl_texture(ctx->trace, tex_or_target);
    if (!rev) ERROR2(false, dsa?"Invalid texture name":"No texture bound to target");
    if (!rev->has_object) ERROR2(false, "Texture name has no object");
    
    const trc_gl_buffer_rev_t* buffer_rev = buffer ? trc_get_gl_buffer(ctx->trace, buffer) : NULL;
    if (!buffer_rev && buffer) ERROR2(false, "Invalid buffer name");
    if (buffer && !buffer_rev->has_object) ERROR2(false, "Buffer name has no object");
    if (offset<0 || size<=0 || offset+size>(buffer_rev->data?buffer_rev->data->size:0)) ERROR2(false, "Invalid range");
    //TODO: Check alignment
    
    trc_gl_texture_image_t img;
    memset(&img, 0, sizeof(img));
    img.internal_format = internalformat;
    img.buffer = buffer;
    img.buffer_start = offset;
    img.buffer_size = buffer ? (size<0?(buffer_rev->data?buffer_rev->data->size:0):size) : 0;
    
    trc_gl_texture_rev_t newrev = *rev;
    newrev.images = trc_create_data(ctx->trace, sizeof(img), &img, TRC_DATA_IMMUTABLE);
    trc_set_gl_texture(ctx->trace, tex_or_target, &newrev);
    
    return true;
}

static bool replay_append_fb_attachment(trc_replay_context_t* ctx, trace_command_t* cmd, bool dsa, uint fb, const trc_gl_framebuffer_attachment_t* attach) {
    const trc_gl_framebuffer_rev_t* rev = trc_get_gl_framebuffer(ctx->trace, fb);
    if (!rev) ERROR2(false, dsa?"Invalid framebuffer name":"No framebuffer bound to target");
    if (!rev->has_object) ERROR2(false, "Framebuffer name has no object");
    
    size_t attach_count = rev->attachments->size / sizeof(trc_gl_framebuffer_attachment_t);
    trc_gl_framebuffer_attachment_t* newattachs = malloc((attach_count+1)*sizeof(trc_gl_framebuffer_attachment_t));
    
    trc_gl_framebuffer_attachment_t* attachs = trc_map_data(rev->attachments, TRC_MAP_READ);
    bool replaced = false;
    for (size_t i = 0; i < attach_count; i++) {
        if (attachs[i].attachment == attach->attachment) {
            newattachs[i] = *attach;
            replaced = true;
        } else {
            newattachs[i] = attachs[i];
        }
    }
    trc_unmap_data(rev->attachments);
    
    trc_gl_framebuffer_rev_t newrev = *rev;
    if (!replaced) newattachs[attach_count++] = *attach;
    
    size_t size = attach_count * sizeof(trc_gl_framebuffer_attachment_t);
    newrev.attachments = trc_create_data_no_copy(ctx->trace, size, newattachs, TRC_DATA_IMMUTABLE);
    
    trc_set_gl_framebuffer(ctx->trace, fb, &newrev);
    
    return true;
}

static bool add_fb_attachment(trc_replay_context_t* ctx, trace_command_t* cmd, uint fb, uint attachment, bool dsa,
                              uint fake_tex, const trc_gl_texture_rev_t* tex, uint target, uint level, uint layer) {
    if (!tex && fake_tex) ERROR2(false, "Invalid texture name");
    if (fake_tex && !tex->has_object) ERROR2(false, "Texture name has no object");
    //TODO: Test if target is compatible with the texture
    //TODO: Test if level is a mipmap of the texture
    trc_gl_framebuffer_attachment_t attach;
    memset(&attach, 0, sizeof(attach));
    attach.has_renderbuffer = false;
    attach.attachment = attachment;
    attach.fake_texture = fake_tex;
    attach.level = level;
    attach.layer = layer;
    attach.face = 0;
    if (target>=GL_TEXTURE_CUBE_MAP_POSITIVE_X && target<=GL_TEXTURE_CUBE_MAP_NEGATIVE_Z) {
        attach.face = target - GL_TEXTURE_CUBE_MAP_POSITIVE_X;
    } else if (target==GL_TEXTURE_CUBE_MAP_ARRAY || target==GL_TEXTURE_CUBE_MAP) {
        attach.face = layer % 6;
        attach.layer /= 6;
    }
    return replay_append_fb_attachment(ctx, cmd, dsa, fb, &attach);
}

static bool add_fb_attachment_rb(trc_replay_context_t* ctx, trace_command_t* cmd, bool dsa, uint fb,
                                 uint attachment, uint fake_rb, const trc_gl_renderbuffer_rev_t* rb) {
    if (!rb && fake_rb) ERROR2(false, "Invalid renderbuffer name");
    if (fake_rb && !rb->has_object) ERROR2(false, "Renderbuffer name has no object");
    trc_gl_framebuffer_attachment_t attach;
    memset(&attach, 0, sizeof(attach));
    attach.has_renderbuffer = true;
    attach.attachment = attachment;
    attach.fake_renderbuffer = fake_rb;
    return replay_append_fb_attachment(ctx, cmd, dsa, fb, &attach);
}

static void replay_update_renderbuffer(trc_replay_context_t* ctx, const trc_gl_renderbuffer_rev_t* rev,
                                       uint fake, uint width, uint height, uint internal_format, uint samples) {
    GLint bits[6];
    GLint prev;
    F(glGetIntegerv)(GL_RENDERBUFFER_BINDING, &prev);
    F(glBindRenderbuffer)(GL_RENDERBUFFER, rev->real);
    F(glGetRenderbufferParameteriv)(GL_RENDERBUFFER, GL_RENDERBUFFER_RED_SIZE, &bits[0]);
    F(glGetRenderbufferParameteriv)(GL_RENDERBUFFER, GL_RENDERBUFFER_GREEN_SIZE, &bits[1]);
    F(glGetRenderbufferParameteriv)(GL_RENDERBUFFER, GL_RENDERBUFFER_BLUE_SIZE, &bits[2]);
    F(glGetRenderbufferParameteriv)(GL_RENDERBUFFER, GL_RENDERBUFFER_ALPHA_SIZE, &bits[3]);
    F(glGetRenderbufferParameteriv)(GL_RENDERBUFFER, GL_RENDERBUFFER_DEPTH_SIZE, &bits[4]);
    F(glGetRenderbufferParameteriv)(GL_RENDERBUFFER, GL_RENDERBUFFER_STENCIL_SIZE, &bits[5]);
    F(glBindRenderbuffer)(GL_RENDERBUFFER, prev);
    
    trc_gl_renderbuffer_rev_t newrev = *rev;
    newrev.width = width;
    newrev.height = height;
    newrev.internal_format = internal_format;
    newrev.sample_count = samples;
    for (size_t i = 0; i < 4; i++) newrev.rgba_bits[i] = bits[i];
    newrev.depth_bits = bits[4];
    newrev.stencil_bits = bits[5];
    newrev.has_storage = true;
    
    trc_set_gl_renderbuffer(ctx->trace, fake, &newrev);
}

//TODO or NOTE: Ensure that the border color is handled with integer glTexParameter(s)
//TODO: More validation
static bool texture_param_double(trc_replay_context_t* ctx, trace_command_t* cmd, bool dsa,
                                 GLuint tex_or_target, GLenum param, uint32_t count, const double* val) {
    const trc_gl_texture_rev_t* rev = NULL;
    if (dsa) {
        rev = trc_get_gl_texture(ctx->trace, tex_or_target);
    } else {
        rev = replay_get_bound_tex(ctx, tex_or_target, &tex_or_target);
    }
    if (!rev) ERROR2(true, dsa?"Invalid texture name":"No texture bound to target");
    if (!rev->has_object) ERROR2(true, "Texture name has no object");
    trc_gl_texture_rev_t newrev = *rev;
    
    switch (param) {
    case GL_TEXTURE_MIN_FILTER:
    case GL_TEXTURE_MAG_FILTER:
    case GL_TEXTURE_MIN_LOD:
    case GL_TEXTURE_MAX_LOD:
    case GL_TEXTURE_WRAP_S:
    case GL_TEXTURE_WRAP_T:
    case GL_TEXTURE_WRAP_R:
    case GL_TEXTURE_COMPARE_MODE:
    case GL_TEXTURE_COMPARE_FUNC:
    case GL_TEXTURE_BORDER_COLOR: {
        bool res = sample_param_double(cmd, &newrev.sample_params, param, count, val);
        trc_set_gl_texture(ctx->trace, tex_or_target, &newrev);
        return res;
    } case GL_DEPTH_STENCIL_TEXTURE_MODE:
    case GL_TEXTURE_BASE_LEVEL:
    case GL_TEXTURE_MAX_LEVEL:
    case GL_TEXTURE_LOD_BIAS:
    case GL_TEXTURE_SWIZZLE_R:
    case GL_TEXTURE_SWIZZLE_G:
    case GL_TEXTURE_SWIZZLE_B:
    case GL_TEXTURE_SWIZZLE_A: {
        if (count != 1) ERROR2(true, "Expected 1 value. Got %u.", count);
        break;
    } case GL_TEXTURE_SWIZZLE_RGBA: {
        if (count != 4) ERROR2(true, "Expected 4 values. Got %u.", count);
        break;
    }
    }
    
    switch (param) {
    case GL_DEPTH_STENCIL_TEXTURE_MODE:
        if (val[0]!=GL_DEPTH_COMPONENT && val[0]!=GL_STENCIL_INDEX)
            ERROR2(true, "Invalid depth stencil texture mode.");
        break;
    case GL_TEXTURE_SWIZZLE_R:
    case GL_TEXTURE_SWIZZLE_G:
    case GL_TEXTURE_SWIZZLE_B:
    case GL_TEXTURE_SWIZZLE_A:
        if (val[0]!=GL_RED && val[0]!=GL_GREEN && val[0]!=GL_BLUE && val[0]!=GL_ALPHA)
            ERROR2(true, "Invalid swizzle.");
        break;
    case GL_TEXTURE_SWIZZLE_RGBA:
        for (uint i = 0; i < 4; i++) {
            if (val[0]!=GL_RED && val[0]!=GL_GREEN && val[0]!=GL_BLUE && val[0]!=GL_ALPHA)
                ERROR2(true, "Invalid swizzle.");
        }
        break;
    }
    
    switch (param) {
    case GL_DEPTH_STENCIL_TEXTURE_MODE:
        newrev.depth_stencil_mode = val[0];
        break;
    case GL_TEXTURE_BASE_LEVEL: newrev.base_level = val[0]; break;
    case GL_TEXTURE_MAX_LEVEL: newrev.max_level = val[0]; break;
    case GL_TEXTURE_LOD_BIAS: newrev.lod_bias = val[0]; break;
    case GL_TEXTURE_SWIZZLE_R: newrev.swizzle[0] = val[0]; break;
    case GL_TEXTURE_SWIZZLE_G: newrev.swizzle[1] = val[0]; break;
    case GL_TEXTURE_SWIZZLE_B: newrev.swizzle[2] = val[0]; break;
    case GL_TEXTURE_SWIZZLE_A: newrev.swizzle[3] = val[0]; break;
    case GL_TEXTURE_SWIZZLE_RGBA:
        for (uint i = 0; i < 4; i++) newrev.swizzle[i] = val[i];
        break;
    }
    
    trc_set_gl_texture(ctx->trace, tex_or_target, &newrev);
    
    return false;
}

static GLuint get_bound_buffer(trc_replay_context_t* ctx, GLenum target) {
    return trc_gl_state_get_bound_buffer(ctx->trace, target);
}

static int uniform(trc_replay_context_t* ctx, trace_command_t* cmd, bool dsa,
                   bool array, uint dimx, uint dimy, GLenum type, void* data_,
                   uint* realprogram) {
    uint8_t* data = data_;
    
    uint arg_pos = 0;
    uint program;
    if (dsa) program = gl_param_GLuint(cmd, arg_pos++);
    else program = trc_gl_state_get_bound_program(ctx->trace);
    const trc_gl_program_rev_t* rev = trc_get_gl_program(ctx->trace, program);
    if (!rev) ERROR2(-1, dsa?"Invalid program":"No current program");
    if (realprogram) *realprogram = rev->real;
    
    int location = gl_param_GLint(cmd, arg_pos++);
    
    trc_gl_program_uniform_t uniform;
    
    size_t uniform_count = rev->uniforms->size /
                           sizeof(trc_gl_program_uniform_t);
    trc_gl_program_uniform_t* uniforms = trc_map_data(rev->uniforms, TRC_MAP_READ);
    uint uniform_index = 0;
    for (; uniform_index < uniform_count; uniform_index++) {
        if (uniforms[uniform_index].fake == location) {
            uniform = uniforms[uniform_index];
            goto success;
        }
    }
    trc_unmap_data(rev->uniforms);
    return -1;
    success: ;
    
    uint count = array ? gl_param_GLsizei(cmd, arg_pos++) : 1;
    bool transpose = dimy==1 ? false : gl_param_GLboolean(cmd, arg_pos++);
    
    uniform.value = trc_create_data(ctx->trace, count*dimx*dimy*sizeof(double), NULL, TRC_DATA_NO_ZERO);
    double* res = trc_map_data(uniform.value, TRC_MAP_REPLACE);
    for (uint i = 0; i < count; i++) {
        for (uint x = 0; x < dimx; x++) {
            for (uint y = 0; y < dimy; y++) {
                double val = 0;
                if (array) {
                    uint si = transpose ? y*dimx+x : x*dimy+y;
                    si += dimx * dimy * i;
                    switch (type) {
                    case GL_FLOAT:
                        val = trc_get_double(trc_get_arg(cmd, arg_pos))[si]; break;
                    case GL_DOUBLE:
                        val = trc_get_double(trc_get_arg(cmd, arg_pos))[si]; break;
                    case GL_INT:
                        val = trc_get_int(trc_get_arg(cmd, arg_pos))[si]; break;
                    case GL_UNSIGNED_INT:
                        val = trc_get_uint(trc_get_arg(cmd, arg_pos))[si]; break;
                    }
                } else {
                    switch (type) {
                    case GL_FLOAT:
                        val = gl_param_GLfloat(cmd, arg_pos++); break;
                    case GL_DOUBLE:
                        val = gl_param_GLdouble(cmd, arg_pos++); break;
                    case GL_INT:
                        val = gl_param_GLint(cmd, arg_pos++); break;
                    case GL_UNSIGNED_INT:
                        val = gl_param_GLuint(cmd, arg_pos++); break;
                    }
                }
                *res++ = val;
                if (data) {
                    switch (type) {
                    case GL_FLOAT: *(float*)data = val; data += sizeof(float); break;
                    case GL_DOUBLE: *(double*)data = val; data += sizeof(double); break;
                    case GL_INT: *(int*)data = val; data += sizeof(int); break;
                    case GL_UNSIGNED_INT: *(uint*)data = val; data += sizeof(uint); break;
                    }
                }
            }
        }
    }
    trc_unmap_freeze_data(ctx->trace, uniform.value);
    uniform.dim[0] = dimx;
    uniform.dim[1] = dimy;
    uniform.count = count;
    
    trc_data_t* new_uniforms = trc_create_data(ctx->trace, uniform_count*sizeof(trc_gl_program_uniform_t), uniforms, 0);
    trc_unmap_data(rev->uniforms);
    
    trc_gl_program_rev_t newrev = *rev;
    newrev.uniforms = new_uniforms;
    
    trc_gl_program_uniform_t* dest = trc_map_data(new_uniforms, TRC_MAP_MODIFY);
    dest[uniform_index] = uniform;
    trc_unmap_freeze_data(ctx->trace, new_uniforms);
    
    trc_set_gl_program(ctx->trace, trc_get_gl_context(ctx->trace, 0)->bound_program, &newrev);
    
    return uniform.real;
}

static void validate_get_uniform(trc_replay_context_t* ctx, trace_command_t* cmd) {
    //TODO: Don't use glGetProgramiv to get the link status
    GLuint fake = gl_param_GLuint(cmd, 0);
    GLuint real_program = trc_get_real_gl_program(ctx->trace, fake);
    if (!real_program) ERROR2(, "No such program.");
    GLint status;
    F(glGetProgramiv)(real_program, GL_LINK_STATUS, &status);
    if (!status) ERROR2(, "Program not successfully linked.");
}

//type in [GL_FLOAT, GL_DOUBLE, GL_UNSIGNED_BYTE, GL_BYTE, GL_SHORT, GL_UNSIGNED_SHORT, GL_UNSIGNED_INT, GL_INT]
//internal in [GL_FLOAT, GL_DOUBLE, GL_UNSIGNED_INT, GL_INT]
static void vertex_attrib(trc_replay_context_t* ctx, trace_command_t* cmd, uint comp,
                          GLenum type, bool array, bool normalized, GLenum internal) {
    uint index = gl_param_GLuint(cmd, 0);
    if (index==0 || index>=trc_gl_state_get_state_int(ctx->trace, GL_MAX_VERTEX_ATTRIBS, 0))
        ERROR2(, "Invalid vertex attribute index");
    index--;
    uint i = 0;
    for (; i < comp; i++) {
        double val = 0;
        if (array) {
            switch (type) {
            case GL_UNSIGNED_BYTE:
            case GL_UNSIGNED_SHORT:
            case GL_UNSIGNED_INT: val = trc_get_int(trc_get_arg(cmd, 1))[i]; break;
            case GL_BYTE:
            case GL_SHORT:
            case GL_INT: val = trc_get_uint(trc_get_arg(cmd, 1))[i]; break;
            case GL_FLOAT:
            case GL_DOUBLE: val = trc_get_double(trc_get_arg(cmd, 1))[i]; break;
            }
        } else {
            switch (type) {
            case GL_UNSIGNED_BYTE:
            case GL_UNSIGNED_SHORT:
            case GL_UNSIGNED_INT: val = trc_get_int(trc_get_arg(cmd, i+1))[0]; break;
            case GL_BYTE:
            case GL_SHORT:
            case GL_INT: val = trc_get_uint(trc_get_arg(cmd, i+1))[0]; break;
            case GL_FLOAT:
            case GL_DOUBLE: val = trc_get_double(trc_get_arg(cmd, i+1))[0]; break;
            }
        }
        if (internal==GL_FLOAT) val = (float)val;
        if (normalized) {
            switch (type) {
            case GL_UNSIGNED_BYTE: val /= UINT8_MAX; break;
            case GL_UNSIGNED_SHORT: val /= UINT16_MAX; break;
            case GL_UNSIGNED_INT: val /= UINT32_MAX; break;
            case GL_BYTE: val = val<0 ? val/-(double)INT8_MIN : val/INT8_MAX; break;
            case GL_SHORT: val = val<0 ? val/-(double)INT16_MIN : val/INT16_MAX; break;
            case GL_INT: val = val<0 ? val/-(double)INT32_MIN : val/INT32_MAX; break;
            }
        }
        trc_gl_state_set_state_double(ctx->trace, GL_CURRENT_VERTEX_ATTRIB, index*4+i, 0);
    }
    for (; i < 3; i++)
        trc_gl_state_set_state_double(ctx->trace, GL_CURRENT_VERTEX_ATTRIB, index*4+i, 0);
    for (; i < 4; i++)
        trc_gl_state_set_state_double(ctx->trace, GL_CURRENT_VERTEX_ATTRIB, index*4+i, 1);
    
    double vals[4];
    for (i = 0; i < 4; i++)
        vals[i] = trc_gl_state_get_state_double(ctx->trace, GL_CURRENT_VERTEX_ATTRIB, index*4+i);
    
    switch (internal) {
    case GL_FLOAT: F(glVertexAttrib4dv(index+1, vals)); break;
    case GL_DOUBLE: F(glVertexAttribL4dv(index+1, vals)); break;
    case GL_UNSIGNED_INT: F(glVertexAttribI4ui(index, vals[0], vals[1], vals[2], vals[3])); break;
    case GL_INT: F(glVertexAttribI4i(index, vals[0], vals[1], vals[2], vals[3])); break;
    }
}

static GLint get_bound_framebuffer(trc_replay_context_t* ctx, GLenum target) {
    const trc_gl_context_rev_t* state = trc_get_gl_context(ctx->trace, 0);
    switch (target) {
    case GL_DRAW_FRAMEBUFFER: 
        return state->draw_framebuffer;
    case GL_READ_FRAMEBUFFER: 
        return state->read_framebuffer;
    case GL_FRAMEBUFFER: 
        return state->draw_framebuffer;
    }
    return 0;
}

static void update_query(trc_replay_context_t* ctx, trace_command_t* cmd, GLenum target, GLuint fake_id, GLuint id) {
    if (!id) return;
    
    GLuint64 res = 0;
    if (target!=GL_TIME_ELAPSED && target!=GL_TIMESTAMP) { //TODO: Why is this branch here?
        F(glFinish)();
        while (!res) F(glGetQueryObjectui64v)(id, GL_QUERY_RESULT_AVAILABLE, &res);
        F(glGetQueryObjectui64v)(id, GL_QUERY_RESULT, &res);
    }
    
    trc_gl_query_rev_t query = *trc_get_gl_query(ctx->trace, fake_id);
    query.result = res;
    trc_set_gl_query(ctx->trace, fake_id, &query);
}

static void begin_get_fb0_data(trc_replay_context_t* ctx, GLint prev[11]) {
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

static void end_get_fb0_data(trc_replay_context_t* ctx, const GLint prev[11]) {
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

static trc_data_t* replay_get_fb0_buffer(trc_replay_context_t* ctx, trc_gl_context_rev_t* state,
                                         GLenum buffer, GLenum format, GLenum type) {
    F(glReadBuffer)(buffer);
    
    size_t data_size = state->drawable_width * state->drawable_height * 4;
    trc_data_t* data = trc_create_data(ctx->trace, data_size, NULL, TRC_DATA_NO_ZERO);
    void* dest = trc_map_data(data, TRC_MAP_REPLACE);
    F(glReadPixels)(0, 0, state->drawable_width, state->drawable_height, format, type, dest);
    trc_unmap_freeze_data(ctx->trace, data);
    
    return data;
}

static void store_and_bind_fb(trc_replay_context_t* ctx, GLint* prev, GLuint fb) {
    F(glGetIntegerv)(GL_DRAW_FRAMEBUFFER_BINDING, prev);
    F(glBindFramebuffer)(GL_DRAW_FRAMEBUFFER, fb);
}

static void replay_update_fb0_buffers(trc_replay_context_t* ctx, bool backcolor, bool frontcolor,
                                      bool depth, bool stencil) {
    F(glFinish)();
    
    GLint prevfb;
    store_and_bind_fb(ctx, &prevfb, 0);
    GLint depth_size, stencil_size;
    F(glGetFramebufferAttachmentParameteriv)(GL_DRAW_FRAMEBUFFER, GL_DEPTH,
                                             GL_FRAMEBUFFER_ATTACHMENT_DEPTH_SIZE, &depth_size);
    F(glGetFramebufferAttachmentParameteriv)(GL_DRAW_FRAMEBUFFER, GL_STENCIL,
                                             GL_FRAMEBUFFER_ATTACHMENT_STENCIL_SIZE, &stencil_size);
    if (depth_size == 0) depth = false;
    if (stencil_size == 0) stencil = false;
    F(glBindFramebuffer)(GL_DRAW_FRAMEBUFFER, prevfb);
    
    GLint prev[11];
    begin_get_fb0_data(ctx, prev);
    trc_gl_context_rev_t state = *trc_get_gl_context(ctx->trace, 0);
    if (backcolor) {
        state.back_color_buffer = replay_get_fb0_buffer(ctx, &state, GL_BACK, GL_RGBA, GL_UNSIGNED_BYTE);
    }
    if (frontcolor) {
        state.front_color_buffer = replay_get_fb0_buffer(ctx, &state, GL_FRONT, GL_RGBA, GL_UNSIGNED_BYTE);
    }
    if (depth) {
        state.back_depth_buffer = replay_get_fb0_buffer(ctx, &state, GL_BACK, GL_DEPTH_COMPONENT, GL_UNSIGNED_INT);
    }
    if (stencil) {
        state.back_stencil_buffer = replay_get_fb0_buffer(ctx, &state, GL_BACK, GL_STENCIL_INDEX, GL_UNSIGNED_INT);
    }
    trc_set_gl_context(ctx->trace, 0, &state);
    end_get_fb0_data(ctx, prev);
}

static bool begin_draw(trc_replay_context_t* ctx, trace_command_t* cmd) {
    const trc_gl_context_rev_t* state = trc_get_gl_context(ctx->trace, 0);
    const trc_gl_vao_rev_t* vao = trc_get_gl_vao(ctx->trace, state->bound_vao);
    if (vao == NULL) ERROR2(false, "No VAO bound");
    const trc_gl_program_rev_t* program = trc_get_gl_program(ctx->trace, state->bound_program);
    if (program == NULL) ERROR2(false, "No program bound");
    
    GLint last_buf;
    F(glGetIntegerv)(GL_ARRAY_BUFFER_BINDING, &last_buf);
    
    size_t prog_vertex_attrib_count = program->vertex_attribs->size / (sizeof(uint)*2);
    uint* prog_vertex_attribs = trc_map_data(program->vertex_attribs, TRC_MAP_READ);
    trc_gl_vao_attrib_t* vao_attribs = vao ? trc_map_data(vao->attribs, TRC_MAP_READ) : NULL;
    for (size_t i = 0; i < (vao?vao->attribs->size/sizeof(trc_gl_vao_attrib_t):0); i++) {
        GLint real_loc = -1;
        for (size_t j = 0; j < prog_vertex_attrib_count; j++) {
            if (prog_vertex_attribs[j*2+1] == i) {
                real_loc = prog_vertex_attribs[j*2];
                break;
            }
        }
        if (real_loc < 0) continue;
        
        trc_gl_vao_attrib_t* a = &vao_attribs[i];
        if (!a->enabled) {
            F(glDisableVertexAttribArray)(real_loc);
            continue;
        }
        F(glEnableVertexAttribArray)(real_loc);
        
        F(glBindBuffer)(GL_ARRAY_BUFFER, a->buffer);
        if (a->integer)
            F(glVertexAttribIPointer)(real_loc, a->size, a->type, a->stride, (const void*)(uintptr_t)a->offset);
        else
            F(glVertexAttribPointer)(real_loc, a->size, a->type, a->normalized, a->stride, (const void*)(uintptr_t)a->offset);
        
        if (trc_gl_state_get_ver(ctx->trace) > 330)
            F(glVertexAttribDivisor)(real_loc, a->divisor);
    }
    trc_unmap_data(vao->attribs);
    trc_unmap_data(program->vertex_attribs);
    
    F(glBindBuffer)(GL_ARRAY_BUFFER, last_buf);
    
    return true;
}

static bool not_one_of(int val, ...) {
    va_list list;
    va_start(list, val);
    while (true) {
        int v = va_arg(list, int);
        if (v == -1) break;
        if (v == val) return false;
    }
    va_end(list);
    return true;
}

#define PARTIAL_VALIDATE_CLEAR_BUFFER do {\
int max_draw_buffers = trc_gl_state_get_state_int(ctx->trace, GL_MAX_DRAW_BUFFERS, 0);\
if (p_drawbuffer<0 || (p_drawbuffer==0&&p_buffer!=GL_COLOR) || p_drawbuffer>=max_draw_buffers)\
    ERROR("Invalid buffer");\
} while (0)

static void update_drawbuffer(trc_replay_context_t* ctx, GLenum buffer, GLuint drawbuffer) {
    const trc_gl_context_rev_t* state = trc_get_gl_context(ctx->trace, 0);
    if (state->draw_framebuffer == 0) {
        switch (buffer) {
        case GL_COLOR: replay_update_fb0_buffers(ctx, true, false, false, false); break;
        case GL_DEPTH: replay_update_fb0_buffers(ctx, false, false, true, false); break;
        case GL_STENCIL: replay_update_fb0_buffers(ctx, false, false, false, true); break;
        }
    } else {
        const trc_gl_framebuffer_rev_t* rev = trc_get_gl_framebuffer(ctx->trace, state->draw_framebuffer);
        uint attachment;
        if (drawbuffer>=rev->draw_buffers->size/sizeof(GLenum)) {
            attachment = GL_NONE;
        } else {
            attachment = ((GLenum*)trc_map_data(rev->draw_buffers, TRC_MAP_READ))[drawbuffer];
            trc_unmap_data(rev->draw_buffers);
        }
        
        size_t attach_count = rev->attachments->size / sizeof(trc_gl_framebuffer_attachment_t);
        const trc_gl_framebuffer_attachment_t* attachs = trc_map_data(rev->attachments, TRC_MAP_READ);
        for (size_t i = 0; i < attach_count; i++) {
            const trc_gl_framebuffer_attachment_t* attach = &attachs[i];
            if (attach->attachment != attachment) continue;
            if (attach->has_renderbuffer) continue;
            const trc_gl_texture_rev_t* tex = trc_get_gl_texture(ctx->trace, attach->fake_texture);
            assert(tex);
            replay_update_tex_image(ctx, tex, attach->fake_texture, attach->level, attach->face);
        }
        trc_unmap_data(rev->attachments);
    }
}

static void update_buffers(trc_replay_context_t* ctx, GLuint fb, GLbitfield mask) {
    if (fb == 0) {
        bool color = mask & GL_COLOR_BUFFER_BIT;
        bool depth = mask & GL_DEPTH_BUFFER_BIT;
        bool stencil = mask & GL_STENCIL_BUFFER_BIT;
        replay_update_fb0_buffers(ctx, color, false, depth, stencil);
    } else {
        const trc_gl_framebuffer_rev_t* rev = trc_get_gl_framebuffer(ctx->trace, fb);
        
        size_t max_updates = rev->draw_buffers->size/sizeof(GLenum) + 3;
        GLenum* updates = replay_alloc(max_updates*sizeof(GLenum));
        size_t update_count = 0;
        
        if (mask & GL_COLOR_BUFFER_BIT) {
            const GLenum* draw_buffers = trc_map_data(rev->draw_buffers, TRC_MAP_READ);
            for (size_t i = 0; i < rev->draw_buffers->size/sizeof(GLenum); i++)
                updates[update_count++] = draw_buffers[i];
            trc_unmap_data(rev->draw_buffers);
        }
        if (mask & GL_DEPTH_BUFFER_BIT)
            updates[update_count++] = GL_DEPTH_ATTACHMENT;
        if (mask & GL_STENCIL_BUFFER_BIT)
            updates[update_count++] = GL_DEPTH_ATTACHMENT;
        if ((mask&GL_DEPTH_BUFFER_BIT) && (mask&GL_STENCIL_BUFFER_BIT))
            updates[update_count++] = GL_DEPTH_STENCIL_ATTACHMENT;
        
        size_t attach_count = rev->attachments->size / sizeof(trc_gl_framebuffer_attachment_t);
        const trc_gl_framebuffer_attachment_t* attachs = trc_map_data(rev->attachments, TRC_MAP_READ);
        for (size_t i = 0; i < update_count; i++) {
            for (size_t j = 0; j < attach_count; j++) {
                const trc_gl_framebuffer_attachment_t* attach = &attachs[j];
                if (attach->attachment == updates[i]) continue;
                if (attach->has_renderbuffer) continue;
                const trc_gl_texture_rev_t* tex = trc_get_gl_texture(ctx->trace, attach->fake_texture);
                replay_update_tex_image(ctx, tex, attach->fake_texture, attach->level, attach->face);
            }
        }
        trc_unmap_data(rev->attachments);
    }
}

static void end_draw(trc_replay_context_t* ctx, trace_command_t* cmd) {
    bool depth = trc_gl_state_get_state_bool(ctx->trace, GL_DEPTH_WRITEMASK, 0);
    bool stencil = trc_gl_state_get_state_int(ctx->trace, GL_STENCIL_WRITEMASK, 0) != 0;
    stencil = stencil || trc_gl_state_get_state_int(ctx->trace, GL_STENCIL_BACK_WRITEMASK, 0)!=0;
    
    GLbitfield mask = GL_COLOR_BUFFER_BIT;
    if (depth) mask |= GL_DEPTH_BUFFER_BIT;
    if (stencil) mask |= GL_STENCIL_BUFFER_BIT;
    //TODO: Only update color buffers that could have been written to using GL_COLOR_WRITEMASK
    update_buffers(ctx, trc_gl_state_get_draw_framebuffer(ctx->trace), mask);
}

static void gen_textures(trc_replay_context_t* ctx, size_t count, const GLuint* real, const GLuint* fake, bool create, GLenum target) {
    trc_gl_texture_rev_t rev;
    rev.fake_context = trc_get_current_fake_gl_context(ctx->trace);
    rev.ref_count = 1;
    rev.has_object = create;
    rev.type = create ? target : 0;
    rev.depth_stencil_mode = GL_DEPTH_COMPONENT;
    rev.base_level = 0;
    rev.sample_params.border_color[0] = 0;
    rev.sample_params.border_color[1] = 0;
    rev.sample_params.border_color[2] = 0;
    rev.sample_params.border_color[3] = 0;
    rev.sample_params.compare_func = GL_LEQUAL;
    rev.sample_params.compare_mode = GL_NONE;
    rev.lod_bias = 0;
    rev.sample_params.min_filter = GL_NEAREST_MIPMAP_LINEAR;
    rev.sample_params.mag_filter = GL_LINEAR;
    rev.sample_params.min_lod = -1000;
    rev.sample_params.max_lod = 1000;
    rev.max_level = 1000;
    rev.swizzle[0] = GL_RED;
    rev.swizzle[1] = GL_GREEN;
    rev.swizzle[2] = GL_BLUE;
    rev.swizzle[3] = GL_ALPHA;
    rev.sample_params.wrap_s = GL_REPEAT;
    rev.sample_params.wrap_t = GL_REPEAT;
    rev.sample_params.wrap_r = GL_REPEAT;
    rev.images = NULL;
    for (size_t i = 0; i < count; ++i) {
        rev.real = real[i];
        trc_set_gl_texture(ctx->trace, fake[i], &rev);
    }
}

static void gen_buffers(trc_replay_context_t* ctx, size_t count, const GLuint* real, const GLuint* fake, bool create) {
    trc_gl_buffer_rev_t rev;
    rev.fake_context = trc_get_current_fake_gl_context(ctx->trace);
    rev.ref_count = 1;
    rev.has_object = create;
    rev.has_data = false;
    rev.data_usage = 0;
    rev.data = NULL;
    rev.mapped = false;
    rev.map_offset = 0;
    rev.map_length = 0;
    rev.map_access = 0;
    for (size_t i = 0; i < count; ++i) {
        rev.real = real[i];
        trc_set_gl_buffer(ctx->trace, fake[i], &rev);
    }
}

static void gen_framebuffers(trc_replay_context_t* ctx, size_t count, const GLuint* real, const GLuint* fake, bool create) {
    trc_gl_framebuffer_rev_t rev;
    rev.fake_context = trc_get_current_fake_gl_context(ctx->trace);
    rev.ref_count = 1;
    rev.has_object = false;
    trc_data_t* empty_data = trc_create_data(ctx->trace, 0, NULL, TRC_DATA_IMMUTABLE);
    rev.attachments = empty_data;
    rev.draw_buffers = empty_data;
    for (size_t i = 0; i < count; ++i) {
        rev.real = real[i];
        trc_set_gl_framebuffer(ctx->trace, fake[i], &rev);
    }
}

static void gen_queries(trc_replay_context_t* ctx, size_t count, const GLuint* real, const GLuint* fake, bool create, GLenum target) {
    trc_gl_query_rev_t rev;
    rev.fake_context = trc_get_current_fake_gl_context(ctx->trace);
    rev.ref_count = 1;
    rev.has_object = create;
    rev.type = create ? target : 0;
    rev.result = 0;
    rev.active_index = -1;
    for (size_t i = 0; i < count; ++i) {
        rev.real = real[i];
        trc_set_gl_query(ctx->trace, fake[i], &rev);
    }
}

static void gen_renderbuffers(trc_replay_context_t* ctx, size_t count, const GLuint* real, const GLuint* fake, bool create) {
    trc_gl_renderbuffer_rev_t rev;
    rev.fake_context = trc_get_current_fake_gl_context(ctx->trace);
    rev.ref_count = 1;
    rev.has_object = create;
    rev.has_storage = false;
    for (size_t i = 0; i < count; ++i) {
        rev.real = real[i];
        trc_set_gl_renderbuffer(ctx->trace, fake[i], &rev);
    }
}

static void gen_vertex_arrays(trc_replay_context_t* ctx, size_t count, const GLuint* real, const GLuint* fake, bool create) {
    trc_gl_vao_rev_t rev;
    rev.fake_context = trc_get_current_fake_gl_context(ctx->trace);
    rev.ref_count = 1;
    rev.has_object = create;
    int attrib_count = trc_gl_state_get_state_int(ctx->trace, GL_MAX_VERTEX_ATTRIBS, 0);
    rev.attribs = trc_create_data(ctx->trace, attrib_count*sizeof(trc_gl_vao_attrib_t), NULL, TRC_DATA_NO_ZERO);
    trc_gl_vao_attrib_t* attribs = trc_map_data(rev.attribs, TRC_MAP_REPLACE);
    memset(attribs, 0, attrib_count*sizeof(trc_gl_vao_attrib_t)); //fill in padding to fix uninitialized memory errors during compession
    for (size_t j = 0; j < attrib_count; j++) {
        attribs[j].enabled = false;
        attribs[j].normalized = false;
        attribs[j].integer = false;
        attribs[j].size = 4;
        attribs[j].stride = 0;
        attribs[j].offset = 0;
        attribs[j].type = GL_FLOAT;
        attribs[j].divisor = 0;
        attribs[j].buffer = 0;
    }
    trc_unmap_data(rev.attribs);
    for (size_t i = 0; i < count; ++i) {
        rev.real = real[i];
        trc_set_gl_vao(ctx->trace, fake[i], &rev);
    }
}

static bool buffer_data(trc_replay_context_t* ctx, trace_command_t* cmd, bool dsa, GLuint fake, GLsizeiptr size, const void* data, GLenum usage) {
    if (size < 0) ERROR2(false, "Invalid size");
    const trc_gl_buffer_rev_t* rev = trc_get_gl_buffer(ctx->trace, fake);
    if (!rev) ERROR2(false, dsa?"Invalid buffer name":"No buffer bound to target");
    if (!rev->has_object) ERROR2(false, "Buffer name has no object");
    trc_gl_buffer_rev_t newrev = *rev;
    newrev.data = trc_create_data(ctx->trace, size, data, TRC_DATA_IMMUTABLE);
    trc_set_gl_buffer(ctx->trace, fake, &newrev);
    return true;
}

static bool buffer_sub_data(trc_replay_context_t* ctx, trace_command_t* cmd, bool dsa, GLuint fake, GLintptr offset, GLsizeiptr size, const void* data) {
    if (offset<0) ERROR2(false, "Invalid offset");
    if (size<0) ERROR2(false, "Invalid size");
    const trc_gl_buffer_rev_t* rev = trc_get_gl_buffer(ctx->trace, fake);
    if (!rev) ERROR2(false, dsa?"Invalid buffer name":"No buffer bound to target");
    if (!rev->data) ERROR2(false, "Buffer has no data");
    if (!rev->has_object) ERROR2(false, "Buffer name does not have an object");
    if (offset+size > rev->data->size) ERROR2(false, "Invalid range");
    
    trc_gl_buffer_rev_t newrev = *rev;
    
    newrev.data = trc_create_data(ctx->trace, rev->data->size, trc_map_data(rev->data, TRC_MAP_READ), 0);
    trc_unmap_data(rev->data);
    
    void* newdata = trc_map_data(newrev.data, TRC_MAP_REPLACE);
    memcpy((uint8_t*)newdata+offset, data, size);
    trc_unmap_freeze_data(ctx->trace, newrev.data);
    
    trc_set_gl_buffer(ctx->trace, fake, &newrev);
    
    return true;
}

static bool copy_buffer_data(trc_replay_context_t* ctx, trace_command_t* cmd, bool dsa, GLuint read,
                             GLuint write, GLintptr read_off, GLintptr write_off, GLsizeiptr size) {
    if (read_off<0 || write_off<0 || size<0) ERROR2(false, "The read offset, write offset or size is negative");
    
    const trc_gl_buffer_rev_t* read_rev = trc_get_gl_buffer(ctx->trace, read);
    if (!read_rev) ERROR2(false, dsa?"Invalid read buffer name":"No buffer bound to read target");
    if (!read_rev->has_object) ERROR2(false, "Read buffer name has no object");
    if (!read_rev->data) ERROR2(false, "Read buffer has no data");
    if (read_off+size > read_rev->data->size) ERROR2(false, "Invalid size and read offset");
    
    const trc_gl_buffer_rev_t* write_rev = trc_get_gl_buffer(ctx->trace, write);
    if (!write_rev) ERROR2(false, dsa?"Invalid write buffer name":"No buffer bound to write target");
    if (!write_rev->has_object) ERROR2(false, "Write buffer name has no object");
    if (!write_rev->data) ERROR2(false, "Write buffer has no data");
    if (write_off+size > write_rev->data->size) ERROR2(false, "Invalid size and write offset");
    
    trc_gl_buffer_rev_t res;
    
    res.data = trc_create_data(ctx->trace, write_rev->data->size, trc_map_data(write_rev->data, TRC_MAP_READ), 0);
    trc_unmap_data(write_rev->data);
    
    void* newdata = trc_map_data(res.data, TRC_MAP_REPLACE);
    uint8_t* readdata = trc_map_data(read_rev->data, TRC_MAP_READ);
    memcpy(newdata+write_off, readdata+read_off, size);
    trc_unmap_data(read_rev->data);
    trc_unmap_freeze_data(ctx->trace, res.data);
    
    trc_set_gl_buffer(ctx->trace, write, &res);
    
    return true;
}

static void map_buffer(trc_replay_context_t* ctx, trace_command_t* cmd, bool dsa, GLuint fake, GLenum access) {
    const trc_gl_buffer_rev_t* rev = trc_get_gl_buffer(ctx->trace, fake);
    if (!rev) ERROR2(, dsa?"Invalid buffer name":"No buffer bound to target");
    if (!rev->has_object) ERROR2(, "Buffer name has no object");
    if (!rev->data) ERROR2(, "Buffer has no data");
    if (rev->mapped) ERROR2(, "Buffer is already mapped");
    
    trc_gl_buffer_rev_t newrev = *rev;
    newrev.mapped = true;
    newrev.map_offset = 0;
    newrev.map_length = rev->data->size;
    switch (access) {
    case GL_READ_ONLY: newrev.map_access = GL_MAP_READ_BIT; break;
    case GL_WRITE_ONLY: newrev.map_access = GL_MAP_WRITE_BIT; break;
    case GL_READ_WRITE: newrev.map_access = GL_MAP_READ_BIT | GL_MAP_WRITE_BIT; break;
    }
    
    trc_set_gl_buffer(ctx->trace, fake, &newrev);
}

static void map_buffer_range(trc_replay_context_t* ctx, trace_command_t* cmd, bool dsa, GLuint fake,
                             GLintptr offset, GLsizeiptr length, GLbitfield access) {
    if (offset<0 || length<=0) ERROR2(, "Invalid length or offset");
    
    if (!(access&GL_MAP_READ_BIT) && !(access&GL_MAP_WRITE_BIT))
        ERROR2(, "Neither GL_MAP_READ_BIT or GL_MAP_WRITE_BIT is set");
    
    if (access&GL_MAP_READ_BIT && (access&GL_MAP_INVALIDATE_RANGE_BIT ||
                                   access&GL_MAP_INVALIDATE_BUFFER_BIT ||
                                   access&GL_MAP_UNSYNCHRONIZED_BIT))
        ERROR2(, "GL_MAP_READ_BIT is set and GL_MAP_INVALIDATE_RANGE_BIT, GL_MAP_INVALIDATE_BUFFER_BIT or GL_MAP_UNSYNCHRONIZED_BIT is set");
    
    if (access&GL_MAP_FLUSH_EXPLICIT_BIT && !(access&GL_MAP_WRITE_BIT))
        ERROR2(, "GL_MAP_FLUSH_EXPLICIT_BIT is set but GL_MAP_WRITE_BIT is not");
    
    if (access&!(GLbitfield)0xff) ERROR2(, "Invalid access flags");
    
    //TODO:
    //Make sure the access is valid with the buffer's storage flags
    
    const trc_gl_buffer_rev_t* rev = trc_get_gl_buffer(ctx->trace, fake);
    if (!rev) ERROR2(, dsa?"Invalid buffer name":"No buffer bound to target");
    trc_gl_buffer_rev_t newrev = *rev;
    if (!newrev.data) ERROR2(, "Buffer has no data");
    
    if (offset+length > newrev.data->size) ERROR2(, "offset+length is greater than the buffer's size");
    if (newrev.mapped) ERROR2(, "Buffer is already mapped");
    
    newrev.mapped = true;
    newrev.map_offset = offset;
    newrev.map_length = length;
    newrev.map_access = access;
    
    trc_set_gl_buffer(ctx->trace, fake, &newrev);
}

static void get_buffer_sub_data(trc_replay_context_t* ctx, trace_command_t* cmd, bool dsa, GLuint fake, GLintptr offset, GLsizeiptr size) {
    const trc_gl_buffer_rev_t* rev = trc_get_gl_buffer(ctx->trace, fake);
    if (!rev) ERROR2(, dsa?"Invalid buffer name":"No buffer bound to target");
    if (!rev->has_object) ERROR2(, "Buffer name has no object");
    if (rev->mapped && !(rev->map_access&GL_MAP_PERSISTENT_BIT)) ERROR2(, "Buffer is mapped without GL_MAP_PERSISTENT_BIT");
    if (offset<0 || size<0 || offset+size>rev->data->size) ERROR2(, "Invalid offset and/or size");
}

static bool renderbuffer_storage(trc_replay_context_t* ctx, trace_command_t* cmd, const trc_gl_renderbuffer_rev_t* rb,
                                 bool dsa, GLenum internal_format, GLsizei width, GLsizei height, GLsizei samples) {
    if (!rb) ERROR2(false, dsa?"Invalid renderbuffer name":"No renderbuffer bound");
    if (!rb->has_object) ERROR2(false, "Renderbuffer name has no obejct");
    int maxsize = trc_gl_state_get_state_int(ctx->trace, GL_MAX_RENDERBUFFER_SIZE, 0);
    if (width<0 || height<0 || width>maxsize || height>maxsize)
        ERROR2(false, "Invalid dimensions");
    //TODO: test if samples if valid
    //TODO: handle when internal_format is not renderable
    return true;
}

static bool begin_query(trc_replay_context_t* ctx, trace_command_t* cmd, GLenum target, GLuint index, GLuint id) {
    if (index >= trc_gl_state_get_bound_queries_size(ctx->trace, target))
        ERROR2(false, "Index is greater than maximum");
    if (trc_gl_state_get_bound_queries(ctx->trace, target, index) != 0)
        ERROR2(false, "Query is already active at target");
    
    const trc_gl_query_rev_t* rev = trc_get_gl_query(ctx->trace, id);
    if (!rev) ERROR2(false, "Invalid query name");
    if (rev->active_index != -1) ERROR2(false, "Query is already active");
    if (rev->has_object && rev->type!=target)
        ERROR2(false, "Query object's type does not match target");
    
    trc_gl_query_rev_t newrev = *rev;
    newrev.type = target;
    newrev.has_object = true;
    newrev.active_index = index;
    trc_set_gl_query(ctx->trace, id, &newrev);
    
    trc_gl_state_set_bound_queries(ctx->trace, target, index, id);
    
    return true;
}

static bool end_query(trc_replay_context_t* ctx, trace_command_t* cmd, GLenum target, GLuint index) {
    if (index >= trc_gl_state_get_bound_queries_size(ctx->trace, target))
        ERROR2(false, "Index is greater than maximum");
    int id = trc_gl_state_get_bound_queries(ctx->trace, target, index);
    if (id == 0) ERROR2(false, "No query active at target");
    
    trc_gl_query_rev_t newrev = *trc_get_gl_query(ctx->trace, id);
    newrev.active_index = -1;
    trc_set_gl_query(ctx->trace, id, &newrev);
    
    trc_gl_state_set_bound_queries(ctx->trace, target, index, 0);
    
    return true;
}

static bool tex_image(trc_replay_context_t* ctx, trace_command_t* cmd, bool dsa, GLuint tex_or_target,
                      GLint level, GLint internal_format, GLint border, GLenum format, GLenum type,
                      bool sub, int dim, ...) {
    int size[3];
    int offset[3];
    va_list list;
    va_start(list, dim);
    for (int i = 0; i < dim; i++) size[i] = va_arg(list, int);
    for (int i = 0; (i<dim) && sub; i++) offset[i] = va_arg(list, int);
    va_end(list);
    
    const trc_gl_texture_rev_t* tex_rev;
    if (dsa) tex_rev = trc_get_gl_texture(ctx->trace, tex_or_target);
    else tex_rev = replay_get_bound_tex(ctx, tex_or_target, &tex_or_target);
    if (!tex_rev) ERROR2(false, dsa?"Invalid texture name":"No texture bound to target");
    if (!tex_rev->has_object) ERROR2(false, "Texture name has no object");
    
    int max_size = trc_gl_state_get_state_int(ctx->trace, GL_MAX_TEXTURE_SIZE, 0);
    if (level<0 || level>ceil(log2(max_size))) ERROR2(false, "Invalid level");
    for (int i = 0; i < dim; i++)
        if (size[i]<0 || size[i]>max_size) ERROR2(false, "Invalid %s", (const char*[]){"width", "height", "depth/layers"}[i]);
    if (sub) {
        size_t img_count = tex_rev->images->size / sizeof(trc_gl_texture_image_t);
        trc_gl_texture_image_t* images = trc_map_data(tex_rev->images, TRC_MAP_READ);
        trc_gl_texture_image_t* image = NULL;
        for (size_t i = 0; i < img_count; i++) {
            if (images[i].level == level) {
                image = &images[i];
                break;
            }
        }
        if (image == NULL) {
            trc_unmap_data(tex_rev->images);
            ERROR2(false, "No such mipmap");
        }
        internal_format = image->internal_format; //Used for format validation later
        if (dim>=1 && (offset[0]<0 || offset[0]+size[0] > image->width)) ERROR2(false, "Invalid x range");
        if (dim>=2 && (offset[1]<0 || offset[1]+size[1] > image->height)) ERROR2(false, "Invalid x range");
        if (dim>=3 && (offset[2]<0 || offset[2]+size[2] > image->depth)) ERROR2(false, "Invalid x range");
        trc_unmap_data(tex_rev->images);
    }
    if (border != 0) ERROR2(false, "Border must be 0");
    
    if (!not_one_of(type, GL_UNSIGNED_BYTE_3_3_2, GL_UNSIGNED_BYTE_2_3_3_REV, GL_UNSIGNED_SHORT_5_6_5, GL_UNSIGNED_SHORT_5_6_5_REV, -1) && format!=GL_RGB)
        ERROR2(false, "Invalid format + internal format combination");
    if (!not_one_of(type, GL_UNSIGNED_SHORT_4_4_4_4, GL_UNSIGNED_SHORT_4_4_4_4_REV, GL_UNSIGNED_SHORT_5_5_5_1, GL_UNSIGNED_SHORT_1_5_5_5_REV, GL_UNSIGNED_INT_8_8_8_8,
        GL_UNSIGNED_INT_8_8_8_8_REV, GL_UNSIGNED_INT_10_10_10_2, GL_UNSIGNED_INT_2_10_10_10_REV, -1) && format!=GL_RGBA && format!=GL_BGRA)
        ERROR2(false, "Invalid format + internal format combination");
    if (format==GL_DEPTH_COMPONENT && not_one_of(internal_format, GL_DEPTH_COMPONENT, GL_DEPTH_COMPONENT16, GL_DEPTH_COMPONENT24, GL_DEPTH_COMPONENT32, -1))
        ERROR2(false, "Invalid format + internal format combination");
    if (format!=GL_DEPTH_COMPONENT && !not_one_of(internal_format, GL_DEPTH_COMPONENT, GL_DEPTH_COMPONENT16, GL_DEPTH_COMPONENT24, GL_DEPTH_COMPONENT32, -1))
        ERROR2(false, "Invalid format + internal format combination");
    if (format==GL_STENCIL_INDEX && internal_format!=GL_STENCIL_INDEX)
        ERROR2(false, "Invalid format + internal format combination");
    if (format!=GL_STENCIL_INDEX && internal_format==GL_STENCIL_INDEX)
        ERROR2(false, "Invalid format + internal format combination");
    
    GLuint pu_buf = trc_gl_state_get_bound_buffer(ctx->trace, GL_PIXEL_UNPACK_BUFFER);
    const trc_gl_buffer_rev_t* pu_buf_rev = trc_get_gl_buffer(ctx->trace, pu_buf);
    if (pu_buf_rev && pu_buf_rev->mapped) ERROR2(false, "GL_PIXEL_UNPACK_BUFFER is mapped");
    //TODO: More validation for GL_PIXEL_UNPACK_BUFFER
    
    return true;
}

glXMakeCurrent: //Display* p_dpy, GLXDrawable p_drawable, GLXContext p_ctx
    SDL_GLContext glctx = NULL;
    if (p_ctx) {
        if (!(glctx=trc_get_real_gl_context(ctx->trace, p_ctx)))
            ERROR("Invalid GLX context");
    }
    
    if (SDL_GL_MakeCurrent(ctx->window, glctx) < 0)
        ERROR("Unable to make a context current");
    
    if (glctx) {
        reload_gl_funcs(ctx);
        trc_set_current_fake_gl_context(ctx->trace, p_ctx);
        if (!trc_gl_state_get_made_current_before(ctx->trace)) {
            trace_extra_t* extra = trc_get_extra(cmd, "replay/glXMakeCurrent/drawable_size");
            if (!extra) ERROR("replay/glXMakeCurrent/drawable_size extra not found");
            if (extra->size != 8) ERROR("replay/glXMakeCurrent/drawable_size is not 8 bytes");
            int32_t width = ((int32_t*)extra->data)[0];
            int32_t height = ((int32_t*)extra->data)[1];
            if (width>=0 && height>=0) {
                trc_gl_state_set_drawable_width(ctx->trace, width);
                trc_gl_state_set_drawable_height(ctx->trace, height);
                SDL_SetWindowSize(ctx->window, width, height);
                for (size_t i = 0; i < trc_gl_state_get_state_int(ctx->trace, GL_MAX_VIEWPORTS, 0); i++) {
                    trc_gl_state_set_state_float(ctx->trace, GL_VIEWPORT, i*4+2, width);
                    trc_gl_state_set_state_float(ctx->trace, GL_VIEWPORT, i*4+3, height);
                    trc_gl_state_set_state_int(ctx->trace, GL_SCISSOR_BOX, i*4+2, width);
                    trc_gl_state_set_state_int(ctx->trace, GL_SCISSOR_BOX, i*4+3, height);
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
        trc_gl_state_set_made_current_before(ctx->trace, true);
    } else {
        reset_gl_funcs(ctx);
        trc_set_current_fake_gl_context(ctx->trace, 0);
    }
    
    //Seems to be messing up the front buffer.
    //But the front buffer is still sometimes black when it should not be.
    /*SDL_GL_SetSwapInterval(0)*/

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

glXCreateContext: //Display* p_dpy, XVisualInfo* p_vis, GLXContext p_shareList, Bool p_direct
    SDL_GLContext shareList = NULL;
    if (p_shareList) {
        shareList = trc_get_real_gl_context(ctx->trace, (uint64_t)p_shareList);
        if (!shareList) ERROR("Invalid share context name");
    }
    
    SDL_GLContext last_ctx = SDL_GL_GetCurrentContext();
    if (shareList) {
        SDL_GL_MakeCurrent(ctx->window, shareList);
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
    reload_gl_funcs(ctx);
    
    trc_gl_context_rev_t rev;
    rev.real = res;
    uint64_t fake = trc_get_ptr(&cmd->ret)[0];
    trc_set_gl_context(ctx->trace, fake, &rev);
    
    uint64_t prev_fake = trc_get_current_fake_gl_context(ctx->trace);
    size_t end = ctx->trace->inspection.cur_ctx_revision_count - 1;
    ctx->trace->inspection.cur_ctx_revisions[end].context = fake; //A hack
    init_context(ctx);
    ctx->trace->inspection.cur_ctx_revisions[end].context = prev_fake;
    
    SDL_GL_MakeCurrent(ctx->window, last_ctx);
    reload_gl_funcs(ctx);

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
    
    SDL_GLContext share_ctx = NULL;
    if (p_share_context) {
        share_ctx = trc_get_real_gl_context(ctx->trace, (uint64_t)p_share_context);
        if (!share_ctx) ERROR("Invalid share context name");
    }
    
    SDL_GLContext last_ctx = SDL_GL_GetCurrentContext();
    if (share_ctx) {
        SDL_GL_MakeCurrent(ctx->window, share_ctx);
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
    reload_gl_funcs(ctx);
    
    trc_gl_context_rev_t rev;
    rev.real = res;
    uint64_t fake = trc_get_ptr(&cmd->ret)[0];
    trc_set_gl_context(ctx->trace, fake, &rev);
    
    uint64_t prev_fake = trc_get_current_fake_gl_context(ctx->trace);
    size_t end = ctx->trace->inspection.cur_ctx_revision_count - 1;
    ctx->trace->inspection.cur_ctx_revisions[end].context = fake; //A hack
    init_context(ctx);
    ctx->trace->inspection.cur_ctx_revisions[end].context = prev_fake;
    
    SDL_GL_MakeCurrent(ctx->window, last_ctx);
    reload_gl_funcs(ctx);
    
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, last_major);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, last_minor);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, last_flags);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, last_profile);

glXQueryExtensionsString: //Display* p_dpy, int p_screen
    ;

glXDestroyContext: //Display* p_dpy, GLXContext p_ctx
    SDL_GLContext glctx = trc_get_real_gl_context(ctx->trace, (uint64_t)p_ctx);
    if (!glctx) ERROR("Invalid context name");
    
    SDL_GL_DeleteContext(glctx);
    //TODO
    //replay_rel_object(ctx, ReplayObjType_GLXContext, p_ctx);

glXSwapBuffers: //Display* p_dpy, GLXDrawable p_drawable
    if (!trc_get_current_fake_gl_context(ctx->trace)) ERROR("No current OpenGL context");
    SDL_GL_SwapWindow(ctx->window);
    replay_update_fb0_buffers(ctx, false, true, false, false);

glSetContextCapsWIP15: //
    ;

glClear: //GLbitfield p_mask
    real(p_mask);
    update_buffers(ctx, trc_gl_state_get_draw_framebuffer(ctx->trace), p_mask);

glGenTextures: //GLsizei p_n, GLuint* p_textures
    if (p_n < 0) ERROR("Invalid texture name count");
    GLuint* textures = replay_alloc(p_n*sizeof(GLuint));
    real(p_n, textures);
    gen_textures(ctx, p_n, textures, p_textures, false, 0);

glCreateTextures: //GLenum p_target, GLsizei p_n, GLuint* p_textures
    if (p_n < 0) ERROR("Invalid texture name count");
    GLuint* textures = replay_alloc(p_n*sizeof(GLuint));
    real(p_target, p_n, textures);
    gen_textures(ctx, p_n, textures, p_textures, true, p_target);

glDeleteTextures: //GLsizei p_n, const GLuint* p_textures
    GLuint* textures = replay_alloc(p_n*sizeof(GLuint));
    for (size_t i = 0; i < p_n; ++i)
        if (!(textures[i] = trc_get_real_gl_texture(ctx->trace, p_textures[i])))
            trc_add_error(cmd, "Invalid texture name");
        else trc_rel_gl_obj(ctx->trace, p_textures[i], TrcGLObj_Texture);
    real(p_n, textures);

glActiveTexture: //GLenum p_texture
    if (p_texture<GL_TEXTURE0 || p_texture-GL_TEXTURE0>=trc_gl_state_get_state_int(ctx->trace, GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS, 0))
        ERROR("No such texture unit");
    trc_gl_state_set_active_texture_unit(ctx->trace, p_texture-GL_TEXTURE0);
    real(p_texture);

glBindTexture: //GLenum p_target, GLuint p_texture
    const trc_gl_texture_rev_t* rev = trc_get_gl_texture(ctx->trace, p_texture);
    if (!rev && p_texture) ERROR("Invalid texture name");
    real(p_target, p_texture?rev->real:0);
    if (rev && !rev->has_object) {
        trc_gl_texture_rev_t newrev = *rev;
        newrev.has_object = true;
        newrev.type = p_target;
        newrev.images = trc_create_data(ctx->trace, 0, NULL, TRC_DATA_IMMUTABLE);
        trc_set_gl_texture(ctx->trace, p_texture, &newrev);
    } else if (rev && rev->type!=p_target) {
        ERROR("Invalid target for texture object");
    }
    //TODO: Reference counting
    uint unit = trc_gl_state_get_active_texture_unit(ctx->trace);
    trc_gl_state_set_bound_textures(ctx->trace, p_target, unit, p_texture);

glBindTextures: //GLuint p_first, GLsizei p_count, const GLuint* p_textures
    if (p_first+p_count>trc_gl_state_get_state_int(ctx->trace, GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS, 0) || p_count<0)
        ERROR("Invalid range");
    GLuint* real_tex = replay_alloc(p_count*sizeof(GLuint));
    //TODO: This probably creates a new trc_data_t for each texture
    for (size_t i = p_first; i < p_first+p_count; i++) {
        const trc_gl_texture_rev_t* rev = trc_get_gl_texture(ctx->trace, p_textures[i]);
        if (!rev) ERROR("Invalid texture name at index %zu", i);
        if (!rev->has_object) ERROR("Texture name at index %zu has no object", i);
        real_tex[i] = rev->real;
        
        if (p_textures[i]) {
            trc_gl_state_set_bound_textures(ctx->trace, rev->type, i, p_textures[i]);
        } else {
            GLenum targets[] = {GL_TEXTURE_1D, GL_TEXTURE_2D, GL_TEXTURE_3D,
                                GL_TEXTURE_1D_ARRAY, GL_TEXTURE_2D_ARRAY,
                                GL_TEXTURE_RECTANGLE, GL_TEXTURE_BUFFER,
                                GL_TEXTURE_CUBE_MAP, GL_TEXTURE_CUBE_MAP_ARRAY,
                                GL_TEXTURE_2D_MULTISAMPLE, GL_TEXTURE_2D_MULTISAMPLE_ARRAY};
            for (size_t j = 0; j < sizeof(targets)/sizeof(targets[0]); j++)
                trc_gl_state_set_bound_textures(ctx->trace, targets[j], i, 0);
        }
    }
    real(p_first, p_count, real_tex);

glTexImage1D: //GLenum p_target, GLint p_level, GLint p_internalformat, GLsizei p_width, GLint p_border, GLenum p_format, GLenum p_type, const void* p_pixels
    if (tex_image(ctx, cmd, false, p_target, p_level, p_internalformat, p_border, p_format, p_type, false, 1, p_width)) {
        real(p_target, p_level, p_internalformat, p_width, p_border, p_format, p_type, p_pixels);
        replay_update_bound_tex_image(ctx, cmd, p_target, p_level);
    }

glCompressedTexImage1D: //GLenum p_target, GLint p_level, GLenum p_internalformat, GLsizei p_width, GLint p_border, GLsizei p_imageSize, const void* p_data
    real(p_target, p_level, p_internalformat, p_width, p_border, p_imageSize, p_data);
    replay_update_bound_tex_image(ctx, cmd, p_target, p_level);

glTexSubImage1D: //GLenum p_target, GLint p_level, GLint p_xoffset, GLsizei p_width, GLenum p_format, GLenum p_type, const void* p_pixels
    if (tex_image(ctx, cmd, false, p_target, p_level, 0, 0, p_format, p_type, true, 1, p_width, p_xoffset)) {
        real(p_target, p_level, p_xoffset, p_width, p_format, p_type, p_pixels);
        replay_update_bound_tex_image(ctx, cmd, p_target, p_level);
    }

glCompressedTexSubImage1D: //GLenum p_target, GLint p_level, GLint p_xoffset, GLsizei p_width, GLenum p_format, GLsizei p_imageSize, const void* p_data
    real(p_target, p_level, p_xoffset, p_width, p_format, p_imageSize, p_data);
    replay_update_bound_tex_image(ctx, cmd, p_target, p_level);

glTexImage2D: //GLenum p_target, GLint p_level, GLint p_internalformat, GLsizei p_width, GLsizei p_height, GLint p_border, GLenum p_format, GLenum p_type, const void* p_pixels
    if (tex_image(ctx, cmd, false, p_target, p_level, p_internalformat, p_border, p_format, p_type, false, 2, p_width, p_height)) {
        real(p_target, p_level, p_internalformat, p_width, p_height, p_border, p_format, p_type, p_pixels);
        replay_update_bound_tex_image(ctx, cmd, p_target, p_level);
    }

glCompressedTexImage2D: //GLenum p_target, GLint p_level, GLenum p_internalformat, GLsizei p_width, GLsizei p_height, GLint p_border, GLsizei p_imageSize, const void* p_data
    real(p_target, p_level, p_internalformat, p_width, p_height, p_border, p_imageSize, p_data);
    replay_update_bound_tex_image(ctx, cmd, p_target, p_level);

glTexSubImage2D: //GLenum p_target, GLint p_level, GLint p_xoffset, GLint p_yoffset, GLsizei p_width, GLsizei p_height, GLenum p_format, GLenum p_type, const void* p_pixels
    if (tex_image(ctx, cmd, false, p_target, p_level, 0, 0, p_format, p_type, true, 2, p_width, p_height, p_xoffset, p_yoffset)) {
        real(p_target, p_level, p_xoffset, p_yoffset, p_width, p_height, p_format, p_type, p_pixels);
        replay_update_bound_tex_image(ctx, cmd, p_target, p_level);
    }

glCompressedTexSubImage2D: //GLenum p_target, GLint p_level, GLint p_xoffset, GLint p_yoffset, GLsizei p_width, GLsizei p_height, GLenum p_format, GLsizei p_imageSize, const void* p_data
    real(p_target, p_level, p_xoffset, p_yoffset, p_width, p_height, p_format, p_imageSize, p_data);
    replay_update_bound_tex_image(ctx, cmd, p_target, p_level);

glTexImage3D: //GLenum p_target, GLint p_level, GLint p_internalformat, GLsizei p_width, GLsizei p_height, GLsizei p_depth, GLint p_border, GLenum p_format, GLenum p_type, const void* p_pixels
    //TODO: Array textures?
    if (tex_image(ctx, cmd, false, p_target, p_level, p_internalformat, p_border, p_format, p_type, false, 3, p_width, p_height, p_depth)) {
        real(p_target, p_level, p_internalformat, p_width, p_height, p_depth, p_border, p_format, p_type, p_pixels);
        replay_update_bound_tex_image(ctx, cmd, p_target, p_level);
    }

glCompressedTexImage3D: //GLenum p_target, GLint p_level, GLenum p_internalformat, GLsizei p_width, GLsizei p_height, GLsizei p_depth, GLint p_border, GLsizei p_imageSize, const void* p_data
    //TODO: Array textures
    real(p_target, p_level, p_internalformat, p_width, p_height, p_depth, p_border, p_imageSize, p_data);
    replay_update_bound_tex_image(ctx, cmd, p_target, p_level);

glTexSubImage3D: //GLenum p_target, GLint p_level, GLint p_xoffset, GLint p_yoffset, GLint p_zoffset, GLsizei p_width, GLsizei p_height, GLsizei p_depth, GLenum p_format, GLenum p_type, const void* p_pixels
    if (tex_image(ctx, cmd, false, p_target, p_level, 0, 0, p_format, p_type, true, 3, p_width, p_height, p_depth, p_xoffset, p_yoffset, p_zoffset)) {
        real(p_target, p_level, p_xoffset, p_yoffset, p_zoffset, p_width, p_height, p_depth, p_format, p_type, p_pixels);
        replay_update_bound_tex_image(ctx, cmd, p_target, p_level);
    }

glCompressedTexSubImage3D: //GLenum p_target, GLint p_level, GLint p_xoffset, GLint p_yoffset, GLint p_zoffset, GLsizei p_width, GLsizei p_height, GLsizei p_depth, GLenum p_format, GLsizei p_imageSize, const void* p_data
    real(p_target, p_level, p_xoffset, p_yoffset, p_zoffset, p_width, p_height, p_depth, p_format, p_imageSize, p_data);
    replay_update_bound_tex_image(ctx, cmd, p_target, p_level);

glTexImage2DMultisample: //GLenum p_target, GLsizei p_samples, GLenum p_internalformat, GLsizei p_width, GLsizei p_height, GLboolean p_fixedsamplelocations
    real(p_target, p_samples, p_internalformat, p_width, p_height, p_fixedsamplelocations);
    //TODO
    //replay_get_tex_params(ctx, cmd, target);

glTexImage3DMultisample: //GLenum p_target, GLsizei p_samples, GLenum p_internalformat, GLsizei p_width, GLsizei p_height, GLsizei p_depth, GLboolean p_fixedsamplelocations
    real(p_target, p_samples, p_internalformat, p_width, p_height, p_depth, p_fixedsamplelocations);
    //TODO
    //replay_get_tex_params(ctx, cmd, target);

glCopyTexImage1D: //GLenum p_target, GLint p_level, GLenum p_internalformat, GLint p_x, GLint p_y, GLsizei p_width, GLint p_border
    real(p_target, p_level, p_internalformat, p_x, p_y, p_width, p_border);
    replay_update_bound_tex_image(ctx, cmd, p_target, p_level);

glCopyTexImage2D: //GLenum p_target, GLint p_level, GLenum p_internalformat, GLint p_x, GLint p_y, GLsizei p_width, GLsizei p_height, GLint p_border
    real(p_target, p_level, p_internalformat, p_x, p_y, p_width, p_height, p_border);
    replay_update_bound_tex_image(ctx, cmd, p_target, p_level);

glCopyTexSubImage1D: //GLenum p_target, GLint p_level, GLint p_xoffset, GLint p_x, GLint p_y, GLsizei p_width
    real(p_target, p_level, p_xoffset, p_x, p_y, p_width);
    replay_update_bound_tex_image(ctx, cmd, p_target, p_level);

glCopyTexSubImage2D: //GLenum p_target, GLint p_level, GLint p_xoffset, GLint p_yoffset, GLint p_x, GLint p_y, GLsizei p_width, GLsizei p_height
    real(p_target, p_level, p_xoffset, p_yoffset, p_x, p_y, p_width, p_height);
    replay_update_bound_tex_image(ctx, cmd, p_target, p_level);

glCopyTexSubImage3D: //GLenum p_target, GLint p_level, GLint p_xoffset, GLint p_yoffset, GLint p_zoffset, GLint p_x, GLint p_y, GLsizei p_width, GLsizei p_height
    real(p_target, p_level, p_xoffset, p_yoffset, p_zoffset, p_x, p_y, p_width, p_height);
    replay_update_bound_tex_image(ctx, cmd, p_target, p_level);

glTexBuffer: //GLenum p_target, GLenum p_internalformat, GLuint p_buffer
    if (tex_buffer(ctx, cmd, p_target, false, p_internalformat, p_buffer, 0, -1))
        real(p_target, p_internalformat, p_buffer?p_buffer_rev->real:0);

glTexBufferRange: //GLenum p_target, GLenum p_internalformat, GLuint p_buffer, GLintptr p_offset, GLsizeiptr p_size
    if (tex_buffer(ctx, cmd, p_target, false, p_internalformat, p_buffer, p_offset, p_size))
        real(p_target, p_internalformat, p_buffer?p_buffer_rev->real:0, p_offset, p_size);

glTextureBuffer: //GLuint p_texture, GLenum p_internalformat, GLuint p_buffer
    if (tex_buffer(ctx, cmd, p_texture, true, p_internalformat, p_buffer, 0, -1))
        real(p_texture_rev->real, p_internalformat, p_buffer?p_buffer_rev->real:0);

glTextureBufferRange: //GLuint p_texture, GLenum p_internalformat, GLuint p_buffer, GLintptr p_offset, GLsizeiptr p_size
    if (tex_buffer(ctx, cmd, p_texture, true, p_internalformat, p_buffer, p_offset, p_size))
        real(p_texture_rev->real, p_internalformat, p_buffer?p_buffer_rev->real:0, p_offset, p_size);

glGenerateMipmap: //GLenum p_target
    real(p_target);
    
    uint unit = trc_gl_state_get_active_texture_unit(ctx->trace);
    uint fake = trc_gl_state_get_bound_textures(ctx->trace, p_target, unit);
    if (!fake) ERROR("No texture bound");
    
    GLint base;
    F(glGetTexParameteriv)(p_target, GL_TEXTURE_BASE_LEVEL, &base);
    GLint w, h, d;
    F(glGetTexLevelParameteriv)(p_target, base, GL_TEXTURE_WIDTH, &w);
    F(glGetTexLevelParameteriv)(p_target, base, GL_TEXTURE_HEIGHT, &h);
    F(glGetTexLevelParameteriv)(p_target, base, GL_TEXTURE_DEPTH, &d);
    uint level = base;
    while (w || h || d) {
        if (level != base)
            replay_update_bound_tex_image(ctx, cmd, p_target, level);
        w /= 2;
        h /= 2;
        d /= 2;
        level++;
    }

glTexParameterf: //GLenum p_target, GLenum p_pname, GLfloat p_param
    GLdouble double_param = p_param;
    if (!texture_param_double(ctx, cmd, false, p_target, p_pname, 1, &double_param))
        real(p_target, p_pname, p_param);

glTexParameteri: //GLenum p_target, GLenum p_pname, GLint p_param
    GLdouble double_param = p_param;
    if (!texture_param_double(ctx, cmd, false, p_target, p_pname, 1, &double_param))
        real(p_target, p_pname, p_param);

glTexParameterfv: //GLenum p_target, GLenum p_pname, const GLfloat* p_params
    trace_value_t* paramsv = trc_get_arg(cmd, 2);
    const double* paramsd = trc_get_double(paramsv);
    if (!texture_param_double(ctx, cmd, false, p_target, p_pname, paramsv->count, paramsd))
        real(p_target, p_pname, p_params);

glTexParameteriv: //GLenum p_target, GLenum p_pname, const GLint* p_params
    trace_value_t* paramsv = trc_get_arg(cmd, 2);
    const int64_t* params64 = trc_get_int(paramsv);
    double* double_params = replay_alloc(paramsv->count*sizeof(double));
    for (size_t i = 0; i < paramsv->count; i++) double_params[i] = params64[i];
    if (!texture_param_double(ctx, cmd, false, p_target, p_pname, paramsv->count, double_params))
        real(p_target, p_pname, p_params);

glTexParameterIiv: //GLenum p_target, GLenum p_pname, const GLint* p_params
    //TODO
    real(p_target, p_pname, p_params);

glTexParameterIuiv: //GLenum p_target, GLenum p_pname, const GLuint* p_params
    //TODO
    real(p_target, p_pname, p_params);

glTextureParameterf: //GLuint p_texture, GLenum p_pname, GLfloat p_param
    GLdouble double_param = p_param;
    if (!texture_param_double(ctx, cmd, true, p_texture, p_pname, 1, &double_param))
        real(p_texture_rev->real, p_pname, p_param);

glTextureParameteri: //GLuint p_texture, GLenum p_pname, GLint p_param
    GLdouble double_param = p_param;
    if (!texture_param_double(ctx, cmd, true, p_texture, p_pname, 1, &double_param))
        real(p_texture_rev->real, p_pname, p_param);

glTextureParameterfv: //GLuint p_texture, GLenum p_pname, const GLfloat* p_param
    trace_value_t* paramsv = trc_get_arg(cmd, 2);
    const double* paramsd = trc_get_double(paramsv);
    if (!texture_param_double(ctx, cmd, true, p_texture, p_pname, paramsv->count, paramsd))
        real(p_texture_rev->real, p_pname, p_param);

glTextureParameteriv: //GLuint p_texture, GLenum p_pname, const GLint* p_param
    trace_value_t* paramsv = trc_get_arg(cmd, 2);
    double* double_params = replay_alloc(paramsv->count*sizeof(double));
    for (size_t i = 0; i < paramsv->count; i++) double_params[i] = p_param[i];
    if (!texture_param_double(ctx, cmd, true, p_texture, p_pname, paramsv->count, double_params))
        real(p_texture_rev->real, p_pname, p_param);

glTextureParameterIiv: //GLuint p_texture, GLenum p_pname, const GLint* p_params
    if (!p_texture_rev) ERROR("Invalid texture name");
    if (!p_texture_rev->has_object) ERROR("Texture name has no object");
    //TODO
    real(p_texture_rev->real, p_pname, p_params);

glTextureParameterIuiv: //GLuint p_texture, GLenum p_pname, const GLuint* p_params
    if (!p_texture_rev) ERROR("Invalid texture name");
    if (!p_texture_rev->has_object) ERROR("Texture name has no object");
    //TODO
    real(p_texture_rev->real, p_pname, p_params);

glGenBuffers: //GLsizei p_n, GLuint* p_buffers
    if (p_n < 0) ERROR("Invalid buffer name count");
    GLuint* buffers = replay_alloc(p_n*sizeof(GLuint));
    real(p_n, buffers);
    gen_buffers(ctx, p_n, buffers, p_buffers, false);

glCreateBuffers: //GLsizei p_n, GLuint* p_buffers
    if (p_n < 0) ERROR("Invalid buffer name count");
    GLuint* buffers = replay_alloc(p_n*sizeof(GLuint));
    real(p_n, buffers);
    gen_buffers(ctx, p_n, buffers, p_buffers, true);

glDeleteBuffers: //GLsizei p_n, const GLuint* p_buffers
    GLuint* buffers = replay_alloc(p_n*sizeof(GLuint));
    for (size_t i = 0; i < p_n; ++i) {
        if (!(buffers[i] = trc_get_real_gl_buffer(ctx->trace, p_buffers[i])))
            trc_add_error(cmd, "Invalid buffer name");
        else trc_rel_gl_obj(ctx->trace, p_buffers[i], TrcGLObj_Buffer);
    }
    real(p_n, buffers);

glBindBuffer: //GLenum p_target, GLuint p_buffer
    const trc_gl_buffer_rev_t* rev = trc_get_gl_buffer(ctx->trace, p_buffer);
    if (!rev && p_buffer) ERROR("Invalid buffer name");
    trc_gl_state_set_bound_buffer(ctx->trace, p_target, p_buffer);
    real(p_target, p_buffer?rev->real:0);
    if (rev && !rev->has_object) {
        trc_gl_buffer_rev_t newrev = *rev;
        newrev.has_object = true;
        trc_set_gl_buffer(ctx->trace, p_buffer, &newrev);
    }

glBindBufferBase: //GLenum p_target, GLuint p_index, GLuint p_buffer
    const trc_gl_buffer_rev_t* rev = trc_get_gl_buffer(ctx->trace, p_buffer);
    if (!rev && p_buffer) ERROR("Invalid buffer name");
    real(p_target, p_index, p_buffer?rev->real:0);
    if (rev && !rev->has_object) {
        trc_gl_buffer_rev_t newrev = *rev;
        newrev.has_object = true;
        trc_set_gl_buffer(ctx->trace, p_buffer, &newrev);
    }

glBindBufferRange: //GLenum p_target, GLuint p_index, GLuint p_buffer, GLintptr p_offset, GLsizeiptr p_size
    const trc_gl_buffer_rev_t* rev = trc_get_gl_buffer(ctx->trace, p_buffer);
    if (!rev && p_buffer) ERROR("Invalid buffer name");
    real(p_target, p_index, p_buffer?rev->real:0, p_offset, p_size);
    if (rev && !rev->has_object) {
        trc_gl_buffer_rev_t newrev = *rev;
        newrev.has_object = true;
        trc_set_gl_buffer(ctx->trace, p_buffer, &newrev);
    }

glBufferData: //GLenum p_target, GLsizeiptr p_size, const void* p_data, GLenum p_usage
    uint fake = get_bound_buffer(ctx, p_target);
    if (buffer_data(ctx, cmd, false, fake, p_size, p_data, p_usage))
        real(p_target, p_size, p_data, p_usage);

glNamedBufferData: //GLuint p_buffer, GLsizeiptr p_size, const void* p_data, GLenum p_usage
    if (buffer_data(ctx, cmd, true, p_buffer, p_size, p_data, p_usage))
        real(p_buffer_rev->real, p_size, p_data, p_usage);

glBufferSubData: //GLenum p_target, GLintptr p_offset, GLsizeiptr p_size, const void* p_data
    uint fake = get_bound_buffer(ctx, p_target);
    if (!fake) ERROR("No buffer bound to target");
    if (buffer_sub_data(ctx, cmd, false, fake, p_offset, p_size, p_data))
        real(p_target, p_offset, p_size, p_data);

glNamedBufferSubData: //GLuint p_buffer, GLintptr p_offset, GLsizeiptr p_size, const void* p_data
    if (buffer_sub_data(ctx, cmd, true, p_buffer, p_offset, p_size, p_data))
        real(p_buffer_rev->real, p_offset, p_size, p_data);

glCopyBufferSubData: //GLenum p_readTarget, GLenum p_writeTarget, GLintptr p_readOffset, GLintptr p_writeOffset, GLsizeiptr p_size
    uint read_fake = get_bound_buffer(ctx, p_readTarget);
    uint write_fake = get_bound_buffer(ctx, p_writeTarget);
    if (copy_buffer_data(ctx, cmd, false, read_fake, write_fake, p_readOffset, p_writeOffset, p_size))
        real(p_readTarget, p_writeTarget, p_readOffset, p_writeOffset, p_size);

glCopyNamedBufferSubData: //GLenum p_readBuffer, GLenum p_writeBuffer, GLintptr p_readOffset, GLintptr p_writeOffset, GLsizeiptr p_size
    if (copy_buffer_data(ctx, cmd, true, p_readBuffer, p_writeBuffer, p_readOffset, p_writeOffset, p_size))
        real(p_readBuffer_rev->real, p_writeBuffer_rev->real, p_readOffset, p_writeOffset, p_size);

glMapBuffer: //GLenum p_target, GLenum p_access
    uint fake = get_bound_buffer(ctx, p_target);
    map_buffer(ctx, cmd, false, fake, p_access);

glMapNamedBuffer: //GLuint p_buffer, GLenum p_access
    map_buffer(ctx, cmd, true, p_buffer, p_access);

glMapBufferRange: //GLenum p_target, GLintptr p_offset, GLsizeiptr p_length, GLbitfield p_access
    map_buffer_range(ctx, cmd, false, get_bound_buffer(ctx, p_target), p_offset, p_length, p_access);

glMapNamedBufferRange: //GLuint p_buffer, GLintptr p_offset, GLsizeiptr p_length, GLbitfield p_access
    map_buffer_range(ctx, cmd, true, p_buffer, p_offset, p_length, p_access);

glUnmapBuffer: //GLenum p_target
    trace_extra_t* extra = trc_get_extra(cmd, "replay/glUnmapBuffer/data");
    if (!extra) ERROR("replay/glUnmapBuffer/data extra not found");
    
    uint fake = get_bound_buffer(ctx, p_target);
    const trc_gl_buffer_rev_t* buf_rev_ptr = trc_get_gl_buffer(ctx->trace, fake);
    if (!buf_rev_ptr) ERROR("Invalid buffer name");
    trc_gl_buffer_rev_t buf = *buf_rev_ptr;
    if (!buf.data) ERROR("Buffer has no data");
    
    if (extra->size != buf.data->size) ERROR("Invalid trace");
    
    if (!buf.mapped) ERROR("Unmapping a buffer that is not mapped");
    
    trc_gl_buffer_rev_t old = buf;
    
    if (buf.map_access & GL_MAP_WRITE_BIT) {
        F(glBufferSubData)(p_target, 0, extra->size, extra->data);
        
        buf.data = trc_create_data(ctx->trace, old.data->size, NULL, TRC_DATA_NO_ZERO);
        void* newdata = trc_map_data(buf.data, TRC_MAP_REPLACE);
        
        void* olddata = trc_map_data(old.data, TRC_MAP_READ);
        memcpy(newdata, olddata, old.data->size);
        trc_unmap_data(old.data);
        
        memcpy(newdata, extra->data, extra->size);
        trc_unmap_freeze_data(ctx->trace, buf.data);
    }
    
    buf.mapped = false;
    buf.map_offset = 0;
    buf.map_length = 0;
    buf.map_access = 0;
    
    trc_set_gl_buffer(ctx->trace, fake, &buf);

glCreateShader: //GLenum p_type
    GLuint real_shdr = F(glCreateShader)(p_type);
    GLuint fake = trc_get_uint(&cmd->ret)[0];
    trc_gl_shader_rev_t rev;
    rev.fake_context = trc_get_current_fake_gl_context(ctx->trace);
    rev.ref_count = 1;
    rev.real = real_shdr;
    rev.sources = trc_create_data(ctx->trace, 0, NULL, TRC_DATA_IMMUTABLE);
    rev.info_log = trc_create_data(ctx->trace, 1, "", TRC_DATA_IMMUTABLE);
    rev.type = p_type;
    trc_set_gl_shader(ctx->trace, fake, &rev);

glDeleteShader: //GLuint p_shader
    if (p_shader == 0) RETURN;
    GLuint real_shdr = trc_get_real_gl_shader(ctx->trace, p_shader);
    if (!real_shdr) ERROR("Invalid shader name");
    
    F(glDeleteShader)(real_shdr);
    
    trc_rel_gl_obj(ctx->trace, p_shader, TrcGLObj_Shader);

glShaderSource: //GLuint p_shader, GLsizei p_count, const GLchar*const* p_string, const GLint* p_length
    GLuint shader = trc_get_real_gl_shader(ctx->trace, p_shader);
    if (!shader) ERROR("Invalid shader name");
    
    size_t res_sources_size = 0;
    char* res_sources = NULL;
    if (trc_get_arg(cmd, 3)->count == 0) {
        real(shader, p_count, p_string, NULL);
        for (GLsizei i = 0; i < p_count; i++) {
            res_sources = realloc(res_sources, res_sources_size+strlen(p_string[i])+1);
            memset(res_sources+res_sources_size, 0, strlen(p_string[i])+1);
            strcpy(res_sources+res_sources_size, p_string[i]);
            res_sources_size += strlen(p_string[i]) + 1;
        }
    } else {
        real(shader, p_count, p_string, p_length);
        for (GLsizei i = 0; i < p_count; i++) {
            res_sources = realloc(res_sources, res_sources_size+p_length[i]+1);
            memset(res_sources+res_sources_size, 0, p_length[i]+1);
            memcpy(res_sources+res_sources_size, p_string[i], p_length[i]);
            res_sources[res_sources_size+p_length[i]+1] = 0;
            res_sources_size += p_length[i] + 1;
        }
    }
    
    trc_gl_shader_rev_t shdr = *trc_get_gl_shader(ctx->trace, p_shader);
    
    shdr.sources = trc_create_data_no_copy(ctx->trace, res_sources_size, res_sources, TRC_DATA_IMMUTABLE);
    
    trc_set_gl_shader(ctx->trace, p_shader, &shdr);

glCompileShader: //GLuint p_shader
    GLuint real_shdr = trc_get_real_gl_shader(ctx->trace, p_shader);
    if (!real_shdr) ERROR("Invalid shader name");
    
    real(real_shdr);
    
    trc_gl_shader_rev_t shdr = *trc_get_gl_shader(ctx->trace, p_shader);
    
    GLint len;
    F(glGetShaderiv)(real_shdr, GL_INFO_LOG_LENGTH, &len);
    shdr.info_log = trc_create_data(ctx->trace, len+1, NULL, TRC_DATA_NO_ZERO);
    F(glGetShaderInfoLog)(real_shdr, len+1, NULL, trc_map_data(shdr.info_log, TRC_MAP_REPLACE));
    trc_unmap_freeze_data(ctx->trace, shdr.info_log);
    
    trc_set_gl_shader(ctx->trace, p_shader, &shdr);
    
    GLint status;
    F(glGetShaderiv)(real_shdr, GL_COMPILE_STATUS, &status);
    if (!status) ERROR("Failed to compile shader");

glCreateProgram: //
    GLuint real_program = F(glCreateProgram)();
    GLuint fake = trc_get_uint(&cmd->ret)[0];
    trc_gl_program_rev_t rev;
    rev.fake_context = trc_get_current_fake_gl_context(ctx->trace);
    rev.ref_count = 1;
    rev.real = real_program;
    trc_data_t* empty_data = trc_create_data(ctx->trace, 0, NULL, TRC_DATA_IMMUTABLE);
    rev.uniforms = empty_data;
    rev.vertex_attribs = empty_data;
    rev.uniform_blocks = empty_data;
    rev.shaders = empty_data;
    rev.info_log = trc_create_data(ctx->trace, 1, "", TRC_DATA_IMMUTABLE);
    rev.binary_retrievable_hint = -1;
    rev.separable = false;
    trc_set_gl_program(ctx->trace, fake, &rev);

glDeleteProgram: //GLuint p_program
    if (p_program == 0) RETURN;
    trc_gl_program_rev_t rev = *trc_get_gl_program(ctx->trace, p_program);
    if (!rev.real) ERROR("Invalid program name");
    real(rev.real);
    
    size_t shader_count = rev.shaders->size / sizeof(trc_gl_program_shader_t);
    trc_gl_program_shader_t* shaders = trc_map_data(rev.shaders, TRC_MAP_READ);
    for (size_t i = 0; i < shader_count; i++)
        trc_rel_gl_obj(ctx->trace, shaders[i].fake_shader, TrcGLObj_Shader);
    trc_unmap_data(rev.shaders);
    
    rev.shaders = trc_create_data(ctx->trace, 0, NULL, TRC_DATA_IMMUTABLE);
    trc_set_gl_program(ctx->trace, p_program, &rev);
    
    trc_rel_gl_obj(ctx->trace, p_program, TrcGLObj_Program);

glProgramParameteri: //GLuint p_program, GLenum p_pname, GLint p_value
    if (!p_program_rev) ERROR("Invalid program name");
    trc_gl_program_rev_t newrev = *p_program_rev;
    switch (p_pname) {
    case GL_PROGRAM_BINARY_RETRIEVABLE_HINT: newrev.binary_retrievable_hint = p_value ? 1 : 0; break;
    case GL_PROGRAM_SEPARABLE: newrev.separable = p_value; break;
    }
    trc_set_gl_program(ctx->trace, p_program, &newrev);
    real(p_program, p_pname, p_value);

glAttachShader: //GLuint p_program, GLuint p_shader
    GLuint real_program = trc_get_real_gl_program(ctx->trace, p_program);
    if (!real_program) ERROR("Invalid program name");
    
    GLuint real_shader = trc_get_real_gl_shader(ctx->trace, p_shader);
    if (!real_shader) ERROR("Invalid shader name");
    
    real(real_program, real_shader);
    
    trc_gl_program_rev_t program = *trc_get_gl_program(ctx->trace, p_program);
    trc_gl_program_rev_t old = program;
    const trc_gl_shader_rev_t* shader = trc_get_gl_shader(ctx->trace, p_shader);
    
    size_t shader_count = program.shaders->size / sizeof(trc_gl_program_shader_t);
    trc_gl_program_shader_t* src = trc_map_data(old.shaders, TRC_MAP_READ);
    
    for (size_t i = 0; i < shader_count; i++) {
        if (src[i].fake_shader == p_shader) ERROR("Shader is already attached");
    }
    
    program.shaders = trc_create_data(ctx->trace, (shader_count+1)*sizeof(trc_gl_program_shader_t), NULL, TRC_DATA_NO_ZERO);
    
    trc_gl_program_shader_t* dest = trc_map_data(program.shaders, TRC_MAP_REPLACE);
    memcpy(dest, src, shader_count*sizeof(trc_gl_program_shader_t));
    dest[shader_count].fake_shader = p_shader;
    dest[shader_count].shader_revision = shader->revision;
    trc_unmap_data(old.shaders);
    trc_unmap_freeze_data(ctx->trace, program.shaders);
    
    trc_grab_gl_obj(ctx->trace, p_shader, TrcGLObj_Shader);
    trc_set_gl_program(ctx->trace, p_program, &program);

glDetachShader: //GLuint p_program, GLuint p_shader
    GLuint real_program = trc_get_real_gl_program(ctx->trace, p_program);
    if (!real_program) ERROR("Invalid program name");
    
    GLuint real_shader = trc_get_real_gl_shader(ctx->trace, p_shader);
    if (!real_shader) ERROR("Invalid shader name");
    
    real(real_program, real_shader);
    
    trc_gl_program_rev_t program = *trc_get_gl_program(ctx->trace, p_program);
    trc_gl_program_rev_t old = program;
    
    size_t shader_count = program.shaders->size / sizeof(trc_gl_program_shader_t);
    program.shaders = trc_create_data(ctx->trace, (shader_count-1)*sizeof(trc_gl_program_shader_t), NULL, TRC_DATA_NO_ZERO);
    
    trc_gl_program_shader_t* dest = trc_map_data(program.shaders, TRC_MAP_REPLACE);
    trc_gl_program_shader_t* src = trc_map_data(old.shaders, TRC_MAP_READ);
    size_t next = 0;
    bool found = false;
    for (size_t i = 0; i < shader_count; i++) {
        if (src[i].fake_shader == p_shader) {found = true; continue;}
        dest[next++] = src[i];
    }
    trc_unmap_data(old.shaders);
    trc_unmap_freeze_data(ctx->trace, program.shaders);
    if (!found) ERROR("Shader is not attached to program");
    
    trc_rel_gl_obj(ctx->trace, p_shader, TrcGLObj_Shader);
    
    trc_set_gl_program(ctx->trace, p_program, &program);

glLinkProgram: //GLuint p_program
    GLuint real_program = trc_get_real_gl_program(ctx->trace, p_program);
    if (!real_program) ERROR("Invalid program name");
    
    real(real_program);
    
    trc_gl_program_rev_t rev = *trc_get_gl_program(ctx->trace, p_program);
    
    GLint len;
    F(glGetProgramiv)(real_program, GL_INFO_LOG_LENGTH, &len);
    rev.info_log = trc_create_data(ctx->trace, len+1, NULL, TRC_DATA_NO_ZERO);
    F(glGetProgramInfoLog)(real_program, len+1, NULL, trc_map_data(rev.info_log, TRC_MAP_REPLACE));
    trc_unmap_freeze_data(ctx->trace, rev.info_log);
    
    size_t uniform_count = 0;
    trc_gl_program_uniform_t* uniforms = NULL;
    trace_extra_t* uniform = NULL;
    size_t i = 0;
    while ((uniform=trc_get_extrai(cmd, "replay/program/uniform", i++))) {
        if (uniform->size < 8) continue;
        void* data = uniform->data;
        uint32_t fake_loc = le32toh(((uint32_t*)data)[0]);
        uint32_t len = le32toh(((uint32_t*)data)[1]);
        char* name = calloc(1, len+1);
        memcpy(name, (uint8_t*)data+8, len);
        
        GLint real_loc = F(glGetUniformLocation)(real_program, name);
        if (real_loc < 0) {
            trc_add_error(cmd, "Nonexistent or inactive uniform while adding uniforms", name);
        } else {
            uniforms = realloc(uniforms, (uniform_count+1)*sizeof(trc_gl_program_uniform_t));
            trc_gl_program_uniform_t uni;
            memset(&uni, 0, sizeof(uni)); //initialize padding to zero - it might be compressed
            uni.real = real_loc;
            uni.fake = fake_loc;
            uni.dim[0] = 0;
            uni.dim[1] = 0;
            uni.count = 0;
            uni.value = NULL;
            uniforms[uniform_count++] = uni;
        }
        
        free(name);
    }
    
    size_t vertex_attrib_count = 0;
    uint* vertex_attribs = NULL;
    trace_extra_t* attrib = NULL;
    i = 0;
    while ((attrib=trc_get_extrai(cmd, "replay/program/vertex_attrib", i++))) {
        if (attrib->size < 8) continue;
        void* data = attrib->data;
        uint32_t fake_idx = le32toh(((uint32_t*)data)[0]);
        uint32_t len = le32toh(((uint32_t*)data)[1]);
        char* name = calloc(1, len+1);
        memcpy(name, (uint8_t*)data+8, len);
        
        GLint real_idx = F(glGetAttribLocation)(real_program, name);
        if (real_idx < 0) {
            trc_add_error(cmd, "Nonexistent or inactive vertex attribute while adding vertex attributes");
        } else {
            vertex_attribs = realloc(vertex_attribs, (vertex_attrib_count+1)*sizeof(uint)*2);
            vertex_attribs[vertex_attrib_count*2] = real_idx;
            vertex_attribs[vertex_attrib_count++*2+1] = fake_idx;
        }
        
        free(name);
    }
    
    size_t uniform_block_count = 0;
    trc_gl_program_uniform_block_t* uniform_blocks = NULL;
    trace_extra_t* uniform_block = NULL;
    i = 0;
    while ((uniform_block=trc_get_extrai(cmd, "replay/program/uniform_block", i++))) {
        if (uniform_block->size < 8) continue;
        void* data = uniform_block->data;
        uint32_t fake_idx = le32toh(((uint32_t*)data)[0]);
        uint32_t len = le32toh(((uint32_t*)data)[1]);
        char* name = calloc(1, len+1);
        memcpy(name, (uint8_t*)data+8, len);
        
        GLint real_idx = F(glGetUniformBlockIndex)(real_program, name);
        if (real_idx < 0) {
            trc_add_error(cmd, "Nonexistent or inactive uniform block while adding uniform blocks");
        } else {
            uniform_blocks = realloc(uniform_blocks, (uniform_block_count+1)*sizeof(trc_gl_program_uniform_block_t));
            trc_gl_program_uniform_block_t block;
            memset(&block, 0, sizeof(block)); //initialize padding to zero - it might be compressed
            block.real = real_idx;
            block.fake = fake_idx;
            block.binding = 0;
            uniform_blocks[uniform_block_count++] = block;
        }
        
        free(name);
    }
    
    rev.uniform_blocks = trc_create_data_no_copy(ctx->trace, uniform_block_count*sizeof(trc_gl_program_uniform_block_t), uniform_blocks, TRC_DATA_IMMUTABLE);
    rev.vertex_attribs = trc_create_data_no_copy(ctx->trace, vertex_attrib_count*2*sizeof(uint), vertex_attribs, TRC_DATA_IMMUTABLE);
    rev.uniforms = trc_create_data_no_copy(ctx->trace, uniform_count*sizeof(trc_gl_program_uniform_t), uniforms, TRC_DATA_IMMUTABLE);
    
    trc_set_gl_program(ctx->trace, p_program, &rev);
    
    GLint status;
    F(glGetProgramiv)(real_program, GL_LINK_STATUS, &status);
    if (!status) ERROR("Failed to link program");

glValidateProgram: //GLuint p_program
    GLuint real_program = trc_get_real_gl_program(ctx->trace, p_program);
    if (!real_program) ERROR("Invalid program name");
    
    real(real_program);
    
    trc_gl_program_rev_t rev = *trc_get_gl_program(ctx->trace, p_program);
    
    GLint len;
    F(glGetProgramiv)(real_program, GL_INFO_LOG_LENGTH, &len);
    rev.info_log = trc_create_data(ctx->trace, len+1, NULL, TRC_DATA_NO_ZERO);
    F(glGetProgramInfoLog)(real_program, len, NULL, trc_map_data(rev.info_log, TRC_MAP_REPLACE));
    trc_unmap_freeze_data(ctx->trace, rev.info_log);
    
    trc_set_gl_program(ctx->trace, p_program, &rev);
    
    GLint status;
    F(glGetProgramiv)(real_program, GL_LINK_STATUS, &status);
    if (!status) ERROR("Program validation failed");

glUseProgram: //GLuint p_program
    GLuint real_program = trc_get_real_gl_program(ctx->trace, p_program);
    if (!real_program && p_program) ERROR("Invalid program name");
    
    trc_gl_context_rev_t state = *trc_get_gl_context(ctx->trace, 0);
    trc_grab_gl_obj(ctx->trace, p_program, TrcGLObj_Program);
    trc_rel_gl_obj(ctx->trace, state.bound_program, TrcGLObj_Program);
    state.bound_program = p_program;
    trc_set_gl_context(ctx->trace, 0, &state);
    
    real(real_program);

glGenProgramPipelines: //GLsizei p_n, GLuint* p_pipelines
    GLuint* pipelines = replay_alloc(p_n*sizeof(GLuint));
    real(p_n, pipelines);
    
    trc_gl_program_pipeline_rev_t rev;
    rev.fake_context = trc_get_current_fake_gl_context(ctx->trace);
    rev.ref_count = 1;
    for (size_t i = 0; i < p_n; ++i) {
        rev.real = pipelines[i];
        trc_set_gl_program_pipeline(ctx->trace, p_pipelines[i], &rev);
    }

glDeleteProgramPipelines: //GLsizei p_n, const GLuint* p_pipelines
    GLuint* pipelines = replay_alloc(p_n*sizeof(GLuint));
    for (size_t i = 0; i < p_n; ++i) {
        if (!(pipelines[i] = trc_get_real_gl_program_pipeline(ctx->trace, p_pipelines[i])))
            trc_add_error(cmd, "Invalid program pipeline name");
        else trc_rel_gl_obj(ctx->trace, p_pipelines[i], TrcGLObj_ProgramPipeline);
    }
    real(p_n, pipelines);

glUseProgramStages: //GLuint p_pipeline, GLbitfield p_stages, GLuint p_program
    GLuint real_pipeline = trc_get_real_gl_program_pipeline(ctx->trace, p_pipeline);
    if (!real_pipeline) ERROR("Invalid program pipeline name");
    GLuint real_program = trc_get_real_gl_program(ctx->trace, p_program);
    if (!real_program) ERROR("Invalid program name");
    real(p_pipeline, p_stages, p_program);
    //TODO: Create new pipeline revision

glFlush: //
    real();

glFinish: //
    real();

glIsEnabled: //GLenum p_cap
    ;

glIsEnabledi: //GLenum p_target, GLuint p_index
    ; //TODO: Validation

glIsBuffer: //GLuint p_buffer
    ;

glIsProgram: //GLuint p_program
    ;

glIsQuery: //GLuint p_id
    ;

glIsShader: //GLuint p_shader
    ;

glIsTexture: //GLuint p_texture
    ;

glIsVertexArray: //GLuint p_array
    ;

glIsProgramPipeline: //GLuint p_pipeline
    ;

glIsFramebuffer: //GLuint p_framebuffer
    ;

glIsRenderbuffer: //GLuint p_renderbuffer
    ;

glIsSampler: //GLuint p_sampler
    ;

glIsSync: //GLsync p_sync
    ;

glIsTransformFeedback: //GLuint p_id
    ;

glTransformFeedbackVaryings: //GLuint p_program, GLsizei p_count, const GLchar*const* p_varyings, GLenum p_bufferMode
    if (!p_program) ERROR("Invalid program name");
    real(p_program_rev->real, p_count, p_varyings, p_bufferMode);

glBindAttribLocation: //GLuint p_program, GLuint p_index, const GLchar* p_name
    GLuint real_program = trc_get_real_gl_program(ctx->trace, p_program);
    if (!real_program) ERROR("Invalid program name");
    real(real_program, p_index, p_name);

glGetAttribLocation: //GLuint p_program, const GLchar* p_name
    GLuint real_program = trc_get_real_gl_program(ctx->trace, p_program);
    if (!real_program) ERROR("Invalid program name");
    real(real_program, p_name);

glGetUniformLocation: //GLuint p_program, const GLchar* p_name
    GLuint real_program = trc_get_real_gl_program(ctx->trace, p_program);
    if (!real_program) ERROR("Invalid program name");
    real(real_program, p_name);

glGetShaderiv: //GLuint p_shader, GLenum p_pname, GLint* p_params
    GLuint real_shdr = trc_get_real_gl_shader(ctx->trace, p_shader);
    if (!real_shdr) ERROR("Invalid shader name");

glGetShaderInfoLog: //GLuint p_shader, GLsizei p_bufSize, GLsizei* p_length, GLchar* p_infoLog
    GLuint real_shdr = trc_get_real_gl_shader(ctx->trace, p_shader);
    if (!real_shdr) ERROR("Invalid shader name");

glGetShaderSource: //GLuint p_shader, GLsizei p_bufSize, GLsizei* p_length, GLchar* p_source
    GLuint real_shdr = trc_get_real_gl_shader(ctx->trace, p_shader);
    if (!real_shdr) ERROR("Invalid shader name");

glGetQueryiv: //GLenum p_target, GLenum p_pname, GLint* p_params
    ; //TODO: Validation

glGetQueryObjectiv: //GLuint p_id, GLenum p_pname, GLint* p_params
    GLuint real_query = trc_get_real_gl_query(ctx->trace, p_id);
    if (!real_query) ERROR("Invalid query name");

glGetQueryObjectuiv: //GLuint p_id, GLenum p_pname, GLuint* p_params
    GLuint real_query = trc_get_real_gl_query(ctx->trace, p_id);
    if (!real_query) ERROR("Invalid query name");

glGetQueryObjecti64v: //GLuint p_id, GLenum p_pname, GLint64* p_params
    GLuint real_query = trc_get_real_gl_query(ctx->trace, p_id);
    if (!real_query) ERROR("Invalid query name");

glGetQueryObjectui64v: //GLuint p_id, GLenum p_pname, GLuint64* p_params
    GLuint real_query = trc_get_real_gl_query(ctx->trace, p_id);
    if (!real_query) ERROR("Invalid query name");

glGetProgramInfoLog: //GLuint p_program, GLsizei p_bufSize, GLsizei* p_length, GLchar* p_infoLog
    GLuint real_prog = trc_get_real_gl_program(ctx->trace, p_program);
    if (!real_prog) ERROR("Invalid program name");

glGetProgramiv: //GLuint p_program, GLenum p_pname, GLint* p_params
    GLuint real_prog = trc_get_real_gl_program(ctx->trace, p_program);
    if (!real_prog) ERROR("Invalid program name");

glGetError: //
    ;

//TODO: Validation for these
glGetTexLevelParameterfv: //GLenum p_target, GLint p_level, GLenum p_pname, GLfloat* p_params
    ;

glGetTexLevelParameteriv: //GLenum p_target, GLint p_level, GLenum p_pname, GLint* p_params
    ;

glGetTexParameterfv: //GLenum p_target, GLenum p_pname, GLfloat* p_params
    ;

glGetTexParameteriv: //GLenum p_target, GLenum p_pname, GLint* p_params
    ;

glGetTexParameterIiv: //GLenum p_target, GLenum p_pname, GLint* p_params
    ;

glGetTexParameterIuiv: //GLenum p_target, GLenum p_pname, GLuint* p_params
    ;

glGetTextureLevelParameterfv: //GLuint p_texture, GLint p_level, GLenum p_pname, GLfloat* p_params
    ;

glGetTextureLevelParameteriv: //GLuint p_texture, GLint p_level, GLenum p_pname, GLint* p_params
    ;

glGetTextureParameterfv: //GLuint p_texture, GLenum p_pname, GLfloat* p_params
    ;

glGetTextureParameteriv: //GLuint p_texture, GLenum p_pname, GLint* p_params
    ;

glGetTextureParameterIiv: //GLuint p_texture, GLenum p_pname, GLint* p_params
    ;

glGetTextureParameterIuiv: //GLuint p_texture, GLenum p_pname, GLuint* p_params
    ;

glGetTransformFeedbackVarying: //GLuint p_program, GLuint p_index, GLsizei p_bufSize, GLsizei* p_length GLsizei* p_size, GLenum* p_type, GLchar* p_name
    ;

glCheckFramebufferStatus: //GLenum p_target
    ;

glGetPointerv: //GLenum p_pname, void ** p_params
    ;

glGetPolygonStipple: //GLubyte* p_mask
    ;

glGetMinmax: //GLenum p_target, GLboolean p_reset, GLenum p_format, GLenum p_type, void * p_values
    ;

glGetMinmaxParameterfv: //GLenum p_target, GLenum p_pname, GLfloat* p_params
    ;

glGetMinmaxParameteriv: //GLenum p_target, GLenum p_pname, GLint* p_params
    ;

glGetPixelMapfv: //GLenum p_map, GLfloat* p_values
    ;

glGetPixelMapuiv: //GLenum p_map, GLuint* p_values
    ;

glGetPixelMapusv: //GLenum p_map, GLushort* p_values
    ;

glGetSeparableFilter: //GLenum p_target, GLenum p_format, GLenum p_type, void * p_row, void * p_column, void * p_span
    ;

glGetProgramPipelineiv: //GLuint p_pipeline, GLenum p_pname, GLint  * p_params
    ;

glGetBufferParameteriv: //GLenum p_target, GLenum p_pname, GLint* p_params
    if (!get_bound_buffer(ctx, p_target)) ERROR("No buffer bound to target");

glGetBufferParameteri64v: //GLenum p_target, GLenum p_pname, GLint64* p_params
    if (!get_bound_buffer(ctx, p_target)) ERROR("No buffer bound to target");

glGetNamedBufferParameteriv: //GLuint p_buffer, GLenum p_pname, GLint* p_params
    if (!p_buffer_rev) ERROR("Invalid buffer name");
    if (!p_buffer_rev->has_object) ERROR("Buffer name has no object");

glGetNamedBufferParameteri64v: //GLuint p_buffer, GLenum p_pname, GLint64* p_params
    if (!p_buffer_rev) ERROR("Invalid buffer name");
    if (!p_buffer_rev->has_object) ERROR("Buffer name has no object");

glGetBufferPointerv: //GLenum p_target, GLenum p_pname, void ** p_params
    if (!get_bound_buffer(ctx, p_target)) ERROR("No buffer bound to target");

glGetNamedBufferPointerv: //GLuint p_buffer, GLenum p_pname, void ** p_params
    if (!p_buffer_rev) ERROR("Invalid buffer name");
    if (!p_buffer_rev->has_object) ERROR("Buffer name has no object");

glGetBufferSubData: //GLenum p_target, GLintptr p_offset, GLsizeiptr p_size, void* p_data
    get_buffer_sub_data(ctx, cmd, false, get_bound_buffer(ctx, p_target), p_offset, p_size);

glGetNamedBufferSubData: //GLuint p_buffer, GLintptr p_offset, GLsizeiptr p_size, void* p_data
    get_buffer_sub_data(ctx, cmd, true, p_buffer, p_offset, p_size);

glGetTexImage: //GLenum p_target, GLint p_level, GLenum p_format, GLenum p_type, void * p_pixels
    ;

glGetBooleanv: //GLenum p_pname, GLboolean* p_data
    ;

glGetDoublev: //GLenum p_pname, GLdouble* p_data
    ;

glGetFloatv: //GLenum p_pname, GLfloat* p_data
    ;

glGetIntegerv: //GLenum p_pname, GLint* p_data
    ;

glGetString: //GLenum p_name
    ;

glGetStringi: //GLenum p_name, GLuint p_index
    ;

glGetDoublei_v: //GLenum p_target, GLuint p_index, GLdouble* p_data
    ;

glGetVertexAttribdv: //GLuint p_index, GLenum p_pname, GLdouble* p_params
    ;

glGetVertexAttribfv: //GLuint p_index, GLenum p_pname, GLfloat* p_params
    ;

glGetVertexAttribiv: //GLuint p_index, GLenum p_pname, GLint* p_params
    ;

glGetVertexAttribIiv: //GLuint p_index, GLenum p_pname, GLint* p_params
    ;

glGetVertexAttribIuiv: //GLuint p_index, GLenum p_pname, GLuint* p_params
    ;

glGetVertexAttribLdv: //GLuint p_index, GLenum p_pname, GLdouble* p_params
    ;

glGetVertexAttribPointerv: //GLuint p_index, GLenum p_pname, void ** p_pointer
    ;

glGetCompressedTexImage: //GLenum p_target, GLint p_level, void * p_img
    ;

glGetAttachedShaders: //GLuint p_program, GLsizei p_maxCount, GLsizei* p_count, GLuint* p_shaders
    ;

glGetActiveUniform: //GLuint p_program, GLuint p_index, GLsizei p_bufSize, GLsizei* p_length, GLint* p_size, GLenum* p_type, GLchar* p_name
    if (!trc_get_real_gl_program(ctx->trace, p_program)) ERROR("Invalid program name");

glGetActiveAttrib: //GLuint p_program, GLuint p_index, GLsizei p_bufSize, GLsizei* p_length, GLint* p_size, GLenum* p_type, GLchar* p_name
    if (!trc_get_real_gl_program(ctx->trace, p_program)) ERROR("Invalid program name");

glGetBooleanv: //GLenum p_pname, GLboolean* p_data
    ;

glGetDoublev: //GLenum p_pname, GLdouble* p_data
    ;

glGetFloatv: //GLenum p_pname, GLfloat* p_data
    ;

glGetIntegerv: //GLenum p_pname, GLint* p_data
    ;

glGetInteger64v: //GLenum p_pname, GLint64* p_data
    ;

glGetBooleani_v: //GLenum p_target, GLuint p_index, GLboolean* p_data
    ;

glGetIntegeri_v: //GLenum p_target, GLuint p_index, GLint* p_data
    ;

glGetFloati_v: //GLenum p_target, GLuint p_index, GLfloat* p_data
    ;

glGetDoublev: //GLenum p_pname, GLdouble* p_data
    ;

glGetInteger64i_v: //GLenum p_target, GLuint p_index, GLint64* p_data
    ;

glReadPixels: //GLint p_x, GLint p_y, GLsizei p_width, GLsizei p_height, GLenum p_format, GLenum p_type, void * p_pixels
    ;

glGetSamplerParameterfv: //GLuint p_sampler, GLenum p_pname, GLfloat* p_params
    if (!trc_get_real_gl_sampler(ctx->trace, p_sampler))
        ERROR("Invalid sampler name");

glGetSamplerParameteriv: //GLuint p_sampler, GLenum p_pname, GLint* p_params
    if (!trc_get_real_gl_sampler(ctx->trace, p_sampler))
        ERROR("Invalid sampler name");

glGetSamplerParameterIiv: //GLuint p_sampler, GLenum p_pname, GLint* p_params
    if (!trc_get_real_gl_sampler(ctx->trace, p_sampler))
        ERROR("Invalid sampler name");

glGetSamplerParameterIuiv: //GLuint p_sampler, GLenum p_pname, GLuint* p_params
    if (!trc_get_real_gl_sampler(ctx->trace, p_sampler))
        ERROR("Invalid sampler name");

glUniformBlockBinding: //GLuint p_program, GLuint p_uniformBlockIndex, GLuint p_uniformBlockBinding
    const trc_gl_program_rev_t* rev_ptr = trc_get_gl_program(ctx->trace, p_program);
    if (!rev_ptr) ERROR("Invalid program name");
    trc_gl_program_rev_t rev = *rev_ptr;
    if (p_uniformBlockBinding >= trc_gl_state_get_state_int(ctx->trace, GL_MAX_UNIFORM_BUFFER_BINDINGS, 0))
        ERROR("Invalid binding");
    uint uniform_block_count = rev.uniform_blocks->size / sizeof(trc_gl_program_uniform_block_t);
    trc_gl_program_uniform_block_t* blocks = trc_map_data(rev.uniform_blocks, TRC_MAP_READ);
    for (uint i = 0; i < uniform_block_count; i++) {
        if (blocks[i].fake == p_uniformBlockIndex) {
            real(p_program, blocks[i].real, p_uniformBlockBinding);
            trc_gl_program_rev_t newrev = rev;
            newrev.uniform_blocks = trc_create_data(ctx->trace, rev.uniform_blocks->size, blocks, 0);
            ((trc_gl_program_uniform_block_t*)trc_map_data(newrev.uniform_blocks, TRC_MAP_MODIFY))[i].binding = p_uniformBlockBinding;
            trc_unmap_data(newrev.uniform_blocks);
            trc_set_gl_program(ctx->trace, p_program, &newrev);
            goto success;
        }
    }
    trc_add_error(cmd, "No such uniform block");
    success:
    trc_unmap_data(rev.uniform_blocks);

glUniform1f: //GLint p_location, GLfloat p_v0
    GLint loc;
    if ((loc=uniform(ctx, cmd, false, false, 1, 1, GL_FLOAT, NULL, NULL))<0) RETURN;
    real(loc, p_v0);

glUniform2f: //GLint p_location, GLfloat p_v0, GLfloat p_v1
    GLint loc;
    if ((loc=uniform(ctx, cmd, false, false, 2, 1, GL_FLOAT, NULL, NULL))<0) RETURN;
    real(loc, p_v0, p_v1);

glUniform3f: //GLint p_location, GLfloat p_v0, GLfloat p_v1, GLfloat p_v2
    GLint loc;
    if ((loc=uniform(ctx, cmd, false, false, 3, 1, GL_FLOAT, NULL, NULL))<0) RETURN;
    real(loc, p_v0, p_v1, p_v2);

glUniform4f: //GLint p_location, GLfloat p_v0, GLfloat p_v1, GLfloat p_v2, GLfloat p_v3
    GLint loc;
    if ((loc=uniform(ctx, cmd, false, false, 4, 1, GL_FLOAT, NULL, NULL))<0) RETURN;
    real(loc, p_v0, p_v1, p_v2, p_v3);

glUniform1i: //GLint p_location, GLint p_v0
    GLint loc;
    if ((loc=uniform(ctx, cmd, false, false, 1, 1, GL_INT, NULL, NULL))<0) RETURN;
    real(loc, p_v0);

glUniform2i: //GLint p_location, GLint p_v0, GLint p_v1
    GLint loc;
    if ((loc=uniform(ctx, cmd, false, false, 2, 1, GL_INT, NULL, NULL))<0) RETURN;
    real(loc, p_v0, p_v1);

glUniform3i: //GLint p_location, GLint p_v0, GLint p_v1, GLint p_v2
    GLint loc;
    if ((loc=uniform(ctx, cmd, false, false, 3, 1, GL_INT, NULL, NULL))<0) RETURN;
    real(loc, p_v0, p_v1, p_v2);

glUniform4i: //GLint p_location, GLint p_v0, GLint p_v1, GLint p_v2, GLint p_v3
    GLint loc;
    if ((loc=uniform(ctx, cmd, false, false, 4, 1, GL_INT, NULL, NULL))<0) RETURN;
    real(loc, p_v0, p_v1, p_v2, p_v3);

glUniform1ui: //GLint p_location, GLuint p_v0
    GLint loc;
    if ((loc=uniform(ctx, cmd, false, false, 1, 1, GL_UNSIGNED_INT, NULL, NULL))<0) RETURN;
    real(loc, p_v0);

glUniform2ui: //GLint p_location, GLuint p_v0, GLuint p_v1
    GLint loc;
    if ((loc=uniform(ctx, cmd, false, false, 2, 1, GL_UNSIGNED_INT, NULL, NULL))<0) RETURN;
    real(loc, p_v0, p_v1);

glUniform3ui: //GLint p_location, GLuint p_v0, GLuint p_v1, GLuint p_v2
    GLint loc;
    if ((loc=uniform(ctx, cmd, false, false, 3, 1, GL_UNSIGNED_INT, NULL, NULL))<0) RETURN;
    real(loc, p_v0, p_v1, p_v2);

glUniform4ui: //GLint p_location, GLuint p_v0, GLuint p_v1, GLuint p_v2, GLuint p_v3
    GLint loc;
    if ((loc=uniform(ctx, cmd, false, false, 4, 1, GL_UNSIGNED_INT, NULL, NULL))<0) RETURN;
    real(loc, p_v0, p_v1, p_v2, p_v3);

glUniform1d: //GLint p_location, GLdouble p_x
    GLint loc;
    if ((loc=uniform(ctx, cmd, false, false, 1, 1, GL_DOUBLE, NULL, NULL))<0) RETURN;
    real(loc, p_x);

glUniform2d: //GLint p_location, GLdouble p_x, GLdouble p_y
    GLint loc;
    if ((loc=uniform(ctx, cmd, false, false, 2, 1, GL_DOUBLE, NULL, NULL))<0) RETURN;
    real(loc, p_x, p_y);

glUniform3d: //GLint p_location, GLdouble p_x, GLdouble p_y, GLdouble p_z
    GLint loc;
    if ((loc=uniform(ctx, cmd, false, false, 3, 1, GL_DOUBLE, NULL, NULL))<0) RETURN;
    real(loc, p_x, p_y, p_z);

glUniform4d: //GLint p_location, GLdouble p_x, GLdouble p_y, GLdouble p_z, GLdouble p_w
    GLint loc;
    if ((loc=uniform(ctx, cmd, false, false, 4, 1, GL_DOUBLE, NULL, NULL))<0) RETURN;
    real(loc, p_x, p_y, p_z, p_w);

glUniform1fv: //GLint p_location, GLsizei p_count, const GLfloat* p_value
    GLfloat* values = replay_alloc(p_count*sizeof(GLfloat));
    GLint loc;
    if ((loc=uniform(ctx, cmd, false, true, 1, 1, GL_FLOAT, values, NULL))<0) RETURN;
    real(loc, p_count, values);

glUniform2fv: //GLint p_location, GLsizei p_count, const GLfloat* p_value
    GLfloat* values = replay_alloc(p_count*2*sizeof(GLfloat));
    GLint loc;
    if ((loc=uniform(ctx, cmd, false, true, 2, 1, GL_FLOAT, values, NULL))<0) RETURN;
    real(loc, p_count, values);

glUniform3fv: //GLint p_location, GLsizei p_count, const GLfloat* p_value
    GLfloat* values = replay_alloc(p_count*3*sizeof(GLfloat));
    GLint loc;
    if ((loc=uniform(ctx, cmd, false, true, 3, 1, GL_FLOAT, values, NULL))<0) RETURN;
    real(loc, p_count, values);

glUniform4fv: //GLint p_location, GLsizei p_count, const GLfloat* p_value
    GLfloat* values = replay_alloc(p_count*4*sizeof(GLfloat));
    GLint loc;
    if ((loc=uniform(ctx, cmd, false, true, 4, 1, GL_FLOAT, values, NULL))<0) RETURN;
    real(loc, p_count, values);

glUniform1iv: //GLint p_location, GLsizei p_count, const GLint* p_value
    GLint* values = replay_alloc(p_count*sizeof(GLint));
    GLint loc;
    if ((loc=uniform(ctx, cmd, false, true, 1, 1, GL_INT, values, NULL))<0) RETURN;
    real(loc, p_count, values);

glUniform2iv: //GLint p_location, GLsizei p_count, const GLint* p_value
    GLint* values = replay_alloc(p_count*2*sizeof(GLint));
    GLint loc;
    if ((loc=uniform(ctx, cmd, false, true, 2, 1, GL_INT, values, NULL))<0) RETURN;
    real(loc, p_count, values);

glUniform3iv: //GLint p_location, GLsizei p_count, const GLint* p_value
    GLint* values = replay_alloc(p_count*3*sizeof(GLint));
    GLint loc;
    if ((loc=uniform(ctx, cmd, false, true, 3, 1, GL_INT, values, NULL))<0) RETURN;
    real(loc, p_count, values);

glUniform4iv: //GLint p_location, GLsizei p_count, const GLint* p_value
    GLint* values = replay_alloc(p_count*4*sizeof(GLint));
    GLint loc;
    if ((loc=uniform(ctx, cmd, false, true, 4, 1, GL_INT, values, NULL))<0) RETURN;
    real(loc, p_count, values);

glUniform1uiv: //GLint p_location, GLsizei p_count, const GLuint* p_value
    GLuint* values = replay_alloc(p_count*sizeof(GLuint));
    GLint loc;
    if ((loc=uniform(ctx, cmd, false, true, 1, 1, GL_UNSIGNED_INT, values, NULL))<0) RETURN;
    real(loc, p_count, values);

glUniform2uiv: //GLint p_location, GLsizei p_count, const GLuint* p_value
    GLuint* values = replay_alloc(p_count*2*sizeof(GLuint));
    GLint loc;
    if ((loc=uniform(ctx, cmd, false, true, 2, 1, GL_UNSIGNED_INT, values, NULL))<0) RETURN;
    real(loc, p_count, values);

glUniform3uiv: //GLint p_location, GLsizei p_count, const GLuint* p_value
    GLuint* values = replay_alloc(p_count*3*sizeof(GLuint));
    GLint loc;
    if ((loc=uniform(ctx, cmd, false, true, 3, 1, GL_UNSIGNED_INT, values, NULL))<0) RETURN;
    real(loc, p_count, values);

glUniform4uiv: //GLint p_location, GLsizei p_count, const GLuint* p_value
    GLuint* values = replay_alloc(p_count*4*sizeof(GLuint));
    GLint loc;
    if ((loc=uniform(ctx, cmd, false, true, 4, 1, GL_UNSIGNED_INT, values, NULL))<0) RETURN;
    real(loc, p_count, values);

glUniform1dv: //GLint p_location, GLsizei p_count, const GLdouble* p_value
    GLdouble* values = replay_alloc(p_count*sizeof(GLdouble));
    GLint loc;
    if ((loc=uniform(ctx, cmd, false, true, 1, 1, GL_DOUBLE, values, NULL))<0) RETURN;
    real(loc, p_count, values);

glUniform2dv: //GLint p_location, GLsizei p_count, const GLdouble* p_value
    GLdouble* values = replay_alloc(p_count*2*sizeof(GLdouble));
    GLint loc;
    if ((loc=uniform(ctx, cmd, false, true, 2, 1, GL_DOUBLE, values, NULL))<0) RETURN;
    real(loc, p_count, values);

glUniform3dv: //GLint p_location, GLsizei p_count, const GLdouble* p_value
    GLdouble* values = replay_alloc(p_count*3*sizeof(GLdouble));
    GLint loc;
    if ((loc=uniform(ctx, cmd, false, true, 3, 1, GL_UNSIGNED_INT, values, NULL))<0) RETURN;
    real(loc, p_count, values);

glUniform4dv: //GLint p_location, GLsizei p_count, const GLdouble* p_value
    GLdouble* values = replay_alloc(p_count*4*sizeof(GLdouble));
    GLint loc;
    if ((loc=uniform(ctx, cmd, false, true, 4, 1, GL_UNSIGNED_INT, values, NULL))<0) RETURN;
    real(loc, p_count, values);

glUniformMatrix2fv: //GLint p_location, GLsizei p_count, GLboolean p_transpose, const GLfloat* p_value
    GLfloat* values = replay_alloc(p_count*4*sizeof(GLfloat));
    GLint loc;
    if ((loc=uniform(ctx, cmd, false, true, 2, 2, GL_FLOAT, values, NULL))<0) RETURN;
    real(loc, p_count, p_transpose, values);

glUniformMatrix3fv: //GLint p_location, GLsizei p_count, GLboolean p_transpose, const GLfloat* p_value
    GLfloat* values = replay_alloc(p_count*9*sizeof(GLfloat));
    GLint loc;
    if ((loc=uniform(ctx, cmd, false, true, 3, 3, GL_FLOAT, values, NULL))<0) RETURN;
    real(loc, p_count, p_transpose, values);

glUniformMatrix4fv: //GLint p_location, GLsizei p_count, GLboolean p_transpose, const GLfloat* p_value
    GLfloat* values = replay_alloc(p_count*16*sizeof(GLfloat));
    GLint loc;
    if ((loc=uniform(ctx, cmd, false, true, 4, 4, GL_FLOAT, values, NULL))<0) RETURN;
    real(loc, p_count, p_transpose, values);

glUniformMatrix2x3fv: //GLint p_location, GLsizei p_count, GLboolean p_transpose, const GLfloat* p_value
    GLfloat* values = replay_alloc(p_count*6*sizeof(GLfloat));
    GLint loc;
    if ((loc=uniform(ctx, cmd, false, true, 2, 3, GL_FLOAT, values, NULL))<0) RETURN;
    real(loc, p_count, p_transpose, values);

glUniformMatrix3x2fv: //GLint p_location, GLsizei p_count, GLboolean p_transpose, const GLfloat* p_value
    GLfloat* values = replay_alloc(p_count*6*sizeof(GLfloat));
    GLint loc;
    if ((loc=uniform(ctx, cmd, false, true, 3, 2, GL_FLOAT, values, NULL))<0) RETURN;
    real(loc, p_count, p_transpose, values);

glUniformMatrix2x4fv: //GLint p_location, GLsizei p_count, GLboolean p_transpose, const GLfloat* p_value
    GLfloat* values = replay_alloc(p_count*8*sizeof(GLfloat));
    GLint loc;
    if ((loc=uniform(ctx, cmd, false, true, 2, 4, GL_FLOAT, values, NULL))<0) RETURN;
    real(loc, p_count, p_transpose, values);

glUniformMatrix4x2fv: //GLint p_location, GLsizei p_count, GLboolean p_transpose, const GLfloat* p_value
    GLfloat* values = replay_alloc(p_count*8*sizeof(GLfloat));
    GLint loc;
    if ((loc=uniform(ctx, cmd, false, true, 4, 2, GL_FLOAT, values, NULL))<0) RETURN;
    real(loc, p_count, p_transpose, values);

glUniformMatrix3x4fv: //GLint p_location, GLsizei p_count, GLboolean p_transpose, const GLfloat* p_value
    GLfloat* values = replay_alloc(p_count*12*sizeof(GLfloat));
    GLint loc;
    if ((loc=uniform(ctx, cmd, false, true, 3, 4, GL_FLOAT, values, NULL))<0) RETURN;
    real(loc, p_count, p_transpose, values);

glUniformMatrix4x3fv: //GLint p_location, GLsizei p_count, GLboolean p_transpose, const GLfloat* p_value
    GLfloat* values = replay_alloc(p_count*12*sizeof(GLfloat));
    GLint loc;
    if ((loc=uniform(ctx, cmd, false, true, 4, 3, GL_FLOAT, values, NULL))<0) RETURN;
    real(loc, p_count, p_transpose, values);

glUniformMatrix2dv: //GLint p_location, GLsizei p_count, GLboolean p_transpose, const GLdouble* p_value
    GLdouble* values = replay_alloc(p_count*4*sizeof(GLdouble));
    GLint loc;
    if ((loc=uniform(ctx, cmd, false, true, 2, 2, GL_DOUBLE, values, NULL))<0) RETURN;
    real(loc, p_count, p_transpose, values);

glUniformMatrix3dv: //GLint p_location, GLsizei p_count, GLboolean p_transpose, const GLdouble* p_value
    GLdouble* values = replay_alloc(p_count*9*sizeof(GLdouble));
    GLint loc;
    if ((loc=uniform(ctx, cmd, false, true, 3, 3, GL_DOUBLE, values, NULL))<0) RETURN;
    real(loc, p_count, p_transpose, values);

glUniformMatrix4dv: //GLint p_location, GLsizei p_count, GLboolean p_transpose, const GLdouble* p_value
    GLdouble* values = replay_alloc(p_count*16*sizeof(GLdouble));
    GLint loc;
    if ((loc=uniform(ctx, cmd, false, true, 4, 4, GL_DOUBLE, values, NULL))<0) RETURN;
    real(loc, p_count, p_transpose, values);

glUniformMatrix2x3dv: //GLint p_location, GLsizei p_count, GLboolean p_transpose, const GLdouble* p_value
    GLdouble* values = replay_alloc(p_count*6*sizeof(GLdouble));
    GLint loc;
    if ((loc=uniform(ctx, cmd, false, true, 2, 3, GL_DOUBLE, values, NULL))<0) RETURN;
    real(loc, p_count, p_transpose, values);

glUniformMatrix3x2dv: //GLint p_location, GLsizei p_count, GLboolean p_transpose, const GLdouble* p_value
    GLdouble* values = replay_alloc(p_count*6*sizeof(GLdouble));
    GLint loc;
    if ((loc=uniform(ctx, cmd, false, true, 3, 2, GL_DOUBLE, values, NULL))<0) RETURN;
    real(loc, p_count, p_transpose, values);

glUniformMatrix2x4dv: //GLint p_location, GLsizei p_count, GLboolean p_transpose, const GLdouble* p_value
    GLdouble* values = replay_alloc(p_count*8*sizeof(GLdouble));
    GLint loc;
    if ((loc=uniform(ctx, cmd, false, true, 2, 4, GL_DOUBLE, values, NULL))<0) RETURN;
    real(loc, p_count, p_transpose, values);

glUniformMatrix4x2dv: //GLint p_location, GLsizei p_count, GLboolean p_transpose, const GLdouble* p_value
    GLdouble* values = replay_alloc(p_count*8*sizeof(GLdouble));
    GLint loc;
    if ((loc=uniform(ctx, cmd, false, true, 4, 2, GL_DOUBLE, values, NULL))<0) RETURN;
    real(loc, p_count, p_transpose, values);

glUniformMatrix3x4dv: //GLint p_location, GLsizei p_count, GLboolean p_transpose, const GLdouble* p_value
    GLdouble* values = replay_alloc(p_count*12*sizeof(GLdouble));
    GLint loc;
    if ((loc=uniform(ctx, cmd, false, true, 3, 4, GL_DOUBLE, values, NULL))<0) RETURN;
    real(loc, p_count, p_transpose, values);

glUniformMatrix4x3dv: //GLint p_location, GLsizei p_count, GLboolean p_transpose, const GLdouble* p_value
    GLdouble* values = replay_alloc(p_count*12*sizeof(GLdouble));
    GLint loc;
    if ((loc=uniform(ctx, cmd, false, true, 4, 3, GL_DOUBLE, values, NULL))<0) RETURN;
    real(loc, p_count, p_transpose, values);

glProgramUniform1f: //GLuint p_program, GLint p_location, GLfloat p_v0
    GLint loc; GLuint program;
    if ((loc=uniform(ctx, cmd, true, false, 1, 1, GL_FLOAT, NULL, &program))<0) RETURN;
    real(program, loc, p_v0);

glProgramUniform2f: //GLuint p_program, GLint p_location, GLfloat p_v0, GLfloat p_v1
    GLint loc; GLuint program;
    if ((loc=uniform(ctx, cmd, true, false, 2, 1, GL_FLOAT, NULL, &program))<0) RETURN;
    real(program, loc, p_v0, p_v1);

glProgramUniform3f: //GLuint p_program, GLint p_location, GLfloat p_v0, GLfloat p_v1, GLfloat p_v2
    GLint loc; GLuint program;
    if ((loc=uniform(ctx, cmd, true, false, 3, 1, GL_FLOAT, NULL, &program))<0) RETURN;
    real(program, loc, p_v0, p_v1, p_v2);

glProgramUniform4f: //GLuint p_program, GLint p_location, GLfloat p_v0, GLfloat p_v1, GLfloat p_v2, GLfloat p_v3
    GLint loc; GLuint program;
    if ((loc=uniform(ctx, cmd, true, false, 4, 1, GL_FLOAT, NULL, &program))<0) RETURN;
    real(program, loc, p_v0, p_v1, p_v2, p_v3);

glProgramUniform1i: //GLuint p_program, GLint p_location, GLint p_v0
    GLint loc; GLuint program;
    if ((loc=uniform(ctx, cmd, true, false, 1, 1, GL_INT, NULL, &program))<0) RETURN;
    real(program, loc, p_v0);

glProgramUniform2i: //GLuint p_program, GLint p_location, GLint p_v0, GLint p_v1
    GLint loc; GLuint program;
    if ((loc=uniform(ctx, cmd, true, false, 2, 1, GL_INT, NULL, &program))<0) RETURN;
    real(program, loc, p_v0, p_v1);

glProgramUniform3i: //GLuint p_program, GLint p_location, GLint p_v0, GLint p_v1, GLint p_v2
    GLint loc; GLuint program;
    if ((loc=uniform(ctx, cmd, true, false, 3, 1, GL_INT, NULL, &program))<0) RETURN;
    real(program, loc, p_v0, p_v1, p_v2);

glProgramUniform4i: //GLuint p_program, GLint p_location, GLint p_v0, GLint p_v1, GLint p_v2, GLint p_v3
    GLint loc; GLuint program;
    if ((loc=uniform(ctx, cmd, true, false, 4, 1, GL_INT, NULL, &program))<0) RETURN;
    real(program, loc, p_v0, p_v1, p_v2, p_v3);

glProgramUniform1ui: //GLuint p_program, GLint p_location, GLuint p_v0
    GLint loc; GLuint program;
    if ((loc=uniform(ctx, cmd, true, false, 1, 1, GL_UNSIGNED_INT, NULL, &program))<0) RETURN;
    real(program, loc, p_v0);

glProgramUniform2ui: //GLuint p_program, GLint p_location, GLuint p_v0, GLuint p_v1
    GLint loc; GLuint program;
    if ((loc=uniform(ctx, cmd, true, false, 2, 1, GL_UNSIGNED_INT, NULL, &program))<0) RETURN;
    real(program, loc, p_v0, p_v1);

glProgramUniform3ui: //GLuint p_program, GLint p_location, GLuint p_v0, GLuint p_v1, GLuint p_v2
    GLint loc; GLuint program;
    if ((loc=uniform(ctx, cmd, true, false, 3, 1, GL_UNSIGNED_INT, NULL, &program))<0) RETURN;
    real(program, loc, p_v0, p_v1, p_v2);

glProgramUniform4ui: //GLuint p_program, GLint p_location, GLuint p_v0, GLuint p_v1, GLuint p_v2, GLuint p_v3
    GLint loc; GLuint program;
    if ((loc=uniform(ctx, cmd, true, false, 4, 1, GL_UNSIGNED_INT, NULL, &program))<0) RETURN;
    real(program, loc, p_v0, p_v1, p_v2, p_v3);

glProgramUniform1d: //GLuint p_program, GLint p_location, GLdouble p_v0
    GLint loc; GLuint program;
    if ((loc=uniform(ctx, cmd, true, false, 1, 1, GL_DOUBLE, NULL, &program))<0) RETURN;
    real(program, loc, p_v0);

glProgramUniform2d: //GLuint p_program, GLint p_location, GLdouble p_v0, GLdouble p_v1
    GLint loc; GLuint program;
    if ((loc=uniform(ctx, cmd, true, false, 2, 1, GL_DOUBLE, NULL, &program))<0) RETURN;
    real(program, loc, p_v0, p_v1);

glProgramUniform3d: //GLuint p_program, GLint p_location, GLdouble p_v0, GLdouble p_v1, GLdouble p_v2
    GLint loc; GLuint program;
    if ((loc=uniform(ctx, cmd, true, false, 3, 1, GL_DOUBLE, NULL, &program))<0) RETURN;
    real(program, loc, p_v0, p_v1, p_v2);

glProgramUniform4d: //GLuint p_program, GLint p_location, GLdouble p_v0, GLdouble p_v1, GLdouble p_v2, GLdouble p_v3
    GLint loc; GLuint program;
    if ((loc=uniform(ctx, cmd, true, false, 4, 1, GL_DOUBLE, NULL, &program))<0) RETURN;
    real(program, loc, p_v0, p_v1, p_v2, p_v3);

glProgramUniform1fv: //GLuint p_program, GLint p_location, GLsizei p_count, const GLfloat* p_value
    GLfloat* values = replay_alloc(p_count*sizeof(GLfloat));
    GLint loc; GLuint program;
    if ((loc=uniform(ctx, cmd, true, true, 1, 1, GL_FLOAT, values, &program))<0) RETURN;
    real(program, loc, p_count, values);

glProgramUniform2fv: //GLuint p_program, GLint p_location, GLsizei p_count, const GLfloat* p_value
    GLfloat* values = replay_alloc(p_count*2*sizeof(GLfloat));
    GLint loc; GLuint program;
    if ((loc=uniform(ctx, cmd, true, true, 2, 1, GL_FLOAT, values, &program))<0) RETURN;
    real(program, loc, p_count, values);

glProgramUniform3fv: //GLuint p_program, GLint p_location, GLsizei p_count, const GLfloat* p_value
    GLfloat* values = replay_alloc(p_count*3*sizeof(GLfloat));
    GLint loc; GLuint program;
    if ((loc=uniform(ctx, cmd, true, true, 3, 1, GL_FLOAT, values, &program))<0) RETURN;
    real(program, loc, p_count, values);

glProgramUniform4fv: //GLuint p_program, GLint p_location, GLsizei p_count, const GLfloat* p_value
    GLfloat* values = replay_alloc(p_count*4*sizeof(GLfloat));
    GLint loc; GLuint program;
    if ((loc=uniform(ctx, cmd, true, true, 4, 1, GL_FLOAT, values, &program))<0) RETURN;
    real(program, loc, p_count, values);

glProgramUniform1iv: //GLuint p_program, GLint p_location, GLsizei p_count, const GLint* p_value
    GLint* values = replay_alloc(p_count*sizeof(GLint));
    GLint loc; GLuint program;
    if ((loc=uniform(ctx, cmd, true, true, 1, 1, GL_INT, values, &program))<0) RETURN;
    real(program, loc, p_count, values);

glProgramUniform2iv: //GLuint p_program, GLint p_location, GLsizei p_count, const GLint* p_value
    GLint* values = replay_alloc(p_count*2*sizeof(GLint));
    GLint loc; GLuint program;
    if ((loc=uniform(ctx, cmd, true, true, 2, 1, GL_INT, values, &program))<0) RETURN;
    real(program, loc, p_count, values);

glProgramUniform3iv: //GLuint p_program, GLint p_location, GLsizei p_count, const GLint* p_value
    GLint* values = replay_alloc(p_count*3*sizeof(GLint));
    GLint loc; GLuint program;
    if ((loc=uniform(ctx, cmd, true, true, 3, 1, GL_INT, values, &program))<0) RETURN;
    real(program, loc, p_count, values);

glProgramUniform4iv: //GLuint p_program, GLint p_location, GLsizei p_count, const GLint* p_value
    GLint* values = replay_alloc(p_count*4*sizeof(GLint));
    GLint loc; GLuint program;
    if ((loc=uniform(ctx, cmd, true, true, 4, 1, GL_INT, values, &program))<0) RETURN;
    real(program, loc, p_count, values);

glProgramUniform1uiv: //GLuint p_program, GLint p_location, GLsizei p_count, const GLuint* p_value
    GLuint* values = replay_alloc(p_count*sizeof(GLuint));
    GLint loc; GLuint program;
    if ((loc=uniform(ctx, cmd, true, true, 1, 1, GL_UNSIGNED_INT, values, &program))<0) RETURN;
    real(program, loc, p_count, values);

glProgramUniform2uiv: //GLuint p_program, GLint p_location, GLsizei p_count, const GLuint* p_value
    GLuint* values = replay_alloc(p_count*2*sizeof(GLuint));
    GLint loc; GLuint program;
    if ((loc=uniform(ctx, cmd, true, true, 2, 1, GL_UNSIGNED_INT, values, &program))<0) RETURN;
    real(program, loc, p_count, values);

glProgramUniform3uiv: //GLuint p_program, GLint p_location, GLsizei p_count, const GLuint* p_value
    GLuint* values = replay_alloc(p_count*3*sizeof(GLuint));
    GLint loc; GLuint program;
    if ((loc=uniform(ctx, cmd, true, true, 3, 1, GL_UNSIGNED_INT, values, &program))<0) RETURN;
    real(program, loc, p_count, values);

glProgramUniform4uiv: //GLuint p_program, GLint p_location, GLsizei p_count, const GLuint* p_value
    GLuint* values = replay_alloc(p_count*4*sizeof(GLuint));
    GLint loc; GLuint program;
    if ((loc=uniform(ctx, cmd, true, true, 4, 1, GL_UNSIGNED_INT, values, &program))<0) RETURN;
    real(program, loc, p_count, values);

glProgramUniform1dv: //GLuint p_program, GLint p_location, GLsizei p_count, const GLdouble* p_value
    GLdouble* values = replay_alloc(p_count*sizeof(GLdouble));
    GLint loc; GLuint program;
    if ((loc=uniform(ctx, cmd, true, true, 1, 1, GL_DOUBLE, values, &program))<0) RETURN;
    real(program, loc, p_count, values);

glProgramUniform2dv: //GLuint p_program, GLint p_location, GLsizei p_count, const GLdouble* p_value
    GLdouble* values = replay_alloc(p_count*2*sizeof(GLdouble));
    GLint loc; GLuint program;
    if ((loc=uniform(ctx, cmd, true, true, 2, 1, GL_DOUBLE, values, &program))<0) RETURN;
    real(program, loc, p_count, values);

glProgramUniform3dv: //GLuint p_program, GLint p_location, GLsizei p_count, const GLdouble* p_value
    GLdouble* values = replay_alloc(p_count*3*sizeof(GLdouble));
    GLint loc; GLuint program;
    if ((loc=uniform(ctx, cmd, true, true, 3, 1, GL_UNSIGNED_INT, values, &program))<0) RETURN;
    real(program, loc, p_count, values);

glProgramUniform4dv: //GLuint p_program, GLint p_location, GLsizei p_count, const GLdouble* p_value
    GLdouble* values = replay_alloc(p_count*4*sizeof(GLdouble));
    GLint loc; GLuint program;
    if ((loc=uniform(ctx, cmd, true, true, 4, 1, GL_UNSIGNED_INT, values, &program))<0) RETURN;
    real(program, loc, p_count, values);

glProgramUniformMatrix2fv: //GLuint p_program, GLint p_location, GLsizei p_count, GLboolean p_transpose, const GLfloat* p_value
    GLfloat* values = replay_alloc(p_count*4*sizeof(GLfloat));
    GLint loc; GLuint program;
    if ((loc=uniform(ctx, cmd, true, true, 2, 2, GL_FLOAT, values, &program))<0) RETURN;
    real(program, loc, p_count, p_transpose, values);

glProgramUniformMatrix3fv: //GLuint p_program, GLint p_location, GLsizei p_count, GLboolean p_transpose, const GLfloat* p_value
    GLfloat* values = replay_alloc(p_count*9*sizeof(GLfloat));
    GLint loc; GLuint program;
    if ((loc=uniform(ctx, cmd, true, true, 3, 3, GL_FLOAT, values, &program))<0) RETURN;
    real(program, loc, p_count, p_transpose, values);

glProgramUniformMatrix4fv: //GLuint p_program, GLint p_location, GLsizei p_count, GLboolean p_transpose, const GLfloat* p_value
    GLfloat* values = replay_alloc(p_count*16*sizeof(GLfloat));
    GLint loc; GLuint program;
    if ((loc=uniform(ctx, cmd, true, true, 4, 4, GL_FLOAT, values, &program))<0) RETURN;
    real(program, loc, p_count, p_transpose, values);

glProgramUniformMatrix2x3fv: //GLuint p_program, GLint p_location, GLsizei p_count, GLboolean p_transpose, const GLfloat* p_value
    GLfloat* values = replay_alloc(p_count*6*sizeof(GLfloat));
    GLint loc; GLuint program;
    if ((loc=uniform(ctx, cmd, true, true, 2, 3, GL_FLOAT, values, &program))<0) RETURN;
    real(program, loc, p_count, p_transpose, values);

glProgramUniformMatrix3x2fv: //GLuint p_program, GLint p_location, GLsizei p_count, GLboolean p_transpose, const GLfloat* p_value
    GLfloat* values = replay_alloc(p_count*6*sizeof(GLfloat));
    GLint loc; GLuint program;
    if ((loc=uniform(ctx, cmd, true, true, 3, 2, GL_FLOAT, values, &program))<0) RETURN;
    real(program, loc, p_count, p_transpose, values);

glProgramUniformMatrix2x4fv: //GLuint p_program, GLint p_location, GLsizei p_count, GLboolean p_transpose, const GLfloat* p_value
    GLfloat* values = replay_alloc(p_count*8*sizeof(GLfloat));
    GLint loc; GLuint program;
    if ((loc=uniform(ctx, cmd, true, true, 2, 4, GL_FLOAT, values, &program))<0) RETURN;
    real(program, loc, p_count, p_transpose, values);

glProgramUniformMatrix4x2fv: //GLuint p_program, GLint p_location, GLsizei p_count, GLboolean p_transpose, const GLfloat* p_value
    GLfloat* values = replay_alloc(p_count*8*sizeof(GLfloat));
    GLint loc; GLuint program;
    if ((loc=uniform(ctx, cmd, true, true, 4, 2, GL_FLOAT, values, &program))<0) RETURN;
    real(program, loc, p_count, p_transpose, values);

glProgramUniformMatrix3x4fv: //GLuint p_program, GLint p_location, GLsizei p_count, GLboolean p_transpose, const GLfloat* p_value
    GLfloat* values = replay_alloc(p_count*12*sizeof(GLfloat));
    GLint loc; GLuint program;
    if ((loc=uniform(ctx, cmd, true, true, 3, 4, GL_FLOAT, values, &program))<0) RETURN;
    real(program, loc, p_count, p_transpose, values);

glProgramUniformMatrix4x3fv: //GLuint p_program, GLint p_location, GLsizei p_count, GLboolean p_transpose, const GLfloat* p_value
    GLfloat* values = replay_alloc(p_count*12*sizeof(GLfloat));
    GLint loc; GLuint program;
    if ((loc=uniform(ctx, cmd, true, true, 4, 3, GL_FLOAT, values, &program))<0) RETURN;
    real(program, loc, p_count, p_transpose, values);

glProgramUniformMatrix2dv: //GLuint p_program, GLint p_location, GLsizei p_count, GLboolean p_transpose, const GLdouble* p_value
    GLdouble* values = replay_alloc(p_count*4*sizeof(GLdouble));
    GLint loc; GLuint program;
    if ((loc=uniform(ctx, cmd, true, true, 2, 2, GL_DOUBLE, values, &program))<0) RETURN;
    real(program, loc, p_count, p_transpose, values);

glProgramUniformMatrix3dv: //GLuint p_program, GLint p_location, GLsizei p_count, GLboolean p_transpose, const GLdouble* p_value
    GLdouble* values = replay_alloc(p_count*9*sizeof(GLdouble));
    GLint loc; GLuint program;
    if ((loc=uniform(ctx, cmd, true, true, 3, 3, GL_DOUBLE, values, &program))<0) RETURN;
    real(program, loc, p_count, p_transpose, values);

glProgramUniformMatrix4dv: //GLuint p_program, GLint p_location, GLsizei p_count, GLboolean p_transpose, const GLdouble* p_value
    GLdouble* values = replay_alloc(p_count*16*sizeof(GLdouble));
    GLint loc; GLuint program;
    if ((loc=uniform(ctx, cmd, true, true, 4, 4, GL_DOUBLE, values, &program))<0) RETURN;
    real(program, loc, p_count, p_transpose, values);

glProgramUniformMatrix2x3dv: //GLuint p_program, GLint p_location, GLsizei p_count, GLboolean p_transpose, const GLdouble* p_value
    GLdouble* values = replay_alloc(p_count*6*sizeof(GLdouble));
    GLint loc; GLuint program;
    if ((loc=uniform(ctx, cmd, true, true, 2, 3, GL_DOUBLE, values, &program))<0) RETURN;
    real(program, loc, p_count, p_transpose, values);

glProgramUniformMatrix3x2dv: //GLuint p_program, GLint p_location, GLsizei p_count, GLboolean p_transpose, const GLdouble* p_value
    GLdouble* values = replay_alloc(p_count*6*sizeof(GLdouble));
    GLint loc; GLuint program;
    if ((loc=uniform(ctx, cmd, true, true, 3, 2, GL_DOUBLE, values, &program))<0) RETURN;
    real(program, loc, p_count, p_transpose, values);

glProgramUniformMatrix2x4dv: //GLuint p_program, GLint p_location, GLsizei p_count, GLboolean p_transpose, const GLdouble* p_value
    GLdouble* values = replay_alloc(p_count*8*sizeof(GLdouble));
    GLint loc; GLuint program;
    if ((loc=uniform(ctx, cmd, true, true, 2, 4, GL_DOUBLE, values, &program))<0) RETURN;
    real(program, loc, p_count, p_transpose, values);

glProgramUniformMatrix4x2dv: //GLuint p_program, GLint p_location, GLsizei p_count, GLboolean p_transpose, const GLdouble* p_value
    GLdouble* values = replay_alloc(p_count*8*sizeof(GLdouble));
    GLint loc; GLuint program;
    if ((loc=uniform(ctx, cmd, true, true, 4, 2, GL_DOUBLE, values, &program))<0) RETURN;
    real(program, loc, p_count, p_transpose, values);

glProgramUniformMatrix3x4dv: //GLuint p_program, GLint p_location, GLsizei p_count, GLboolean p_transpose, const GLdouble* p_value
    GLdouble* values = replay_alloc(p_count*12*sizeof(GLdouble));
    GLint loc; GLuint program;
    if ((loc=uniform(ctx, cmd, true, true, 3, 4, GL_DOUBLE, values, &program))<0) RETURN;
    real(program, loc, p_count, p_transpose, values);

glProgramUniformMatrix4x3dv: //GLuint p_program, GLint p_location, GLsizei p_count, GLboolean p_transpose, const GLdouble* p_value
    GLdouble* values = replay_alloc(p_count*12*sizeof(GLdouble));
    GLint loc; GLuint program;
    if ((loc=uniform(ctx, cmd, true, true, 4, 3, GL_DOUBLE, values, &program))<0) RETURN;
    real(program, loc, p_count, p_transpose, values);

//TODO: There is some duplicate code among glVertexAttrib*Pointer and gl*VertexAttribArray
glVertexAttribPointer: //GLuint p_index, GLint p_size, GLenum p_type, GLboolean p_normalized, GLsizei p_stride, const void* p_pointer
    //if (p_pointer > UINTPTR_MAX) //TODO
    if (trc_gl_state_get_bound_vao(ctx->trace) == 0) RETURN;
    trc_gl_vao_rev_t rev = *trc_get_gl_vao(ctx->trace, trc_gl_state_get_bound_vao(ctx->trace));
    if (p_index < rev.attribs->size/sizeof(trc_gl_vao_attrib_t)) {
        trc_data_t* newattribs = trc_create_data(ctx->trace, rev.attribs->size, trc_map_data(rev.attribs, TRC_MAP_READ), 0);
        trc_unmap_data(rev.attribs);
        trc_gl_vao_attrib_t* a = &((trc_gl_vao_attrib_t*)trc_map_data(newattribs, TRC_MAP_MODIFY))[p_index];
        a->normalized = p_normalized;
        a->integer = false;
        a->size = p_size;
        a->stride = p_stride;
        a->offset = p_pointer;
        a->type = p_type;
        a->buffer = trc_gl_state_get_bound_buffer(ctx->trace, GL_ARRAY_BUFFER);
        trc_unmap_freeze_data(ctx->trace, newattribs);
        rev.attribs = newattribs;
        trc_set_gl_vao(ctx->trace, trc_gl_state_get_bound_vao(ctx->trace), &rev);
    }

glVertexAttribIPointer: //GLuint p_index, GLint p_size, GLenum p_type, GLsizei p_stride, const void* p_pointer
    //if (p_pointer > UINTPTR_MAX) //TODO
    if (trc_gl_state_get_bound_vao(ctx->trace) == 0) RETURN;
    trc_gl_vao_rev_t rev = *trc_get_gl_vao(ctx->trace, trc_gl_state_get_bound_vao(ctx->trace));
    if (p_index < rev.attribs->size/sizeof(trc_gl_vao_attrib_t)) {
        trc_data_t* newattribs = trc_create_data(ctx->trace, rev.attribs->size, trc_map_data(rev.attribs, TRC_MAP_READ), 0);
        trc_unmap_data(rev.attribs);
        trc_gl_vao_attrib_t* a = &((trc_gl_vao_attrib_t*)trc_map_data(newattribs, TRC_MAP_MODIFY))[p_index];
        a->integer = true;
        a->size = p_size;
        a->stride = p_stride;
        a->offset = p_pointer;
        a->type = p_type;
        a->buffer = trc_gl_state_get_bound_buffer(ctx->trace, GL_ARRAY_BUFFER);
        trc_unmap_freeze_data(ctx->trace, newattribs);
        rev.attribs = newattribs;
        trc_set_gl_vao(ctx->trace, trc_gl_state_get_bound_vao(ctx->trace), &rev);
    }
    trc_set_gl_vao(ctx->trace, trc_gl_state_get_bound_vao(ctx->trace), &rev);

glEnableVertexAttribArray: //GLuint p_index
    if (trc_gl_state_get_bound_vao(ctx->trace) == 0) RETURN;
    trc_gl_vao_rev_t rev = *trc_get_gl_vao(ctx->trace, trc_gl_state_get_bound_vao(ctx->trace));
    if (p_index < rev.attribs->size/sizeof(trc_gl_vao_attrib_t)) {
        trc_data_t* newattribs = trc_create_data(ctx->trace, rev.attribs->size, trc_map_data(rev.attribs, TRC_MAP_READ), 0);
        trc_unmap_data(rev.attribs);
        trc_gl_vao_attrib_t* a = &((trc_gl_vao_attrib_t*)trc_map_data(newattribs, TRC_MAP_MODIFY))[p_index];
        a->enabled = true;
        trc_unmap_freeze_data(ctx->trace, newattribs);
        rev.attribs = newattribs;
        trc_set_gl_vao(ctx->trace, trc_gl_state_get_bound_vao(ctx->trace), &rev);
    }
    trc_set_gl_vao(ctx->trace, trc_gl_state_get_bound_vao(ctx->trace), &rev);

glDisableVertexAttribArray: //GLuint p_index
    if (trc_gl_state_get_bound_vao(ctx->trace) == 0) RETURN;
    trc_gl_vao_rev_t rev = *trc_get_gl_vao(ctx->trace, trc_gl_state_get_bound_vao(ctx->trace));
    if (p_index < rev.attribs->size/sizeof(trc_gl_vao_attrib_t)) {
        trc_data_t* newattribs = trc_create_data(ctx->trace, rev.attribs->size, trc_map_data(rev.attribs, TRC_MAP_READ), 0);
        trc_unmap_data(rev.attribs);
        trc_gl_vao_attrib_t* a = &((trc_gl_vao_attrib_t*)trc_map_data(newattribs, TRC_MAP_MODIFY))[p_index];
        a->enabled = true;
        trc_unmap_freeze_data(ctx->trace, newattribs);
        rev.attribs = newattribs;
        trc_set_gl_vao(ctx->trace, trc_gl_state_get_bound_vao(ctx->trace), &rev);
    }
    trc_set_gl_vao(ctx->trace, trc_gl_state_get_bound_vao(ctx->trace), &rev);

glVertexAttribDivisor: //GLuint p_index, GLuint p_divisor
    if (trc_gl_state_get_bound_vao(ctx->trace) == 0) RETURN;
    trc_gl_vao_rev_t rev = *trc_get_gl_vao(ctx->trace, trc_gl_state_get_bound_vao(ctx->trace));
    if (p_index < rev.attribs->size/sizeof(trc_gl_vao_attrib_t)) {
        trc_data_t* newattribs = trc_create_data(ctx->trace, rev.attribs->size, trc_map_data(rev.attribs, TRC_MAP_READ), 0);
        trc_unmap_data(rev.attribs);
        trc_gl_vao_attrib_t* a = &((trc_gl_vao_attrib_t*)trc_map_data(newattribs, TRC_MAP_MODIFY))[p_index];
        a->divisor = p_divisor;
        trc_unmap_freeze_data(ctx->trace, newattribs);
        rev.attribs = newattribs;
        trc_set_gl_vao(ctx->trace, trc_gl_state_get_bound_vao(ctx->trace), &rev);
    }
    trc_set_gl_vao(ctx->trace, trc_gl_state_get_bound_vao(ctx->trace), &rev);

glVertexAttrib1f: //GLuint p_index, GLfloat p_v0
    vertex_attrib(ctx, cmd, 1, GL_FLOAT, false, false, GL_FLOAT);

glVertexAttrib1s: //GLuint p_index, GLshort p_v0
    vertex_attrib(ctx, cmd, 1, GL_INT, false, false, GL_FLOAT);

glVertexAttrib1d: //GLuint p_index, GLdouble p_v0
    vertex_attrib(ctx, cmd, 1, GL_DOUBLE, false, false, GL_FLOAT);

glVertexAttribI1i: //GLuint p_index, GLint p_v0
    vertex_attrib(ctx, cmd, 1, GL_INT, false, false, GL_INT);

glVertexAttribI1ui: //GLuint p_index, GLuint p_v0
    vertex_attrib(ctx, cmd, 1, GL_UNSIGNED_INT, false, false, GL_UNSIGNED_INT);

glVertexAttribL1d: //GLuint p_index, GLdouble p_v0
    vertex_attrib(ctx, cmd, 1, GL_DOUBLE, false, false, GL_DOUBLE);

glVertexAttrib2f: //GLuint p_index, GLfloat p_v0, GLfloat p_v1
    vertex_attrib(ctx, cmd, 2, GL_FLOAT, false, false, GL_FLOAT);

glVertexAttrib2s: //GLuint p_index, GLshort p_v0, GLshort p_v1
    vertex_attrib(ctx, cmd, 2, GL_INT, false, false, GL_FLOAT);

glVertexAttrib2d: //GLuint p_index, GLdouble p_v0, GLdouble p_v1
    vertex_attrib(ctx, cmd, 2, GL_DOUBLE, false, false, GL_FLOAT);

glVertexAttribI2i: //GLuint p_index, GLint p_v0, GLint p_v1
    vertex_attrib(ctx, cmd, 2, GL_INT, false, false, GL_INT);

glVertexAttribI2ui: //GLuint p_index, GLuint p_v0, GLuint p_v1
    vertex_attrib(ctx, cmd, 2, GL_UNSIGNED_INT, false, false, GL_UNSIGNED_INT);

glVertexAttribL2d: //GLuint p_index, GLdouble p_v0, GLdouble p_v1
    vertex_attrib(ctx, cmd, 2, GL_DOUBLE, false, false, GL_DOUBLE);

glVertexAttrib3f: //GLuint p_index, GLfloat p_v0, GLfloat p_v1, GLfloat p_v2
    vertex_attrib(ctx, cmd, 3, GL_FLOAT, false, false, GL_FLOAT);

glVertexAttrib3s: //GLuint p_index, GLshort p_v0, GLshort p_v1, GLshort p_v2
    vertex_attrib(ctx, cmd, 3, GL_INT, false, false, GL_FLOAT);

glVertexAttrib3d: //GLuint p_index, GLdouble p_v0, GLdouble p_v1, GLdouble p_v2
    vertex_attrib(ctx, cmd, 3, GL_DOUBLE, false, false, GL_FLOAT);

glVertexAttribI3i: //GLuint p_index, GLint p_v0, GLint p_v1, GLint p_v2
    vertex_attrib(ctx, cmd, 3, GL_INT, false, false, GL_INT);

glVertexAttribI3ui: //GLuint p_index, GLuint p_v0, GLuint p_v1, GLuint p_v2
    vertex_attrib(ctx, cmd, 3, GL_UNSIGNED_INT, false, false, GL_UNSIGNED_INT);

glVertexAttribL3d: //GLuint p_index, GLdouble p_v0, GLdouble p_v1, GLdouble p_v2
    vertex_attrib(ctx, cmd, 3, GL_DOUBLE, false, false, GL_DOUBLE);

glVertexAttrib4f: //GLuint p_index, GLfloat p_v0, GLfloat p_v1, GLfloat p_v2, GLfloat p_v3
    vertex_attrib(ctx, cmd, 4, GL_FLOAT, false, false, GL_FLOAT);

glVertexAttrib4s: //GLuint p_index, GLshort p_v0, GLshort p_v1, GLshort p_v2, GLshort p_v3
    vertex_attrib(ctx, cmd, 4, GL_INT, false, false, GL_FLOAT);

glVertexAttrib4d: //GLuint p_index, GLdouble p_v0, GLdouble p_v1, GLdouble p_v2, GLdouble p_v3
    vertex_attrib(ctx, cmd, 4, GL_DOUBLE, false, false, GL_FLOAT);

glVertexAttribI4i: //GLuint p_index, GLint p_v0, GLint p_v1, GLint p_v2, GLint p_v3
    vertex_attrib(ctx, cmd, 4, GL_INT, false, false, GL_INT);

glVertexAttribI4ui: //GLuint p_index, GLuint p_v0, GLuint p_v1, GLuint p_v2, GLuint p_v3
    vertex_attrib(ctx, cmd, 4, GL_UNSIGNED_INT, false, false, GL_UNSIGNED_INT);

glVertexAttribL4d: //GLuint p_index, GLdouble p_v0, GLdouble p_v1, GLdouble p_v2, GLdouble p_v3
    vertex_attrib(ctx, cmd, 4, GL_DOUBLE, false, false, GL_DOUBLE);

glVertexAttrib1dv: //GLuint p_index, const GLdouble* p_v
    vertex_attrib(ctx, cmd, 1, GL_DOUBLE, true, false, GL_FLOAT);

glVertexAttrib1fv: //GLuint p_index, const GLfloat* p_v
    vertex_attrib(ctx, cmd, 1, GL_FLOAT, true, false, GL_FLOAT);

glVertexAttrib1sv: //GLuint p_index, const GLshort* p_v
    vertex_attrib(ctx, cmd, 1, GL_SHORT, true, false, GL_FLOAT);

glVertexAttrib2dv: //GLuint p_index, const GLdouble* p_v
    vertex_attrib(ctx, cmd, 2, GL_DOUBLE, true, false, GL_FLOAT);

glVertexAttrib2fv: //GLuint p_index, const GLfloat* p_v
    vertex_attrib(ctx, cmd, 2, GL_FLOAT, true, false, GL_FLOAT);

glVertexAttrib2sv: //GLuint p_index, const GLshort* p_v
    vertex_attrib(ctx, cmd, 2, GL_SHORT, true, false, GL_FLOAT);

glVertexAttrib3dv: //GLuint p_index, const GLdouble* p_v
    vertex_attrib(ctx, cmd, 3, GL_DOUBLE, true, false, GL_FLOAT);

glVertexAttrib3fv: //GLuint p_index, const GLfloat* p_v
    vertex_attrib(ctx, cmd, 3, GL_FLOAT, true, false, GL_FLOAT);

glVertexAttrib3sv: //GLuint p_index, const GLshort* p_v
    vertex_attrib(ctx, cmd, 3, GL_SHORT, true, false, GL_FLOAT);

glVertexAttrib4Nbv: //GLuint p_index, const GLbyte* p_v
    vertex_attrib(ctx, cmd, 4, GL_BYTE, true, true, GL_FLOAT);

glVertexAttrib4Niv: //GLuint p_index, const GLint* p_v
    vertex_attrib(ctx, cmd, 4, GL_INT, true, true, GL_FLOAT);

glVertexAttrib4Nsv: //GLuint p_index, const GLshort* p_v
    vertex_attrib(ctx, cmd, 4, GL_SHORT, true, true, GL_FLOAT);

glVertexAttrib4Nubv: //GLuint p_index, const GLubyte* p_v
    vertex_attrib(ctx, cmd, 4, GL_UNSIGNED_BYTE, true, true, GL_FLOAT);

glVertexAttrib4Nuiv: //GLuint p_index, const GLuint* p_v
    vertex_attrib(ctx, cmd, 4, GL_UNSIGNED_INT, true, true, GL_FLOAT);

glVertexAttrib4Nusv: //GLuint p_index, const GLushort* p_v
    vertex_attrib(ctx, cmd, 4, GL_UNSIGNED_SHORT, false, false, GL_DOUBLE);

glVertexAttrib4bv: //GLuint p_index, const GLbyte* p_v
    vertex_attrib(ctx, cmd, 4, GL_BYTE, false, false, GL_DOUBLE);

glVertexAttrib4dv: //GLuint p_index, const GLdouble* p_v
    vertex_attrib(ctx, cmd, 4, GL_DOUBLE, false, false, GL_DOUBLE);

glVertexAttrib4fv: //GLuint p_index, const GLfloat* p_v
    vertex_attrib(ctx, cmd, 4, GL_FLOAT, false, false, GL_DOUBLE);

glVertexAttrib4iv: //GLuint p_index, const GLint* p_v
    vertex_attrib(ctx, cmd, 4, GL_INT, false, false, GL_DOUBLE);

glVertexAttrib4sv: //GLuint p_index, const GLshort* p_v
    vertex_attrib(ctx, cmd, 4, GL_SHORT, false, false, GL_DOUBLE);

glVertexAttrib4ubv: //GLuint p_index, const GLubyte* p_v
    vertex_attrib(ctx, cmd, 4, GL_UNSIGNED_BYTE, false, false, GL_DOUBLE);

glVertexAttrib4uiv: //GLuint p_index, const GLuint* p_v
    vertex_attrib(ctx, cmd, 4, GL_UNSIGNED_INT, false, false, GL_DOUBLE);

glVertexAttrib4usv: //GLuint p_index, const GLushort* p_v
    vertex_attrib(ctx, cmd, 4, GL_UNSIGNED_SHORT, false, false, GL_DOUBLE);

glVertexAttrib4Nub: //GLuint p_index, GLubyte p_x, GLubyte p_y, GLubyte p_z, GLubyte p_w
    vertex_attrib(ctx, cmd, 4, GL_UNSIGNED_BYTE, false, true, GL_FLOAT);

glVertexAttribI4ubv: //GLuint p_index, const GLubyte* p_v
    vertex_attrib(ctx, cmd, 4, GL_UNSIGNED_SHORT, true, false, GL_UNSIGNED_INT);

glVertexAttribI4usv: //GLuint p_index, const GLushort* p_v
    vertex_attrib(ctx, cmd, 4, GL_UNSIGNED_SHORT, true, false, GL_UNSIGNED_INT);

glVertexAttribI4sv: //GLuint p_index, const GLshort* p_v
    vertex_attrib(ctx, cmd, 4, GL_SHORT, true, false, GL_INT);

glVertexAttribI3iv: //GLuint p_index, const GLint* p_v
    vertex_attrib(ctx, cmd, 3, GL_INT, true, false, GL_INT);

glVertexAttribI4iv: //GLuint p_index, const GLint* p_v
    vertex_attrib(ctx, cmd, 4, GL_INT, true, false, GL_INT);

glVertexAttribI2uiv: //GLuint p_index, const GLuint* p_v
    vertex_attrib(ctx, cmd, 2, GL_UNSIGNED_INT, true, false, GL_UNSIGNED_INT);

glVertexAttribI4uiv: //GLuint p_index, const GLuint* p_v
    vertex_attrib(ctx, cmd, 4, GL_UNSIGNED_INT, true, false, GL_UNSIGNED_INT);

glVertexAttribI4bv: //GLuint p_index, const GLbyte* p_v
    vertex_attrib(ctx, cmd, 4, GL_BYTE, true, false, GL_INT);

glVertexAttribI1iv: //GLuint p_index, const GLint* p_v
    vertex_attrib(ctx, cmd, 1, GL_INT, true, false, GL_INT);

glVertexAttribI2iv: //GLuint p_index, const GLint* p_v
    vertex_attrib(ctx, cmd, 2, GL_INT, true, false, GL_INT);

glVertexAttribI3uiv: //GLuint p_index, const GLuint* p_v
    vertex_attrib(ctx, cmd, 3, GL_UNSIGNED_INT, true, false, GL_UNSIGNED_INT);

glVertexAttribI1uiv: //GLuint p_index, const GLuint* p_v
    vertex_attrib(ctx, cmd, 1, GL_UNSIGNED_INT, true, false, GL_UNSIGNED_INT);

glVertexAttribL1dv: //GLuint p_index, const GLdouble* p_v
    vertex_attrib(ctx, cmd, 1, GL_DOUBLE, true, false, GL_DOUBLE);

glVertexAttribL2dv: //GLuint p_index, const GLdouble* p_v
    vertex_attrib(ctx, cmd, 2, GL_DOUBLE, true, false, GL_DOUBLE);

glVertexAttribL3dv: //GLuint p_index, const GLdouble* p_v
    vertex_attrib(ctx, cmd, 3, GL_DOUBLE, true, false, GL_DOUBLE);

glVertexAttribL4dv: //GLuint p_index, const GLdouble* p_v
    vertex_attrib(ctx, cmd, 4, GL_DOUBLE, true, false, GL_DOUBLE);

glBeginConditionalRender: //GLuint p_id, GLenum p_mode
    uint real_id = trc_get_real_gl_query(ctx->trace, p_id);
    if (!real_id) ERROR("Invalid query name");
    real(real_id, p_mode);

glEndConditionalRender: //
    real();

glDrawArrays: //GLenum p_mode, GLint p_first, GLsizei p_count
    if (!begin_draw(ctx, cmd)) RETURN;
    real(p_mode, p_first, p_count);
    end_draw(ctx, cmd);

glDrawArraysInstanced: //GLenum p_mode, GLint p_first, GLsizei p_count, GLsizei p_instancecount
    if (!begin_draw(ctx, cmd)) RETURN;
    real(p_mode, p_first, p_count, p_instancecount);
    end_draw(ctx, cmd);

glMultiDrawArrays: //GLenum p_mode, const GLint* p_first, const GLsizei* p_count, GLsizei p_drawcount
    if (!begin_draw(ctx, cmd)) RETURN;
    real(p_mode, p_first, p_count, p_drawcount);
    end_draw(ctx, cmd);

glMultiDrawElements: //GLenum p_mode, const GLsizei* p_count, GLenum p_type, const void *const* p_indices, GLsizei p_drawcount
    if (!begin_draw(ctx, cmd)) RETURN;
    real(p_mode, p_count, p_type, p_indices, p_drawcount);
    end_draw(ctx, cmd);

glMultiDrawElementsBaseVertex: //GLenum p_mode, const GLsizei* p_count, GLenum p_type, const void *const* p_indices, GLsizei p_drawcount, const GLint* p_basevertex
    if (!begin_draw(ctx, cmd)) RETURN;
    real(p_mode, p_count, p_type, p_indices, p_drawcount, p_basevertex);
    end_draw(ctx, cmd);

glDrawElements: //GLenum p_mode, GLsizei p_count, GLenum p_type, const void* p_indices
    if (!begin_draw(ctx, cmd)) RETURN;
    real(p_mode, p_count, p_type, (const GLvoid*)p_indices);
    end_draw(ctx, cmd);

glDrawElementsBaseVertex: //GLenum p_mode, GLsizei p_count, GLenum p_type, const void* p_indices, GLint p_basevertex
    if (!begin_draw(ctx, cmd)) RETURN;
    real(p_mode, p_count, p_type, (const GLvoid*)p_indices, p_basevertex);
    end_draw(ctx, cmd);

glDrawElementsInstanced: //GLenum p_mode, GLsizei p_count, GLenum p_type, const void* p_indices, GLsizei p_instancecount
    if (!begin_draw(ctx, cmd)) RETURN;
    real(p_mode, p_count, p_type, (const GLvoid*)p_indices, p_instancecount);
    end_draw(ctx, cmd);

glDrawElementsInstancedBaseVertex: //GLenum p_mode, GLsizei p_count, GLenum p_type, const void* p_indices, GLsizei p_instancecount, GLint p_basevertex
    if (!begin_draw(ctx, cmd)) RETURN;
    real(p_mode, p_count, p_type, (const GLvoid*)p_indices, p_instancecount, p_basevertex);
    end_draw(ctx, cmd);

glDrawElementsInstancedBaseInstance: //GLenum p_mode, GLsizei p_count, GLenum p_type, const void* p_indices, GLsizei p_instancecount, GLuint p_baseinstance
    if (!begin_draw(ctx, cmd)) RETURN;
    real(p_mode, p_count, p_type, (const GLvoid*)p_indices, p_instancecount, p_baseinstance);
    end_draw(ctx, cmd);

glDrawElementsInstancedBaseVertexBaseInstance: //GLenum p_mode, GLsizei p_count, GLenum p_type, const void* p_indices, GLsizei p_instancecount, GLint p_basevertex, GLuint p_baseinstance
    if (!begin_draw(ctx, cmd)) RETURN;
    real(p_mode, p_count, p_type, (const GLvoid*)p_indices, p_instancecount, p_basevertex, p_baseinstance);
    end_draw(ctx, cmd);

glDrawArraysInstancedBaseInstance: //GLenum p_mode, GLint p_first, GLsizei p_count, GLsizei p_instancecount, GLuint p_baseinstance
    if (!begin_draw(ctx, cmd)) RETURN;
    real(p_mode, p_first, p_count, p_instancecount, p_baseinstance);
    end_draw(ctx, cmd);

glDrawRangeElements: //GLenum p_mode, GLuint p_start, GLuint p_end, GLsizei p_count, GLenum p_type, const void* p_indices
    if (!begin_draw(ctx, cmd)) RETURN;
    real(p_mode, p_start, p_end, p_count, p_type, (const GLvoid*)p_indices);
    end_draw(ctx, cmd);

glDrawRangeElementsBaseVertex: //GLenum p_mode, GLuint p_start, GLuint p_end, GLsizei p_count, GLenum p_type, const void* p_indices, GLint p_basevertex
    if (!begin_draw(ctx, cmd)) RETURN;
    real(p_mode, p_start, p_end, p_count, p_type, (const GLvoid*)p_indices, p_basevertex);
    end_draw(ctx, cmd);

glTestFBWIP15: //const GLchar* p_name, const GLvoid* p_color, const GLvoid* p_depth
    F(glFinish)();
    
    //TODO: Save, modify and restore more state (e.g. pack parameters)
    
    GLint last_buf;
    F(glGetIntegerv)(GL_READ_BUFFER, &last_buf);
    
    int w, h;
    SDL_GL_GetDrawableSize(ctx->window, &w, &h);
    
    F(glReadBuffer)(GL_BACK);
    uint32_t* back = malloc(w*h*4);
    F(glReadPixels)(0, 0, w, h, GL_RGBA, GL_UNSIGNED_BYTE, back);
    
    uint32_t* depth = malloc(w*h*4);
    F(glReadPixels)(0, 0, w, h, GL_DEPTH_COMPONENT, GL_UNSIGNED_INT, depth);
    
    F(glReadBuffer)(last_buf);
    
    if (memcmp(back, p_color, w*h*4) != 0)
        fprintf(stderr, "%s did not result in the correct back color buffer (test: %s).\n", p_name, ctx->current_test_name);
    if (memcmp(depth, p_depth, w*h*4) != 0)
        fprintf(stderr, "%s did not result in the correct back color buffer (test: %s).\n", p_name, ctx->current_test_name);
    
    free(back);
    free(depth);

glCurrentTestWIP15: //const GLchar* p_name
    ctx->current_test_name = p_name;

glGenVertexArrays: //GLsizei p_n, GLuint* p_arrays
    if (p_n < 0) ERROR("Invalid vertex array object name count");
    GLuint* arrays = replay_alloc(p_n*sizeof(GLuint));
    real(p_n, arrays);
    gen_vertex_arrays(ctx, p_n, arrays, p_arrays, false);

glCreateVertexArrays: //GLsizei p_n, GLuint* p_arrays
    if (p_n < 0) ERROR("Invalid vertex array object name count");
    GLuint* arrays = replay_alloc(p_n*sizeof(GLuint));
    real(p_n, arrays);
    gen_vertex_arrays(ctx, p_n, arrays, p_arrays, true);

glDeleteVertexArrays: //GLsizei p_n, const GLuint* p_arrays
    GLuint* arrays = replay_alloc(p_n*sizeof(GLuint));
    for (size_t i = 0; i < p_n; ++i) {
        if (p_arrays[i] && p_arrays[i]==trc_gl_state_get_bound_vao(ctx->trace))
            trc_gl_state_set_bound_vao(ctx->trace, 0);
        if (!(arrays[i]=trc_get_real_gl_vao(ctx->trace, p_arrays[i])))
            trc_add_error(cmd, "Invalid vertex array name");
        else trc_rel_gl_obj(ctx->trace, p_arrays[i], TrcGLObj_VAO);
    }
    real(p_n, arrays);

glBindVertexArray: //GLuint p_array
    const trc_gl_vao_rev_t* rev = trc_get_gl_vao(ctx->trace, p_array);
    if (!rev && p_array) ERROR("Invalid vertex array name");
    trc_gl_state_set_bound_vao(ctx->trace, p_array);
    if (rev && !rev->has_object) {
        trc_gl_vao_rev_t newrev = *rev;
        newrev.has_object = true;
        trc_set_gl_vao(ctx->trace, p_array, &newrev);
    }

glGetFragDataIndex: //GLuint p_program, const GLchar* p_name
    GLuint real_program = trc_get_real_gl_program(ctx->trace, p_program);
    if (!real_program) ERROR("Invalid program name");

glGetFragDataLocation: //GLuint p_program, const GLchar* p_name
    GLuint real_program = trc_get_real_gl_program(ctx->trace, p_program);
    if (!real_program) ERROR("Invalid program name");

glGetUniformBlockIndex: //GLuint p_program, const GLchar* p_uniformBlockName
    GLuint real_program = trc_get_real_gl_program(ctx->trace, p_program);
    if (!real_program) ERROR("Invalid program name");

glGetUniformIndices: //GLuint p_program, GLsizei p_uniformCount, const GLchar  *const* p_uniformNames, GLuint* p_uniformIndices
    GLuint real_program = trc_get_real_gl_program(ctx->trace, p_program);
    if (!real_program) ERROR("Invalid program name");

glDrawableSizeWIP15: //GLsizei p_width, GLsizei p_height
    if (p_width < 0) p_width = 100;
    if (p_height < 0) p_height = 100;
    
    SDL_SetWindowSize(ctx->window, p_width, p_height);
    
    trc_gl_context_rev_t state = *trc_get_gl_context(ctx->trace, 0);
    if (state.drawable_width==p_width && state.drawable_height==p_height) return;
    state.drawable_width = p_width;
    state.drawable_height = p_height;
    replay_create_context_buffers(ctx->trace, &state);
    trc_set_gl_context(ctx->trace, 0, &state);

glGetUniformfv: //GLuint p_program, GLint p_location, GLfloat* p_params
    validate_get_uniform(ctx, cmd);

glGetUniformiv: //GLuint p_program, GLint p_location, GLint* p_params
    validate_get_uniform(ctx, cmd);

glGetUniformuiv: //GLuint p_program, GLint p_location, GLuint* p_params
    validate_get_uniform(ctx, cmd);

glGetUniformdv: //GLuint p_program, GLint p_location, GLdouble* p_params
    validate_get_uniform(ctx, cmd);

glGetnUniformfv: //GLuint p_program, GLint p_location, GLsizei p_bufSize, GLfloat* p_params
    validate_get_uniform(ctx, cmd);

glGetnUniformiv: //GLuint p_program, GLint p_location, GLsizei p_bufSize, GLint* p_params
    validate_get_uniform(ctx, cmd);

glGetnUniformuiv: //GLuint p_program, GLint p_location, GLsizei p_bufSize, GLuint* p_params
    validate_get_uniform(ctx, cmd);

glGetnUniformdv: //GLuint p_program, GLint p_location, GLsizei p_bufSize, GLdouble* p_params
    validate_get_uniform(ctx, cmd);

glGetMultisamplefv: //GLenum p_pname, GLuint p_index, GLfloat* p_val
    ; //TODO: More validation should be done

glGetInternalformativ: //GLenum p_target, GLenum p_internalformat, GLenum p_pname, GLsizei p_bufSize, GLint* p_params
    GLint* params = replay_alloc(p_bufSize*sizeof(GLint));
    real(p_target, p_internalformat, p_pname, p_bufSize, params);

glGetInternalformati64v: //GLenum p_target, GLenum p_internalformat, GLenum p_pname, GLsizei p_bufSize, GLint64* p_params
    GLint64* params = replay_alloc(p_bufSize*sizeof(GLint64));
    real(p_target, p_internalformat, p_pname, p_bufSize, params);

glGetBufferParameteriv: //GLenum p_target, GLenum p_pname, GLint* p_params
    GLint i;
    real(p_target, p_pname, &i);

glGetBufferPointerv: //GLenum p_target, GLenum p_pname, void ** p_params
    GLvoid* p;
    real(p_target, p_pname, &p);

glGenSamplers: //GLsizei p_count, GLuint* p_samplers
    GLuint* samplers = replay_alloc(p_count*sizeof(GLuint));
    real(p_count, samplers);
    
    trc_gl_sampler_rev_t rev;
    rev.fake_context = trc_get_current_fake_gl_context(ctx->trace);
    rev.ref_count = 1;
    for (size_t i = 0; i < p_count; ++i) {
        rev.real = samplers[i];
        trc_set_gl_sampler(ctx->trace, p_samplers[i], &rev);
    }

glDeleteSamplers: //GLsizei p_count, const GLuint* p_samplers
    GLuint* samplers = replay_alloc(p_count*sizeof(GLuint));
    for (size_t i = 0; i < p_count; ++i) {
        if (!(samplers[i] = trc_get_real_gl_sampler(ctx->trace, p_samplers[i])))
            trc_add_error(cmd, "Invalid sampler name");
        else trc_rel_gl_obj(ctx->trace, p_samplers[i], TrcGLObj_Sampler);
    }
    
    real(p_count, samplers);

glBindSampler: //GLuint p_unit, GLuint p_sampler
    GLuint real_sampler = trc_get_real_gl_sampler(ctx->trace, p_sampler);
    if (!real_sampler && p_sampler) ERROR("Invalid sampler name");
    real(p_unit, real_sampler);

glBindSamplers: //GLuint p_first, GLsizei p_count, const GLuint* p_samplers
    if (p_first+p_count>trc_gl_state_get_state_int(ctx->trace, GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS, 0) || p_count<0)
        ERROR("Invalid range");
    GLuint* real_tex = replay_alloc(p_count*sizeof(GLuint));
    for (size_t i = 0; i < p_count; i++) {
        const trc_gl_sampler_rev_t* rev = trc_get_gl_sampler(ctx->trace, p_samplers[i]);
        if (!rev) ERROR("Invalid sampler name at index %zu", i);
        //if (!rev->has_object) ERROR("Sample name at index %zu has no object", i); //TODO
        real_tex[i] = rev->real;
    }
    real(p_first, p_count, real_tex);

glGetSynciv: //GLsync p_sync, GLenum p_pname, GLsizei p_bufSize, GLsizei* p_length, GLint* p_values
    if (!trc_get_real_gl_sync(ctx->trace, p_sync)) ERROR("Invalid sync name");
    //TODO: More validation should be done

glSamplerParameterf: //GLuint p_sampler, GLenum p_pname, GLfloat p_param
    GLuint sampler = trc_get_real_gl_sampler(ctx->trace, p_sampler);
    if (!sampler) ERROR("Invalid sampler name");
    real(sampler, p_pname, p_param);

glSamplerParameteri: //GLuint p_sampler, GLenum p_pname, GLint p_param
    GLuint sampler = trc_get_real_gl_sampler(ctx->trace, p_sampler);
    if (!sampler) ERROR("Invalid sampler name");
    real(sampler, p_pname, p_param);

glSamplerParameterfv: //GLuint p_sampler, GLenum p_pname, const GLfloat* p_param
    GLuint sampler = trc_get_real_gl_sampler(ctx->trace, p_sampler);
    if (!sampler) ERROR("Invalid sampler name");
    real(sampler, p_pname, p_param);

glSamplerParameteriv: //GLuint p_sampler, GLenum p_pname, const GLint* p_param
    GLuint sampler = trc_get_real_gl_sampler(ctx->trace, p_sampler);
    if (!sampler) ERROR("Invalid sampler name");
    real(sampler, p_pname, p_param);

glSamplerParameterIiv: //GLuint p_sampler, GLenum p_pname, const GLint* p_param
    GLuint sampler = trc_get_real_gl_sampler(ctx->trace, p_sampler);
    if (!sampler) ERROR("Invalid sampler name");
    real(sampler, p_pname, p_param);

glSamplerParameterIuiv: //GLuint p_sampler, GLenum p_pname, const GLuint* p_param
    GLuint sampler = trc_get_real_gl_sampler(ctx->trace, p_sampler);
    if (!sampler) ERROR("Invalid sampler name");
    real(sampler, p_pname, p_param);

glGenFramebuffers: //GLsizei p_n, GLuint* p_framebuffers
    if (p_n < 0) ERROR("Invalid framebuffer name count");
    GLuint* fbs = replay_alloc(p_n*sizeof(GLuint));
    real(p_n, fbs);
    gen_framebuffers(ctx, p_n, fbs, p_framebuffers, false);

glCreateFramebuffers: //GLsizei p_n, GLuint* p_framebuffers
    if (p_n < 0) ERROR("Invalid framebuffer name count");
    GLuint* fbs = replay_alloc(p_n*sizeof(GLuint));
    real(p_n, fbs);
    gen_framebuffers(ctx, p_n, fbs, p_framebuffers, true);

glDeleteFramebuffers: //GLsizei p_n, const GLuint* p_framebuffers
    GLuint* fbs = replay_alloc(p_n*sizeof(GLuint));
    for (size_t i = 0; i < p_n; ++i) {
        if (p_framebuffers[i] && p_framebuffers[i]==trc_gl_state_get_read_framebuffer(ctx->trace))
            trc_gl_state_set_read_framebuffer(ctx->trace, 0);
        if (p_framebuffers[i] && p_framebuffers[i]==trc_gl_state_get_draw_framebuffer(ctx->trace))
            trc_gl_state_set_draw_framebuffer(ctx->trace, 0);
        if (!(fbs[i] = trc_get_real_gl_framebuffer(ctx->trace, p_framebuffers[i])))
            trc_add_error(cmd, "Invalid framebuffer name");
        else trc_rel_gl_obj(ctx->trace, p_framebuffers[i], TrcGLObj_Framebuffer);
    }
    real(p_n, fbs);

glBindFramebuffer: //GLenum p_target, GLuint p_framebuffer
    const trc_gl_framebuffer_rev_t* rev = trc_get_gl_framebuffer(ctx->trace, p_framebuffer);
    if (!rev && p_framebuffer) ERROR("Invalid framebuffer name");
    real(p_target, p_framebuffer?rev->real:0);
    
    if (rev && !rev->has_object) {
        trc_gl_framebuffer_rev_t newrev = *rev;
        newrev.has_object = true;
        trc_set_gl_framebuffer(ctx->trace, p_framebuffer, &newrev);
    }
    
    bool read = true;
    bool draw = true;
    switch (p_target) {
    case GL_READ_FRAMEBUFFER: draw = false; break;
    case GL_DRAW_FRAMEBUFFER: read = false; break;
    }
    if (read) trc_gl_state_set_read_framebuffer(ctx->trace, p_framebuffer);
    if (draw) trc_gl_state_set_draw_framebuffer(ctx->trace, p_framebuffer);

glGenRenderbuffers: //GLsizei p_n, GLuint* p_renderbuffers
    if (p_n < 0) ERROR("Invalid renderbuffer name count");
    GLuint* rbs = replay_alloc(p_n*sizeof(GLuint));
    real(p_n, rbs);
    gen_renderbuffers(ctx, p_n, rbs, p_renderbuffers, false);

glCreateRenderbuffers: //GLsizei p_n, GLuint* p_renderbuffers
    if (p_n < 0) ERROR("Invalid renderbuffer name count");
    GLuint* rbs = replay_alloc(p_n*sizeof(GLuint));
    real(p_n, rbs);
    gen_renderbuffers(ctx, p_n, rbs, p_renderbuffers, true);

glDeleteRenderbuffers: //GLsizei p_n, const GLuint* p_renderbuffers
    GLuint* rbs = replay_alloc(p_n*sizeof(GLuint));
    for (size_t i = 0; i < p_n; ++i) {
        if (p_renderbuffers[i] && p_renderbuffers[i]==trc_gl_state_get_bound_renderbuffer(ctx->trace))
            trc_gl_state_set_bound_renderbuffer(ctx->trace, 0);
        //TODO: Detach from bound framebuffers
        //TODO: What to do with renderbuffers attached to non-bound framebuffers?
        if (!(rbs[i] = trc_get_real_gl_renderbuffer(ctx->trace, p_renderbuffers[i])))
            trc_add_error(cmd, "Invalid renderbuffer name");
        else trc_rel_gl_obj(ctx->trace, p_renderbuffers[i], TrcGLObj_Renderbuffer);
    }
    
    real(p_n, rbs);

glBindRenderbuffer: //GLenum p_target, GLuint p_renderbuffer
    const trc_gl_renderbuffer_rev_t* rev = trc_get_gl_renderbuffer(ctx->trace, p_renderbuffer);
    if (!rev && p_renderbuffer) ERROR("Invalid renderbuffer name");
    real(p_target, p_renderbuffer?rev->real:0);
    trc_gl_state_set_bound_renderbuffer(ctx->trace, p_renderbuffer);
    if (rev && !rev->has_object) {
        trc_gl_renderbuffer_rev_t newrev = *rev;
        newrev.has_object = true;
        trc_set_gl_renderbuffer(ctx->trace, p_renderbuffer, &newrev);
    }

glGetActiveUniformBlockiv: //GLuint p_program, GLuint p_uniformBlockIndex, GLenum p_pname, GLint* p_params
    GLuint program = trc_get_real_gl_program(ctx->trace, p_program);
    if (!program) ERROR("Invalid program ");
    
    if (p_pname == GL_UNIFORM_BLOCK_ACTIVE_UNIFORM_INDICES) {
        GLint count;
        real(program, p_uniformBlockIndex, GL_UNIFORM_BLOCK_ACTIVE_UNIFORMS, &count);
        
        GLint* vals = malloc(sizeof(GLint)*count);
        real(program, p_uniformBlockIndex, p_pname, vals);
        free(vals);
    } else {
        GLint v;
        real(program, p_uniformBlockIndex, p_pname, &v);
    }

glGetActiveUniformBlockName: //GLuint p_program, GLuint p_uniformBlockIndex, GLsizei p_bufSize, GLsizei* p_length, GLchar* p_uniformBlockName
    GLuint program = trc_get_real_gl_program(ctx->trace, p_program);
    if (!program) ERROR("Invalid program name");
    GLchar buf[64];
    real(program, p_uniformBlockIndex, 64, NULL, buf);

glGetActiveUniformName: //GLuint p_program, GLuint p_uniformIndex, GLsizei p_bufSize, GLsizei* p_length, GLchar* p_uniformName
    GLuint program = trc_get_real_gl_program(ctx->trace, p_program);
    if (!program) ERROR("Invalid program name");
    GLchar buf[64];
    real(program, p_uniformIndex, 64, NULL, buf);

glGetActiveUniformsiv: //GLuint p_program, GLsizei p_uniformCount, const GLuint* p_uniformIndices, GLenum p_pname, GLint* p_params
    GLuint program = trc_get_real_gl_program(ctx->trace, p_program);
    if (!program) ERROR("Invalid program name");
    GLint* params = replay_alloc(p_uniformCount*sizeof(GLint));
    real(program, p_uniformCount, p_uniformIndices, p_pname, params);

glGetFramebufferAttachmentParameteriv: //GLenum p_target, GLenum p_attachment, GLenum p_pname, GLint* p_params
    GLint params;
    real(p_target, p_attachment, p_pname, &params);

glGetRenderbufferParameteriv: //GLenum p_target, GLenum p_pname, GLint* p_params
    const trc_gl_renderbuffer_rev_t* rev = trc_get_gl_renderbuffer(ctx->trace, trc_gl_state_get_bound_renderbuffer(ctx->trace));
    if (!rev) ERROR("No renderbuffer bound");
    if (!rev->has_object) ERROR("Renderbuffer name has no object");

glGetNamedRenderbufferParameteriv: //GLuint p_renderbuffer, GLenum p_pname, GLint* p_params
    if (!p_renderbuffer_rev) ERROR("Invalid renderbuffer name");
    if (!p_renderbuffer_rev->has_object) ERROR("Renderbuffer name has no object");

glFramebufferRenderbuffer: //GLenum p_target, GLenum p_attachment, GLenum p_renderbuffertarget, GLuint p_renderbuffer
    GLint fb = get_bound_framebuffer(ctx, p_target);
    if (add_fb_attachment_rb(ctx, cmd, false, fb, p_attachment, p_renderbuffer, p_renderbuffer_rev))
        real(p_target, p_attachment, p_renderbuffertarget, p_renderbuffer?p_renderbuffer_rev->real:0);

glNamedFramebufferRenderbuffer: //GLuint p_framebuffer, GLenum p_attachment, GLenum p_renderbuffertarget, GLuint p_renderbuffer
    if (add_fb_attachment_rb(ctx, cmd, true, p_framebuffer, p_attachment, p_renderbuffer, p_renderbuffer_rev))
        real(p_framebuffer_rev->real, p_attachment, p_renderbuffertarget, p_renderbuffer?p_renderbuffer_rev->real:0);

glFramebufferTexture: //GLenum p_target, GLenum p_attachment, GLuint p_texture, GLint p_level
    GLint fb = get_bound_framebuffer(ctx, p_target);
    if (add_fb_attachment(ctx, cmd, fb, p_attachment, false, p_texture, p_texture_rev, p_texture_rev?p_texture_rev->type:0, p_level, 0))
        real(p_target, p_attachment, p_texture?p_texture_rev->real:0, p_level);

glNamedFramebufferTexture: //GLuint p_framebuffer, GLenum p_attachment, GLuint p_texture, GLint p_level
    if (add_fb_attachment(ctx, cmd, p_framebuffer, p_attachment, true, p_texture, p_texture_rev, p_texture_rev?p_texture_rev->type:0, p_level, 0))
        real(p_framebuffer_rev->real, p_attachment, p_texture?p_texture_rev->real:0, p_level);

glFramebufferTextureLayer: //GLenum p_target, GLenum p_attachment GLuint p_texture, GLint p_level, GLint p_layer
    GLint fb = get_bound_framebuffer(ctx, p_target);
    if (add_fb_attachment(ctx, cmd, fb, p_attachment, false, p_texture, p_texture_rev, p_texture_rev?p_texture_rev->type:0, p_level, p_layer))
        real(p_target, p_attachment, p_texture?p_texture_rev->real:0, p_level, p_layer);

glNamedFramebufferTextureLayer: //GLuint p_framebuffer, GLenum p_attachment GLuint p_texture, GLint p_level, GLint p_layer
    if (add_fb_attachment(ctx, cmd, p_framebuffer, p_attachment, true, p_texture, p_texture_rev, p_texture_rev?p_texture_rev->type:0, p_level, p_layer))
        real(p_framebuffer_rev->real, p_attachment, p_texture?p_texture_rev->real:0, p_level, p_layer);

glFramebufferTexture1D: //GLenum p_target, GLenum p_attachment, GLenum p_textarget, GLuint p_texture, GLint p_level
    GLint fb = get_bound_framebuffer(ctx, p_target);
    if (add_fb_attachment(ctx, cmd, fb, p_attachment, false, p_texture, p_texture_rev, p_textarget, p_level, 0))
        real(p_target, p_attachment, p_textarget, p_texture?p_texture_rev->real:0, p_level);

glFramebufferTexture2D: //GLenum p_target, GLenum p_attachment, GLenum p_textarget, GLuint p_texture, GLint p_level
    GLint fb = get_bound_framebuffer(ctx, p_target);
    if (add_fb_attachment(ctx, cmd, fb, p_attachment, false, p_texture, p_texture_rev, p_textarget, p_level, 0))
        real(p_target, p_attachment, p_textarget, p_texture?p_texture_rev->real:0, p_level);

glFramebufferTexture3D: //GLenum p_target, GLenum p_attachment, GLenum p_textarget, GLuint p_texture, GLint p_level, GLint p_zoffset
    GLint fb = get_bound_framebuffer(ctx, p_target);
    if (add_fb_attachment(ctx, cmd, fb, p_attachment, false, p_texture, p_texture_rev, p_textarget, p_level, p_zoffset))
        real(p_target, p_attachment, p_textarget, p_texture?p_texture_rev->real:0, p_level, p_zoffset);

glRenderbufferStorage: //GLenum p_target, GLenum p_internalformat, GLsizei p_width, GLsizei p_height
    uint rb = trc_gl_state_get_bound_renderbuffer(ctx->trace);
    const trc_gl_renderbuffer_rev_t* rb_rev = trc_get_gl_renderbuffer(ctx->trace, rb);
    if (renderbuffer_storage(ctx, cmd, rb_rev, false, p_internalformat, p_width, p_height, 1)) {
        real(p_target, p_internalformat, p_width, p_height);
        replay_update_renderbuffer(ctx, rb_rev, rb, p_width, p_height, p_internalformat, 1);
    }

glNamedRenderbufferStorage: //GLuint p_renderbuffer, GLenum p_internalformat, GLsizei p_width, GLsizei p_height
    if (renderbuffer_storage(ctx, cmd, p_renderbuffer_rev, false, p_internalformat, p_width, p_height, 1)) {
        real(p_renderbuffer_rev->real, p_internalformat, p_width, p_height);
        replay_update_renderbuffer(ctx, p_renderbuffer_rev, p_renderbuffer, p_width, p_height, p_internalformat, 1);
    }

glRenderbufferStorageMultisample: //GLenum p_target, GLsizei p_samples, GLenum p_internalformat, GLsizei p_width, GLsizei p_height
    uint rb = trc_gl_state_get_bound_renderbuffer(ctx->trace);
    const trc_gl_renderbuffer_rev_t* rb_rev = trc_get_gl_renderbuffer(ctx->trace, rb);
    if (renderbuffer_storage(ctx, cmd, rb_rev, false, p_internalformat, p_width, p_height, p_samples)) {
        real(p_target, p_samples, p_internalformat, p_width, p_height);
        replay_update_renderbuffer(ctx, rb_rev, rb, p_width, p_height, p_internalformat, p_samples);
    }

glNamedRenderbufferStorageMultisample: //GLuint p_renderbuffer, GLsizei p_samples, GLenum p_internalformat, GLsizei p_width, GLsizei p_height
    if (renderbuffer_storage(ctx, cmd, p_renderbuffer_rev, false, p_internalformat, p_width, p_height, p_samples)) {
        real(p_renderbuffer, p_samples, p_internalformat, p_width, p_height);
        replay_update_renderbuffer(ctx, p_renderbuffer_rev, p_renderbuffer, p_width, p_height, p_internalformat, p_samples);
    }

glFenceSync: //GLenum p_condition, GLbitfield p_flags
    GLsync real_sync = real(p_condition, p_flags);
    
    uint64_t fake = *trc_get_ptr(&cmd->ret);
    
    trc_gl_sync_rev_t rev;
    rev.fake_context = trc_get_current_fake_gl_context(ctx->trace);
    rev.ref_count = 1;
    rev.real = (uint64_t)real_sync;
    rev.type = GL_SYNC_FENCE;
    rev.condition = p_condition;
    rev.flags = p_flags;
    trc_set_gl_sync(ctx->trace, fake, &rev);

glDeleteSync: //GLsync p_sync
    if (!p_sync_rev && p_sync) ERROR("Invalid sync name");
    real(p_sync?(GLsync)p_sync_rev->real:0);
    if (p_sync) trc_rel_gl_obj(ctx->trace, p_sync, TrcGLObj_Sync);

glWaitSync: //GLsync p_sync, GLbitfield p_flags, GLuint64 p_timeout
    if (!p_sync_rev) ERROR("Invalid sync name");
    real((GLsync)p_sync_rev->real, p_flags, p_timeout);
    //TODO: grab the object and release it once the wait is over

glClientWaitSync: //GLsync p_sync, GLbitfield p_flags, GLuint64 p_timeout
    if (!p_sync_rev) ERROR("Invalid sync name");
    real((GLsync)p_sync_rev->real, p_flags, p_timeout);

glGenQueries: //GLsizei p_n, GLuint* p_ids
    if (p_n < 0) ERROR("Invalid name query count");
    GLuint* queries = replay_alloc(p_n*sizeof(GLuint));
    real(p_n, queries);
    gen_queries(ctx, p_n, queries, p_ids, false, 0);

glCreateQueries: //GLenum p_target, GLsizei p_n, GLuint* p_ids
    if (p_n < 0) ERROR("Invalid name query count");
    GLuint* queries = replay_alloc(p_n*sizeof(GLuint));
    real(p_target, p_n, queries);
    gen_queries(ctx, p_n, queries, p_ids, false, p_target);

glDeleteQueries: //GLsizei p_n, const GLuint* p_ids
    GLuint* queries = replay_alloc(p_n*sizeof(GLuint));
    for (size_t i = 0; i < p_n; ++i) {
        //TODO: Handle when queries are in use
        if (!(queries[i] = trc_get_real_gl_query(ctx->trace, p_ids[i])))
            trc_add_error(cmd, "Invalid query name");
        else trc_rel_gl_obj(ctx->trace, p_ids[i], TrcGLObj_Query);
    }
    real(p_n, queries);

glBeginQuery: //GLenum p_target, GLuint p_id
    if (begin_query(ctx, cmd, p_target, 0, p_id))
        real(p_target, p_id_rev->real);

glEndQuery: //GLenum p_target
    if (end_query(ctx, cmd, p_target, 0)) real(p_target);

glBeginQueryIndexed: //GLenum p_target, GLuint p_index, GLuint p_id
    if (begin_query(ctx, cmd, p_target, p_index, p_id))
        real(p_target, p_index, p_id_rev->real);

glEndQueryIndexed: //GLenum p_target, GLuint p_index
    if (end_query(ctx, cmd, p_target, p_index)) real(p_target, p_index);

glQueryCounter: //GLuint p_id, GLenum p_target
    const trc_gl_query_rev_t* rev = trc_get_gl_query(ctx->trace, p_id);
    if (!rev) ERROR("Invalid query name");
    real(rev->real, p_target);
    if (!rev->has_object) {
        trc_gl_query_rev_t newrev = *rev;
        newrev.has_object = true;
        newrev.type = GL_TIMESTAMP;
        trc_set_gl_query(ctx->trace, p_id, &newrev);
    }
    //TODO: This clears any errors
    if (F(glGetError)() == GL_NO_ERROR) update_query(ctx, cmd, p_target, p_id, rev->real);

glSampleMaski: //GLuint p_maskNumber, GLbitfield p_mask
    real(p_maskNumber, p_mask);
    //TODO: Set state

glDrawBuffer: //GLenum p_buf
    real(p_buf);
    //TODO: Set state

glDrawBuffers: //GLsizei p_n, const GLenum* p_bufs
    if (p_n < 0) ERROR("buffer count is less than zero");
    if (p_n > trc_gl_state_get_state_int(ctx->trace, GL_MAX_DRAW_BUFFERS, 0))
        ERROR("buffer count is greater than GL_MAX_DRAW_BUFFERS");
    
    GLuint fb = trc_gl_state_get_draw_framebuffer(ctx->trace);
    
    uint color_min = GL_COLOR_ATTACHMENT0;
    uint color_max = GL_COLOR_ATTACHMENT0 + trc_gl_state_get_state_int(ctx->trace, GL_MAX_COLOR_ATTACHMENTS, 0);
    for (uint i = 0; i < p_n; i++) {
        if (fb==0 && not_one_of(p_bufs[i], GL_NONE, GL_FRONT_LEFT, GL_FRONT_RIGHT, GL_BACK_LEFT, GL_BACK_RIGHT, -1))
            ERROR("Invalid buffer");
        else if (fb>0 && (p_bufs[i]<color_min||p_bufs[i]>color_max) && p_bufs[i]!=GL_NONE)
            ERROR("Invalid buffer");
        for (uint j = 0; j < p_n; j++) {
            if (p_bufs[j]==p_bufs[i] && i!=j && p_bufs[i]!=GL_NONE)
                ERROR("Buffer %u appears more than once", j);
        }
        if (p_bufs[i]==GL_BACK && p_n!=1)
            ERROR("GL_BACK can only be a buffer is the buffer count is one");
    }
    //TODO: From reference:
    //    GL_INVALID_OPERATION is generated if any of the entries in bufs (other than GL_NONE)
    //    indicates a color buffer that does not exist in the current GL context.
    
    if (fb == 0) {
        trc_gl_state_state_enum_init(ctx->trace, GL_DRAW_BUFFER, p_n, p_bufs);
    } else {
        trc_gl_framebuffer_rev_t rev = *trc_get_gl_framebuffer(ctx->trace, fb);
        rev.draw_buffers = trc_create_data(ctx->trace, p_n*sizeof(GLenum), p_bufs, TRC_DATA_IMMUTABLE);
        trc_set_gl_framebuffer(ctx->trace, fb, &rev);
    }
    
    real(p_n, p_bufs);

glReadBuffer: //GLenum p_src
    real(p_src);
    //TODO

glClearBufferiv: //GLenum p_buffer, GLint p_drawbuffer, const GLint* p_value
    //if (not_one_of(p_buffer, GL_COLOR, GL_STENCIL, -1))
    //    ERROR("Buffer is not one of GL_COLOR or GL_STENCIL");
    PARTIAL_VALIDATE_CLEAR_BUFFER;
    real(p_buffer, p_drawbuffer, p_value);
    update_drawbuffer(ctx, p_buffer, p_drawbuffer);

glClearBufferuiv: //GLenum p_buffer, GLint p_drawbuffer, const GLuint* p_value
    //if (p_buffer != GL_COLOR) ERROR("Buffer is not GL_COLOR");
    PARTIAL_VALIDATE_CLEAR_BUFFER;
    real(p_buffer, p_drawbuffer, p_value);
    update_drawbuffer(ctx, p_buffer, p_drawbuffer);

glClearBufferfv: //GLenum p_buffer, GLint p_drawbuffer, const GLfloat* p_value
    //if (not_one_of(p_buffer, GL_COLOR, GL_DEPTH, -1))
    //    ERROR("Buffer is not one of GL_COLOR or GL_DEPTH");
    PARTIAL_VALIDATE_CLEAR_BUFFER;
    real(p_buffer, p_drawbuffer, p_value);
    update_drawbuffer(ctx, p_buffer, p_drawbuffer);

glClearBufferfi: //GLenum p_buffer, GLint p_drawbuffer, GLfloat p_depth, GLint p_stencil
    //if (p_buffer != GL_DEPTH_STENCIL) ERROR("Buffer is not GL_DEPTH_STENCIL");
    PARTIAL_VALIDATE_CLEAR_BUFFER;
    real(p_buffer, p_drawbuffer, p_depth, p_stencil);
    update_drawbuffer(ctx, GL_DEPTH, p_drawbuffer);
    update_drawbuffer(ctx, GL_STENCIL, p_drawbuffer);

glBindFragDataLocation: //GLuint p_program, GLuint p_color, const GLchar* p_name
    GLuint real_program = trc_get_real_gl_program(ctx->trace, p_program);
    if (!real_program) ERROR("Invalid program name");
    real(real_program, p_color, p_name);

glBindFragDataLocationIndexed: //GLuint p_program, GLuint p_colorNumber, GLuint p_index, const GLchar* p_name
    GLuint real_program = trc_get_real_gl_program(ctx->trace, p_program);
    if (!real_program) ERROR("Invalid program name");
    real(real_program, p_colorNumber, p_index, p_name);

glEnable: //GLenum p_cap
    //GL_CLIP_DISTANCEi are the only constants in the form of 0x3xxx accepted by glEnable and glDisable
    if ((p_cap&0x3000)==0x3000 && p_cap-0x3000<trc_gl_state_get_state_int(ctx->trace, GL_MAX_CLIP_DISTANCES, 0))
        trc_gl_state_set_enabled(ctx->trace, GL_CLIP_DISTANCE0, p_cap-0x3000, true);
    else
        trc_gl_state_set_enabled(ctx->trace, p_cap, 0, true);
    real(p_cap);

glDisable: //GLenum p_cap
    if ((p_cap&0x3000)==0x3000 && p_cap-0x3000<trc_gl_state_get_state_int(ctx->trace, GL_MAX_CLIP_DISTANCES, 0))
        trc_gl_state_set_enabled(ctx->trace, GL_CLIP_DISTANCE0, p_cap-0x3000, false);
    else
        trc_gl_state_set_enabled(ctx->trace, p_cap, 0, true);
    real(p_cap);

glEnablei: //GLenum p_target, GLuint p_index
    if ((p_target&0x3000) == 0x3000) {
        if (p_index==0 && p_target-0x3000<trc_gl_state_get_state_int(ctx->trace, GL_MAX_CLIP_DISTANCES, 0))
            trc_gl_state_set_enabled(ctx->trace, GL_CLIP_DISTANCE0, p_target-0x3000, true);
    } else if (p_index < trc_gl_state_get_enabled_size(ctx->trace, p_target)) {
        trc_gl_state_set_enabled(ctx->trace, p_target, p_index, true);
    }
    real(p_target, p_index);

glDisablei: //GLenum p_target, GLuint p_index
    if ((p_target&0x3000) == 0x3000) {
        if (p_index==0 && p_target-0x3000<trc_gl_state_get_state_int(ctx->trace, GL_MAX_CLIP_DISTANCES, 0))
            trc_gl_state_set_enabled(ctx->trace, GL_CLIP_DISTANCE0, p_target-0x3000, false);
    } else if (p_index < trc_gl_state_get_enabled_size(ctx->trace, p_target)) {
        trc_gl_state_set_enabled(ctx->trace, p_target, p_index, false);
    }
    real(p_target, p_index);

glDepthMask: //GLboolean p_flag
    trc_gl_state_set_state_bool(ctx->trace, GL_DEPTH_WRITEMASK, 0, p_flag);
    real(p_flag);

glColorMask: //GLboolean p_red, GLboolean p_green, GLboolean p_blue, GLboolean p_alpha
    for (size_t i = 0; i < trc_gl_state_get_state_int(ctx->trace, GL_MAX_DRAW_BUFFERS, 0); i+=4) {
        trc_gl_state_set_state_bool(ctx->trace, GL_COLOR_WRITEMASK, i+0, p_red);
        trc_gl_state_set_state_bool(ctx->trace, GL_COLOR_WRITEMASK, i+1, p_green);
        trc_gl_state_set_state_bool(ctx->trace, GL_COLOR_WRITEMASK, i+2, p_blue);
        trc_gl_state_set_state_bool(ctx->trace, GL_COLOR_WRITEMASK, i+3, p_alpha);
    }
    real(p_red, p_green, p_blue, p_alpha);

glColorMaski: //GLuint p_index, GLboolean p_r, GLboolean p_g, GLboolean p_b, GLboolean p_a
    if (p_index < trc_gl_state_get_state_int(ctx->trace, GL_MAX_DRAW_BUFFERS, 0)) {
        trc_gl_state_set_state_bool(ctx->trace, GL_COLOR_WRITEMASK, p_index*4+0, p_r);
        trc_gl_state_set_state_bool(ctx->trace, GL_COLOR_WRITEMASK, p_index*4+1, p_g);
        trc_gl_state_set_state_bool(ctx->trace, GL_COLOR_WRITEMASK, p_index*4+2, p_b);
        trc_gl_state_set_state_bool(ctx->trace, GL_COLOR_WRITEMASK, p_index*4+3, p_a);
    }
    real(p_index, p_r, p_g, p_b, p_a);

glStencilMask: //GLuint mask
    union {int32_t maski; uint32_t masku;} u;
    u.masku = p_mask;
    trc_gl_state_set_state_int(ctx->trace, GL_STENCIL_WRITEMASK, 0, u.maski);
    trc_gl_state_set_state_int(ctx->trace, GL_STENCIL_BACK_WRITEMASK, 0, u.maski);
    real(p_mask);

glStencilMaskSeparate: //GLenum face, GLuint mask
    union {int32_t maski; uint32_t masku;} u;
    u.masku = p_mask;
    if (p_face==GL_FRONT || p_face==GL_FRONT_AND_BACK)
        trc_gl_state_set_state_int(ctx->trace, GL_STENCIL_WRITEMASK, 0, u.maski);
    if (p_face==GL_BACK || p_face==GL_FRONT_AND_BACK)
        trc_gl_state_set_state_int(ctx->trace, GL_STENCIL_BACK_WRITEMASK, 0, u.maski);
    real(p_face, p_mask);

glPixelStoref: //GLenum p_pname, GLfloat p_param
    replay_pixel_store(ctx, cmd, p_pname, p_param);

glPixelStorei: //GLenum p_pname, GLint p_param
    replay_pixel_store(ctx, cmd, p_pname, p_param);

glClearStencil: //GLint p_s
    //TODO: Mask p_s?
    trc_gl_state_set_state_int(ctx->trace, GL_STENCIL_CLEAR_VALUE, 0, p_s);
    real(p_s);

glClearColor: //GLfloat p_red, GLfloat p_green, GLfloat p_blue, GLfloat p_alpha
    trc_gl_state_set_state_float(ctx->trace, GL_COLOR_CLEAR_VALUE, 0, p_red);
    trc_gl_state_set_state_float(ctx->trace, GL_COLOR_CLEAR_VALUE, 1, p_green);
    trc_gl_state_set_state_float(ctx->trace, GL_COLOR_CLEAR_VALUE, 2, p_blue);
    trc_gl_state_set_state_float(ctx->trace, GL_COLOR_CLEAR_VALUE, 3, p_alpha);
    real(p_red, p_green, p_blue, p_alpha);

glClearDepth: //GLdouble p_depth
    trc_gl_state_set_state_float(ctx->trace, GL_DEPTH_CLEAR_VALUE, 0, fminf(fmaxf(p_depth, 0.0f), 1.0f));
    real(p_depth);

glClearDepthf: //GLfloat p_d
    trc_gl_state_set_state_float(ctx->trace, GL_DEPTH_CLEAR_VALUE, 0, fminf(fmaxf(p_d, 0.0f), 1.0f));
    real(p_d);

glDepthRange: //GLdouble p_near, GLdouble p_far
    for (int i = 0; i < trc_gl_state_get_state_int(ctx->trace, GL_MAX_VIEWPORTS, 0); i++) {
        trc_gl_state_set_state_float(ctx->trace, GL_DEPTH_RANGE, i*2+0, p_near);
        trc_gl_state_set_state_float(ctx->trace, GL_DEPTH_RANGE, i*2+1, p_far);
    }
    real(p_near, p_far);

glDepthRangef: //GLfloat p_n, GLfloat p_f
    for (int i = 0; i < trc_gl_state_get_state_int(ctx->trace, GL_MAX_VIEWPORTS, 0); i++) {
        trc_gl_state_set_state_float(ctx->trace, GL_DEPTH_RANGE, i*2+0, p_n);
        trc_gl_state_set_state_float(ctx->trace, GL_DEPTH_RANGE, i*2+1, p_f);
    }
    real(p_n, p_f);

glDepthRangeArrayv: //GLuint p_first, GLsizei p_count, const GLdouble* p_v
    int max_viewports = trc_gl_state_get_state_int(ctx->trace, GL_MAX_VIEWPORTS, 0);
    if (p_count<0 || p_first+p_count>max_viewports) ERROR("Invalid first and count");
    for (GLuint i = p_first*2; i < (p_first+p_count)*2; i++)
        trc_gl_state_set_state_float(ctx->trace, GL_DEPTH_RANGE, i, p_v[i]);
    real(p_first, p_count, p_v);

glPointSize: //GLfloat p_size
    if (p_size <= 0.0f) ERROR("size is not greater than zero");
    trc_gl_state_set_state_float(ctx->trace, GL_POINT_SIZE, 0, p_size);
    real(p_size);

glLineWidth: //GLfloat p_width
    if (p_width <= 0.0f) ERROR("width is not greater than zero");
    trc_gl_state_set_state_float(ctx->trace, GL_LINE_WIDTH, 0, p_width);
    real(p_width);

glPolygonOffset: //GLfloat p_factor, GLfloat p_units
    trc_gl_state_set_state_float(ctx->trace, GL_POLYGON_OFFSET_UNITS, 0, p_units);
    trc_gl_state_set_state_float(ctx->trace, GL_POLYGON_OFFSET_FACTOR, 0, p_factor);
    real(p_factor, p_units);

glSampleCoverage: //GLfloat p_value, GLboolean p_invert
    trc_gl_state_set_state_float(ctx->trace, GL_SAMPLE_COVERAGE_VALUE, 0, p_value);
    trc_gl_state_set_state_bool(ctx->trace, GL_SAMPLE_COVERAGE_INVERT, 0, p_invert);
    real(p_value, p_invert);

glStencilFunc: //GLenum p_func, GLint p_ref, GLuint p_mask
    union {int32_t maski; uint32_t masku;} u;
    u.masku = p_mask;
    trc_gl_state_set_state_enum(ctx->trace, GL_STENCIL_FUNC, 0, p_func);
    trc_gl_state_set_state_int(ctx->trace, GL_STENCIL_REF, 0, p_ref);
    trc_gl_state_set_state_int(ctx->trace, GL_STENCIL_VALUE_MASK, 0, u.maski);
    trc_gl_state_set_state_enum(ctx->trace, GL_STENCIL_BACK_FUNC, 0, p_func);
    trc_gl_state_set_state_int(ctx->trace, GL_STENCIL_BACK_REF, 0, p_ref);
    trc_gl_state_set_state_int(ctx->trace, GL_STENCIL_BACK_VALUE_MASK, 0, u.maski);
    real(p_func, p_ref, p_mask);

glStencilFuncSeparate: //GLenum p_face, GLenum p_func, GLint p_ref, GLuint p_mask
    union {int32_t maski; uint32_t masku;} u;
    u.masku = p_mask;
    if (p_face==GL_FRONT || p_face==GL_FRONT_AND_BACK) {
        trc_gl_state_set_state_enum(ctx->trace, GL_STENCIL_FUNC, 0, p_func);
        trc_gl_state_set_state_int(ctx->trace, GL_STENCIL_REF, 0, p_ref);
        trc_gl_state_set_state_int(ctx->trace, GL_STENCIL_VALUE_MASK, 0, u.maski);
    }
    if (p_face==GL_BACK || p_face==GL_FRONT_AND_BACK) {
        trc_gl_state_set_state_enum(ctx->trace, GL_STENCIL_BACK_FUNC, 0, p_func);
        trc_gl_state_set_state_int(ctx->trace, GL_STENCIL_BACK_REF, 0, p_ref);
        trc_gl_state_set_state_int(ctx->trace, GL_STENCIL_BACK_VALUE_MASK, 0, u.maski);
    }
    real(p_face, p_func, p_ref, p_mask);

glStencilOp: //GLenum p_fail, GLenum p_zfail, GLenum p_zpass
    trc_gl_state_set_state_enum(ctx->trace, GL_STENCIL_FAIL, 0, p_fail);
    trc_gl_state_set_state_enum(ctx->trace, GL_STENCIL_PASS_DEPTH_FAIL, 0, p_zfail);
    trc_gl_state_set_state_enum(ctx->trace, GL_STENCIL_PASS_DEPTH_PASS, 0, p_zpass);
    trc_gl_state_set_state_enum(ctx->trace, GL_STENCIL_BACK_FAIL, 0, p_fail);
    trc_gl_state_set_state_enum(ctx->trace, GL_STENCIL_BACK_PASS_DEPTH_FAIL, 0, p_zfail);
    trc_gl_state_set_state_enum(ctx->trace, GL_STENCIL_BACK_PASS_DEPTH_PASS, 0, p_zpass);
    real(p_fail, p_zfail, p_zpass);

glStencilOpSeparate: //GLenum p_face, GLenum p_sfail, GLenum p_dpfail, GLenum p_dppass
    if (p_face==GL_FRONT || p_face==GL_FRONT_AND_BACK) {
        trc_gl_state_set_state_enum(ctx->trace, GL_STENCIL_FAIL, 0, p_sfail);
        trc_gl_state_set_state_enum(ctx->trace, GL_STENCIL_PASS_DEPTH_FAIL, 0, p_dpfail);
        trc_gl_state_set_state_enum(ctx->trace, GL_STENCIL_PASS_DEPTH_PASS, 0, p_dppass);
    }
    if (p_face==GL_BACK || p_face==GL_FRONT_AND_BACK) {
        trc_gl_state_set_state_enum(ctx->trace, GL_STENCIL_BACK_FAIL, 0, p_sfail);
        trc_gl_state_set_state_enum(ctx->trace, GL_STENCIL_BACK_PASS_DEPTH_FAIL, 0, p_dpfail);
        trc_gl_state_set_state_enum(ctx->trace, GL_STENCIL_BACK_PASS_DEPTH_PASS, 0, p_dppass);
    }
    real(p_face, p_sfail, p_dpfail, p_dppass);

glBlendFunc: //GLenum p_sfactor, GLenum p_dfactor
    for (size_t i = 0; i < trc_gl_state_get_state_int(ctx->trace, GL_MAX_DRAW_BUFFERS, 0); i++) {
        trc_gl_state_set_state_enum(ctx->trace, GL_BLEND_SRC_RGB, i, p_sfactor);
        trc_gl_state_set_state_enum(ctx->trace, GL_BLEND_DST_RGB, i, p_dfactor);
        trc_gl_state_set_state_enum(ctx->trace, GL_BLEND_SRC_ALPHA, i, p_sfactor);
        trc_gl_state_set_state_enum(ctx->trace, GL_BLEND_DST_ALPHA, i, p_dfactor);
    }
    real(p_sfactor, p_dfactor);

glBlendFuncSeparate: //GLenum p_sfactorRGB, GLenum p_dfactorRGB, GLenum p_sfactorAlpha, GLenum p_dfactorAlpha
    for (size_t i = 0; i < trc_gl_state_get_state_int(ctx->trace, GL_MAX_DRAW_BUFFERS, 0); i++) {
        trc_gl_state_set_state_enum(ctx->trace, GL_BLEND_SRC_RGB, i, p_sfactorRGB);
        trc_gl_state_set_state_enum(ctx->trace, GL_BLEND_DST_RGB, i, p_dfactorRGB);
        trc_gl_state_set_state_enum(ctx->trace, GL_BLEND_SRC_ALPHA, i, p_sfactorAlpha);
        trc_gl_state_set_state_enum(ctx->trace, GL_BLEND_DST_ALPHA, i, p_dfactorAlpha);
    }
    real(p_sfactorRGB, p_dfactorRGB, p_sfactorAlpha, p_dfactorAlpha);

glBlendFunci: //GLuint p_buf, GLenum p_src, GLenum p_dst
    if (p_buf < trc_gl_state_get_state_int(ctx->trace, GL_MAX_DRAW_BUFFERS, 0)) {
        trc_gl_state_set_state_enum(ctx->trace, GL_BLEND_SRC_RGB, p_buf, p_src);
        trc_gl_state_set_state_enum(ctx->trace, GL_BLEND_DST_RGB, p_buf, p_dst);
        trc_gl_state_set_state_enum(ctx->trace, GL_BLEND_SRC_ALPHA, p_buf, p_src);
        trc_gl_state_set_state_enum(ctx->trace, GL_BLEND_DST_ALPHA, p_buf, p_dst);
    }
    real(p_buf, p_src, p_dst);

glBlendFuncSeparatei: //GLuint p_buf, GLenum p_srcRGB, GLenum p_dstRGB, GLenum p_srcAlpha, GLenum p_dstAlpha
    if (p_buf < trc_gl_state_get_state_int(ctx->trace, GL_MAX_DRAW_BUFFERS, 0)) {
        trc_gl_state_set_state_enum(ctx->trace, GL_BLEND_SRC_RGB, p_buf, p_srcRGB);
        trc_gl_state_set_state_enum(ctx->trace, GL_BLEND_DST_RGB, p_buf, p_dstRGB);
        trc_gl_state_set_state_enum(ctx->trace, GL_BLEND_SRC_ALPHA, p_buf, p_srcAlpha);
        trc_gl_state_set_state_enum(ctx->trace, GL_BLEND_DST_ALPHA, p_buf, p_dstAlpha);
    }
    real(p_buf, p_srcRGB, p_dstRGB, p_srcAlpha, p_dstAlpha);

glBlendColor: //GLfloat p_red, GLfloat p_green, GLfloat p_blue, GLfloat p_alpha
    trc_gl_state_set_state_float(ctx->trace, GL_BLEND_COLOR, 0, p_red);
    trc_gl_state_set_state_float(ctx->trace, GL_BLEND_COLOR, 1, p_green);
    trc_gl_state_set_state_float(ctx->trace, GL_BLEND_COLOR, 2, p_blue);
    trc_gl_state_set_state_float(ctx->trace, GL_BLEND_COLOR, 3, p_alpha);
    real(p_red, p_green, p_blue, p_alpha);

glBlendEquation: //GLenum p_mode
    for (size_t i = 0; i < trc_gl_state_get_state_int(ctx->trace, GL_MAX_DRAW_BUFFERS, 0); i++) {
        trc_gl_state_set_state_enum(ctx->trace, GL_BLEND_EQUATION_RGB, i, p_mode);
        trc_gl_state_set_state_enum(ctx->trace, GL_BLEND_EQUATION_ALPHA, i, p_mode);
    }
    real(p_mode);

glBlendEquationSeparate: //GLenum p_modeRGB, GLenum p_modeAlpha
    for (size_t i = 0; i < trc_gl_state_get_state_int(ctx->trace, GL_MAX_DRAW_BUFFERS, 0); i++) {
        trc_gl_state_set_state_enum(ctx->trace, GL_BLEND_EQUATION_RGB, i, p_modeRGB);
        trc_gl_state_set_state_enum(ctx->trace, GL_BLEND_EQUATION_ALPHA, i, p_modeAlpha);
    }
    real(p_modeRGB, p_modeAlpha);

glBlendEquationi: //GLuint p_buf, GLenum p_mode
    if (p_buf < trc_gl_state_get_state_int(ctx->trace, GL_MAX_DRAW_BUFFERS, 0)) {
        trc_gl_state_set_state_enum(ctx->trace, GL_BLEND_EQUATION_RGB, p_buf, p_mode);
        trc_gl_state_set_state_enum(ctx->trace, GL_BLEND_EQUATION_ALPHA, p_buf, p_mode);
    }
    real(p_buf, p_mode);

glBlendEquationSeparatei: //GLuint p_buf, GLenum p_modeRGB, GLenum p_modeAlpha
    if (p_buf < trc_gl_state_get_state_int(ctx->trace, GL_MAX_DRAW_BUFFERS, 0)) {
        trc_gl_state_set_state_enum(ctx->trace, GL_BLEND_EQUATION_RGB, p_buf, p_modeRGB);
        trc_gl_state_set_state_enum(ctx->trace, GL_BLEND_EQUATION_ALPHA, p_buf, p_modeAlpha);
    }
    real(p_buf, p_modeRGB, p_modeAlpha);

glViewport: //GLint p_x, GLint p_y, GLsizei p_width, GLsizei p_height
    trc_gl_state_set_state_float(ctx->trace, GL_VIEWPORT, 0, p_x);
    trc_gl_state_set_state_float(ctx->trace, GL_VIEWPORT, 1, p_y);
    trc_gl_state_set_state_float(ctx->trace, GL_VIEWPORT, 2, p_width);
    trc_gl_state_set_state_float(ctx->trace, GL_VIEWPORT, 3, p_height);
    real(p_x, p_y, p_width, p_height);

glViewportIndexedf: //GLuint p_index, GLfloat p_x, GLfloat p_y, GLfloat p_w, GLfloat p_h
    trc_gl_state_set_state_float(ctx->trace, GL_VIEWPORT, p_index*4+0, p_x);
    trc_gl_state_set_state_float(ctx->trace, GL_VIEWPORT, p_index*4+1, p_y);
    trc_gl_state_set_state_float(ctx->trace, GL_VIEWPORT, p_index*4+2, p_w);
    trc_gl_state_set_state_float(ctx->trace, GL_VIEWPORT, p_index*4+3, p_h);
    real(p_index, p_x, p_y, p_w, p_h);

glViewportArrayv: //GLuint p_first, GLsizei p_count, const GLfloat* p_v
    for (size_t i = p_first*4; i < (p_first+p_count)*4; i++)
        trc_gl_state_set_state_float(ctx->trace, GL_VIEWPORT, i, p_v[i]);
    real(p_first, p_count, p_v);

glViewportIndexedfv: //GLuint p_index, const GLfloat* p_v
    for (size_t i = 0; i < 4; i++)
        trc_gl_state_set_state_float(ctx->trace, GL_VIEWPORT, p_index*4+i, p_v[i]);
    real(p_index, p_v);

glScissor: //GLint p_x, GLint p_y, GLsizei p_width, GLsizei p_height
    trc_gl_state_set_state_int(ctx->trace, GL_SCISSOR_BOX, 0, p_x);
    trc_gl_state_set_state_int(ctx->trace, GL_SCISSOR_BOX, 1, p_y);
    trc_gl_state_set_state_int(ctx->trace, GL_SCISSOR_BOX, 2, p_width);
    trc_gl_state_set_state_int(ctx->trace, GL_SCISSOR_BOX, 3, p_height);
    real(p_x, p_y, p_width, p_height);

glScissorIndexed: //GLuint p_index, GLint p_left, GLint p_bottom, GLsizei p_width, GLsizei p_height
    trc_gl_state_set_state_int(ctx->trace, GL_SCISSOR_BOX, p_index*4+0, p_left);
    trc_gl_state_set_state_int(ctx->trace, GL_SCISSOR_BOX, p_index*4+1, p_bottom);
    trc_gl_state_set_state_int(ctx->trace, GL_SCISSOR_BOX, p_index*4+2, p_width);
    trc_gl_state_set_state_int(ctx->trace, GL_SCISSOR_BOX, p_index*4+3, p_height);
    real(p_index, p_left, p_bottom, p_width, p_height);

glScissorArrayv: //GLuint p_first, GLsizei p_count, const GLint* p_v
    for (size_t i = p_first*4; i < (p_first+p_count)*4; i++)
        trc_gl_state_set_state_float(ctx->trace, GL_SCISSOR_BOX, i, p_v[i]);
    real(p_first, p_count, p_v);

glScissorIndexedv: //GLuint p_index, const GLint* p_v
    for (size_t i = 0; i < 4; i++)
        trc_gl_state_set_state_int(ctx->trace, GL_SCISSOR_BOX, p_index*4+i, p_v[i]);
    real(p_index, p_v);

glHint: //GLenum p_target, GLenum p_mode
    trc_gl_state_set_hints(ctx->trace, p_target, p_mode);
    real(p_target, p_mode);

glProvokingVertex: //GLenum p_mode
    trc_gl_state_set_state_enum(ctx->trace, GL_PROVOKING_VERTEX, 0, p_mode);
    real(p_mode);

glLogicOp: //GLenum p_opcode
    trc_gl_state_set_state_enum(ctx->trace, GL_LOGIC_OP_MODE, 0, p_opcode);
    real(p_opcode);

glPrimitiveRestartIndex: //GLuint p_index
    trc_gl_state_set_state_int(ctx->trace, GL_PRIMITIVE_RESTART_INDEX, 0, p_index);
    real(p_index);

glPolygonMode: //GLenum p_face, GLenum p_mode
    trc_gl_state_set_state_enum(ctx->trace, GL_POLYGON_MODE, 0, p_mode);
    real(p_face, p_mode);

glCullFace: //GLenum p_mode
    trc_gl_state_set_state_enum(ctx->trace, GL_CULL_FACE_MODE, 0, p_mode);
    real(p_mode);

glFrontFace: //GLenum p_mode
    trc_gl_state_set_state_enum(ctx->trace, GL_FRONT_FACE, 0, p_mode);
    real(p_mode);

glDepthFunc: //GLenum p_func
    trc_gl_state_set_state_enum(ctx->trace, GL_DEPTH_FUNC, 0, p_func);
    real(p_func);

glClampColor: //GLenum p_target, GLenum p_clamp
    real(p_target, p_clamp);

glPointParameterf: //GLenum p_pname, GLfloat p_param
    switch (p_pname) {
    case GL_POINT_FADE_THRESHOLD_SIZE: trc_gl_state_set_state_float(ctx->trace, p_pname, 0, p_param);
    case GL_POINT_SPRITE_COORD_ORIGIN: trc_gl_state_set_state_enum(ctx->trace, p_pname, 0, p_param);
    }
    F(glPointParameterf)(p_pname, p_param);

glPointParameteri: //GLenum p_pname, GLint p_param
    switch (p_pname) {
    case GL_POINT_FADE_THRESHOLD_SIZE: trc_gl_state_set_state_float(ctx->trace, p_pname, 0, p_param);
    case GL_POINT_SPRITE_COORD_ORIGIN: trc_gl_state_set_state_enum(ctx->trace, p_pname, 0, p_param);
    }
    F(glPointParameteri)(p_pname, p_param);

glPointParameterfv: //GLenum p_pname, const GLfloat* p_params
    switch (p_pname) {
    case GL_POINT_FADE_THRESHOLD_SIZE: trc_gl_state_set_state_float(ctx->trace, p_pname, 0, p_params[0]);
    case GL_POINT_SPRITE_COORD_ORIGIN: trc_gl_state_set_state_enum(ctx->trace, p_pname, 0, p_params[0]);
    }
    real(p_pname, p_params);

glPointParameteriv: //GLenum p_pname, const GLint* p_params
    switch (p_pname) {
    case GL_POINT_FADE_THRESHOLD_SIZE: trc_gl_state_set_state_float(ctx->trace, p_pname, 0, p_params[0]);
    case GL_POINT_SPRITE_COORD_ORIGIN: trc_gl_state_set_state_enum(ctx->trace, p_pname, 0, p_params[0]);
    }
    real(p_pname, p_params);

glMinSampleShading: //GLfloat p_value
    trc_gl_state_set_state_float(ctx->trace, GL_MIN_SAMPLE_SHADING_VALUE, 0, p_value);
    real(p_value);

glDebugMessageCallback: //GLDEBUGPROC p_callback, const void* p_userParam
    ;

glPatchParameteri: //GLenum p_pname, GLint p_value
    if (p_value<=0 || p_value>trc_gl_state_get_state_int(ctx->trace, GL_MAX_PATCH_VERTICES, 0))
        ERROR("Invalid value");
    real(p_pname, p_value);

glPatchParameterfv: //GLenum p_pname, const GLfloat* p_values
    real(p_pname, p_values);

glGetUniformSubroutineuiv: //GLenum p_shadertype, GLint p_location, GLuint* p_params
    ;

glGetSubroutineIndex: //GLuint p_program, GLenum p_shadertype, const GLchar* p_name
    ;

glGetActiveSubroutineName: //GLuint p_program, GLenum p_shadertype, GLuint p_index, GLsizei p_bufsize, GLsizei* p_length, GLchar* p_name
    ;

glGetProgramStageiv: //GLuint p_program, GLenum p_shadertype, GLenum p_pname, GLint* p_values
    ;

glGetActiveSubroutineUniformName: //GLuint p_program, GLenum p_shadertype, GLuint p_index, GLsizei p_bufsize, GLsizei* p_length, GLchar* p_name
    ;

glGetSubroutineUniformLocation: //GLuint p_program, GLenum p_shadertype, const GLchar* p_name
    ;

glGetQueryIndexediv: //GLenum p_target, GLuint p_index, GLenum p_pname, GLint* p_params
    ;

//TODO: More validation
#define VALIDATE_BLIT_FRAMEBUFFER\
    if (((p_mask&GL_DEPTH_BUFFER_BIT)||(p_mask&GL_STENCIL_BUFFER_BIT)) && p_filter!=GL_NEAREST)\
        ERROR("Invalid filter for mask");

glBlitFramebuffer: //GLint p_srcX0, GLint p_srcY0, GLint p_srcX1, GLint p_srcY1, GLint p_dstX0, GLint p_dstY0, GLint p_dstX1, GLint p_dstY1, GLbitfield p_mask, GLenum p_filter
    real(p_srcX0, p_srcY0, p_srcX1, p_srcY1, p_dstX0, p_dstY0, p_dstX1, p_dstY1, p_mask, p_filter);
    VALIDATE_BLIT_FRAMEBUFFER
    update_buffers(ctx, trc_gl_state_get_draw_framebuffer(ctx->trace), p_mask);

glBlitNamedFramebuffer: //GLuint p_readFramebuffer, GLuint p_drawFramebuffer, GLint p_srcX0, GLint p_srcY0, GLint p_srcX1, GLint p_srcY1, GLint p_dstX0, GLint p_dstY0, GLint p_dstX1, GLint p_dstY1, GLbitfield p_mask, GLenum p_filter
    if (!p_readFramebuffer_rev || !p_drawFramebuffer_rev) ERROR("Invalid framebuffer name");
    if (!p_readFramebuffer_rev->has_object || !p_drawFramebuffer_rev->has_object)
        ERROR("Framebuffer name has no object");
    VALIDATE_BLIT_FRAMEBUFFER
    real(p_readFramebuffer_rev->real, p_drawFramebuffer_rev->real, p_srcX0, p_srcY0, p_srcX1, p_srcY1, p_dstX0, p_dstY0, p_dstX1, p_dstY1, p_mask, p_filter);
    update_buffers(ctx, p_drawFramebuffer, p_mask);
