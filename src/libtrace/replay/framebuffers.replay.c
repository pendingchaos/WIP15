#include "libtrace/replay/utils.h"

static void gen_framebuffers(size_t count, const GLuint* real, const GLuint* fake, bool create) {
    trc_gl_framebuffer_rev_t rev;
    rev.has_object = false;
    trc_data_t* empty_data = trc_create_data(ctx->trace, 0, NULL, 0);
    rev.attachments = empty_data;
    rev.draw_buffers = empty_data;
    for (size_t i = 0; i < count; ++i) {
        rev.real = real[i];
        trc_create_named_obj(ctx->priv_ns, TrcFramebuffer, fake[i], &rev);
    }
}

static void gen_renderbuffers(size_t count, const GLuint* real, const GLuint* fake, bool create) {
    trc_gl_renderbuffer_rev_t rev;
    rev.has_object = create;
    rev.has_storage = false;
    for (size_t i = 0; i < count; ++i) {
        rev.real = real[i];
        trc_create_named_obj(ctx->ns, TrcRenderbuffer, fake[i], &rev);
    }
}

static bool append_fb_attachment(bool dsa, trc_obj_t* fb, const trc_gl_framebuffer_attachment_t* attach) {
    const trc_gl_framebuffer_rev_t* rev = trc_obj_get_rev(fb, -1);
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
    trc_unmap_data(attachs);
    
    trc_gl_framebuffer_rev_t newrev = *rev;
    if (!replaced) newattachs[attach_count++] = *attach;
    
    size_t size = attach_count * sizeof(trc_gl_framebuffer_attachment_t);
    newrev.attachments = trc_create_data_no_copy(ctx->trace, size, newattachs, 0);
    
    set_framebuffer(&newrev);
    
    return true;
}

static bool add_fb_attachment(trc_obj_t* fb, uint attachment, bool dsa, uint fake_tex,
                              const trc_gl_texture_rev_t* tex, uint target, uint level, uint layer) {
    bool cubemap = target>=GL_TEXTURE_CUBE_MAP_POSITIVE_X && target<=GL_TEXTURE_CUBE_MAP_NEGATIVE_Z;
    
    if (!tex && fake_tex) ERROR2(false, "Invalid texture name");
    if (fake_tex && !tex->has_object) ERROR2(false, "Texture name has no object");
    if (tex && (cubemap?GL_TEXTURE_CUBE_MAP:target) != tex->type)
        ERROR2(false, "Incompatible target for texture\n");
    
    if (tex) {
        size_t image_count = tex->images->size / sizeof(trc_gl_texture_image_t);
        trc_gl_texture_image_t* images = trc_map_data(tex->images, TRC_MAP_READ);
        bool found = false;
        for (size_t i = 0; i < image_count; i++) if (images[i].level == level) found = true;;
        trc_unmap_data(images);
        if (!found) ERROR2(false, "No such level for texture");
    }
    
    trc_gl_framebuffer_attachment_t attach;
    memset(&attach, 0, sizeof(attach));
    attach.has_renderbuffer = false;
    attach.attachment = attachment;
    trc_set_obj_ref(&attach.renderbuffer, NULL);
    trc_set_obj_ref(&attach.texture, tex->head.obj);
    attach.level = level;
    attach.layer = layer;
    attach.face = 0;
    if (cubemap) {
        attach.face = target - GL_TEXTURE_CUBE_MAP_POSITIVE_X;
    } else if (target==GL_TEXTURE_CUBE_MAP_ARRAY || target==GL_TEXTURE_CUBE_MAP) {
        attach.face = layer % 6;
        attach.layer /= 6;
    }
    return append_fb_attachment(dsa, fb, &attach);
}

