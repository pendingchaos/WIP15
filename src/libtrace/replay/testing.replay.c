#define REPLAY 1
#define SWITCH_REPLAY(a, b) a
#include "testing/objects/objects.h"
#undef REPLAY
#undef SWITCH_REPLAY

static bool expect_property_common(GLenum objType, GLuint64 objName, const trc_obj_rev_head_t** rev,
                                   uint64_t* real, const testing_property_t** properties) {
    //This code would usually be done before the call, but that is not the case for wip15* functions
    trc_obj_t* cur_ctx = trc_get_current_gl_context(ctx->trace, -1);
    if (!cur_ctx && objType!=0) {
        fprintf(stderr, "No context bound at wip15Expect...");
        return NULL;
    }
    const trc_gl_context_rev_t* ctx_rev = trc_obj_get_rev(cur_ctx, -1);
    
    *properties = get_object_type_properties(objType);
    switch (objType) {
    #define O(val, name, type, namespace)\
    case val: {\
        const trc_gl_##name##_rev_t* obj_rev;\
        obj_rev = trc_obj_get_rev(trc_lookup_name(ctx_rev->namespace, type, objName, -1), -1);\
        *rev = &obj_rev->head;\
        *real = obj_rev ? obj_rev->real : 0;\
        break;\
    }
    O(GL_BUFFER, buffer, TrcBuffer, namespace)
    O(GL_SHADER, shader, TrcShader, namespace)
    O(GL_PROGRAM, program, TrcProgram, namespace)
    O(GL_VERTEX_ARRAY, vao, TrcVAO, priv_ns)
    O(GL_QUERY, query, TrcQuery, namespace)
    O(GL_PROGRAM_PIPELINE, program_pipeline, TrcProgramPipeline, priv_ns)
    O(GL_TRANSFORM_FEEDBACK, transform_feedback, TrcTransformFeedback, priv_ns)
    O(GL_SAMPLER, sampler, TrcSampler, namespace)
    O(GL_TEXTURE, texture, TrcTexture, namespace)
    O(GL_RENDERBUFFER, renderbuffer, TrcRenderbuffer, namespace)
    O(GL_FRAMEBUFFER, framebuffer, TrcFramebuffer, priv_ns)
    case 0: {
        const trc_gl_context_rev_t* obj_rev = trc_get_context(ctx->trace);
        *rev = &obj_rev->head;
        *real = obj_rev ? (uintptr_t)obj_rev->real : 0;
        break;
    }
    }
    if (!*rev) fprintf(stderr, "Invalid object name\n");
    return *rev;
}

#define EXPECT_NUMERICAL_PROPERTY(type, fmt) do {\
    const trc_obj_rev_head_t* rev = NULL;\
    const testing_property_t* properties = NULL;\
    uint64_t realobj;\
    if (!expect_property_common(p_objType, p_objName, &rev, &realobj, &properties)) RETURN;\
    bool tested = false;\
    type val, gl_val;\
    bool has_val, has_gl_val, success = true;\
    for (const testing_property_t* prop = properties; prop; prop = prop->next) {\
        if (strcmp(prop->name, p_name) != 0) continue;\
        if (prop->get_func_int)\
            success = (val=prop->get_func_int(p_index, rev))==p_val && success;\
        if (prop->get_func_gl_int)\
            success = (gl_val=prop->get_func_gl_int(p_index, ctx, rev, realobj))==p_val && success;\
        if (prop->get_func_double)\
            success = (val=prop->get_func_double(p_index, rev))==p_val && success;\
        if (prop->get_func_gl_double)\
            success = (gl_val=prop->get_func_gl_double(p_index, ctx, rev, realobj))==p_val && success;\
        has_val = prop->get_func_int || prop->get_func_double;\
        has_gl_val = prop->get_func_gl_int || prop->get_func_gl_double;\
        if (!has_val && !has_gl_val) ERROR("Property is not of a compatible type");\
        tested = true;\
        break;\
    }\
    if (!tested || !success) {\
        trc_replay_test_failure_t* f = malloc(sizeof(trc_replay_test_failure_t));\
        f->error_message[sizeof(f->error_message)-1] = 0;\
        size_t len = 0;\
        if (!tested) {\
            len += snprintf(f->error_message, sizeof(f->error_message)-1, "No such property: '%s'", p_name);\
        } else {\
            len += snprintf(f->error_message, sizeof(f->error_message)-1, "Expectation of '%s' failed. Expected "fmt", got ", p_name, p_val);\
            if (has_gl_val)\
                len += snprintf(&f->error_message[len], sizeof(f->error_message)-len-1, fmt"(gl)", gl_val);\
            if (has_gl_val && has_val) {\
                strncat(&f->error_message[len], " and ", sizeof(f->error_message)-len-1);\
                len += 5;\
            }\
            if (has_val)\
                len += snprintf(&f->error_message[len], sizeof(f->error_message)-len-1, fmt"(rev)", val);\
            strncat(f->error_message, ".", sizeof(f->error_message)-len-1);\
        }\
        f->next = ctx->current_test->failures;\
        ctx->current_test->failures = f;\
    }\
} while (0)

