glXMakeCurrent:
    SDL_GLContext glctx = NULL;
    uint64_t fake_ctx = *trc_get_uint(trc_get_arg(command, 2));
    if (fake_ctx) {
        if (!(glctx=trc_get_real_gl_context(ctx->trace, fake_ctx)))
            ERROR("Invalid GLX context.");
    }
    
    if (SDL_GL_MakeCurrent(ctx->window, glctx) < 0)
        ERROR("Unable to make a context current.");
    
    if (glctx) {
        reload_gl_funcs(ctx);
        trc_set_current_fake_gl_context(ctx->trace, fake_ctx);
    } else {
        reset_gl_funcs(ctx);
        trc_set_current_fake_gl_context(ctx->trace, 0);
    }
    
    //Seems to be messing up the front buffer.
    //But the front buffer is still sometimes black when it should not be.
    /*SDL_GL_SetSwapInterval(0)*/

glXSwapIntervalEXT:
    ;

glXSwapIntervalMESA:
    ;

glXSwapIntervalSGI:
    ;

glXGetProcAddressARB:
    ;

glXGetProcAddress:
    ;

glXQueryExtension:
    ;

glXQueryVersion:
    ;

glXQueryDrawable:
    ;

glXChooseVisual:
    ;

glXChooseFBConfig:
    ;

glXGetFBConfigs:
    ;

glXGetFBConfigAttrib:
    ;

glXGetVisualFromFBConfig:
    ;

glXChooseFBConfigSGIX:
    ;

glXGetFBConfigAttribSGIX:
    ;

glXGetVisualFromFBConfigSGIX:
    ;

glXGetClientString:
    ;

glXCreateContext:
    SDL_GLContext shareList = NULL;
    if (*trc_get_ptr(trc_get_arg(command, 2))) {
        shareList = trc_get_real_gl_context(ctx->trace, *trc_get_ptr(trc_get_arg(command, 2)));
        if (!shareList) ERROR("Invalid share context name.");
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
        trc_add_error(command, "Unable to create context: %s", SDL_GetError());
        SDL_GL_MakeCurrent(ctx->window, last_ctx);
        RETURN;
    }
    reload_gl_funcs(ctx);
    
    trc_gl_context_rev_t rev = create_context_rev(ctx, res);
    uint64_t fake = trc_get_ptr(&command->ret)[0];
    trc_set_gl_context(ctx->trace, fake, &rev);
    
    uint64_t prev_fake = trc_get_current_fake_gl_context(ctx->trace);
    size_t end = ctx->trace->inspection.cur_ctx_revision_count - 1;
    ctx->trace->inspection.cur_ctx_revisions[end].context = fake; //A hack
    replay_update_buffers(ctx, true, true, true, true);
    ctx->trace->inspection.cur_ctx_revisions[end].context = prev_fake;
    
    SDL_GL_MakeCurrent(ctx->window, last_ctx);
    reload_gl_funcs(ctx);

glXCreateContextAttribsARB:
    int last_major, last_minor, last_flags, last_profile;
    SDL_GL_GetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, &last_major);
    SDL_GL_GetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, &last_minor);
    SDL_GL_GetAttribute(SDL_GL_CONTEXT_FLAGS, &last_flags);
    SDL_GL_GetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, &last_profile);
    
    int major = last_major;
    int minor = last_minor;
    int flags = 0;
    int profile = 0;
    
    const int64_t* attribs = trc_get_int(trc_get_arg(command, 4));
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
            trc_add_warning(command, "Unnamed attribute: %d", attr);
            attribs++;
        }
    }
    
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, major);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, minor);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, flags);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, profile);
    
    SDL_GLContext share_ctx = NULL;
    if (gl_param_GLXContext(command, 2)) {
        share_ctx = trc_get_real_gl_context(ctx->trace, gl_param_GLXContext(command, 2));
        if (!share_ctx) ERROR("Invalid share context name.");
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
        trc_add_error(command, "Unable to create context: %s", SDL_GetError());
        SDL_GL_MakeCurrent(ctx->window, last_ctx);
        RETURN;
    }
    reload_gl_funcs(ctx);
    
    trc_gl_context_rev_t rev = create_context_rev(ctx, res);
    uint64_t fake = trc_get_ptr(&command->ret)[0];
    trc_set_gl_context(ctx->trace, fake, &rev);
    
    uint64_t prev_fake = trc_get_current_fake_gl_context(ctx->trace);
    size_t end = ctx->trace->inspection.cur_ctx_revision_count - 1;
    ctx->trace->inspection.cur_ctx_revisions[end].context = fake; //A hack
    replay_update_buffers(ctx, true, true, true, true);
    ctx->trace->inspection.cur_ctx_revisions[end].context = prev_fake;
    
    SDL_GL_MakeCurrent(ctx->window, last_ctx);
    reload_gl_funcs(ctx);
    
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, last_major);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, last_minor);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, last_flags);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, last_profile);

glXQueryExtensionsString:
    ;

glXDestroyContext:
    SDL_GLContext glctx = trc_get_real_gl_context(ctx->trace, *trc_get_ptr(trc_get_arg(command, 1)));
    if (!glctx) ERROR("Invalid context name.");
    
    SDL_GL_DeleteContext(glctx);
    //TODO
    //replay_rel_object(ctx, ReplayObjType_GLXContext, *trc_get_ptr(trc_get_arg(command, 1)));

glXSwapBuffers:
    if (!trc_get_current_fake_gl_context(ctx->trace)) ERROR("No current OpenGL context.");
    SDL_GL_SwapWindow(ctx->window);
    replay_update_buffers(ctx, false, true, false, false);

glSetContextCapsWIP15:
    ;

glClear:
    GLbitfield mask = gl_param_GLbitfield(command, 0);
    real(mask);
    bool color = mask & GL_COLOR_BUFFER_BIT;
    bool depth = mask & GL_DEPTH_BUFFER_BIT;
    bool stencil = mask & GL_STENCIL_BUFFER_BIT;
    replay_update_buffers(ctx, color, false, depth, stencil);

glGenTextures:
    GLsizei n = gl_param_GLsizei(command, 0);
    GLuint textures[n];
    const uint64_t* fake = trc_get_uint(trc_get_arg(command, 1));
    
    real(n, textures);
    
    for (size_t i = 0; i < n; ++i) {
        trc_gl_texture_rev_t rev;
        rev.fake_context = trc_get_current_fake_gl_context(ctx->trace);
        rev.ref_count = 1;
        rev.real = textures[i];
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
        trc_set_gl_texture(ctx->trace, fake[i], &rev);
    }

glDeleteTextures:
    GLsizei n = gl_param_GLsizei(command, 0);
    GLuint textures[n];
    const uint64_t* fake = trc_get_uint(trc_get_arg(command, 1));
    
    for (size_t i = 0; i < n; ++i)
        if (!(textures[i] = trc_get_real_gl_texture(ctx->trace, fake[i])))
            trc_add_error(command, "Invalid texture name.");
        else trc_rel_gl_obj(ctx->trace, fake[i], TrcGLObj_Texture);
    
    real(n, textures);

glBindTexture:
    GLuint target = gl_param_GLenum(command, 0);
    GLuint fake = gl_param_GLuint(command, 1);
    GLuint real_tex = trc_get_real_gl_texture(ctx->trace, fake);
    const trc_gl_texture_rev_t* rev = trc_get_gl_texture(ctx->trace, fake);
    if (!rev && fake) ERROR("Invalid texture name.");
    if (!rev->created) {
        trc_gl_texture_rev_t newrev = *rev;
        newrev.created = true;
        newrev.type = target;
        newrev.images = trc_create_inspection_data(ctx->trace, 0, NULL);
        trc_set_gl_texture(ctx->trace, fake, &newrev);
    }
    //TODO: Reference counting
    real(target, real_tex);
    set_bound_texture(ctx->trace, target, -1, fake);

glTexImage1D:
    GLenum target = gl_param_GLenum(command, 0);
    GLint level = gl_param_GLint(command, 1);
    GLint internalFormat = gl_param_GLint(command, 2);
    GLsizei width = gl_param_GLsizei(command, 3);
    GLint border = gl_param_GLint(command, 4);
    GLenum format = gl_param_GLenum(command, 5);
    GLenum type = gl_param_GLenum(command, 6);
    const void* data = gl_param_data(command, 7);
    real(target, level, internalFormat, width, border, format, type, data);
    replay_update_tex_image(ctx, command, target, level);

glCompressedTexImage1D:
    GLenum target = gl_param_GLenum(command, 0);
    GLint level = gl_param_GLint(command, 1);
    GLenum internalformat = gl_param_GLenum(command, 2);
    GLsizei width = gl_param_GLsizei(command, 3);
    GLint border = gl_param_GLint(command, 4);
    GLsizei imageSize = gl_param_GLsizei(command, 5);
    const void* data = gl_param_data(command, 6);
    real(target, level, internalformat, width, border, imageSize, data);
    replay_update_tex_image(ctx, command, target, level);

glTexSubImage1D:
    GLenum target = gl_param_GLenum(command, 0);
    GLint level = gl_param_GLint(command, 1);
    GLint xoffset = gl_param_GLint(command, 2);
    GLsizei width = gl_param_GLsizei(command, 3);
    GLenum format = gl_param_GLenum(command, 4);
    GLenum type = gl_param_GLenum(command, 5);
    const void* data = gl_param_data(command, 6);
    real(target, level, xoffset, width, format, type, data);
    replay_update_tex_image(ctx, command, target, level);

glCompressedTexSubImage1D:
    GLenum target = gl_param_GLenum(command, 0);
    GLint level = gl_param_GLint(command, 1);
    GLint xoffset = gl_param_GLint(command, 2);
    GLsizei width = gl_param_GLsizei(command, 3);
    GLenum format = gl_param_GLenum(command, 4);
    GLsizei imageSize = gl_param_GLsizei(command, 5);
    const void* data = gl_param_data(command, 6);
    real(target, level, xoffset, width, format, imageSize, data);
    replay_update_tex_image(ctx, command, target, level);

glTexImage2D:
    GLenum target = gl_param_GLenum(command, 0);
    GLint level = gl_param_GLint(command, 1);
    GLint internalFormat = gl_param_GLint(command, 2);
    GLsizei width = gl_param_GLenum(command, 3);
    GLsizei height = gl_param_GLenum(command, 4);
    GLint border = gl_param_GLenum(command, 5);
    GLenum format = gl_param_GLenum(command, 6);
    GLenum type = gl_param_GLenum(command, 7);
    const void* data = gl_param_data(command, 8);
    real(target, level, internalFormat, width, height, border, format, type, data);
    replay_update_tex_image(ctx, command, target, level);

glCompressedTexImage2D:
    GLenum target = gl_param_GLenum(command, 0);
    GLint level = gl_param_GLint(command, 1);
    GLenum internalformat = gl_param_GLenum(command, 2);
    GLsizei width = gl_param_GLsizei(command, 3);
    GLsizei height = gl_param_GLsizei(command, 4);
    GLint border = gl_param_GLint(command, 5);
    GLsizei imageSize = gl_param_GLsizei(command, 6);
    const void* data = gl_param_data(command, 7);
    real(target, level, internalformat, width, height, border, imageSize, data);
    replay_update_tex_image(ctx, command, target, level);

glTexSubImage2D:
    GLenum target = gl_param_GLenum(command, 0);
    GLint level = gl_param_GLint(command, 1);
    GLint xoffset = gl_param_GLint(command, 2);
    GLint yoffset = gl_param_GLint(command, 3);
    GLsizei width = gl_param_GLsizei(command, 4);
    GLsizei height = gl_param_GLsizei(command, 5);
    GLenum format = gl_param_GLenum(command, 6);
    GLenum type = gl_param_GLenum(command, 7);
    const void* data = gl_param_data(command, 8);
    real(target, level, xoffset, yoffset, width, height, format, type, data);
    replay_update_tex_image(ctx, command, target, level);

glCompressedTexSubImage2D:
    GLenum target = gl_param_GLenum(command, 0);
    GLint level = gl_param_GLint(command, 1);
    GLint xoffset = gl_param_GLint(command, 2);
    GLint yoffset = gl_param_GLint(command, 3);
    GLsizei width = gl_param_GLsizei(command, 4);
    GLsizei height = gl_param_GLsizei(command, 5);
    GLenum format = gl_param_GLenum(command, 6);
    GLsizei imageSize = gl_param_GLsizei(command, 7);
    const void* data = gl_param_data(command, 8);
    real(target, level, xoffset, yoffset, width, height, format, imageSize, data);
    replay_update_tex_image(ctx, command, target, level);

