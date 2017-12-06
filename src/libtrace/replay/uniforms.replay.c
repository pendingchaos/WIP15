#include "libtrace/replay/utils.h"

#define D(e, t) case e: {t tmp = val; memcpy(data, &tmp, sizeof(t)); return 1 + (t*)data;}
#define WUV(name, srct) static void* name(void* data, trc_gl_uniform_base_dtype_t dtype, srct val) {\
    switch (dtype) {\
    D(TrcUniformBaseType_Float, float)\
    D(TrcUniformBaseType_Double, double)\
    D(TrcUniformBaseType_Uint, uint32_t)\
    D(TrcUniformBaseType_Int, int32_t)\
    D(TrcUniformBaseType_Uint64, uint64_t)\
    D(TrcUniformBaseType_Int64, int64_t)\
    D(TrcUniformBaseType_Bool, bool)\
    D(TrcUniformBaseType_Sampler, int32_t)\
    D(TrcUniformBaseType_Image, int32_t)\
    default: return data;\
    }\
}
WUV(write_uniform_value_uint64, uint64_t)
WUV(write_uniform_value_int64, int64_t)
WUV(write_uniform_value_double, double)
#undef WUV
#undef D

static int uniform(bool dsa, bool array, uint dimx, uint dimy, GLenum type, uint* realprogram) {
    uint arg_pos = 0;
    const trc_gl_program_rev_t* rev;
    if (dsa) rev = get_program(trc_get_uint(&cmd->args[arg_pos++])[0]);
    else rev = trc_obj_get_rev(get_active_program(), -1);
    if (!rev) ERROR2(-1, dsa?"Invalid program":"No active program");
    if (realprogram) *realprogram = rev->real;
    
    int location = trc_get_int(&cmd->args[arg_pos++])[0];
    if (location == -1) return -1;
    
    trc_gl_uniform_t uniform;
    
    size_t uniform_count = rev->uniforms->size / sizeof(trc_gl_uniform_t);
    trc_gl_uniform_t* uniforms = trc_map_data(rev->uniforms, TRC_MAP_READ);
    uint uniform_index = 0;
    for (; uniform_index < uniform_count; uniform_index++) {
        if ((int)uniforms[uniform_index].dtype.base<=8 && uniforms[uniform_index].fake_loc == location) {
            uniform = uniforms[uniform_index];
            goto success1;
        }
    }
    trc_unmap_data(uniforms);
    ERROR2(-1, "Failed to find uniform based on location");
    success1: ;
    int real_location = uniform.real_loc;
    
    int count = array ? trc_get_int(&cmd->args[arg_pos++])[0] : 1;
    if (count < 0) {
        ERROR2(-1, "count is less than zero");
        trc_unmap_data(uniforms);
    }
    bool transpose = dimy==1 ? false : trc_get_bool(&cmd->args[arg_pos++])[0];
    
    switch (uniform.dtype.base) {
    case TrcUniformBaseType_Float:
    case TrcUniformBaseType_Double:
        if (type==GL_FLOAT || type==GL_DOUBLE) goto success2; else break;
    case TrcUniformBaseType_Uint:
    case TrcUniformBaseType_Uint64:
        if (type==GL_UNSIGNED_INT) goto success2; else break;
    case TrcUniformBaseType_Int:
    case TrcUniformBaseType_Int64:
    case TrcUniformBaseType_Sampler:
    case TrcUniformBaseType_Image:
        if (type==GL_INT) goto success2; else break;
    case TrcUniformBaseType_Bool: goto success2;
    default: break;
    }
    trc_unmap_data(uniforms);
    return -1;
    success2: ;
    
    bool is_array = false;
    if (uniform.parent != 0xffffffff)
        is_array = uniforms[uniform.parent].dtype.base == TrcUniformBaseType_Array;
    if (!is_array && count>1) {
        trc_unmap_data(uniforms);
        ERROR2(-1, "cound is greater than one but the uniform is not an array");
    }
    
    size_t array_size = 1;
    for (uint u = uniform_index; uniforms[u].next!=0xffffffff; u = uniforms[u].next) array_size++;
    if (count!=array_size || uniform.dtype.dim[0]!=dimx || uniform.dtype.dim[1]!=dimy) {
        trc_unmap_data(uniforms);
        return -1;
    }
    
    trc_gl_program_rev_t newrev = *rev;
    
    uint8_t* old_data = trc_map_data(rev->uniform_data, TRC_MAP_READ);
    newrev.uniform_data = trc_create_data(ctx->trace, rev->uniform_data->size, old_data, 0);
    trc_unmap_data(old_data);
    uint8_t* data_base = trc_map_data(newrev.uniform_data, TRC_MAP_MODIFY);
    uint8_t* data = data_base + uniform.data_offset;
    
    for (uint i = 0; i < count; i++) {
        for (uint x = 0; x < dimx; x++) {
            for (uint y = 0; y < dimy; y++) {
                if (array) {
                    uint si = transpose ? y*dimx+x : x*dimy+y;
                    si += dimx * dimy * i;
                    switch (type) {
                    case GL_FLOAT:
                    case GL_DOUBLE:
                        data = write_uniform_value_double(data, uniform.dtype.base, trc_get_double(&cmd->args[arg_pos])[si]);
                        break;
                    case GL_INT:
                        data = write_uniform_value_int64(data, uniform.dtype.base, trc_get_int(&cmd->args[arg_pos])[si]);
                        break;
                    case GL_UNSIGNED_INT:
                        data = write_uniform_value_uint64(data, uniform.dtype.base, trc_get_uint(&cmd->args[arg_pos])[si]);
                        break;
                    }
                } else {
                    switch (type) {
                    case GL_FLOAT:
                        data = write_uniform_value_double(data, uniform.dtype.base, trc_get_double(&cmd->args[arg_pos++])[0]);
                        break;
                    case GL_DOUBLE:
                        data = write_uniform_value_double(data, uniform.dtype.base, trc_get_double(&cmd->args[arg_pos++])[0]);
                        break;
                    case GL_INT:
                        data = write_uniform_value_int64(data, uniform.dtype.base, trc_get_int(&cmd->args[arg_pos++])[0]);
                        break;
                    case GL_UNSIGNED_INT:
                        data = write_uniform_value_uint64(data, uniform.dtype.base, trc_get_uint(&cmd->args[arg_pos++])[0]);
                        break;
                    }
                }
            }
        }
        if (i != count-1) uniform = uniforms[uniform.next];
    }
    
    trc_unmap_data(data_base);
    trc_unmap_data(uniforms);
    
    set_program(&newrev);
    
    return real_location;
}

