#include "libtrace/replay/utils.h"
#include "libtrace/replay/textures.h"

static bool is_tf_buffer(uint count, trc_obj_t** bufs, trc_obj_t* test) {
    for (uint i = 0; i < count; i++) if (bufs[i] == test) return true;
    return false;
}

static bool tf_draw_validation(GLenum primitive) {
    trc_gl_transform_feedback_rev_t tf = *get_current_tf();
    if (!tf.active_not_paused) return true;
    
    size_t buf_count;
    trc_obj_t** bufs = get_tf_buffer_list(&buf_count);
    
    //Find any buffers that can be accessed by the program(s) that are also transform feedback buffers
    //TODO: Find out if the binding is actually referenced in the shader
    #define T(max, type) do {int count = trc_gl_state_get_state_int(ctx->trace, max, 0);\
    for (size_t i = 0; i < count; i++) {\
        trc_obj_t* buf = trc_gl_state_get_bound_buffer_indexed(ctx->trace, type, i).buf.obj;\
        if (is_tf_buffer(buf_count, bufs, buf))\
            trc_add_warning(cmd, "A transform feedback buffer is bound to "#type"[%u]", i);\
    }} while (0)
    T(GL_MAX_UNIFORM_BUFFER_BINDINGS, GL_UNIFORM_BUFFER);
    T(GL_MAX_SHADER_STORAGE_BUFFER_BINDINGS, GL_SHADER_STORAGE_BUFFER);
    T(GL_MAX_ATOMIC_COUNTER_BUFFER_BINDINGS, GL_ATOMIC_COUNTER_BUFFER);
    #undef T
    
    //Test the primitive
    GLint test_primitive = primitive;
    trc_obj_t* geom_program = get_active_program_for_stage(GL_GEOMETRY_SHADER);
    if (geom_program) {
        const trc_gl_program_rev_t* geom_program_rev = trc_obj_get_rev(geom_program, -1);
        F(glGetProgramiv)(geom_program_rev->real, GL_GEOMETRY_OUTPUT_TYPE, &test_primitive);
    }
    
    switch (trc_gl_state_get_tf_primitive(ctx->trace)) {
    case GL_POINTS:
        if (test_primitive != GL_POINTS)
            ERROR2(false, "Primitive not compatible with transform feedback primitive");
        break;
    case GL_LINES:
        if (not_one_of(test_primitive, GL_LINES, GL_LINE_LOOP, GL_LINE_STRIP, GL_LINES_ADJACENCY, GL_LINE_STRIP_ADJACENCY, -1))
            ERROR2(false, "Primitive not compatible with transform feedback primitive");
        break;
    case GL_TRIANGLES:
        if (not_one_of(test_primitive, GL_TRIANGLES, GL_TRIANGLE_STRIP, GL_TRIANGLE_FAN, GL_TRIANGLES_ADJACENCY, GL_TRIANGLE_STRIP_ADJACENCY, -1))
            ERROR2(false, "Primitive not compatible with transform feedback primitive");
        break;
    }
    
    return true;
}

static GLenum get_sampler_target(trc_gl_uniform_dtype_t dtype) {
    switch (dtype.tex_type) {
    case TrcUniformTexType_1D:
        return dtype.tex_array ? GL_TEXTURE_1D_ARRAY : GL_TEXTURE_1D;
    case TrcUniformTexType_2D:
        if (dtype.tex_multisample)
            return dtype.tex_array ? GL_TEXTURE_2D_MULTISAMPLE_ARRAY : GL_TEXTURE_2D_MULTISAMPLE;
        else
            return dtype.tex_array ? GL_TEXTURE_2D_ARRAY : GL_TEXTURE_2D;
    case TrcUniformTexType_3D:
        return GL_TEXTURE_3D;
    case TrcUniformTexType_Cube:
        return dtype.tex_array ? GL_TEXTURE_CUBE_MAP_ARRAY : GL_TEXTURE_CUBE_MAP;
    case TrcUniformTexType_Rect:
        return GL_TEXTURE_RECTANGLE;
    case TrcUniformTexType_Buffer:
        return GL_TEXTURE_BUFFER;
    }
    assert(false);
}

