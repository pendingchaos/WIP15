static testing_property_t* sampler_properties = NULL;

static int64_t get_int_prop_sampler_gl(uint64_t index, void* ctx, GLuint real, GLenum param) {
    GLint res[4];
    F(glGetSamplerParameteriv)(real, param, res);
    return res[index];
}

static double get_double_prop_sampler_gl(uint64_t index, void* ctx, GLuint real, GLenum param) {
    GLfloat res[4];
    F(glGetSamplerParameterfv)(real, param, res);
    return res[index];
}

PROPERTY_INT(sampler, 330, mag_filter, GL_TEXTURE_MAG_FILTER,
    rev->params.mag_filter)
PROPERTY_INT(sampler, 330, min_filter, GL_TEXTURE_MIN_FILTER,
    rev->params.min_filter)
PROPERTY_INT(sampler, 330, min_lod, GL_TEXTURE_MIN_LOD,
    rev->params.min_lod)
PROPERTY_INT(sampler, 330, max_lod, GL_TEXTURE_MAX_LOD,
    rev->params.max_lod)
PROPERTY_INT(sampler, 330, wrap_s, GL_TEXTURE_WRAP_S,
    rev->params.wrap_s)
PROPERTY_INT(sampler, 330, wrap_t, GL_TEXTURE_WRAP_T,
    rev->params.wrap_t)
PROPERTY_INT(sampler, 330, wrap_r, GL_TEXTURE_WRAP_R,
    rev->params.wrap_r)
PROPERTY_DOUBLE(sampler, 330, border_color, GL_TEXTURE_BORDER_COLOR,
    rev->params.border_color[index])
PROPERTY_INT(sampler, 330, compare_mode, GL_TEXTURE_COMPARE_MODE,
    rev->params.compare_mode)
PROPERTY_INT(sampler, 330, compare_func, GL_TEXTURE_COMPARE_FUNC,
    rev->params.compare_func)
PROPERTY_DOUBLE(sampler, 330, lod_bias, GL_TEXTURE_LOD_BIAS,
    rev->params.lod_bias)
PROPERTY_DOUBLE(sampler, 460, max_anisotropy, GL_TEXTURE_MAX_ANISOTROPY,
    rev->params.max_anisotropy)
