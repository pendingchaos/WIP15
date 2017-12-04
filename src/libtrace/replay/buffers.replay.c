#include "libtrace/replay/utils.h"

static void gen_buffers(size_t count, const GLuint* real, const GLuint* fake, bool create) {
    trc_gl_buffer_rev_t rev;
    rev.has_object = create;
    rev.tf_binding_count = 0;
    rev.data_usage = GL_STATIC_DRAW;
    rev.data = trc_create_chunked_data(ctx->trace, 0, NULL);
    rev.mapped = false;
    rev.map_offset = 0;
    rev.map_length = 0;
    rev.map_access = 0;
    for (size_t i = 0; i < count; ++i) {
        rev.real = real[i];
        trc_create_named_obj(ctx->ns, TrcBuffer, fake[i], &rev);
    }
}

trc_obj_t* get_bound_buffer(GLenum target) {
    if (target == GL_ELEMENT_ARRAY_BUFFER) {
        trc_obj_t* vao = gls_get_bound_vao();
        if (vao) {
            const trc_gl_vao_rev_t* rev = trc_obj_get_rev(vao, -1);
            return rev->element_buffer.obj;
        }
    }
    return gls_get_bound_buffer(target);
}

void update_buffer_from_gl(trc_obj_t* obj, size_t offset, ptrdiff_t size_) {
    trc_gl_buffer_rev_t rev = *(trc_gl_buffer_rev_t*)trc_obj_get_rev(obj, -1);
    size_t size = size_ <= 0 ? rev.data.size : size_;
    
    GLint prev_array_buffer;
    F(glGetIntegerv)(GL_ARRAY_BUFFER_BINDING, &prev_array_buffer);
    F(glBindBuffer)(GL_ARRAY_BUFFER, rev.real);
    
    void* newdata = malloc(size);
    F(glGetBufferSubData)(GL_ARRAY_BUFFER, offset, size, newdata);
    
    trc_chunked_data_mod_t mod = {.next=NULL, .start=offset, .size=size, .data=newdata};
    trc_modify_chunked_data_t info = {.base=rev.data, .mods=&mod};
    rev.data = trc_modify_chunked_data(ctx->trace, info);
    
    free(newdata);
    
    set_buffer(&rev);
    
    F(glBindBuffer)(GL_ARRAY_BUFFER, prev_array_buffer);
}

static bool buffer_data_prologue(bool dsa, trc_obj_t* buf, GLsizeiptr size) {
    if (size < 0) ERROR2(false, "Invalid size");
    const trc_gl_buffer_rev_t* rev = trc_obj_get_rev(buf, -1);
    if (!rev) ERROR2(false, dsa?"Invalid buffer name":"No buffer bound to target");
    if (!rev->has_object) ERROR2(false, "Buffer name has no object");
    if (get_current_tf()->active_not_paused && rev->tf_binding_count)
        trc_add_warning(cmd, "Buffer should not be modified as it is a transform feedback one while transform feedback is active and unpaused");
    return true;
}

static void buffer_data_epilogue(bool dsa, GLenum target, trc_obj_t* buf, GLsizeiptr size, const void* data) {
    trc_gl_buffer_rev_t newrev = *(const trc_gl_buffer_rev_t*)trc_obj_get_rev(buf, -1);
    if (data) {
        newrev.data = trc_create_chunked_data(ctx->trace, size, data);
    } else {
        void* newdata = malloc(size);
        if (!dsa) F(glGetBufferSubData)(target, 0, size, newdata);
        else F(glGetNamedBufferSubData)(newrev.real, 0, size, newdata);
        newrev.data = trc_create_chunked_data(ctx->trace, size, newdata);
        free(newdata);
    }
    set_buffer(&newrev);
}