static void validate_get_uniform() {
    //TODO: Don't use glGetProgramiv to get the link status
    GLuint fake = trc_get_uint(&cmd->args[0])[0];
    GLuint real_program = get_real_program(fake);
    if (!real_program) ERROR2(, "No such program.");
    GLint status;
    F(glGetProgramiv)(real_program, GL_LINK_STATUS, &status);
    if (!status) ERROR2(, "Program not successfully linked.");
}

glUniform1f: //GLint p_location, GLfloat p_v0
    GLint loc;
    if ((loc=uniform(false, false, 1, 1, GL_FLOAT, NULL))<0) RETURN;
    real(loc, p_v0);

glUniform2f: //GLint p_location, GLfloat p_v0, GLfloat p_v1
    GLint loc;
    if ((loc=uniform(false, false, 2, 1, GL_FLOAT, NULL))<0) RETURN;
    real(loc, p_v0, p_v1);

glUniform3f: //GLint p_location, GLfloat p_v0, GLfloat p_v1, GLfloat p_v2
    GLint loc;
    if ((loc=uniform(false, false, 3, 1, GL_FLOAT, NULL))<0) RETURN;
    real(loc, p_v0, p_v1, p_v2);

glUniform4f: //GLint p_location, GLfloat p_v0, GLfloat p_v1, GLfloat p_v2, GLfloat p_v3
    GLint loc;
    if ((loc=uniform(false, false, 4, 1, GL_FLOAT, NULL))<0) RETURN;
    real(loc, p_v0, p_v1, p_v2, p_v3);

glUniform1i: //GLint p_location, GLint p_v0
    GLint loc;
    if ((loc=uniform(false, false, 1, 1, GL_INT, NULL))<0) RETURN;
    real(loc, p_v0);

glUniform2i: //GLint p_location, GLint p_v0, GLint p_v1
    GLint loc;
    if ((loc=uniform(false, false, 2, 1, GL_INT, NULL))<0) RETURN;
    real(loc, p_v0, p_v1);

glUniform3i: //GLint p_location, GLint p_v0, GLint p_v1, GLint p_v2
    GLint loc;
    if ((loc=uniform(false, false, 3, 1, GL_INT, NULL))<0) RETURN;
    real(loc, p_v0, p_v1, p_v2);

glUniform4i: //GLint p_location, GLint p_v0, GLint p_v1, GLint p_v2, GLint p_v3
    GLint loc;
    if ((loc=uniform(false, false, 4, 1, GL_INT, NULL))<0) RETURN;
    real(loc, p_v0, p_v1, p_v2, p_v3);

glUniform1ui: //GLint p_location, GLuint p_v0
    GLint loc;
    if ((loc=uniform(false, false, 1, 1, GL_UNSIGNED_INT, NULL))<0) RETURN;
    real(loc, p_v0);

glUniform2ui: //GLint p_location, GLuint p_v0, GLuint p_v1
    GLint loc;
    if ((loc=uniform(false, false, 2, 1, GL_UNSIGNED_INT, NULL))<0) RETURN;
    real(loc, p_v0, p_v1);