//TODO: Validate images
static void validate_samplers(const trc_gl_program_rev_t* rev) {
    size_t units = trc_gl_state_get_state_int(ctx->trace, GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS, 0);
    
    size_t uniform_count = rev->uniforms->size / sizeof(trc_gl_uniform_t);
    const trc_gl_uniform_t* uniforms = trc_map_data(rev->uniforms, TRC_MAP_READ);
    uint8_t* uniform_data = trc_map_data(rev->uniform_data, TRC_MAP_READ);
    for (size_t i = 0; i < uniform_count; i++) {
        const trc_gl_uniform_t* uniform = &uniforms[i];
        if (uniform->dtype.base == TrcUniformBaseType_Sampler) {
            GLenum target = get_sampler_target(uniform->dtype);
            uint unit;
            memcpy(&unit, uniform_data+uniform->data_offset, sizeof(uint));
            
            if (unit < units) {
                trc_obj_t* tex = trc_gl_state_get_bound_textures(ctx->trace, target, unit);
                trc_obj_t* sampler = trc_gl_state_get_bound_samplers(ctx->trace, unit);
                const char* err = validate_texture_completeness(tex, sampler);
                if (err) {
                    trc_add_error(cmd, "Uniform at location %u uses an incomplete texture: %s",
                                  uniform->fake_loc, err);
                }
            } else {
                trc_add_error(cmd, "Uniform at location %u is set to an invalid texture unit",
                              uniform->fake_loc);
            }
        }
    }
    trc_unmap_data(uniform_data);
    trc_unmap_data(uniforms);
}

#define DRAW_INDIRECT (1<<0)
#define DRAW_INDEXED (1<<1)