static bool buffer_sub_data(bool dsa, trc_obj_t* buf, GLintptr offset, GLsizeiptr size, const void* data) {
    if (offset<0) ERROR2(false, "Invalid offset");
    if (size<0) ERROR2(false, "Invalid size");
    const trc_gl_buffer_rev_t* rev = trc_obj_get_rev(buf, -1);
    if (!rev) ERROR2(false, dsa?"Invalid buffer name":"No buffer bound to target");
    if (!rev->has_object) ERROR2(false, "Buffer name does not have an object");
    if (get_current_tf()->active_not_paused && rev->tf_binding_count)
        trc_add_warning(cmd, "Buffer should not be modified as it is a transform feedback one while transform feedback is active and unpaused");
    if (offset+size > rev->data.size) ERROR2(false, "Invalid range");
    if (rev->mapped && !(rev->map_access&GL_MAP_PERSISTENT_BIT))
        ERROR2(false, "Buffer is mapped without persistent access");
    
    trc_gl_buffer_rev_t newrev = *rev;
    
    trc_chunked_data_mod_t mod = {.next=NULL, .start=offset, .size=size, .data=data};
    trc_modify_chunked_data_t info = {.base=newrev.data, .mods=&mod};
    newrev.data = trc_modify_chunked_data(ctx->trace, info);
    
    set_buffer(&newrev);
    return true;
}

static bool copy_buffer_data(bool dsa, trc_obj_t* read, trc_obj_t* write, GLintptr read_off, GLintptr write_off, GLsizeiptr size) {
    if (read_off<0 || write_off<0 || size<0) ERROR2(false, "The read offset, write offset or size is negative");
    
    const trc_gl_buffer_rev_t* read_rev = trc_obj_get_rev(read, -1);
    if (!read_rev) ERROR2(false, dsa?"Invalid read buffer name":"No buffer bound to read target");
    if (!read_rev->has_object) ERROR2(false, "Read buffer name has no object");
    if (get_current_tf()->active_not_paused && read_rev->tf_binding_count)
        trc_add_warning(cmd, "Read buffer should not be read as it is a transform feedback one while transform feedback is active and unpaused");
    if (read_off+size > read_rev->data.size) ERROR2(false, "Invalid size and read offset");
    
    const trc_gl_buffer_rev_t* write_rev = trc_obj_get_rev(write, -1);
    if (!write_rev) ERROR2(false, dsa?"Invalid write buffer name":"No buffer bound to write target");
    if (!write_rev->has_object) ERROR2(false, "Write buffer name has no object");
    if (get_current_tf()->active_not_paused && write_rev->tf_binding_count)
        trc_add_warning(cmd, "Write buffer should not be modified as it is a transform feedback one while transform feedback is active and unpaused");
    if (write_off+size > write_rev->data.size) ERROR2(false, "Invalid size and write offset");
    
    trc_gl_buffer_rev_t res = *write_rev;
    
    //TODO: It should be possible to share chunks here
    void* buf = malloc(size);
    trc_read_chunked_data_t rinfo = {.data=read_rev->data, .start=read_off, .size=size, .dest=buf};
    trc_read_chunked_data(rinfo);
    trc_chunked_data_mod_t mod = {.next=NULL, .start=write_off, .size=size, .data=buf};
    trc_modify_chunked_data_t minfo = {.base=res.data, .mods=&mod};
    res.data = trc_modify_chunked_data(ctx->trace, minfo);
    free(buf);
    
    set_buffer(&res);
    return true;
}

static void map_buffer(bool dsa, GLuint fake, GLenum access) {
    const trc_gl_buffer_rev_t* rev = get_buffer(fake);
    if (!rev) ERROR2(, dsa?"Invalid buffer name":"No buffer bound to target");
    if (!rev->has_object) ERROR2(, "Buffer name has no object");
    if (get_current_tf()->active_not_paused && rev->tf_binding_count)
        trc_add_warning(cmd, "Buffer should not be mapped as it is a transform feedback one while transform feedback is active and unpaused");
    if (rev->mapped) ERROR2(, "Buffer is already mapped");
    
    trc_gl_buffer_rev_t newrev = *rev;
    newrev.mapped = true;
    newrev.map_offset = 0;
    newrev.map_length = rev->data.size;
    switch (access) {
    case GL_READ_ONLY: newrev.map_access = GL_MAP_READ_BIT; break;
    case GL_WRITE_ONLY: newrev.map_access = GL_MAP_WRITE_BIT; break;
    case GL_READ_WRITE: newrev.map_access = GL_MAP_READ_BIT | GL_MAP_WRITE_BIT; break;
    }
    
    set_buffer(&newrev);
}

