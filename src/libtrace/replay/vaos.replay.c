#include "libtrace/replay/utils.h"

static void gen_vertex_arrays(size_t count, const GLuint* real, const GLuint* fake, bool create) {
    trc_gl_vao_rev_t rev;
    rev.has_object = create;
    rev.element_buffer.obj = NULL;
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
        attribs[j].buffer = (trc_obj_ref_t){NULL};
    }
    trc_unmap_data(attribs);
    for (size_t i = 0; i < count; ++i) {
        rev.real = real[i];
        trc_create_named_obj(ctx->priv_ns, TrcVAO, fake[i], &rev);
    }
}

static void get_vertex_attrib(GLuint index, GLenum pname) {
    if (pname!=GL_CURRENT_VERTEX_ATTRIB && !trc_gl_state_get_bound_vao(ctx->trace))
        ERROR2(, "No vertex array object is bound");
    if (index >= trc_gl_state_get_state_int(ctx->trace, GL_MAX_VERTEX_ATTRIBS, 0))
        ERROR2(, "Index is out of bounds");
}

//type in [GL_FLOAT, GL_DOUBLE, GL_UNSIGNED_BYTE, GL_BYTE, GL_SHORT, GL_UNSIGNED_SHORT, GL_UNSIGNED_INT, GL_INT]
//internal in [GL_FLOAT, GL_DOUBLE, GL_UNSIGNED_INT, GL_INT]
static void vertex_attrib(uint comp, GLenum type, bool array, bool normalized, GLenum internal) {
    uint index = trc_get_uint(&cmd->args[0])[0];
    if (index>=trc_gl_state_get_state_int(ctx->trace, GL_MAX_VERTEX_ATTRIBS, 0))
        ERROR2(, "Invalid vertex attribute index");
    uint i = 0;
    for (; i < comp; i++) {
        double val = 0;
        if (array) {
            switch (type) {
            case GL_UNSIGNED_BYTE:
            case GL_UNSIGNED_SHORT:
            case GL_UNSIGNED_INT: val = trc_get_int(&cmd->args[1])[i]; break;
            case GL_BYTE:
            case GL_SHORT:
            case GL_INT: val = trc_get_uint(&cmd->args[1])[i]; break;
            case GL_FLOAT:
            case GL_DOUBLE: val = trc_get_double(&cmd->args[1])[i]; break;
            }
        } else {
            switch (type) {
            case GL_UNSIGNED_BYTE:
            case GL_UNSIGNED_SHORT:
            case GL_UNSIGNED_INT: val = trc_get_int(&cmd->args[i+1])[0]; break;
            case GL_BYTE:
            case GL_SHORT:
            case GL_INT: val = trc_get_uint(&cmd->args[i+1])[0]; break;
            case GL_FLOAT:
            case GL_DOUBLE: val = trc_get_double(&cmd->args[i+1])[0]; break;
            }
        }
        if (internal==GL_FLOAT) val = (float)val;
        if (normalized) {
            uint ver = trc_gl_state_get_ver(ctx->trace);
            switch (type) {
            case GL_UNSIGNED_BYTE: val /= UINT8_MAX; break;
            case GL_UNSIGNED_SHORT: val /= UINT16_MAX; break;
            case GL_UNSIGNED_INT: val /= UINT32_MAX; break;
            case GL_BYTE: val = conv_from_signed_norm(ver, val, 8); break;
            case GL_SHORT: val = conv_from_signed_norm(ver, val, 16); break;
            case GL_INT: val = conv_from_signed_norm(ver, val, 32); break;
            }
        }
        trc_gl_state_set_state_double(ctx->trace, GL_CURRENT_VERTEX_ATTRIB, index*4+i, val);
    }
    for (; i < 3; i++)
        trc_gl_state_set_state_double(ctx->trace, GL_CURRENT_VERTEX_ATTRIB, index*4+i, 0);
    for (; i < 4; i++)
        trc_gl_state_set_state_double(ctx->trace, GL_CURRENT_VERTEX_ATTRIB, index*4+i, 1);
    
    double vals[4];
    for (i = 0; i < 4; i++)
        vals[i] = trc_gl_state_get_state_double(ctx->trace, GL_CURRENT_VERTEX_ATTRIB, index*4+i);
    
    switch (internal) {
    case GL_FLOAT: F(glVertexAttrib4dv)(index, vals); break;
    case GL_DOUBLE: F(glVertexAttribL4dv)(index, vals); break;
    case GL_UNSIGNED_INT: F(glVertexAttribI4ui)(index, vals[0], vals[1], vals[2], vals[3]); break;
    case GL_INT: F(glVertexAttribI4i)(index, vals[0], vals[1], vals[2], vals[3]); break;
    }
}