static bool begin_draw(GLenum primitive, uint flags) {
    const trc_gl_context_rev_t* state = trc_get_context(ctx->trace);
    const trc_gl_vao_rev_t* vao = trc_obj_get_rev(state->bound_vao.obj, -1);
    if (!vao) ERROR2(false, "No vertex array object bound");
    trc_obj_t* vertex_program_obj = get_active_program_for_stage(GL_VERTEX_SHADER);
    const trc_gl_program_rev_t* vertex_program = trc_obj_get_rev(vertex_program_obj, -1);
    if (!vertex_program) ERROR2(false, "No vertex program active");
    
    //Validate samplers
    trc_obj_t* programs[5] = {vertex_program_obj, NULL, NULL, NULL, NULL};
    for (size_t i = 0; i < 4; i++) {
        GLenum stage = ((GLenum[]){GL_FRAGMENT_SHADER, GL_GEOMETRY_SHADER,
                                   GL_TESS_EVALUATION_SHADER, GL_TESS_CONTROL_SHADER})[i];
        trc_obj_t* program = get_active_program_for_stage(stage);
        //Remove duplicates
        for (size_t j = 0; j < 5; j++) if (programs[j] == program) goto found;
        programs[i] = program;
        found: ;
    }
    for (size_t i = 0; i < 5; i++) {
        if (!programs[i]) continue;
        const trc_gl_program_rev_t* rev = trc_obj_get_rev(programs[i], -1);
        if (!rev) continue;
        validate_samplers(rev);
    }
    
    if (!tf_draw_validation(primitive)) return false;
    
    if (primitive==GL_PATCHES && get_active_program_for_stage(GL_TESS_CONTROL_SHADER)==NULL)
        ERROR2(false, "No tesselation control shader is active");
    
    if (flags & DRAW_INDIRECT) {
        trc_obj_t* buf = trc_gl_state_get_bound_buffer(ctx->trace, GL_DRAW_INDIRECT_BUFFER);
        if (!buf)
            ERROR2(false, "No buffer is bound at GL_DRAW_INDIRECT_BUFFER");
        const trc_gl_buffer_rev_t* buf_rev = trc_obj_get_rev(buf, -1);
        if (buf_rev->mapped)
            ERROR2(false, "Buffer bound at GL_DRAW_INDIRECT_BUFFER is mapped");
    }
    
    if (flags & DRAW_INDEXED) {
        if (!vao->element_buffer.obj)
            ERROR2(false, "No buffer is bound at the current VAO's GL_ELEMENT_ARRAY_BUFFER");
        const trc_gl_buffer_rev_t* buf_rev = trc_obj_get_rev(vao->element_buffer.obj, -1);
        if (buf_rev->mapped)
            ERROR2(false, "Buffer bound at the current VAO's GL_ELEMENT_ARRAY_BUFFER is mapped");
    }
    
    GLint last_buf;
    F(glGetIntegerv)(GL_ARRAY_BUFFER_BINDING, &last_buf);
    
    trc_gl_vao_attrib_t* vao_attribs = trc_map_data(vao->attribs, TRC_MAP_READ);
    trc_gl_vao_buffer_t* vao_buffers = trc_map_data(vao->buffers, TRC_MAP_READ);
    
    for (size_t i = 0; i < vao->attribs->size/sizeof(trc_gl_vao_attrib_t); i++) {
        trc_gl_vao_attrib_t* a = &vao_attribs[i];
        if (!a->enabled) continue;
        const trc_gl_buffer_rev_t* buf = trc_obj_get_rev(vao_buffers[a->buffer_index].buffer.obj, -1);
        if (buf->mapped) {
            trc_unmap_data(vao_attribs);
            ERROR2(false, "Buffer for vertex attribute %zu is mapped", i);
        }
    }
    
    uint glver = trc_gl_state_get_ver(ctx->trace);
    
    size_t prog_vertex_attrib_count = vertex_program->vertex_attribs->size;
    prog_vertex_attrib_count /= sizeof(trc_gl_program_vertex_attrib_t);
    const trc_gl_program_vertex_attrib_t* prog_vertex_attribs =
        trc_map_data(vertex_program->vertex_attribs, TRC_MAP_READ);
    for (size_t i = 0; i < vao->attribs->size/sizeof(trc_gl_vao_attrib_t); i++) {
        GLint real_loc = -1;
        for (size_t j = 0; j < prog_vertex_attrib_count; j++) {
            int rel_loc = i - prog_vertex_attribs[j].fake;
            if (rel_loc>=0 && rel_loc<prog_vertex_attribs[j].locations_used) {
                real_loc = prog_vertex_attribs[j].real + rel_loc;
                break;
            }
        }
        if (real_loc < 0) continue;
        
        trc_gl_vao_attrib_t* a = &vao_attribs[i];
        if (!a->enabled) {
            F(glDisableVertexAttribArray)(real_loc);
            continue;
        }
        
        trc_gl_vao_buffer_t* b = &vao_buffers[a->buffer_index];
        
        F(glEnableVertexAttribArray)(real_loc);
        
        GLuint real_buf = ((trc_gl_buffer_rev_t*)trc_obj_get_rev(b->buffer.obj, -1))->real;
        F(glBindBuffer)(GL_ARRAY_BUFFER, real_buf);
        
        if (a->integer) {
            F(glVertexAttribIPointer)(real_loc, a->size, a->type, b->stride,
                                      (const void*)(uintptr_t)a->offset+b->offset);
        } else {
            F(glVertexAttribPointer)(real_loc, a->size, a->type, a->normalized, b->stride,
                                     (const void*)(uintptr_t)a->offset+b->offset);
        }
        
        if (glver > 330)
            F(glVertexAttribDivisor)(real_loc, b->divisor);
    }
    trc_unmap_data(vao_buffers);
    trc_unmap_data(vao_attribs);
    trc_unmap_data(prog_vertex_attribs);
    
    F(glBindBuffer)(GL_ARRAY_BUFFER, last_buf);
    
    return true;
}

static void update_drawbuffer(GLenum buffer, GLuint drawbuffer) {
    const trc_gl_context_rev_t* state = trc_get_context(ctx->trace);
    if (state->draw_framebuffer.obj == NULL) {
        switch (buffer) {
        case GL_COLOR: update_fb0_buffers(true, false, false, false); break;
        case GL_DEPTH: update_fb0_buffers(false, false, true, false); break;
        case GL_STENCIL: update_fb0_buffers(false, false, false, true); break;
        }
    } else {
        const trc_gl_framebuffer_rev_t* rev = trc_obj_get_rev(state->draw_framebuffer.obj, -1);
        uint attachment;
        if (drawbuffer>=rev->draw_buffers->size/sizeof(GLenum)) {
            attachment = GL_NONE;
        } else {
            GLenum* draw_buffers = trc_map_data(rev->draw_buffers, TRC_MAP_READ);
            attachment = draw_buffers[drawbuffer];
            trc_unmap_data(draw_buffers);
        }
        
        size_t attach_count = rev->attachments->size / sizeof(trc_gl_framebuffer_attachment_t);
        const trc_gl_framebuffer_attachment_t* attachs = trc_map_data(rev->attachments, TRC_MAP_READ);
        for (size_t i = 0; i < attach_count; i++) {
            const trc_gl_framebuffer_attachment_t* attach = &attachs[i];
            if (attach->attachment != attachment) continue;
            if (attach->has_renderbuffer) continue;
            update_tex_image(trc_obj_get_rev(attach->texture.obj, -1), attach->level, attach->face);
        }
        trc_unmap_data(attachs);
    }
}