glUniform3ui: //GLint p_location, GLuint p_v0, GLuint p_v1, GLuint p_v2
    GLint loc;
    if ((loc=uniform(false, false, 3, 1, GL_UNSIGNED_INT, NULL))<0) RETURN;
    real(loc, p_v0, p_v1, p_v2);

glUniform4ui: //GLint p_location, GLuint p_v0, GLuint p_v1, GLuint p_v2, GLuint p_v3
    GLint loc;
    if ((loc=uniform(false, false, 4, 1, GL_UNSIGNED_INT, NULL))<0) RETURN;
    real(loc, p_v0, p_v1, p_v2, p_v3);

glUniform1d: //GLint p_location, GLdouble p_x
    GLint loc;
    if ((loc=uniform(false, false, 1, 1, GL_DOUBLE, NULL))<0) RETURN;
    real(loc, p_x);

glUniform2d: //GLint p_location, GLdouble p_x, GLdouble p_y
    GLint loc;
    if ((loc=uniform(false, false, 2, 1, GL_DOUBLE, NULL))<0) RETURN;
    real(loc, p_x, p_y);

glUniform3d: //GLint p_location, GLdouble p_x, GLdouble p_y, GLdouble p_z
    GLint loc;
    if ((loc=uniform(false, false, 3, 1, GL_DOUBLE, NULL))<0) RETURN;
    real(loc, p_x, p_y, p_z);

glUniform4d: //GLint p_location, GLdouble p_x, GLdouble p_y, GLdouble p_z, GLdouble p_w
    GLint loc;
    if ((loc=uniform(false, false, 4, 1, GL_DOUBLE, NULL))<0) RETURN;
    real(loc, p_x, p_y, p_z, p_w);

glUniform1fv: //GLint p_location, GLsizei p_count, const GLfloat* p_value
    GLint loc;
    if ((loc=uniform(false, true, 1, 1, GL_FLOAT, NULL))<0) RETURN;
    real(loc, p_count, p_value);

glUniform2fv: //GLint p_location, GLsizei p_count, const GLfloat* p_value
    GLint loc;
    if ((loc=uniform(false, true, 2, 1, GL_FLOAT, NULL))<0) RETURN;
    real(loc, p_count, p_value);

glUniform3fv: //GLint p_location, GLsizei p_count, const GLfloat* p_value
    GLint loc;
    if ((loc=uniform(false, true, 3, 1, GL_FLOAT, NULL))<0) RETURN;
    real(loc, p_count, p_value);

glUniform4fv: //GLint p_location, GLsizei p_count, const GLfloat* p_value
    GLint loc;
    if ((loc=uniform(false, true, 4, 1, GL_FLOAT, NULL))<0) RETURN;
    real(loc, p_count, p_value);

glUniform1iv: //GLint p_location, GLsizei p_count, const GLint* p_value
    GLint loc;
    if ((loc=uniform(false, true, 1, 1, GL_INT, NULL))<0) RETURN;
    real(loc, p_count, p_value);

glUniform2iv: //GLint p_location, GLsizei p_count, const GLint* p_value
    GLint loc;
    if ((loc=uniform(false, true, 2, 1, GL_INT, NULL))<0) RETURN;
    real(loc, p_count, p_value);

glUniform3iv: //GLint p_location, GLsizei p_count, const GLint* p_value
    GLint loc;
    if ((loc=uniform(false, true, 3, 1, GL_INT, NULL))<0) RETURN;
    real(loc, p_count, p_value);

glUniform4iv: //GLint p_location, GLsizei p_count, const GLint* p_value
    GLint loc;
    if ((loc=uniform(false, true, 4, 1, GL_INT, NULL))<0) RETURN;
    real(loc, p_count, p_value);

glUniform1uiv: //GLint p_location, GLsizei p_count, const GLuint* p_value
    GLint loc;
    if ((loc=uniform(false, true, 1, 1, GL_UNSIGNED_INT, NULL))<0) RETURN;
    real(loc, p_count, p_value);

glUniform2uiv: //GLint p_location, GLsizei p_count, const GLuint* p_value
    GLint loc;
    if ((loc=uniform(false, true, 2, 1, GL_UNSIGNED_INT, NULL))<0) RETURN;
    real(loc, p_count, p_value);

glUniform3uiv: //GLint p_location, GLsizei p_count, const GLuint* p_value
    GLint loc;
    if ((loc=uniform(false, true, 3, 1, GL_UNSIGNED_INT, NULL))<0) RETURN;
    real(loc, p_count, p_value);

glUniform4uiv: //GLint p_location, GLsizei p_count, const GLuint* p_value
    GLint loc;
    if ((loc=uniform(false, true, 4, 1, GL_UNSIGNED_INT, NULL))<0) RETURN;
    real(loc, p_count, p_value);