static void map_buffer_range(bool dsa, GLuint fake, GLintptr offset, GLsizeiptr length, GLbitfield access) {
    if (offset < 0) ERROR2(, "Invalid offset");
    if (length <= 0) ERROR2(, "Invalid length");
    
    if (!(access&GL_MAP_READ_BIT) && !(access&GL_MAP_WRITE_BIT))
        ERROR2(, "Neither GL_MAP_READ_BIT or GL_MAP_WRITE_BIT is set");
    
    if (access&GL_MAP_READ_BIT && (access&GL_MAP_INVALIDATE_RANGE_BIT ||
                                   access&GL_MAP_INVALIDATE_BUFFER_BIT ||
                                   access&GL_MAP_UNSYNCHRONIZED_BIT))
        ERROR2(, "GL_MAP_READ_BIT is set and GL_MAP_INVALIDATE_RANGE_BIT, GL_MAP_INVALIDATE_BUFFER_BIT and/or GL_MAP_UNSYNCHRONIZED_BIT is set");
    
    if (access&GL_MAP_FLUSH_EXPLICIT_BIT && !(access&GL_MAP_WRITE_BIT))
        ERROR2(, "GL_MAP_FLUSH_EXPLICIT_BIT is set but GL_MAP_WRITE_BIT is not");
        
    //TODO:
    //Make sure the access is valid with the buffer's storage flags
    
    const trc_gl_buffer_rev_t* rev = get_buffer(fake);
    if (!rev) ERROR2(, dsa?"Invalid buffer name":"No buffer bound to target");
    if (get_current_tf()->active_not_paused && rev->tf_binding_count)
        trc_add_warning(cmd, "Buffer should not be mapped as it is a transform feedback one while transform feedback is active and unpaused");
    trc_gl_buffer_rev_t newrev = *rev;
    
    if (offset+length > newrev.data.size)
        ERROR2(, "Invalid range");
    if (newrev.mapped) ERROR2(, "Buffer is already mapped");
    
    newrev.mapped = true;
    newrev.map_offset = offset;
    newrev.map_length = length;
    newrev.map_access = access;
    
    set_buffer(&newrev);
}

static void unmap_buffer(bool dsa, GLuint target_or_buf) {
    const trc_gl_buffer_rev_t* rev;
    if (dsa) rev = get_buffer(target_or_buf);
    else rev = trc_obj_get_rev(get_bound_buffer(target_or_buf), -1);
    if (!rev) ERROR2(, dsa?"Invalid buffer name":"No buffer bound to target");
    if (!rev->mapped) ERROR2(, "Buffer is not mapped");
    
    trc_gl_buffer_rev_t newrev = *rev;
    
    if (rev->map_access & GL_MAP_WRITE_BIT) {
        trace_extra_t* extra = trc_get_extra(cmd, "replay/glUnmapBuffer/data_ranged");
        if (!extra) {
            trc_add_error(cmd, "replay/glUnmapBuffer/data_ranged not found");
            goto end;
        }
        
        if (extra->size < 8) {
            trc_add_error(cmd, "Invalid trace");
            goto end;
        }
        
        uint64_t offset = le64toh(*(const uint64_t*)extra->data);
        uint64_t length = extra->size - 8;
        const void* data = (const uint8_t*)extra->data + 8;
        
        if (extra->size != length+8) {
            trc_add_error(cmd, "Invalid trace");
            goto end;
        }
        
        if (dsa) //Assume glNamedBufferSubData is supported if glUnmapNamedBuffer is being called
            F(glNamedBufferSubData)(rev->real, offset, length, data);
        else
            F(glBufferSubData)(target_or_buf, offset, length, data);
        
        trc_chunked_data_mod_t mod = {.next=NULL, .start=offset, .size=length, .data=data};
        trc_modify_chunked_data_t info = {.base=newrev.data, .mods=&mod};
        newrev.data = trc_modify_chunked_data(ctx->trace, info);
    }
    
    end:
    newrev.mapped = false;
    newrev.map_offset = 0;
    newrev.map_length = 0;
    newrev.map_access = 0;
    
    set_buffer(&newrev);
}