static void vertex_attrib_packed(GLuint index, GLenum type, uint comp, GLboolean normalized, GLuint val) {
    if (index>=trc_gl_state_get_state_int(ctx->trace, GL_MAX_VERTEX_ATTRIBS, 0))
        ERROR2(, "Invalid vertex attribute index");
    double res[4];
    switch (type) {
    case GL_INT_2_10_10_10_REV:
        parse_int_2_10_10_10_rev(trc_gl_state_get_ver(ctx->trace), res, val, normalized);
        break;
    case GL_UNSIGNED_INT_2_10_10_10_REV:
        parse_uint_2_10_10_10_rev(res, val, normalized);
        break;
    case GL_UNSIGNED_INT_10F_11F_11F_REV:
        parse_int_10f_11f_11f_rev(res, val);
        break;
    }
    for (uint i = comp; i < 4; i++) res[i] = i==3 ? 1.0 : 0.0;
    for (uint i = 0; i < 4; i++)
        trc_gl_state_set_state_double(ctx->trace, GL_CURRENT_VERTEX_ATTRIB, index*4+i, res[i]);
    
    F(glVertexAttrib4dv(index, res));
}

glGenVertexArrays: //GLsizei p_n, GLuint* p_arrays
    if (p_n < 0) ERROR("Invalid vertex array object name count");
    GLuint* arrays = replay_alloc(p_n*sizeof(GLuint));
    real(p_n, arrays);
    gen_vertex_arrays(p_n, arrays, p_arrays, false);

glCreateVertexArrays: //GLsizei p_n, GLuint* p_arrays
    if (p_n < 0) ERROR("Invalid vertex array object name count");
    GLuint* arrays = replay_alloc(p_n*sizeof(GLuint));
    real(p_n, arrays);
    gen_vertex_arrays(p_n, arrays, p_arrays, true);

glDeleteVertexArrays: //GLsizei p_n, const GLuint* p_arrays
    GLuint* arrays = replay_alloc(p_n*sizeof(GLuint));
    for (size_t i = 0; i < p_n; ++i) {
        trc_obj_t* vao = trc_lookup_name(ctx->priv_ns, TrcVAO, p_arrays[i], -1);
        if (vao && vao==trc_gl_state_get_bound_vao(ctx->trace))
            trc_gl_state_set_bound_vao(ctx->trace, 0);
        if (!(arrays[i]=get_real_vao(p_arrays[i])) && p_arrays[i])
            trc_add_warning(cmd, "Invalid vertex array name");
        else
            delete_obj(p_arrays[i], TrcVAO);
    }
    real(p_n, arrays);

glBindVertexArray: //GLuint p_array
    const trc_gl_vao_rev_t* rev = get_vao(p_array);
    if (!rev && p_array) ERROR("Invalid vertex array name");
    if (rev && !rev->has_object) {
        trc_gl_vao_rev_t newrev = *rev;
        newrev.has_object = true;
        set_vao(&newrev);
    }
    trc_gl_state_set_bound_vao(ctx->trace, rev?rev->head.obj:NULL);