glTexImage3D:
    GLenum target = gl_param_GLenum(command, 0); //TODO: Array textures
    GLint level = gl_param_GLint(command, 1);
    GLint internalFormat = gl_param_GLint(command, 2);
    GLsizei width = gl_param_GLsizei(command, 3);
    GLsizei height = gl_param_GLsizei(command, 4);
    GLsizei depth = gl_param_GLsizei(command, 5);
    GLint border = gl_param_GLsizei(command, 6);
    GLenum format = gl_param_GLenum(command, 7);
    GLenum type = gl_param_GLenum(command, 8);
    const void* data = gl_param_data(command, 9);
    real(target, level, internalFormat, width, height, depth, border, format, type, data);
    replay_update_tex_image(ctx, command, target, level);

glCompressedTexImage3D:
    GLenum target = gl_param_GLenum(command, 0); //TODO: Array textures
    GLint level = gl_param_GLint(command, 1);
    GLenum internalformat = gl_param_GLenum(command, 2);
    GLsizei width = gl_param_GLsizei(command, 3);
    GLsizei height = gl_param_GLsizei(command, 4);
    GLsizei depth = gl_param_GLsizei(command, 5);
    GLint border = gl_param_GLint(command, 6);
    GLsizei imageSize = gl_param_GLsizei(command, 7);
    const void* data = gl_param_data(command, 8);
    real(target, level, internalformat, width, height, depth, border, imageSize, data);
    replay_update_tex_image(ctx, command, target, level);

glTexSubImage3D:
    GLenum target = gl_param_GLenum(command, 0);
    GLint level = gl_param_GLint(command, 1);
    GLint xoffset = gl_param_GLint(command, 2);
    GLint yoffset = gl_param_GLint(command, 3);
    GLint zoffset = gl_param_GLint(command, 4);
    GLsizei width = gl_param_GLsizei(command, 5);
    GLsizei height = gl_param_GLsizei(command, 6);
    GLsizei depth = gl_param_GLsizei(command, 7);
    GLenum format = gl_param_GLenum(command, 8);
    GLenum type = gl_param_GLenum(command, 9);
    const void* data = gl_param_data(command, 10);
    real(target, level, xoffset, yoffset, zoffset, width, height, depth, format, type, data);
    replay_update_tex_image(ctx, command, target, level);

glCompressedTexSubImage3D:
    GLenum target = gl_param_GLenum(command, 0);
    GLint level = gl_param_GLint(command, 1);
    GLint xoffset = gl_param_GLint(command, 2);
    GLint yoffset = gl_param_GLint(command, 3);
    GLint zoffset = gl_param_GLint(command, 4);
    GLsizei width = gl_param_GLsizei(command, 5);
    GLsizei height = gl_param_GLsizei(command, 6);
    GLsizei depth = gl_param_GLsizei(command, 7);
    GLenum format = gl_param_GLenum(command, 8);
    GLsizei imageSize = gl_param_GLsizei(command, 9);
    const void* data = gl_param_data(command, 10);
    real(target, level, xoffset, yoffset, zoffset, width, height, depth, format, imageSize, data);
    replay_update_tex_image(ctx, command, target, level);

glTexImage2DMultisample:
    GLenum target = gl_param_GLenum(command, 0);
    GLsizei samples = gl_param_GLsizei(command, 1);
    GLenum internalformat = gl_param_GLenum(command, 2);
    GLsizei width = gl_param_GLsizei(command, 3);
    GLsizei height = gl_param_GLsizei(command, 4);
    GLboolean fixedsamplelocations = gl_param_GLboolean(command, 5);
    real(target, samples, internalformat, width, height, fixedsamplelocations);
    //TODO
    //replay_get_tex_params(ctx, command, target);

glTexImage3DMultisample:
    GLenum target = gl_param_GLenum(command, 0);
    GLsizei samples = gl_param_GLsizei(command, 1);
    GLenum internalformat = gl_param_GLenum(command, 2);
    GLsizei width = gl_param_GLsizei(command, 3);
    GLsizei height = gl_param_GLsizei(command, 4);
    GLsizei depth = gl_param_GLsizei(command, 5);
    GLboolean fixedsamplelocations = gl_param_GLboolean(command, 6);
    real(target, samples, internalformat, width, height, depth, fixedsamplelocations);
    //TODO
    //replay_get_tex_params(ctx, command, target);

glGenerateMipmap:
    GLenum target = gl_param_GLenum(command, 0);
    real(target);
    
    //TODO
    /*GLint w, h, d;
    F(glGetTexLevelParameteriv)(target, 0, GL_TEXTURE_WIDTH, &w);
    F(glGetTexLevelParameteriv)(target, 0, GL_TEXTURE_HEIGHT, &h);
    F(glGetTexLevelParameteriv)(target, 0, GL_TEXTURE_DEPTH, &d);
    switch (target) {
    case GL_TEXTURE_1D:
    case GL_TEXTURE_1D_ARRAY:
        for (GLsizei i = 0; w; i++, w/=2)
            replay_get_tex_data(ctx, command, target, i);
    case GL_TEXTURE_2D:
    case GL_TEXTURE_2D_ARRAY:
    case GL_TEXTURE_CUBE_MAP:
    case GL_TEXTURE_CUBE_MAP_ARRAY:
        for (GLsizei i = 0; w && h; i++, w/=2, h/=2)
            replay_get_tex_data(ctx, command, target, i);
    case GL_TEXTURE_3D:
        for (GLsizei i = 0; w && h && d; i++, w/=2, h/=2, d/=2)
            replay_get_tex_data(ctx, command, target, i);
    }*/

glTexParameterf:
    GLenum target = gl_param_GLenum(command, 0);
    GLenum pname = gl_param_GLenum(command, 1);
    GLfloat param = gl_param_GLfloat(command, 2);
    GLdouble double_param = param;
    if (!texture_param_double(ctx, command, target, pname, 1, &double_param))
        real(target, pname, param);

glTexParameteri:
    GLenum target = gl_param_GLenum(command, 0);
    GLenum pname = gl_param_GLenum(command, 1);
    GLint param = gl_param_GLint(command, 2);
    GLdouble double_param = param;
    if (!texture_param_double(ctx, command, target, pname, 1, &double_param))
        real(target, pname, param);

glTexParameterfv:
    GLenum target = gl_param_GLenum(command, 0);
    GLenum pname = gl_param_GLenum(command, 1);
    
    trace_value_t* paramsv = trc_get_arg(command, 2);
    const double* paramsd = trc_get_double(paramsv);
    GLfloat params[paramsv->count];
    for (size_t i = 0; i < paramsv->count; i++) params[i] = paramsd[i];
    if (!texture_param_double(ctx, command, target, pname, paramsv->count, paramsd))
        real(target, pname, params);

glTexParameteriv:
    GLenum target = gl_param_GLenum(command, 0);
    GLenum pname = gl_param_GLenum(command, 1);
    
    trace_value_t* paramsv = trc_get_arg(command, 2);
    const int64_t* params64 = trc_get_int(paramsv);
    GLint params[paramsv->count];
    double double_params[paramsv->count];
    for (size_t i = 0; i < paramsv->count; i++) params[i] = params64[i];
    for (size_t i = 0; i < paramsv->count; i++) double_params[i] = params64[i];
    
    if (!texture_param_double(ctx, command, target, pname, paramsv->count, double_params))
        real(target, pname, params);

glTexParameterIiv:
    //TODO
    GLenum target = gl_param_GLenum(command, 0);
    GLenum pname = gl_param_GLenum(command, 1);
    
    const int64_t* params64 = trc_get_int(trc_get_arg(command, 2));
    GLint params[4];
    if (pname==GL_TEXTURE_BORDER_COLOR || pname==GL_TEXTURE_SWIZZLE_RGBA)
        for (size_t i = 0; i < 4; i++) params[i] = params64[i];
    else params[0] = params64[0];
    
    real(target, pname, params);

glTexParameterIuiv:
    //TODO
    GLenum target = gl_param_GLenum(command, 0);
    GLenum pname = gl_param_GLenum(command, 1);
    
    const uint64_t* params64 = trc_get_uint(trc_get_arg(command, 2));
    GLuint params[4];
    if (pname==GL_TEXTURE_BORDER_COLOR || pname==GL_TEXTURE_SWIZZLE_RGBA)
        for (size_t i = 0; i < 4; i++) params[i] = params64[i];
    else params[0] = params64[0];
    
    real(target, pname, params);

glGenBuffers:
    GLsizei n = gl_param_GLsizei(command, 0);
    GLuint buffers[n];
    const uint64_t* fake = trc_get_uint(trc_get_arg(command, 1));
    
    real(n, buffers);
    
    for (size_t i = 0; i < n; ++i) {
        trc_gl_buffer_rev_t rev;
        rev.fake_context = trc_get_current_fake_gl_context(ctx->trace);
        rev.ref_count = 1;
        rev.real = buffers[i];
        rev.has_data = false;
        rev.data_usage = 0;
        rev.data = NULL;
        rev.mapped = false;
        rev.map_offset = 0;
        rev.map_length = 0;
        rev.map_access = 0;
        trc_set_gl_buffer(ctx->trace, fake[i], &rev);
    }

glDeleteBuffers:
    GLsizei n = gl_param_GLsizei(command, 0);
    GLuint buffers[n];
    const uint64_t* fake = trc_get_uint(trc_get_arg(command, 1));
    
    for (size_t i = 0; i < n; ++i) {
        if (!(buffers[i] = trc_get_real_gl_buffer(ctx->trace, fake[i])))
            trc_add_error(command, "Invalid buffer name.");
        else trc_rel_gl_obj(ctx->trace, fake[i], TrcGLObj_Buffer);
    }
    
    real(n, buffers);

glBindBuffer:
    GLenum target = gl_param_GLenum(command, 0);
    GLuint fake = gl_param_GLuint(command, 1);
    GLuint real_buf = trc_get_real_gl_buffer(ctx->trace, fake);
    if (!real_buf && fake) ERROR("Invalid buffer name.");
    
    trc_gl_context_rev_t state = *trc_get_gl_context(ctx->trace, 0);
    
    switch (target) {
    case GL_ARRAY_BUFFER: state.array_buffer = fake; break;
    case GL_ATOMIC_COUNTER_BUFFER: state.atomic_counter_buffer = fake; break;
    case GL_COPY_READ_BUFFER: state.copy_read_buffer = fake; break;
    case GL_COPY_WRITE_BUFFER: state.copy_write_buffer = fake; break;
    case GL_DISPATCH_INDIRECT_BUFFER: state.dispatch_indirect_buffer = fake; break;
    case GL_DRAW_INDIRECT_BUFFER: state.draw_indirect_buffer = fake; break;
    case GL_ELEMENT_ARRAY_BUFFER: state.element_array_buffer = fake; break;
    case GL_PIXEL_PACK_BUFFER: state.pixel_pack_buffer = fake; break;
    case GL_PIXEL_UNPACK_BUFFER: state.pixel_unpack_buffer = fake; break;
    case GL_QUERY_BUFFER: state.query_buffer = fake; break;
    case GL_SHADER_STORAGE_BUFFER: state.shader_storage_buffer = fake; break;
    case GL_TEXTURE_BUFFER: state.texture_buffer = fake; break;
    case GL_TRANSFORM_FEEDBACK_BUFFER: state.transform_feedback_buffer = fake; break;
    case GL_UNIFORM_BUFFER: state.uniform_buffer = fake; break;
    }
    trc_set_gl_context(ctx->trace, 0, &state);
    
    real(target, real_buf);

glBindBufferBase:
    GLenum target = gl_param_GLenum(command, 0);
    GLuint index = gl_param_GLuint(command, 1);
    GLuint fake = gl_param_GLuint(command, 2);
    GLuint buf = trc_get_real_gl_buffer(ctx->trace, fake);
    if (!buf && fake) ERROR("Invalid buffer name.");
    real(target, index, buf);

glBindBufferRange:
    GLenum target = gl_param_GLenum(command, 0);
    GLuint index = gl_param_GLuint(command, 1);
    GLuint fake = gl_param_GLuint(command, 2);
    GLuint buf = trc_get_real_gl_buffer(ctx->trace, fake);
    if (!buf && fake) ERROR("Invalid buffer name.");
    int64_t offset = gl_param_GLintptr(command, 3);
    int64_t size = gl_param_GLsizeiptr(command, 4);
    real(target, index, buf, offset, size);

