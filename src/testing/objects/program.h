static testing_property_t* program_properties = NULL;

#if REPLAY
static double get_double_prop_program_uniforms(uint64_t index, const void* rev_) {
    uint64_t loc = index >> 32;
    uint64_t uniform_index = index & 0xffff;
    
    const trc_gl_program_rev_t* rev = rev_;
    size_t uniform_count = rev->uniforms->size / sizeof(trc_gl_program_uniform_t);
    trc_gl_program_uniform_t* uniforms = trc_map_data(rev->uniforms, TRC_MAP_READ);
    double val = NAN;
    //TODO: Handle arrays
    for (size_t i = 0; i < uniform_count; i++) {
        trc_gl_program_uniform_t* u = &uniforms[i];
        if (u->fake != loc) continue;
        if (u->value == NULL) return NAN;
        switch (u->dtype) {
        case 0: val = ((double*)trc_map_data(u->value, TRC_MAP_READ))[uniform_index]; break;
        case 1: val = ((uint64_t*)trc_map_data(u->value, TRC_MAP_READ))[uniform_index]; break;
        case 2: val = ((int64_t*)trc_map_data(u->value, TRC_MAP_READ))[uniform_index]; break;
        default: assert(false);
        }
        trc_unmap_data(u->value);
        break;
    }
    trc_unmap_data(rev->uniforms);
    return val;
}
#endif

static double get_double_prop_program_uniforms_gl(uint64_t index, void* ctx, const void* rev_, GLuint64 real) {
    uint64_t loc = index >> 32;
    uint64_t uniform_index = index & 0xffff;
    
    #if REPLAY
    const trc_gl_program_rev_t* rev = rev_;
    size_t uniform_count = rev->uniforms->size / sizeof(trc_gl_program_uniform_t);
    trc_gl_program_uniform_t* uniforms = trc_map_data(rev->uniforms, TRC_MAP_READ);
    int64_t real_loc = -1;
    //TODO: Handle arrays
    for (size_t i = 0; i < uniform_count; i++) {
        if (uniforms[i].fake != loc) continue;
        real_loc = uniforms[i].real;
        break;
    }
    trc_unmap_data(rev->uniforms);
    if (real_loc < 0) return NAN;
    #else
    GLuint real_loc = loc;
    #endif
    
    GLint major;
    F(glGetIntegerv)(GL_MAJOR_VERSION, &major);
    if (major >= 4) {
        GLdouble data[4] = {NAN, NAN, NAN, NAN};
        F(glGetUniformdv)(real, real_loc, data);
        return data[uniform_index];
    } else {
        GLfloat data[4] = {NAN, NAN, NAN, NAN};
        F(glGetUniformfv)(real, real_loc, data);
        return data[uniform_index];
    }
}

REGISTER_PROPERTY_DOUBLE(program, uniforms, SWITCH_REPLAY(get_double_prop_program_uniforms, NULL), get_double_prop_program_uniforms_gl)
