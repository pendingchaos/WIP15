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
    replay_update_buffers(ctx, false, true, false, false);

glSetContextCapsWIP15: //
    ;

glClear: //GLbitfield p_mask
    real(p_mask);
    bool color = p_mask & GL_COLOR_BUFFER_BIT;
    bool depth = p_mask & GL_DEPTH_BUFFER_BIT;
    bool stencil = p_mask & GL_STENCIL_BUFFER_BIT;
    replay_update_buffers(ctx, color, false, depth, stencil);

glGenTextures: //GLsizei p_n, GLuint* p_textures
    GLuint textures[p_n];
    real(p_n, textures);
    
    trc_gl_texture_rev_t rev;
    rev.fake_context = trc_get_current_fake_gl_context(ctx->trace);
    rev.ref_count = 1;
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
    rev.created = false;
    rev.type = 0;
    rev.images = NULL;
    for (size_t i = 0; i < p_n; ++i) {
        rev.real = textures[i];
        trc_set_gl_texture(ctx->trace, p_textures[i], &rev);
    }

glDeleteTextures: //GLsizei p_n, const GLuint* p_textures
    GLuint textures[p_n];
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
    GLuint real_tex = trc_get_real_gl_texture(ctx->trace, p_texture);
    const trc_gl_texture_rev_t* rev = trc_get_gl_texture(ctx->trace, p_texture);
    if (!rev && p_texture) ERROR("Invalid texture name");
    if (rev && !rev->created) {
        trc_gl_texture_rev_t newrev = *rev;
        newrev.created = true;
        newrev.type = p_target;
        newrev.images = trc_create_data(ctx->trace, 0, NULL, TRC_DATA_IMMUTABLE);
        trc_set_gl_texture(ctx->trace, p_texture, &newrev);
    } else if (rev && rev->type!=p_target)
        ERROR("Invalid target for texture");
    //TODO: Reference counting
    real(p_target, real_tex);
    uint unit = trc_gl_state_get_active_texture_unit(ctx->trace);
    trc_gl_state_set_bound_textures(ctx->trace, p_target, unit, p_texture);

glTexImage1D: //GLenum p_target, GLint p_level, GLint p_internalformat, GLsizei p_width, GLint p_border, GLenum p_format, GLenum p_type, const void* p_pixels
    real(p_target, p_level, p_internalformat, p_width, p_border, p_format, p_type, p_pixels);
    replay_update_bound_tex_image(ctx, cmd, p_target, p_level);

glCompressedTexImage1D: //GLenum p_target, GLint p_level, GLenum p_internalformat, GLsizei p_width, GLint p_border, GLsizei p_imageSize, const void* p_data
    real(p_target, p_level, p_internalformat, p_width, p_border, p_imageSize, p_data);
    replay_update_bound_tex_image(ctx, cmd, p_target, p_level);

glTexSubImage1D: //GLenum p_target, GLint p_level, GLint p_xoffset, GLsizei p_width, GLenum p_format, GLenum p_type, const void* p_pixels
    real(p_target, p_level, p_xoffset, p_width, p_format, p_type, p_pixels);
    replay_update_bound_tex_image(ctx, cmd, p_target, p_level);

glCompressedTexSubImage1D: //GLenum p_target, GLint p_level, GLint p_xoffset, GLsizei p_width, GLenum p_format, GLsizei p_imageSize, const void* p_data
    real(p_target, p_level, p_xoffset, p_width, p_format, p_imageSize, p_data);
    replay_update_bound_tex_image(ctx, cmd, p_target, p_level);

glTexImage2D: //GLenum p_target, GLint p_level, GLint p_internalformat, GLsizei p_width, GLsizei p_height, GLint p_border, GLenum p_format, GLenum p_type, const void* p_pixels
    real(p_target, p_level, p_internalformat, p_width, p_height, p_border, p_format, p_type, p_pixels);
    replay_update_bound_tex_image(ctx, cmd, p_target, p_level);

glCompressedTexImage2D: //GLenum p_target, GLint p_level, GLenum p_internalformat, GLsizei p_width, GLsizei p_height, GLint p_border, GLsizei p_imageSize, const void* p_data
    real(p_target, p_level, p_internalformat, p_width, p_height, p_border, p_imageSize, p_data);
    replay_update_bound_tex_image(ctx, cmd, p_target, p_level);

glTexSubImage2D: //GLenum p_target, GLint p_level, GLint p_xoffset, GLint p_yoffset, GLsizei p_width, GLsizei p_height, GLenum p_format, GLenum p_type, const void* p_pixels
    real(p_target, p_level, p_xoffset, p_yoffset, p_width, p_height, p_format, p_type, p_pixels);
    replay_update_bound_tex_image(ctx, cmd, p_target, p_level);

glCompressedTexSubImage2D: //GLenum p_target, GLint p_level, GLint p_xoffset, GLint p_yoffset, GLsizei p_width, GLsizei p_height, GLenum p_format, GLsizei p_imageSize, const void* p_data
    real(p_target, p_level, p_xoffset, p_yoffset, p_width, p_height, p_format, p_imageSize, p_data);
    replay_update_bound_tex_image(ctx, cmd, p_target, p_level);

glTexImage3D: //GLenum p_target, GLint p_level, GLint p_internalformat, GLsizei p_width, GLsizei p_height, GLsizei p_depth, GLint p_border, GLenum p_format, GLenum p_type, const void* p_pixels
    //TODO: Array textures
    real(p_target, p_level, p_internalformat, p_width, p_height, p_depth, p_border, p_format, p_type, p_pixels);
    replay_update_bound_tex_image(ctx, cmd, p_target, p_level);

glCompressedTexImage3D: //GLenum p_target, GLint p_level, GLenum p_internalformat, GLsizei p_width, GLsizei p_height, GLsizei p_depth, GLint p_border, GLsizei p_imageSize, const void* p_data
    //TODO: Array textures
    real(p_target, p_level, p_internalformat, p_width, p_height, p_depth, p_border, p_imageSize, p_data);
    replay_update_bound_tex_image(ctx, cmd, p_target, p_level);

glTexSubImage3D: //GLenum p_target, GLint p_level, GLint p_xoffset, GLint p_yoffset, GLint p_zoffset, GLsizei p_width, GLsizei p_height, GLsizei p_depth, GLenum p_format, GLenum p_type, const void* p_pixels
    real(p_target, p_level, p_xoffset, p_yoffset, p_zoffset, p_width, p_height, p_depth, p_format, p_type, p_pixels);
    replay_update_bound_tex_image(ctx, cmd, p_target, p_level);

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

glGenerateMipmap: //GLenum p_target
    real(p_target);
    
    uint unit = trc_gl_state_get_active_texture_unit(ctx->trace);
    uint fake = trc_gl_state_get_bound_textures(ctx->trace, p_target, unit);
    if (!fake) ERROR("No texture bound or invalid target");
    
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
    if (!texture_param_double(ctx, cmd, p_target, p_pname, 1, &double_param))
        real(p_target, p_pname, p_param);

glTexParameteri: //GLenum p_target, GLenum p_pname, GLint p_param
    GLdouble double_param = p_param;
    if (!texture_param_double(ctx, cmd, p_target, p_pname, 1, &double_param))
        real(p_target, p_pname, p_param);

glTexParameterfv: //GLenum p_target, GLenum p_pname, const GLfloat* p_params
    trace_value_t* paramsv = trc_get_arg(cmd, 2);
    const double* paramsd = trc_get_double(paramsv);
    GLfloat params[paramsv->count];
    for (size_t i = 0; i < paramsv->count; i++) params[i] = paramsd[i];
    if (!texture_param_double(ctx, cmd, p_target, p_pname, paramsv->count, paramsd))
        real(p_target, p_pname, params);