static void update_buffers(trc_obj_t* fb, GLbitfield mask, bool use_color_writemask) {
    if (fb == 0) {
        bool color = mask & GL_COLOR_BUFFER_BIT;
        bool writemask[4];
        for (size_t j = 0; j < 4; j++)
            writemask[j] = trc_gl_state_get_state_bool(ctx->trace, GL_COLOR_WRITEMASK, j);
        if (use_color_writemask)
            color = color && (writemask[0]||writemask[1]||writemask[2]||writemask[3]);
        
        bool depth = mask & GL_DEPTH_BUFFER_BIT;
        bool stencil = mask & GL_STENCIL_BUFFER_BIT;
        update_fb0_buffers(color, false, depth, stencil);
    } else {
        const trc_gl_framebuffer_rev_t* rev = trc_obj_get_rev(fb, -1);
        
        size_t max_updates = rev->draw_buffers->size/sizeof(GLenum) + 3;
        GLenum* updates = replay_alloc(max_updates*sizeof(GLenum));
        size_t update_count = 0;
        
        if (mask & GL_COLOR_BUFFER_BIT) {
            const GLenum* draw_buffers = trc_map_data(rev->draw_buffers, TRC_MAP_READ);
            for (size_t i = 0; i < rev->draw_buffers->size/sizeof(GLenum); i++) {
                bool writemask[4];
                for (size_t j = 0; j < 4; j++)
                    writemask[j] = trc_gl_state_get_state_bool(ctx->trace, GL_COLOR_WRITEMASK, i*4+j);
                if (use_color_writemask && (writemask[0]||writemask[1]||writemask[2]||writemask[3]))
                    updates[update_count++] = draw_buffers[i];
            }
            trc_unmap_data(draw_buffers);
        }
        
        if (mask & GL_DEPTH_BUFFER_BIT)
            updates[update_count++] = GL_DEPTH_ATTACHMENT;
        if (mask & GL_STENCIL_BUFFER_BIT)
            updates[update_count++] = GL_DEPTH_ATTACHMENT;
        if ((mask&GL_DEPTH_BUFFER_BIT) && (mask&GL_STENCIL_BUFFER_BIT))
            updates[update_count++] = GL_DEPTH_STENCIL_ATTACHMENT;
        
        size_t attach_count = rev->attachments->size / sizeof(trc_gl_framebuffer_attachment_t);
        const trc_gl_framebuffer_attachment_t* attachs = trc_map_data(rev->attachments, TRC_MAP_READ);
        for (size_t i = 0; i < update_count; i++) {
            for (size_t j = 0; j < attach_count; j++) {
                const trc_gl_framebuffer_attachment_t* attach = &attachs[j];
                if (attach->attachment == updates[i]) continue;
                if (attach->has_renderbuffer) continue;
                update_tex_image(trc_obj_get_rev(attach->texture.obj, -1), attach->level, attach->face);
            }
        }
        trc_unmap_data(attachs);
    }
}