static bool flush_mapped_buffer_range(bool dsa, GLenum target_or_buf, GLintptr offset, GLsizeiptr length) {
    const trc_gl_buffer_rev_t* rev;
    if (dsa) rev = get_buffer(target_or_buf);
    else rev = trc_obj_get_rev(get_bound_buffer(target_or_buf), -1);
    if (!rev) ERROR2(false, "No buffer bound to target");
    if (!rev->has_object) ERROR2(false, "Buffer name has no object");
    if (!rev->mapped) ERROR2(false, "Buffer object is not mapped");
    if (!(rev->map_access&GL_MAP_FLUSH_EXPLICIT_BIT))
        ERROR2(false, "Buffer object is mapped without GL_MAP_FLUSH_EXPLICIT_BIT");
    if (offset<0 || length<0 || offset+length>rev->map_length)
        ERROR2(false, "Invalid bounds");
    //Currently does nothing but validate - this is only useful for persistently mapped buffers
    //but those are not supported at the moment
    return true;
}

static void get_buffer_sub_data(bool dsa, trc_obj_t* obj, GLintptr offset, GLsizeiptr size) {
    const trc_gl_buffer_rev_t* rev = trc_obj_get_rev(obj, -1);
    if (!rev) ERROR2(, dsa?"Invalid buffer name":"No buffer bound to target");
    if (!rev->has_object) ERROR2(, "Buffer name has no object");
    if (rev->mapped && !(rev->map_access&GL_MAP_PERSISTENT_BIT)) ERROR2(, "Buffer is mapped without GL_MAP_PERSISTENT_BIT");
    if (offset<0 || size<0 || offset+size>rev->data.size) ERROR2(, "Invalid offset and/or size");
}

static const trc_gl_buffer_rev_t* on_change_tf_binding(trc_obj_t* prev_buf, trc_obj_t* new_buf) {
    if (prev_buf) {
        trc_gl_buffer_rev_t newrev = *(const trc_gl_buffer_rev_t*)trc_obj_get_rev(prev_buf, -1);
        newrev.tf_binding_count--;
        trc_obj_set_rev(prev_buf, &newrev);
    }
    if (new_buf) {
        trc_gl_buffer_rev_t newrev = *(const trc_gl_buffer_rev_t*)trc_obj_get_rev(new_buf, -1);
        newrev.tf_binding_count++;
        trc_obj_set_rev(new_buf, &newrev);
    }
    return trc_obj_get_rev(new_buf, -1);
}

static void bind_buffer(GLenum target, GLuint buffer) {
    const trc_gl_buffer_rev_t* rev = get_buffer(buffer);
    if (rev && !rev->has_object) {
        trc_gl_buffer_rev_t newrev = *rev;
        newrev.has_object = true;
        set_buffer(&newrev);
    }
    if (target == GL_ELEMENT_ARRAY_BUFFER) {
        trc_obj_t* vao = gls_get_bound_vao();
        if (vao) {
            trc_gl_vao_rev_t vao_rev = *(const trc_gl_vao_rev_t*)trc_obj_get_rev(vao, -1);
            trc_set_obj_ref(&vao_rev.element_buffer, rev?rev->head.obj:NULL);
            set_vao(&vao_rev);
        } else {
            gls_set_bound_buffer(target, rev?rev->head.obj:NULL);
        }
    } else {
        gls_set_bound_buffer(target, rev?rev->head.obj:NULL);
    }
}