glTexParameteriv: //GLenum p_target, GLenum p_pname, const GLint* p_params
    trace_value_t* paramsv = trc_get_arg(cmd, 2);
    const int64_t* params64 = trc_get_int(paramsv);
    GLint params[paramsv->count];
    double double_params[paramsv->count];
    for (size_t i = 0; i < paramsv->count; i++) params[i] = params64[i];
    for (size_t i = 0; i < paramsv->count; i++) double_params[i] = params64[i];
    
    if (!texture_param_double(ctx, cmd, p_target, p_pname, paramsv->count, double_params))
        real(p_target, p_pname, params);

glTexParameterIiv: //GLenum p_target, GLenum p_pname, const GLint* p_params
    //TODO
    const int64_t* params64 = trc_get_int(trc_get_arg(cmd, 2));
    GLint params[4];
    if (p_pname==GL_TEXTURE_BORDER_COLOR || p_pname==GL_TEXTURE_SWIZZLE_RGBA)
        for (size_t i = 0; i < 4; i++) params[i] = params64[i];
    else params[0] = params64[0];
    
    real(p_target, p_pname, params);

glTexParameterIuiv: //GLenum p_target, GLenum p_pname, const GLuint* p_params
    //TODO
    const uint64_t* params64 = trc_get_uint(trc_get_arg(cmd, 2));
    GLuint params[4];
    if (p_pname==GL_TEXTURE_BORDER_COLOR || p_pname==GL_TEXTURE_SWIZZLE_RGBA)
        for (size_t i = 0; i < 4; i++) params[i] = params64[i];
    else params[0] = params64[0];
    
    real(p_target, p_pname, params);

glGenBuffers: //GLsizei p_n, GLuint* p_buffers
    GLuint buffers[p_n];
    real(p_n, buffers);
    
    trc_gl_buffer_rev_t rev;
    rev.fake_context = trc_get_current_fake_gl_context(ctx->trace);
    rev.ref_count = 1;
    rev.has_data = false;
    rev.data_usage = 0;
    rev.data = NULL;
    rev.mapped = false;
    rev.map_offset = 0;
    rev.map_length = 0;
    rev.map_access = 0;
    for (size_t i = 0; i < p_n; ++i) {
        rev.real = buffers[i];
        trc_set_gl_buffer(ctx->trace, p_buffers[i], &rev);
    }

glDeleteBuffers: //GLsizei p_n, const GLuint* p_buffers
    GLuint buffers[p_n];
    for (size_t i = 0; i < p_n; ++i) {
        if (!(buffers[i] = trc_get_real_gl_buffer(ctx->trace, p_buffers[i])))
            trc_add_error(cmd, "Invalid buffer name");
        else trc_rel_gl_obj(ctx->trace, p_buffers[i], TrcGLObj_Buffer);
    }
    real(p_n, buffers);

glBindBuffer: //GLenum p_target, GLuint p_buffer
    GLuint real_buf = trc_get_real_gl_buffer(ctx->trace, p_buffer);
    if (!real_buf && p_buffer) ERROR("Invalid buffer name");
    trc_gl_state_set_bound_buffer(ctx->trace, p_target, p_buffer);
    real(p_target, real_buf);

glBindBufferBase: //GLenum p_target, GLuint p_index, GLuint p_buffer
    GLuint buf = trc_get_real_gl_buffer(ctx->trace, p_buffer);
    if (!buf && p_buffer) ERROR("Invalid buffer name");
    real(p_target, p_index, buf);

glBindBufferRange: //GLenum p_target, GLuint p_index, GLuint p_buffer, GLintptr p_offset, GLsizeiptr p_size
    GLuint buf = trc_get_real_gl_buffer(ctx->trace, p_buffer);
    if (!buf && p_buffer) ERROR("Invalid buffer name");
    real(p_target, p_index, buf, p_offset, p_size);

glBufferData: //GLenum p_target, GLsizeiptr p_size, const void * p_data, GLenum p_usage
    real(p_target, p_size, p_data, p_usage);
    
    uint fake = get_bound_buffer(ctx, p_target);
    const trc_gl_buffer_rev_t* buf_rev_ptr = trc_get_gl_buffer(ctx->trace, fake);
    if (!buf_rev_ptr) ERROR("Invalid buffer name or buffer target");
    trc_gl_buffer_rev_t buf = *buf_rev_ptr;
    buf.data = trc_create_data(ctx->trace, p_size, p_data, TRC_DATA_IMMUTABLE);
    trc_set_gl_buffer(ctx->trace, fake, &buf);

glBufferSubData: //GLenum p_target, GLintptr p_offset, GLsizeiptr p_size, const void * p_data
    real(p_target, p_offset, p_size, p_data);
    
    uint fake = get_bound_buffer(ctx, p_target);
    if (!fake) ERROR("No buffer bound to target");
    const trc_gl_buffer_rev_t* buf_rev_ptr = trc_get_gl_buffer(ctx->trace, fake);
    trc_gl_buffer_rev_t buf = *buf_rev_ptr;
    if (!buf.data) ERROR("Buffer has no data");
    
    trc_gl_buffer_rev_t old = buf;
    
    buf.data = trc_create_data(ctx->trace, old.data->size, NULL, TRC_DATA_NO_ZERO);
    void* newdata = trc_map_data(buf.data, TRC_MAP_REPLACE);
    
    void* olddata = trc_map_data(old.data, TRC_MAP_READ);
    memcpy(newdata, olddata, old.data->size);
    trc_unmap_data(old.data);
    
    memcpy((uint8_t*)newdata+p_offset, p_data, p_size);
    trc_unmap_freeze_data(ctx->trace, buf.data);
    
    trc_set_gl_buffer(ctx->trace, fake, &buf);

glCopyBufferSubData: //GLenum p_readTarget, GLenum p_writeTarget, GLintptr p_readOffset, GLintptr p_writeOffset, GLsizeiptr p_size
    real(p_readTarget, p_writeTarget, p_readOffset, p_writeOffset, p_size);
    
    uint read_fake = get_bound_buffer(ctx, p_readTarget);
    if (!read_fake) ERROR("No buffer bound to read target");
    const trc_gl_buffer_rev_t* read_buf = trc_get_gl_buffer(ctx->trace, read_fake);
    if (!read_buf->data) ERROR("Read buffer has no data");
    
    uint write_fake = get_bound_buffer(ctx, p_writeTarget);
    if (!write_fake) ERROR("No buffer bound to write target");
    const trc_gl_buffer_rev_t* write_buf = trc_get_gl_buffer(ctx->trace, write_fake);
    if (!write_buf->data) ERROR("Write buffer has no data");
    
    trc_gl_buffer_rev_t res;
    
    res.data = trc_create_data(ctx->trace, write_buf->data->size, trc_map_data(write_buf->data, TRC_MAP_READ), TRC_DATA_NO_ZERO);
    trc_unmap_data(write_buf->data);
    
    void* newdata = trc_map_data(res.data, TRC_MAP_REPLACE);
    uint8_t* readdata = trc_map_data(read_buf->data, TRC_MAP_READ);
    memcpy(newdata+p_writeOffset, readdata+p_readOffset, p_size);
    trc_unmap_data(read_buf->data);
    trc_unmap_freeze_data(ctx->trace, res.data);
    
    trc_set_gl_buffer(ctx->trace, write_fake, &res);