static bool add_fb_attachment_rb(bool dsa, trc_obj_t* fb, uint attachment, uint fake_rb,
                                 const trc_gl_renderbuffer_rev_t* rb) {
    if (!rb && fake_rb) ERROR2(false, "Invalid renderbuffer name");
    if (fake_rb && !rb->has_object) ERROR2(false, "Renderbuffer name has no object");
    trc_gl_framebuffer_attachment_t attach;
    memset(&attach, 0, sizeof(attach));
    attach.has_renderbuffer = true;
    attach.attachment = attachment;
    trc_set_obj_ref(&attach.texture, NULL);
    trc_set_obj_ref(&attach.renderbuffer, rb->head.obj);
    return append_fb_attachment(dsa, fb, &attach);
}

static void update_renderbuffer(const trc_gl_renderbuffer_rev_t* rev, uint width,
                                uint height, uint internal_format, uint samples) {
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
    
    set_renderbuffer(&newrev);
}

static bool renderbuffer_storage(const trc_gl_renderbuffer_rev_t* rb, bool dsa, GLenum internal_format,
                                 GLsizei width, GLsizei height, GLsizei samples) {
    if (!rb) ERROR2(false, dsa?"Invalid renderbuffer name":"No renderbuffer bound");
    if (!rb->has_object) ERROR2(false, "Renderbuffer name has no obejct");
    int maxsize = gls_get_state_int(GL_MAX_RENDERBUFFER_SIZE, 0);
    if (width<0 || height<0 || width>maxsize || height>maxsize)
        ERROR2(false, "Invalid dimensions");
    //TODO: test if samples if valid
    //TODO: handle when internal_format is not renderable
    return true;
}

trc_obj_t* get_bound_framebuffer(GLenum target) {
    const trc_gl_context_rev_t* state = trc_get_context(ctx->trace);
    switch (target) {
    case GL_DRAW_FRAMEBUFFER: 
        return state->draw_framebuffer.obj;
    case GL_READ_FRAMEBUFFER: 
        return state->read_framebuffer.obj;
    case GL_FRAMEBUFFER: 
        return state->draw_framebuffer.obj;
    }
    return NULL;
}

glGenFramebuffers: //GLsizei p_n, GLuint* p_framebuffers
    if (p_n < 0) ERROR("Invalid framebuffer name count");
    GLuint* fbs = replay_alloc(p_n*sizeof(GLuint));
    real(p_n, fbs);
    gen_framebuffers(p_n, fbs, p_framebuffers, false);

glCreateFramebuffers: //GLsizei p_n, GLuint* p_framebuffers
    if (p_n < 0) ERROR("Invalid framebuffer name count");
    GLuint* fbs = replay_alloc(p_n*sizeof(GLuint));
    real(p_n, fbs);
    gen_framebuffers(p_n, fbs, p_framebuffers, true);

glDeleteFramebuffers: //GLsizei p_n, const GLuint* p_framebuffers
    GLuint* fbs = replay_alloc(p_n*sizeof(GLuint));
    for (size_t i = 0; i < p_n; ++i) {
        trc_obj_t* fb = trc_lookup_name(ctx->priv_ns, TrcFramebuffer, p_framebuffers[i], -1);
        if (fb && fb==gls_get_read_framebuffer())
            gls_set_read_framebuffer(NULL);
        if (fb && fb==gls_get_draw_framebuffer())
            gls_set_draw_framebuffer(NULL);
        if (!(fbs[i] = get_real_framebuffer(p_framebuffers[i])) && p_framebuffers[i]) {
            trc_add_warning(cmd, "Invalid framebuffer name");
        } else {
            delete_obj(p_framebuffers[i], TrcFramebuffer);
        }
    }
    real(p_n, fbs);

glBindFramebuffer: //GLenum p_target, GLuint p_framebuffer
    const trc_gl_framebuffer_rev_t* rev = get_framebuffer(p_framebuffer);
    if (!rev && p_framebuffer) ERROR("Invalid framebuffer name");
    real(p_target, p_framebuffer?rev->real:0);
    
    if (rev && !rev->has_object) {
        trc_gl_framebuffer_rev_t newrev = *rev;
        newrev.has_object = true;
        set_framebuffer(&newrev);
    }
    
    bool read = true;
    bool draw = true;
    switch (p_target) {
    case GL_READ_FRAMEBUFFER: draw = false; break;
    case GL_DRAW_FRAMEBUFFER: read = false; break;
    }
    
    if (read) gls_set_read_framebuffer(rev?rev->head.obj:NULL);
    if (draw) gls_set_draw_framebuffer(rev?rev->head.obj:NULL);