glUniform1dv: //GLint p_location, GLsizei p_count, const GLdouble* p_value
    GLint loc;
    if ((loc=uniform(false, true, 1, 1, GL_DOUBLE, NULL))<0) RETURN;
    real(loc, p_count, p_value);

glUniform2dv: //GLint p_location, GLsizei p_count, const GLdouble* p_value
    GLint loc;
    if ((loc=uniform(false, true, 2, 1, GL_DOUBLE, NULL))<0) RETURN;
    real(loc, p_count, p_value);

glUniform3dv: //GLint p_location, GLsizei p_count, const GLdouble* p_value
    GLint loc;
    if ((loc=uniform(false, true, 3, 1, GL_UNSIGNED_INT, NULL))<0) RETURN;
    real(loc, p_count, p_value);

glUniform4dv: //GLint p_location, GLsizei p_count, const GLdouble* p_value
    GLint loc;
    if ((loc=uniform(false, true, 4, 1, GL_UNSIGNED_INT, NULL))<0) RETURN;
    real(loc, p_count, p_value);

glUniformMatrix2fv: //GLint p_location, GLsizei p_count, GLboolean p_transpose, const GLfloat* p_value
    GLfloat* values = replay_alloc(p_count*4*sizeof(GLfloat));
    GLint loc;
    if ((loc=uniform(false, true, 2, 2, GL_FLOAT, NULL))<0) RETURN;
    real(loc, p_count, p_transpose, p_value);

glUniformMatrix3fv: //GLint p_location, GLsizei p_count, GLboolean p_transpose, const GLfloat* p_value
    GLfloat* values = replay_alloc(p_count*9*sizeof(GLfloat));
    GLint loc;
    if ((loc=uniform(false, true, 3, 3, GL_FLOAT, NULL))<0) RETURN;
    real(loc, p_count, p_transpose, p_value);

glUniformMatrix4fv: //GLint p_location, GLsizei p_count, GLboolean p_transpose, const GLfloat* p_value
    GLint loc;
    if ((loc=uniform(false, true, 4, 4, GL_FLOAT, NULL))<0) RETURN;
    real(loc, p_count, p_transpose, p_value);

glUniformMatrix2x3fv: //GLint p_location, GLsizei p_count, GLboolean p_transpose, const GLfloat* p_value
    GLint loc;
    if ((loc=uniform(false, true, 2, 3, GL_FLOAT, NULL))<0) RETURN;
    real(loc, p_count, p_transpose, p_value);

glUniformMatrix3x2fv: //GLint p_location, GLsizei p_count, GLboolean p_transpose, const GLfloat* p_value
    GLint loc;
    if ((loc=uniform(false, true, 3, 2, GL_FLOAT, NULL))<0) RETURN;
    real(loc, p_count, p_transpose, p_value);

glUniformMatrix2x4fv: //GLint p_location, GLsizei p_count, GLboolean p_transpose, const GLfloat* p_value
    GLint loc;
    if ((loc=uniform(false, true, 2, 4, GL_FLOAT, NULL))<0) RETURN;
    real(loc, p_count, p_transpose, p_value);

glUniformMatrix4x2fv: //GLint p_location, GLsizei p_count, GLboolean p_transpose, const GLfloat* p_value
    GLint loc;
    if ((loc=uniform(false, true, 4, 2, GL_FLOAT, NULL))<0) RETURN;
    real(loc, p_count, p_transpose, p_value);

glUniformMatrix3x4fv: //GLint p_location, GLsizei p_count, GLboolean p_transpose, const GLfloat* p_value
    GLint loc;
    if ((loc=uniform(false, true, 3, 4, GL_FLOAT, NULL))<0) RETURN;
    real(loc, p_count, p_transpose, p_value);

glUniformMatrix4x3fv: //GLint p_location, GLsizei p_count, GLboolean p_transpose, const GLfloat* p_value
    GLint loc;
    if ((loc=uniform(false, true, 4, 3, GL_FLOAT, NULL))<0) RETURN;
    real(loc, p_count, p_transpose, p_value);

glUniformMatrix2dv: //GLint p_location, GLsizei p_count, GLboolean p_transpose, const GLdouble* p_value
    GLint loc;
    if ((loc=uniform(false, true, 2, 2, GL_DOUBLE, NULL))<0) RETURN;
    real(loc, p_count, p_transpose, p_value);

glUniformMatrix3dv: //GLint p_location, GLsizei p_count, GLboolean p_transpose, const GLdouble* p_value
    GLint loc;
    if ((loc=uniform(false, true, 3, 3, GL_DOUBLE, NULL))<0) RETURN;
    real(loc, p_count, p_transpose, p_value);

glUniformMatrix4dv: //GLint p_location, GLsizei p_count, GLboolean p_transpose, const GLdouble* p_value
    GLint loc;
    if ((loc=uniform(false, true, 4, 4, GL_DOUBLE, NULL))<0) RETURN;
    real(loc, p_count, p_transpose, p_value);