//TODO: There is some duplicate code among glVertexAttrib*Pointer and gl*VertexAttribArray
glVertexAttribPointer: //GLuint p_index, GLint p_size, GLenum p_type, GLboolean p_normalized, GLsizei p_stride, const void* p_pointer
    //if (p_pointer > UINTPTR_MAX) //TODO
    if (!trc_gl_state_get_bound_vao(ctx->trace)) RETURN;
    trc_gl_vao_rev_t rev = *(const trc_gl_vao_rev_t*)trc_obj_get_rev(trc_gl_state_get_bound_vao(ctx->trace), -1);
    if (p_index < rev.attribs->size/sizeof(trc_gl_vao_attrib_t)) {
        trc_data_t* newattribs = trc_copy_data(ctx->trace, rev.attribs);
        trc_gl_vao_attrib_t* attribs = trc_map_data(newattribs, TRC_MAP_READ);
        trc_gl_vao_attrib_t* a = &attribs[p_index];
        a->normalized = p_normalized;
        a->integer = false;
        a->size = p_size;
        a->stride = p_stride;
        a->offset = p_pointer;
        a->type = p_type;
        trc_set_obj_ref(&a->buffer, trc_gl_state_get_bound_buffer(ctx->trace, GL_ARRAY_BUFFER));
        trc_unmap_data(attribs);
        rev.attribs = newattribs;
        set_vao(&rev);
    }

glVertexAttribIPointer: //GLuint p_index, GLint p_size, GLenum p_type, GLsizei p_stride, const void* p_pointer
    //if (p_pointer > UINTPTR_MAX) //TODO
    if (!trc_gl_state_get_bound_vao(ctx->trace)) RETURN;
    trc_gl_vao_rev_t rev = *(const trc_gl_vao_rev_t*)trc_obj_get_rev(trc_gl_state_get_bound_vao(ctx->trace), -1);
    if (p_index < rev.attribs->size/sizeof(trc_gl_vao_attrib_t)) {
        trc_data_t* newattribs = trc_copy_data(ctx->trace, rev.attribs);
        trc_gl_vao_attrib_t* attribs = trc_map_data(newattribs, TRC_MAP_READ);
        trc_gl_vao_attrib_t* a = &attribs[p_index];
        a->integer = true;
        a->size = p_size;
        a->stride = p_stride;
        a->offset = p_pointer;
        a->type = p_type;
        trc_set_obj_ref(&a->buffer, trc_gl_state_get_bound_buffer(ctx->trace, GL_ARRAY_BUFFER));
        trc_unmap_data(attribs);
        rev.attribs = newattribs;
        set_vao(&rev);
    }

glEnableVertexAttribArray: //GLuint p_index
    if (!trc_gl_state_get_bound_vao(ctx->trace)) RETURN;
    trc_gl_vao_rev_t rev = *(const trc_gl_vao_rev_t*)trc_obj_get_rev(trc_gl_state_get_bound_vao(ctx->trace), -1);
    if (p_index < rev.attribs->size/sizeof(trc_gl_vao_attrib_t)) {
        trc_data_t* newattribs = trc_copy_data(ctx->trace, rev.attribs);
        trc_gl_vao_attrib_t* attribs = trc_map_data(newattribs, TRC_MAP_READ);
        trc_gl_vao_attrib_t* a = &attribs[p_index];
        a->enabled = true;
        trc_unmap_data(attribs);
        rev.attribs = newattribs;
        set_vao(&rev);
    }

glDisableVertexAttribArray: //GLuint p_index
    if (!trc_gl_state_get_bound_vao(ctx->trace)) RETURN;
    trc_gl_vao_rev_t rev = *(const trc_gl_vao_rev_t*)trc_obj_get_rev(trc_gl_state_get_bound_vao(ctx->trace), -1);
    if (p_index < rev.attribs->size/sizeof(trc_gl_vao_attrib_t)) {
        trc_data_t* newattribs = trc_copy_data(ctx->trace, rev.attribs);
        trc_gl_vao_attrib_t* attribs = trc_map_data(newattribs, TRC_MAP_READ);
        trc_gl_vao_attrib_t* a = &attribs[p_index];
        a->enabled = false;
        trc_unmap_data(attribs);
        rev.attribs = newattribs;
        set_vao(&rev);
    }