static void bind_buffer_indexed_ranged(GLenum target, GLuint index, GLuint buffer, uint64_t offset, uint64_t size) {
    const trc_gl_buffer_rev_t* rev = get_buffer(buffer);
    if (target == GL_TRANSFORM_FEEDBACK_BUFFER)
        rev = on_change_tf_binding(gls_get_bound_buffer_indexed(target, index).buf.obj, rev?rev->head.obj:NULL);
    if (rev && !rev->has_object) {
        trc_gl_buffer_rev_t newrev = *rev;
        newrev.has_object = true;
        set_buffer(&newrev);
    }
    trc_gl_buffer_binding_point_t point;
    point.buf.obj = rev ? rev->head.obj : NULL;
    point.offset = offset;
    point.size = size;
    gls_set_bound_buffer_indexed(target, index, point);
    if (target == GL_TRANSFORM_FEEDBACK_BUFFER) {
        trc_gl_context_rev_t ctx_rev = *trc_get_context(ctx->trace);
        trc_gl_transform_feedback_rev_t tf_rev = *get_current_tf();
        tf_rev.bindings = ctx_rev.bound_buffer_indexed_GL_TRANSFORM_FEEDBACK_BUFFER;
        set_transform_feedback(&tf_rev);
    }
}

static void bind_buffer_indexed(GLenum target, GLuint index, GLuint buffer) {
    bind_buffer_indexed_ranged(target, index, buffer, 0, 0);
}

glGenBuffers: //GLsizei p_n, GLuint* p_buffers
    if (p_n < 0) ERROR("Invalid buffer name count");
    GLuint* buffers = replay_alloc(p_n*sizeof(GLuint));
    real(p_n, buffers);
    gen_buffers(p_n, buffers, p_buffers, false);

glCreateBuffers: //GLsizei p_n, GLuint* p_buffers
    if (p_n < 0) ERROR("Invalid buffer name count");
    GLuint* buffers = replay_alloc(p_n*sizeof(GLuint));
    real(p_n, buffers);
    gen_buffers(p_n, buffers, p_buffers, true);

glDeleteBuffers: //GLsizei p_n, const GLuint* p_buffers
    GLuint* buffers = replay_alloc(p_n*sizeof(GLuint));
    for (size_t i = 0; i < p_n; ++i) {
        if (!(buffers[i] = get_real_buffer(p_buffers[i])) && p_buffers[i])
            trc_add_warning(cmd, "Invalid buffer name");
        else {
            trc_obj_t* obj = get_buffer(p_buffers[i])->head.obj;
            
            //Reset targets
            GLenum targets[13] = {
                GL_ARRAY_BUFFER, GL_ATOMIC_COUNTER_BUFFER, GL_COPY_READ_BUFFER,
                GL_COPY_WRITE_BUFFER, GL_DISPATCH_INDIRECT_BUFFER, GL_DRAW_INDIRECT_BUFFER,
                GL_PIXEL_PACK_BUFFER, GL_PIXEL_UNPACK_BUFFER, GL_QUERY_BUFFER,
                GL_SHADER_STORAGE_BUFFER, GL_TEXTURE_BUFFER, GL_TRANSFORM_FEEDBACK_BUFFER,
                GL_UNIFORM_BUFFER};
            for (size_t j = 0; j < 13; j++) {
                if (gls_get_bound_buffer(targets[j]) == obj)
                    bind_buffer(targets[j], 0);
            }
            
            GLenum indexed_targets[4] = {
                GL_TRANSFORM_FEEDBACK_BUFFER, GL_UNIFORM_BUFFER,
                GL_SHADER_STORAGE_BUFFER, GL_ATOMIC_COUNTER_BUFFER};
            for (size_t j = 0; j < 4; j++) {
                size_t count = gls_get_bound_buffer_indexed_size(indexed_targets[j]);
                for (size_t k = 0; k < count; k++) {
                    if (gls_get_bound_buffer_indexed(indexed_targets[j], k).buf.obj == obj)
                        bind_buffer_indexed(indexed_targets[j], k, 0);
                }
            }
            
            delete_obj(p_buffers[i], TrcBuffer);
        }
    }
    real(p_n, buffers);

glBindBuffer: //GLenum p_target, GLuint p_buffer
    const trc_gl_buffer_rev_t* rev = get_buffer(p_buffer);
    if (!rev && p_buffer) ERROR("Invalid buffer name");
    real(p_target, rev?rev->real:0);
    bind_buffer(p_target, p_buffer);