static void end_draw() {
    //Update framebuffer
    bool depth = trc_gl_state_get_state_bool(ctx->trace, GL_DEPTH_WRITEMASK, 0);
    bool stencil = trc_gl_state_get_state_int(ctx->trace, GL_STENCIL_WRITEMASK, 0) != 0;
    stencil = stencil || trc_gl_state_get_state_int(ctx->trace, GL_STENCIL_BACK_WRITEMASK, 0)!=0;
    
    GLbitfield mask = GL_COLOR_BUFFER_BIT;
    if (depth) mask |= GL_DEPTH_BUFFER_BIT;
    if (stencil) mask |= GL_STENCIL_BUFFER_BIT;
    //TODO: Only update color buffers that could have been written to using GL_COLOR_WRITEMASK
    //update_buffers(trc_gl_state_get_draw_framebuffer(ctx->trace), mask);
    
    //TODO: Update shader storage buffers, images and atomic counters
    
    //Update transform feedback buffers
    size_t xfb_buffer_count = trc_gl_state_get_bound_buffer_indexed_size(ctx->trace, GL_TRANSFORM_FEEDBACK_BUFFER);
    for (size_t i = 0; i < xfb_buffer_count; i++) {
        trc_gl_buffer_binding_point_t binding =
            trc_gl_state_get_bound_buffer_indexed(ctx->trace, GL_TRANSFORM_FEEDBACK_BUFFER, i);
        if (!binding.buf.obj) continue;
        update_buffer_from_gl(binding.buf.obj, binding.offset, binding.size);
    }
}

glBeginConditionalRender: //GLuint p_id, GLenum p_mode
    if (!p_id_rev) ERROR("Invalid query name");
    real(p_id_rev->real, p_mode);
    //TODO: More validation

glEndConditionalRender: //
    real();

glDrawArrays: //GLenum p_mode, GLint p_first, GLsizei p_count
    if (p_count<0) ERROR("Invalid count parameters");
    if (!begin_draw(p_mode, 0)) RETURN;
    real(p_mode, p_first, p_count);
    end_draw();

glDrawArraysIndirect: //GLenum p_mode, const void* p_indirect
    if (!begin_draw(p_mode, DRAW_INDIRECT)) RETURN;
    real(p_mode, (const GLvoid*)p_indirect);
    end_draw();

glDrawArraysInstanced: //GLenum p_mode, GLint p_first, GLsizei p_count, GLsizei p_instancecount
    if (p_count<0 || p_instancecount<0)
        ERROR("Invalid count or instance count parameters");
    if (!begin_draw(p_mode, 0)) RETURN;
    real(p_mode, p_first, p_count, p_instancecount);
    end_draw();

glMultiDrawArrays: //GLenum p_mode, const GLint* p_first, const GLsizei* p_count, GLsizei p_drawcount
    if (p_drawcount < 0) ERROR("Invalid draw count parameter");
    for (size_t i = 0; i < p_drawcount; i++)
        if (p_count[i]<0) ERROR("Invalid count parameter at index %zu", i);
    if (!begin_draw(p_mode, 0)) RETURN;
    real(p_mode, p_first, p_count, p_drawcount);
    end_draw();

glMultiDrawElements: //GLenum p_mode, const GLsizei* p_count, GLenum p_type, const void*const* p_indices, GLsizei p_drawcount
    if (p_drawcount < 0) ERROR("Invalid draw count parameter");
    for (size_t i = 0; i < p_drawcount; i++)
        if (p_count[i] < 0) ERROR("Invalid count parameter at index %zu", i);
    if (!begin_draw(p_mode, 0)) RETURN;
    real(p_mode, p_count, p_type, p_indices, p_drawcount);
    end_draw();

glMultiDrawElementsBaseVertex: //GLenum p_mode, const GLsizei* p_count, GLenum p_type, const void*const* p_indices, GLsizei p_drawcount, const GLint* p_basevertex
    if (p_drawcount < 0) ERROR("Invalid draw count parameter");
    for (size_t i = 0; i < p_drawcount; i++)
        if (p_count[i] < 0) ERROR("Invalid count parameter at index %zu", i);
    if (!begin_draw(p_mode, DRAW_INDEXED)) RETURN;
    real(p_mode, p_count, p_type, p_indices, p_drawcount, p_basevertex);
    end_draw();

glDrawElements: //GLenum p_mode, GLsizei p_count, GLenum p_type, const void* p_indices
    if (p_count < 0) ERROR("Invalid count parameter");
    if (!begin_draw(p_mode, DRAW_INDEXED)) RETURN;
    real(p_mode, p_count, p_type, (const void*)p_indices);
    end_draw();

glDrawElementsIndirect: //GLenum p_mode, GLenum p_type, const void* indirect
    if (!begin_draw(p_mode, DRAW_INDEXED|DRAW_INDIRECT)) RETURN;
    real(p_mode, p_type, (const void*)p_indirect);
    end_draw();