glVertexAttribDivisor: //GLuint p_index, GLuint p_divisor
    if (!trc_gl_state_get_bound_vao(ctx->trace)) RETURN;
    trc_gl_vao_rev_t rev = *(const trc_gl_vao_rev_t*)trc_obj_get_rev(trc_gl_state_get_bound_vao(ctx->trace), -1);
    if (p_index < rev.attribs->size/sizeof(trc_gl_vao_attrib_t)) {
        trc_data_t* newattribs = trc_copy_data(ctx->trace, rev.attribs);
        trc_gl_vao_attrib_t* attribs = trc_map_data(newattribs, TRC_MAP_READ);
        trc_gl_vao_attrib_t* a = &attribs[p_index];
        a->divisor = p_divisor;
        trc_unmap_data(attribs);
        rev.attribs = newattribs;
        set_vao(&rev);
    }

glVertexAttrib1f: //GLuint p_index, GLfloat p_v0
    vertex_attrib(1, GL_FLOAT, false, false, GL_FLOAT);

glVertexAttrib1s: //GLuint p_index, GLshort p_v0
    vertex_attrib(1, GL_INT, false, false, GL_FLOAT);

glVertexAttrib1d: //GLuint p_index, GLdouble p_v0
    vertex_attrib(1, GL_DOUBLE, false, false, GL_FLOAT);

glVertexAttribI1i: //GLuint p_index, GLint p_v0
    vertex_attrib(1, GL_INT, false, false, GL_INT);

glVertexAttribI1ui: //GLuint p_index, GLuint p_v0
    vertex_attrib(1, GL_UNSIGNED_INT, false, false, GL_UNSIGNED_INT);

glVertexAttribL1d: //GLuint p_index, GLdouble p_v0
    vertex_attrib(1, GL_DOUBLE, false, false, GL_DOUBLE);

glVertexAttrib2f: //GLuint p_index, GLfloat p_v0, GLfloat p_v1
    vertex_attrib(2, GL_FLOAT, false, false, GL_FLOAT);

glVertexAttrib2s: //GLuint p_index, GLshort p_v0, GLshort p_v1
    vertex_attrib(2, GL_INT, false, false, GL_FLOAT);

glVertexAttrib2d: //GLuint p_index, GLdouble p_v0, GLdouble p_v1
    vertex_attrib(2, GL_DOUBLE, false, false, GL_FLOAT);

glVertexAttribI2i: //GLuint p_index, GLint p_v0, GLint p_v1
    vertex_attrib(2, GL_INT, false, false, GL_INT);

glVertexAttribI2ui: //GLuint p_index, GLuint p_v0, GLuint p_v1
    vertex_attrib(2, GL_UNSIGNED_INT, false, false, GL_UNSIGNED_INT);

glVertexAttribL2d: //GLuint p_index, GLdouble p_v0, GLdouble p_v1
    vertex_attrib(2, GL_DOUBLE, false, false, GL_DOUBLE);

glVertexAttrib3f: //GLuint p_index, GLfloat p_v0, GLfloat p_v1, GLfloat p_v2
    vertex_attrib(3, GL_FLOAT, false, false, GL_FLOAT);

glVertexAttrib3s: //GLuint p_index, GLshort p_v0, GLshort p_v1, GLshort p_v2
    vertex_attrib(3, GL_INT, false, false, GL_FLOAT);

glVertexAttrib3d: //GLuint p_index, GLdouble p_v0, GLdouble p_v1, GLdouble p_v2
    vertex_attrib(3, GL_DOUBLE, false, false, GL_FLOAT);