glBindBufferBase: //GLenum p_target, GLuint p_index, GLuint p_buffer
    if (get_current_tf()->active_not_paused && p_target==GL_TRANSFORM_FEEDBACK_BUFFER)
        ERROR("Cannot modify GL_TRANSFORM_FEEDBACK_BUFFER when transform feedback is active and not paused");
    if (p_index >= gls_get_bound_buffer_indexed_size(p_target))
        ERROR("Invalid index");
    const trc_gl_buffer_rev_t* rev = get_buffer(p_buffer);
    if (!rev && p_buffer) ERROR("Invalid buffer name");
    real(p_target, p_index, p_buffer?rev->real:0);
    bind_buffer(p_target, p_buffer);
    bind_buffer_indexed(p_target, p_index, p_buffer);

glBindBufferRange: //GLenum p_target, GLuint p_index, GLuint p_buffer, GLintptr p_offset, GLsizeiptr p_size
    if (get_current_tf()->active_not_paused && p_target==GL_TRANSFORM_FEEDBACK_BUFFER)
        ERROR("Cannot modify GL_TRANSFORM_FEEDBACK_BUFFER when transform feedback is active and not paused");
    if (p_index >= gls_get_bound_buffer_indexed_size(p_target))
        ERROR("Invalid index");
    const trc_gl_buffer_rev_t* rev = get_buffer(p_buffer);
    if (!rev && p_buffer) ERROR("Invalid buffer name");
    if (rev && (p_size<=0 || p_offset+p_size>rev->data.size))
        ERROR("Invalid range");
    //TODO: Check alignment of offset
    real(p_target, p_index, p_buffer?rev->real:0, p_offset, p_size);
    
    bind_buffer(p_target, p_buffer);
    bind_buffer_indexed_ranged(p_target, p_index, p_buffer, p_offset, p_size);

glBufferData: //GLenum p_target, GLsizeiptr p_size, const void* p_data, GLenum p_usage
    void* data = cmd->args[2].type==Type_Ptr ? NULL : trc_map_data(cmd->args[2].data, TRC_MAP_READ);
    trc_obj_t* obj = get_bound_buffer(p_target);
    if (buffer_data_prologue(false, obj, p_size)) {
        real(p_target, p_size, data, p_usage);
        buffer_data_epilogue(false, p_target, obj, p_size, data);
    }
    trc_unmap_data(data);

glNamedBufferData: //GLuint p_buffer, GLsizeiptr p_size, const void* p_data, GLenum p_usage
    void* data = cmd->args[2].type==Type_Ptr ? NULL : trc_map_data(cmd->args[2].data, TRC_MAP_READ);
    trc_obj_t* obj = p_buffer_rev ? p_buffer_rev->head.obj : NULL;
    if (buffer_data_prologue(true, obj, p_size)) {
        real(p_buffer_rev->real, p_size, data, p_usage);
        buffer_data_epilogue(true, 0, obj, p_size, data);
    }
    trc_unmap_data(data);

glBufferSubData: //GLenum p_target, GLintptr p_offset, GLsizeiptr p_size, const void* p_data
    trc_obj_t* buf = get_bound_buffer(p_target);
    if (!buf) ERROR("No buffer bound to target");
    if (buffer_sub_data(false, buf, p_offset, p_size, p_data))
        real(p_target, p_offset, p_size, p_data);

glNamedBufferSubData: //GLuint p_buffer, GLintptr p_offset, GLsizeiptr p_size, const void* p_data
    if (buffer_sub_data(true, trc_lookup_name(ctx->ns, TrcBuffer, p_buffer, -1), p_offset, p_size, p_data))
        real(p_buffer_rev->real, p_offset, p_size, p_data);

glCopyBufferSubData: //GLenum p_readTarget, GLenum p_writeTarget, GLintptr p_readOffset, GLintptr p_writeOffset, GLsizeiptr p_size
    trc_obj_t* read = get_bound_buffer(p_readTarget);
    trc_obj_t* write = get_bound_buffer(p_writeTarget);
    if (copy_buffer_data(false, read, write, p_readOffset, p_writeOffset, p_size))
        real(p_readTarget, p_writeTarget, p_readOffset, p_writeOffset, p_size);

