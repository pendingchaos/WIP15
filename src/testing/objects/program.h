static testing_property_t* program_properties = NULL;

#if REPLAY
static double get_double_prop_program_uniforms(uint64_t index, const void* rev_) {
    uint64_t loc = index >> 32;
    uint64_t uniform_index = index & 0xffff;
    
    const trc_gl_program_rev_t* rev = rev_;
    size_t uniform_count = rev->uniforms->size / sizeof(trc_gl_uniform_t);
    trc_gl_uniform_t* uniforms = trc_map_data(rev->uniforms, TRC_MAP_READ);
    trc_gl_uniform_t uniform;
    for (size_t i = 0; i < uniform_count; i++) {
        if ((int)uniforms[i].dtype.base<=8 && uniforms[i].fake_loc==loc) {
            uniform = uniforms[i];
            goto success;
        }
    }
    trc_unmap_data(rev->uniforms);
    return NAN;
    success:
    trc_unmap_data(rev->uniforms);
    
    uint8_t* data = trc_map_data(rev->uniform_data, TRC_MAP_READ);
    data += uniform.data_offset;
    
    double val = NAN;
    switch (uniform.dtype.base) {
    case TrcUniformBaseType_Float: val = ((float*)data)[uniform_index]; break;
    case TrcUniformBaseType_Double: val = ((double*)data)[uniform_index]; break;
    case TrcUniformBaseType_Uint:
    case TrcUniformBaseType_Sampler:
    case TrcUniformBaseType_Image: val = ((uint32_t*)data)[uniform_index]; break;
    case TrcUniformBaseType_Int: val = ((int32_t*)data)[uniform_index]; break;
    case TrcUniformBaseType_Uint64: val = ((uint64_t*)data)[uniform_index]; break;
    case TrcUniformBaseType_Int64: val = ((int64_t*)data)[uniform_index]; break;
    case TrcUniformBaseType_Bool: val = ((bool*)data)[uniform_index] ? 1 : 0; break;
    default: break;
    }
    
    trc_unmap_data(rev->uniform_data);
    
    return val;
}
#endif

static double get_double_prop_program_uniforms_gl(uint64_t index, void* ctx, const void* rev_, GLuint64 real) {
    uint64_t loc = index >> 32;
    uint64_t uniform_index = index & 0xffff;
    
    #if REPLAY
    const trc_gl_program_rev_t* rev = rev_;
    size_t uniform_count = rev->uniforms->size / sizeof(trc_gl_uniform_t);
    trc_gl_uniform_t* uniforms = trc_map_data(rev->uniforms, TRC_MAP_READ);
    int64_t real_loc = -1;
    for (size_t i = 0; i < uniform_count; i++) {
        if ((int)uniforms[i].dtype.base<=8 && uniforms[i].fake_loc==loc) {
            real_loc = uniforms[i].real_loc;
            break;
        }
    }
    trc_unmap_data(rev->uniforms);
    if (real_loc < 0) return NAN;
    #else
    GLuint real_loc = loc;
    #endif
    
    GLint major;
    F(glGetIntegerv)(GL_MAJOR_VERSION, &major);
    if (major >= 4) {
        GLdouble data[16] = {NAN, NAN, NAN, NAN, NAN, NAN, NAN, NAN, NAN, NAN, NAN, NAN, NAN, NAN, NAN, NAN};
        F(glGetUniformdv)(real, real_loc, data);
        return data[uniform_index];
    } else {
        GLfloat data[16] = {NAN, NAN, NAN, NAN, NAN, NAN, NAN, NAN, NAN, NAN, NAN, NAN, NAN, NAN, NAN, NAN};
        F(glGetUniformfv)(real, real_loc, data);
        return data[uniform_index];
    }
}

REGISTER_PROPERTY_DOUBLE(program, uniforms, SWITCH_REPLAY(get_double_prop_program_uniforms, NULL), get_double_prop_program_uniforms_gl)