glDrawElementsBaseVertex: //GLenum p_mode, GLsizei p_count, GLenum p_type, const void* p_indices, GLint p_basevertex
    if (p_count < 0) ERROR("Invalid count parameter");
    if (!begin_draw(p_mode, false)) RETURN;
    real(p_mode, p_count, p_type, (const GLvoid*)p_indices, p_basevertex);
    end_draw();

glDrawElementsInstanced: //GLenum p_mode, GLsizei p_count, GLenum p_type, const void* p_indices, GLsizei p_instancecount
    if (p_count<0 || p_instancecount<0) ERROR("Invalid count or instance count parameters");
    if (!begin_draw(p_mode, DRAW_INDEXED)) RETURN;
    real(p_mode, p_count, p_type, (const GLvoid*)p_indices, p_instancecount);
    end_draw();

glDrawElementsInstancedBaseVertex: //GLenum p_mode, GLsizei p_count, GLenum p_type, const void* p_indices, GLsizei p_instancecount, GLint p_basevertex
    if (p_count<0 || p_instancecount<0) ERROR("Invalid count or instance count parameters");
    if (!begin_draw(p_mode, DRAW_INDEXED)) RETURN;
    real(p_mode, p_count, p_type, (const GLvoid*)p_indices, p_instancecount, p_basevertex);
    end_draw();

glDrawElementsInstancedBaseInstance: //GLenum p_mode, GLsizei p_count, GLenum p_type, const void* p_indices, GLsizei p_instancecount, GLuint p_baseinstance
    if (p_count<0 || p_instancecount<0) ERROR("Invalid count or instance count parameters");
    if (!begin_draw(p_mode, DRAW_INDEXED)) RETURN;
    real(p_mode, p_count, p_type, (const GLvoid*)p_indices, p_instancecount, p_baseinstance);
    end_draw();

glDrawElementsInstancedBaseVertexBaseInstance: //GLenum p_mode, GLsizei p_count, GLenum p_type, const void* p_indices, GLsizei p_instancecount, GLint p_basevertex, GLuint p_baseinstance
    if (p_count<0 || p_instancecount<0) ERROR("Invalid count or instance count parameters");
    if (!begin_draw(p_mode, DRAW_INDEXED)) RETURN;
    real(p_mode, p_count, p_type, (const GLvoid*)p_indices, p_instancecount, p_basevertex, p_baseinstance);
    end_draw();

glDrawArraysInstancedBaseInstance: //GLenum p_mode, GLint p_first, GLsizei p_count, GLsizei p_instancecount, GLuint p_baseinstance
    if (p_count<0 || p_instancecount<0) ERROR("Invalid count or instance count parameters");
    if (!begin_draw(p_mode, DRAW_INDEXED)) RETURN;
    real(p_mode, p_first, p_count, p_instancecount, p_baseinstance);
    end_draw();

//TODO: Check if indices are in-range
glDrawRangeElements: //GLenum p_mode, GLuint p_start, GLuint p_end, GLsizei p_count, GLenum p_type, const void* p_indices
    if (p_count<0 || p_end<p_start) ERROR("Invalid count or range parameters");
    if (!begin_draw(p_mode, DRAW_INDEXED)) RETURN;
    real(p_mode, p_start, p_end, p_count, p_type, (const GLvoid*)p_indices);
    end_draw();

//TODO: Check if indices are in-range
glDrawRangeElementsBaseVertex: //GLenum p_mode, GLuint p_start, GLuint p_end, GLsizei p_count, GLenum p_type, const void* p_indices, GLint p_basevertex
    if (p_count<0 || p_end<p_start) ERROR("Invalid count or range parameters");
    if (!begin_draw(p_mode, DRAW_INDEXED)) RETURN;
    real(p_mode, p_start, p_end, p_count, p_type, (const GLvoid*)p_indices, p_basevertex);
    end_draw();

glClear: //GLbitfield p_mask
    real(p_mask);
    update_buffers(trc_gl_state_get_draw_framebuffer(ctx->trace), p_mask, true);