glMapBuffer: //GLenum p_target, GLenum p_access
    if (p_access!=GL_READ_ONLY && p_access!=GL_WRITE_ONLY && p_access!=GL_READ_WRITE) 
        ERROR("Invalid access policy");
    
    uint fake = get_bound_buffer(ctx, p_target);
    const trc_gl_buffer_rev_t* buf_rev_ptr = trc_get_gl_buffer(ctx->trace, fake);
    if (!buf_rev_ptr) ERROR("Invalid buffer name or buffer target");
    trc_gl_buffer_rev_t buf = *buf_rev_ptr;
    if (!buf.data) ERROR("Buffer has no data");
    
    if (buf.mapped) ERROR("Buffer is already mapped");
    
    buf.mapped = true;
    buf.map_offset = 0;
    buf.map_length = buf.data->size;
    switch (p_access) {
    case GL_READ_ONLY: buf.map_access = GL_MAP_READ_BIT; break;
    case GL_WRITE_ONLY: buf.map_access = GL_MAP_WRITE_BIT; break;
    case GL_READ_WRITE: buf.map_access = GL_MAP_READ_BIT | GL_MAP_WRITE_BIT; break;
    }
    
    trc_set_gl_buffer(ctx->trace, fake, &buf);

glMapBufferRange: //GLenum p_target, GLintptr p_offset, GLsizeiptr p_length, GLbitfield p_access
    if (p_offset<0 || p_length<0 || !p_length) ERROR("Invalid length or offset");
    
    if (!(p_access&GL_MAP_READ_BIT) && !(p_access&GL_MAP_WRITE_BIT))
        ERROR("Neither GL_MAP_READ_BIT or GL_MAP_WRITE_BIT is set");
    
    if (p_access&GL_MAP_READ_BIT && (p_access&GL_MAP_INVALIDATE_RANGE_BIT ||
                                     p_access&GL_MAP_INVALIDATE_BUFFER_BIT ||
                                     p_access&GL_MAP_UNSYNCHRONIZED_BIT))
        ERROR("GL_MAP_READ_BIT is set and GL_MAP_INVALIDATE_RANGE_BIT, GL_MAP_INVALIDATE_BUFFER_BIT or GL_MAP_UNSYNCHRONIZED_BIT is set");
    
    if (p_access&GL_MAP_FLUSH_EXPLICIT_BIT && !(p_access&GL_MAP_WRITE_BIT))
        ERROR("GL_MAP_FLUSH_EXPLICIT_BIT is set but GL_MAP_WRITE_BIT is not");
    
    if (p_access&!(GLbitfield)0xff) ERROR("Invalid access flags");
    
    //TODO:
    //Make sure the access is valid with the buffer's storage flags
    
    uint fake = get_bound_buffer(ctx, p_target);
    const trc_gl_buffer_rev_t* buf_rev_ptr = trc_get_gl_buffer(ctx->trace, fake);
    if (!buf_rev_ptr) ERROR("Invalid buffer name or buffer target");
    trc_gl_buffer_rev_t buf = *buf_rev_ptr;
    if (!buf.data) ERROR("Buffer has no data");
    
    if (p_offset+p_length > buf.data->size)
        ERROR("offset+length is greater than the buffer's size");
    if (buf.mapped) ERROR("Buffer is already mapped");
    
    buf.mapped = true;
    buf.map_offset = p_offset;
    buf.map_length = p_length;
    buf.map_access = p_access;
    
    trc_set_gl_buffer(ctx->trace, fake, &buf);

glUnmapBuffer: //GLenum p_target
    trace_extra_t* extra = trc_get_extra(cmd, "replay/glUnmapBuffer/data");
    if (!extra) ERROR("replay/glUnmapBuffer/data extra not found");
    
    uint fake = get_bound_buffer(ctx, p_target);
    const trc_gl_buffer_rev_t* buf_rev_ptr = trc_get_gl_buffer(ctx->trace, fake);
    if (!buf_rev_ptr) ERROR("Invalid buffer name or buffer target");
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
            uniform_blocks = realloc(uniform_blocks, (uniform_block_count+1)*sizeof(uint)*2);
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
    GLuint pipelines[p_n];
    real(p_n, pipelines);
    
    trc_gl_program_pipeline_rev_t rev;
    rev.fake_context = trc_get_current_fake_gl_context(ctx->trace);
    rev.ref_count = 1;
    for (size_t i = 0; i < p_n; ++i) {
        rev.real = pipelines[i];
        trc_set_gl_program_pipeline(ctx->trace, p_pipelines[i], &rev);
    }

glDeleteProgramPipelines: //GLsizei p_n, const GLuint* p_pipelines
    GLuint pipelines[p_n];
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
    ;

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
    if (!real_prog) trc_add_error(cmd, "Invalid program name");

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

glGetBufferParameteriv: //GLenum p_target, GLenum p_pname, GLint* p_params
    ;

glGetBufferParameteri64v: //GLenum p_target, GLenum p_pname, GLint64* p_params
    ;

glGetBufferPointerv: //GLenum p_target, GLenum p_pname, void ** p_params
    ;

glGetBufferSubData: //GLenum p_target, GLintptr p_offset, GLsizeiptr p_size, void * p_data
    ;

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
        trc_add_error(cmd, "Invalid sampler name");

glGetSamplerParameterIuiv: //GLuint p_sampler, GLenum p_pname, GLuint* p_params
    if (!trc_get_real_gl_sampler(ctx->trace, p_sampler))
        ERROR("Invalid sampler name");

glUniformBlockBinding: //GLuint p_program, GLuint p_uniformBlockIndex, GLuint p_uniformBlockBinding
    const trc_gl_program_rev_t* rev = trc_get_gl_program(ctx->trace, p_program);
    if (!rev) ERROR("Invalid program name");
    if (p_uniformBlockBinding >= trc_gl_state_get_state_int(ctx->trace, GL_MAX_UNIFORM_BUFFER_BINDINGS, 0))
        ERROR("Invalid binding");
    uint uniform_block_count = rev->uniform_blocks->size / sizeof(trc_gl_program_uniform_block_t);
    trc_gl_program_uniform_block_t* blocks = trc_map_data(rev->uniform_blocks, TRC_MAP_READ);
    for (uint i = 0; i < uniform_block_count; i++) {
        if (blocks[i].fake == p_uniformBlockIndex) {
            real(p_program, blocks[i].real, p_uniformBlockBinding);
            goto success;
        }
    }
    trc_add_error(cmd, "No such uniform block");
    success:
    trc_unmap_data(rev->uniform_blocks);

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
    GLfloat values[p_count];
    GLint loc;
    if ((loc=uniform(ctx, cmd, false, true, 1, 1, GL_FLOAT, values, NULL))<0) RETURN;
    real(loc, p_count, values);

glUniform2fv: //GLint p_location, GLsizei p_count, const GLfloat* p_value
    GLfloat values[p_count*2];
    GLint loc;
    if ((loc=uniform(ctx, cmd, false, true, 2, 1, GL_FLOAT, values, NULL))<0) RETURN;
    real(loc, p_count, values);

glUniform3fv: //GLint p_location, GLsizei p_count, const GLfloat* p_value
    GLfloat values[p_count*3];
    GLint loc;
    if ((loc=uniform(ctx, cmd, false, true, 3, 1, GL_FLOAT, values, NULL))<0) RETURN;
    real(loc, p_count, values);

