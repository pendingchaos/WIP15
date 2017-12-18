#include "libtrace/replay/utils.h"

static void gen_vertex_arrays(size_t count, const GLuint* real, const GLuint* fake, bool create) {
    trc_gl_vao_rev_t rev;
    rev.has_object = create;
    rev.element_buffer.obj = NULL;
    
    int attrib_count = gls_get_state_int(GL_MAX_VERTEX_ATTRIBS, 0);
    rev.attribs = trc_create_data(ctx->trace, attrib_count*sizeof(trc_gl_vao_attrib_t), NULL, TRC_DATA_NO_ZERO);
    trc_gl_vao_attrib_t* attribs = trc_map_data(rev.attribs, TRC_MAP_REPLACE);
    memset(attribs, 0, attrib_count*sizeof(trc_gl_vao_attrib_t)); //fill in padding to fix uninitialized memory errors during compession
    for (size_t i = 0; i < attrib_count; i++) {
        attribs[i].enabled = false;
        attribs[i].normalized = false;
        attribs[i].integer = false;
        attribs[i].size = 4;
        attribs[i].offset = 0;
        attribs[i].type = GL_FLOAT;
        attribs[i].buffer_index = 0;
    }
    trc_unmap_data(attribs);
    
    int binding_count = gls_get_state_int(GL_MAX_VERTEX_ATTRIB_BINDINGS, 0);
    if (gls_get_ver() < 430) binding_count = attrib_count;
    rev.buffers = trc_create_data(ctx->trace, binding_count*sizeof(trc_gl_vao_buffer_t), NULL, TRC_DATA_NO_ZERO);
    trc_gl_vao_buffer_t* buffers = trc_map_data(rev.buffers, TRC_MAP_REPLACE);
    memset(buffers, 0, binding_count*sizeof(trc_gl_vao_buffer_t)); //fill in padding to fix uninitialized memory errors during compession
    for (size_t i = 0; i < binding_count; i++) {
        buffers[i].offset = 0;
        buffers[i].stride = 0;
        buffers[i].divisor = 0;
        buffers[i].buffer = (trc_obj_ref_t){NULL};
    }
    trc_unmap_data(buffers);
    
    for (size_t i = 0; i < count; ++i) {
        rev.real = real[i];
        trc_create_named_obj(ctx->priv_ns, TrcVAO, fake[i], &rev);
    }
}

static void get_vertex_attrib(GLuint index, GLenum pname) {
    if (pname!=GL_CURRENT_VERTEX_ATTRIB && !gls_get_bound_vao())
        ERROR2(, "No vertex array object is bound");
    if (index >= gls_get_state_int(GL_MAX_VERTEX_ATTRIBS, 0))
        ERROR2(, "Index is greater than GL_MAX_VERTEX_ATTRIBS");
}

//type in [GL_FLOAT, GL_DOUBLE, GL_UNSIGNED_BYTE, GL_BYTE, GL_SHORT, GL_UNSIGNED_SHORT, GL_UNSIGNED_INT, GL_INT]
//internal in [GL_FLOAT, GL_DOUBLE, GL_UNSIGNED_INT, GL_INT]
static void vertex_attrib(uint comp, GLenum type, bool array, bool normalized, GLenum internal) {
    uint index = trc_get_uint(&cmd->args[0])[0];
    if (index>=gls_get_state_int(GL_MAX_VERTEX_ATTRIBS, 0))
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
            uint ver = gls_get_ver();
            switch (type) {
            case GL_UNSIGNED_BYTE: val /= UINT8_MAX; break;
            case GL_UNSIGNED_SHORT: val /= UINT16_MAX; break;
            case GL_UNSIGNED_INT: val /= UINT32_MAX; break;
            case GL_BYTE: val = conv_from_signed_norm(ver, val, 8); break;
            case GL_SHORT: val = conv_from_signed_norm(ver, val, 16); break;
            case GL_INT: val = conv_from_signed_norm(ver, val, 32); break;
            }
        }
        gls_set_current_vertex_attrib(index*4+i, val);
    }
    for (; i < 3; i++)
        gls_set_current_vertex_attrib(index*4+i, 0);
    for (; i < 4; i++)
        gls_set_current_vertex_attrib(index*4+i, 1);
    
    gls_set_current_vertex_attrib_types(index, internal);
    
    double vals[4];
    for (i = 0; i < 4; i++)
        vals[i] = gls_get_current_vertex_attrib(index*4+i);
    
    switch (internal) {
    case GL_FLOAT: F(glVertexAttrib4dv)(index, vals); break;
    case GL_DOUBLE: F(glVertexAttribL4dv)(index, vals); break;
    case GL_UNSIGNED_INT: F(glVertexAttribI4ui)(index, vals[0], vals[1], vals[2], vals[3]); break;
    case GL_INT: F(glVertexAttribI4i)(index, vals[0], vals[1], vals[2], vals[3]); break;
    }
}