wip15ExpectPropertyi64: //GLenum p_objType, GLuint64 p_objName, const char* p_name, GLuint64 p_index, GLint64 p_val
    if (!ctx->current_test) ERROR("No test is current");
    EXPECT_NUMERICAL_PROPERTY(int64_t, "%"PRIu64);

wip15ExpectPropertyd: //GLenum p_objType, GLuint64 p_objName, const char* p_name, GLuint64 p_index, GLdouble p_val
    if (!ctx->current_test) ERROR("No test is current");
    EXPECT_NUMERICAL_PROPERTY(double, "%f");

wip15ExpectPropertybv: //GLenum p_objType, GLuint64 p_objName, const char* p_name, GLuint64 p_index, GLuint64 p_size, const void* p_data
    if (!ctx->current_test) ERROR("No test is current");
    
    bool success = true;
    const trc_obj_rev_head_t* rev = NULL;
    const testing_property_t* properties = NULL;
    uint64_t realobj;
    if (!expect_property_common(p_objType, p_objName, &rev, &realobj, &properties)) RETURN;
    bool tested = false;
    for (const testing_property_t* prop = properties; prop; prop = prop->next) {
        if (strcmp(prop->name, p_name) != 0) continue;
        size_t size;
        if (prop->get_func_data) {
            void* data = prop->get_func_data(p_index, rev, &size);
            success = success && size==p_size && memcmp(data, p_data, size)==0;
            free(data);
        }
        if (prop->get_func_gl_data) {
            void* data = prop->get_func_gl_data(p_index, ctx, rev, realobj, &size);
            success = success && size==p_size && memcmp(data, p_data, size)==0;
            free(data);
        }
        tested = prop->get_func_data || prop->get_func_gl_data;
        if (!tested) ERROR("Property is not of a compatible type");
        break;
    }
    if (!success || !tested) {
        trc_replay_test_failure_t* f = malloc(sizeof(trc_replay_test_failure_t));
        if (!tested) snprintf(f->error_message, sizeof(f->error_message), "No such property: '%s'", p_name);
        else snprintf(f->error_message, sizeof(f->error_message), "Expectation of '%s' failed.", p_name);
        f->next = ctx->current_test->failures;
        ctx->current_test->failures = f;
    }