glVertexAttribI3i: //GLuint p_index, GLint p_v0, GLint p_v1, GLint p_v2
    vertex_attrib(3, GL_INT, false, false, GL_INT);

glVertexAttribI3ui: //GLuint p_index, GLuint p_v0, GLuint p_v1, GLuint p_v2
    vertex_attrib(3, GL_UNSIGNED_INT, false, false, GL_UNSIGNED_INT);

glVertexAttribL3d: //GLuint p_index, GLdouble p_v0, GLdouble p_v1, GLdouble p_v2
    vertex_attrib(3, GL_DOUBLE, false, false, GL_DOUBLE);

glVertexAttrib4f: //GLuint p_index, GLfloat p_v0, GLfloat p_v1, GLfloat p_v2, GLfloat p_v3
    vertex_attrib(4, GL_FLOAT, false, false, GL_FLOAT);

glVertexAttrib4s: //GLuint p_index, GLshort p_v0, GLshort p_v1, GLshort p_v2, GLshort p_v3
    vertex_attrib(4, GL_INT, false, false, GL_FLOAT);

glVertexAttrib4d: //GLuint p_index, GLdouble p_v0, GLdouble p_v1, GLdouble p_v2, GLdouble p_v3
    vertex_attrib(4, GL_DOUBLE, false, false, GL_FLOAT);

glVertexAttribI4i: //GLuint p_index, GLint p_v0, GLint p_v1, GLint p_v2, GLint p_v3
    vertex_attrib(4, GL_INT, false, false, GL_INT);

glVertexAttribI4ui: //GLuint p_index, GLuint p_v0, GLuint p_v1, GLuint p_v2, GLuint p_v3
    vertex_attrib(4, GL_UNSIGNED_INT, false, false, GL_UNSIGNED_INT);

glVertexAttribL4d: //GLuint p_index, GLdouble p_v0, GLdouble p_v1, GLdouble p_v2, GLdouble p_v3
    vertex_attrib(4, GL_DOUBLE, false, false, GL_DOUBLE);

glVertexAttrib1dv: //GLuint p_index, const GLdouble* p_v
    vertex_attrib(1, GL_DOUBLE, true, false, GL_FLOAT);

glVertexAttrib1fv: //GLuint p_index, const GLfloat* p_v
    vertex_attrib(1, GL_FLOAT, true, false, GL_FLOAT);

glVertexAttrib1sv: //GLuint p_index, const GLshort* p_v
    vertex_attrib(1, GL_SHORT, true, false, GL_FLOAT);

glVertexAttrib2dv: //GLuint p_index, const GLdouble* p_v
    vertex_attrib(2, GL_DOUBLE, true, false, GL_FLOAT);

glVertexAttrib2fv: //GLuint p_index, const GLfloat* p_v
    vertex_attrib(2, GL_FLOAT, true, false, GL_FLOAT);

glVertexAttrib2sv: //GLuint p_index, const GLshort* p_v
    vertex_attrib(2, GL_SHORT, true, false, GL_FLOAT);

glVertexAttrib3dv: //GLuint p_index, const GLdouble* p_v
    vertex_attrib(3, GL_DOUBLE, true, false, GL_FLOAT);

glVertexAttrib3fv: //GLuint p_index, const GLfloat* p_v
    vertex_attrib(3, GL_FLOAT, true, false, GL_FLOAT);

glVertexAttrib3sv: //GLuint p_index, const GLshort* p_v
    vertex_attrib(3, GL_SHORT, true, false, GL_FLOAT);

glVertexAttrib4Nbv: //GLuint p_index, const GLbyte* p_v
    vertex_attrib(4, GL_BYTE, true, true, GL_FLOAT);

glVertexAttrib4Niv: //GLuint p_index, const GLint* p_v
    vertex_attrib(4, GL_INT, true, true, GL_FLOAT);