glGenRenderbuffers: //GLsizei p_n, GLuint* p_renderbuffers
    if (p_n < 0) ERROR("Invalid renderbuffer name count");
    GLuint* rbs = replay_alloc(p_n*sizeof(GLuint));
    real(p_n, rbs);
    gen_renderbuffers(p_n, rbs, p_renderbuffers, false);

glCreateRenderbuffers: //GLsizei p_n, GLuint* p_renderbuffers
    if (p_n < 0) ERROR("Invalid renderbuffer name count");
    GLuint* rbs = replay_alloc(p_n*sizeof(GLuint));
    real(p_n, rbs);
    gen_renderbuffers(p_n, rbs, p_renderbuffers, true);

glDeleteRenderbuffers: //GLsizei p_n, const GLuint* p_renderbuffers
    GLuint* rbs = replay_alloc(p_n*sizeof(GLuint));
    for (size_t i = 0; i < p_n; ++i) {
        trc_obj_t* rb = trc_lookup_name(ctx->ns, TrcRenderbuffer, p_renderbuffers[i], -1);
        if (p_renderbuffers[i] && rb==gls_get_bound_renderbuffer())
            gls_set_bound_renderbuffer(NULL);
        //TODO: Detach from bound framebuffers
        //TODO: What to do with renderbuffers attached to non-bound framebuffers?
        if (!(rbs[i] = get_real_renderbuffer(p_renderbuffers[i])) && p_renderbuffers[i])
            trc_add_warning(cmd, "Invalid renderbuffer name");
        else delete_obj(p_renderbuffers[i], TrcRenderbuffer);
    }
    
    real(p_n, rbs);

glBindRenderbuffer: //GLenum p_target, GLuint p_renderbuffer
    const trc_gl_renderbuffer_rev_t* rev = get_renderbuffer(p_renderbuffer);
    if (!rev && p_renderbuffer) ERROR("Invalid renderbuffer name");
    real(p_target, p_renderbuffer?rev->real:0);
    if (rev && !rev->has_object) {
        trc_gl_renderbuffer_rev_t newrev = *rev;
        newrev.has_object = true;
        set_renderbuffer(&newrev);
    }
    gls_set_bound_renderbuffer(rev?rev->head.obj:NULL);

glGetFramebufferAttachmentParameteriv: //GLenum p_target, GLenum p_attachment, GLenum p_pname, GLint* p_params
    GLint params;
    real(p_target, p_attachment, p_pname, &params);

glGetRenderbufferParameteriv: //GLenum p_target, GLenum p_pname, GLint* p_params
    const trc_gl_renderbuffer_rev_t* rev = trc_obj_get_rev(gls_get_bound_renderbuffer(), -1);
    if (!rev) ERROR("No renderbuffer bound");
    if (!rev->has_object) ERROR("Renderbuffer name has no object");

glGetNamedRenderbufferParameteriv: //GLuint p_renderbuffer, GLenum p_pname, GLint* p_params
    if (!p_renderbuffer_rev) ERROR("Invalid renderbuffer name");
    if (!p_renderbuffer_rev->has_object) ERROR("Renderbuffer name has no object");

glFramebufferRenderbuffer: //GLenum p_target, GLenum p_attachment, GLenum p_renderbuffertarget, GLuint p_renderbuffer
    trc_obj_t* fb = get_bound_framebuffer(p_target);
    if (add_fb_attachment_rb(false, fb, p_attachment, p_renderbuffer, p_renderbuffer_rev))
        real(p_target, p_attachment, p_renderbuffertarget, p_renderbuffer?p_renderbuffer_rev->real:0);

