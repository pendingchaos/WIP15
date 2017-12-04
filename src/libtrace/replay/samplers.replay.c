#include "libtrace/replay/textures.h"
#include "libtrace/replay/utils.h"

static void gen_samplers(size_t count, const GLuint* real, const GLuint* fake) {
    trc_gl_sampler_rev_t rev;
    rev.params.border_color[0] = 0;
    rev.params.border_color[1] = 0;
    rev.params.border_color[2] = 0;
    rev.params.border_color[3] = 0;
    rev.params.compare_func = GL_LEQUAL;
    rev.params.compare_mode = GL_NONE;
    rev.params.min_filter = GL_NEAREST_MIPMAP_LINEAR;
    rev.params.mag_filter = GL_LINEAR;
    rev.params.min_lod = -1000;
    rev.params.max_lod = 1000;
    rev.params.wrap_s = GL_REPEAT;
    rev.params.wrap_t = GL_REPEAT;
    rev.params.wrap_r = GL_REPEAT;
    rev.params.lod_bias = 0.0;
    rev.params.max_anisotropy = 1.0;
    for (size_t i = 0; i < count; ++i) {
        rev.real = real[i];
        trc_create_named_obj(ctx->ns, TrcSampler, fake[i], &rev);
    }
}

glGenSamplers: //GLsizei p_count, GLuint* p_samplers
    GLuint* samplers = replay_alloc(p_count*sizeof(GLuint));
    real(p_count, samplers);
    gen_samplers(p_count, samplers, p_samplers);

glCreateSamplers: //GLsizei p_n, GLuint* p_samplers
    GLuint* samplers = replay_alloc(p_n*sizeof(GLuint));
    real(p_n, samplers);
    gen_samplers(p_n, samplers, p_samplers);

glDeleteSamplers: //GLsizei p_count, const GLuint* p_samplers
    GLuint* samplers = replay_alloc(p_count*sizeof(GLuint));
    for (size_t i = 0; i < p_count; ++i) {
        if (!(samplers[i] = get_real_sampler(p_samplers[i])) && p_samplers[i]) {
            trc_add_warning(cmd, "Invalid sampler name");
        } else {
            trc_obj_t* obj = get_sampler(p_samplers[i])->head.obj;
            for (size_t i = 0; i < gls_get_bound_samplers_size(); i++) {
                if (gls_get_bound_samplers(i) == obj)
                    gls_set_bound_samplers(i, NULL);
            }
            
            delete_obj(p_samplers[i], TrcSampler);
        }
    }
    
    real(p_count, samplers);

glBindSampler: //GLuint p_unit, GLuint p_sampler
    const trc_gl_sampler_rev_t* rev = get_sampler(p_sampler);
    if (!rev && p_sampler) ERROR("Invalid sampler name");
    real(p_unit, p_sampler?rev->real:0);
    gls_set_bound_samplers(p_unit, rev?rev->head.obj:NULL);

glBindSamplers: //GLuint p_first, GLsizei p_count, const GLuint* p_samplers
    if (p_first+p_count>gls_get_state_int(GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS, 0) || p_count<0)
        ERROR("Invalid range");
    GLuint* real_samplers = replay_alloc(p_count*sizeof(GLuint));
    for (size_t i = 0; i < p_count; i++) {
        const trc_gl_sampler_rev_t* rev = get_sampler(p_samplers[i]);
        if (!rev) ERROR("Invalid sampler name at index %zu", i);
        real_samplers[i] = rev->real;
        gls_set_bound_samplers(p_first+i, rev?rev->head.obj:NULL);
    }
    real(p_first, p_count, real_samplers);

glSamplerParameterf: //GLuint p_sampler, GLenum p_pname, GLfloat p_param
    if (!p_sampler_rev) ERROR("Invalid sampler name");
    double double_param = p_param;
    trc_gl_sampler_rev_t newrev = *p_sampler_rev;
    if (!sample_param_double(&newrev.params, p_pname, 1, &double_param)) {
        real(p_sampler_rev->real, p_pname, p_param);
        set_sampler(&newrev);
    }