glVertexAttrib4Nsv: //GLuint p_index, const GLshort* p_v
    vertex_attrib(4, GL_SHORT, true, true, GL_FLOAT);

glVertexAttrib4Nubv: //GLuint p_index, const GLubyte* p_v
    vertex_attrib(4, GL_UNSIGNED_BYTE, true, true, GL_FLOAT);

glVertexAttrib4Nuiv: //GLuint p_index, const GLuint* p_v
    vertex_attrib(4, GL_UNSIGNED_INT, true, true, GL_FLOAT);

glVertexAttrib4Nusv: //GLuint p_index, const GLushort* p_v
    vertex_attrib(4, GL_UNSIGNED_SHORT, true, false, GL_DOUBLE);

glVertexAttrib4bv: //GLuint p_index, const GLbyte* p_v
    vertex_attrib(4, GL_BYTE, true, false, GL_DOUBLE);

glVertexAttrib4dv: //GLuint p_index, const GLdouble* p_v
    vertex_attrib(4, GL_DOUBLE, true, false, GL_DOUBLE);

glVertexAttrib4fv: //GLuint p_index, const GLfloat* p_v
    vertex_attrib(4, GL_FLOAT, true, false, GL_DOUBLE);

glVertexAttrib4iv: //GLuint p_index, const GLint* p_v
    vertex_attrib(4, GL_INT, true, false, GL_DOUBLE);

glVertexAttrib4sv: //GLuint p_index, const GLshort* p_v
    vertex_attrib(4, GL_SHORT, true, false, GL_DOUBLE);

glVertexAttrib4ubv: //GLuint p_index, const GLubyte* p_v
    vertex_attrib(4, GL_UNSIGNED_BYTE, true, false, GL_DOUBLE);

glVertexAttrib4uiv: //GLuint p_index, const GLuint* p_v
    vertex_attrib(4, GL_UNSIGNED_INT, true, false, GL_DOUBLE);

glVertexAttrib4usv: //GLuint p_index, const GLushort* p_v
    vertex_attrib(4, GL_UNSIGNED_SHORT, true, false, GL_DOUBLE);

glVertexAttrib4Nub: //GLuint p_index, GLubyte p_x, GLubyte p_y, GLubyte p_z, GLubyte p_w
    vertex_attrib(4, GL_UNSIGNED_BYTE, true, true, GL_FLOAT);

glVertexAttribI4ubv: //GLuint p_index, const GLubyte* p_v
    vertex_attrib(4, GL_UNSIGNED_SHORT, true, false, GL_UNSIGNED_INT);

glVertexAttribI4usv: //GLuint p_index, const GLushort* p_v
    vertex_attrib(4, GL_UNSIGNED_SHORT, true, false, GL_UNSIGNED_INT);

glVertexAttribI4sv: //GLuint p_index, const GLshort* p_v
    vertex_attrib(4, GL_SHORT, true, false, GL_INT);

glVertexAttribI3iv: //GLuint p_index, const GLint* p_v
    vertex_attrib(3, GL_INT, true, false, GL_INT);

glVertexAttribI4iv: //GLuint p_index, const GLint* p_v
    vertex_attrib(4, GL_INT, true, false, GL_INT);

glVertexAttribI2uiv: //GLuint p_index, const GLuint* p_v
    vertex_attrib(2, GL_UNSIGNED_INT, true, false, GL_UNSIGNED_INT);

glVertexAttribI4uiv: //GLuint p_index, const GLuint* p_v
    vertex_attrib(4, GL_UNSIGNED_INT, true, false, GL_UNSIGNED_INT);

glVertexAttribI4bv: //GLuint p_index, const GLbyte* p_v
    vertex_attrib(4, GL_BYTE, true, false, GL_INT);

glVertexAttribI1iv: //GLuint p_index, const GLint* p_v
    vertex_attrib(1, GL_INT, true, false, GL_INT);

glVertexAttribI2iv: //GLuint p_index, const GLint* p_v
    vertex_attrib(2, GL_INT, true, false, GL_INT);