glBufferData:
    GLuint target = gl_param_GLenum(command, 0);
    GLsizeiptr size = gl_param_GLsizeiptr(command, 1);
    const void* data = gl_param_data(command, 2);
    GLenum usage = gl_param_GLenum(command, 3);
    real(target, size, data, usage);
    
    uint fake = get_bound_buffer(ctx, target);
    const trc_gl_buffer_rev_t* buf_rev_ptr = trc_get_gl_buffer(ctx->trace, fake);
    if (!buf_rev_ptr) ERROR("Invalid buffer name or buffer target");
    trc_gl_buffer_rev_t buf = *buf_rev_ptr;
    buf.data = trc_create_inspection_data(ctx->trace, size, data);
    trc_set_gl_buffer(ctx->trace, fake, &buf);

glBufferSubData:
    GLuint target = gl_param_GLenum(command, 0);
    GLintptr offset = gl_param_GLintptr(command, 1);
    GLsizeiptr size = gl_param_GLsizeiptr(command, 2);
    const void* data = gl_param_data(command, 3);
    real(target, offset, size, data);
    
    uint fake = get_bound_buffer(ctx, target);
    const trc_gl_buffer_rev_t* buf_rev_ptr = trc_get_gl_buffer(ctx->trace, fake);
    if (!buf_rev_ptr) ERROR("Invalid buffer name or buffer target");
    trc_gl_buffer_rev_t buf = *buf_rev_ptr;
    if (!buf.data) RETURN; //TODO: Error
    
    trc_gl_buffer_rev_t old = buf;
    
    buf.data = trc_create_inspection_data(ctx->trace, old.data->uncompressed_size, NULL);
    void* newdata = trc_lock_data(buf.data, false, true);
    
    memcpy(newdata, trc_lock_data(old.data, true, false), old.data->uncompressed_size);
    trc_unlock_data(old.data);
    
    memcpy((uint8_t*)newdata+offset, data, size);
    trc_unlock_data(buf.data);
    
    trc_set_gl_buffer(ctx->trace, fake, &buf);

glMapBuffer:
    GLuint target = gl_param_GLenum(command, 0);
    GLuint access = gl_param_GLenum(command, 1);
    
    if (access!=GL_READ_ONLY && access!=GL_WRITE_ONLY && access!=GL_READ_WRITE) 
        ERROR("Invalid access policy");
    
    uint fake = get_bound_buffer(ctx, target);
    const trc_gl_buffer_rev_t* buf_rev_ptr = trc_get_gl_buffer(ctx->trace, fake);
    if (!buf_rev_ptr) ERROR("Invalid buffer name or buffer target");
    trc_gl_buffer_rev_t buf = *buf_rev_ptr;
    if (!buf.data) RETURN; //TODO: Error
    
    if (buf.mapped) ERROR("Buffer is already mapped");
    
    buf.mapped = true;
    buf.map_offset = 0;
    buf.map_length = buf.data->uncompressed_size;
    switch (access) {
    case GL_READ_ONLY: buf.map_access = GL_MAP_READ_BIT; break;
    case GL_WRITE_ONLY: buf.map_access = GL_MAP_WRITE_BIT; break;
    case GL_READ_WRITE: buf.map_access = GL_MAP_READ_BIT | GL_MAP_WRITE_BIT; break;
    }
    
    trc_set_gl_buffer(ctx->trace, fake, &buf);

glMapBufferRange:
    GLuint target = gl_param_GLenum(command, 0);
    GLintptr offset = gl_param_GLintptr(command, 1);
    GLsizeiptr length = gl_param_GLsizeiptr(command, 2);
    GLbitfield access = gl_param_GLbitfield(command, 3);
    
    if (offset<0 || length<0 || !length) ERROR("Invalid length or offset");
    
    if (!(access&GL_MAP_READ_BIT) && !(access&GL_MAP_WRITE_BIT))
        ERROR("Neither GL_MAP_READ_BIT or GL_MAP_WRITE_BIT is set");
    
    if (access&GL_MAP_READ_BIT && (access&GL_MAP_INVALIDATE_RANGE_BIT ||
                                   access&GL_MAP_INVALIDATE_BUFFER_BIT ||
                                   access&GL_MAP_UNSYNCHRONIZED_BIT))
        ERROR("GL_MAP_READ_BIT is set and GL_MAP_INVALIDATE_RANGE_BIT, GL_MAP_INVALIDATE_BUFFER_BIT or GL_MAP_UNSYNCHRONIZED_BIT is set");
    
    if (access&GL_MAP_FLUSH_EXPLICIT_BIT && !(access&GL_MAP_WRITE_BIT))
        ERROR("GL_MAP_FLUSH_EXPLICIT_BIT is set but GL_MAP_WRITE_BIT is not");
    
    if (access&!(GLbitfield)0xff) ERROR("Invalid access flags");
    
    //TODO:
    //Make sure the access is valid with the buffer's storage flags
    
    uint fake = get_bound_buffer(ctx, target);
    const trc_gl_buffer_rev_t* buf_rev_ptr = trc_get_gl_buffer(ctx->trace, fake);
    if (!buf_rev_ptr) ERROR("Invalid buffer name or buffer target");
    trc_gl_buffer_rev_t buf = *buf_rev_ptr;
    if (!buf.data) RETURN; //TODO: Error
    
    if (offset+length > buf.data->uncompressed_size)
        ERROR("offset+length is greater than the buffer's size");
    if (buf.mapped) ERROR("Buffer is already mapped");
    
    buf.mapped = true;
    buf.map_offset = offset;
    buf.map_length = length;
    buf.map_access = access;
    
    trc_set_gl_buffer(ctx->trace, fake, &buf);

glUnmapBuffer:
    GLuint target = gl_param_GLenum(command, 0);
    trace_extra_t* extra = trc_get_extra(command, "replay/glUnmapBuffer/data");
    
    if (!extra) ERROR("replay/glUnmapBuffer/data extra not found");
    
    uint fake = get_bound_buffer(ctx, target);
    const trc_gl_buffer_rev_t* buf_rev_ptr = trc_get_gl_buffer(ctx->trace, fake);
    if (!buf_rev_ptr) ERROR("Invalid buffer name or buffer target");
    trc_gl_buffer_rev_t buf = *buf_rev_ptr;
    if (!buf.data) RETURN; //TODO: Error
    
    if (extra->size != buf.data->uncompressed_size) {
        //TODO
    }
    
    if (!buf.mapped) ERROR("Unmapping a buffer that is not mapped");
    
    F(glBufferSubData)(target, 0, extra->size, extra->data);
    
    trc_gl_buffer_rev_t old = buf;
    
    buf.data = trc_create_inspection_data(ctx->trace, old.data->uncompressed_size, NULL);
    void* newdata = trc_lock_data(buf.data, false, true);
    
    memcpy(newdata, trc_lock_data(old.data, true, false), old.data->uncompressed_size);
    trc_unlock_data(old.data);
    
    memcpy(newdata, extra->data, extra->size);
    trc_unlock_data(buf.data);
    
    buf.mapped = false;
    buf.map_offset = 0;
    buf.map_length = 0;
    buf.map_access = 0;
    
    trc_set_gl_buffer(ctx->trace, fake, &buf);

glCreateShader:
    GLenum type = gl_param_GLenum(command, 0);
    GLuint real_shdr = F(glCreateShader)(type);
    GLuint fake = trc_get_uint(&command->ret)[0];
    trc_gl_shader_rev_t rev;
    rev.fake_context = trc_get_current_fake_gl_context(ctx->trace);
    rev.ref_count = 1;
    rev.real = real_shdr;
    rev.source_count = 0;
    rev.source_lengths = NULL;
    rev.sources = NULL;
    rev.info_log = NULL;
    rev.type = type;
    trc_set_gl_shader(ctx->trace, fake, &rev);

glDeleteShader:
    GLuint fake = gl_param_GLuint(command, 0);
    GLuint real_shdr = trc_get_real_gl_shader(ctx->trace, fake);
    if (!real_shdr) ERROR("Invalid shader name.");
    
    F(glDeleteShader)(real_shdr);
    
    trc_rel_gl_obj(ctx->trace, fake, TrcGLObj_Shader);

glShaderSource:
    GLuint fake = gl_param_GLuint(command, 0);
    GLsizei count = gl_param_GLsizei(command, 1);
    const char*const* sources = gl_param_string_array(command, 2);
    
    GLuint shader = trc_get_real_gl_shader(ctx->trace, fake);
    if (!shader) ERROR("Invalid shader name.");
    
    trc_gl_shader_rev_t shdr = *trc_get_gl_shader(ctx->trace, fake);
    shdr.source_count = count;
    shdr.source_lengths = malloc(count*sizeof(size_t));
    shdr.sources = malloc(count*sizeof(char*));
    
    if (trc_get_arg(command, 3)->count == 0) {
        real(shader, count, (const GLchar*const*)sources, NULL);
        for (GLsizei i = 0; i < count; i++) {
            shdr.source_lengths[i] = strlen(sources[i]);
            shdr.sources[i] = malloc(shdr.source_lengths[i]);
            memcpy(shdr.sources[i], sources[i], shdr.source_lengths[i]);
        }
    } else {
        const uint64_t* lengths64 = trc_get_uint(trc_get_arg(command, 3));
        
        GLint lengths[count];
        for (GLsizei i = 0; i < count; i++) lengths[i] = lengths64[i];
        real(shader, count, (const GLchar*const*)sources, lengths);
        
        for (GLsizei i = 0; i < count; i++) {
            shdr.source_lengths[i] = lengths[i];
            shdr.sources[i] = malloc(lengths[i]);
            memcpy(shdr.sources[i], sources[i], lengths[i]);
        }
    }
    
    trc_set_gl_shader(ctx->trace, fake, &shdr);

glCompileShader:
    GLuint fake = gl_param_GLuint(command, 0);
    GLuint real_shdr = trc_get_real_gl_shader(ctx->trace, fake);
    if (!real_shdr) ERROR("Invalid shader name.");
    
    real(real_shdr);
    
    GLint status;
    F(glGetShaderiv)(real_shdr, GL_COMPILE_STATUS, &status);
    if (!status) ERROR("Failed to compile shader.");
    
    trc_gl_shader_rev_t shdr = *trc_get_gl_shader(ctx->trace, fake);
    
    GLint len;
    F(glGetShaderiv)(real_shdr, GL_INFO_LOG_LENGTH, &len);
    shdr.info_log = malloc(len+1);
    shdr.info_log[len] = 0;
    F(glGetShaderInfoLog)(real_shdr, len, NULL, shdr.info_log);
    
    trc_set_gl_shader(ctx->trace, fake, &shdr);

glCreateProgram:
    GLuint real_program = F(glCreateProgram)();
    GLuint fake = trc_get_uint(&command->ret)[0];
    trc_gl_program_rev_t rev;
    rev.fake_context = trc_get_current_fake_gl_context(ctx->trace);
    rev.ref_count = 1;
    rev.real = real_program;
    rev.uniforms = trc_create_inspection_data(ctx->trace, 0, NULL);
    rev.vertex_attribs = trc_create_inspection_data(ctx->trace, 0, NULL);
    rev.shaders = trc_create_inspection_data(ctx->trace, 0, NULL);
    rev.info_log = NULL;
    trc_set_gl_program(ctx->trace, fake, &rev);

glDeleteProgram:
    GLuint fake = gl_param_GLuint(command, 0);
    GLuint real_program = trc_get_real_gl_program(ctx->trace, fake);
    if (!real_program) ERROR("Invalid program name.");
    
    real(real_program);
    
    trc_rel_gl_obj(ctx->trace, fake, TrcGLObj_Program);

glAttachShader:
    GLuint fake_program = gl_param_GLuint(command, 0);
    GLuint real_program = trc_get_real_gl_program(ctx->trace, fake_program);
    if (!real_program) ERROR("Invalid program name.");
    
    GLuint fake_shader = gl_param_GLuint(command, 1);
    GLuint real_shader = trc_get_real_gl_shader(ctx->trace, fake_shader);
    if (!real_shader) ERROR("Invalid shader name.");
    //TODO: Reference counting
    
    real(real_program, real_shader);
    
    trc_gl_program_rev_t program = *trc_get_gl_program(ctx->trace, fake_program);
    trc_gl_program_rev_t old = program;
    const trc_gl_shader_rev_t* shader = trc_get_gl_shader(ctx->trace, fake_shader);
    
    size_t shader_count = program.shaders->uncompressed_size / sizeof(trc_gl_program_shader_t);
    program.shaders = trc_create_inspection_data(ctx->trace, (shader_count+1)*sizeof(trc_gl_program_shader_t), NULL);
    
    trc_gl_program_shader_t* dest = trc_lock_data(program.shaders, false, true);
    trc_gl_program_shader_t* src = trc_lock_data(old.shaders, true, false);
    memcpy(dest, src, shader_count*sizeof(trc_gl_program_shader_t));
    dest[shader_count].fake_shader = fake_shader;
    dest[shader_count].shader_revision = shader->revision;
    trc_unlock_data(old.shaders);
    trc_unlock_data(program.shaders);
    
    trc_set_gl_program(ctx->trace, fake_program, &program);