glSamplerParameteri: //GLuint p_sampler, GLenum p_pname, GLint p_param
    if (!p_sampler_rev) ERROR("Invalid sampler name");
    double double_param = p_param;
    trc_gl_sampler_rev_t newrev = *p_sampler_rev;
    if (!sample_param_double(&newrev.params, p_pname, 1, &double_param)) {
        real(p_sampler_rev->real, p_pname, p_param);
        set_sampler(&newrev);
    }

glSamplerParameterfv: //GLuint p_sampler, GLenum p_pname, const GLfloat* p_param
    if (!p_sampler_rev) ERROR("Invalid sampler name");
    trc_gl_sampler_rev_t newrev = *p_sampler_rev;
    if (!sample_param_double(&newrev.params, p_pname, arg_param->count, trc_get_double(arg_param))) {
        real(p_sampler_rev->real, p_pname, p_param);
        set_sampler(&newrev);
    }

glSamplerParameteriv: //GLuint p_sampler, GLenum p_pname, const GLint* p_param
    if (!p_sampler_rev) ERROR("Invalid sampler name");
    double* double_params = replay_alloc(arg_param->count*sizeof(double));
    if (p_pname == GL_TEXTURE_BORDER_COLOR)
        conv_from_signed_norm_array_i32(arg_param->count, double_params, p_param, 32);
    else
        for (size_t i = 0; i < arg_param->count; i++) double_params[i] = p_param[i];
    trc_gl_sampler_rev_t newrev = *p_sampler_rev;
    if (!sample_param_double(&newrev.params, p_pname, arg_param->count, double_params)) {
        real(p_sampler_rev->real, p_pname, p_param);
        set_sampler(&newrev);
    }

glSamplerParameterIiv: //GLuint p_sampler, GLenum p_pname, const GLint* p_param
    if (!p_sampler_rev) ERROR("Invalid sampler name");
    double* double_params = replay_alloc(arg_param->count*sizeof(double));
    for (size_t i = 0; i < arg_param->count; i++) double_params[i] = p_param[i];
    trc_gl_sampler_rev_t newrev = *p_sampler_rev;
    if (!sample_param_double(&newrev.params, p_pname, arg_param->count, double_params)) {
        real(p_sampler_rev->real, p_pname, p_param);
        set_sampler(&newrev);
    }

glSamplerParameterIuiv: //GLuint p_sampler, GLenum p_pname, const GLuint* p_param
    if (!p_sampler_rev) ERROR("Invalid sampler name");
    double* double_params = replay_alloc(arg_param->count*sizeof(double));
    for (size_t i = 0; i < arg_param->count; i++) double_params[i] = p_param[i];
    trc_gl_sampler_rev_t newrev = *p_sampler_rev;
    if (!sample_param_double(&newrev.params, p_pname, arg_param->count, double_params)) {
        real(p_sampler_rev->real, p_pname, p_param);
        set_sampler(&newrev);
    }

glGetSamplerParameterfv: //GLuint p_sampler, GLenum p_pname, GLfloat* p_params
    if (!p_sampler_rev) ERROR("Invalid sampler name");

glGetSamplerParameteriv: //GLuint p_sampler, GLenum p_pname, GLint* p_params
    if (!p_sampler_rev) ERROR("Invalid sampler name");

glGetSamplerParameterIiv: //GLuint p_sampler, GLenum p_pname, GLint* p_params
    if (!p_sampler_rev) ERROR("Invalid sampler name");

glGetSamplerParameterIuiv: //GLuint p_sampler, GLenum p_pname, GLuint* p_params
    if (!p_sampler_rev) ERROR("Invalid sampler name");

glIsSampler: //GLuint p_sampler
    ;