glNamedFramebufferRenderbuffer: //GLuint p_framebuffer, GLenum p_attachment, GLenum p_renderbuffertarget, GLuint p_renderbuffer
    trc_obj_t* fb = trc_lookup_name(ctx->priv_ns, TrcFramebuffer, p_framebuffer, -1);
    if (add_fb_attachment_rb(true, fb, p_attachment, p_renderbuffer, p_renderbuffer_rev))
        real(p_framebuffer_rev->real, p_attachment, p_renderbuffertarget, p_renderbuffer?p_renderbuffer_rev->real:0);

glFramebufferTexture: //GLenum p_target, GLenum p_attachment, GLuint p_texture, GLint p_level
    trc_obj_t* fb = get_bound_framebuffer(p_target);
    if (add_fb_attachment(fb, p_attachment, false, p_texture, p_texture_rev, p_texture_rev?p_texture_rev->type:0, p_level, 0))
        real(p_target, p_attachment, p_texture?p_texture_rev->real:0, p_level);

glNamedFramebufferTexture: //GLuint p_framebuffer, GLenum p_attachment, GLuint p_texture, GLint p_level
    trc_obj_t* fb = trc_lookup_name(ctx->priv_ns, TrcFramebuffer, p_framebuffer, -1);
    if (add_fb_attachment(fb, p_attachment, true, p_texture, p_texture_rev, p_texture_rev?p_texture_rev->type:0, p_level, 0))
        real(p_framebuffer_rev->real, p_attachment, p_texture?p_texture_rev->real:0, p_level);

glFramebufferTextureLayer: //GLenum p_target, GLenum p_attachment GLuint p_texture, GLint p_level, GLint p_layer
    trc_obj_t* fb = get_bound_framebuffer(p_target);
    if (add_fb_attachment(fb, p_attachment, false, p_texture, p_texture_rev, p_texture_rev?p_texture_rev->type:0, p_level, p_layer))
        real(p_target, p_attachment, p_texture?p_texture_rev->real:0, p_level, p_layer);

glNamedFramebufferTextureLayer: //GLuint p_framebuffer, GLenum p_attachment GLuint p_texture, GLint p_level, GLint p_layer
    trc_obj_t* fb = trc_lookup_name(ctx->priv_ns, TrcFramebuffer, p_framebuffer, -1);
    if (add_fb_attachment(fb, p_attachment, true, p_texture, p_texture_rev, p_texture_rev?p_texture_rev->type:0, p_level, p_layer))
        real(p_framebuffer_rev->real, p_attachment, p_texture?p_texture_rev->real:0, p_level, p_layer);

glFramebufferTexture1D: //GLenum p_target, GLenum p_attachment, GLenum p_textarget, GLuint p_texture, GLint p_level
    trc_obj_t* fb = get_bound_framebuffer(p_target);
    if (add_fb_attachment(fb, p_attachment, false, p_texture, p_texture_rev, p_textarget, p_level, 0))
        real(p_target, p_attachment, p_textarget, p_texture?p_texture_rev->real:0, p_level);

glFramebufferTexture2D: //GLenum p_target, GLenum p_attachment, GLenum p_textarget, GLuint p_texture, GLint p_level
    trc_obj_t* fb = get_bound_framebuffer(p_target);
    if (add_fb_attachment(fb, p_attachment, false, p_texture, p_texture_rev, p_textarget, p_level, 0))
        real(p_target, p_attachment, p_textarget, p_texture?p_texture_rev->real:0, p_level);

glFramebufferTexture3D: //GLenum p_target, GLenum p_attachment, GLenum p_textarget, GLuint p_texture, GLint p_level, GLint p_zoffset
    trc_obj_t* fb = get_bound_framebuffer(p_target);
    if (add_fb_attachment(fb, p_attachment, false, p_texture, p_texture_rev, p_textarget, p_level, p_zoffset))
        real(p_target, p_attachment, p_textarget, p_texture?p_texture_rev->real:0, p_level, p_zoffset);