glDetachShader:
    GLuint fake_program = gl_param_GLuint(command, 0);
    GLuint real_program = trc_get_real_gl_program(ctx->trace, fake_program);
    if (!real_program) ERROR("Invalid program name.");
    
    GLuint fake_shader = gl_param_GLuint(command, 1);
    GLuint real_shader = trc_get_real_gl_shader(ctx->trace, fake_shader);
    if (!real_shader) ERROR("Invalid shader name.");
    //TODO: Reference counting
    
    real(real_program, real_shader);
    
    trc_gl_program_rev_t program = *trc_get_gl_program(ctx->trace, fake_program);
    trc_gl_program_rev_t old = program;
    
    size_t shader_count = program.shaders->uncompressed_size / sizeof(trc_gl_program_shader_t);
    program.shaders = trc_create_inspection_data(ctx->trace, (shader_count-1)*sizeof(trc_gl_program_shader_t), NULL);
    
    trc_gl_program_shader_t* dest = trc_lock_data(program.shaders, false, true);
    trc_gl_program_shader_t* src = trc_lock_data(old.shaders, true, false);
    size_t next = 0;
    for (size_t i = 0; i < shader_count; i++) {
        if (src[i].fake_shader == fake_shader) continue;
        dest[next++] = src[i];
    }
    trc_unlock_data(old.shaders);
    trc_unlock_data(program.shaders);
    
    trc_set_gl_program(ctx->trace, fake_program, &program);

glLinkProgram:
    GLuint fake = gl_param_GLuint(command, 0);
    GLuint real_program = trc_get_real_gl_program(ctx->trace, fake);
    if (!real_program) ERROR("Invalid program name.");
    
    real(real_program);
    
    GLint status;
    F(glGetProgramiv)(real_program, GL_LINK_STATUS, &status);
    if (!status) ERROR("Failed to link program.");
    
    trc_gl_program_rev_t rev = *trc_get_gl_program(ctx->trace, fake);
    
    GLint len;
    F(glGetProgramiv)(real_program, GL_INFO_LOG_LENGTH, &len);
    rev.info_log = malloc(len+1);
    rev.info_log[len] = 0;
    F(glGetProgramInfoLog)(real_program, len, NULL, rev.info_log);
    
    size_t uniform_count = 0;
    uint* uniforms = NULL;
    trace_extra_t* uniform = NULL;
    size_t i = 0;
    while ((uniform=trc_get_extrai(command, "replay/program/uniform", i++))) {
        if (uniform->size < 8) continue;
        void* data = uniform->data;
        uint32_t fake_loc = le32toh(((uint32_t*)data)[0]);
        uint32_t len = le32toh(((uint32_t*)data)[1]);
        char* name = calloc(1, len+1);
        memcpy(name, (uint8_t*)data+8, len);
        
        GLint real_loc = F(glGetUniformLocation)(real_program, name);
        if (real_loc < 0) {
            trc_add_error(command, "Nonexistent or inactive uniform while adding uniforms.");
        } else {
            uniforms = realloc(uniforms, (uniform_count+1)*sizeof(uint)*2);
            uniforms[uniform_count*2] = real_loc;
            uniforms[uniform_count++*2+1] = fake_loc;
        }
        
        free(name);
    }
    
    size_t vertex_attrib_count = 0;
    uint* vertex_attribs = NULL;
    trace_extra_t* attrib = NULL;
    i = 0;
    while ((attrib=trc_get_extrai(command, "replay/program/vertex_attrib", i++))) {
        if (attrib->size < 8) continue;
        void* data = attrib->data;
        uint32_t fake_loc = le32toh(((uint32_t*)data)[0]);
        uint32_t len = le32toh(((uint32_t*)data)[1]);
        char* name = calloc(1, len+1);
        memcpy(name, (uint8_t*)data+8, len);
        
        GLint real_loc = F(glGetAttribLocation)(real_program, name);
        if (real_loc < 0) {
            trc_add_error(command, "Nonexistent or inactive uniform while adding vertex attributes.");
        } else {
            vertex_attribs = realloc(vertex_attribs, (vertex_attrib_count+1)*sizeof(uint)*2);
            vertex_attribs[vertex_attrib_count*2] = real_loc;
            vertex_attribs[vertex_attrib_count++*2+1] = fake_loc;
        }
        
        free(name);
    }
    
    rev.uniforms = trc_create_inspection_data(ctx->trace, uniform_count*2*sizeof(uint), uniforms);
    rev.vertex_attribs = trc_create_inspection_data(ctx->trace, vertex_attrib_count*2*sizeof(uint),
                                                    vertex_attribs);
    
    trc_set_gl_program(ctx->trace, fake, &rev);

glValidateProgram:
    GLuint fake = gl_param_GLuint(command, 0);
    GLuint real_program = trc_get_real_gl_program(ctx->trace, fake);
    if (!real_program) ERROR("Invalid program name.");
    
    real(real_program);
    
    GLint status;
    F(glGetProgramiv)(real_program, GL_LINK_STATUS, &status);
    if (!status) ERROR("Program validation failed.");
    
    trc_gl_program_rev_t rev = *trc_get_gl_program(ctx->trace, fake);
    
    GLint len;
    F(glGetProgramiv)(real_program, GL_INFO_LOG_LENGTH, &len);
    rev.info_log = malloc(len+1);
    rev.info_log[len] = 0;
    F(glGetProgramInfoLog)(real_program, len, NULL, rev.info_log);
    
    trc_set_gl_program(ctx->trace, fake, &rev);

glUseProgram:
    GLuint fake = gl_param_GLuint(command, 0);
    GLuint real_program = trc_get_real_gl_program(ctx->trace, fake);
    if (!real_program && fake) ERROR("Invalid program name.");
    
    trc_gl_context_rev_t state = *trc_get_gl_context(ctx->trace, 0);
    trc_grab_gl_obj(ctx->trace, fake, TrcGLObj_Program);
    trc_rel_gl_obj(ctx->trace, state.bound_program, TrcGLObj_Program);
    state.bound_program = fake;
    trc_set_gl_context(ctx->trace, 0, &state);
    
    real(real_program);

glIsBuffer:
    ;

glIsProgram:
    ;

glIsQuery:
    ;

glIsShader:
    ;

glIsTexture:
    ;

glIsVertexArray:
    ;

glIsProgramPipeline:
    ;

glIsRenderbuffer:
    ;

glIsSampler:
    ;

glIsSync:
    ;

glIsTransformFeedback:
    ;

glBindAttribLocation:
    GLuint fake_prog = gl_param_GLuint(command, 0);
    GLuint index = gl_param_GLuint(command, 1);
    const GLchar* name = gl_param_string(command, 2);
    
    GLuint program = trc_get_real_gl_program(ctx->trace, fake_prog);
    if (!program) ERROR("Invalid program name.");
    
    real(program, index, name);

glGetAttribLocation:
    GLuint fake = gl_param_GLuint(command, 0);
    const GLchar* name = gl_param_string(command, 1);
    GLuint real_program = trc_get_real_gl_program(ctx->trace, fake);
    if (!real_program) ERROR("Invalid program name.");
    if (real(real_program, name) < 0) ERROR("No such attribute \"%s\".", name);

glGetUniformLocation:
    GLuint fake = gl_param_GLuint(command, 0);
    const GLchar* name = gl_param_string(command, 1);
    GLuint real_program = trc_get_real_gl_program(ctx->trace, fake);
    if (!real_program) ERROR("Invalid program name.");
    if (real(real_program, name) < 0) ERROR("No such uniform \"%s\".", name);

glGetShaderiv:
    GLuint fake = gl_param_GLuint(command, 0);
    GLuint real_shdr = trc_get_real_gl_shader(ctx->trace, fake);
    if (!real_shdr) ERROR("Invalid shader name.");

glGetShaderInfoLog:
    GLuint fake = gl_param_GLuint(command, 0);
    GLuint real_shdr = trc_get_real_gl_shader(ctx->trace, fake);
    if (!real_shdr) ERROR("Invalid shader name.");

glGetShaderSource:
    GLuint fake = gl_param_GLuint(command, 0);
    GLuint real_shdr = trc_get_real_gl_shader(ctx->trace, fake);
    if (!real_shdr) ERROR("Invalid shader name.");

glGetQueryiv:
    ;

glGetQueryObjectiv:
    GLuint fake = gl_param_GLuint(command, 0);
    GLuint real_query = trc_get_real_gl_query(ctx->trace, fake);
    if (!real_query) ERROR("Invalid query name.");

glGetQueryObjectuiv:
    GLuint fake = gl_param_GLuint(command, 0);
    GLuint real_query = trc_get_real_gl_query(ctx->trace, fake);
    if (!real_query) ERROR("Invalid query name.");

glGetProgramInfoLog:
    GLuint fake = gl_param_GLuint(command, 0);
    GLuint real_prog = trc_get_real_gl_program(ctx->trace, fake);
    if (!real_prog) trc_add_error(command, "Invalid program name.");

glGetProgramiv:
    GLuint fake = gl_param_GLuint(command, 0);
    GLuint real_prog = trc_get_real_gl_program(ctx->trace, fake);
    if (!real_prog) ERROR("Invalid program name.");

glGetTexLevelParameterfv:
    ;

glGetTexLevelParameteriv:
    ;

glGetTexParameterfv:
    ;

glGetTexParameteriv:
    ;

glGetPointerv:
    ;

glGetPolygonStipple:
    ;

glGetMinmax:
    ;

glGetMinmaxParameterfv:
    ;

glGetMinmaxParameteriv:
    ;

glGetPixelMapfv:
    ;

glGetPixelMapuiv:
    ;

glGetPixelMapusv:
    ;

glGetSeparableFilter:
    ;

glGetBufferParameteriv:
    ;

glGetBufferPointerv:
    ;

glGetBufferSubData:
    ;

glGetTexImage:
    ;

glGetBooleanv:
    ;

glGetDoublev:
    ;

glGetFloatv:
    ;

glGetIntegerv:
    ;

glGetString:
    ;

glGetStringi:
    ;

glGetVertexAttribdv:
    ;

glGetVertexAttrivfv:
    ;

glGetVertexAttribiv:
    ;

glGetVertexAttribPointerv:
    ;

glGetCompressedTexImage:
    ;

glGetAttachedShaders:
    ;

glGetActiveUniform:
    if (!trc_get_real_gl_program(ctx->trace, gl_param_GLuint(command, 0)))
        ERROR("Invalid program name.");

glGetActiveAttrib:
    if (!trc_get_real_gl_program(ctx->trace, gl_param_GLuint(command, 0)))
        ERROR("Invalid program name.");

glGetBooleanv:
    ;

glGetDoublev:
    ;

glGetFloatv:
    ;

glGetIntegerv:
    ;

glGetInteger64v:
    ;

glGetBooleani_v:
    ;

glGetIntegeri_v:
    ;

glGetFloati_v:
    ;

glGetDouble_v:
    ;

glGetInteger64i_v:
    ;

glReadPixels:
    ;

glGetSamplerParamaterfv:
    if (!trc_get_real_gl_sampler(ctx->trace, gl_param_GLuint(command, 0)))
        ERROR("Invalid sampler name.");

glGetSamplerParamateriv:
    if (!trc_get_real_gl_sampler(ctx->trace, gl_param_GLuint(command, 0)))
        ERROR("Invalid sampler name.");

glGetSamplerParamaterIiv:
    if (!trc_get_real_gl_sampler(ctx->trace, gl_param_GLuint(command, 0)))
        trc_add_error(command, "Invalid sampler name.");

glGetSamplerParamaterIuiv:
    if (!trc_get_real_gl_sampler(ctx->trace, gl_param_GLuint(command, 0)))
        ERROR("Invalid sampler name.");

glUniform1f:
    GLint loc;
    if ((loc=uniform(ctx, command))<0) RETURN;
    real(loc, gl_param_GLfloat(command, 1));

glUniform2f:
    GLint loc;
    if ((loc=uniform(ctx, command))<0) RETURN;
    real(loc, gl_param_GLfloat(command, 1), gl_param_GLfloat(command, 2));