#define PARTIAL_VALIDATE_CLEAR_BUFFER do {\
int max_draw_buffers = trc_gl_state_get_state_int(ctx->trace, GL_MAX_DRAW_BUFFERS, 0);\
if (p_drawbuffer<0 || (p_drawbuffer==0&&p_buffer!=GL_COLOR) || p_drawbuffer>=max_draw_buffers)\
    ERROR("Invalid buffer");\
} while (0)

//TODO: More validation
glClearBufferiv: //GLenum p_buffer, GLint p_drawbuffer, const GLint* p_value
    //if (not_one_of(p_buffer, GL_COLOR, GL_STENCIL, -1))
    //    ERROR("Buffer is not one of GL_COLOR or GL_STENCIL");
    PARTIAL_VALIDATE_CLEAR_BUFFER;
    real(p_buffer, p_drawbuffer, p_value);
    update_drawbuffer(p_buffer, p_drawbuffer);

glClearBufferuiv: //GLenum p_buffer, GLint p_drawbuffer, const GLuint* p_value
    //if (p_buffer != GL_COLOR) ERROR("Buffer is not GL_COLOR");
    PARTIAL_VALIDATE_CLEAR_BUFFER;
    real(p_buffer, p_drawbuffer, p_value);
    update_drawbuffer(p_buffer, p_drawbuffer);

glClearBufferfv: //GLenum p_buffer, GLint p_drawbuffer, const GLfloat* p_value
    //if (not_one_of(p_buffer, GL_COLOR, GL_DEPTH, -1))
    //    ERROR("Buffer is not one of GL_COLOR or GL_DEPTH");
    PARTIAL_VALIDATE_CLEAR_BUFFER;
    real(p_buffer, p_drawbuffer, p_value);
    update_drawbuffer(p_buffer, p_drawbuffer);

glClearBufferfi: //GLenum p_buffer, GLint p_drawbuffer, GLfloat p_depth, GLint p_stencil
    //if (p_buffer != GL_DEPTH_STENCIL) ERROR("Buffer is not GL_DEPTH_STENCIL");
    PARTIAL_VALIDATE_CLEAR_BUFFER;
    real(p_buffer, p_drawbuffer, p_depth, p_stencil);
    update_drawbuffer(GL_DEPTH, p_drawbuffer);
    update_drawbuffer(GL_STENCIL, p_drawbuffer);

//TODO: More validation
#define VALIDATE_BLIT_FRAMEBUFFER\
    if (((p_mask&GL_DEPTH_BUFFER_BIT)||(p_mask&GL_STENCIL_BUFFER_BIT)) && p_filter!=GL_NEAREST)\
        ERROR("Invalid filter for mask");

glBlitFramebuffer: //GLint p_srcX0, GLint p_srcY0, GLint p_srcX1, GLint p_srcY1, GLint p_dstX0, GLint p_dstY0, GLint p_dstX1, GLint p_dstY1, GLbitfield p_mask, GLenum p_filter
    real(p_srcX0, p_srcY0, p_srcX1, p_srcY1, p_dstX0, p_dstY0, p_dstX1, p_dstY1, p_mask, p_filter);
    VALIDATE_BLIT_FRAMEBUFFER
    update_buffers(trc_gl_state_get_draw_framebuffer(ctx->trace), p_mask, false); //TODO: Use GL_COLOR_WRITEMASK here?

glBlitNamedFramebuffer: //GLuint p_readFramebuffer, GLuint p_drawFramebuffer, GLint p_srcX0, GLint p_srcY0, GLint p_srcX1, GLint p_srcY1, GLint p_dstX0, GLint p_dstY0, GLint p_dstX1, GLint p_dstY1, GLbitfield p_mask, GLenum p_filter
    if (!p_readFramebuffer_rev || !p_drawFramebuffer_rev) ERROR("Invalid framebuffer name");
    if (!p_readFramebuffer_rev->has_object || !p_drawFramebuffer_rev->has_object)
        ERROR("Framebuffer name has no object");
    VALIDATE_BLIT_FRAMEBUFFER
    real(p_readFramebuffer_rev->real, p_drawFramebuffer_rev->real, p_srcX0, p_srcY0, p_srcX1, p_srcY1, p_dstX0, p_dstY0, p_dstX1, p_dstY1, p_mask, p_filter);
    update_buffers(p_drawFramebuffer_rev->head.obj, p_mask, false); //TODO: Use GL_COLOR_WRITEMASK here?
