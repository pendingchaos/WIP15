#include "libtrace/replay/utils.h"

static void gen_transform_feedbacks(size_t count, const GLuint* real, const GLuint* fake, bool create) {
    trc_gl_transform_feedback_rev_t rev;
    rev.has_object = create;
    size_t size = gls_get_state_int(GL_MAX_TRANSFORM_FEEDBACK_BUFFERS, 0);
    size *= sizeof(trc_gl_buffer_binding_point_t);
    rev.bindings = trc_create_data(ctx->trace, size, NULL, 0);
    rev.active = false;
    rev.paused = false;
    rev.active_not_paused = false;
    for (size_t i = 0; i < count; ++i) {
        rev.real = real[i];
        trc_create_named_obj(ctx->priv_ns, TrcTransformFeedback, fake[i], &rev);
    }
}

static void gen_queries(size_t count, const GLuint* real, const GLuint* fake, bool create, GLenum target) {
    trc_gl_query_rev_t rev;
    rev.has_object = create;
    rev.type = create ? target : 0;
    rev.result = 0;
    rev.active_index = -1;
    for (size_t i = 0; i < count; ++i) {
        rev.real = real[i];
        trc_create_named_obj(ctx->ns, TrcQuery, fake[i], &rev);
    }
}

trc_obj_t** get_tf_buffer_list(size_t* count) {
    size_t max = gls_get_state_int(GL_MAX_TRANSFORM_FEEDBACK_BUFFERS, 0) + 1;
    trc_obj_t** bufs = replay_alloc(max*sizeof(trc_obj_t*));
    *count = 0;
    
    for (uint i = 0; i < max-1; i++) {
        trc_obj_t* buf = gls_get_bound_buffer_indexed(GL_TRANSFORM_FEEDBACK_BUFFER, i).buf.obj;
        if (buf) bufs[(*count)++] = buf;
    }
    
    return bufs;
}

static void bind_tf_buffer(GLuint tf, GLuint index, GLuint buffer,
                           bool ranged, GLintptr offset, GLsizeiptr size) {
    const trc_gl_transform_feedback_rev_t* tf_rev = get_transform_feedback(tf);
    if (!tf_rev) ERROR2(, "Invalid transform feedback name");
    if (tf_rev->active) ERROR2(, "The specified transform feedback object is active");
    size_t count = tf_rev->bindings->size / sizeof(trc_gl_buffer_binding_point_t);
    if (index >= count) ERROR2(, "Invalid index");
    
    const trc_gl_buffer_rev_t* buf_rev = get_buffer(buffer);
    if (!buf_rev && buffer) ERROR2(, "Invalid buffer name");
    if (ranged && buf_rev && (size<=0 || offset+size>buf_rev->data.size))
        ERROR2(, "Invalid range");
    //TODO: Check alignment of offset?
    
    if (!ranged) F(glTransformFeedbackBufferBase)(tf_rev->real, index, buf_rev?buf_rev->real:0);
    else F(glTransformFeedbackBufferRange)(tf_rev->real, index, buf_rev?buf_rev->real:0, offset, size);
    
    trc_gl_transform_feedback_rev_t newrev = *tf_rev;
    newrev.bindings = trc_copy_data(ctx->trace, newrev.bindings);
    trc_gl_buffer_binding_point_t* bindings = trc_map_data(newrev.bindings, TRC_MAP_MODIFY);
    trc_set_obj_ref(&bindings[index].buf, buf_rev?buf_rev->head.obj:NULL);
    bindings[index].offset = ranged ? offset : 0;
    bindings[index].size = ranged ? size : 0;
    trc_unmap_data(bindings);
    set_transform_feedback(&newrev);
}

static void on_activate_tf() {
    size_t buf_count;
    trc_obj_t** bufs = get_tf_buffer_list(&buf_count);
    for (size_t i = 0; i < buf_count; i++) {
        if (((const trc_gl_buffer_rev_t*)trc_obj_get_rev(bufs[i], -1))->mapped)
            trc_add_warning(cmd, "Buffer bound to GL_TRANSFORM_FEEDBACK_BUFFER is mapped");
    }
}