glUniformMatrix2x3dv: //GLint p_location, GLsizei p_count, GLboolean p_transpose, const GLdouble* p_value
    GLint loc;
    if ((loc=uniform(false, true, 2, 3, GL_DOUBLE, NULL))<0) RETURN;
    real(loc, p_count, p_transpose, p_value);

glUniformMatrix3x2dv: //GLint p_location, GLsizei p_count, GLboolean p_transpose, const GLdouble* p_value
    GLint loc;
    if ((loc=uniform(false, true, 3, 2, GL_DOUBLE, NULL))<0) RETURN;
    real(loc, p_count, p_transpose, p_value);

glUniformMatrix2x4dv: //GLint p_location, GLsizei p_count, GLboolean p_transpose, const GLdouble* p_value
    GLint loc;
    if ((loc=uniform(false, true, 2, 4, GL_DOUBLE, NULL))<0) RETURN;
    real(loc, p_count, p_transpose, p_value);

glUniformMatrix4x2dv: //GLint p_location, GLsizei p_count, GLboolean p_transpose, const GLdouble* p_value
    GLint loc;
    if ((loc=uniform(false, true, 4, 2, GL_DOUBLE, NULL))<0) RETURN;
    real(loc, p_count, p_transpose, p_value);

glUniformMatrix3x4dv: //GLint p_location, GLsizei p_count, GLboolean p_transpose, const GLdouble* p_value
    GLint loc;
    if ((loc=uniform(false, true, 3, 4, GL_DOUBLE, NULL))<0) RETURN;
    real(loc, p_count, p_transpose, p_value);

glUniformMatrix4x3dv: //GLint p_location, GLsizei p_count, GLboolean p_transpose, const GLdouble* p_value
    GLint loc;
    if ((loc=uniform(false, true, 4, 3, GL_DOUBLE, NULL))<0) RETURN;
    real(loc, p_count, p_transpose, p_value);

glProgramUniform1f: //GLuint p_program, GLint p_location, GLfloat p_v0
    GLint loc; GLuint program;
    if ((loc=uniform(true, false, 1, 1, GL_FLOAT, &program))<0) RETURN;
    real(program, loc, p_v0);

glProgramUniform2f: //GLuint p_program, GLint p_location, GLfloat p_v0, GLfloat p_v1
    GLint loc; GLuint program;
    if ((loc=uniform(true, false, 2, 1, GL_FLOAT, &program))<0) RETURN;
    real(program, loc, p_v0, p_v1);

glProgramUniform3f: //GLuint p_program, GLint p_location, GLfloat p_v0, GLfloat p_v1, GLfloat p_v2
    GLint loc; GLuint program;
    if ((loc=uniform(true, false, 3, 1, GL_FLOAT, &program))<0) RETURN;
    real(program, loc, p_v0, p_v1, p_v2);

glProgramUniform4f: //GLuint p_program, GLint p_location, GLfloat p_v0, GLfloat p_v1, GLfloat p_v2, GLfloat p_v3
    GLint loc; GLuint program;
    if ((loc=uniform(true, false, 4, 1, GL_FLOAT, &program))<0) RETURN;
    real(program, loc, p_v0, p_v1, p_v2, p_v3);

glProgramUniform1i: //GLuint p_program, GLint p_location, GLint p_v0
    GLint loc; GLuint program;
    if ((loc=uniform(true, false, 1, 1, GL_INT, &program))<0) RETURN;
    real(program, loc, p_v0);

glProgramUniform2i: //GLuint p_program, GLint p_location, GLint p_v0, GLint p_v1
    GLint loc; GLuint program;
    if ((loc=uniform(true, false, 2, 1, GL_INT, &program))<0) RETURN;
    real(program, loc, p_v0, p_v1);

glProgramUniform3i: //GLuint p_program, GLint p_location, GLint p_v0, GLint p_v1, GLint p_v2
    GLint loc; GLuint program;
    if ((loc=uniform(true, false, 3, 1, GL_INT, &program))<0) RETURN;
    real(program, loc, p_v0, p_v1, p_v2);

glProgramUniform4i: //GLuint p_program, GLint p_location, GLint p_v0, GLint p_v1, GLint p_v2, GLint p_v3
    GLint loc; GLuint program;
    if ((loc=uniform(true, false, 4, 1, GL_INT, &program))<0) RETURN;
    real(program, loc, p_v0, p_v1, p_v2, p_v3);

glProgramUniform1ui: //GLuint p_program, GLint p_location, GLuint p_v0
    GLint loc; GLuint program;
    if ((loc=uniform(true, false, 1, 1, GL_UNSIGNED_INT, &program))<0) RETURN;
    real(program, loc, p_v0);