glUniform4fv: //GLint p_location, GLsizei p_count, const GLfloat* p_value
    GLfloat values[p_count*4];
    GLint loc;
    if ((loc=uniform(ctx, cmd, false, true, 4, 1, GL_FLOAT, values, NULL))<0) RETURN;
    real(loc, p_count, values);

glUniform1iv: //GLint p_location, GLsizei p_count, const GLint* p_value
    GLint values[p_count];
    GLint loc;
    if ((loc=uniform(ctx, cmd, false, true, 1, 1, GL_INT, values, NULL))<0) RETURN;
    real(loc, p_count, values);

glUniform2iv: //GLint p_location, GLsizei p_count, const GLint* p_value
    GLint values[p_count*2];
    GLint loc;
    if ((loc=uniform(ctx, cmd, false, true, 2, 1, GL_INT, values, NULL))<0) RETURN;
    real(loc, p_count, values);

glUniform3iv: //GLint p_location, GLsizei p_count, const GLint* p_value
    GLint values[p_count*3];
    GLint loc;
    if ((loc=uniform(ctx, cmd, false, true, 3, 1, GL_INT, values, NULL))<0) RETURN;
    real(loc, p_count, values);

glUniform4iv: //GLint p_location, GLsizei p_count, const GLint* p_value
    GLint values[p_count*4];
    GLint loc;
    if ((loc=uniform(ctx, cmd, false, true, 4, 1, GL_INT, values, NULL))<0) RETURN;
    real(loc, p_count, values);

glUniform1uiv: //GLint p_location, GLsizei p_count, const GLuint* p_value
    GLuint values[p_count];
    GLint loc;
    if ((loc=uniform(ctx, cmd, false, true, 1, 1, GL_UNSIGNED_INT, values, NULL))<0) RETURN;
    real(loc, p_count, values);

glUniform2uiv: //GLint p_location, GLsizei p_count, const GLuint* p_value
    GLuint values[p_count*2];
    GLint loc;
    if ((loc=uniform(ctx, cmd, false, true, 2, 1, GL_UNSIGNED_INT, values, NULL))<0) RETURN;
    real(loc, p_count, values);

glUniform3uiv: //GLint p_location, GLsizei p_count, const GLuint* p_value
    GLuint values[p_count*3];
    GLint loc;
    if ((loc=uniform(ctx, cmd, false, true, 3, 1, GL_UNSIGNED_INT, values, NULL))<0) RETURN;
    real(loc, p_count, values);

glUniform4uiv: //GLint p_location, GLsizei p_count, const GLuint* p_value
    GLuint values[p_count*4];
    GLint loc;
    if ((loc=uniform(ctx, cmd, false, true, 4, 1, GL_UNSIGNED_INT, values, NULL))<0) RETURN;
    real(loc, p_count, values);

glUniform1dv: //GLint p_location, GLsizei p_count, const GLdouble* p_value
    GLdouble values[p_count];
    GLint loc;
    if ((loc=uniform(ctx, cmd, false, true, 1, 1, GL_DOUBLE, values, NULL))<0) RETURN;
    real(loc, p_count, values);

glUniform2dv: //GLint p_location, GLsizei p_count, const GLdouble* p_value
    GLdouble values[p_count*2];
    GLint loc;
    if ((loc=uniform(ctx, cmd, false, true, 2, 1, GL_DOUBLE, values, NULL))<0) RETURN;
    real(loc, p_count, values);

glUniform3dv: //GLint p_location, GLsizei p_count, const GLdouble* p_value
    GLdouble values[p_count*3];
    GLint loc;
    if ((loc=uniform(ctx, cmd, false, true, 3, 1, GL_UNSIGNED_INT, values, NULL))<0) RETURN;
    real(loc, p_count, values);

glUniform4dv: //GLint p_location, GLsizei p_count, const GLdouble* p_value
    GLdouble values[p_count*4];
    GLint loc;
    if ((loc=uniform(ctx, cmd, false, true, 4, 1, GL_UNSIGNED_INT, values, NULL))<0) RETURN;
    real(loc, p_count, values);

glUniformMatrix2fv: //GLint p_location, GLsizei p_count, GLboolean p_transpose, const GLfloat* p_value
    GLfloat values[p_count*4];
    GLint loc;
    if ((loc=uniform(ctx, cmd, false, true, 2, 2, GL_FLOAT, values, NULL))<0) RETURN;
    real(loc, p_count, p_transpose, values);

glUniformMatrix3fv: //GLint p_location, GLsizei p_count, GLboolean p_transpose, const GLfloat* p_value
    GLfloat values[p_count*9];
    GLint loc;
    if ((loc=uniform(ctx, cmd, false, true, 3, 3, GL_FLOAT, values, NULL))<0) RETURN;
    real(loc, p_count, p_transpose, values);

glUniformMatrix4fv: //GLint p_location, GLsizei p_count, GLboolean p_transpose, const GLfloat* p_value
    GLfloat values[p_count*16];
    GLint loc;
    if ((loc=uniform(ctx, cmd, false, true, 4, 4, GL_FLOAT, values, NULL))<0) RETURN;
    real(loc, p_count, p_transpose, values);

glUniformMatrix2x3fv: //GLint p_location, GLsizei p_count, GLboolean p_transpose, const GLfloat* p_value
    GLfloat values[p_count*6];
    GLint loc;
    if ((loc=uniform(ctx, cmd, false, true, 2, 3, GL_FLOAT, values, NULL))<0) RETURN;
    real(loc, p_count, p_transpose, values);

glUniformMatrix3x2fv: //GLint p_location, GLsizei p_count, GLboolean p_transpose, const GLfloat* p_value
    GLfloat values[p_count*6];
    GLint loc;
    if ((loc=uniform(ctx, cmd, false, true, 3, 2, GL_FLOAT, values, NULL))<0) RETURN;
    real(loc, p_count, p_transpose, values);

glUniformMatrix2x4fv: //GLint p_location, GLsizei p_count, GLboolean p_transpose, const GLfloat* p_value
    GLfloat values[p_count*8];
    GLint loc;
    if ((loc=uniform(ctx, cmd, false, true, 2, 4, GL_FLOAT, values, NULL))<0) RETURN;
    real(loc, p_count, p_transpose, values);

glUniformMatrix4x2fv: //GLint p_location, GLsizei p_count, GLboolean p_transpose, const GLfloat* p_value
    GLfloat values[p_count*8];
    GLint loc;
    if ((loc=uniform(ctx, cmd, false, true, 4, 2, GL_FLOAT, values, NULL))<0) RETURN;
    real(loc, p_count, p_transpose, values);

glUniformMatrix3x4fv: //GLint p_location, GLsizei p_count, GLboolean p_transpose, const GLfloat* p_value
    GLfloat values[p_count*12];
    GLint loc;
    if ((loc=uniform(ctx, cmd, false, true, 3, 4, GL_FLOAT, values, NULL))<0) RETURN;
    real(loc, p_count, p_transpose, values);

glUniformMatrix4x3fv: //GLint p_location, GLsizei p_count, GLboolean p_transpose, const GLfloat* p_value
    GLfloat values[p_count*12];
    GLint loc;
    if ((loc=uniform(ctx, cmd, false, true, 4, 3, GL_FLOAT, values, NULL))<0) RETURN;
    real(loc, p_count, p_transpose, values);

glUniformMatrix2dv: //GLint p_location, GLsizei p_count, GLboolean p_transpose, const GLdouble* p_value
    GLdouble values[p_count*4];
    GLint loc;
    if ((loc=uniform(ctx, cmd, false, true, 2, 2, GL_DOUBLE, values, NULL))<0) RETURN;
    real(loc, p_count, p_transpose, values);