static void update_query(GLenum target, GLuint fake_id, GLuint id) {
    if (!id) return;
    
    GLuint64 res = 0;
    //Replay is currently much slower than recording - so the results of time queries are useless
    if (target!=GL_TIME_ELAPSED && target!=GL_TIMESTAMP) {
        F(glFinish)();
        while (!res) F(glGetQueryObjectui64v)(id, GL_QUERY_RESULT_AVAILABLE, &res);
        F(glGetQueryObjectui64v)(id, GL_QUERY_RESULT, &res);
    }
    
    trc_gl_query_rev_t query = *get_query(fake_id);
    query.result = res;
    set_query(&query);
}

static bool begin_query(GLenum target, GLuint index, GLuint id) {
    if (index >= gls_get_bound_queries_size(target))
        ERROR2(false, "Index is greater than maximum");
    if (gls_get_bound_queries(target, index))
        ERROR2(false, "Query is already active at target");
    
    const trc_gl_query_rev_t* rev = get_query(id);
    if (!rev) ERROR2(false, "Invalid query name");
    if (rev->active_index != -1) ERROR2(false, "Query is already active");
    if (rev->has_object && rev->type!=target)
        ERROR2(false, "Query object's type does not match target");
    
    trc_gl_query_rev_t newrev = *rev;
    newrev.type = target;
    newrev.has_object = true;
    newrev.active_index = index;
    set_query(&newrev);
    
    gls_set_bound_queries(target, index, newrev.head.obj);
    
    return true;
}

static bool end_query(GLenum target, GLuint index) {
    if (index >= gls_get_bound_queries_size(target))
        ERROR2(false, "Index is greater than maximum");
    trc_obj_t* query = gls_get_bound_queries(target, index);
    if (!query) ERROR2(false, "No query active at target");
    
    trc_gl_query_rev_t newrev = *(const trc_gl_query_rev_t*)trc_obj_get_rev(query, -1);
    newrev.active_index = -1;
    set_query(&newrev);
    
    gls_set_bound_queries(target, index, NULL);
    return true;
}

glTransformFeedbackVaryings: //GLuint p_program, GLsizei p_count, const GLchar*const* p_varyings, GLenum p_bufferMode
    if (!p_program_rev) ERROR("Invalid program name");
    real(p_program_rev->real, p_count, p_varyings, p_bufferMode);
    //TODO: Store this in the program object

glGetTransformFeedbackVarying: //GLuint p_program, GLuint p_index, GLsizei p_bufSize, GLsizei* p_length, GLsizei* p_size, GLenum* p_type, GLchar* p_name
    if (!p_program_rev) ERROR("Invalid program name");
    if (!p_program_rev->has_been_linked) ERROR("Program has not been linked");
    GLint varying_count;
    F(glGetProgramiv)(p_program_rev->real, GL_TRANSFORM_FEEDBACK_VARYINGS, &varying_count);
    if (p_index >= varying_count) ERROR("Index is out of bounds");

glGenTransformFeedbacks: //GLsizei p_n, GLuint* p_ids
    if (p_n < 0) ERROR("Invalid transform feedback name count");
    GLuint* tf = replay_alloc(p_n*sizeof(GLuint));
    real(p_n, tf);
    gen_transform_feedbacks(p_n, tf, p_ids, false);

glCreateTransformFeedbacks: //GLsizei p_n, GLuint* p_ids
    if (p_n < 0) ERROR("Invalid transform feedback name count");
    GLuint* tf = replay_alloc(p_n*sizeof(GLuint));
    real(p_n, tf);
    gen_transform_feedbacks(p_n, tf, p_ids, true);

glBindTransformFeedback: //GLenum p_target, GLuint p_id
    if (get_current_tf()->active_not_paused)
        ERROR("The current transform feedback object is active and not paused");
    
    const trc_gl_transform_feedback_rev_t* rev = get_transform_feedback(p_id);
    if (!rev) ERROR("Invalid transform feedback name");
    
    if (!rev->has_object) {
        trc_gl_transform_feedback_rev_t newrev = *rev;
        newrev.has_object = true;
        set_transform_feedback(&newrev);
        rev = trc_obj_get_rev(newrev.head.obj, -1);
    }
    
    trc_gl_context_rev_t ctx_rev = *trc_get_context(ctx->trace);
    ctx_rev.bound_buffer_indexed_GL_TRANSFORM_FEEDBACK_BUFFER = rev->bindings;
    trc_set_context(ctx->trace, &ctx_rev);
    
    gls_set_current_tf(rev->head.obj);