glProgramUniform2ui: //GLuint p_program, GLint p_location, GLuint p_v0, GLuint p_v1
    GLint loc; GLuint program;
    if ((loc=uniform(true, false, 2, 1, GL_UNSIGNED_INT, &program))<0) RETURN;
    real(program, loc, p_v0, p_v1);

glProgramUniform3ui: //GLuint p_program, GLint p_location, GLuint p_v0, GLuint p_v1, GLuint p_v2
    GLint loc; GLuint program;
    if ((loc=uniform(true, false, 3, 1, GL_UNSIGNED_INT, &program))<0) RETURN;
    real(program, loc, p_v0, p_v1, p_v2);

glProgramUniform4ui: //GLuint p_program, GLint p_location, GLuint p_v0, GLuint p_v1, GLuint p_v2, GLuint p_v3
    GLint loc; GLuint program;
    if ((loc=uniform(true, false, 4, 1, GL_UNSIGNED_INT, &program))<0) RETURN;
    real(program, loc, p_v0, p_v1, p_v2, p_v3);

glProgramUniform1d: //GLuint p_program, GLint p_location, GLdouble p_v0
    GLint loc; GLuint program;
    if ((loc=uniform(true, false, 1, 1, GL_DOUBLE, &program))<0) RETURN;
    real(program, loc, p_v0);

glProgramUniform2d: //GLuint p_program, GLint p_location, GLdouble p_v0, GLdouble p_v1
    GLint loc; GLuint program;
    if ((loc=uniform(true, false, 2, 1, GL_DOUBLE, &program))<0) RETURN;
    real(program, loc, p_v0, p_v1);

glProgramUniform3d: //GLuint p_program, GLint p_location, GLdouble p_v0, GLdouble p_v1, GLdouble p_v2
    GLint loc; GLuint program;
    if ((loc=uniform(true, false, 3, 1, GL_DOUBLE, &program))<0) RETURN;
    real(program, loc, p_v0, p_v1, p_v2);

glProgramUniform4d: //GLuint p_program, GLint p_location, GLdouble p_v0, GLdouble p_v1, GLdouble p_v2, GLdouble p_v3
    GLint loc; GLuint program;
    if ((loc=uniform(true, false, 4, 1, GL_DOUBLE, &program))<0) RETURN;
    real(program, loc, p_v0, p_v1, p_v2, p_v3);

glProgramUniform1fv: //GLuint p_program, GLint p_location, GLsizei p_count, const GLfloat* p_value
    GLint loc; GLuint program;
    if ((loc=uniform(true, true, 1, 1, GL_FLOAT, &program))<0) RETURN;
    real(program, loc, p_count, p_value);

glProgramUniform2fv: //GLuint p_program, GLint p_location, GLsizei p_count, const GLfloat* p_value
    GLint loc; GLuint program;
    if ((loc=uniform(true, true, 2, 1, GL_FLOAT, &program))<0) RETURN;
    real(program, loc, p_count, p_value);

glProgramUniform3fv: //GLuint p_program, GLint p_location, GLsizei p_count, const GLfloat* p_value
    GLint loc; GLuint program;
    if ((loc=uniform(true, true, 3, 1, GL_FLOAT, &program))<0) RETURN;
    real(program, loc, p_count, p_value);

glProgramUniform4fv: //GLuint p_program, GLint p_location, GLsizei p_count, const GLfloat* p_value
    GLint loc; GLuint program;
    if ((loc=uniform(true, true, 4, 1, GL_FLOAT, &program))<0) RETURN;
    real(program, loc, p_count, p_value);

glProgramUniform1iv: //GLuint p_program, GLint p_location, GLsizei p_count, const GLint* p_value
    GLint loc; GLuint program;
    if ((loc=uniform(true, true, 1, 1, GL_INT, &program))<0) RETURN;
    real(program, loc, p_count, p_value);

glProgramUniform2iv: //GLuint p_program, GLint p_location, GLsizei p_count, const GLint* p_value
    GLint loc; GLuint program;
    if ((loc=uniform(true, true, 2, 1, GL_INT, &program))<0) RETURN;
    real(program, loc, p_count, p_value);

glProgramUniform3iv: //GLuint p_program, GLint p_location, GLsizei p_count, const GLint* p_value
    GLint loc; GLuint program;
    if ((loc=uniform(true, true, 3, 1, GL_INT, &program))<0) RETURN;
    real(program, loc, p_count, p_value);

glProgramUniform4iv: //GLuint p_program, GLint p_location, GLsizei p_count, const GLint* p_value
    GLint loc; GLuint program;
    if ((loc=uniform(true, true, 4, 1, GL_INT, &program))<0) RETURN;
    real(program, loc, p_count, p_value);

glProgramUniform1uiv: //GLuint p_program, GLint p_location, GLsizei p_count, const GLuint* p_value
    GLint loc; GLuint program;
    if ((loc=uniform(true, true, 1, 1, GL_UNSIGNED_INT, &program))<0) RETURN;
    real(program, loc, p_count, p_value);