glVertexAttribI3uiv: //GLuint p_index, const GLuint* p_v
    vertex_attrib(3, GL_UNSIGNED_INT, true, false, GL_UNSIGNED_INT);

glVertexAttribI1uiv: //GLuint p_index, const GLuint* p_v
    vertex_attrib(1, GL_UNSIGNED_INT, true, false, GL_UNSIGNED_INT);

glVertexAttribL1dv: //GLuint p_index, const GLdouble* p_v
    vertex_attrib(1, GL_DOUBLE, true, false, GL_DOUBLE);

glVertexAttribL2dv: //GLuint p_index, const GLdouble* p_v
    vertex_attrib(2, GL_DOUBLE, true, false, GL_DOUBLE);

glVertexAttribL3dv: //GLuint p_index, const GLdouble* p_v
    vertex_attrib(3, GL_DOUBLE, true, false, GL_DOUBLE);

glVertexAttribL4dv: //GLuint p_index, const GLdouble* p_v
    vertex_attrib(4, GL_DOUBLE, true, false, GL_DOUBLE);

glVertexAttribP1ui: //GLuint p_index, GLenum p_type, GLboolean p_normalized, GLuint p_value
    vertex_attrib_packed(p_index, p_type, 1, p_normalized, p_value);

glVertexAttribP2ui: //GLuint p_index, GLenum p_type, GLboolean p_normalized, GLuint p_value
    vertex_attrib_packed(p_index, p_type, 2, p_normalized, p_value);

glVertexAttribP3ui: //GLuint p_index, GLenum p_type, GLboolean p_normalized, GLuint p_value
    vertex_attrib_packed(p_index, p_type, 3, p_normalized, p_value);

glVertexAttribP4ui: //GLuint p_index, GLenum p_type, GLboolean p_normalized, GLuint p_value
    vertex_attrib_packed(p_index, p_type, 4, p_normalized, p_value);

glVertexAttribP1uiv: //GLuint p_index, GLenum p_type, GLboolean p_normalized, GLuint* p_value
    vertex_attrib_packed(p_index, p_type, 1, p_normalized, *p_value);

glVertexAttribP2uiv: //GLuint p_index, GLenum p_type, GLboolean p_normalized, GLuint* p_value
    vertex_attrib_packed(p_index, p_type, 2, p_normalized, *p_value);

glVertexAttribP3uiv: //GLuint p_index, GLenum p_type, GLboolean p_normalized, GLuint* p_value
    vertex_attrib_packed(p_index, p_type, 3, p_normalized, *p_value);

glVertexAttribP4uiv: //GLuint p_index, GLenum p_type, GLboolean p_normalized, GLuint* p_value
    vertex_attrib_packed(p_index, p_type, 4, p_normalized, *p_value);

glGetVertexAttribdv: //GLuint p_index, GLenum p_pname, GLdouble* p_params
    get_vertex_attrib(p_index, p_pname);

glGetVertexAttribfv: //GLuint p_index, GLenum p_pname, GLfloat* p_params
    get_vertex_attrib(p_index, p_pname);

glGetVertexAttribiv: //GLuint p_index, GLenum p_pname, GLint* p_params
    get_vertex_attrib(p_index, p_pname);

glGetVertexAttribIiv: //GLuint p_index, GLenum p_pname, GLint* p_params
    get_vertex_attrib(p_index, p_pname);

glGetVertexAttribIuiv: //GLuint p_index, GLenum p_pname, GLuint* p_params
    get_vertex_attrib(p_index, p_pname);

glGetVertexAttribLdv: //GLuint p_index, GLenum p_pname, GLdouble* p_params
    get_vertex_attrib(p_index, p_pname);

glGetVertexAttribPointerv: //GLuint p_index, GLenum p_pname, void** p_pointer
    get_vertex_attrib(p_index, p_pname);

glIsVertexArray: //GLuint p_array
    ;