glUniform3f:
    GLint loc;
    if ((loc=uniform(ctx, command))<0) RETURN;
    real(loc,
         gl_param_GLfloat(command, 1),
         gl_param_GLfloat(command, 2),
         gl_param_GLfloat(command, 3));

glUniform4f:
    GLint loc;
    if ((loc=uniform(ctx, command))<0) RETURN;
    real(loc,
         gl_param_GLfloat(command, 1),
         gl_param_GLfloat(command, 2),
         gl_param_GLfloat(command, 3),
         gl_param_GLfloat(command, 4));

glUniform1i:
    GLint loc;
    if ((loc=uniform(ctx, command))<0) RETURN;
    real(loc, gl_param_GLint(command, 1));

glUniform2i:
    GLint loc;
    if ((loc=uniform(ctx, command))<0) RETURN;
    real(loc, gl_param_GLint(command, 1), gl_param_GLint(command, 2));

glUniform3i:
    GLint loc;
    if ((loc=uniform(ctx, command))<0) RETURN;
    real(loc,
         gl_param_GLint(command, 1),
         gl_param_GLint(command, 2),
         gl_param_GLint(command, 3));

glUniform4i:
    GLint loc;
    if ((loc=uniform(ctx, command))<0) RETURN;
    real(loc,
         gl_param_GLint(command, 1),
         gl_param_GLint(command, 2),
         gl_param_GLint(command, 3),
         gl_param_GLint(command, 4));

glUniform1ui:
    GLint loc;
    if ((loc=uniform(ctx, command))<0) RETURN;
    real(loc, gl_param_GLuint(command, 1));

glUniform2ui:
    GLint loc;
    if ((loc=uniform(ctx, command))<0) RETURN;
    real(loc, gl_param_GLuint(command, 1), gl_param_GLuint(command, 2));

glUniform3ui:
    GLint loc;
    if ((loc=uniform(ctx, command))<0) RETURN;
    real(loc,
         gl_param_GLuint(command, 1),
         gl_param_GLuint(command, 2),
         gl_param_GLuint(command, 3));

glUniform4ui:
    GLint loc;
    if ((loc=uniform(ctx, command))<0) RETURN;
    real(loc,
         gl_param_GLuint(command, 1),
         gl_param_GLuint(command, 2),
         gl_param_GLuint(command, 3),
         gl_param_GLuint(command, 4));

glUniform1fv:
    GLint loc;
    if ((loc=uniform(ctx, command))<0) RETURN;
    
    GLsizei count = gl_param_GLint(command, 1);
    GLfloat values[count];
    for (GLsizei i = 0; i < count; i++)
        values[i] = trc_get_double(trc_get_arg(command, 2))[i];
    
    real(loc, count, values);

glUniform2fv:
    GLint loc;
    if ((loc=uniform(ctx, command))<0) RETURN;
    
    GLsizei count = gl_param_GLint(command, 1);
    GLfloat values[count];
    for (GLsizei i = 0; i < count*2; i++)
        values[i] = trc_get_double(trc_get_arg(command, 2))[i];
    
    real(loc, count, values);

glUniform3fv:
    GLint loc;
    if ((loc=uniform(ctx, command))<0) RETURN;
    
    GLsizei count = gl_param_GLint(command, 1);
    GLfloat values[count];
    for (GLsizei i = 0; i < count*3; i++)
        values[i] = trc_get_double(trc_get_arg(command, 2))[i];
    
    real(loc, count, values);

glUniform4fv:
    GLint loc;
    if ((loc=uniform(ctx, command))<0) RETURN;
    
    GLsizei count = gl_param_GLint(command, 1);
    GLfloat values[count];
    for (GLsizei i = 0; i < count*4; i++)
        values[i] = trc_get_double(trc_get_arg(command, 2))[i];
    
    real(loc, count, values);

glUniform1iv:
    GLint loc;
    if ((loc=uniform(ctx, command))<0) RETURN;
    
    GLsizei count = gl_param_GLint(command, 1);
    GLint values[count];
    for (GLsizei i = 0 ; i < count; i++)
        values[i] = trc_get_int(trc_get_arg(command, 2))[i];
    
    real(loc, count, values);

glUniform2iv:
    GLint loc;
    if ((loc=uniform(ctx, command))<0) RETURN;
    
    GLsizei count = gl_param_GLint(command, 1);
    GLint values[count];
    for (GLsizei i = 0 ; i < count*2; i++)
        values[i] = trc_get_int(trc_get_arg(command, 2))[i];
    
    real(loc, count, values);

glUniform3iv:
    GLint loc;
    if ((loc=uniform(ctx, command))<0) RETURN;
    
    GLsizei count = gl_param_GLint(command, 1);
    GLint values[count];
    for (GLsizei i = 0 ; i < count*3; i++)
        values[i] = trc_get_int(trc_get_arg(command, 2))[i];
    
    real(loc, count, values);

glUniform4iv:
    GLint loc;
    if ((loc=uniform(ctx, command))<0) RETURN;
    
    GLsizei count = gl_param_GLint(command, 1);
    GLint values[count];
    for (GLsizei i = 0 ; i < count*4; i++)
        values[i] = trc_get_int(trc_get_arg(command, 2))[i];
    
    real(loc, count, values);

glUniform1uiv:
    GLint loc;
    if ((loc=uniform(ctx, command))<0) RETURN;
    
    GLsizei count = gl_param_GLint(command, 1);
    GLuint values[count];
    for (GLsizei i = 0 ; i < count; i++)
        values[i] = trc_get_uint(trc_get_arg(command, 2))[i];
    
    real(loc, count, values);

glUniform2uiv:
    GLint loc;
    if ((loc=uniform(ctx, command))<0) RETURN;
    
    GLsizei count = gl_param_GLint(command, 1);
    GLuint values[count];
    for (GLsizei i = 0 ; i < count*2; i++)
        values[i] = trc_get_uint(trc_get_arg(command, 2))[i];
    
    real(loc, count, values);

glUniform3uiv:
    GLint loc;
    if ((loc=uniform(ctx, command))<0) RETURN;
    
    GLsizei count = gl_param_GLint(command, 1);
    GLuint values[count];
    for (GLsizei i = 0 ; i < count*3; i++)
        values[i] = trc_get_uint(trc_get_arg(command, 2))[i];
    
    real(loc, count, values);

glUniform4uiv:
    GLint loc;
    if ((loc=uniform(ctx, command))<0) RETURN;
    
    GLsizei count = gl_param_GLint(command, 1);
    GLuint values[count];
    for (GLsizei i = 0 ; i < count*4; i++)
        values[i] = trc_get_uint(trc_get_arg(command, 2))[i];
    
    real(loc, count, values);

glUniformMatrix2fv:
    GLint loc;
    if ((loc=uniform(ctx, command))<0) RETURN;
    
    GLsizei count = gl_param_GLint(command, 1);
    GLfloat values[count*4];
    for (GLsizei i = 0 ; i < count*4; i++)
        values[i] = trc_get_double(trc_get_arg(command, 3))[i];
    
    real(loc, count, gl_param_GLboolean(command, 2), values);

glUniformMatrix3fv:
    GLint loc;
    if ((loc=uniform(ctx, command))<0) RETURN;
    
    GLsizei count = gl_param_GLint(command, 1);
    GLfloat values[count*9];
    for (GLsizei i = 0 ; i < count*9; i++)
        values[i] = trc_get_double(trc_get_arg(command, 3))[i];
    
    real(loc, count, gl_param_GLboolean(command, 2), values);

glUniformMatrix4fv:
    GLint loc;
    if ((loc=uniform(ctx, command))<0) RETURN;
    
    GLsizei count = gl_param_GLint(command, 1);
    GLfloat values[count*16];
    for (GLsizei i = 0 ; i < count*16; i++)
        values[i] = trc_get_double(trc_get_arg(command, 3))[i];
    
    real(loc, count, gl_param_GLboolean(command, 2), values);

glUniformMatrix2x3fv:
    GLint loc;
    if ((loc=uniform(ctx, command))<0) RETURN;
    
    GLsizei count = gl_param_GLint(command, 1);
    GLfloat values[count*6];
    for (GLsizei i = 0 ; i < count*6; i++)
        values[i] = trc_get_double(trc_get_arg(command, 3))[i];
    
    real(loc, count, gl_param_GLboolean(command, 2), values);

glUniformMatrix3x2fv:
    GLint loc;
    if ((loc=uniform(ctx, command))<0) RETURN;
    
    GLsizei count = gl_param_GLint(command, 1);
    GLfloat values[count*6];
    for (GLsizei i = 0 ; i < count*6; i++)
        values[i] = trc_get_double(trc_get_arg(command, 3))[i];
    
    real(loc, count, gl_param_GLboolean(command, 2), values);

glUniformMatrix2x4fv:
    GLint loc;
    if ((loc=uniform(ctx, command))<0) RETURN;
    
    GLsizei count = gl_param_GLint(command, 1);
    GLfloat values[count*8];
    for (GLsizei i = 0 ; i < count*8; i++)
        values[i] = trc_get_double(trc_get_arg(command, 3))[i];
    
    real(loc, count, gl_param_GLboolean(command, 2), values);

glUniformMatrix4x2fv:
    GLint loc;
    if ((loc=uniform(ctx, command))<0) RETURN;
    
    GLsizei count = gl_param_GLint(command, 1);
    GLfloat values[count*8];
    for (GLsizei i = 0 ; i < count*8; i++)
        values[i] = trc_get_double(trc_get_arg(command, 3))[i];
    
    real(loc, count, gl_param_GLboolean(command, 2), values);

glUniformMatrix3x4fv:
    GLint loc;
    if ((loc=uniform(ctx, command))<0) RETURN;
    
    GLsizei count = gl_param_GLint(command, 1);
    GLfloat values[count*12];
    for (GLsizei i = 0 ; i < count*12; i++)
        values[i] = trc_get_double(trc_get_arg(command, 3))[i];
    
    real(loc, count, gl_param_GLboolean(command, 2), values);

glUniformMatrix4x3fv:
    GLint loc;
    if ((loc=uniform(ctx, command))<0) RETURN;
    
    GLsizei count = gl_param_GLint(command, 1);
    GLfloat values[count*12];
    for (GLsizei i = 0 ; i < count*12; i++)
        values[i] = trc_get_double(trc_get_arg(command, 3))[i];
    
    real(loc, count, gl_param_GLboolean(command, 2), values);

glVertexAttribPointer:
    GLuint index = gl_param_GLuint(command, 0);
    GLint size = gl_param_GLint(command, 1);
    GLenum type = gl_param_GLenum(command, 2);
    GLboolean normalized = gl_param_GLboolean(command, 3);
    GLsizei stride = gl_param_GLsizei(command, 4);
    uint64_t pointer = gl_param_pointer(command, 5);
    //if (pointer > UINTPTR_MAX) //TODO
    real(index, size, type, normalized, stride, (const GLvoid*)(uintptr_t)pointer);
    trc_gl_vao_rev_t rev = *trc_get_gl_vao(ctx->trace, trc_get_gl_context(ctx->trace, 0)->bound_vao);
    if (gl_param_GLint(command, 0) < rev.attrib_count) {
        trc_gl_vao_attrib_t* a = &rev.attribs[gl_param_GLint(command, 0)];
        a->normalized = normalized;
        a->integer = false;
        a->size = size;
        a->stride = stride;
        a->offset = pointer;
        a->type = gl_param_GLenum(command, 2);
        a->buffer = trc_get_gl_context(ctx->trace, 0)->array_buffer;
    }
    trc_set_gl_vao(ctx->trace, trc_get_gl_context(ctx->trace, 0)->bound_vao, &rev);

glVertexAttribIPointer:
    GLuint index = gl_param_GLuint(command, 0);
    GLint size = gl_param_GLint(command, 1);
    GLenum type = gl_param_GLenum(command, 2);
    GLsizei stride = gl_param_GLsizei(command, 3);
    uint64_t pointer = gl_param_pointer(command, 4);
    //if (pointer > UINTPTR_MAX) //TODO
    real(index, size, type, stride, (const GLvoid*)(uintptr_t)pointer);
    trc_gl_vao_rev_t rev = *trc_get_gl_vao(ctx->trace, trc_get_gl_context(ctx->trace, 0)->bound_vao);
    if (gl_param_GLint(command, 0) < rev.attrib_count) {
        trc_gl_vao_attrib_t* a = &rev.attribs[gl_param_GLint(command, 0)];
        a->integer = true;
        a->size = size;
        a->stride = stride;
        a->offset = pointer;
        a->type = type;
        a->buffer = trc_get_gl_context(ctx->trace, 0)->array_buffer;
    }
    trc_set_gl_vao(ctx->trace, trc_get_gl_context(ctx->trace, 0)->bound_vao, &rev);