glUniformMatrix3dv: //GLint p_location, GLsizei p_count, GLboolean p_transpose, const GLdouble* p_value
    GLdouble values[p_count*9];
    GLint loc;
    if ((loc=uniform(ctx, cmd, false, true, 3, 3, GL_DOUBLE, values, NULL))<0) RETURN;
    real(loc, p_count, p_transpose, values);

glUniformMatrix4dv: //GLint p_location, GLsizei p_count, GLboolean p_transpose, const GLdouble* p_value
    GLdouble values[p_count*16];
    GLint loc;
    if ((loc=uniform(ctx, cmd, false, true, 4, 4, GL_DOUBLE, values, NULL))<0) RETURN;
    real(loc, p_count, p_transpose, values);

glUniformMatrix2x3dv: //GLint p_location, GLsizei p_count, GLboolean p_transpose, const GLdouble* p_value
    GLdouble values[p_count*6];
    GLint loc;
    if ((loc=uniform(ctx, cmd, false, true, 2, 3, GL_DOUBLE, values, NULL))<0) RETURN;
    real(loc, p_count, p_transpose, values);

glUniformMatrix3x2dv: //GLint p_location, GLsizei p_count, GLboolean p_transpose, const GLdouble* p_value
    GLdouble values[p_count*6];
    GLint loc;
    if ((loc=uniform(ctx, cmd, false, true, 3, 2, GL_DOUBLE, values, NULL))<0) RETURN;
    real(loc, p_count, p_transpose, values);

glUniformMatrix2x4dv: //GLint p_location, GLsizei p_count, GLboolean p_transpose, const GLdouble* p_value
    GLdouble values[p_count*8];
    GLint loc;
    if ((loc=uniform(ctx, cmd, false, true, 2, 4, GL_DOUBLE, values, NULL))<0) RETURN;
    real(loc, p_count, p_transpose, values);

glUniformMatrix4x2dv: //GLint p_location, GLsizei p_count, GLboolean p_transpose, const GLdouble* p_value
    GLdouble values[p_count*8];
    GLint loc;
    if ((loc=uniform(ctx, cmd, false, true, 4, 2, GL_DOUBLE, values, NULL))<0) RETURN;
    real(loc, p_count, p_transpose, values);

glUniformMatrix3x4dv: //GLint p_location, GLsizei p_count, GLboolean p_transpose, const GLdouble* p_value
    GLdouble values[p_count*12];
    GLint loc;
    if ((loc=uniform(ctx, cmd, false, true, 3, 4, GL_DOUBLE, values, NULL))<0) RETURN;
    real(loc, p_count, p_transpose, values);

glUniformMatrix4x3dv: //GLint p_location, GLsizei p_count, GLboolean p_transpose, const GLdouble* p_value
    GLdouble values[p_count*12];
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
    GLfloat values[p_count];
    GLint loc; GLuint program;
    if ((loc=uniform(ctx, cmd, true, true, 1, 1, GL_FLOAT, values, &program))<0) RETURN;
    real(program, loc, p_count, values);

glProgramUniform2fv: //GLuint p_program, GLint p_location, GLsizei p_count, const GLfloat* p_value
    GLfloat values[p_count*2];
    GLint loc; GLuint program;
    if ((loc=uniform(ctx, cmd, true, true, 2, 1, GL_FLOAT, values, &program))<0) RETURN;
    real(program, loc, p_count, values);

glProgramUniform3fv: //GLuint p_program, GLint p_location, GLsizei p_count, const GLfloat* p_value
    GLfloat values[p_count*3];
    GLint loc; GLuint program;
    if ((loc=uniform(ctx, cmd, true, true, 3, 1, GL_FLOAT, values, &program))<0) RETURN;
    real(program, loc, p_count, values);

glProgramUniform4fv: //GLuint p_program, GLint p_location, GLsizei p_count, const GLfloat* p_value
    GLfloat values[p_count*4];
    GLint loc; GLuint program;
    if ((loc=uniform(ctx, cmd, true, true, 4, 1, GL_FLOAT, values, &program))<0) RETURN;
    real(program, loc, p_count, values);

glProgramUniform1iv: //GLuint p_program, GLint p_location, GLsizei p_count, const GLint* p_value
    GLint values[p_count];
    GLint loc; GLuint program;
    if ((loc=uniform(ctx, cmd, true, true, 1, 1, GL_INT, values, &program))<0) RETURN;
    real(program, loc, p_count, values);

glProgramUniform2iv: //GLuint p_program, GLint p_location, GLsizei p_count, const GLint* p_value
    GLint values[p_count*2];
    GLint loc; GLuint program;
    if ((loc=uniform(ctx, cmd, true, true, 2, 1, GL_INT, values, &program))<0) RETURN;
    real(program, loc, p_count, values);

glProgramUniform3iv: //GLuint p_program, GLint p_location, GLsizei p_count, const GLint* p_value
    GLint values[p_count*3];
    GLint loc; GLuint program;
    if ((loc=uniform(ctx, cmd, true, true, 3, 1, GL_INT, values, &program))<0) RETURN;
    real(program, loc, p_count, values);

glProgramUniform4iv: //GLuint p_program, GLint p_location, GLsizei p_count, const GLint* p_value
    GLint values[p_count*4];
    GLint loc; GLuint program;
    if ((loc=uniform(ctx, cmd, true, true, 4, 1, GL_INT, values, &program))<0) RETURN;
    real(program, loc, p_count, values);

glProgramUniform1uiv: //GLuint p_program, GLint p_location, GLsizei p_count, const GLuint* p_value
    GLuint values[p_count];
    GLint loc; GLuint program;
    if ((loc=uniform(ctx, cmd, true, true, 1, 1, GL_UNSIGNED_INT, values, &program))<0) RETURN;
    real(program, loc, p_count, values);

glProgramUniform2uiv: //GLuint p_program, GLint p_location, GLsizei p_count, const GLuint* p_value
    GLuint values[p_count*2];
    GLint loc; GLuint program;
    if ((loc=uniform(ctx, cmd, true, true, 2, 1, GL_UNSIGNED_INT, values, &program))<0) RETURN;
    real(program, loc, p_count, values);

glProgramUniform3uiv: //GLuint p_program, GLint p_location, GLsizei p_count, const GLuint* p_value
    GLuint values[p_count*3];
    GLint loc; GLuint program;
    if ((loc=uniform(ctx, cmd, true, true, 3, 1, GL_UNSIGNED_INT, values, &program))<0) RETURN;
    real(program, loc, p_count, values);

glProgramUniform4uiv: //GLuint p_program, GLint p_location, GLsizei p_count, const GLuint* p_value
    GLuint values[p_count*4];
    GLint loc; GLuint program;
    if ((loc=uniform(ctx, cmd, true, true, 4, 1, GL_UNSIGNED_INT, values, &program))<0) RETURN;
    real(program, loc, p_count, values);

glProgramUniform1dv: //GLuint p_program, GLint p_location, GLsizei p_count, const GLdouble* p_value
    GLdouble values[p_count];
    GLint loc; GLuint program;
    if ((loc=uniform(ctx, cmd, true, true, 1, 1, GL_DOUBLE, values, &program))<0) RETURN;
    real(program, loc, p_count, values);

glProgramUniform2dv: //GLuint p_program, GLint p_location, GLsizei p_count, const GLdouble* p_value
    GLdouble values[p_count*2];
    GLint loc; GLuint program;
    if ((loc=uniform(ctx, cmd, true, true, 2, 1, GL_DOUBLE, values, &program))<0) RETURN;
    real(program, loc, p_count, values);