wip15ExpectAttachment: //const char* p_attachment
    if (!ctx->current_test) ERROR("No test is current");
    
    size_t frame_index = ctx->trace->inspection.frame_index;
    size_t cmd_index = ctx->trace->inspection.cmd_index;
    trace_command_t* other_cmd;
    while (true) {
        trace_frame_t* frame = &ctx->trace->frames[frame_index];
        other_cmd = &frame->commands[cmd_index];
        const char* name = ctx->trace->func_names[other_cmd->func_index];
        if (strncmp(name, "wip15", 5) == 0) {
            if (frame_index==0 && cmd_index==0) {
                goto failure;
            } else if (cmd_index == 0) {
                frame_index--;
                cmd_index = ctx->trace->frames[frame_index].command_count - 1;
            } else {
                cmd_index--;
            }
            continue;
        }
        break;
    }
    for (trc_attachment_t* a = other_cmd->attachments; a; a=a->next) {
        if (strcmp(a->message, p_attachment) == 0) goto success;
    }
    
    failure: ;
    trc_replay_test_failure_t* f = malloc(sizeof(trc_replay_test_failure_t));
    snprintf(f->error_message, sizeof(f->error_message), "Expectation of attachment failed.");
    f->next = ctx->current_test->failures;
    ctx->current_test->failures = f;
    success: ;

wip15TestFB: //const GLchar* p_name, const GLvoid* p_color, const GLvoid* p_depth
    if (!ctx->current_test) ERROR("No test is current");
    
    F(glFinish)();
    
    //TODO: Save, modify and restore more state (e.g. pack parameters)
    
    GLint last_buf;
    F(glGetIntegerv)(GL_READ_BUFFER, &last_buf);
    
    int w, h;
    SDL_GL_GetDrawableSize(ctx->window, &w, &h);
    
    F(glReadBuffer)(GL_BACK);
    uint32_t* back = malloc(w*h*4);
    F(glReadPixels)(0, 0, w, h, GL_RGBA, GL_UNSIGNED_BYTE, back);
    
    uint32_t* depth = malloc(w*h*4);
    F(glReadPixels)(0, 0, w, h, GL_DEPTH_COMPONENT, GL_UNSIGNED_INT, depth);
    
    F(glReadBuffer)(last_buf);
    
    if (memcmp(back, p_color, w*h*4) != 0) {
        trc_replay_test_failure_t* f = malloc(sizeof(trc_replay_test_failure_t));
        snprintf(f->error_message, sizeof(f->error_message),
            "%s did not result in the correct back color buffer", p_name);
        f->next = ctx->current_test->failures;
        ctx->current_test->failures = f;
    }
    if (memcmp(depth, p_depth, w*h*4) != 0) {
        trc_replay_test_failure_t* f = malloc(sizeof(trc_replay_test_failure_t));
        snprintf(f->error_message, sizeof(f->error_message),
            "%s did not result in the correct depth buffer", p_name);
        f->next = ctx->current_test->failures;
        ctx->current_test->failures = f;
    }
    
    free(back);
    free(depth);

wip15BeginTest: //const GLchar* p_name
    if (ctx->current_test) ERROR("A test is already current");
    
    trc_replay_test_t* test = malloc(sizeof(trc_replay_test_t));
    test->name[sizeof(test->name)-1] = 0;
    strncpy(test->name, p_name, sizeof(test->name)-1);
    test->successes = 0;
    test->failures = NULL;
    test->next = NULL;
    ctx->current_test = test;

wip15EndTest: //
    ctx->current_test->next = ctx->tests;
    ctx->tests = ctx->current_test;
    ctx->current_test = NULL;

wip15PrintTestResults: //
    size_t tests = 0;
    size_t tests_passed = 0;
    for (const trc_replay_test_t* test = ctx->tests; test; test = test->next) {
        for (const trc_replay_test_failure_t* e = test->failures;
             e; e = e->next) {
            fprintf(stderr, "\x1b[38;5;196m%s failed: %s\x1b[39m\n",
                    test->name, e->error_message);
        }
        tests++;
        if (test->failures == NULL)
            tests_passed++;
    }
    fprintf(stderr, "%zu/%zu tests passed\n", tests_passed, tests);