glDeleteTransformFeedbacks: //GLsizei p_n, const GLuint* p_ids
    GLuint* tf = replay_alloc(p_n*sizeof(GLuint));
    memset(tf, 0, p_n*sizeof(GLuint));
    for (size_t i = 0; i < p_n; ++i) {
        if (!p_ids[i]) continue;
        
        if (!(tf[i] = get_real_transform_feedback(p_ids[i]))) {
            trc_add_error(cmd, "Invalid transform feedback object name");
        } else {
            const trc_gl_transform_feedback_rev_t* tf = get_transform_feedback(p_ids[i]);
            if (tf->active) {
                trc_add_error(cmd, "Transform feedback object at index %zu is active", i);
                continue;
            }
            if (gls_get_current_tf() == tf->head.obj)
                gls_set_current_tf(get_transform_feedback(0)->head.obj);
            delete_obj(p_ids[i], TrcTransformFeedback);
        }
    }
    real(p_n, tf);

glTransformFeedbackBufferBase: //GLuint p_xfb, GLuint p_index, GLuint p_buffer
    bind_tf_buffer(p_xfb, p_index, p_buffer, false, 0, 0);

glTransformFeedbackBufferRange: //GLuint p_xfb, GLuint p_index, GLuint p_buffer, GLintptr p_offset, GLsizeiptr p_size
    bind_tf_buffer(p_xfb, p_index, p_buffer, true, p_offset, p_size);

glBeginTransformFeedback: //GLenum p_primitiveMode
    trc_gl_transform_feedback_rev_t rev = *get_current_tf();
    if (rev.active) ERROR("Transform feedback is already active");
    //TODO: Check that needed binding points have buffers
    //TODO: Check to see if a program or pipeline object is bound and that it has varying variables
    real(p_primitiveMode);
    rev.active = true;
    rev.active_not_paused = true;
    set_transform_feedback(&rev);
    gls_set_tf_primitive(p_primitiveMode);
    on_activate_tf();

glEndTransformFeedback: //
    trc_gl_transform_feedback_rev_t rev = *get_current_tf();
    if (!rev.active) ERROR("Transform feedback is not active");
    real();
    rev.active = false;
    rev.paused = false;
    rev.active_not_paused = false;
    set_transform_feedback(&rev);

glPauseTransformFeedback: //
    trc_gl_transform_feedback_rev_t rev = *get_current_tf();
    if (!rev.active) ERROR("Transform feedback is not active");
    if (rev.paused) ERROR("Transform feedback is already paused");
    real();
    rev.paused = true;
    rev.active_not_paused = false;
    set_transform_feedback(&rev);

glResumeTransformFeedback: //
    trc_gl_transform_feedback_rev_t rev = *get_current_tf();
    if (!rev.active) ERROR("Transform feedback is not active");
    if (!rev.paused) ERROR("Transform feedback is not paused");
    real();
    rev.paused = false;
    rev.active_not_paused = true;
    set_transform_feedback(&rev);
    on_activate_tf();

glGenQueries: //GLsizei p_n, GLuint* p_ids
    if (p_n < 0) ERROR("Invalid name query count");
    GLuint* queries = replay_alloc(p_n*sizeof(GLuint));
    real(p_n, queries);
    gen_queries(p_n, queries, p_ids, false, 0);

glCreateQueries: //GLenum p_target, GLsizei p_n, GLuint* p_ids
    if (p_n < 0) ERROR("Invalid name query count");
    GLuint* queries = replay_alloc(p_n*sizeof(GLuint));
    real(p_target, p_n, queries);
    gen_queries(p_n, queries, p_ids, false, p_target);

glDeleteQueries: //GLsizei p_n, const GLuint* p_ids
    GLuint* queries = replay_alloc(p_n*sizeof(GLuint));
    for (size_t i = 0; i < p_n; ++i) {
        //TODO: Handle when queries are in use
        if (!(queries[i] = get_real_query(p_ids[i])) && p_ids[i])
            trc_add_warning(cmd, "Invalid query name");
        else delete_obj(p_ids[i], TrcQuery);
    }
    real(p_n, queries);