glEnableVertexAttribArray:
    real(gl_param_GLint(command, 0));
    trc_gl_vao_rev_t rev = *trc_get_gl_vao(ctx->trace, trc_get_gl_context(ctx->trace, 0)->bound_vao);
    if (gl_param_GLint(command, 0) < rev.attrib_count)
        rev.attribs[gl_param_GLint(command, 0)].enabled = true;
    trc_set_gl_vao(ctx->trace, trc_get_gl_context(ctx->trace, 0)->bound_vao, &rev);

glDisableVertexAttribArray:
    real(gl_param_GLint(command, 0));
    trc_gl_vao_rev_t rev = *trc_get_gl_vao(ctx->trace, trc_get_gl_context(ctx->trace, 0)->bound_vao);
    if (gl_param_GLint(command, 0) < rev.attrib_count)
        rev.attribs[gl_param_GLint(command, 0)].enabled = false;
    trc_set_gl_vao(ctx->trace, trc_get_gl_context(ctx->trace, 0)->bound_vao, &rev);

glDrawArrays:
    begin_draw(ctx);
    
    GLenum mode = gl_param_GLenum(command, 0);
    GLint first = gl_param_GLint(command, 1);
    GLsizei count = gl_param_GLsizei(command, 2);
    
    real(mode, first, count);
    
    end_draw(ctx, command);

glDrawArraysInstanced:
    begin_draw(ctx);
    
    GLenum mode = gl_param_GLenum(command, 0);
    GLint first = gl_param_GLint(command, 1);
    GLsizei count = gl_param_GLsizei(command, 2);
    GLsizei primcount = gl_param_GLsizei(command, 3);
    
    real(mode, first, count, primcount);
    
    end_draw(ctx, command);

glMultiDrawArrays:
    begin_draw(ctx);
    
    GLenum mode = gl_param_GLenum(command, 0);
    const int64_t* first64 = trc_get_int(trc_get_arg(command, 1));
    const int64_t* count64 = trc_get_int(trc_get_arg(command, 2));
    GLsizei primcount = gl_param_GLsizei(command, 3);
    
    GLint first[primcount];
    GLint count[primcount];
    for (GLsizei i = 0; i < primcount; i++) {
        first[i] = first64[i];
        count[i] = count64[i];
    }
    
    real(mode, first, count, primcount);
    
    end_draw(ctx, command);

glMultiDrawElements:
    begin_draw(ctx);
    
    GLenum mode = gl_param_GLenum(command, 0);
    const int64_t* count64 = trc_get_int(trc_get_arg(command, 1));
    GLenum type = gl_param_GLenum(command, 2);
    const uint64_t* indicesi = trc_get_ptr(trc_get_arg(command, 3));
    GLsizei drawcount = gl_param_GLsizei(command, 4);
    
    GLint count[drawcount];
    const GLvoid* indices[drawcount];
    for (GLsizei i = 0; i < drawcount; i++) {
        count[i] = count64[i];
        indices[i] = (const GLvoid*)indicesi[i];
    }
    
    real(mode, count, type, indices, drawcount);
    
    end_draw(ctx, command);

glMultiDrawElementsBaseVertex:
    begin_draw(ctx);
    
    GLenum mode = gl_param_GLenum(command, 0);
    const int64_t* count64 = trc_get_int(trc_get_arg(command, 1));
    GLenum type = gl_param_GLenum(command, 2);
    const uint64_t* indicesi = trc_get_ptr(trc_get_arg(command, 3));
    GLsizei drawcount = gl_param_GLsizei(command, 4);
    const int64_t* basevertex64 = trc_get_int(trc_get_arg(command, 5));
    
    GLint count[drawcount];
    const GLvoid* indices[drawcount];
    GLint basevertex[drawcount];
    for (GLsizei i = 0; i < drawcount; i++) {
        count[i] = count64[i];
        indices[i] = (const GLvoid*)indicesi[i];
        basevertex[i] = basevertex64[i];
    }
    
    real(mode, count, type, indices, drawcount, basevertex);
    
    end_draw(ctx, command);

glDrawElements:
    begin_draw(ctx);
    
    GLenum mode = gl_param_GLenum(command, 0);
    GLsizei count = gl_param_GLsizei(command, 1);
    GLenum type = gl_param_GLenum(command, 2);
    const GLvoid* indices = (const GLvoid*)gl_param_pointer(command, 3);
    
    real(mode, count, type, indices);
    
    end_draw(ctx, command);

glDrawElementsBaseVertex:
    begin_draw(ctx);
    
    GLenum mode = gl_param_GLenum(command, 0);
    GLsizei count = gl_param_GLsizei(command, 1);
    GLenum type = gl_param_GLenum(command, 2);
    const GLvoid* indices = (const GLvoid*)gl_param_pointer(command, 3);
    GLint basevertex = gl_param_GLint(command, 4);
    
    real(mode, count, type, indices, basevertex);
    
    end_draw(ctx, command);

glDrawElementsInstanced:
    begin_draw(ctx);
    
    GLenum mode = gl_param_GLenum(command, 0);
    GLsizei count = gl_param_GLsizei(command, 1);
    GLenum type = gl_param_GLenum(command, 2);
    const GLvoid* indices = (const GLvoid*)gl_param_pointer(command, 3);
    GLsizei primcount = gl_param_GLsizei(command, 4);
    
    real(mode, count, type, indices, primcount);
    
    end_draw(ctx, command);

glDrawElementsInstancedBaseVertex:
    begin_draw(ctx);
    
    GLenum mode = gl_param_GLenum(command, 0);
    GLsizei count = gl_param_GLsizei(command, 1);
    GLenum type = gl_param_GLenum(command, 2);
    const GLvoid* indices = (const GLvoid*)gl_param_pointer(command, 3);
    GLsizei primcount = gl_param_GLsizei(command, 4);
    GLint basevertex = gl_param_GLint(command, 4);
    
    real(mode, count, type, indices, primcount, basevertex);
    
    end_draw(ctx, command);

glDrawRangeElements:
    begin_draw(ctx);
    
    GLenum mode = gl_param_GLenum(command, 0);
    GLuint start = gl_param_GLuint(command, 1);
    GLuint end = gl_param_GLuint(command, 2);
    GLsizei count = gl_param_GLsizei(command, 3);
    GLenum type = gl_param_GLenum(command, 4);
    const GLvoid* indices = (const GLvoid*)gl_param_pointer(command, 5);
    
    real(mode, start, end, count, type, indices);
    
    end_draw(ctx, command);

glDrawRangeElementsBaseVertex:
    begin_draw(ctx);
    
    GLenum mode = gl_param_GLenum(command, 0);
    GLuint start = gl_param_GLuint(command, 1);
    GLuint end = gl_param_GLuint(command, 2);
    GLsizei count = gl_param_GLsizei(command, 3);
    GLenum type = gl_param_GLenum(command, 4);
    const GLvoid* indices = (const GLvoid*)gl_param_pointer(command, 5);
    GLint basevertex = gl_param_GLint(command, 6);
    
    real(mode, start, end, count, type, indices, basevertex);
    
    end_draw(ctx, command);

glTestFBWIP15:
    F(glFinish)();
    
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
    
    if (memcmp(back, gl_param_data(command, 1), w*h*4) != 0)
        fprintf(stderr, "%s did not result in the correct back color buffer (test: %s).\n", gl_param_string(command, 0), ctx->current_test_name);
    
    //TODO
    /*for (int32_t i = 0; i < 100*100; i++)
        if ((int64_t)depth[i] - (int64_t)((int32_t*)gl_param_data(command, 2))[i] > 16843009) {
            fprintf(stderr, "%s did not result in the correct depth buffer (test: %s).\n", gl_param_string(command, 0), ctx->current_test_name);
            break;
        }*/
    
    free(back);
    free(depth);

glCurrentTestWIP15:
    ctx->current_test_name = gl_param_string(command, 0);

glGenVertexArrays:
    GLsizei n = gl_param_GLsizei(command, 0);
    GLuint arrays[n];
    const uint64_t* fake = trc_get_uint(trc_get_arg(command, 1));
    
    real(n, arrays);
    
    GLint attrib_count;
    F(glGetIntegerv)( GL_MAX_VERTEX_ATTRIBS, &attrib_count);
    for (size_t i = 0; i < n; ++i) {
        trc_gl_vao_rev_t rev;
        rev.fake_context = trc_get_current_fake_gl_context(ctx->trace);
        rev.ref_count = 1;
        rev.real = arrays[i];
        rev.attrib_count = attrib_count;
        rev.attribs = malloc(attrib_count*sizeof(trc_gl_vao_attrib_t));
        for (size_t j = 0; j < attrib_count; j++) {
            rev.attribs[j].enabled = false;
            rev.attribs[j].normalized = false;
            rev.attribs[j].integer = false;
            rev.attribs[j].size = 4;
            rev.attribs[j].stride = 0;
            rev.attribs[j].offset = 0;
            rev.attribs[j].type = GL_FLOAT;
            rev.attribs[j].divisor = 0;
            rev.attribs[j].buffer = 0;
            rev.attribs[j].value[0] = 0;
            rev.attribs[j].value[1] = 0;
            rev.attribs[j].value[2] = 0;
            rev.attribs[j].value[3] = 1;
        }
        trc_set_gl_vao(ctx->trace, fake[i], &rev);
    }

glDeleteVertexArrays:
    GLsizei n = gl_param_GLsizei(command, 0);
    GLuint arrays[n];
    const uint64_t* fake = trc_get_uint(trc_get_arg(command, 1));
    
    for (size_t i = 0; i < n; ++i)
        if (!(arrays[i] = trc_get_real_gl_vao(ctx->trace, fake[i])))
            trc_add_error(command, "Invalid vertex array name.");
        else trc_rel_gl_obj(ctx->trace, fake[i], TrcGLObj_VAO);
    
    real(n, arrays);

glBindVertexArray:
    GLuint fake = gl_param_GLuint(command, 0);
    GLuint real_vao = trc_get_real_gl_vao(ctx->trace, fake);
    if (!real_vao && fake) ERROR("Invalid vertex array name.");
    
    trc_gl_context_rev_t state = *trc_get_gl_context(ctx->trace, 0);
    trc_grab_gl_obj(ctx->trace, fake, TrcGLObj_VAO);
    trc_rel_gl_obj(ctx->trace, state.bound_vao, TrcGLObj_VAO);
    state.bound_vao = fake;
    trc_set_gl_context(ctx->trace, 0, &state);

glPatchParameterfv:
    GLenum pname = gl_param_GLenum(command, 0);
    GLfloat values[trc_get_arg(command, 1)->count];
    for (size_t i = 0; i < trc_get_arg(command, 1)->count; i++)
        values[i] = trc_get_double(trc_get_arg(command, 1))[i];
    real(pname, values);

glGetFragDataIndex:
    GLuint fake = gl_param_GLuint(command, 0);
    GLuint real_program = trc_get_real_gl_program(ctx->trace, fake);
    if (!real_program) ERROR("Invalid program name.");

glGetFragDataLocation:
    GLuint fake = gl_param_GLuint(command, 0);
    GLuint real_program = trc_get_real_gl_program(ctx->trace, fake);
    if (!real_program) ERROR("Invalid program name.");

glGetUniformBlockIndex:
    GLuint fake = gl_param_GLuint(command, 0);
    GLuint real_program = trc_get_real_gl_program(ctx->trace, fake);
    if (!real_program) ERROR("Invalid program name.");

glGetUniformIndices:
    GLuint fake = gl_param_GLuint(command, 0);
    GLuint real_program = trc_get_real_gl_program(ctx->trace, fake);
    if (!real_program) ERROR("Invalid program name.");

glDrawableSizeWIP15:
    GLsizei w = gl_param_GLsizei(command, 0);
    GLsizei h = gl_param_GLsizei(command, 1);
    
    if (w < 0) w = 100;
    if (h < 0) h = 100;
    
    SDL_SetWindowSize(ctx->window, w, h);
    
    trc_gl_context_rev_t state = *trc_get_gl_context(ctx->trace, 0);
    if (state.drawable_width==w && state.drawable_height==h) return;
    state.drawable_width = w;
    state.drawable_height = h;
    replay_create_context_buffers(ctx->trace, &state);
    trc_set_gl_context(ctx->trace, 0, &state);
    
    //TODO: This is a hack
    F(glViewport)(0, 0, w, h);

glGetUniformfv:
    get_uniform(ctx, command);