static void vertex_attrib_packed(GLuint index, GLenum type, uint comp, GLboolean normalized, GLuint val) {
    if (index>=gls_get_state_int(GL_MAX_VERTEX_ATTRIBS, 0))
        ERROR2(, "Invalid vertex attribute index");
    double res[4];
    switch (type) {
    case GL_INT_2_10_10_10_REV:
        parse_int_2_10_10_10_rev(gls_get_ver(), res, val, normalized);
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
        gls_set_current_vertex_attrib(index*4+i, (float)res[i]);
    gls_set_current_vertex_attrib_types(index, GL_FLOAT);
    
    F(glVertexAttrib4dv(index, res));
}

static trc_gl_vao_buffer_t* map_bindings(trc_gl_vao_rev_t* vao) {
    vao->buffers = trc_copy_data(ctx->trace, vao->buffers);
    set_vao(vao);
    return trc_map_data(vao->buffers, TRC_MAP_MODIFY);
}

static trc_gl_vao_attrib_t* map_attribs(trc_gl_vao_rev_t* vao) {
    vao->attribs = trc_copy_data(ctx->trace, vao->attribs);
    set_vao(vao);
    return trc_map_data(vao->attribs, TRC_MAP_MODIFY);
}

static void vertex_attrib_format(trc_obj_t* vao, GLuint index, bool normalized, bool integer,
                                 GLint size, GLenum type, GLuint offset, GLint binding) {
    if (index >= gls_get_state_int(GL_MAX_VERTEX_ATTRIBS, 0))
        ERROR2(, "Index is greater than GL_MAX_VERTEX_ATTRIBS");
    if (not_one_of(size, 1, 2, 3, 4, -1) && (integer?true:size!=GL_BGRA))
        ERROR2(, "Invalid size");
    if (size==GL_BGRA && not_one_of(type, GL_UNSIGNED_BYTE, GL_INT_2_10_10_10_REV, GL_UNSIGNED_INT_2_10_10_10_REV, -1))
        ERROR2(, "Invalid size and type combination");
    if (type==GL_INT_2_10_10_10_REV && size!=4 && size!=GL_BGRA)
        ERROR2(, "Invalid size and type combination");
    if (type==GL_UNSIGNED_INT_2_10_10_10_REV && size!=4 && size!=GL_BGRA)
        ERROR2(, "Invalid size and type combination");
    if (type==GL_UNSIGNED_INT_10F_11F_11F_REV && size!=3)
        ERROR2(, "Invalid size and type combination");
    if (size==GL_BGRA && !normalized)
        ERROR2(, "Attributes of size GL_BGRA must be normalized");
    if (gls_get_ver()>=430 && offset>=gls_get_state_int(GL_MAX_VERTEX_ATTRIB_RELATIVE_OFFSET, 0))
        ERROR2(, "The relative offset is greater than GL_MAX_VERTEX_ATTRIB_RELATIVE_OFFSET");
    
    trc_gl_vao_rev_t rev = *(const trc_gl_vao_rev_t*)trc_obj_get_rev(vao, -1);
    
    trc_gl_vao_attrib_t* attribs = map_attribs(&rev);
    attribs[index].normalized = normalized;
    attribs[index].integer = integer;
    attribs[index].size = size;
    attribs[index].offset = offset;
    attribs[index].type = type;
    if (binding >= 0) attribs[index].buffer_index = binding;
    trc_unmap_data(attribs);
}

static void vertex_attrib_ptr(GLuint index, bool normalized, bool integer,
                              GLint size, GLenum type, uint64_t pointer, GLsizei stride) {
    if (!gls_get_bound_vao())
        ERROR2(, "No vertex array object is bound");
    if (gls_get_ver()>=430 && index>=gls_get_state_int(GL_MAX_VERTEX_ATTRIB_BINDINGS, 0))
        ERROR2(, "Index is greater than GL_MAX_VERTEX_ATTRIB_BINDINGS");
    if (!gls_get_bound_buffer(GL_ARRAY_BUFFER) && pointer)
        ERROR2(, "No buffer bound when pointer is not NULL");
    if (gls_get_ver()>=440 && stride>gls_get_state_int(GL_MAX_VERTEX_ATTRIB_STRIDE, 0))
        ERROR2(, "Stride is greater than GL_MAX_VERTEX_ATTRIB_STRIDE");
    
    vertex_attrib_format(gls_get_bound_vao(), index, normalized, integer, size, type, 0, index);
    
    trc_gl_vao_rev_t rev = *(const trc_gl_vao_rev_t*)trc_obj_get_rev(gls_get_bound_vao(), -1);
    trc_gl_vao_buffer_t* buffers = map_bindings(&rev);
    buffers[index].offset = pointer;
    if (!stride) {
        int size2 = size==GL_BGRA ? 4 : size;
        switch (type) {
        case GL_BYTE:
        case GL_UNSIGNED_BYTE: buffers[index].stride = size2; break;
        case GL_HALF_FLOAT:
        case GL_SHORT:
        case GL_UNSIGNED_SHORT: buffers[index].stride = size2 * 2; break;
        case GL_FLOAT:
        case GL_FIXED:
        case GL_INT:
        case GL_UNSIGNED_INT: buffers[index].stride = size2 * 4; break;
        case GL_DOUBLE: buffers[index].stride = size2 * 8; break;
        case GL_INT_2_10_10_10_REV:
        case GL_UNSIGNED_INT_2_10_10_10_REV:
        case GL_UNSIGNED_INT_10F_11F_11F_REV: buffers[index].stride = 4; break;
        }
    } else {
        buffers[index].stride = stride;
    }
    trc_set_obj_ref(&buffers[index].buffer, gls_get_bound_buffer(GL_ARRAY_BUFFER));
    trc_unmap_data(buffers);
}

static void bind_vertex_buffer(bool dsa, const trc_gl_vao_rev_t* vao, GLuint index, GLuint buffer, GLintptr offset, GLintptr stride) {
    if (!vao)
        ERROR2(, dsa?"Invalid vertex array object name":"No vertex array object is bound");
    if (!vao->has_object) ERROR2(, "Vertex array object name has no object");
    const trc_gl_buffer_rev_t* buffer_rev = get_buffer(buffer);
    if (buffer && !buffer_rev) ERROR2(, "Invalid buffer name");
    if (buffer_rev && !buffer_rev->has_object) ERROR2(, "Buffer name has no object");
    
    if (gls_get_ver()>=430 && index>=gls_get_state_int(GL_MAX_VERTEX_ATTRIB_BINDINGS, 0))
        ERROR2(, "Index is greater than GL_MAX_VERTEX_ATTRIB_BINDINGS");
    if (offset < 0) ERROR2(, "Invalid offset");
    if (stride < 0) ERROR2(, "Invalid stride");
    if (gls_get_ver()>=440 && stride>gls_get_state_int(GL_MAX_VERTEX_ATTRIB_STRIDE, 0))
        ERROR2(, "Stride is greater than GL_MAX_VERTEX_ATTRIB_STRIDE");
    
    trc_gl_vao_rev_t rev = *vao;
    
    trc_gl_vao_buffer_t* buffers = map_bindings(&rev);
    buffers[index].offset = offset;
    buffers[index].stride = stride;
    trc_set_obj_ref(&buffers[index].buffer, buffer_rev?buffer_rev->head.obj:NULL);
    trc_unmap_data(buffers);
    
    if (buffer_rev && !buffer_rev->has_object) {
        trc_gl_buffer_rev_t newrev = *buffer_rev;
        newrev.has_object = true;
        set_buffer(&newrev);
    }
}

static void bind_vertex_buffers(bool dsa, const trc_gl_vao_rev_t* vao, GLuint first, GLsizei count, const GLuint* buffers,
                                const trc_gl_buffer_rev_t** buffer_revs, const GLintptr* offsets, const GLsizei* strides) {
    if (!vao)
        ERROR2(, dsa?"Invalid vertex array object name":"No vertex array object is bound");
    if (!vao->has_object) ERROR2(, "Vertex array object name has no object");
    
    if (first+count>=gls_get_state_int(GL_MAX_VERTEX_ATTRIB_BINDINGS, 0) || count<0)
        ERROR2(, "Invalid range");
    
    for (GLsizei i = 0; i < count; i++) {
        if (offsets[i] < 0) ERROR2(, "Invalid offset at index %d", i);
        if (strides[i] < 0) ERROR2(, "Invalid stride at index %d", i);
        if (strides[i]>gls_get_state_int(GL_MAX_VERTEX_ATTRIB_STRIDE, 0))
            ERROR2(, "Stride is greater than GL_MAX_VERTEX_ATTRIB_STRIDE at index %d", i);
        if (buffers[i] && !buffer_revs[i])
            ERROR2(, "Invalid buffer name at index %d", i);
        if (buffers[i] && !buffer_revs[i]->has_object)
            ERROR2(, "Buffer name has no object at index %d", i);
    }
    
    trc_gl_vao_rev_t rev = *vao;
    
    trc_gl_vao_buffer_t* bindings = map_bindings(&rev);
    for (GLsizei i = 0; i < count; i++) {
        bindings[first+i].offset = offsets[i];
        bindings[first+i].stride = strides[i];
        const trc_gl_buffer_rev_t* buffer_rev = buffer_revs[i];
        if (buffer_rev && !buffer_rev->has_object) {
            trc_gl_buffer_rev_t newrev = *buffer_rev;
            newrev.has_object = true;
            buffer_rev = set_buffer(&newrev);
        }
        trc_set_obj_ref(&bindings[first+i].buffer, buffer_rev?buffer_rev->head.obj:NULL);
    }
    trc_unmap_data(bindings);
}

static void set_attrib_enabled(trc_obj_t* vao, GLuint index, bool enabled) {
    if (index >= gls_get_state_int(GL_MAX_VERTEX_ATTRIBS, 0))
        ERROR2(, "Index is greater than GL_MAX_VERTEX_ATTRIBS");
    
    trc_gl_vao_rev_t rev = *(const trc_gl_vao_rev_t*)trc_obj_get_rev(vao, -1);
    trc_gl_vao_attrib_t* attribs = map_attribs(&rev);
    attribs[index].enabled = enabled;
    trc_unmap_data(attribs);
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
        if (vao && vao==gls_get_bound_vao())
            gls_set_bound_vao(0);
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
    gls_set_bound_vao(rev?rev->head.obj:NULL);

glVertexAttribPointer: //GLuint p_index, GLint p_size, GLenum p_type, GLboolean p_normalized, GLsizei p_stride, const void* p_pointer
    //if (p_pointer > UINTPTR_MAX) //TODO
    vertex_attrib_ptr(p_index, p_normalized, false, p_size, p_type, p_pointer, p_stride);

glVertexAttribIPointer: //GLuint p_index, GLint p_size, GLenum p_type, GLsizei p_stride, const void* p_pointer
    //if (p_pointer > UINTPTR_MAX) //TODO
    vertex_attrib_ptr(p_index, false, true, p_size, p_type, p_pointer, p_stride);

glVertexAttribDivisor: //GLuint p_index, GLuint p_divisor
    if (!gls_get_bound_vao())
        ERROR("No vertex array object is bound");
    if (p_index >= gls_get_state_int(GL_MAX_VERTEX_ATTRIBS, 0))
        ERROR("Index is greater than GL_MAX_VERTEX_ATTRIBS");
    if (gls_get_ver()>=430 && p_index>=gls_get_state_int(GL_MAX_VERTEX_ATTRIB_BINDINGS, 0))
        ERROR("Index is greater than GL_MAX_VERTEX_ATTRIB_BINDINGS");
    
    trc_gl_vao_rev_t rev = *(const trc_gl_vao_rev_t*)trc_obj_get_rev(gls_get_bound_vao(), -1);
    trc_gl_vao_buffer_t* buffers = map_bindings(&rev);
    buffers[p_index].divisor = p_divisor;
    trc_unmap_data(buffers);

glVertexBindingDivisor: //GLuint p_bindingindex, GLuint p_divisor
    if (!gls_get_bound_vao())
        ERROR("No vertex array object is bound");
    if (p_bindingindex>=gls_get_state_int(GL_MAX_VERTEX_ATTRIB_BINDINGS, 0))
        ERROR("Index is greater than GL_MAX_VERTEX_ATTRIB_BINDINGS");
    
    trc_gl_vao_rev_t rev = *(const trc_gl_vao_rev_t*)trc_obj_get_rev(gls_get_bound_vao(), -1);
    trc_gl_vao_buffer_t* buffers = map_bindings(&rev);
    buffers[p_bindingindex].divisor = p_divisor;
    trc_unmap_data(buffers);

glVertexArrayBindingDivisor: //GLuint p_vaobj, GLuint p_bindingindex, GLuint p_divisor
    if (!p_vaobj_rev) ERROR("Invalid vertex array object name");
    if (!p_vaobj_rev->has_object) ERROR("Vertex array object name has no object");
    if (p_bindingindex>=gls_get_state_int(GL_MAX_VERTEX_ATTRIB_BINDINGS, 0))
        ERROR("Index is greater than GL_MAX_VERTEX_ATTRIB_BINDINGS");
    
    trc_gl_vao_rev_t rev = *p_vaobj_rev;
    trc_gl_vao_buffer_t* buffers = map_bindings(&rev);
    buffers[p_bindingindex].divisor = p_divisor;
    trc_unmap_data(buffers);

glVertexArrayAttribBinding: //GLuint p_vaobj, GLuint p_attribindex GLuint p_bindingindex
    if (!p_vaobj_rev) ERROR("Invalid vertex array object name");
    if (!p_vaobj_rev->has_object) ERROR("Vertex array object name has no object");
    if (p_attribindex >= gls_get_state_int(GL_MAX_VERTEX_ATTRIBS, 0))
        ERROR("Attribute index is greater than GL_MAX_VERTEX_ATTRIBS");
    if (p_bindingindex >= gls_get_state_int(GL_MAX_VERTEX_ATTRIB_BINDINGS, 0))
        ERROR("Binding index is greater than GL_MAX_VERTEX_ATTRIB_BINDINGS");
    
    trc_gl_vao_rev_t rev = *p_vaobj_rev;
    trc_gl_vao_attrib_t* attribs = map_attribs(&rev);
    attribs[p_attribindex].buffer_index = p_bindingindex;
    trc_unmap_data(attribs);

glVertexAttribBinding: //GLuint p_attribindex GLuint p_bindingindex
    if (!gls_get_bound_vao()) ERROR("No vertex array object is bound");
    if (p_attribindex >= gls_get_state_int(GL_MAX_VERTEX_ATTRIBS, 0))
        ERROR("Attribute index is greater than GL_MAX_VERTEX_ATTRIBS");
    if (p_bindingindex >= gls_get_state_int(GL_MAX_VERTEX_ATTRIB_BINDINGS, 0))
        ERROR("Binding index is greater than GL_MAX_VERTEX_ATTRIB_BINDINGS");
    
    trc_gl_vao_rev_t rev = *(const trc_gl_vao_rev_t*)trc_obj_get_rev(gls_get_bound_vao(), -1);
    trc_gl_vao_attrib_t* attribs = map_attribs(&rev);
    attribs[p_attribindex].buffer_index = p_bindingindex;
    trc_unmap_data(attribs);

glVertexArrayAttribFormat: //GLuint p_vaobj, GLuint p_attribindex, GLint p_size, GLenum p_type, GLboolean p_normalized, GLuint p_relativeoffset
    if (!p_vaobj_rev) ERROR("Invalid vertex array object name");
    if (!p_vaobj_rev->has_object) ERROR("Vertex array object name has no object");
    vertex_attrib_format(p_vaobj_rev->head.obj, p_attribindex, p_normalized, false, p_size, p_type, p_relativeoffset, -1);

glVertexArrayAttribIFormat: //GLuint p_vaobj, GLuint p_attribindex, GLint p_size, GLenum p_type, GLuint p_relativeoffset
    if (!p_vaobj_rev) ERROR("Invalid vertex array object name");
    if (!p_vaobj_rev->has_object) ERROR("Vertex array object name has no object");
    vertex_attrib_format(p_vaobj_rev->head.obj, p_attribindex, false, true, p_size, p_type, p_relativeoffset, -1);

glVertexAttribFormat: //GLuint p_attribindex, GLint p_size, GLenum p_type, GLboolean p_normalized, GLuint p_relativeoffset
    if (!gls_get_bound_vao()) ERROR2(, "No vertex array object is bound");
    vertex_attrib_format(gls_get_bound_vao(), p_attribindex, p_normalized, false, p_size, p_type, p_relativeoffset, -1);

glVertexAttribIFormat: //GLuint p_vaobj, GLuint p_attribindex, GLint p_size, GLenum p_type, GLuint p_relativeoffset
    if (!gls_get_bound_vao()) ERROR2(, "No vertex array object is bound");
    vertex_attrib_format(gls_get_bound_vao(), p_attribindex, false, true, p_size, p_type, p_relativeoffset, -1);

glEnableVertexAttribArray: //GLuint p_index
    if (!gls_get_bound_vao())
        ERROR("No vertex array object is bound");
    set_attrib_enabled(gls_get_bound_vao(), p_index, true);

glDisableVertexAttribArray: //GLuint p_index
    if (!gls_get_bound_vao())
        ERROR("No vertex array object is bound");
    set_attrib_enabled(gls_get_bound_vao(), p_index, true);

glEnableVertexArrayAttrib: //GLuint p_vaobj, GLuint p_index
    if (!p_vaobj_rev) ERROR("Invalid vertex array object name");
    if (!p_vaobj_rev->has_object) ERROR("Vertex array object name has no object");
    set_attrib_enabled(p_vaobj_rev->head.obj, p_index, true);

glDisableVertexArrayAttrib: //GLuint p_vaobj, GLuint p_index
    if (!p_vaobj_rev) ERROR("Invalid vertex array object name");
    if (!p_vaobj_rev->has_object) ERROR("Vertex array object name has no object");
    set_attrib_enabled(p_vaobj_rev->head.obj, p_index, true);

glBindVertexBuffer: //GLuint p_bindingindex, GLuint p_buffer, GLintptr p_offset, GLintptr p_stride
    const trc_gl_vao_rev_t* vao = (const trc_gl_vao_rev_t*)trc_obj_get_rev(gls_get_bound_vao(), -1);
    bind_vertex_buffer(false, vao, p_bindingindex, p_buffer, p_offset, p_stride);

glVertexArrayVertexBuffer: //GLuint vaobj, GLuint bindingindex, GLuint buffer, GLintptr offset, GLsizei stride
    bind_vertex_buffer(true, p_vaobj_rev, p_bindingindex, p_buffer, p_offset, p_stride);

glBindVertexBuffers: //GLuint first, GLsizei count, const GLuint* buffers, const GLintptr* offsets, const GLsizei* strides
    const trc_gl_vao_rev_t* vao = (const trc_gl_vao_rev_t*)trc_obj_get_rev(gls_get_bound_vao(), -1);
    bind_vertex_buffers(false, vao, p_first, p_count, p_buffers, p_buffers_rev, p_offsets, p_strides);

glVertexArrayVertexBuffers: //GLuint vaobj, GLuint first, GLsizei count, const GLuint* buffers, const GLintptr* offsets, const GLsizei* strides
    bind_vertex_buffers(true, p_vaobj_rev, p_first, p_count, p_buffers, p_buffers_rev, p_offsets, p_strides);

glVertexArrayElementBuffer: //GLuint p_vaobj, GLuint p_buffer
    if (!p_vaobj_rev) ERROR("Invalid vertex array object name");
    if (!p_vaobj_rev->has_object) ERROR("Vertex array object name has no object");
    
    if (p_buffer && !p_buffer_rev) ERROR("Invalid buffer name");
    if (p_buffer && !p_buffer_rev->has_object) ERROR("Buffer name has no object");
    
    trc_gl_vao_rev_t rev = *p_vaobj_rev;
    trc_set_obj_ref(&rev.element_buffer, p_buffer_rev?p_buffer_rev->head.obj:NULL);
    set_vao(&rev);

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

glGetVertexArrayIndexediv: //GLuint p_vaobj, GLuint p_index, GLenum p_panem, GLint* p_param
    if (!p_vaobj_rev) ERROR("Invalid vertex array object name");
    if (!p_vaobj_rev->has_object) ERROR("Vertex array object name has no object");
    if (p_index >= gls_get_state_int(GL_MAX_VERTEX_ATTRIBS, 0))
        ERROR("Index is greater than GL_MAX_VERTEX_ATTRIBS");

glGetVertexArrayIndexed64iv: //GLuint p_vaobj, GLuint p_index, GLenum p_panem, GLint64* p_param
    if (!p_vaobj_rev) ERROR("Invalid vertex array object name");
    if (!p_vaobj_rev->has_object) ERROR("Vertex array object name has no object");
    if (p_index >= gls_get_state_int(GL_MAX_VERTEX_ATTRIBS, 0))
        ERROR("Index is greater than GL_MAX_VERTEX_ATTRIBS");

glGetVertexArrayiv: //GLuint p_vaobj, GLenum p_panem, GLint* p_param
    if (!p_vaobj_rev) ERROR("Invalid vertex array object name");
    if (!p_vaobj_rev->has_object) ERROR("Vertex array object name has no object");

glIsVertexArray: //GLuint p_array
    ;