glBeginQuery: //GLenum p_target, GLuint p_id
    if (begin_query(p_target, 0, p_id))
        real(p_target, p_id_rev->real);

glEndQuery: //GLenum p_target
    if (end_query(p_target, 0)) real(p_target);

glBeginQueryIndexed: //GLenum p_target, GLuint p_index, GLuint p_id
    if (begin_query(p_target, p_index, p_id))
        real(p_target, p_index, p_id_rev->real);

glEndQueryIndexed: //GLenum p_target, GLuint p_index
    if (end_query(p_target, p_index)) real(p_target, p_index);

glQueryCounter: //GLuint p_id, GLenum p_target
    const trc_gl_query_rev_t* rev = get_query(p_id);
    if (!rev) ERROR("Invalid query name");
    
    bool bound = false;
    GLenum targets[6] = {
        GL_SAMPLES_PASSED, GL_ANY_SAMPLES_PASSED, GL_ANY_SAMPLES_PASSED_CONSERVATIVE,
        GL_PRIMITIVES_GENERATED, GL_TRANSFORM_FEEDBACK_PRIMITIVES_WRITTEN, GL_TIME_ELAPSED};
    for (size_t i = 0; i < 6; i++) {
        for (size_t j = 0; j < gls_get_bound_queries_size(targets[j]); j++) {
            if (gls_get_bound_queries(targets[j], i) == rev->head.obj)
                ERROR("Query is used by a glBeginQuery/glEndQuery block");
        }
    }
    
    real(rev->real, p_target);
    if (!rev->has_object) {
        trc_gl_query_rev_t newrev = *rev;
        newrev.has_object = true;
        newrev.type = p_target;
        set_query(&newrev);
    }
    
    update_query(p_target, p_id, rev->real);

glGetQueryiv: //GLenum p_target, GLenum p_pname, GLint* p_params
    ;

glGetQueryObjectiv: //GLuint p_id, GLenum p_pname, GLint* p_params
    if (!p_id_rev) ERROR("Invalid query name");

glGetQueryObjectuiv: //GLuint p_id, GLenum p_pname, GLuint* p_params
    if (!p_id_rev) ERROR("Invalid query name");

glGetQueryObjecti64v: //GLuint p_id, GLenum p_pname, GLint64* p_params
    if (!p_id_rev) ERROR("Invalid query name");

glGetQueryObjectui64v: //GLuint p_id, GLenum p_pname, GLuint64* p_params
    if (!p_id_rev) ERROR("Invalid query name");

glGetQueryIndexediv: //GLenum p_target, GLuint p_index, GLenum p_pname, GLint* p_params
    ;

glGetSynciv: //GLsync p_sync, GLenum p_pname, GLsizei p_bufSize, GLsizei* p_length, GLint* p_values
    if (!get_real_sync(p_sync)) ERROR("Invalid sync name");
    //TODO: More validation should be done

glFenceSync: //GLenum p_condition, GLbitfield p_flags
    GLsync real_sync = real(p_condition, p_flags);
    
    uint64_t fake = *trc_get_ptr(&cmd->ret);
    
    trc_gl_sync_rev_t rev;
    rev.real = (uint64_t)real_sync;
    rev.type = GL_SYNC_FENCE;
    rev.condition = p_condition;
    rev.flags = p_flags;
    trc_create_named_obj(ctx->ns, TrcSync, fake, &rev);

glDeleteSync: //GLsync p_sync
    if (!p_sync_rev && p_sync) ERROR("Invalid sync name");
    real(p_sync?(GLsync)p_sync_rev->real:0);
    if (p_sync) delete_obj(p_sync, TrcSync);

glWaitSync: //GLsync p_sync, GLbitfield p_flags, GLuint64 p_timeout
    if (!p_sync_rev) ERROR("Invalid sync name");
    real((GLsync)p_sync_rev->real, p_flags, p_timeout);
    //TODO: grab the object and release it once the wait is over

glClientWaitSync: //GLsync p_sync, GLbitfield p_flags, GLuint64 p_timeout
    if (!p_sync_rev) ERROR("Invalid sync name");
    real((GLsync)p_sync_rev->real, p_flags, p_timeout);

glIsQuery: //GLuint p_id
    ;

glIsTransformFeedback: //GLuint p_id
    ;

glIsSync: //GLsync p_sync
    ;