glGetUniformiv:
    get_uniform(ctx, command);

glGetUniformuiv:
    get_uniform(ctx, command);

glGetUniformdv:
    get_uniform(ctx, command);

glGetnUniformfv:
    get_uniform(ctx, command);

glGetnUniformiv:
    get_uniform(ctx, command);

glGetnUniformuiv:
    get_uniform(ctx, command);

glGetnUniformdv:
    get_uniform(ctx, command);

glGetMultisamplefv:
    ;

glGetInternalFormativ:
    GLenum target = gl_param_GLenum(command, 0);
    GLenum internalformat = gl_param_GLenum(command, 1);
    GLenum pname = gl_param_Glenum(command, 2);
    GLsizei bufSize = gl_param_GLsizei(command, 3);
    GLubyte params[bufSize];
    real(target, internalformat, pname, bufSize, params);

glGetInternalFormati64v:
    GLenum target = gl_param_GLenum(command, 0);
    GLenum internalformat = gl_param_GLenum(command, 1);
    GLenum pname = gl_param_Glenum(command, 2);
    GLsizei bufSize = gl_param_GLsizei(command, 3);
    GLubyte params[bufSize];
    real(target, internalformat, pname, bufSize, params);

glGetBufferParameteriv:
    GLint i;
    real(gl_param_GLenum(command, 0), gl_param_GLenum(command, 1), &i);

glGetBufferPointerv:
    GLvoid* p;
    real(gl_param_GLenum(command, 1), gl_param_GLenum(command, 1), &p);

glGenSamplers:
    GLsizei n = gl_param_GLsizei(command, 0);
    GLuint samplers[n];
    const uint64_t* fake = trc_get_uint(trc_get_arg(command, 1));
    
    real(n, samplers);
    
    for (size_t i = 0; i < n; ++i) {
        trc_gl_sampler_rev_t rev;
        rev.fake_context = trc_get_current_fake_gl_context(ctx->trace);
        rev.ref_count = 1;
        rev.real = samplers[i];
        trc_set_gl_sampler(ctx->trace, fake[i], &rev);
    }

glDeleteSamplers:
    GLsizei n = gl_param_GLsizei(command, 0);
    GLuint samplers[n];
    const uint64_t* fake = trc_get_uint(trc_get_arg(command, 1));
    
    for (size_t i = 0; i < n; ++i)
        if (!(samplers[i] = trc_get_real_gl_sampler(ctx->trace, fake[i])))
            trc_add_error(command, "Invalid sampler name.");
        else trc_rel_gl_obj(ctx->trace, fake[i], TrcGLObj_Sampler);
    
    real(n, samplers);

glBindSampler:
    GLuint unit = gl_param_GLuint(command, 0);
    GLuint fake = gl_param_GLuint(command, 1);
    GLuint real_tex = trc_get_real_gl_sampler(ctx->trace, fake);
    if (!real_tex && fake) ERROR("Invalid sampler name.");
    real(unit, real_tex);
    //TODO: Reference counting

glPointParameterfv:
    F(glPointParameterf)(gl_param_GLenum(command, 0), gl_param_GLfloat(command, 1));

glPointParameteriv:
    F(glPointParameteri)(gl_param_GLenum(command, 0), gl_param_GLint(command, 1));

glGetSynciv:
    if (!trc_get_real_gl_sync(ctx->trace, gl_param_GLsync(command, 0)))
        ERROR("Invalid sync name.");

glSamplerParameterf:
    GLuint fake = gl_param_GLuint(command, 0);
    GLuint sampler = trc_get_real_gl_sampler(ctx->trace, fake);
    if (!sampler) ERROR("Invalid sampler name.");
    GLenum pname = gl_param_GLenum(command, 1);
    GLfloat param = gl_param_GLfloat(command, 2);
    real(sampler, pname, param);

glSamplerParameteri:
    GLuint fake = gl_param_GLuint(command, 0);
    GLuint sampler = trc_get_real_gl_sampler(ctx->trace, fake);
    if (!sampler) ERROR("Invalid sampler name.");
    GLenum pname = gl_param_GLenum(command, 1);
    GLint param = gl_param_GLint(command, 2);
    real(sampler, pname, param);

glSamplerParameterfv:
    GLuint fake = gl_param_GLuint(command, 0);
    GLuint sampler = trc_get_real_gl_sampler(ctx->trace, fake);
    if (!sampler) ERROR("Invalid sampler name.");
    GLenum pname = gl_param_GLenum(command, 1);
    const double* paramsd = trc_get_double(trc_get_arg(command, 2));
    
    GLfloat params[4];
    if (pname == GL_TEXTURE_BORDER_COLOR)
        for (size_t i = 0; i < 4; i++) params[i] = paramsd[i];
    else params[0] = paramsd[0];
    
    real(sampler, pname, params);

glSamplerParameteriv:
    GLuint fake = gl_param_GLuint(command, 0);
    GLuint sampler = trc_get_real_gl_sampler(ctx->trace, fake);
    if (!sampler) ERROR("Invalid sampler name.");
    GLenum pname = gl_param_GLenum(command, 1);
    const int64_t* params64 = trc_get_int(trc_get_arg(command, 2));
    
    GLint params[4];
    if (pname == GL_TEXTURE_BORDER_COLOR)
        for (size_t i = 0; i < 4; i++) params[i] = params64[i];
    else params[0] = params64[0];
    
    real(sampler, pname, params);

glSamplerParameterIiv:
    GLuint fake = gl_param_GLuint(command, 0);
    GLuint sampler = trc_get_real_gl_sampler(ctx->trace, fake);
    if (!sampler) ERROR("Invalid sampler name.");
    GLenum pname = gl_param_GLenum(command, 1);
    const int64_t* params64 = trc_get_int(trc_get_arg(command, 2));
    
    GLint params[4];
    if (pname == GL_TEXTURE_BORDER_COLOR)
        for (size_t i = 0; i < 4; i++) params[i] = params64[i];
    else params[0] = params64[0];
    
    real(sampler, pname, params);

glSamplerParameterIuiv:
    GLuint fake = gl_param_GLuint(command, 0);
    GLuint sampler = trc_get_real_gl_sampler(ctx->trace, fake);
    if (!sampler) ERROR("Invalid sampler name.");
    GLenum pname = gl_param_GLenum(command, 1);
    const uint64_t* params64 = trc_get_uint(trc_get_arg(command, 2));
    
    GLuint params[4];
    if (pname == GL_TEXTURE_BORDER_COLOR)
        for (size_t i = 0; i < 4; i++) params[i] = params64[i];
    else params[0] = params64[0];
    
    real(sampler, pname, params);

glGenFramebuffers:
    GLsizei n = gl_param_GLsizei(command, 0);
    GLuint fbs[n];
    const uint64_t* fake = trc_get_uint(trc_get_arg(command, 1));
    
    real(n, fbs);
    
    for (size_t i = 0; i < n; ++i) {
        trc_gl_framebuffer_rev_t rev;
        rev.fake_context = trc_get_current_fake_gl_context(ctx->trace);
        rev.ref_count = 1;
        rev.real = fbs[i];
        rev.attachments = trc_create_inspection_data(ctx->trace, 0, NULL);
        trc_set_gl_framebuffer(ctx->trace, fake[i], &rev);
    }

glDeleteFramebuffers:
    GLsizei n = gl_param_GLsizei(command, 0);
    GLuint fbs[n];
    const uint64_t* fake = trc_get_uint(trc_get_arg(command, 1));
    
    for (size_t i = 0; i < n; ++i)
        if (!(fbs[i] = trc_get_real_gl_framebuffer(ctx->trace, fake[i])))
            trc_add_error(command, "Invalid framebuffer name.");
        else trc_rel_gl_obj(ctx->trace, fake[i], TrcGLObj_Framebuffer);
    
    real(n, fbs);

glBindFramebuffer:
    GLenum target = gl_param_GLenum(command, 0);
    GLuint fake = gl_param_GLuint(command, 1);
    GLuint fb = trc_get_real_gl_framebuffer(ctx->trace, fake);
    if (!fb && fake) ERROR("Invalid framebuffer name.");
    real(target, fb);
    
    bool read = true;
    bool draw = true;
    switch (target) {
    case GL_READ_FRAMEBUFFER: draw = false; break;
    case GL_DRAW_FRAMEBUFFER: read = false; break;
    }
    trc_gl_context_rev_t state = *trc_get_gl_context(ctx->trace, 0);
    if (read) {
        trc_grab_gl_obj(ctx->trace, fake, TrcGLObj_Framebuffer);
        trc_rel_gl_obj(ctx->trace, state.read_framebuffer, TrcGLObj_Framebuffer);
        state.read_framebuffer = fake;
    }
    if (draw) {
        trc_grab_gl_obj(ctx->trace, fake, TrcGLObj_Framebuffer);
        trc_rel_gl_obj(ctx->trace, state.draw_framebuffer, TrcGLObj_Framebuffer);
        state.draw_framebuffer = fake;
    }
    trc_set_gl_context(ctx->trace, 0, &state);

glGenRenderbuffers:
    GLsizei n = gl_param_GLsizei(command, 0);
    GLuint rbs[n];
    const uint64_t* fake = trc_get_uint(trc_get_arg(command, 1));
    
    real(n, rbs);
    
    for (size_t i = 0; i < n; ++i) {
        trc_gl_renderbuffer_rev_t rev;
        rev.fake_context = trc_get_current_fake_gl_context(ctx->trace);
        rev.ref_count = 1;
        rev.real = rbs[i];
        rev.has_storage = false;
        trc_set_gl_renderbuffer(ctx->trace, fake[i], &rev);
    }

glDeleteRenderbuffers:
    GLsizei n = gl_param_GLsizei(command, 0);
    GLuint rbs[n];
    const uint64_t* fake = trc_get_uint(trc_get_arg(command, 1));
    
    for (size_t i = 0; i < n; ++i)
        if (!(rbs[i] = trc_get_real_gl_renderbuffer(ctx->trace, fake[i])))
            trc_add_error(command, "Invalid renderbuffer name.");
        else trc_rel_gl_obj(ctx->trace, fake[i], TrcGLObj_Renderbuffer);
    
    real(n, rbs);

glBindRenderbuffer:
    GLenum target = gl_param_GLenum(command, 0);
    GLuint fake = gl_param_GLuint(command, 1);
    GLuint rb = trc_get_real_gl_renderbuffer(ctx->trace, fake);
    if (!rb && fake) ERROR("Invalid renderbuffer name.");
    real(target, rb);
    const trc_gl_context_rev_t* state = trc_get_gl_context(ctx->trace, 0);
    trc_gl_context_rev_t newstate = *state;
    newstate.bound_renderbuffer = rb;
    trc_set_gl_context(ctx->trace, 0, &newstate);

glGetActiveUniformBlockiv:
    GLuint fake = gl_param_GLuint(command, 0);
    GLuint program = trc_get_real_gl_program(ctx->trace, fake);
    if (!program) ERROR("Invalid program .");
    GLuint uniformBlockIndex = gl_param_GLuint(command, 1);
    GLenum pname = gl_param_GLenum(command, 2);
    
    if (pname == GL_UNIFORM_BLOCK_ACTIVE_UNIFORM_INDICES) {
        GLint count;
        real(program, uniformBlockIndex, GL_UNIFORM_BLOCK_ACTIVE_UNIFORMS, &count);
        
        GLint* vals = malloc(sizeof(GLint)*count);
        real(program, uniformBlockIndex, pname, vals);
        free(vals);
    } else {
        GLint v;
        real(program, uniformBlockIndex, pname, &v);
    }

glGetActiveUniformBlockName:
    GLuint fake = gl_param_GLuint(command, 0);
    GLuint program = trc_get_real_gl_program(ctx->trace, fake);
    if (!program) ERROR("Invalid program name.");
    GLuint uniformBlockIndex = gl_param_GLuint(command, 1);
    GLchar buf[64];
    real(program, uniformBlockIndex, 64, NULL, buf);

glGetActiveUniformName:
    GLuint fake = gl_param_GLuint(command, 0);
    GLuint program = trc_get_real_gl_program(ctx->trace, fake);
    if (!program) ERROR("Invalid program name.");
    GLuint uniformIndex = gl_param_GLuint(command, 1);
    GLchar buf[64];
    real(program, uniformIndex, 64, NULL, buf);