glProgramUniform3dv: //GLuint p_program, GLint p_location, GLsizei p_count, const GLdouble* p_value
    GLdouble values[p_count*3];
    GLint loc; GLuint program;
    if ((loc=uniform(ctx, cmd, true, true, 3, 1, GL_UNSIGNED_INT, values, &program))<0) RETURN;
    real(program, loc, p_count, values);

glProgramUniform4dv: //GLuint p_program, GLint p_location, GLsizei p_count, const GLdouble* p_value
    GLdouble values[p_count*4];
    GLint loc; GLuint program;
    if ((loc=uniform(ctx, cmd, true, true, 4, 1, GL_UNSIGNED_INT, values, &program))<0) RETURN;
    real(program, loc, p_count, values);

glProgramUniformMatrix2fv: //GLuint p_program, GLint p_location, GLsizei p_count, GLboolean p_transpose, const GLfloat* p_value
    GLfloat values[p_count*4];
    GLint loc; GLuint program;
    if ((loc=uniform(ctx, cmd, true, true, 2, 2, GL_FLOAT, values, &program))<0) RETURN;
    real(program, loc, p_count, p_transpose, values);

glProgramUniformMatrix3fv: //GLuint p_program, GLint p_location, GLsizei p_count, GLboolean p_transpose, const GLfloat* p_value
    GLfloat values[p_count*9];
    GLint loc; GLuint program;
    if ((loc=uniform(ctx, cmd, true, true, 3, 3, GL_FLOAT, values, &program))<0) RETURN;
    real(program, loc, p_count, p_transpose, values);

glProgramUniformMatrix4fv: //GLuint p_program, GLint p_location, GLsizei p_count, GLboolean p_transpose, const GLfloat* p_value
    GLfloat values[p_count*16];
    GLint loc; GLuint program;
    if ((loc=uniform(ctx, cmd, true, true, 4, 4, GL_FLOAT, values, &program))<0) RETURN;
    real(program, loc, p_count, p_transpose, values);

glProgramUniformMatrix2x3fv: //GLuint p_program, GLint p_location, GLsizei p_count, GLboolean p_transpose, const GLfloat* p_value
    GLfloat values[p_count*6];
    GLint loc; GLuint program;
    if ((loc=uniform(ctx, cmd, true, true, 2, 3, GL_FLOAT, values, &program))<0) RETURN;
    real(program, loc, p_count, p_transpose, values);

glProgramUniformMatrix3x2fv: //GLuint p_program, GLint p_location, GLsizei p_count, GLboolean p_transpose, const GLfloat* p_value
    GLfloat values[p_count*6];
    GLint loc; GLuint program;
    if ((loc=uniform(ctx, cmd, true, true, 3, 2, GL_FLOAT, values, &program))<0) RETURN;
    real(program, loc, p_count, p_transpose, values);

glProgramUniformMatrix2x4fv: //GLuint p_program, GLint p_location, GLsizei p_count, GLboolean p_transpose, const GLfloat* p_value
    GLfloat values[p_count*8];
    GLint loc; GLuint program;
    if ((loc=uniform(ctx, cmd, true, true, 2, 4, GL_FLOAT, values, &program))<0) RETURN;
    real(program, loc, p_count, p_transpose, values);

glProgramUniformMatrix4x2fv: //GLuint p_program, GLint p_location, GLsizei p_count, GLboolean p_transpose, const GLfloat* p_value
    GLfloat values[p_count*8];
    GLint loc; GLuint program;
    if ((loc=uniform(ctx, cmd, true, true, 4, 2, GL_FLOAT, values, &program))<0) RETURN;
    real(program, loc, p_count, p_transpose, values);

glProgramUniformMatrix3x4fv: //GLuint p_program, GLint p_location, GLsizei p_count, GLboolean p_transpose, const GLfloat* p_value
    GLfloat values[p_count*12];
    GLint loc; GLuint program;
    if ((loc=uniform(ctx, cmd, true, true, 3, 4, GL_FLOAT, values, &program))<0) RETURN;
    real(program, loc, p_count, p_transpose, values);

glProgramUniformMatrix4x3fv: //GLuint p_program, GLint p_location, GLsizei p_count, GLboolean p_transpose, const GLfloat* p_value
    GLfloat values[p_count*12];
    GLint loc; GLuint program;
    if ((loc=uniform(ctx, cmd, true, true, 4, 3, GL_FLOAT, values, &program))<0) RETURN;
    real(program, loc, p_count, p_transpose, values);

glProgramUniformMatrix2dv: //GLuint p_program, GLint p_location, GLsizei p_count, GLboolean p_transpose, const GLdouble* p_value
    GLdouble values[p_count*4];
    GLint loc; GLuint program;
    if ((loc=uniform(ctx, cmd, true, true, 2, 2, GL_DOUBLE, values, &program))<0) RETURN;
    real(program, loc, p_count, p_transpose, values);

glProgramUniformMatrix3dv: //GLuint p_program, GLint p_location, GLsizei p_count, GLboolean p_transpose, const GLdouble* p_value
    GLdouble values[p_count*9];
    GLint loc; GLuint program;
    if ((loc=uniform(ctx, cmd, true, true, 3, 3, GL_DOUBLE, values, &program))<0) RETURN;
    real(program, loc, p_count, p_transpose, values);

glProgramUniformMatrix4dv: //GLuint p_program, GLint p_location, GLsizei p_count, GLboolean p_transpose, const GLdouble* p_value
    GLdouble values[p_count*16];
    GLint loc; GLuint program;
    if ((loc=uniform(ctx, cmd, true, true, 4, 4, GL_DOUBLE, values, &program))<0) RETURN;
    real(program, loc, p_count, p_transpose, values);

glProgramUniformMatrix2x3dv: //GLuint p_program, GLint p_location, GLsizei p_count, GLboolean p_transpose, const GLdouble* p_value
    GLdouble values[p_count*6];
    GLint loc; GLuint program;
    if ((loc=uniform(ctx, cmd, true, true, 2, 3, GL_DOUBLE, values, &program))<0) RETURN;
    real(program, loc, p_count, p_transpose, values);

glProgramUniformMatrix3x2dv: //GLuint p_program, GLint p_location, GLsizei p_count, GLboolean p_transpose, const GLdouble* p_value
    GLdouble values[p_count*6];
    GLint loc; GLuint program;
    if ((loc=uniform(ctx, cmd, true, true, 3, 2, GL_DOUBLE, values, &program))<0) RETURN;
    real(program, loc, p_count, p_transpose, values);

glProgramUniformMatrix2x4dv: //GLuint p_program, GLint p_location, GLsizei p_count, GLboolean p_transpose, const GLdouble* p_value
    GLdouble values[p_count*8];
    GLint loc; GLuint program;
    if ((loc=uniform(ctx, cmd, true, true, 2, 4, GL_DOUBLE, values, &program))<0) RETURN;
    real(program, loc, p_count, p_transpose, values);

glProgramUniformMatrix4x2dv: //GLuint p_program, GLint p_location, GLsizei p_count, GLboolean p_transpose, const GLdouble* p_value
    GLdouble values[p_count*8];
    GLint loc; GLuint program;
    if ((loc=uniform(ctx, cmd, true, true, 4, 2, GL_DOUBLE, values, &program))<0) RETURN;
    real(program, loc, p_count, p_transpose, values);