glProgramUniform2uiv: //GLuint p_program, GLint p_location, GLsizei p_count, const GLuint* p_value
    GLint loc; GLuint program;
    if ((loc=uniform(true, true, 2, 1, GL_UNSIGNED_INT, &program))<0) RETURN;
    real(program, loc, p_count, p_value);

glProgramUniform3uiv: //GLuint p_program, GLint p_location, GLsizei p_count, const GLuint* p_value
    GLint loc; GLuint program;
    if ((loc=uniform(true, true, 3, 1, GL_UNSIGNED_INT, &program))<0) RETURN;
    real(program, loc, p_count, p_value);

glProgramUniform4uiv: //GLuint p_program, GLint p_location, GLsizei p_count, const GLuint* p_value
    GLint loc; GLuint program;
    if ((loc=uniform(true, true, 4, 1, GL_UNSIGNED_INT, &program))<0) RETURN;
    real(program, loc, p_count, p_value);

glProgramUniform1dv: //GLuint p_program, GLint p_location, GLsizei p_count, const GLdouble* p_value
    GLint loc; GLuint program;
    if ((loc=uniform(true, true, 1, 1, GL_DOUBLE, &program))<0) RETURN;
    real(program, loc, p_count, p_value);

glProgramUniform2dv: //GLuint p_program, GLint p_location, GLsizei p_count, const GLdouble* p_value
    GLint loc; GLuint program;
    if ((loc=uniform(true, true, 2, 1, GL_DOUBLE, &program))<0) RETURN;
    real(program, loc, p_count, p_value);

glProgramUniform3dv: //GLuint p_program, GLint p_location, GLsizei p_count, const GLdouble* p_value
    GLint loc; GLuint program;
    if ((loc=uniform(true, true, 3, 1, GL_UNSIGNED_INT, &program))<0) RETURN;
    real(program, loc, p_count, p_value);

glProgramUniform4dv: //GLuint p_program, GLint p_location, GLsizei p_count, const GLdouble* p_value
    GLint loc; GLuint program;
    if ((loc=uniform(true, true, 4, 1, GL_UNSIGNED_INT, &program))<0) RETURN;
    real(program, loc, p_count, p_value);

glProgramUniformMatrix2fv: //GLuint p_program, GLint p_location, GLsizei p_count, GLboolean p_transpose, const GLfloat* p_value
    GLint loc; GLuint program;
    if ((loc=uniform(true, true, 2, 2, GL_FLOAT, &program))<0) RETURN;
    real(program, loc, p_count, p_transpose, p_value);

glProgramUniformMatrix3fv: //GLuint p_program, GLint p_location, GLsizei p_count, GLboolean p_transpose, const GLfloat* p_value
    GLint loc; GLuint program;
    if ((loc=uniform(true, true, 3, 3, GL_FLOAT, &program))<0) RETURN;
    real(program, loc, p_count, p_transpose, p_value);

glProgramUniformMatrix4fv: //GLuint p_program, GLint p_location, GLsizei p_count, GLboolean p_transpose, const GLfloat* p_value
    GLint loc; GLuint program;
    if ((loc=uniform(true, true, 4, 4, GL_FLOAT, &program))<0) RETURN;
    real(program, loc, p_count, p_transpose, p_value);

glProgramUniformMatrix2x3fv: //GLuint p_program, GLint p_location, GLsizei p_count, GLboolean p_transpose, const GLfloat* p_value
    GLint loc; GLuint program;
    if ((loc=uniform(true, true, 2, 3, GL_FLOAT, &program))<0) RETURN;
    real(program, loc, p_count, p_transpose, p_value);

glProgramUniformMatrix3x2fv: //GLuint p_program, GLint p_location, GLsizei p_count, GLboolean p_transpose, const GLfloat* p_value
    GLint loc; GLuint program;
    if ((loc=uniform(true, true, 3, 2, GL_FLOAT, &program))<0) RETURN;
    real(program, loc, p_count, p_transpose, p_value);

glProgramUniformMatrix2x4fv: //GLuint p_program, GLint p_location, GLsizei p_count, GLboolean p_transpose, const GLfloat* p_value
    GLint loc; GLuint program;
    if ((loc=uniform(true, true, 2, 4, GL_FLOAT, &program))<0) RETURN;
    real(program, loc, p_count, p_transpose, p_value);

glProgramUniformMatrix4x2fv: //GLuint p_program, GLint p_location, GLsizei p_count, GLboolean p_transpose, const GLfloat* p_value
    GLint loc; GLuint program;
    if ((loc=uniform(true, true, 4, 2, GL_FLOAT, &program))<0) RETURN;
    real(program, loc, p_count, p_transpose, p_value);