glGetActiveUniformsiv:
    GLuint fake = gl_param_GLuint(command, 0);
    GLuint program = trc_get_real_gl_program(ctx->trace, fake);
    if (!program) ERROR("Invalid program name.");
    GLsizei uniformCount = gl_param_GLsizei(command, 1);
    const uint64_t* uniformIndices64 = trc_get_uint(trc_get_arg(command, 2));
    GLenum pname = gl_param_GLenum(command, 3);
    
    GLuint* uniformIndices = malloc(uniformCount*sizeof(GLuint));
    for (GLsizei i = 0; i < uniformCount; i++) uniformIndices[i] = uniformIndices64[i];
    
    GLint* params = malloc(uniformCount*sizeof(GLint));
    
    real(program, uniformCount, uniformIndices, pname, params);
    
    free(params);
    free(uniformIndices);

glGetFramebufferAttachmentParameteriv:
    GLenum target = gl_param_GLenum(command, 0);
    GLenum attachment = gl_param_GLenum(command, 1);
    GLenum pname = gl_param_GLenum(command, 2);
    GLint params;
    real(target, attachment, pname, &params);

glGetRenderbufferParameteriv:
    GLenum target = gl_param_GLenum(command, 0);
    GLenum pname = gl_param_GLenum(command, 1);
    GLint params;
    real(target, pname, &params);

glFramebufferRenderbuffer:
    GLenum target = gl_param_GLenum(command, 0);
    GLenum attachment = gl_param_GLenum(command, 1);
    GLenum renderbuffertarget = gl_param_GLenum(command, 2);
    GLuint renderbuffer = gl_param_GLuint(command, 3);
    
    GLuint real_rb = trc_get_real_gl_renderbuffer(ctx->trace, renderbuffer);
    if (!real_rb && renderbuffer) ERROR("Invalid renderbuffer name.");
    //TODO: Reference counting
    
    real(target, attachment, renderbuffertarget, real_rb);
    
    GLint fb = get_bound_framebuffer(ctx, target);
    replay_add_fb_attachment_rb(ctx->trace, command, fb, attachment, renderbuffer);

glFramebufferTexture:
    GLenum target = gl_param_GLenum(command, 0);
    GLenum attachment = gl_param_GLenum(command, 1);
    GLuint texture = gl_param_GLuint(command, 2);
    GLint level = gl_param_GLint(command, 3);
    
    const trc_gl_texture_rev_t* texrev = trc_get_gl_texture(ctx->trace, texture);
    if (!texrev && texture) ERROR("Invalid texture name.");
    //TODO: Reference counting
    
    real(target, attachment, texrev->real, level);
        
    if (!texrev->created) ERROR("Although it has a valid name, the texture has not been created."
                                "Use glBindTexture or use glCreateTextures instead of glGenTextures.");
    GLint fb = get_bound_framebuffer(ctx, target);
    replay_add_fb_attachment(ctx->trace, command, fb, attachment, texture, texrev->type, level, 0);

glFramebufferTexture2D:
    GLenum target = gl_param_GLenum(command, 0);
    GLenum attachment = gl_param_GLenum(command, 1);
    GLenum textarget = gl_param_GLenum(command, 2);
    GLuint texture = gl_param_GLuint(command, 3);
    GLint level = gl_param_GLint(command, 4);
    
    const trc_gl_texture_rev_t* texrev = trc_get_gl_texture(ctx->trace, texture);
    if (!texrev && texture) ERROR("Invalid texture name.");
    //TODO: Reference counting
    
    real(target, attachment, textarget, texrev->real, level);
    
    if (!texrev->created) ERROR("Although it has a valid name, the texture has not been created."
                                "Use glBindTexture or use glCreateTextures instead of glGenTextures.");
    GLint fb = get_bound_framebuffer(ctx, target);
    replay_add_fb_attachment(ctx->trace, command, fb, attachment, texture, textarget, level, 0);

glRenderbufferStorage:
    GLenum target = gl_param_GLenum(command, 0);
    GLenum internalformat = gl_param_GLenum(command, 1);
    GLsizei width = gl_param_GLsizei(command, 2);
    GLsizei height = gl_param_GLsizei(command, 3);
    const trc_gl_context_rev_t* state = trc_get_gl_context(ctx->trace, 0);
    const trc_gl_renderbuffer_rev_t* rev = trc_get_gl_renderbuffer(ctx->trace, state->bound_renderbuffer);
    if (!rev) ERROR("Invalid renderbuffer name");
    real(target, internalformat, width, height);
    replay_update_renderbuffer(ctx, rev, state->bound_renderbuffer, width, height, internalformat, 1);

glRenderbufferStorageMultisample:
    GLenum target = gl_param_GLenum(command, 0);
    GLsizei samples = gl_param_GLsizei(command, 1);
    GLenum internalformat = gl_param_GLenum(command, 2);
    GLsizei width = gl_param_GLsizei(command, 3);
    GLsizei height = gl_param_GLsizei(command, 4);
    const trc_gl_context_rev_t* state = trc_get_gl_context(ctx->trace, 0);
    const trc_gl_renderbuffer_rev_t* rev = trc_get_gl_renderbuffer(ctx->trace, state->bound_renderbuffer);
    if (!rev) ERROR("Invalid renderbuffer name");
    real(target, samples, internalformat, width, height);
    replay_update_renderbuffer(ctx, rev, state->bound_renderbuffer, width, height, internalformat, samples);

glFenceSync:
    GLenum condition = gl_param_GLenum(command, 0);
    GLbitfield flags = gl_param_GLbitfield(command, 1);
    GLsync real_sync = real(condition, flags);
    
    uint64_t fake = *trc_get_ptr(&command->ret);
    
    trc_gl_sync_rev_t rev;
    rev.fake_context = trc_get_current_fake_gl_context(ctx->trace);
    rev.ref_count = 1;
    rev.real = (uint64_t)real_sync;
    rev.type = GL_SYNC_FENCE;
    rev.condition = condition;
    rev.flags = flags;
    trc_set_gl_sync(ctx->trace, fake, &rev);

glDeleteSync:
    uint64_t fake = gl_param_GLsync(command, 0);
    GLsync real_sync = (GLsync)trc_get_real_gl_sync(ctx->trace, fake);
    if (!real_sync && fake) ERROR("Invalid sync name.");
    
    real(real_sync);
    
    if (fake) trc_rel_gl_obj(ctx->trace, fake, TrcGLObj_Sync);

glWaitSync:
    uint64_t fake = gl_param_GLsync(command, 0);
    GLsync real_sync = (GLsync)trc_get_real_gl_sync(ctx->trace, fake);
    if (!real_sync) ERROR("Invalid sync name.");
    
    real(real_sync, gl_param_GLbitfield(command, 1), gl_param_GLuint64(command, 2));

glClientWaitSync:
    uint64_t fake = gl_param_GLsync(command, 0);
    GLsync real_sync = (GLsync)trc_get_real_gl_sync(ctx->trace, fake);
    if (!real_sync) ERROR("Invalid sync name.");
    
    real(real_sync, gl_param_GLbitfield(command, 1), gl_param_GLuint64(command, 2));

glGenQueries:
    GLsizei n = gl_param_GLsizei(command, 0);
    GLuint queries[n];
    const uint64_t* fake = trc_get_uint(trc_get_arg(command, 1));
    
    real(n, queries);
    
    for (size_t i = 0; i < n; ++i) {
        trc_gl_query_rev_t rev;
        rev.fake_context = trc_get_current_fake_gl_context(ctx->trace);
        rev.ref_count = 1;
        rev.real = queries[i];
        rev.type = 0;
        rev.result = 0;
        trc_set_gl_query(ctx->trace, fake[i], &rev);
    }

glDeleteQueries:
    GLsizei n = gl_param_GLsizei(command, 0);
    GLuint queries[n];
    const uint64_t* fake = trc_get_uint(trc_get_arg(command, 1));
    
    for (size_t i = 0; i < n; ++i)
        if (!(queries[i] = trc_get_real_gl_query(ctx->trace, fake[i])))
            trc_add_error(command, "Invalid query name.");
        else trc_rel_gl_obj(ctx->trace, fake[i], TrcGLObj_Query);
    
    real(n, queries);

glBeginQuery:
    GLuint id = gl_param_GLuint(command, 1);
    GLuint real_id = trc_get_real_gl_query(ctx->trace, id);
    if (!real_id) ERROR("Invalid query name.");
    GLenum target = gl_param_GLenum(command, 0);
    real(target, real_id);
    
    trc_gl_query_rev_t query = *trc_get_gl_query(ctx->trace, id);
    query.type = target;
    trc_set_gl_query(ctx->trace, id, &query);
    
    trc_gl_context_rev_t state = *trc_get_gl_context(ctx->trace, 0);
    *get_query_binding_pointer(&state, target) = id;
    trc_set_gl_context(ctx->trace, 0, &state);
    //TODO: Reference counting

glEndQuery:
    GLenum target = gl_param_GLenum(command, 0);
    real(target);
    
    trc_gl_context_rev_t state = *trc_get_gl_context(ctx->trace, 0);
    GLuint id = *get_query_binding_pointer(&state, target);
    GLuint real_id = trc_get_real_gl_query(ctx->trace, id);
    *get_query_binding_pointer(&state, target) = 0;
    trc_set_gl_context(ctx->trace, 0, &state);
    //TODO: This clears any errors
    if (F(glGetError)() == GL_NO_ERROR) update_query(ctx, command, target, id, real_id);
    //TODO: Reference counting

glQueryCounter:
    GLuint id = gl_param_GLuint(command, 0);
    GLuint real_id = trc_get_real_gl_query(ctx->trace, id);
    if (!real_id) ERROR("Invalid query name.");
    GLenum target = gl_param_GLenum(command, 1);
    real(real_id, target);
    //TODO: This clears any errors
    if (F(glGetError)() == GL_NO_ERROR) update_query(ctx, command, target, id, real_id);

glDrawBuffers:
    GLsizei n = gl_param_GLsizei(command, 0);
    
    GLuint* bufs = malloc(sizeof(GLuint)*n);
    for (GLsizei i = 0; i < n; i++) bufs[i] = trc_get_uint(trc_get_arg(command, 1))[i];
    
    real(n, bufs);
    free(bufs);

glClearBufferiv:
    GLenum buffer = gl_param_GLenum(command, 0);
    GLint drawbuffer = gl_param_GLint(command, 1);
    size_t count = buffer == GL_COLOR ? 4 : 1;
    GLint value[count];
    for (size_t i = 0; i < count; i++) value[i] = trc_get_int(trc_get_arg(command, 2))[i];
    
    real(buffer, drawbuffer, value);
    
    //TODO
    //update_drawbuffer(ctx, command, buffer, drawbuffer);

glClearBufferuiv:
    GLenum buffer = gl_param_GLenum(command, 0);
    GLint drawbuffer = gl_param_GLint(command, 1);
    size_t count = buffer == GL_COLOR ? 4 : 1;
    GLuint value[count];
    for (size_t i = 0; i < count; i++) value[i] = trc_get_uint(trc_get_arg(command, 2))[i];
    
    real(buffer, drawbuffer, value);
    
    //TODO
    //update_drawbuffer(ctx, command, buffer, drawbuffer);

glClearBufferfv:
    GLenum buffer = gl_param_GLenum(command, 0);
    GLint drawbuffer = gl_param_GLint(command, 1);
    size_t count = buffer == GL_COLOR ? 4 : 1;
    GLfloat value[count];
    for (size_t i = 0; i < count; i++)
        value[i] = trc_get_double(trc_get_arg(command, 2))[i];
    
    real(buffer, drawbuffer, value);
    
    //TODO
    //update_drawbuffer(ctx, command, buffer, drawbuffer);

glClearBufferfi:
    GLenum buffer = gl_param_GLenum(command, 0);
    GLint drawbuffer = gl_param_GLint(command, 1);
    GLfloat depth = gl_param_GLfloat(command, 2);
    GLint stencil = gl_param_GLint(command, 3);
    real(buffer, drawbuffer, depth, stencil);
    
    //TODO
    //update_drawbuffer(ctx, command, GL_DEPTH, 0);
    //update_drawbuffer(ctx, command, GL_STENCIL, 0);

glBindFragDataLocation:
    GLuint fake = gl_param_GLuint(command, 0);
    GLuint real_program = trc_get_real_gl_program(ctx->trace, fake);
    if (!real_program) ERROR("Invalid program name.");
    real(real_program, gl_param_GLuint(command, 1), gl_param_string(command, 2));

glBindFragDataLocationIndexed:
    GLuint fake = gl_param_GLuint(command, 0);
    GLuint real_program = trc_get_real_gl_program(ctx->trace, fake);
    if (!real_program) ERROR("Invalid program name.");
    real(real_program, gl_param_GLuint(command, 1), gl_param_GLuint(command, 2), gl_param_string(command, 3));