glProgramUniformMatrix3x4dv: //GLuint p_program, GLint p_location, GLsizei p_count, GLboolean p_transpose, const GLdouble* p_value
    GLdouble values[p_count*12];
    GLint loc; GLuint program;
    if ((loc=uniform(ctx, cmd, true, true, 3, 4, GL_DOUBLE, values, &program))<0) RETURN;
    real(program, loc, p_count, p_transpose, values);

glProgramUniformMatrix4x3dv: //GLuint p_program, GLint p_location, GLsizei p_count, GLboolean p_transpose, const GLdouble* p_value
    GLdouble values[p_count*12];
    GLint loc; GLuint program;
    if ((loc=uniform(ctx, cmd, true, true, 4, 3, GL_DOUBLE, values, &program))<0) RETURN;
    real(program, loc, p_count, p_transpose, values);

//TODO: There is some duplicate code among glVertexAttrib*Pointer and gl*VertexAttribArray
glVertexAttribPointer: //GLuint p_index, GLint p_size, GLenum p_type, GLboolean p_normalized, GLsizei p_stride, const void* p_pointer
    //if (p_pointer > UINTPTR_MAX) //TODO
    real(p_index, p_size, p_type, p_normalized, p_stride, (const GLvoid*)(uintptr_t)p_pointer);
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
    real(p_index, p_size, p_type, p_stride, (const GLvoid*)(uintptr_t)p_pointer);
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
    real(p_index);
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
    real(p_index);
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
    GLuint arrays[p_n];
    real(p_n, arrays);
    
    int attrib_count = trc_gl_state_get_state_int(ctx->trace, GL_MAX_VERTEX_ATTRIBS, 0);
    for (size_t i = 0; i < p_n; ++i) {
        trc_gl_vao_rev_t rev;
        rev.fake_context = trc_get_current_fake_gl_context(ctx->trace);
        rev.ref_count = 1;
        rev.real = arrays[i];
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
        trc_set_gl_vao(ctx->trace, p_arrays[i], &rev);
    }

glDeleteVertexArrays: //GLsizei p_n, const GLuint* p_arrays
    GLuint arrays[p_n];
    for (size_t i = 0; i < p_n; ++i) {
        if (p_arrays[i] && p_arrays[i]==trc_gl_state_get_bound_vao(ctx->trace))
            trc_gl_state_set_bound_vao(ctx->trace, 0);
        if (!(arrays[i]=trc_get_real_gl_vao(ctx->trace, p_arrays[i])))
            trc_add_error(cmd, "Invalid vertex array name");
        else trc_rel_gl_obj(ctx->trace, p_arrays[i], TrcGLObj_VAO);
    }
    real(p_n, arrays);

glBindVertexArray: //GLuint p_array
    GLuint real_vao = trc_get_real_gl_vao(ctx->trace, p_array);
    if (!real_vao && p_array) ERROR("Invalid vertex array name");
    trc_gl_state_set_bound_vao(ctx->trace, p_array);

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
    GLint params[p_bufSize];
    real(p_target, p_internalformat, p_pname, p_bufSize, params);

glGetInternalformati64v: //GLenum p_target, GLenum p_internalformat, GLenum p_pname, GLsizei p_bufSize, GLint64* p_params
    GLint64 params[p_bufSize];
    real(p_target, p_internalformat, p_pname, p_bufSize, params);

glGetBufferParameteriv: //GLenum p_target, GLenum p_pname, GLint* p_params
    GLint i;
    real(p_target, p_pname, &i);

glGetBufferPointerv: //GLenum p_target, GLenum p_pname, void ** p_params
    GLvoid* p;
    real(p_target, p_pname, &p);

glGenSamplers: //GLsizei p_count, GLuint* p_samplers
    GLuint samplers[p_count];
    real(p_count, samplers);
    
    trc_gl_sampler_rev_t rev;
    rev.fake_context = trc_get_current_fake_gl_context(ctx->trace);
    rev.ref_count = 1;
    for (size_t i = 0; i < p_count; ++i) {
        rev.real = samplers[i];
        trc_set_gl_sampler(ctx->trace, p_samplers[i], &rev);
    }

glDeleteSamplers: //GLsizei p_count, const GLuint* p_samplers
    GLuint samplers[p_count];
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
    //TODO: Reference counting

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
    GLuint fbs[p_n];
    real(p_n, fbs);
    
    trc_gl_framebuffer_rev_t rev;
    rev.fake_context = trc_get_current_fake_gl_context(ctx->trace);
    rev.ref_count = 1;
    trc_data_t* empty_data = trc_create_data(ctx->trace, 0, NULL, TRC_DATA_IMMUTABLE);
    rev.attachments = empty_data;
    rev.draw_buffers = empty_data;
    for (size_t i = 0; i < p_n; ++i) {
        rev.real = fbs[i];
        trc_set_gl_framebuffer(ctx->trace, p_framebuffers[i], &rev);
    }

glDeleteFramebuffers: //GLsizei p_n, const GLuint* p_framebuffers
    GLuint fbs[p_n];
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
    GLuint fb = trc_get_real_gl_framebuffer(ctx->trace, p_framebuffer);
    if (!fb && p_framebuffer) ERROR("Invalid framebuffer name");
    real(p_target, fb);
    
    bool read = true;
    bool draw = true;
    switch (p_target) {
    case GL_READ_FRAMEBUFFER: draw = false; break;
    case GL_DRAW_FRAMEBUFFER: read = false; break;
    }
    if (read) trc_gl_state_set_read_framebuffer(ctx->trace, p_framebuffer);
    if (draw) trc_gl_state_set_draw_framebuffer(ctx->trace, p_framebuffer);

glGenRenderbuffers: //GLsizei p_n, GLuint* p_renderbuffers
    GLuint rbs[p_n];
    real(p_n, rbs);
    
    trc_gl_renderbuffer_rev_t rev;
    rev.fake_context = trc_get_current_fake_gl_context(ctx->trace);
    rev.ref_count = 1;
    rev.has_storage = false;
    for (size_t i = 0; i < p_n; ++i) {
        rev.real = rbs[i];
        trc_set_gl_renderbuffer(ctx->trace, p_renderbuffers[i], &rev);
    }

glDeleteRenderbuffers: //GLsizei p_n, const GLuint* p_renderbuffers
    GLuint rbs[p_n];
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
    GLuint rb = trc_get_real_gl_renderbuffer(ctx->trace, p_renderbuffer);
    if (!rb && p_renderbuffer) ERROR("Invalid renderbuffer name");
    real(p_target, rb);
    trc_gl_state_set_bound_renderbuffer(ctx->trace, p_renderbuffer);

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
    GLint params;
    real(p_target, p_pname, &params);

glFramebufferRenderbuffer: //GLenum p_target, GLenum p_attachment, GLenum p_renderbuffertarget, GLuint p_renderbuffer
    GLuint real_rb = trc_get_real_gl_renderbuffer(ctx->trace, p_renderbuffer);
    if (!real_rb && p_renderbuffer) ERROR("Invalid renderbuffer name");
    
    real(p_target, p_attachment, p_renderbuffertarget, real_rb);
    
    GLint fb = get_bound_framebuffer(ctx, p_target);
    replay_add_fb_attachment_rb(ctx->trace, cmd, fb, p_attachment, p_renderbuffer);