glCopyNamedBufferSubData: //GLuint p_readBuffer, GLuint p_writeBuffer, GLintptr p_readOffset, GLintptr p_writeOffset, GLsizeiptr p_size
    trc_obj_t* read = p_readBuffer_rev ? p_readBuffer_rev->head.obj : NULL;
    trc_obj_t* write = p_writeBuffer_rev ? p_writeBuffer_rev->head.obj : NULL;
    if (copy_buffer_data(true, read, write, p_readOffset, p_writeOffset, p_size))
        real(p_readBuffer_rev->real, p_writeBuffer_rev->real, p_readOffset, p_writeOffset, p_size);

glMapBuffer: //GLenum p_target, GLenum p_access
    const trc_gl_buffer_rev_t* rev = trc_obj_get_rev(get_bound_buffer(p_target), -1);
    map_buffer(false, rev?rev->head.name:0, p_access);

glMapNamedBuffer: //GLuint p_buffer, GLenum p_access
    map_buffer(true, p_buffer, p_access);

glMapBufferRange: //GLenum p_target, GLintptr p_offset, GLsizeiptr p_length, GLbitfield p_access
    const trc_gl_buffer_rev_t* rev = trc_obj_get_rev(get_bound_buffer(p_target), -1);
    map_buffer_range(false, rev?rev->head.name:0, p_offset, p_length, p_access);

glMapNamedBufferRange: //GLuint p_buffer, GLintptr p_offset, GLsizeiptr p_length, GLbitfield p_access
    map_buffer_range(true, p_buffer, p_offset, p_length, p_access);

glUnmapBuffer: //GLenum p_target
    unmap_buffer(false, p_target);

glUnmapNamedBuffer: //GLuint p_buffer
    unmap_buffer(true, p_buffer);

glFlushMappedBufferRange: //GLenum p_target, GLintptr p_offset, GLsizeiptr p_length
    flush_mapped_buffer_range(false, p_target, p_offset, p_length);

glFlushMappedNamedBufferRange: //GLuint p_buffer, GLintptr p_offset, GLsizeiptr p_length
    flush_mapped_buffer_range(true, p_buffer, p_offset, p_length);

glGetBufferParameteriv: //GLenum p_target, GLenum p_pname, GLint* p_params
    if (!get_bound_buffer(p_target)) ERROR("No buffer bound to target");

glGetBufferParameteri64v: //GLenum p_target, GLenum p_pname, GLint64* p_params
    if (!get_bound_buffer(p_target)) ERROR("No buffer bound to target");

glGetNamedBufferParameteriv: //GLuint p_buffer, GLenum p_pname, GLint* p_params
    if (!p_buffer_rev) ERROR("Invalid buffer name");
    if (!p_buffer_rev->has_object) ERROR("Buffer name has no object");

glGetNamedBufferParameteri64v: //GLuint p_buffer, GLenum p_pname, GLint64* p_params
    if (!p_buffer_rev) ERROR("Invalid buffer name");
    if (!p_buffer_rev->has_object) ERROR("Buffer name has no object");

glGetBufferPointerv: //GLenum p_target, GLenum p_pname, void ** p_params
    if (!get_bound_buffer(p_target)) ERROR("No buffer bound to target");

glGetNamedBufferPointerv: //GLuint p_buffer, GLenum p_pname, void ** p_params
    if (!p_buffer_rev) ERROR("Invalid buffer name");
    if (!p_buffer_rev->has_object) ERROR("Buffer name has no object");

glGetBufferSubData: //GLenum p_target, GLintptr p_offset, GLsizeiptr p_size, void* p_data
    get_buffer_sub_data(false, get_bound_buffer(p_target), p_offset, p_size);

glGetNamedBufferSubData: //GLuint p_buffer, GLintptr p_offset, GLsizeiptr p_size, void* p_data
    get_buffer_sub_data(true, trc_lookup_name(ctx->ns, TrcBuffer, p_buffer, -1), p_offset, p_size);

glIsBuffer: //GLuint p_buffer
    ;