glRenderbufferStorage: //GLenum p_target, GLenum p_internalformat, GLsizei p_width, GLsizei p_height
    trc_obj_t* rb = gls_get_bound_renderbuffer();
    const trc_gl_renderbuffer_rev_t* rb_rev = trc_obj_get_rev(rb, -1);
    if (renderbuffer_storage(rb_rev, false, p_internalformat, p_width, p_height, 1)) {
        real(p_target, p_internalformat, p_width, p_height);
        update_renderbuffer(rb_rev, p_width, p_height, p_internalformat, 1);
    }

glNamedRenderbufferStorage: //GLuint p_renderbuffer, GLenum p_internalformat, GLsizei p_width, GLsizei p_height
    if (renderbuffer_storage(p_renderbuffer_rev, false, p_internalformat, p_width, p_height, 1)) {
        real(p_renderbuffer_rev->real, p_internalformat, p_width, p_height);
        update_renderbuffer(p_renderbuffer_rev, p_width, p_height, p_internalformat, 1);
    }

glRenderbufferStorageMultisample: //GLenum p_target, GLsizei p_samples, GLenum p_internalformat, GLsizei p_width, GLsizei p_height
    trc_obj_t* rb = gls_get_bound_renderbuffer();
    const trc_gl_renderbuffer_rev_t* rb_rev = trc_obj_get_rev(rb, -1);
    if (renderbuffer_storage(rb_rev, false, p_internalformat, p_width, p_height, p_samples)) {
        real(p_target, p_samples, p_internalformat, p_width, p_height);
        update_renderbuffer(rb_rev, p_width, p_height, p_internalformat, p_samples);
    }

glNamedRenderbufferStorageMultisample: //GLuint p_renderbuffer, GLsizei p_samples, GLenum p_internalformat, GLsizei p_width, GLsizei p_height
    if (renderbuffer_storage(p_renderbuffer_rev, false, p_internalformat, p_width, p_height, p_samples)) {
        real(p_renderbuffer, p_samples, p_internalformat, p_width, p_height);
        update_renderbuffer(p_renderbuffer_rev, p_width, p_height, p_internalformat, p_samples);
    }

glGetFramebufferParameteriv: //GLenum p_target, GLenum p_pname, GLint* p_params
    trc_obj_t* fb = get_bound_framebuffer(p_target);
    if (!fb && not_one_of(p_pname, GL_DOUBLEBUFFER, GL_IMPLEMENTATION_COLOR_READ_FORMAT,
                          GL_IMPLEMENTATION_COLOR_READ_TYPE, GL_SAMPLES, GL_SAMPLE_BUFFERS, GL_STEREO, -1))
        ERROR("Invalid parameter name");

glGetNamedFramebufferParameteriv: //GLuint p_framebuffer, GLenum p_pname, GLint  * p_param
    if (!p_framebuffer && not_one_of(p_pname, GL_DOUBLEBUFFER, GL_IMPLEMENTATION_COLOR_READ_FORMAT,
                                     GL_IMPLEMENTATION_COLOR_READ_TYPE, GL_SAMPLES, GL_SAMPLE_BUFFERS, GL_STEREO, -1))
        ERROR("Invalid parameter name");
    if (!p_framebuffer_rev && p_framebuffer) ERROR("Invalid framebuffer name");
    if (p_framebuffer_rev && !p_framebuffer_rev->has_object)
        ERROR("Framebuffer name has no object");

glCheckFramebufferStatus: //GLenum p_target
    ;

glCheckNamedFramebufferStatus: //GLuint p_framebuffer
    if (!p_framebuffer_rev) ERROR2(, "Invalid framebuffer name");
    if (!p_framebuffer_rev->has_object) ERROR2(, "Framebuffer name has no object");

glIsFramebuffer: //GLuint p_framebuffer
    ;

glIsRenderbuffer: //GLuint p_renderbuffer
    ;