glFramebufferTexture: //GLenum p_target, GLenum p_attachment, GLuint p_texture, GLint p_level
    if (!p_texture_rev && p_texture) ERROR("Invalid texture name");
    
    real(p_target, p_attachment, p_texture_rev?p_texture_rev->real:0, p_level);
    
    if (!p_texture_rev->created) ERROR("Although it has a valid name, the texture has not been created"
                                       "Use glBindTexture or use glCreateTextures instead of glGenTextures");
    GLint fb = get_bound_framebuffer(ctx, p_target);
    replay_add_fb_attachment(ctx->trace, cmd, fb, p_attachment, p_texture, p_texture_rev->type, p_level, 0);

glFramebufferTextureLayer: //GLenum p_target, GLenum p_attachment GLuint p_texture, GLint p_level, GLint p_layer
    if (!p_texture_rev && p_texture) ERROR("Invalid texture name");
    
    real(p_target, p_attachment, p_texture_rev?p_texture_rev->real:0, p_level, p_layer);
    
    if (!p_texture_rev->created) ERROR("Although it has a valid name, the texture has not been created"
                                       "Use glBindTexture or use glCreateTextures instead of glGenTextures");
    GLint fb = get_bound_framebuffer(ctx, p_target);
    replay_add_fb_attachment(ctx->trace, cmd, fb, p_attachment, p_texture, p_texture_rev->type, p_level, p_layer);

glFramebufferTexture1D: //GLenum p_target, GLenum p_attachment, GLenum p_textarget, GLuint p_texture, GLint p_level
    if (!p_texture_rev && p_texture) ERROR("Invalid texture name");
    
    real(p_target, p_attachment, p_textarget, p_texture_rev?p_texture_rev->real:0, p_level);
    
    if (!p_texture_rev->created) ERROR("Although it has a valid name, the texture has not been created"
                                       "Use glBindTexture or use glCreateTextures instead of glGenTextures");
    GLint fb = get_bound_framebuffer(ctx, p_target);
    replay_add_fb_attachment(ctx->trace, cmd, fb, p_attachment, p_texture, p_textarget, p_level, 0);

glFramebufferTexture2D: //GLenum p_target, GLenum p_attachment, GLenum p_textarget, GLuint p_texture, GLint p_level
    if (!p_texture_rev && p_texture) ERROR("Invalid texture name");
    
    real(p_target, p_attachment, p_textarget, p_texture_rev?p_texture_rev->real:0, p_level);
    
    if (!p_texture_rev->created) ERROR("Although it has a valid name, the texture has not been created"
                                       "Use glBindTexture or use glCreateTextures instead of glGenTextures");
    GLint fb = get_bound_framebuffer(ctx, p_target);
    replay_add_fb_attachment(ctx->trace, cmd, fb, p_attachment, p_texture, p_textarget, p_level, 0);

glFramebufferTexture3D: //GLenum p_target, GLenum p_attachment, GLenum p_textarget, GLuint p_texture, GLint p_level, GLint p_zoffset
    if (!p_texture_rev && p_texture) ERROR("Invalid texture name");
    
    real(p_target, p_attachment, p_textarget, p_texture_rev?p_texture_rev->real:0, p_level, p_zoffset);
    
    if (!p_texture_rev->created) ERROR("Although it has a valid name, the texture has not been created"
                                       "Use glBindTexture or use glCreateTextures instead of glGenTextures");
    GLint fb = get_bound_framebuffer(ctx, p_target);
    replay_add_fb_attachment(ctx->trace, cmd, fb, p_attachment, p_texture, p_textarget, p_level, p_zoffset);

glRenderbufferStorage: //GLenum p_target, GLenum p_internalformat, GLsizei p_width, GLsizei p_height
    uint rb = trc_gl_state_get_bound_renderbuffer(ctx->trace);
    const trc_gl_renderbuffer_rev_t* rev = trc_get_gl_renderbuffer(ctx->trace, rb);
    if (!rev) ERROR("Invalid renderbuffer name");
    real(p_target, p_internalformat, p_width, p_height);
    replay_update_renderbuffer(ctx, rev, rb, p_width, p_height, p_internalformat, 1);

glRenderbufferStorageMultisample: //GLenum p_target, GLsizei p_samples, GLenum p_internalformat, GLsizei p_width, GLsizei p_height
    uint rb = trc_gl_state_get_bound_renderbuffer(ctx->trace);
    const trc_gl_renderbuffer_rev_t* rev = trc_get_gl_renderbuffer(ctx->trace, rb);
    if (!rev) ERROR("Invalid renderbuffer name");
    real(p_target, p_samples, p_internalformat, p_width, p_height);
    replay_update_renderbuffer(ctx, rev, rb, p_width, p_height, p_internalformat, p_samples);

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
    GLuint queries[p_n];
    real(p_n, queries);
    
    trc_gl_query_rev_t rev;
    rev.fake_context = trc_get_current_fake_gl_context(ctx->trace);
    rev.ref_count = 1;
    rev.type = 0;
    rev.result = 0;
    for (size_t i = 0; i < p_n; ++i) {
        rev.real = queries[i];
        trc_set_gl_query(ctx->trace, p_ids[i], &rev);
    }

glDeleteQueries: //GLsizei p_n, const GLuint* p_ids
    GLuint queries[p_n];
    for (size_t i = 0; i < p_n; ++i) {
        //TODO: Handle when queries are in use
        if (!(queries[i] = trc_get_real_gl_query(ctx->trace, p_ids[i])))
            trc_add_error(cmd, "Invalid query name");
        else trc_rel_gl_obj(ctx->trace, p_ids[i], TrcGLObj_Query);
    }
    real(p_n, queries);

glBeginQuery: //GLenum p_target, GLuint p_id
    GLuint real_id = trc_get_real_gl_query(ctx->trace, p_id);
    if (!real_id) ERROR("Invalid query name");
    real(p_target, real_id);
    
    trc_gl_query_rev_t query = *trc_get_gl_query(ctx->trace, p_id);
    query.type = p_target;
    trc_set_gl_query(ctx->trace, p_id, &query);
    
    trc_gl_state_set_bound_queries(ctx->trace, p_target, 0, p_id);
    //TODO: Reference counting

glEndQuery: //GLenum p_target
    real(p_target);
    
    GLuint id = trc_gl_state_get_bound_queries(ctx->trace, p_target, 0);
    GLuint real_id = trc_get_real_gl_query(ctx->trace, id);
    trc_gl_state_set_bound_queries(ctx->trace, p_target, 0, 0);
    
    //TODO: This clears any errors
    if (F(glGetError)() == GL_NO_ERROR) update_query(ctx, cmd, p_target, id, real_id);
    //TODO: Reference counting

glQueryCounter: //GLuint p_id, GLenum p_target
    GLuint real_id = trc_get_real_gl_query(ctx->trace, p_id);
    if (!real_id) ERROR("Invalid query name");
    real(real_id, p_target);
    //TODO: This clears any errors
    if (F(glGetError)() == GL_NO_ERROR) update_query(ctx, cmd, p_target, p_id, real_id);

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
    trc_gl_state_set_state_float(ctx->trace, GL_DEPTH_RANGE, 0, p_near);
    trc_gl_state_set_state_float(ctx->trace, GL_DEPTH_RANGE, 0, p_far);
    real(p_near, p_far);

glDepthRangef: //GLfloat p_n, GLfloat p_f
    trc_gl_state_set_state_float(ctx->trace, GL_DEPTH_RANGE, 0, p_n);
    trc_gl_state_set_state_float(ctx->trace, GL_DEPTH_RANGE, 0, p_f);
    real(p_n, p_f);

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

//TODO: glViewportIndexedfv

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

//TODO: glScissorIndexedv

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