glProgramUniformMatrix3x4fv: //GLuint p_program, GLint p_location, GLsizei p_count, GLboolean p_transpose, const GLfloat* p_value
    GLint loc; GLuint program;
    if ((loc=uniform(true, true, 3, 4, GL_FLOAT, &program))<0) RETURN;
    real(program, loc, p_count, p_transpose, p_value);

glProgramUniformMatrix4x3fv: //GLuint p_program, GLint p_location, GLsizei p_count, GLboolean p_transpose, const GLfloat* p_value
    GLint loc; GLuint program;
    if ((loc=uniform(true, true, 4, 3, GL_FLOAT, &program))<0) RETURN;
    real(program, loc, p_count, p_transpose, p_value);

glProgramUniformMatrix2dv: //GLuint p_program, GLint p_location, GLsizei p_count, GLboolean p_transpose, const GLdouble* p_value
    GLint loc; GLuint program;
    if ((loc=uniform(true, true, 2, 2, GL_DOUBLE, &program))<0) RETURN;
    real(program, loc, p_count, p_transpose, p_value);

glProgramUniformMatrix3dv: //GLuint p_program, GLint p_location, GLsizei p_count, GLboolean p_transpose, const GLdouble* p_value
    GLint loc; GLuint program;
    if ((loc=uniform(true, true, 3, 3, GL_DOUBLE, &program))<0) RETURN;
    real(program, loc, p_count, p_transpose, p_value);

glProgramUniformMatrix4dv: //GLuint p_program, GLint p_location, GLsizei p_count, GLboolean p_transpose, const GLdouble* p_value
    GLint loc; GLuint program;
    if ((loc=uniform(true, true, 4, 4, GL_DOUBLE, &program))<0) RETURN;
    real(program, loc, p_count, p_transpose, p_value);

glProgramUniformMatrix2x3dv: //GLuint p_program, GLint p_location, GLsizei p_count, GLboolean p_transpose, const GLdouble* p_value
    GLint loc; GLuint program;
    if ((loc=uniform(true, true, 2, 3, GL_DOUBLE, &program))<0) RETURN;
    real(program, loc, p_count, p_transpose, p_value);

glProgramUniformMatrix3x2dv: //GLuint p_program, GLint p_location, GLsizei p_count, GLboolean p_transpose, const GLdouble* p_value
    GLint loc; GLuint program;
    if ((loc=uniform(true, true, 3, 2, GL_DOUBLE, &program))<0) RETURN;
    real(program, loc, p_count, p_transpose, p_value);

glProgramUniformMatrix2x4dv: //GLuint p_program, GLint p_location, GLsizei p_count, GLboolean p_transpose, const GLdouble* p_value
    GLint loc; GLuint program;
    if ((loc=uniform(true, true, 2, 4, GL_DOUBLE, &program))<0) RETURN;
    real(program, loc, p_count, p_transpose, p_value);

glProgramUniformMatrix4x2dv: //GLuint p_program, GLint p_location, GLsizei p_count, GLboolean p_transpose, const GLdouble* p_value
    GLint loc; GLuint program;
    if ((loc=uniform(true, true, 4, 2, GL_DOUBLE, &program))<0) RETURN;
    real(program, loc, p_count, p_transpose, p_value);

glProgramUniformMatrix3x4dv: //GLuint p_program, GLint p_location, GLsizei p_count, GLboolean p_transpose, const GLdouble* p_value
    GLint loc; GLuint program;
    if ((loc=uniform(true, true, 3, 4, GL_DOUBLE, &program))<0) RETURN;
    real(program, loc, p_count, p_transpose, p_value);

glProgramUniformMatrix4x3dv: //GLuint p_program, GLint p_location, GLsizei p_count, GLboolean p_transpose, const GLdouble* p_value
    GLint loc; GLuint program;
    if ((loc=uniform(true, true, 4, 3, GL_DOUBLE, &program))<0) RETURN;
    real(program, loc, p_count, p_transpose, p_value);

glGetUniformfv: //GLuint p_program, GLint p_location, GLfloat* p_params
    validate_get_uniform();

glGetUniformiv: //GLuint p_program, GLint p_location, GLint* p_params
    validate_get_uniform();

glGetUniformuiv: //GLuint p_program, GLint p_location, GLuint* p_params
    validate_get_uniform();

glGetUniformdv: //GLuint p_program, GLint p_location, GLdouble* p_params
    validate_get_uniform();

glGetnUniformfv: //GLuint p_program, GLint p_location, GLsizei p_bufSize, GLfloat* p_params
    validate_get_uniform();

glGetnUniformiv: //GLuint p_program, GLint p_location, GLsizei p_bufSize, GLint* p_params
    validate_get_uniform();

glGetnUniformuiv: //GLuint p_program, GLint p_location, GLsizei p_bufSize, GLuint* p_params
    validate_get_uniform();

glGetnUniformdv: //GLuint p_program, GLint p_location, GLsizei p_bufSize, GLdouble* p_params
    validate_get_uniform();
