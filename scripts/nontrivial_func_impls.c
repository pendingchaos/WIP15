#define REPLAY 1
#define SWITCH_REPLAY(a, b) a
#include "testing/objects/objects.h"
#undef REPLAY
#undef SWITCH_REPLAY

static bool expect_property_common(trace_command_t* cmd, trc_replay_context_t* ctx, GLenum objType, GLuint64 objName,
                                   const trc_obj_rev_head_t** rev, uint64_t* real, const testing_property_t** properties) {
    //This code would usually be done before the call, but that is not the case for wip15* functions
    trc_obj_t* cur_ctx = trc_get_current_gl_context(ctx->trace, -1);
    if (!cur_ctx) {
        fprintf(stderr, "No context bound at wip15Expect...");
        return NULL;
    }
    trc_namespace_t* namespace = ((const trc_gl_context_rev_t*)trc_obj_get_rev(cur_ctx, -1))->namespace;
    
    *properties = get_object_type_properties(objType);
    switch (objType) {
    #define O(val, name)\
    case val: {\
        const trc_gl_##name##_rev_t* obj_rev = get_##name(namespace, objName);\
        *rev = &obj_rev->head;\
        *real = obj_rev ? obj_rev->real : 0;\
        break;\
    }
    O(GL_BUFFER, buffer)
    O(GL_SHADER, shader)
    O(GL_PROGRAM, program)
    O(GL_VERTEX_ARRAY, vao)
    O(GL_QUERY, query)
    O(GL_PROGRAM_PIPELINE, program_pipeline)
    O(GL_TRANSFORM_FEEDBACK, transform_feedback)
    O(GL_SAMPLER, sampler)
    O(GL_TEXTURE, texture)
    O(GL_RENDERBUFFER, renderbuffer)
    O(GL_FRAMEBUFFER, framebuffer)
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
    if (!expect_property_common(cmd, ctx, p_objType, p_objName, &rev, &realobj, &properties)) return;\
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
    if (!expect_property_common(cmd, ctx, p_objType, p_objName, &rev, &realobj, &properties)) return;
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

wip15ExpectError: //const char* p_error
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
        if (a->type != TrcAttachType_Error) continue;
        if (strcmp(a->message, p_error) == 0) goto success;
    }
    
    failure: ;
    trc_replay_test_failure_t* f = malloc(sizeof(trc_replay_test_failure_t));
    snprintf(f->error_message, sizeof(f->error_message), "Expectation of error failed.");
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

double conv_from_signed_norm(trace_t* trace, int64_t val, size_t bits) {
    uint ver = trc_gl_state_get_ver(trace);
    if (ver >= 420) return fmax(val/pow(2.0, bits-1), -1.0);
    else return (val*2+1) / pow(2.0, bits-1);
}

void conv_from_signed_norm_array_i64(trace_t* trace, size_t count, double* dest, const int64_t* src, size_t bits) {
    for (size_t i = 0; i < count; i++)
        dest[i] = conv_from_signed_norm(trace, src[i], bits);
}

void conv_from_signed_norm_array_i32(trace_t* trace, size_t count, double* dest, const int32_t* src, size_t bits) {
    for (size_t i = 0; i < count; i++)
        dest[i] = conv_from_signed_norm(trace, src[i], bits);
}

static void delete_obj(trc_namespace_t* ns, uint64_t fake, trc_obj_type_t type) {
    if (!fake) return;
    trc_obj_t* obj = trc_lookup_name(ns, type, fake, -1);
    if (!obj) return;
    trc_drop_obj(obj);
    trc_free_name(ns, type, fake);
}

static bool sample_param_double(trace_command_t* cmd, trc_gl_sample_params_t* params,
                                GLenum param, uint32_t count, const double* val) {
    switch (param) {
    case GL_TEXTURE_MIN_FILTER:
    case GL_TEXTURE_MAG_FILTER:
    case GL_TEXTURE_MIN_LOD:
    case GL_TEXTURE_MAX_LOD:
    case GL_TEXTURE_WRAP_S:
    case GL_TEXTURE_WRAP_T:
    case GL_TEXTURE_WRAP_R:
    case GL_TEXTURE_COMPARE_MODE:
    case GL_TEXTURE_COMPARE_FUNC:
    case GL_TEXTURE_LOD_BIAS:
    case GL_TEXTURE_MAX_ANISOTROPY:
        if (count != 1)
            ERROR2(true, "Expected 1 value. Got %u.", count);
        break;
    case GL_TEXTURE_BORDER_COLOR:
        if (count != 4)
            ERROR2(true, "Expected 4 values. Got %u.", count);
        break;
    }
    
    switch (param) {
    case GL_TEXTURE_MIN_FILTER:
        if (val[0]!=GL_LINEAR && val[0]!=GL_NEAREST && val[0]!=GL_NEAREST_MIPMAP_NEAREST &&
            val[0]!=GL_LINEAR_MIPMAP_NEAREST && val[0]!=GL_NEAREST_MIPMAP_LINEAR &&
            val[0]!=GL_LINEAR_MIPMAP_LINEAR)
            ERROR2(true, "Invalid minification filter");
        break;
    case GL_TEXTURE_MAG_FILTER:
        if (val[0]!=GL_LINEAR && val[0]!=GL_NEAREST)
            ERROR2(true, "Invalid magnification filter");
        break;
    case GL_TEXTURE_WRAP_S:
    case GL_TEXTURE_WRAP_T:
    case GL_TEXTURE_WRAP_R:
        if (val[0]!=GL_CLAMP_TO_EDGE && val[0]!=GL_CLAMP_TO_BORDER && val[0]!=GL_MIRRORED_REPEAT &&
            val[0]!=GL_REPEAT && val[0]!=GL_MIRROR_CLAMP_TO_EDGE && val[0]!=GL_CLAMP_TO_EDGE)
            ERROR2(true, "Invalid wrap mode");
        break;
    case GL_TEXTURE_COMPARE_MODE:
        if (val[0]!=GL_COMPARE_REF_TO_TEXTURE && val[0]!=GL_NONE)
            ERROR2(true, "Invalid compare mode");
        break;
    case GL_TEXTURE_COMPARE_FUNC:
        if (val[0]!=GL_LEQUAL && val[0]!=GL_GEQUAL && val[0]!=GL_LESS && val[0]!=GL_GREATER &&
            val[0]!=GL_EQUAL && val[0]!=GL_NOTEQUAL && val[0]!=GL_ALWAYS && val[0]!=GL_NEVER)
            ERROR2(true, "Invalid compare function");
        break;
    case GL_TEXTURE_MAX_ANISOTROPY:
        if (val[0] < 1.0) ERROR2(true, "Invalid max anisotropy");
        break;
    }
    
    switch (param) {
    case GL_TEXTURE_MIN_FILTER: params->min_filter = val[0]; break;
    case GL_TEXTURE_MAG_FILTER: params->mag_filter = val[0]; break;
    case GL_TEXTURE_MIN_LOD: params->min_lod = val[0]; break;
    case GL_TEXTURE_MAX_LOD: params->max_lod = val[0]; break;
    case GL_TEXTURE_WRAP_S: params->wrap_s = val[0]; break;
    case GL_TEXTURE_WRAP_T: params->wrap_t = val[0]; break;
    case GL_TEXTURE_WRAP_R: params->wrap_r = val[0]; break;
    case GL_TEXTURE_BORDER_COLOR:
        for (uint i = 0; i < 4; i++) params->border_color[i] = val[i];
        break;
    case GL_TEXTURE_COMPARE_MODE: params->compare_mode = val[0]; break;
    case GL_TEXTURE_COMPARE_FUNC: params->compare_func = val[0]; break;
    case GL_TEXTURE_LOD_BIAS: params->lod_bias = val[0]; break;
    case GL_TEXTURE_MAX_ANISOTROPY: params->max_anisotropy = val[0]; break;
    }
    
    return false;
}

static void replay_create_context_buffers(trace_t* trace, trc_gl_context_rev_t* rev) {
    size_t size = rev->drawable_width * rev->drawable_height * 4;
    rev->front_color_buffer = trc_create_data(trace, size, NULL, TRC_DATA_IMMUTABLE);
    rev->back_color_buffer = rev->front_color_buffer;
    rev->back_depth_buffer = rev->back_color_buffer;
    rev->back_stencil_buffer = rev->back_depth_buffer;
}

static void replay_update_fb0_buffers(trc_replay_context_t* ctx, bool backcolor, bool frontcolor, bool depth, bool stencil);

static void init_context(trc_replay_context_t* ctx) {
    trace_t* trace = ctx->trace;
    
    trc_gl_state_set_made_current_before(trace, false);
    
    GLint major, minor;
    F(glGetIntegerv)(GL_MAJOR_VERSION, &major);
    F(glGetIntegerv)(GL_MINOR_VERSION, &minor);
    uint ver = major*100 + minor*10;
    
    int w, h;
    SDL_GL_GetDrawableSize(ctx->window, &w, &h);
    trc_gl_state_set_drawable_width(trace, w);
    trc_gl_state_set_drawable_height(trace, h);
    trc_gl_state_bound_buffer_init(trace, GL_ARRAY_BUFFER, (trc_obj_ref_t){NULL});
    trc_gl_state_bound_buffer_init(trace, GL_ATOMIC_COUNTER_BUFFER, (trc_obj_ref_t){NULL});
    trc_gl_state_bound_buffer_init(trace, GL_COPY_READ_BUFFER, (trc_obj_ref_t){NULL});
    trc_gl_state_bound_buffer_init(trace, GL_COPY_WRITE_BUFFER, (trc_obj_ref_t){NULL});
    trc_gl_state_bound_buffer_init(trace, GL_DISPATCH_INDIRECT_BUFFER, (trc_obj_ref_t){NULL});
    trc_gl_state_bound_buffer_init(trace, GL_DRAW_INDIRECT_BUFFER, (trc_obj_ref_t){NULL});
    trc_gl_state_bound_buffer_init(trace, GL_ELEMENT_ARRAY_BUFFER, (trc_obj_ref_t){NULL});
    trc_gl_state_bound_buffer_init(trace, GL_PIXEL_PACK_BUFFER, (trc_obj_ref_t){NULL});
    trc_gl_state_bound_buffer_init(trace, GL_PIXEL_UNPACK_BUFFER, (trc_obj_ref_t){NULL});
    trc_gl_state_bound_buffer_init(trace, GL_QUERY_BUFFER, (trc_obj_ref_t){NULL});
    trc_gl_state_bound_buffer_init(trace, GL_SHADER_STORAGE_BUFFER, (trc_obj_ref_t){NULL});
    trc_gl_state_bound_buffer_init(trace, GL_TEXTURE_BUFFER, (trc_obj_ref_t){NULL});
    trc_gl_state_bound_buffer_init(trace, GL_TRANSFORM_FEEDBACK_BUFFER, (trc_obj_ref_t){NULL});
    trc_gl_state_bound_buffer_init(trace, GL_UNIFORM_BUFFER, (trc_obj_ref_t){NULL});
    trc_gl_state_bound_program_init(trace, (trc_obj_ref_t){NULL});
    trc_gl_state_bound_pipeline_init(trace, (trc_obj_ref_t){NULL});
    trc_gl_state_bound_vao_init(trace, (trc_obj_ref_t){NULL});
    trc_gl_state_bound_renderbuffer_init(trace, (trc_obj_ref_t){NULL});
    trc_gl_state_read_framebuffer_init(trace, (trc_obj_ref_t){NULL});
    trc_gl_state_draw_framebuffer_init(trace, (trc_obj_ref_t){NULL});
    trc_gl_state_set_active_texture_unit(trace, 0);
    
    GLint max_query_bindings = 64; //TODO
    trc_gl_state_bound_queries_init(trace, GL_SAMPLES_PASSED, max_query_bindings, NULL);
    trc_gl_state_bound_queries_init(trace, GL_ANY_SAMPLES_PASSED, max_query_bindings, NULL);
    trc_gl_state_bound_queries_init(trace, GL_ANY_SAMPLES_PASSED_CONSERVATIVE, max_query_bindings, NULL);
    trc_gl_state_bound_queries_init(trace, GL_PRIMITIVES_GENERATED, max_query_bindings, NULL);
    trc_gl_state_bound_queries_init(trace, GL_TRANSFORM_FEEDBACK_PRIMITIVES_WRITTEN, max_query_bindings, NULL);
    trc_gl_state_bound_queries_init(trace, GL_TIME_ELAPSED, max_query_bindings, NULL);
    
    GLint max_clip_distances, max_draw_buffers, max_viewports;
    GLint max_vertex_attribs, max_color_attachments, max_tex_units;
    GLint max_patch_vertices, max_renderbuffer_size, max_texture_size, max_transform_feedback_buffers;
    GLint max_uniform_buffer_bindings, max_atomic_counter_buffer_bindings, max_shader_storage_buffer_bindings;
    F(glGetIntegerv)(GL_MAX_CLIP_DISTANCES, &max_clip_distances);
    F(glGetIntegerv)(GL_MAX_DRAW_BUFFERS, &max_draw_buffers);
    if (ver>=410) F(glGetIntegerv)(GL_MAX_VIEWPORTS, &max_viewports);
    else max_viewports = 1;
    F(glGetIntegerv)(GL_MAX_VERTEX_ATTRIBS, &max_vertex_attribs);
    F(glGetIntegerv)(GL_MAX_COLOR_ATTACHMENTS, &max_color_attachments);
    F(glGetIntegerv)(GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS, &max_tex_units);
    F(glGetIntegerv)(GL_MAX_PATCH_VERTICES, &max_patch_vertices);
    F(glGetIntegerv)(GL_MAX_RENDERBUFFER_SIZE, &max_renderbuffer_size);
    F(glGetIntegerv)(GL_MAX_TEXTURE_SIZE, &max_texture_size);
    F(glGetIntegerv)(GL_MAX_TRANSFORM_FEEDBACK_BUFFERS, &max_transform_feedback_buffers);
    F(glGetIntegerv)(GL_MAX_UNIFORM_BUFFER_BINDINGS, &max_uniform_buffer_bindings);
    F(glGetIntegerv)(GL_MAX_ATOMIC_COUNTER_BUFFER_BINDINGS, &max_atomic_counter_buffer_bindings);
    F(glGetIntegerv)(GL_MAX_SHADER_STORAGE_BUFFER_BINDINGS, &max_shader_storage_buffer_bindings);
    
    trc_gl_state_state_int_init1(trace, GL_MAX_CLIP_DISTANCES, max_clip_distances);
    trc_gl_state_state_int_init1(trace, GL_MAX_DRAW_BUFFERS, max_draw_buffers);
    trc_gl_state_state_int_init1(trace, GL_MAX_VIEWPORTS, max_viewports);
    trc_gl_state_state_int_init1(trace, GL_MAX_VERTEX_ATTRIBS, max_vertex_attribs);
    trc_gl_state_state_int_init1(trace, GL_MAX_COLOR_ATTACHMENTS, max_color_attachments);
    trc_gl_state_state_int_init1(trace, GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS, max_tex_units);
    trc_gl_state_state_int_init1(trace, GL_MAX_UNIFORM_BUFFER_BINDINGS, max_uniform_buffer_bindings);
    trc_gl_state_state_int_init1(trace, GL_MAX_PATCH_VERTICES, max_patch_vertices);
    trc_gl_state_state_int_init1(trace, GL_MAX_RENDERBUFFER_SIZE, max_renderbuffer_size);
    trc_gl_state_state_int_init1(trace, GL_MAX_TEXTURE_SIZE, max_texture_size);
    trc_gl_state_state_int_init1(trace, GL_MAX_ATOMIC_COUNTER_BUFFER_BINDINGS, max_atomic_counter_buffer_bindings);
    trc_gl_state_state_int_init1(trace, GL_MAX_SHADER_STORAGE_BUFFER_BINDINGS, max_shader_storage_buffer_bindings);
    trc_gl_state_state_int_init1(trace, GL_MAJOR_VERSION, major);
    trc_gl_state_state_int_init1(trace, GL_MINOR_VERSION, minor);
    trc_gl_state_set_ver(trace, ver);
    
    trc_gl_state_bound_buffer_indexed_init(trace, GL_UNIFORM_BUFFER, max_uniform_buffer_bindings, NULL);
    trc_gl_state_bound_buffer_indexed_init(trace, GL_ATOMIC_COUNTER_BUFFER, max_atomic_counter_buffer_bindings, NULL);
    trc_gl_state_bound_buffer_indexed_init(trace, GL_SHADER_STORAGE_BUFFER, max_shader_storage_buffer_bindings, NULL);
    
    trc_gl_context_rev_t rev = *trc_get_context(ctx->trace);
    
    trc_gl_transform_feedback_rev_t default_tf;
    default_tf.has_object = true;
    default_tf.real = 0;
    size_t size = max_transform_feedback_buffers * sizeof(trc_gl_buffer_binding_point_t);
    default_tf.bindings = trc_create_data(ctx->trace, size, NULL, TRC_DATA_IMMUTABLE);
    default_tf.active = false;
    default_tf.paused = false;
    default_tf.active_not_paused = false;
    trc_obj_t* default_tf_obj = trc_create_named_obj(rev.namespace, TrcTransformFeedback, 0, &default_tf);
    
    rev.bound_buffer_indexed_GL_TRANSFORM_FEEDBACK_BUFFER = default_tf.bindings;
    trc_set_context(ctx->trace, &rev);
    
    trc_gl_state_set_tf_primitive(ctx->trace, 0);
    
    trc_gl_state_default_tf_init(ctx->trace, (trc_obj_ref_t){default_tf_obj});
    trc_gl_state_current_tf_init(ctx->trace, (trc_obj_ref_t){default_tf_obj});
    
    trc_gl_state_bound_textures_init(trace, GL_TEXTURE_1D, max_tex_units, NULL);
    trc_gl_state_bound_textures_init(trace, GL_TEXTURE_2D, max_tex_units, NULL);
    trc_gl_state_bound_textures_init(trace, GL_TEXTURE_3D, max_tex_units, NULL);
    trc_gl_state_bound_textures_init(trace, GL_TEXTURE_1D_ARRAY, max_tex_units, NULL);
    trc_gl_state_bound_textures_init(trace, GL_TEXTURE_2D_ARRAY, max_tex_units, NULL);
    trc_gl_state_bound_textures_init(trace, GL_TEXTURE_RECTANGLE, max_tex_units, NULL);
    trc_gl_state_bound_textures_init(trace, GL_TEXTURE_CUBE_MAP, max_tex_units, NULL);
    trc_gl_state_bound_textures_init(trace, GL_TEXTURE_CUBE_MAP_ARRAY, max_tex_units, NULL);
    trc_gl_state_bound_textures_init(trace, GL_TEXTURE_BUFFER, max_tex_units, NULL);
    trc_gl_state_bound_textures_init(trace, GL_TEXTURE_2D_MULTISAMPLE, max_tex_units, NULL);
    trc_gl_state_bound_textures_init(trace, GL_TEXTURE_2D_MULTISAMPLE_ARRAY, max_tex_units, NULL);
    
    trc_gl_state_bound_samplers_init(trace, max_tex_units, NULL);
    
    trc_gl_state_enabled_init(trace, GL_BLEND, max_draw_buffers, NULL);
    trc_gl_state_enabled_init(trace, GL_CLIP_DISTANCE0, max_clip_distances, NULL);
    trc_gl_state_enabled_init1(trace, GL_COLOR_LOGIC_OP, false);
    trc_gl_state_enabled_init1(trace, GL_CULL_FACE, false);
    trc_gl_state_enabled_init1(trace, GL_DEBUG_OUTPUT, false);
    trc_gl_state_enabled_init1(trace, GL_DEBUG_OUTPUT_SYNCHRONOUS, false);
    trc_gl_state_enabled_init1(trace, GL_DEPTH_CLAMP, false);
    trc_gl_state_enabled_init1(trace, GL_DEPTH_TEST, false);
    trc_gl_state_enabled_init1(trace, GL_DITHER, true);
    trc_gl_state_enabled_init1(trace, GL_FRAMEBUFFER_SRGB, false);
    trc_gl_state_enabled_init1(trace, GL_LINE_SMOOTH, false);
    trc_gl_state_enabled_init1(trace, GL_MULTISAMPLE, false);
    trc_gl_state_enabled_init1(trace, GL_POLYGON_OFFSET_FILL, false);
    trc_gl_state_enabled_init1(trace, GL_POLYGON_OFFSET_LINE, false);
    trc_gl_state_enabled_init1(trace, GL_POLYGON_OFFSET_POINT, false);
    trc_gl_state_enabled_init1(trace, GL_POLYGON_SMOOTH, false);
    trc_gl_state_enabled_init1(trace, GL_PRIMITIVE_RESTART, false);
    trc_gl_state_enabled_init1(trace, GL_PRIMITIVE_RESTART_FIXED_INDEX, false);
    trc_gl_state_enabled_init1(trace, GL_RASTERIZER_DISCARD, false);
    trc_gl_state_enabled_init1(trace, GL_SAMPLE_ALPHA_TO_COVERAGE, false);
    trc_gl_state_enabled_init1(trace, GL_SAMPLE_ALPHA_TO_ONE, false);
    trc_gl_state_enabled_init1(trace, GL_SAMPLE_COVERAGE, false);
    trc_gl_state_enabled_init1(trace, GL_SAMPLE_SHADING, false);
    trc_gl_state_enabled_init1(trace, GL_SAMPLE_MASK, false);
    trc_gl_state_enabled_init(trace, GL_SCISSOR_TEST, max_viewports, NULL);
    trc_gl_state_enabled_init1(trace, GL_STENCIL_TEST, false);
    trc_gl_state_enabled_init1(trace, GL_TEXTURE_CUBE_MAP_SEAMLESS, false);
    trc_gl_state_enabled_init1(trace, GL_PROGRAM_POINT_SIZE, false);
    
    trc_gl_state_state_bool_init1(trace, GL_DEPTH_WRITEMASK, GL_TRUE);
    bool color_mask[max_draw_buffers*4];
    for (size_t i = 0; i < max_draw_buffers*4; i++) color_mask[i] = GL_TRUE;
    trc_gl_state_state_bool_init(trace, GL_COLOR_WRITEMASK, max_draw_buffers*4, color_mask);
    trc_gl_state_state_int_init1(trace, GL_STENCIL_WRITEMASK, 0xffffffff);
    trc_gl_state_state_int_init1(trace, GL_STENCIL_BACK_WRITEMASK, 0xffffffff);
    
    trc_gl_state_state_bool_init1(trace, GL_PACK_SWAP_BYTES, GL_FALSE);
    trc_gl_state_state_bool_init1(trace, GL_PACK_LSB_FIRST, GL_FALSE);
    trc_gl_state_state_bool_init1(trace, GL_UNPACK_SWAP_BYTES, GL_FALSE);
    trc_gl_state_state_bool_init1(trace, GL_UNPACK_LSB_FIRST, GL_FALSE);
    
    trc_gl_state_state_int_init1(trace, GL_PACK_ROW_LENGTH, 0);
    trc_gl_state_state_int_init1(trace, GL_PACK_IMAGE_HEIGHT, 0);
    trc_gl_state_state_int_init1(trace, GL_PACK_SKIP_ROWS, 0);
    trc_gl_state_state_int_init1(trace, GL_PACK_SKIP_PIXELS, 0);
    trc_gl_state_state_int_init1(trace, GL_PACK_SKIP_IMAGES, 0);
    trc_gl_state_state_int_init1(trace, GL_PACK_ALIGNMENT, 4);
    trc_gl_state_state_int_init1(trace, GL_UNPACK_ROW_LENGTH, 0);
    trc_gl_state_state_int_init1(trace, GL_UNPACK_IMAGE_HEIGHT, 0);
    trc_gl_state_state_int_init1(trace, GL_UNPACK_SKIP_ROWS, 0);
    trc_gl_state_state_int_init1(trace, GL_UNPACK_SKIP_PIXELS, 0);
    trc_gl_state_state_int_init1(trace, GL_UNPACK_SKIP_IMAGES, 0);
    trc_gl_state_state_int_init1(trace, GL_UNPACK_ALIGNMENT, 4);
    
    trc_gl_state_state_int_init1(trace, GL_STENCIL_CLEAR_VALUE, 0);
    trc_gl_state_state_float_init1(trace, GL_DEPTH_CLEAR_VALUE, 0);
    float color_clear[4] = {0.0f, 0.0f, 0.0f, 0.0f};
    trc_gl_state_state_float_init(trace, GL_COLOR_CLEAR_VALUE, 4, color_clear);
    
    trc_gl_state_state_float_init1(trace, GL_POINT_SIZE, 1.0f);
    trc_gl_state_state_float_init1(trace, GL_LINE_WIDTH, 1.0f);
    
    trc_gl_state_state_float_init1(trace, GL_POLYGON_OFFSET_UNITS, 0.0f);
    trc_gl_state_state_float_init1(trace, GL_POLYGON_OFFSET_FACTOR, 0.0f);
    trc_gl_state_state_float_init1(trace, GL_POLYGON_OFFSET_CLAMP, 0.0f);
    
    trc_gl_state_state_float_init1(trace, GL_SAMPLE_COVERAGE_VALUE, 1.0f);
    trc_gl_state_state_bool_init1(trace, GL_SAMPLE_COVERAGE_INVERT, false);
    
    trc_gl_state_state_enum_init1(trace, GL_STENCIL_FUNC, GL_ALWAYS);
    trc_gl_state_state_int_init1(trace, GL_STENCIL_REF, 0);
    trc_gl_state_state_int_init1(trace, GL_STENCIL_VALUE_MASK, 0xffffffff);
    trc_gl_state_state_enum_init1(trace, GL_STENCIL_BACK_FUNC, GL_ALWAYS);
    trc_gl_state_state_int_init1(trace, GL_STENCIL_BACK_REF, 0);
    trc_gl_state_state_int_init1(trace, GL_STENCIL_BACK_VALUE_MASK, 0xffffffff);
    trc_gl_state_state_enum_init1(trace, GL_STENCIL_FAIL, GL_KEEP);
    trc_gl_state_state_enum_init1(trace, GL_STENCIL_PASS_DEPTH_PASS, GL_KEEP);
    trc_gl_state_state_enum_init1(trace, GL_STENCIL_PASS_DEPTH_FAIL, GL_KEEP);
    trc_gl_state_state_enum_init1(trace, GL_STENCIL_BACK_FAIL, GL_KEEP);
    trc_gl_state_state_enum_init1(trace, GL_STENCIL_BACK_PASS_DEPTH_PASS, GL_KEEP);
    trc_gl_state_state_enum_init1(trace, GL_STENCIL_BACK_PASS_DEPTH_FAIL, GL_KEEP);
    
    GLenum blenddata[max_draw_buffers];
    for (size_t i = 0; i < max_draw_buffers; i++) blenddata[i] = GL_ONE;
    trc_gl_state_state_enum_init(trace, GL_BLEND_SRC_RGB, max_draw_buffers, blenddata);
    trc_gl_state_state_enum_init(trace, GL_BLEND_SRC_ALPHA, max_draw_buffers, blenddata);
    
    for (size_t i = 0; i < max_draw_buffers; i++) blenddata[i] = GL_ZERO;
    trc_gl_state_state_enum_init(trace, GL_BLEND_DST_RGB, max_draw_buffers, blenddata);
    trc_gl_state_state_enum_init(trace, GL_BLEND_DST_ALPHA, max_draw_buffers, blenddata);
    
    float blend_color[4] = {0.0f, 0.0f, 0.0f, 0.0f};
    trc_gl_state_state_float_init(trace, GL_BLEND_COLOR, 4, blend_color);
    
    for (size_t i = 0; i < max_draw_buffers; i++) blenddata[i] = GL_FUNC_ADD;
    trc_gl_state_state_enum_init(trace, GL_BLEND_EQUATION_RGB, max_draw_buffers, blenddata);
    trc_gl_state_state_enum_init(trace, GL_BLEND_EQUATION_ALPHA, max_draw_buffers, blenddata);
    
    trc_gl_state_set_hints(trace, GL_FRAGMENT_SHADER_DERIVATIVE_HINT, GL_DONT_CARE);
    trc_gl_state_set_hints(trace, GL_LINE_SMOOTH_HINT, GL_DONT_CARE);
    trc_gl_state_set_hints(trace, GL_POLYGON_SMOOTH_HINT, GL_DONT_CARE);
    trc_gl_state_set_hints(trace, GL_TEXTURE_COMPRESSION_HINT, GL_DONT_CARE);
    
    float zerof[max_viewports*4];
    for (size_t i = 0; i < max_viewports*4; i++) zerof[i] = 0.0f;
    trc_gl_state_state_float_init(trace, GL_VIEWPORT, max_viewports*4, zerof);
    trc_gl_state_state_int_init(trace, GL_SCISSOR_BOX, max_viewports*4, NULL);
    float depth_range[max_viewports*2];
    for (int i = 0; i < max_viewports*2; i++) depth_range[i] = (float[]){0.0f, 1.0f}[i%2];
    trc_gl_state_state_float_init(trace, GL_DEPTH_RANGE, max_viewports*2, depth_range);
    
    trc_gl_state_state_enum_init1(trace, GL_PROVOKING_VERTEX, GL_LAST_VERTEX_CONVENTION);
    trc_gl_state_state_enum_init1(trace, GL_LOGIC_OP_MODE, GL_COPY);
    trc_gl_state_state_int_init1(trace, GL_PRIMITIVE_RESTART_INDEX, 0);
    trc_gl_state_state_enum_init1(trace, GL_POLYGON_MODE, GL_FILL);
    trc_gl_state_state_enum_init1(trace, GL_CULL_FACE_MODE, GL_BACK);
    trc_gl_state_state_enum_init1(trace, GL_FRONT_FACE, GL_CCW);
    trc_gl_state_state_enum_init1(trace, GL_DEPTH_FUNC, GL_LESS);
    trc_gl_state_state_enum_init1(trace, GL_LOGIC_OP_MODE, GL_COPY);
    trc_gl_state_state_enum_init1(ctx->trace, GL_CLIP_ORIGIN, GL_LOWER_LEFT);
    trc_gl_state_state_enum_init1(ctx->trace, GL_CLIP_DEPTH_MODE, GL_NEGATIVE_ONE_TO_ONE);
    trc_gl_state_state_float_init1(trace, GL_POINT_FADE_THRESHOLD_SIZE, GL_UPPER_LEFT);
    trc_gl_state_state_enum_init1(trace, GL_POINT_SPRITE_COORD_ORIGIN, GL_UPPER_LEFT);
    trc_gl_state_state_float_init1(trace, GL_MIN_SAMPLE_SHADING_VALUE, 0.0f);
    
    trc_gl_state_state_int_init1(trace, GL_PATCH_VERTICES, 3);
    float one4[4] = {1.0f, 1.0f, 1.0f, 1.0f};
    trc_gl_state_state_float_init(trace, GL_PATCH_DEFAULT_OUTER_LEVEL, 4, one4);
    trc_gl_state_state_float_init(trace, GL_PATCH_DEFAULT_INNER_LEVEL, 2, one4);
    
    double* va = malloc((max_vertex_attribs-1)*4*sizeof(double));
    for (size_t i = 0; i < (max_vertex_attribs-1)*4; i++) va[i] = i%4==3 ? 1 : 0;
    trc_gl_state_state_double_init(trace, GL_CURRENT_VERTEX_ATTRIB, (max_vertex_attribs-1)*4, va);
    free(va);
    
    GLenum draw_buffers[1] = {GL_BACK};
    trc_gl_state_state_enum_init(trace, GL_DRAW_BUFFER, 1, draw_buffers);
    
    GLint max_sample_mask_words;
    F(glGetIntegerv)(GL_MAX_SAMPLE_MASK_WORDS, &max_sample_mask_words);
    int sample_mask_value[max_sample_mask_words];
    memset(sample_mask_value, 0xff, max_sample_mask_words*sizeof(int));
    trc_gl_state_state_int_init1(ctx->trace, GL_MAX_SAMPLE_MASK_WORDS, max_sample_mask_words);
    trc_gl_state_state_int_init(ctx->trace, GL_SAMPLE_MASK_VALUE, max_sample_mask_words, sample_mask_value);
    
    uint draw_vao;
    F(glGenVertexArrays)(1, &draw_vao);
    F(glBindVertexArray)(draw_vao);
    trc_gl_state_set_draw_vao(trace, draw_vao);
    
    rev = *trc_get_context(ctx->trace);
    replay_create_context_buffers(ctx->trace, &rev);
    trc_set_context(ctx->trace, &rev);
    replay_update_fb0_buffers(ctx, true, true, true, true);
}

static void replay_pixel_store(trc_replay_context_t* ctx, trace_command_t* cmd, GLenum pname, GLint param) {
    switch (pname) {
    case GL_PACK_SWAP_BYTES:
    case GL_PACK_LSB_FIRST:
    case GL_UNPACK_SWAP_BYTES:
    case GL_UNPACK_LSB_FIRST: trc_gl_state_set_state_bool(ctx->trace, pname, 0, param!=0); break;
    case GL_PACK_ROW_LENGTH:
    case GL_PACK_IMAGE_HEIGHT:
    case GL_PACK_SKIP_ROWS:
    case GL_PACK_SKIP_PIXELS:
    case GL_PACK_SKIP_IMAGES:
    case GL_UNPACK_ROW_LENGTH:
    case GL_UNPACK_IMAGE_HEIGHT:
    case GL_UNPACK_SKIP_ROWS:
    case GL_UNPACK_SKIP_PIXELS:
    case GL_UNPACK_SKIP_IMAGES:
        if (param < 0) ERROR2(, "param is negative");
        trc_gl_state_set_state_int(ctx->trace, pname, 0, param);
        break;
    case GL_PACK_ALIGNMENT:
    case GL_UNPACK_ALIGNMENT:
        if (param!=1 && param!=2 && param!=4 && param!=8)
            ERROR2(, "param is not 1, 2, 4, or 8");
        trc_gl_state_set_state_int(ctx->trace, pname, 0, param);
        break;
    default:
        ERROR2(, "Invalid parameter");
    }
    F(glPixelStorei)(pname, param);
}

static void replay_set_texture_image(trace_t* trace, const trc_gl_texture_rev_t* rev, uint level, uint face,
                                     uint internal_format, uint width, uint height, uint depth,
                                     trc_image_format_t format, trc_data_t* data) {
    trc_gl_texture_image_t img;
    memset(&img, 0, sizeof(img)); //Fill in padding to fix use of uninitialized memory errors because of compression
    img.face = face;
    img.level = level;
    img.internal_format = internal_format;
    img.width = width;
    img.height = height;
    img.depth = depth;
    img.data_format = format;
    img.data = data;
    
    size_t img_count = rev->images->size / sizeof(trc_gl_texture_image_t);
    trc_gl_texture_image_t* newimages = malloc((img_count+1)*sizeof(trc_gl_texture_image_t));
    
    trc_gl_texture_image_t* images = trc_map_data(rev->images, TRC_MAP_READ);
    bool replaced = false;
    for (size_t i = 0; i < img_count; i++) {
        if (images[i].face==img.face && images[i].level==img.level) {
            newimages[i] = img;
            replaced = true;
        } else {
            newimages[i] = images[i];
        }
    }
    trc_unmap_data(rev->images);
    
    trc_gl_texture_rev_t newrev = *rev;
    if (!replaced) newimages[img_count++] = img;
    
    size_t size = img_count * sizeof(trc_gl_texture_image_t);
    newrev.images = trc_create_data_no_copy(trace, size, newimages, TRC_DATA_IMMUTABLE);
    
    set_texture(&newrev);
}

static trc_image_format_t get_image_format(GLenum internal_format) {
    switch (internal_format) {
    case GL_DEPTH_COMPONENT: return TrcImageFormat_Red_F32;
    case GL_DEPTH_COMPONENT16: return TrcImageFormat_Red_F32;
    case GL_DEPTH_COMPONENT24: return TrcImageFormat_Red_F32;
    case GL_DEPTH_COMPONENT32: return TrcImageFormat_Red_F32;
    case GL_DEPTH_COMPONENT32F: return TrcImageFormat_Red_F32;
    case GL_DEPTH_STENCIL: return TrcImageFormat_F32_U24_U8;
    case GL_DEPTH24_STENCIL8: return TrcImageFormat_F32_U24_U8;
    case GL_DEPTH32F_STENCIL8: return TrcImageFormat_F32_U24_U8;
    case GL_STENCIL_INDEX: return TrcImageFormat_Red_U32;
    case GL_STENCIL_INDEX1: return TrcImageFormat_Red_U32;
    case GL_STENCIL_INDEX4: return TrcImageFormat_Red_U32;
    case GL_STENCIL_INDEX8: return TrcImageFormat_Red_U32;
    case GL_STENCIL_INDEX16: return TrcImageFormat_Red_U32;
    case GL_RED: return TrcImageFormat_Red_F32;
    case GL_RG: return TrcImageFormat_RedGreen_F32;
    case GL_RGB: return TrcImageFormat_RGB_F32;
    case GL_RGBA: return TrcImageFormat_RGBA_F32;
    case GL_R8: return TrcImageFormat_Red_F32;
    case GL_R8_SNORM: return TrcImageFormat_Red_F32;
    case GL_R16: return TrcImageFormat_Red_F32;
    case GL_R16_SNORM: return TrcImageFormat_Red_F32;
    case GL_RG8: return TrcImageFormat_RedGreen_F32;
    case GL_RG8_SNORM: return TrcImageFormat_RedGreen_F32;
    case GL_RG16: return TrcImageFormat_RedGreen_F32;
    case GL_RG16_SNORM: return TrcImageFormat_RedGreen_F32;
    case GL_R3_G3_B2: return TrcImageFormat_RGB_F32;
    case GL_RGB4: return TrcImageFormat_RGB_F32;
    case GL_RGB5: return TrcImageFormat_RGB_F32;
    case GL_RGB8: return TrcImageFormat_RGB_F32;
    case GL_RGB8_SNORM: return TrcImageFormat_RGB_F32;
    case GL_RGB10: return TrcImageFormat_RGB_F32;
    case GL_RGB12: return TrcImageFormat_RGB_F32;
    case GL_RGB16_SNORM: return TrcImageFormat_RGB_F32;
    case GL_RGBA2: return TrcImageFormat_RGBA_F32;
    case GL_RGBA4: return TrcImageFormat_RGBA_F32;
    case GL_RGB5_A1: return TrcImageFormat_RGBA_F32;
    case GL_RGBA8: return TrcImageFormat_RGBA_F32;
    case GL_RGBA8_SNORM: return TrcImageFormat_RGBA_F32;
    case GL_RGB10_A2: return TrcImageFormat_RGBA_F32;
    case GL_RGB10_A2UI: return TrcImageFormat_RGBA_U32;
    case GL_RGBA12: return TrcImageFormat_RGBA_F32;
    case GL_RGBA16: return TrcImageFormat_RGBA_F32;
    case GL_SRGB8: return TrcImageFormat_SRGB_U8;
    case GL_SRGB8_ALPHA8: return TrcImageFormat_SRGBA_U8;
    case GL_R16F: return TrcImageFormat_Red_F32;
    case GL_RG16F: return TrcImageFormat_RedGreen_F32;
    case GL_RGB16F: return TrcImageFormat_RGB_F32;
    case GL_RGBA16F: return TrcImageFormat_RGBA_F32;
    case GL_R32F: return TrcImageFormat_Red_F32;
    case GL_RG32F: return TrcImageFormat_RedGreen_F32;
    case GL_RGB32F: return TrcImageFormat_RGB_F32;
    case GL_RGBA32F: return TrcImageFormat_RGBA_F32;
    case GL_R11F_G11F_B10F: return TrcImageFormat_RGB_F32;
    case GL_RGB9_E5: return TrcImageFormat_RGB_F32;
    case GL_R8I: return TrcImageFormat_Red_I32;
    case GL_R8UI: return TrcImageFormat_Red_U32;
    case GL_R16I: return TrcImageFormat_Red_I32;
    case GL_R16UI: return TrcImageFormat_Red_U32;
    case GL_R32I: return TrcImageFormat_Red_I32;
    case GL_R32UI: return TrcImageFormat_Red_U32;
    case GL_RG8I: return TrcImageFormat_RedGreen_I32;
    case GL_RG8UI: return TrcImageFormat_RedGreen_U32;
    case GL_RG16I: return TrcImageFormat_RedGreen_I32;
    case GL_RG16UI: return TrcImageFormat_RedGreen_U32;
    case GL_RG32I: return TrcImageFormat_RedGreen_I32;
    case GL_RG32UI: return TrcImageFormat_RedGreen_U32;
    case GL_RGB8I: return TrcImageFormat_RGB_I32;
    case GL_RGB8UI: return TrcImageFormat_RGB_U32;
    case GL_RGB16I: return TrcImageFormat_RGB_I32;
    case GL_RGB16UI: return TrcImageFormat_RGB_U32;
    case GL_RGB32I: return TrcImageFormat_RGB_I32;
    case GL_RGB32UI: return TrcImageFormat_RGB_U32;
    case GL_RGBA8I: return TrcImageFormat_RGBA_I32;
    case GL_RGBA8UI: return TrcImageFormat_RGBA_U32;
    case GL_RGBA16I: return TrcImageFormat_RGBA_I32;
    case GL_RGBA16UI: return TrcImageFormat_RGBA_U32;
    case GL_RGBA32I: return TrcImageFormat_RGBA_I32;
    case GL_RGBA32UI: return TrcImageFormat_RGBA_U32;
    case GL_COMPRESSED_RED: return TrcImageFormat_Red_F32;
    case GL_COMPRESSED_RG: return TrcImageFormat_RedGreen_F32;
    case GL_COMPRESSED_RGB: return TrcImageFormat_RGB_F32;
    case GL_COMPRESSED_RGBA: return TrcImageFormat_RGBA_F32;
    case GL_COMPRESSED_SRGB: return TrcImageFormat_SRGB_U8;
    case GL_COMPRESSED_SRGB_ALPHA: return TrcImageFormat_SRGBA_U8;
    case GL_COMPRESSED_RED_RGTC1: return TrcImageFormat_Red_F32;
    case GL_COMPRESSED_SIGNED_RED_RGTC1: return TrcImageFormat_Red_F32;
    case GL_COMPRESSED_RG_RGTC2: return TrcImageFormat_RedGreen_F32;
    case GL_COMPRESSED_SIGNED_RG_RGTC2: return TrcImageFormat_RedGreen_F32;
    case GL_COMPRESSED_RGBA_BPTC_UNORM: return TrcImageFormat_RGBA_F32;
    case GL_COMPRESSED_SRGB_ALPHA_BPTC_UNORM: return TrcImageFormat_SRGBA_U8;
    case GL_COMPRESSED_RGB_BPTC_SIGNED_FLOAT: return TrcImageFormat_RGB_F32;
    case GL_COMPRESSED_RGB_BPTC_UNSIGNED_FLOAT: return TrcImageFormat_RGB_F32;
    default: assert(false);
    }
}

static void save_init_packing_config(trc_replay_context_t* ctx, GLint temp[9]) {
    F(glGetIntegerv)(GL_PACK_SWAP_BYTES, &temp[0]);
    F(glGetIntegerv)(GL_PACK_LSB_FIRST, &temp[1]);
    F(glGetIntegerv)(GL_PACK_ROW_LENGTH, &temp[2]);
    F(glGetIntegerv)(GL_PACK_IMAGE_HEIGHT, &temp[3]);
    F(glGetIntegerv)(GL_PACK_SKIP_ROWS, &temp[4]);
    F(glGetIntegerv)(GL_PACK_SKIP_PIXELS, &temp[5]);
    F(glGetIntegerv)(GL_PACK_SKIP_IMAGES, &temp[6]);
    F(glGetIntegerv)(GL_PACK_ALIGNMENT, &temp[7]);
    F(glGetIntegerv)(GL_PIXEL_PACK_BUFFER_BINDING, &temp[8]);
    F(glPixelStorei)(GL_PACK_SWAP_BYTES, 0);
    F(glPixelStorei)(GL_PACK_LSB_FIRST, 0);
    F(glPixelStorei)(GL_PACK_ROW_LENGTH, 0);
    F(glPixelStorei)(GL_PACK_IMAGE_HEIGHT, 0);
    F(glPixelStorei)(GL_PACK_SKIP_ROWS, 0);
    F(glPixelStorei)(GL_PACK_SKIP_PIXELS, 0);
    F(glPixelStorei)(GL_PACK_SKIP_IMAGES, 0);
    F(glPixelStorei)(GL_PACK_ALIGNMENT, 1);
    F(glBindBuffer)(GL_PIXEL_PACK_BUFFER, 0);
}

static void restore_packing_config(trc_replay_context_t* ctx, GLint temp[9]) {
    F(glPixelStorei)(GL_PACK_SWAP_BYTES, temp[0]);
    F(glPixelStorei)(GL_PACK_LSB_FIRST, temp[1]);
    F(glPixelStorei)(GL_PACK_ROW_LENGTH, temp[2]);
    F(glPixelStorei)(GL_PACK_IMAGE_HEIGHT, temp[3]);
    F(glPixelStorei)(GL_PACK_SKIP_ROWS, temp[4]);
    F(glPixelStorei)(GL_PACK_SKIP_PIXELS, temp[5]);
    F(glPixelStorei)(GL_PACK_SKIP_IMAGES, temp[6]);
    F(glPixelStorei)(GL_PACK_ALIGNMENT, temp[7]);
    F(glBindBuffer)(GL_PIXEL_PACK_BUFFER, temp[8]);
}

static void replay_update_tex_image(trc_replay_context_t* ctx, const trc_gl_texture_rev_t* tex,
                                    uint level, uint face) {
    GLenum prevget;
    switch (tex->type) {
    case GL_TEXTURE_1D: prevget = GL_TEXTURE_BINDING_1D; break;
    case GL_TEXTURE_2D: prevget = GL_TEXTURE_BINDING_2D; break;
    case GL_TEXTURE_3D: prevget = GL_TEXTURE_BINDING_3D; break;
    case GL_TEXTURE_1D_ARRAY: prevget = GL_TEXTURE_BINDING_1D_ARRAY; break;
    case GL_TEXTURE_2D_ARRAY: prevget = GL_TEXTURE_BINDING_2D_ARRAY; break;
    case GL_TEXTURE_RECTANGLE: prevget = GL_TEXTURE_BINDING_RECTANGLE; break;
    case GL_TEXTURE_CUBE_MAP: prevget = GL_TEXTURE_BINDING_CUBE_MAP; break;
    case GL_TEXTURE_CUBE_MAP_ARRAY: prevget = GL_TEXTURE_BINDING_CUBE_MAP; break;
    case GL_TEXTURE_BUFFER: return; //TODO: Error
    case GL_TEXTURE_2D_MULTISAMPLE: return; //TODO: Handle
    case GL_TEXTURE_2D_MULTISAMPLE_ARRAY: return; //TODO: Handle
    }
    GLint prev;
    F(glGetIntegerv)(prevget, &prev);
    F(glBindTexture)(tex->type, tex->real);
    
    GLint width, height, depth, internal_format;
    F(glGetTexLevelParameteriv)(tex->type, level, GL_TEXTURE_WIDTH, &width);
    F(glGetTexLevelParameteriv)(tex->type, level, GL_TEXTURE_HEIGHT, &height);
    F(glGetTexLevelParameteriv)(tex->type, level, GL_TEXTURE_DEPTH, &depth);
    F(glGetTexLevelParameteriv)(tex->type, level, GL_TEXTURE_INTERNAL_FORMAT, &internal_format);
    if (!width) width = 1;
    if (!height) height = 1;
    if (!depth) depth = 1;
    
    trc_image_format_t image_format = get_image_format(internal_format);
    
    GLenum format, type;
    size_t pixel_size;
    switch (image_format) {
    case TrcImageFormat_Red_U32: pixel_size = 4; format = GL_RED; type = GL_UNSIGNED_INT; break;
    case TrcImageFormat_RedGreen_U32: pixel_size = 8; format = GL_RG; type = GL_UNSIGNED_INT; break;
    case TrcImageFormat_RGB_U32: pixel_size = 12; format = GL_RGB; type = GL_UNSIGNED_INT; break;
    case TrcImageFormat_RGBA_U32: pixel_size = 16; format = GL_RGBA; type = GL_UNSIGNED_INT; break;
    case TrcImageFormat_Red_I32: pixel_size = 4; format = GL_RED; type = GL_INT; break;
    case TrcImageFormat_RedGreen_I32: pixel_size = 8; format = GL_RG; type = GL_INT; break;
    case TrcImageFormat_RGB_I32: pixel_size = 12; format = GL_RGB; type = GL_INT; break;
    case TrcImageFormat_RGBA_I32: pixel_size = 16; format = GL_RGBA; type = GL_INT; break;
    case TrcImageFormat_Red_F32: pixel_size = 4; format = GL_RED; type = GL_FLOAT; break;
    case TrcImageFormat_RedGreen_F32: pixel_size = 8; format = GL_RG; type = GL_FLOAT; break;
    case TrcImageFormat_RGB_F32: pixel_size = 12; format = GL_RGB; type = GL_FLOAT; break;
    case TrcImageFormat_RGBA_F32: pixel_size = 16; format = GL_RGBA; type = GL_FLOAT; break;
    case TrcImageFormat_SRGB_U8: pixel_size = 3; format = GL_RGB; type = GL_UNSIGNED_BYTE; break;
    case TrcImageFormat_SRGBA_U8: pixel_size = 4; format = GL_RGBA; type = GL_UNSIGNED_BYTE; break;
    case TrcImageFormat_F32_U24_U8: pixel_size = 8; format = GL_DEPTH_STENCIL; type = GL_FLOAT_32_UNSIGNED_INT_24_8_REV; break;
    case TrcImageFormat_RGBA_U8: pixel_size = 4; format = GL_RGBA; type = GL_UNSIGNED_BYTE; break;
    }
    switch (internal_format) {
    case GL_DEPTH_COMPONENT: format = GL_DEPTH_COMPONENT; break;
    case GL_DEPTH_COMPONENT16: format = GL_DEPTH_COMPONENT; break;
    case GL_DEPTH_COMPONENT24: format = GL_DEPTH_COMPONENT; break;
    case GL_DEPTH_COMPONENT32: format = GL_DEPTH_COMPONENT; break;
    case GL_DEPTH_COMPONENT32F: format = GL_DEPTH_COMPONENT; break;
    case GL_STENCIL_INDEX: format = GL_STENCIL_INDEX; break;
    case GL_STENCIL_INDEX1: format = GL_STENCIL_INDEX; break;
    case GL_STENCIL_INDEX4: format = GL_STENCIL_INDEX; break;
    case GL_STENCIL_INDEX8: format = GL_STENCIL_INDEX; break;
    case GL_STENCIL_INDEX16: format = GL_STENCIL_INDEX; break;
    default: break;
    }
    
    size_t data_size = width * height * depth * pixel_size;
    trc_data_t* data = trc_create_data(ctx->trace, data_size, NULL, TRC_DATA_NO_ZERO);
    void* dest = trc_map_data(data, TRC_MAP_REPLACE);
    
    uint target = tex->type;
    if (target==GL_TEXTURE_CUBE_MAP) target = GL_TEXTURE_CUBE_MAP_POSITIVE_X + face;
    
    GLint temp[9];
    save_init_packing_config(ctx, temp);
    F(glGetTexImage)(target, level, format, type, dest);
    restore_packing_config(ctx, temp);
    
    F(glBindTexture)(tex->type, prev);
    
    trc_unmap_freeze_data(ctx->trace, data);
    
    replay_set_texture_image(ctx->trace, tex, level, face, internal_format,
                             width, height, depth, image_format, data);
}

static const trc_gl_texture_rev_t* replay_get_bound_tex(trc_replay_context_t* ctx, uint target) {
    uint unit = trc_gl_state_get_active_texture_unit(ctx->trace);
    return trc_obj_get_rev(trc_gl_state_get_bound_textures(ctx->trace, target, unit), -1);
}

static void replay_update_bound_tex_image(trc_replay_context_t* ctx, trace_command_t* cmd, uint target, uint level) {
    const trc_gl_texture_rev_t* rev = replay_get_bound_tex(ctx, target);
    if (!rev) ERROR2(, "No texture bound to target");
    
    uint face = 0;
    if (target>=GL_TEXTURE_CUBE_MAP_POSITIVE_X && target<=GL_TEXTURE_CUBE_MAP_NEGATIVE_Z)
        face = target - GL_TEXTURE_CUBE_MAP_POSITIVE_X;
    replay_update_tex_image(ctx, rev, level, face);
}

static bool tex_buffer(trc_replay_context_t* ctx, trace_command_t* cmd, GLuint tex_or_target, bool dsa,
                       GLenum internalformat, GLuint buffer, GLintptr offset, GLsizeiptr size) {
    const trc_gl_texture_rev_t* rev = NULL;
    if (!dsa) rev = replay_get_bound_tex(ctx, tex_or_target);
    else rev = get_texture(ctx->ns, tex_or_target);
    if (!rev) ERROR2(false, dsa?"Invalid texture name":"No texture bound to target");
    if (!rev->has_object) ERROR2(false, "Texture name has no object");
    
    const trc_gl_buffer_rev_t* buffer_rev = buffer ? get_buffer(ctx->ns, buffer) : NULL;
    if (!buffer_rev && buffer) ERROR2(false, "Invalid buffer name");
    if (buffer && !buffer_rev->has_object) ERROR2(false, "Buffer name has no object");
    if (offset<0 || size<=0 || offset+size>buffer_rev->data->size) ERROR2(false, "Invalid range");
    //TODO: Check alignment
    
    trc_gl_texture_rev_t newrev = *rev;
    newrev.buffer.internal_format = internalformat;
    newrev.buffer.buffer = buffer;
    newrev.buffer.offset = offset;
    newrev.buffer.size = buffer ? (size<0?buffer_rev->data->size:size) : 0;
    set_texture(&newrev);
    
    return true;
}

static bool replay_append_fb_attachment(trc_replay_context_t* ctx, trace_command_t* cmd, bool dsa, trc_obj_t* fb, const trc_gl_framebuffer_attachment_t* attach) {
    const trc_gl_framebuffer_rev_t* rev = trc_obj_get_rev(fb, -1);
    if (!rev) ERROR2(false, dsa?"Invalid framebuffer name":"No framebuffer bound to target");
    if (!rev->has_object) ERROR2(false, "Framebuffer name has no object");
    
    size_t attach_count = rev->attachments->size / sizeof(trc_gl_framebuffer_attachment_t);
    trc_gl_framebuffer_attachment_t* newattachs = malloc((attach_count+1)*sizeof(trc_gl_framebuffer_attachment_t));
    
    trc_gl_framebuffer_attachment_t* attachs = trc_map_data(rev->attachments, TRC_MAP_READ);
    bool replaced = false;
    for (size_t i = 0; i < attach_count; i++) {
        if (attachs[i].attachment == attach->attachment) {
            newattachs[i] = *attach;
            replaced = true;
        } else {
            newattachs[i] = attachs[i];
        }
    }
    trc_unmap_data(rev->attachments);
    
    trc_gl_framebuffer_rev_t newrev = *rev;
    if (!replaced) newattachs[attach_count++] = *attach;
    
    size_t size = attach_count * sizeof(trc_gl_framebuffer_attachment_t);
    newrev.attachments = trc_create_data_no_copy(ctx->trace, size, newattachs, TRC_DATA_IMMUTABLE);
    
    set_framebuffer(&newrev);
    
    return true;
}

static bool add_fb_attachment(trc_replay_context_t* ctx, trace_command_t* cmd, trc_obj_t* fb, uint attachment, bool dsa,
                              uint fake_tex, const trc_gl_texture_rev_t* tex, uint target, uint level, uint layer) {
    bool cubemap = target>=GL_TEXTURE_CUBE_MAP_POSITIVE_X && target<=GL_TEXTURE_CUBE_MAP_NEGATIVE_Z;
    
    if (!tex && fake_tex) ERROR2(false, "Invalid texture name");
    if (fake_tex && !tex->has_object) ERROR2(false, "Texture name has no object");
    if (tex && (cubemap?GL_TEXTURE_CUBE_MAP:target) != tex->type)
        ERROR2(false, "Incompatible target for texture\n");
    
    if (tex) {
        size_t image_count = tex->images->size / sizeof(trc_gl_texture_image_t);
        trc_gl_texture_image_t* images = trc_map_data(tex->images, TRC_MAP_READ);
        bool found = false;
        for (size_t i = 0; i < image_count; i++) if (images[i].level == level) found = true;;
        trc_unmap_data(tex->images);
        if (!found) ERROR2(false, "No such level for texture");
    }
    
    trc_gl_framebuffer_attachment_t attach;
    memset(&attach, 0, sizeof(attach));
    attach.has_renderbuffer = false;
    attach.attachment = attachment;
    trc_set_obj_ref(&attach.renderbuffer, NULL);
    trc_set_obj_ref(&attach.texture, tex->head.obj);
    attach.level = level;
    attach.layer = layer;
    attach.face = 0;
    if (cubemap) {
        attach.face = target - GL_TEXTURE_CUBE_MAP_POSITIVE_X;
    } else if (target==GL_TEXTURE_CUBE_MAP_ARRAY || target==GL_TEXTURE_CUBE_MAP) {
        attach.face = layer % 6;
        attach.layer /= 6;
    }
    return replay_append_fb_attachment(ctx, cmd, dsa, fb, &attach);
}

static bool add_fb_attachment_rb(trc_replay_context_t* ctx, trace_command_t* cmd, bool dsa, trc_obj_t* fb,
                                 uint attachment, uint fake_rb, const trc_gl_renderbuffer_rev_t* rb) {
    if (!rb && fake_rb) ERROR2(false, "Invalid renderbuffer name");
    if (fake_rb && !rb->has_object) ERROR2(false, "Renderbuffer name has no object");
    trc_gl_framebuffer_attachment_t attach;
    memset(&attach, 0, sizeof(attach));
    attach.has_renderbuffer = true;
    attach.attachment = attachment;
    trc_set_obj_ref(&attach.texture, NULL);
    trc_set_obj_ref(&attach.renderbuffer, rb->head.obj);
    return replay_append_fb_attachment(ctx, cmd, dsa, fb, &attach);
}

static void replay_update_renderbuffer(trc_replay_context_t* ctx, const trc_gl_renderbuffer_rev_t* rev,
                                       uint width, uint height, uint internal_format, uint samples) {
    GLint bits[6];
    GLint prev;
    F(glGetIntegerv)(GL_RENDERBUFFER_BINDING, &prev);
    F(glBindRenderbuffer)(GL_RENDERBUFFER, rev->real);
    F(glGetRenderbufferParameteriv)(GL_RENDERBUFFER, GL_RENDERBUFFER_RED_SIZE, &bits[0]);
    F(glGetRenderbufferParameteriv)(GL_RENDERBUFFER, GL_RENDERBUFFER_GREEN_SIZE, &bits[1]);
    F(glGetRenderbufferParameteriv)(GL_RENDERBUFFER, GL_RENDERBUFFER_BLUE_SIZE, &bits[2]);
    F(glGetRenderbufferParameteriv)(GL_RENDERBUFFER, GL_RENDERBUFFER_ALPHA_SIZE, &bits[3]);
    F(glGetRenderbufferParameteriv)(GL_RENDERBUFFER, GL_RENDERBUFFER_DEPTH_SIZE, &bits[4]);
    F(glGetRenderbufferParameteriv)(GL_RENDERBUFFER, GL_RENDERBUFFER_STENCIL_SIZE, &bits[5]);
    F(glBindRenderbuffer)(GL_RENDERBUFFER, prev);
    
    trc_gl_renderbuffer_rev_t newrev = *rev;
    newrev.width = width;
    newrev.height = height;
    newrev.internal_format = internal_format;
    newrev.sample_count = samples;
    for (size_t i = 0; i < 4; i++) newrev.rgba_bits[i] = bits[i];
    newrev.depth_bits = bits[4];
    newrev.stencil_bits = bits[5];
    newrev.has_storage = true;
    
    set_renderbuffer(&newrev);
}

static bool texture_param_double(trc_replay_context_t* ctx, trace_command_t* cmd, bool dsa,
                                 GLuint tex_or_target, GLenum param, uint32_t count, const double* val) {
    const trc_gl_texture_rev_t* rev = NULL;
    if (dsa) rev = get_texture(ctx->ns, tex_or_target);
    else rev = replay_get_bound_tex(ctx, tex_or_target);
    if (!rev) ERROR2(true, dsa?"Invalid texture name":"No texture bound to target");
    if (!rev->has_object) ERROR2(true, "Texture name has no object");
    trc_gl_texture_rev_t newrev = *rev;
    
    bool rectangle = rev->type==GL_TEXTURE_RECTANGLE;
    bool multisampled = rev->type==GL_TEXTURE_2D_MULTISAMPLE || rev->type==GL_TEXTURE_2D_MULTISAMPLE_ARRAY;
    
    if (rectangle && param==GL_TEXTURE_MIN_FILTER && count==1) {
        if (val[0]!=GL_NEAREST && val[0]!=GL_LINEAR)
            ERROR2(true, "Mipmapping is not supported for rectangle textures");
    }
    
    if (rectangle && (param==GL_TEXTURE_WRAP_S || param==GL_TEXTURE_WRAP_T) && count==1) {
        if (val[0]==GL_MIRROR_CLAMP_TO_EDGE || val[0]==GL_MIRRORED_REPEAT || val[0]==GL_REPEAT)
            ERROR2(true, "Provided wrap mode is not supported for rectangle textures");
    }
    
    switch (param) {
    case GL_TEXTURE_MIN_FILTER:
    case GL_TEXTURE_MAG_FILTER:
    case GL_TEXTURE_MIN_LOD:
    case GL_TEXTURE_MAX_LOD:
    case GL_TEXTURE_WRAP_S:
    case GL_TEXTURE_WRAP_T:
    case GL_TEXTURE_WRAP_R:
    case GL_TEXTURE_COMPARE_MODE:
    case GL_TEXTURE_COMPARE_FUNC:
    case GL_TEXTURE_BORDER_COLOR:
    case GL_TEXTURE_LOD_BIAS:
    case GL_TEXTURE_MAX_ANISOTROPY: {
        if (multisampled)
            ERROR2(true, "Sampler parameters cannot be set for multisampled textures");
        bool res = sample_param_double(cmd, &newrev.sample_params, param, count, val);
        set_texture(&newrev);
        return res;
    }
    case GL_DEPTH_STENCIL_TEXTURE_MODE:
    case GL_TEXTURE_BASE_LEVEL:
    case GL_TEXTURE_MAX_LEVEL:
    case GL_TEXTURE_SWIZZLE_R:
    case GL_TEXTURE_SWIZZLE_G:
    case GL_TEXTURE_SWIZZLE_B:
    case GL_TEXTURE_SWIZZLE_A: {
        if (count != 1) ERROR2(true, "Expected 1 value. Got %u", count);
        break;
    }
    case GL_TEXTURE_SWIZZLE_RGBA: {
        if (count != 4) ERROR2(true, "Expected 4 values. Got %u", count);
        break;
    }
    }
    
    switch (param) {
    case GL_TEXTURE_BASE_LEVEL:
        if ((multisampled||rev->type==GL_TEXTURE_RECTANGLE) && val[0]!=0.0)
            ERROR2(true, "Parameter value must be zero due to the texture's type");
    case GL_TEXTURE_MAX_LEVEL:
        if (val[0] < 0.0) ERROR2(true, "Parameter value must be nonnegative");
        break;
    case GL_DEPTH_STENCIL_TEXTURE_MODE:
        if (val[0]!=GL_DEPTH_COMPONENT && val[0]!=GL_STENCIL_INDEX)
            ERROR2(true, "Invalid depth stencil texture mode");
        break;
    case GL_TEXTURE_SWIZZLE_R:
    case GL_TEXTURE_SWIZZLE_G:
    case GL_TEXTURE_SWIZZLE_B:
    case GL_TEXTURE_SWIZZLE_A:
        if (val[0]!=GL_RED && val[0]!=GL_GREEN && val[0]!=GL_BLUE && val[0]!=GL_ALPHA)
            ERROR2(true, "Invalid swizzle");
        break;
    case GL_TEXTURE_SWIZZLE_RGBA:
        for (uint i = 0; i < 4; i++) {
            if (val[0]!=GL_RED && val[0]!=GL_GREEN && val[0]!=GL_BLUE && val[0]!=GL_ALPHA)
                ERROR2(true, "Invalid swizzle");
        }
        break;
    }
    
    switch (param) {
    case GL_DEPTH_STENCIL_TEXTURE_MODE:
        newrev.depth_stencil_mode = val[0];
        break;
    case GL_TEXTURE_BASE_LEVEL: newrev.base_level = val[0]; break;
    case GL_TEXTURE_MAX_LEVEL: newrev.max_level = val[0]; break;
    case GL_TEXTURE_SWIZZLE_R: newrev.swizzle[0] = val[0]; break;
    case GL_TEXTURE_SWIZZLE_G: newrev.swizzle[1] = val[0]; break;
    case GL_TEXTURE_SWIZZLE_B: newrev.swizzle[2] = val[0]; break;
    case GL_TEXTURE_SWIZZLE_A: newrev.swizzle[3] = val[0]; break;
    case GL_TEXTURE_SWIZZLE_RGBA:
        for (uint i = 0; i < 4; i++) newrev.swizzle[i] = val[i];
        break;
    }
    
    set_texture(&newrev);
    
    return false;
}

static trc_obj_t* get_bound_buffer(trc_replay_context_t* ctx, GLenum target) {
    return trc_gl_state_get_bound_buffer(ctx->trace, target);
}

static trc_obj_t* get_active_program_for_stage(trc_replay_context_t* ctx, GLenum stage) {
    trc_obj_t* program = trc_gl_state_get_bound_program(ctx->trace);
    if (program) return program;
    trc_obj_t* pipeline = trc_gl_state_get_bound_pipeline(ctx->trace);
    if (!pipeline) return NULL;
    const trc_gl_program_pipeline_rev_t* rev = trc_obj_get_rev(pipeline, -1); 
    switch (stage) {
    case GL_VERTEX_SHADER: return rev->vertex_program.obj;
    case GL_FRAGMENT_SHADER: return rev->fragment_program.obj;
    case GL_GEOMETRY_SHADER: return rev->geometry_program.obj;
    case GL_TESS_CONTROL_SHADER: return rev->tess_control_program.obj;
    case GL_TESS_EVALUATION_SHADER: return rev->tess_eval_program.obj;
    case GL_COMPUTE_SHADER: return rev->compute_program.obj;
    default: return NULL;
    }
}

static trc_obj_t* get_active_program(trc_replay_context_t* ctx) {
    trc_obj_t* program = trc_gl_state_get_bound_program(ctx->trace);
    if (program) return program;
    trc_obj_t* pipeline = trc_gl_state_get_bound_pipeline(ctx->trace);
    if (!pipeline) return NULL;
    const trc_gl_program_pipeline_rev_t* rev = trc_obj_get_rev(pipeline, -1); 
    return rev->active_program.obj;
}

#define D(e, t) case e: *(t*)data = val; return 1 + (t*)data;
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

static int uniform(trc_replay_context_t* ctx, trace_command_t* cmd, bool dsa,
                   bool array, uint dimx, uint dimy, GLenum type, void* data_,
                   uint* realprogram) {
    uint arg_pos = 0;
    const trc_gl_program_rev_t* rev;
    if (dsa) rev = get_program(ctx->ns, gl_param_GLuint(cmd, arg_pos++));
    else rev = trc_obj_get_rev(get_active_program(ctx), -1);
    if (!rev) ERROR2(-1, dsa?"Invalid program":"No active program");
    if (realprogram) *realprogram = rev->real;
    
    int location = gl_param_GLint(cmd, arg_pos++);
    
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
    trc_unmap_data(rev->uniforms);
    return -1;
    success1: ;
    
    uint count = array ? gl_param_GLsizei(cmd, arg_pos++) : 1;
    bool transpose = dimy==1 ? false : gl_param_GLboolean(cmd, arg_pos++);
    
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
    trc_unmap_data(rev->uniforms);
    return -1;
    success2: ;
    
    if (uniform.parent!=0xffffffff && uniforms[uniform.parent].dtype.base!=TrcUniformBaseType_Array && count>1) {
        trc_unmap_data(rev->uniforms);
        return -1;
    }
    
    size_t array_size = 1;
    for (uint u = uniform_index; uniforms[u].next!=0xffffffff; u = uniforms[u].next) array_size++;
    if (count!=array_size || uniform.dtype.dim[0]!=dimx || uniform.dtype.dim[1]!=dimy) {
        trc_unmap_data(rev->uniforms);
        return -1;
    }
    
    trc_gl_program_rev_t newrev = *rev;
    
    uint8_t* old_data = trc_map_data(rev->uniform_data, TRC_MAP_READ);
    newrev.uniform_data = trc_create_data(ctx->trace, rev->uniform_data->size, old_data, 0);
    trc_unmap_data(rev->uniform_data);
    uint8_t* data = trc_map_data(newrev.uniform_data, TRC_MAP_MODIFY) + uniform.data_offset;
    
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
                        data = write_uniform_value_double(data, uniform.dtype.base, gl_param_GLfloat(cmd, arg_pos++));
                        break;
                    case GL_DOUBLE:
                        data = write_uniform_value_double(data, uniform.dtype.base, gl_param_GLdouble(cmd, arg_pos++));
                        break;
                    case GL_INT:
                        data = write_uniform_value_int64(data, uniform.dtype.base, gl_param_GLint(cmd, arg_pos++));
                        break;
                    case GL_UNSIGNED_INT:
                        data = write_uniform_value_uint64(data, uniform.dtype.base, gl_param_GLuint(cmd, arg_pos++));
                        break;
                    }
                }
            }
        }
        if (count > 1) uniform = uniforms[uniform.next];
    }
    
    trc_unmap_freeze_data(ctx->trace, newrev.uniform_data);
    trc_unmap_data(rev->uniforms);
    
    set_program(&newrev);
    
    return uniform.real_loc;
}

static void validate_get_uniform(trc_replay_context_t* ctx, trace_command_t* cmd) {
    //TODO: Don't use glGetProgramiv to get the link status
    GLuint fake = gl_param_GLuint(cmd, 0);
    GLuint real_program = get_real_program(ctx->ns, fake);
    if (!real_program) ERROR2(, "No such program.");
    GLint status;
    F(glGetProgramiv)(real_program, GL_LINK_STATUS, &status);
    if (!status) ERROR2(, "Program not successfully linked.");
}

//type in [GL_FLOAT, GL_DOUBLE, GL_UNSIGNED_BYTE, GL_BYTE, GL_SHORT, GL_UNSIGNED_SHORT, GL_UNSIGNED_INT, GL_INT]
//internal in [GL_FLOAT, GL_DOUBLE, GL_UNSIGNED_INT, GL_INT]
static void vertex_attrib(trc_replay_context_t* ctx, trace_command_t* cmd, uint comp,
                          GLenum type, bool array, bool normalized, GLenum internal) {
    uint index = gl_param_GLuint(cmd, 0);
    if (index==0 || index>=trc_gl_state_get_state_int(ctx->trace, GL_MAX_VERTEX_ATTRIBS, 0))
        ERROR2(, "Invalid vertex attribute index");
    index--;
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
            switch (type) {
            case GL_UNSIGNED_BYTE: val /= UINT8_MAX; break;
            case GL_UNSIGNED_SHORT: val /= UINT16_MAX; break;
            case GL_UNSIGNED_INT: val /= UINT32_MAX; break;
            case GL_BYTE: val = val<0 ? val/-(double)INT8_MIN : val/INT8_MAX; break;
            case GL_SHORT: val = val<0 ? val/-(double)INT16_MIN : val/INT16_MAX; break;
            case GL_INT: val = val<0 ? val/-(double)INT32_MIN : val/INT32_MAX; break;
            }
        }
        trc_gl_state_set_state_double(ctx->trace, GL_CURRENT_VERTEX_ATTRIB, index*4+i, 0);
    }
    for (; i < 3; i++)
        trc_gl_state_set_state_double(ctx->trace, GL_CURRENT_VERTEX_ATTRIB, index*4+i, 0);
    for (; i < 4; i++)
        trc_gl_state_set_state_double(ctx->trace, GL_CURRENT_VERTEX_ATTRIB, index*4+i, 1);
    
    double vals[4];
    for (i = 0; i < 4; i++)
        vals[i] = trc_gl_state_get_state_double(ctx->trace, GL_CURRENT_VERTEX_ATTRIB, index*4+i);
    
    switch (internal) {
    case GL_FLOAT: F(glVertexAttrib4dv(index+1, vals)); break;
    case GL_DOUBLE: F(glVertexAttribL4dv(index+1, vals)); break;
    case GL_UNSIGNED_INT: F(glVertexAttribI4ui(index, vals[0], vals[1], vals[2], vals[3])); break;
    case GL_INT: F(glVertexAttribI4i(index, vals[0], vals[1], vals[2], vals[3])); break;
    }
}

static double float11_to_double(uint v) {
    uint e = v >> 6;
    uint m = v & 63;
    if (!e && m) return 6.103515625e-05 * (m/64.0);
    else if (e>0 && e<31) return pow(2, e-15) * (1.0+m/64.0);
    else if (e==31 && !m) return INFINITY;
    else if (e==31 && m) return NAN;
    else assert(false);
}

static double float10_to_double(uint v) {
    uint e = v >> 5;
    uint m = v & 31;
    if (!e && m) return 6.103515625e-05 * (m/32.0);
    else if (e>0 && e<31) return pow(2, e-15) * (1.0+m/32.0);
    else if (e==31 && !m) return INFINITY;
    else if (e==31 && m) return NAN;
    else assert(false);
}

static void vertex_attrib_packed(trc_replay_context_t* ctx, trace_command_t* cmd, GLuint index,
                                 GLenum type, uint comp, GLboolean normalized, GLuint val) {
    if (index==0 || index>=trc_gl_state_get_state_int(ctx->trace, GL_MAX_VERTEX_ATTRIBS, 0))
        ERROR2(, "Invalid vertex attribute index");
    double res[4];
    switch (type) {
    case GL_INT_2_10_10_10_REV:
    case GL_UNSIGNED_INT_2_10_10_10_REV: {
        int64_t vals[4] = {val&1023, (val>>10)&1023, (val>>20)&1023, (val>>30)&3};
        bool signed_ = type == GL_INT_2_10_10_10_REV;
        if (signed_) {
            for (uint i = 0; i < 3; i++) {
                if (vals[i] & (1<<9)) //check sign bit
                    vals[i] = (vals[i]&511) | ~(int64_t)511;
            }
            if (vals[3] & 2)
                vals[3] = (vals[3]&1) | ~(int64_t)1;
        }
        if (normalized) {
            for (uint i = 0; i < 3; i++)
                res[i] = signed_ ? (vals[i]<0?vals[i]/512.0:vals[i]/511.0) : vals[i]/1023.0;
            res[3] = signed_ ? (vals[3]<0?vals[3]/2.0:vals[3]/1.0) : vals[3]/2.0;
        } else {
            for (uint i = 0; i < 4; i++)
                res[i] = vals[i];
        }
        break;
    }
    case GL_UNSIGNED_INT_10F_11F_11F_REV: {
        res[0] = float11_to_double(val&2047);
        res[1] = float11_to_double((val>>11)&2047);
        res[2] = float10_to_double((val>>22)&1023);
        break;
    }
    }
    for (uint i = comp; i < 4; i++) res[i] = i==3 ? 1.0 : 0.0;
    for (uint i = 0; i < 4; i++)
        trc_gl_state_set_state_double(ctx->trace, GL_CURRENT_VERTEX_ATTRIB, (index-1)*4+i, res[i]);
    
    F(glVertexAttrib4dv(index, res));
}

static trc_obj_t* get_bound_framebuffer(trc_replay_context_t* ctx, GLenum target) {
    const trc_gl_context_rev_t* state = trc_get_context(ctx->trace);
    switch (target) {
    case GL_DRAW_FRAMEBUFFER: 
        return state->draw_framebuffer.obj;
    case GL_READ_FRAMEBUFFER: 
        return state->read_framebuffer.obj;
    case GL_FRAMEBUFFER: 
        return state->draw_framebuffer.obj;
    }
    return NULL;
}

static void update_query(trc_replay_context_t* ctx, trace_command_t* cmd, GLenum target, GLuint fake_id, GLuint id) {
    if (!id) return;
    
    GLuint64 res = 0;
    if (target!=GL_TIME_ELAPSED && target!=GL_TIMESTAMP) { //TODO: Why is this branch here?
        F(glFinish)();
        while (!res) F(glGetQueryObjectui64v)(id, GL_QUERY_RESULT_AVAILABLE, &res);
        F(glGetQueryObjectui64v)(id, GL_QUERY_RESULT, &res);
    }
    
    trc_gl_query_rev_t query = *get_query(ctx->ns, fake_id);
    query.result = res;
    set_query(&query);
}

static void begin_get_fb0_data(trc_replay_context_t* ctx, GLint prev[11]) {
    F(glGetIntegerv)(GL_PACK_SWAP_BYTES, &prev[0]);
    F(glGetIntegerv)(GL_PACK_LSB_FIRST, &prev[1]);
    F(glGetIntegerv)(GL_PACK_ROW_LENGTH, &prev[2]);
    F(glGetIntegerv)(GL_PACK_IMAGE_HEIGHT, &prev[3]);
    F(glGetIntegerv)(GL_PACK_SKIP_PIXELS, &prev[4]);
    F(glGetIntegerv)(GL_PACK_SKIP_ROWS, &prev[5]);
    F(glGetIntegerv)(GL_PACK_SKIP_IMAGES, &prev[6]);
    F(glGetIntegerv)(GL_PACK_ALIGNMENT, &prev[7]);
    F(glGetIntegerv)(GL_READ_BUFFER, &prev[8]);
    F(glGetIntegerv)(GL_READ_FRAMEBUFFER_BINDING, &prev[9]);
    F(glGetIntegerv)(GL_PIXEL_PACK_BUFFER_BINDING, &prev[10]);
    
    F(glPixelStorei)(GL_PACK_SWAP_BYTES, GL_FALSE);
    F(glPixelStorei)(GL_PACK_LSB_FIRST, GL_FALSE);
    F(glPixelStorei)(GL_PACK_ROW_LENGTH, 0);
    F(glPixelStorei)(GL_PACK_IMAGE_HEIGHT, 0);
    F(glPixelStorei)(GL_PACK_SKIP_PIXELS, 0);
    F(glPixelStorei)(GL_PACK_SKIP_ROWS, 0);
    F(glPixelStorei)(GL_PACK_SKIP_IMAGES, 0);
    F(glPixelStorei)(GL_PACK_ALIGNMENT, 1);
    F(glBindFramebuffer)(GL_READ_FRAMEBUFFER, 0);
    F(glBindBuffer)(GL_PIXEL_PACK_BUFFER, 0);
}

static void end_get_fb0_data(trc_replay_context_t* ctx, const GLint prev[11]) {
    F(glBindBuffer)(GL_PIXEL_PACK_BUFFER, prev[10]);
    F(glBindFramebuffer)(GL_READ_FRAMEBUFFER, prev[9]);
    F(glReadBuffer)(prev[8]);
    F(glPixelStorei)(GL_PACK_ALIGNMENT, prev[7]);
    F(glPixelStorei)(GL_PACK_SKIP_IMAGES, prev[6]);
    F(glPixelStorei)(GL_PACK_SKIP_ROWS, prev[5]);
    F(glPixelStorei)(GL_PACK_SKIP_PIXELS, prev[4]);
    F(glPixelStorei)(GL_PACK_IMAGE_HEIGHT, prev[3]);
    F(glPixelStorei)(GL_PACK_ROW_LENGTH, prev[2]);
    F(glPixelStorei)(GL_PACK_LSB_FIRST, prev[1]);
    F(glPixelStorei)(GL_PACK_SWAP_BYTES, prev[0]);
}

static trc_data_t* replay_get_fb0_buffer(trc_replay_context_t* ctx, trc_gl_context_rev_t* state,
                                         GLenum buffer, GLenum format, GLenum type) {
    F(glReadBuffer)(buffer);
    
    size_t data_size = state->drawable_width * state->drawable_height * 4;
    trc_data_t* data = trc_create_data(ctx->trace, data_size, NULL, TRC_DATA_NO_ZERO);
    void* dest = trc_map_data(data, TRC_MAP_REPLACE);
    F(glReadPixels)(0, 0, state->drawable_width, state->drawable_height, format, type, dest);
    trc_unmap_freeze_data(ctx->trace, data);
    
    return data;
}

static void store_and_bind_fb(trc_replay_context_t* ctx, GLint* prev, GLuint fb) {
    F(glGetIntegerv)(GL_DRAW_FRAMEBUFFER_BINDING, prev);
    F(glBindFramebuffer)(GL_DRAW_FRAMEBUFFER, fb);
}

static void replay_update_fb0_buffers(trc_replay_context_t* ctx, bool backcolor, bool frontcolor,
                                      bool depth, bool stencil) {
    F(glFinish)();
    
    GLint prevfb;
    store_and_bind_fb(ctx, &prevfb, 0);
    GLint depth_size, stencil_size;
    F(glGetFramebufferAttachmentParameteriv)(GL_DRAW_FRAMEBUFFER, GL_DEPTH,
                                             GL_FRAMEBUFFER_ATTACHMENT_DEPTH_SIZE, &depth_size);
    F(glGetFramebufferAttachmentParameteriv)(GL_DRAW_FRAMEBUFFER, GL_STENCIL,
                                             GL_FRAMEBUFFER_ATTACHMENT_STENCIL_SIZE, &stencil_size);
    if (depth_size == 0) depth = false;
    if (stencil_size == 0) stencil = false;
    F(glBindFramebuffer)(GL_DRAW_FRAMEBUFFER, prevfb);
    
    GLint prev[11];
    begin_get_fb0_data(ctx, prev);
    trc_gl_context_rev_t state = *trc_get_context(ctx->trace);
    if (backcolor)
        state.back_color_buffer = replay_get_fb0_buffer(ctx, &state, GL_BACK, GL_RGBA, GL_UNSIGNED_BYTE);
    if (frontcolor)
        state.front_color_buffer = replay_get_fb0_buffer(ctx, &state, GL_FRONT, GL_RGBA, GL_UNSIGNED_BYTE);
    if (depth)
        state.back_depth_buffer = replay_get_fb0_buffer(ctx, &state, GL_BACK, GL_DEPTH_COMPONENT, GL_FLOAT);
    if (stencil)
        state.back_stencil_buffer = replay_get_fb0_buffer(ctx, &state, GL_BACK, GL_STENCIL_INDEX, GL_UNSIGNED_INT);
    trc_set_context(ctx->trace, &state);
    end_get_fb0_data(ctx, prev);
}

static bool not_one_of(int val, ...) {
    va_list list;
    va_start(list, val);
    while (true) {
        int v = va_arg(list, int);
        if (v == -1) break;
        if (v == val) return false;
    }
    va_end(list);
    return true;
}

static trc_obj_t** get_tf_buffer_list(trc_replay_context_t* ctx, size_t* count) {
    size_t max = trc_gl_state_get_state_int(ctx->trace, GL_MAX_TRANSFORM_FEEDBACK_BUFFERS, 0) + 1;
    trc_obj_t** bufs = replay_alloc(max*sizeof(trc_obj_t*));
    *count = 0;
    
    for (uint i = 0; i < max-1; i++) {
        trc_obj_t* buf = trc_gl_state_get_bound_buffer_indexed(ctx->trace, GL_TRANSFORM_FEEDBACK_BUFFER, i).buf.obj;
        if (buf) bufs[(*count)++] = buf;
    }
    
    return bufs;
}

static bool is_tf_buffer(uint count, trc_obj_t** bufs, trc_obj_t* test) {
    for (uint i = 0; i < count; i++) if (bufs[i] == test) return true;
    return false;
}

static const trc_gl_transform_feedback_rev_t* get_current_tf(trc_replay_context_t* ctx) {
    return trc_obj_get_rev(trc_gl_state_get_current_tf(ctx->trace), -1);
}

static bool tf_draw_validation(trc_replay_context_t* ctx, trace_command_t* cmd, GLenum primitive) {
    trc_gl_transform_feedback_rev_t tf = *get_current_tf(ctx);
    if (!tf.active_not_paused) return true;
    
    size_t buf_count;
    trc_obj_t** bufs = get_tf_buffer_list(ctx, &buf_count);
    
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
    
    trc_obj_t* geom_program;
    if ((geom_program=trc_gl_state_get_bound_program(ctx->trace))) {
        F(glGetError)(); //TODO: Do this in a less hackish way
        F(glGetProgramiv)(((trc_gl_program_rev_t*)trc_obj_get_rev(geom_program, -1))->real, GL_GEOMETRY_OUTPUT_TYPE, &test_primitive);
        if (F(glGetError)() == GL_INVALID_OPERATION) test_primitive = primitive; //No geometry shader
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

static bool begin_draw(trc_replay_context_t* ctx, trace_command_t* cmd, GLenum primitive) {
    const trc_gl_context_rev_t* state = trc_get_context(ctx->trace);
    const trc_gl_vao_rev_t* vao = trc_obj_get_rev(state->bound_vao.obj, -1);
    if (!vao) ERROR2(false, "No VAO bound");
    trc_obj_t* vertex_program_obj = get_active_program_for_stage(ctx, GL_VERTEX_SHADER);
    const trc_gl_program_rev_t* vertex_program = trc_obj_get_rev(vertex_program_obj, -1);
    if (!vertex_program) ERROR2(false, "No vertex program active");
    
    if (!tf_draw_validation(ctx, cmd, primitive)) return false;
    
    GLint last_buf;
    F(glGetIntegerv)(GL_ARRAY_BUFFER_BINDING, &last_buf);
    
    trc_gl_vao_attrib_t* vao_attribs = trc_map_data(vao->attribs, TRC_MAP_READ);
    
    for (size_t i = 0; i < vao->attribs->size/sizeof(trc_gl_vao_attrib_t); i++) {
        trc_gl_vao_attrib_t* a = &vao_attribs[i];
        if (!a->enabled) continue;
        const trc_gl_buffer_rev_t* buf = trc_obj_get_rev(a->buffer.obj, -1);
        if (buf->mapped) {
            trc_unmap_data(vao->attribs);
            ERROR2(false, "Buffer for vertex attribute %zu is mapped", i);
        }
    }
    
    size_t prog_vertex_attrib_count = vertex_program->vertex_attribs->size / (sizeof(uint)*2);
    uint* prog_vertex_attribs = trc_map_data(vertex_program->vertex_attribs, TRC_MAP_READ);
    for (size_t i = 0; i < vao->attribs->size/sizeof(trc_gl_vao_attrib_t); i++) {
        GLint real_loc = -1;
        for (size_t j = 0; j < prog_vertex_attrib_count; j++) {
            if (prog_vertex_attribs[j*2+1] == i) {
                real_loc = prog_vertex_attribs[j*2];
                break;
            }
        }
        if (real_loc < 0) continue;
        
        trc_gl_vao_attrib_t* a = &vao_attribs[i];
        if (!a->enabled) {
            F(glDisableVertexAttribArray)(real_loc);
            continue;
        }
        
        F(glEnableVertexAttribArray)(real_loc);
        
        F(glBindBuffer)(GL_ARRAY_BUFFER, ((trc_gl_buffer_rev_t*)trc_obj_get_rev(a->buffer.obj, -1))->real);
        if (a->integer)
            F(glVertexAttribIPointer)(real_loc, a->size, a->type, a->stride, (const void*)(uintptr_t)a->offset);
        else
            F(glVertexAttribPointer)(real_loc, a->size, a->type, a->normalized, a->stride, (const void*)(uintptr_t)a->offset);
        
        if (trc_gl_state_get_ver(ctx->trace) > 330)
            F(glVertexAttribDivisor)(real_loc, a->divisor);
    }
    trc_unmap_data(vao->attribs);
    trc_unmap_data(vertex_program->vertex_attribs);
    
    F(glBindBuffer)(GL_ARRAY_BUFFER, last_buf);
    
    return true;
}

#define PARTIAL_VALIDATE_CLEAR_BUFFER do {\
int max_draw_buffers = trc_gl_state_get_state_int(ctx->trace, GL_MAX_DRAW_BUFFERS, 0);\
if (p_drawbuffer<0 || (p_drawbuffer==0&&p_buffer!=GL_COLOR) || p_drawbuffer>=max_draw_buffers)\
    ERROR("Invalid buffer");\
} while (0)

static void update_drawbuffer(trc_replay_context_t* ctx, GLenum buffer, GLuint drawbuffer) {
    const trc_gl_context_rev_t* state = trc_get_context(ctx->trace);
    if (state->draw_framebuffer.obj == NULL) {
        switch (buffer) {
        case GL_COLOR: replay_update_fb0_buffers(ctx, true, false, false, false); break;
        case GL_DEPTH: replay_update_fb0_buffers(ctx, false, false, true, false); break;
        case GL_STENCIL: replay_update_fb0_buffers(ctx, false, false, false, true); break;
        }
    } else {
        const trc_gl_framebuffer_rev_t* rev = trc_obj_get_rev(state->draw_framebuffer.obj, -1);
        uint attachment;
        if (drawbuffer>=rev->draw_buffers->size/sizeof(GLenum)) {
            attachment = GL_NONE;
        } else {
            attachment = ((GLenum*)trc_map_data(rev->draw_buffers, TRC_MAP_READ))[drawbuffer];
            trc_unmap_data(rev->draw_buffers);
        }
        
        size_t attach_count = rev->attachments->size / sizeof(trc_gl_framebuffer_attachment_t);
        const trc_gl_framebuffer_attachment_t* attachs = trc_map_data(rev->attachments, TRC_MAP_READ);
        for (size_t i = 0; i < attach_count; i++) {
            const trc_gl_framebuffer_attachment_t* attach = &attachs[i];
            if (attach->attachment != attachment) continue;
            if (attach->has_renderbuffer) continue;
            replay_update_tex_image(ctx, trc_obj_get_rev(attach->texture.obj, -1), attach->level, attach->face);
        }
        trc_unmap_data(rev->attachments);
    }
}

static void update_buffers(trc_replay_context_t* ctx, trc_obj_t* fb, GLbitfield mask) {
    if (fb == 0) {
        bool color = mask & GL_COLOR_BUFFER_BIT;
        bool depth = mask & GL_DEPTH_BUFFER_BIT;
        bool stencil = mask & GL_STENCIL_BUFFER_BIT;
        replay_update_fb0_buffers(ctx, color, false, depth, stencil);
    } else {
        const trc_gl_framebuffer_rev_t* rev = trc_obj_get_rev(fb, -1);
        
        size_t max_updates = rev->draw_buffers->size/sizeof(GLenum) + 3;
        GLenum* updates = replay_alloc(max_updates*sizeof(GLenum));
        size_t update_count = 0;
        
        if (mask & GL_COLOR_BUFFER_BIT) {
            const GLenum* draw_buffers = trc_map_data(rev->draw_buffers, TRC_MAP_READ);
            for (size_t i = 0; i < rev->draw_buffers->size/sizeof(GLenum); i++)
                updates[update_count++] = draw_buffers[i];
            trc_unmap_data(rev->draw_buffers);
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
                replay_update_tex_image(ctx, trc_obj_get_rev(attach->texture.obj, -1), attach->level, attach->face);
            }
        }
        trc_unmap_data(rev->attachments);
    }
}

static void update_buffer_from_gl(trc_replay_context_t* ctx, trc_obj_t* obj, size_t offset, ptrdiff_t size_) {
    trc_gl_buffer_rev_t rev = *(trc_gl_buffer_rev_t*)trc_obj_get_rev(obj, -1);
    size_t size = size_ < 0 ? rev.data->size : size_;
    
    GLint prev_array_buffer;
    F(glGetIntegerv)(GL_ARRAY_BUFFER_BINDING, &prev_array_buffer);
    F(glBindBuffer)(GL_ARRAY_BUFFER, rev.real);
    
    void* newdata = malloc(size);
    F(glGetBufferSubData)(GL_ARRAY_BUFFER, offset, size, newdata);
    
    rev.data = trc_copy_data(ctx->trace, rev.data, 0);
    uint8_t* data = trc_map_data(rev.data, TRC_MAP_MODIFY);
    memcpy(data+offset, newdata, size);
    trc_unmap_freeze_data(ctx->trace, rev.data);
    
    free(newdata);
    
    set_buffer(&rev);
    
    F(glBindBuffer)(GL_ARRAY_BUFFER, prev_array_buffer);
}

static void end_draw(trc_replay_context_t* ctx, trace_command_t* cmd) {
    //Update framebuffer
    bool depth = trc_gl_state_get_state_bool(ctx->trace, GL_DEPTH_WRITEMASK, 0);
    bool stencil = trc_gl_state_get_state_int(ctx->trace, GL_STENCIL_WRITEMASK, 0) != 0;
    stencil = stencil || trc_gl_state_get_state_int(ctx->trace, GL_STENCIL_BACK_WRITEMASK, 0)!=0;
    
    GLbitfield mask = GL_COLOR_BUFFER_BIT;
    if (depth) mask |= GL_DEPTH_BUFFER_BIT;
    if (stencil) mask |= GL_STENCIL_BUFFER_BIT;
    //TODO: Only update color buffers that could have been written to using GL_COLOR_WRITEMASK
    update_buffers(ctx, trc_gl_state_get_draw_framebuffer(ctx->trace), mask);
    
    //Update transform feedback buffers
    size_t xfb_buffer_count = trc_gl_state_get_bound_buffer_indexed_size(ctx->trace, GL_TRANSFORM_FEEDBACK_BUFFER);
    for (size_t i = 0; i < xfb_buffer_count; i++) {
        trc_gl_buffer_binding_point_t binding =
            trc_gl_state_get_bound_buffer_indexed(ctx->trace, GL_TRANSFORM_FEEDBACK_BUFFER, i);
        if (!binding.buf.obj) continue;
        update_buffer_from_gl(ctx, binding.buf.obj, binding.offset, binding.size);
    }
}

static void gen_textures(trc_replay_context_t* ctx, size_t count, const GLuint* real, const GLuint* fake, bool create, GLenum target) {
    trc_gl_texture_rev_t rev;
    rev.has_object = create;
    rev.type = create ? target : 0;
    rev.depth_stencil_mode = GL_DEPTH_COMPONENT;
    rev.base_level = 0;
    rev.sample_params.border_color[0] = 0;
    rev.sample_params.border_color[1] = 0;
    rev.sample_params.border_color[2] = 0;
    rev.sample_params.border_color[3] = 0;
    rev.sample_params.compare_func = GL_LEQUAL;
    rev.sample_params.compare_mode = GL_NONE;
    rev.sample_params.lod_bias = 0;
    rev.sample_params.min_filter = rev.type==GL_TEXTURE_RECTANGLE ? GL_LINEAR : GL_NEAREST_MIPMAP_LINEAR;
    rev.sample_params.mag_filter = GL_LINEAR;
    rev.sample_params.min_lod = -1000;
    rev.sample_params.max_lod = 1000;
    rev.max_level = 1000;
    rev.swizzle[0] = GL_RED;
    rev.swizzle[1] = GL_GREEN;
    rev.swizzle[2] = GL_BLUE;
    rev.swizzle[3] = GL_ALPHA;
    GLenum wrap_mode = rev.type==GL_TEXTURE_RECTANGLE ? GL_CLAMP_TO_EDGE : GL_REPEAT;
    rev.sample_params.wrap_s = wrap_mode;
    rev.sample_params.wrap_t = wrap_mode;
    rev.sample_params.wrap_r = wrap_mode;
    rev.sample_params.max_anisotropy = 1.0;
    rev.images = NULL;
    for (size_t i = 0; i < count; ++i) {
        rev.real = real[i];
        trc_create_named_obj(ctx->ns, TrcTexture, fake[i], &rev);
    }
}

static void gen_buffers(trc_replay_context_t* ctx, size_t count, const GLuint* real, const GLuint* fake, bool create) {
    trc_gl_buffer_rev_t rev;
    rev.has_object = create;
    rev.tf_binding_count = 0;
    rev.data_usage = GL_STATIC_DRAW;
    rev.data = trc_create_data(ctx->trace, 0, NULL, TRC_DATA_IMMUTABLE);
    rev.mapped = false;
    rev.map_offset = 0;
    rev.map_length = 0;
    rev.map_access = 0;
    for (size_t i = 0; i < count; ++i) {
        rev.real = real[i];
        trc_create_named_obj(ctx->ns, TrcBuffer, fake[i], &rev);
    }
}

static void gen_framebuffers(trc_replay_context_t* ctx, size_t count, const GLuint* real, const GLuint* fake, bool create) {
    trc_gl_framebuffer_rev_t rev;
    rev.has_object = false;
    trc_data_t* empty_data = trc_create_data(ctx->trace, 0, NULL, TRC_DATA_IMMUTABLE);
    rev.attachments = empty_data;
    rev.draw_buffers = empty_data;
    for (size_t i = 0; i < count; ++i) {
        rev.real = real[i];
        trc_create_named_obj(ctx->ns, TrcFramebuffer, fake[i], &rev);
    }
}

static void gen_queries(trc_replay_context_t* ctx, size_t count, const GLuint* real, const GLuint* fake, bool create, GLenum target) {
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

static void gen_samplers(trc_replay_context_t* ctx, size_t count, const GLuint* real, const GLuint* fake) {
    trc_gl_sampler_rev_t rev;
    rev.params.border_color[0] = 0;
    rev.params.border_color[1] = 0;
    rev.params.border_color[2] = 0;
    rev.params.border_color[3] = 0;
    rev.params.compare_func = GL_LEQUAL;
    rev.params.compare_mode = GL_NONE;
    rev.params.min_filter = GL_NEAREST_MIPMAP_LINEAR;
    rev.params.mag_filter = GL_LINEAR;
    rev.params.min_lod = -1000;
    rev.params.max_lod = 1000;
    rev.params.wrap_s = GL_REPEAT;
    rev.params.wrap_t = GL_REPEAT;
    rev.params.wrap_r = GL_REPEAT;
    rev.params.lod_bias = 0.0;
    rev.params.max_anisotropy = 1.0;
    for (size_t i = 0; i < count; ++i) {
        rev.real = real[i];
        trc_create_named_obj(ctx->ns, TrcSampler, fake[i], &rev);
    }
}

static void gen_renderbuffers(trc_replay_context_t* ctx, size_t count, const GLuint* real, const GLuint* fake, bool create) {
    trc_gl_renderbuffer_rev_t rev;
    rev.has_object = create;
    rev.has_storage = false;
    for (size_t i = 0; i < count; ++i) {
        rev.real = real[i];
        trc_create_named_obj(ctx->ns, TrcRenderbuffer, fake[i], &rev);
    }
}

static void gen_vertex_arrays(trc_replay_context_t* ctx, size_t count, const GLuint* real, const GLuint* fake, bool create) {
    trc_gl_vao_rev_t rev;
    rev.has_object = create;
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
    trc_unmap_data(rev.attribs);
    for (size_t i = 0; i < count; ++i) {
        rev.real = real[i];
        trc_create_named_obj(ctx->ns, TrcVAO, fake[i], &rev);
    }
}

static void gen_transform_feedbacks(trc_replay_context_t* ctx, size_t count, const GLuint* real, const GLuint* fake, bool create) {
    trc_gl_transform_feedback_rev_t rev;
    rev.has_object = create;
    size_t size = trc_gl_state_get_state_int(ctx->trace, GL_MAX_TRANSFORM_FEEDBACK_BUFFERS, 0);
    size *= sizeof(trc_gl_buffer_binding_point_t);
    rev.bindings = trc_create_data(ctx->trace, size, NULL, TRC_DATA_IMMUTABLE);
    rev.active = false;
    rev.paused = false;
    rev.active_not_paused = false;
    for (size_t i = 0; i < count; ++i) {
        rev.real = real[i];
        trc_create_named_obj(ctx->ns, TrcTransformFeedback, fake[i], &rev);
    }
}

static bool buffer_data(trc_replay_context_t* ctx, trace_command_t* cmd, bool dsa, trc_obj_t* buf, GLsizeiptr size, const void* data, GLenum usage) {
    if (size < 0) ERROR2(false, "Invalid size");
    const trc_gl_buffer_rev_t* rev = trc_obj_get_rev(buf, -1);
    if (!rev) ERROR2(false, dsa?"Invalid buffer name":"No buffer bound to target");
    if (!rev->has_object) ERROR2(false, "Buffer name has no object");
    if (get_current_tf(ctx)->active_not_paused && rev->tf_binding_count)
        trc_add_warning(cmd, "Buffer cannot be modified as it is a transform feedback one while transform feedback is active and unpaused");
    trc_gl_buffer_rev_t newrev = *rev;
    newrev.data = trc_create_data(ctx->trace, size, data, TRC_DATA_IMMUTABLE);
    set_buffer(&newrev);
    return true;
}

static bool buffer_sub_data(trc_replay_context_t* ctx, trace_command_t* cmd, bool dsa, trc_obj_t* buf, GLintptr offset, GLsizeiptr size, const void* data) {
    if (offset<0) ERROR2(false, "Invalid offset");
    if (size<0) ERROR2(false, "Invalid size");
    const trc_gl_buffer_rev_t* rev = trc_obj_get_rev(buf, -1);
    if (!rev) ERROR2(false, dsa?"Invalid buffer name":"No buffer bound to target");
    if (!rev->has_object) ERROR2(false, "Buffer name does not have an object");
    if (get_current_tf(ctx)->active_not_paused && rev->tf_binding_count)
        trc_add_warning(cmd, "Buffer cannot be modified as it is a transform feedback one while transform feedback is active and unpaused");
    if (offset+size > rev->data->size) ERROR2(false, "Invalid range");
    if (rev->mapped && !(rev->map_access&GL_MAP_PERSISTENT_BIT))
        ERROR2(false, "Buffer is mapped without persistent access");
    
    trc_gl_buffer_rev_t newrev = *rev;
    
    newrev.data = trc_create_data(ctx->trace, rev->data->size, trc_map_data(rev->data, TRC_MAP_READ), 0);
    trc_unmap_data(rev->data);
    
    void* newdata = trc_map_data(newrev.data, TRC_MAP_REPLACE);
    memcpy((uint8_t*)newdata+offset, data, size);
    trc_unmap_freeze_data(ctx->trace, newrev.data);
    
    set_buffer(&newrev);
    return true;
}

static bool copy_buffer_data(trc_replay_context_t* ctx, trace_command_t* cmd, bool dsa, trc_obj_t* read,
                             trc_obj_t* write, GLintptr read_off, GLintptr write_off, GLsizeiptr size) {
    if (read_off<0 || write_off<0 || size<0) ERROR2(false, "The read offset, write offset or size is negative");
    
    const trc_gl_buffer_rev_t* read_rev = trc_obj_get_rev(read, -1);
    if (!read_rev) ERROR2(false, dsa?"Invalid read buffer name":"No buffer bound to read target");
    if (!read_rev->has_object) ERROR2(false, "Read buffer name has no object");
    if (get_current_tf(ctx)->active_not_paused && read_rev->tf_binding_count)
        trc_add_warning(cmd, "Read buffer cannot be read as it is a transform feedback one while transform feedback is active and unpaused");
    if (read_off+size > read_rev->data->size) ERROR2(false, "Invalid size and read offset");
    
    const trc_gl_buffer_rev_t* write_rev = trc_obj_get_rev(write, -1);
    if (!write_rev) ERROR2(false, dsa?"Invalid write buffer name":"No buffer bound to write target");
    if (!write_rev->has_object) ERROR2(false, "Write buffer name has no object");
    if (get_current_tf(ctx)->active_not_paused && write_rev->tf_binding_count)
        trc_add_warning(cmd, "Write buffer cannot be modified as it is a transform feedback one while transform feedback is active and unpaused");
    if (write_off+size > write_rev->data->size) ERROR2(false, "Invalid size and write offset");
    
    trc_gl_buffer_rev_t res = *write_rev;
    
    res.data = trc_create_data(ctx->trace, write_rev->data->size, trc_map_data(write_rev->data, TRC_MAP_READ), 0);
    trc_unmap_data(write_rev->data);
    
    void* newdata = trc_map_data(res.data, TRC_MAP_REPLACE);
    uint8_t* readdata = trc_map_data(read_rev->data, TRC_MAP_READ);
    memcpy(newdata+write_off, readdata+read_off, size);
    trc_unmap_data(read_rev->data);
    trc_unmap_freeze_data(ctx->trace, res.data);
    
    set_buffer(&res);
    return true;
}

static void map_buffer(trc_replay_context_t* ctx, trace_command_t* cmd, bool dsa, GLuint fake, GLenum access) {
    const trc_gl_buffer_rev_t* rev = get_buffer(ctx->ns, fake);
    if (!rev) ERROR2(, dsa?"Invalid buffer name":"No buffer bound to target");
    if (!rev->has_object) ERROR2(, "Buffer name has no object");
    if (get_current_tf(ctx)->active_not_paused && rev->tf_binding_count)
        trc_add_warning(cmd, "Buffer cannot be mapped as it is a transform feedback one while transform feedback is active and unpaused");
    if (rev->mapped) ERROR2(, "Buffer is already mapped");
    
    trc_gl_buffer_rev_t newrev = *rev;
    newrev.mapped = true;
    newrev.map_offset = 0;
    newrev.map_length = rev->data->size;
    switch (access) {
    case GL_READ_ONLY: newrev.map_access = GL_MAP_READ_BIT; break;
    case GL_WRITE_ONLY: newrev.map_access = GL_MAP_WRITE_BIT; break;
    case GL_READ_WRITE: newrev.map_access = GL_MAP_READ_BIT | GL_MAP_WRITE_BIT; break;
    }
    
    set_buffer(&newrev);
}

static void map_buffer_range(trc_replay_context_t* ctx, trace_command_t* cmd, bool dsa, GLuint fake,
                             GLintptr offset, GLsizeiptr length, GLbitfield access) {
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
    
    const trc_gl_buffer_rev_t* rev = get_buffer(ctx->ns, fake);
    if (!rev) ERROR2(, dsa?"Invalid buffer name":"No buffer bound to target");
    if (get_current_tf(ctx)->active_not_paused && rev->tf_binding_count)
        trc_add_warning(cmd, "Buffer cannot be mapped as it is a transform feedback one while transform feedback is active and unpaused");
    trc_gl_buffer_rev_t newrev = *rev;
    
    if (offset+length > newrev.data->size)
        ERROR2(, "Invalid range");
    if (newrev.mapped) ERROR2(, "Buffer is already mapped");
    
    newrev.mapped = true;
    newrev.map_offset = offset;
    newrev.map_length = length;
    newrev.map_access = access;
    
    set_buffer(&newrev);
}

static void unmap_buffer(trc_replay_context_t* ctx, trace_command_t* cmd, bool dsa, GLuint target_or_buf) {
    const trc_gl_buffer_rev_t* rev;
    if (dsa) rev = get_buffer(ctx->ns, target_or_buf);
    else rev = trc_obj_get_rev(get_bound_buffer(ctx, target_or_buf), -1);
    if (!rev) ERROR2(, dsa?"Invalid buffer name":"No buffer bound to target");
    if (!rev->mapped) ERROR2(, "Buffer is not mapped");
    
    trc_gl_buffer_rev_t newrev = *rev;
    
    if (rev->map_access & GL_MAP_WRITE_BIT) {
        trace_extra_t* extra = trc_get_extra(cmd, "replay/glUnmapBuffer/data");
        if (!extra) trc_add_error(cmd, "replay/glUnmapBuffer/data extra not found");
        
        if (extra) {
            if (extra->size != rev->data->size) ERROR2(, "Invalid trace");
            if (dsa) //Assume glNamedBufferSubData is supported if glUnmapNamedBuffer is being called
                F(glNamedBufferSubData)(rev->real, 0, extra->size, extra->data);
            else
                F(glBufferSubData)(target_or_buf, 0, extra->size, extra->data);
        }
        
        newrev.data = trc_create_data(ctx->trace, rev->data->size, NULL, TRC_DATA_NO_ZERO);
        void* newdata = trc_map_data(newrev.data, TRC_MAP_REPLACE);
        
        void* olddata = trc_map_data(rev->data, TRC_MAP_READ);
        memcpy(newdata, olddata, rev->data->size);
        trc_unmap_data(rev->data);
        
        if (extra) memcpy(newdata, extra->data, extra->size);
        trc_unmap_freeze_data(ctx->trace, newrev.data);
    }
    
    newrev.mapped = false;
    newrev.map_offset = 0;
    newrev.map_length = 0;
    newrev.map_access = 0;
    
    set_buffer(&newrev);
}

static bool flush_mapped_buffer_range(trc_replay_context_t* ctx, trace_command_t* cmd, bool dsa,
                                      GLenum target_or_buf, GLintptr offset, GLsizeiptr length) {
    const trc_gl_buffer_rev_t* rev;
    if (dsa) rev = get_buffer(ctx->ns, target_or_buf);
    else rev = trc_obj_get_rev(get_bound_buffer(ctx, target_or_buf), -1);
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

static void get_buffer_sub_data(trc_replay_context_t* ctx, trace_command_t* cmd, bool dsa, trc_obj_t* obj, GLintptr offset, GLsizeiptr size) {
    const trc_gl_buffer_rev_t* rev = trc_obj_get_rev(obj, -1);
    if (!rev) ERROR2(, dsa?"Invalid buffer name":"No buffer bound to target");
    if (!rev->has_object) ERROR2(, "Buffer name has no object");
    if (rev->mapped && !(rev->map_access&GL_MAP_PERSISTENT_BIT)) ERROR2(, "Buffer is mapped without GL_MAP_PERSISTENT_BIT");
    if (offset<0 || size<0 || offset+size>rev->data->size) ERROR2(, "Invalid offset and/or size");
}

static bool renderbuffer_storage(trc_replay_context_t* ctx, trace_command_t* cmd, const trc_gl_renderbuffer_rev_t* rb,
                                 bool dsa, GLenum internal_format, GLsizei width, GLsizei height, GLsizei samples) {
    if (!rb) ERROR2(false, dsa?"Invalid renderbuffer name":"No renderbuffer bound");
    if (!rb->has_object) ERROR2(false, "Renderbuffer name has no obejct");
    int maxsize = trc_gl_state_get_state_int(ctx->trace, GL_MAX_RENDERBUFFER_SIZE, 0);
    if (width<0 || height<0 || width>maxsize || height>maxsize)
        ERROR2(false, "Invalid dimensions");
    //TODO: test if samples if valid
    //TODO: handle when internal_format is not renderable
    return true;
}

static bool begin_query(trc_replay_context_t* ctx, trace_command_t* cmd, GLenum target, GLuint index, GLuint id) {
    if (index >= trc_gl_state_get_bound_queries_size(ctx->trace, target))
        ERROR2(false, "Index is greater than maximum");
    if (trc_gl_state_get_bound_queries(ctx->trace, target, index))
        ERROR2(false, "Query is already active at target");
    
    const trc_gl_query_rev_t* rev = get_query(ctx->ns, id);
    if (!rev) ERROR2(false, "Invalid query name");
    if (rev->active_index != -1) ERROR2(false, "Query is already active");
    if (rev->has_object && rev->type!=target)
        ERROR2(false, "Query object's type does not match target");
    
    trc_gl_query_rev_t newrev = *rev;
    newrev.type = target;
    newrev.has_object = true;
    newrev.active_index = index;
    set_query(&newrev);
    
    trc_gl_state_set_bound_queries(ctx->trace, target, index, newrev.head.obj);
    
    return true;
}

static bool end_query(trc_replay_context_t* ctx, trace_command_t* cmd, GLenum target, GLuint index) {
    if (index >= trc_gl_state_get_bound_queries_size(ctx->trace, target))
        ERROR2(false, "Index is greater than maximum");
    trc_obj_t* query = trc_gl_state_get_bound_queries(ctx->trace, target, index);
    if (!query) ERROR2(false, "No query active at target");
    
    trc_gl_query_rev_t newrev = *(const trc_gl_query_rev_t*)trc_obj_get_rev(query, -1);
    newrev.active_index = -1;
    set_query(&newrev);
    
    trc_gl_state_set_bound_queries(ctx->trace, target, index, NULL);
    return true;
}

static bool tex_image(trc_replay_context_t* ctx, trace_command_t* cmd, bool dsa, GLuint tex_or_target,
                      GLint level, GLint internal_format, GLint border, GLenum format, GLenum type,
                      bool sub, int dim, ...) {
    int size[3];
    int offset[3];
    va_list list;
    va_start(list, dim);
    for (int i = 0; i < dim; i++) size[i] = va_arg(list, int);
    for (int i = 0; (i<dim) && sub; i++) offset[i] = va_arg(list, int);
    va_end(list);
    
    const trc_gl_texture_rev_t* tex_rev;
    if (dsa) tex_rev = get_texture(ctx->ns, tex_or_target);
    else tex_rev = replay_get_bound_tex(ctx, tex_or_target);
    if (!tex_rev) ERROR2(false, dsa?"Invalid texture name":"No texture bound to target");
    if (!tex_rev->has_object) ERROR2(false, "Texture name has no object");
    
    int max_size = trc_gl_state_get_state_int(ctx->trace, GL_MAX_TEXTURE_SIZE, 0);
    if (level<0 || level>ceil(log2(max_size))) ERROR2(false, "Invalid level");
    for (int i = 0; i < dim; i++)
        if (size[i]<0 || size[i]>max_size) ERROR2(false, "Invalid %s", (const char*[]){"width", "height", "depth/layers"}[i]);
    if (sub) {
        size_t img_count = tex_rev->images->size / sizeof(trc_gl_texture_image_t);
        trc_gl_texture_image_t* images = trc_map_data(tex_rev->images, TRC_MAP_READ);
        trc_gl_texture_image_t* image = NULL;
        for (size_t i = 0; i < img_count; i++) {
            if (images[i].level == level) {
                image = &images[i];
                break;
            }
        }
        if (image == NULL) {
            trc_unmap_data(tex_rev->images);
            ERROR2(false, "No such mipmap");
        }
        internal_format = image->internal_format; //Used for format validation later
        if (dim>=1 && (offset[0]<0 || offset[0]+size[0] > image->width)) ERROR2(false, "Invalid x range");
        if (dim>=2 && (offset[1]<0 || offset[1]+size[1] > image->height)) ERROR2(false, "Invalid x range");
        if (dim>=3 && (offset[2]<0 || offset[2]+size[2] > image->depth)) ERROR2(false, "Invalid x range");
        trc_unmap_data(tex_rev->images);
    }
    if (border != 0) ERROR2(false, "Border must be 0");
    
    if (!not_one_of(type, GL_UNSIGNED_BYTE_3_3_2, GL_UNSIGNED_BYTE_2_3_3_REV, GL_UNSIGNED_SHORT_5_6_5, GL_UNSIGNED_SHORT_5_6_5_REV, -1) && format!=GL_RGB)
        ERROR2(false, "Invalid format + internal format combination");
    if (!not_one_of(type, GL_UNSIGNED_SHORT_4_4_4_4, GL_UNSIGNED_SHORT_4_4_4_4_REV, GL_UNSIGNED_SHORT_5_5_5_1, GL_UNSIGNED_SHORT_1_5_5_5_REV, GL_UNSIGNED_INT_8_8_8_8,
        GL_UNSIGNED_INT_8_8_8_8_REV, GL_UNSIGNED_INT_10_10_10_2, GL_UNSIGNED_INT_2_10_10_10_REV, -1) && format!=GL_RGBA && format!=GL_BGRA)
        ERROR2(false, "Invalid format + internal format combination");
    if (format==GL_DEPTH_COMPONENT && not_one_of(internal_format, GL_DEPTH_COMPONENT, GL_DEPTH_COMPONENT16, GL_DEPTH_COMPONENT24, GL_DEPTH_COMPONENT32, -1))
        ERROR2(false, "Invalid format + internal format combination");
    if (format!=GL_DEPTH_COMPONENT && !not_one_of(internal_format, GL_DEPTH_COMPONENT, GL_DEPTH_COMPONENT16, GL_DEPTH_COMPONENT24, GL_DEPTH_COMPONENT32, -1))
        ERROR2(false, "Invalid format + internal format combination");
    if (format==GL_STENCIL_INDEX && internal_format!=GL_STENCIL_INDEX)
        ERROR2(false, "Invalid format + internal format combination");
    if (format!=GL_STENCIL_INDEX && internal_format==GL_STENCIL_INDEX)
        ERROR2(false, "Invalid format + internal format combination");
    
    trc_obj_t* pu_buf = trc_gl_state_get_bound_buffer(ctx->trace, GL_PIXEL_UNPACK_BUFFER);
    const trc_gl_buffer_rev_t* pu_buf_rev = trc_obj_get_rev(pu_buf, -1);
    if (pu_buf_rev && pu_buf_rev->mapped) ERROR2(false, "GL_PIXEL_UNPACK_BUFFER is mapped");
    //TODO: More validation for GL_PIXEL_UNPACK_BUFFER
    
    return true;
}

static const trc_gl_buffer_rev_t* on_change_tf_binding(trc_replay_context_t* ctx, trc_obj_t* prev_buf, trc_obj_t* new_buf) {
    if (prev_buf) {
        trc_gl_buffer_rev_t newrev = *(const trc_gl_buffer_rev_t*)trc_obj_get_rev(prev_buf, -1);
        newrev.tf_binding_count--;
        trc_obj_set_rev(prev_buf, &newrev);
    }
    trc_gl_buffer_rev_t newrev = *(const trc_gl_buffer_rev_t*)trc_obj_get_rev(new_buf, -1);
    newrev.tf_binding_count++;
    trc_obj_set_rev(new_buf, &newrev);
    return trc_obj_get_rev(new_buf, -1);
}

static void on_activate_tf(trc_replay_context_t* ctx, trace_command_t* cmd) {
    size_t buf_count;
    trc_obj_t** bufs = get_tf_buffer_list(ctx, &buf_count);
    for (size_t i = 0; i < buf_count; i++) {
        if (((const trc_gl_buffer_rev_t*)trc_obj_get_rev(bufs[i], -1))->mapped)
            trc_add_warning(cmd, "Buffer bound to GL_TRANSFORM_FEEDBACK_BUFFER is mapped");
    }
}

static void bind_buffer(trc_replay_context_t* ctx, GLenum target, GLuint buffer) {
    const trc_gl_buffer_rev_t* rev = get_buffer(ctx->ns, buffer);
    if (rev && !rev->has_object) {
        trc_gl_buffer_rev_t newrev = *rev;
        newrev.has_object = true;
        set_buffer(&newrev);
    }
    trc_gl_state_set_bound_buffer(ctx->trace, target, rev?rev->head.obj:NULL);
}

static void bind_buffer_indexed_ranged(trc_replay_context_t* ctx, GLenum target, GLuint index, GLuint buffer, uint64_t offset, uint64_t size) {
    const trc_gl_buffer_rev_t* rev = get_buffer(ctx->ns, buffer);
    if (target == GL_TRANSFORM_FEEDBACK_BUFFER)
        rev = on_change_tf_binding(ctx, trc_gl_state_get_bound_buffer_indexed(ctx->trace, target, index).buf.obj, rev?rev->head.obj:NULL);
    if (rev && !rev->has_object) {
        trc_gl_buffer_rev_t newrev = *rev;
        newrev.has_object = true;
        set_buffer(&newrev);
    }
    trc_gl_buffer_binding_point_t point;
    point.buf.obj = rev ? rev->head.obj : NULL;
    point.offset = offset;
    point.size = size;
    trc_gl_state_set_bound_buffer_indexed(ctx->trace, target, index, point);
}

static void bind_buffer_indexed(trc_replay_context_t* ctx, GLenum target, GLuint index, GLuint buffer) {
    bind_buffer_indexed_ranged(ctx, target, index, buffer, 0, 0);
}

glXMakeCurrent: //Display* p_dpy, GLXDrawable p_drawable, GLXContext p_ctx
    SDL_GLContext glctx = NULL;
    trc_namespace_t* global_ns = &ctx->trace->inspection.global_namespace;
    if (p_ctx) {
        if (!(glctx=((trc_gl_context_rev_t*)trc_get_obj(global_ns, TrcContext, p_ctx))->real))
            ERROR("Invalid GLX context");
    }
    
    if (SDL_GL_MakeCurrent(ctx->window, glctx) < 0)
        ERROR("Unable to make a context current");
    
    if (glctx) {
        reload_gl_funcs(ctx);
        trc_set_current_gl_context(ctx->trace, trc_lookup_name(global_ns, TrcContext, p_ctx, -1));
        if (!trc_gl_state_get_made_current_before(ctx->trace)) {
            trace_extra_t* extra = trc_get_extra(cmd, "replay/glXMakeCurrent/drawable_size");
            if (!extra) ERROR("replay/glXMakeCurrent/drawable_size extra not found");
            if (extra->size != 8) ERROR("replay/glXMakeCurrent/drawable_size is not 8 bytes");
            int32_t width = ((int32_t*)extra->data)[0];
            int32_t height = ((int32_t*)extra->data)[1];
            if (width>=0 && height>=0) {
                trc_gl_state_set_drawable_width(ctx->trace, width);
                trc_gl_state_set_drawable_height(ctx->trace, height);
                SDL_SetWindowSize(ctx->window, width, height);
                for (size_t i = 0; i < trc_gl_state_get_state_int(ctx->trace, GL_MAX_VIEWPORTS, 0); i++) {
                    trc_gl_state_set_state_float(ctx->trace, GL_VIEWPORT, i*4+2, width);
                    trc_gl_state_set_state_float(ctx->trace, GL_VIEWPORT, i*4+3, height);
                    trc_gl_state_set_state_int(ctx->trace, GL_SCISSOR_BOX, i*4+2, width);
                    trc_gl_state_set_state_int(ctx->trace, GL_SCISSOR_BOX, i*4+3, height);
                    if (i == 0) {
                        F(glViewport)(0, 0, width, height);
                        F(glScissor)(0, 0, width, height);
                    } else {
                        F(glViewportIndexedf)(i, 0, 0, width, height);
                        F(glScissorIndexed)(i, 0, 0, width, height);
                    }
                }
            }
        }
        trc_gl_state_set_made_current_before(ctx->trace, true);
    } else {
        reset_gl_funcs(ctx);
        trc_set_current_gl_context(ctx->trace, NULL);
    }
    
    //Seems to be messing up the front buffer.
    //But the front buffer is still sometimes black when it should not be.
    /*SDL_GL_SetSwapInterval(0)*/

glXSwapIntervalEXT: //Display* p_dpy, GLXDrawable p_drawable, int p_interval
    ;

glXSwapIntervalSGI: //int p_interval
    ;

glXGetProcAddressARB: //const GLubyte* p_procName
    ;

glXGetProcAddress: //const GLubyte* p_procName
    ;

glXQueryExtension: //Display* p_dpy, int * p_errorb, int * p_event
    ;

glXQueryVersion: //Display* p_dpy, int * p_maj, int * p_min
    ;

glXQueryDrawable: //Display* p_dpy, GLXDrawable p_draw, int p_attribute, unsigned int * p_value
    ;

glXChooseVisual: //Display* p_dpy, int p_screen, int * p_attribList
    ;

glXChooseFBConfig: //Display* p_dpy, int p_screen, const int * p_attrib_list, int * p_nelements
    ;

glXGetFBConfigs: //Display* p_dpy, int p_screen, int * p_nelements
    ;

glXGetFBConfigAttrib: //Display* p_dpy, GLXFBConfig p_config, int p_attribute, int * p_value
    ;

glXGetVisualFromFBConfig: //Display* p_dpy, GLXFBConfig p_config
    ;

glXChooseFBConfigSGIX: //Display* p_dpy, int p_screen, int * p_attrib_list, int * p_nelements
    ;

glXGetFBConfigAttribSGIX: //Display* p_dpy, GLXFBConfigSGIX p_config, int p_attribute, int * p_value
    ;

glXGetVisualFromFBConfigSGIX: //Display* p_dpy, GLXFBConfigSGIX p_config
    ;

glXGetClientString: //Display* p_dpy, int p_name
    ;

glXCreateContext: //Display* p_dpy, XVisualInfo* p_vis, GLXContext p_shareList, Bool p_direct
    trc_namespace_t* global_ns = &ctx->trace->inspection.global_namespace;
    
    const trc_gl_context_rev_t* shareList = NULL;
    if (p_shareList) {
        shareList = trc_get_obj(global_ns, TrcContext, (uint64_t)p_shareList);
        if (!shareList) ERROR("Invalid share context name");
    }
    
    SDL_GLContext last_ctx = SDL_GL_GetCurrentContext();
    if (shareList) {
        SDL_GL_MakeCurrent(ctx->window, shareList->real);
        SDL_GL_SetAttribute(SDL_GL_SHARE_WITH_CURRENT_CONTEXT, 1);
    } else {
        SDL_GL_SetAttribute(SDL_GL_SHARE_WITH_CURRENT_CONTEXT, 0);
    }
    
    SDL_GLContext res = SDL_GL_CreateContext(ctx->window);
    if (!res) {
        trc_add_error(cmd, "Unable to create context: %s", SDL_GetError());
        SDL_GL_MakeCurrent(ctx->window, last_ctx);
        RETURN;
    }
    reload_gl_funcs(ctx);
    
    trc_gl_context_rev_t rev;
    rev.real = res;
    if (shareList) rev.namespace = shareList->namespace;
    else rev.namespace = trc_create_namespace(ctx->trace);
    uint64_t fake = trc_get_ptr(&cmd->ret)[0];
    trc_obj_t* cur_ctx = trc_create_named_obj(global_ns, TrcContext, fake, &rev);
    
    trc_obj_t* prev_ctx = trc_get_current_gl_context(ctx->trace, -1);
    size_t end = ctx->trace->inspection.cur_ctx_revision_count - 1;
    ctx->trace->inspection.cur_ctx_revisions[end].context.obj = cur_ctx; //TODO: A hack
    init_context(ctx);
    ctx->trace->inspection.cur_ctx_revisions[end].context.obj = prev_ctx;
    
    SDL_GL_MakeCurrent(ctx->window, last_ctx);
    reload_gl_funcs(ctx);

glXCreateContextAttribsARB: //Display* p_dpy, GLXFBConfig p_config, GLXContext p_share_context, Bool p_direct, const int* p_attrib_list
    int last_major, last_minor, last_flags, last_profile;
    SDL_GL_GetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, &last_major);
    SDL_GL_GetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, &last_minor);
    SDL_GL_GetAttribute(SDL_GL_CONTEXT_FLAGS, &last_flags);
    SDL_GL_GetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, &last_profile);
    
    int major = last_major;
    int minor = last_minor;
    int flags = 0;
    int profile = 0;
    
    const int* attribs = p_attrib_list;
    while (*attribs) {
        int attr = *(attribs++);
        if (attr == GLX_CONTEXT_MAJOR_VERSION_ARB) {
            major = *(attribs++);
        } else if (attr == GLX_CONTEXT_MINOR_VERSION_ARB) {
            minor = *(attribs++);
        } else if (attr == GLX_CONTEXT_FLAGS_ARB) {
            int glx_flags = *(attribs++);
            flags = 0;
            if (glx_flags & GLX_CONTEXT_DEBUG_BIT_ARB)
                flags |= SDL_GL_CONTEXT_DEBUG_FLAG;
            if (glx_flags & GLX_CONTEXT_FORWARD_COMPATIBLE_BIT_ARB)
                flags |= SDL_GL_CONTEXT_FORWARD_COMPATIBLE_FLAG;
        } else if (attr == GLX_CONTEXT_PROFILE_MASK_ARB) {
            int mask = *(attribs++);
            profile = 0;
            if (mask & GLX_CONTEXT_CORE_PROFILE_BIT_ARB)
                profile = SDL_GL_CONTEXT_PROFILE_CORE;
            if (mask & GLX_CONTEXT_COMPATIBILITY_PROFILE_BIT_ARB)
                profile = SDL_GL_CONTEXT_PROFILE_COMPATIBILITY;
        } else {
            trc_add_warning(cmd, "Unnamed attribute: %d", attr);
            attribs++;
        }
    }
    
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, major);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, minor);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, flags);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, profile);
    
    trc_namespace_t* global_ns = &ctx->trace->inspection.global_namespace;
    
    const trc_gl_context_rev_t* share_ctx = NULL;
    if (p_share_context) {
        share_ctx = trc_get_obj(global_ns, TrcContext, (uint64_t)p_share_context);
        if (!share_ctx) ERROR("Invalid share context name");
    }
    
    SDL_GLContext last_ctx = SDL_GL_GetCurrentContext();
    if (share_ctx) {
        SDL_GL_MakeCurrent(ctx->window, share_ctx->real);
        SDL_GL_SetAttribute(SDL_GL_SHARE_WITH_CURRENT_CONTEXT, 1);
    } else {
        SDL_GL_SetAttribute(SDL_GL_SHARE_WITH_CURRENT_CONTEXT, 0);
    }
    
    SDL_GLContext res = SDL_GL_CreateContext(ctx->window);
    if (!res) {
        trc_add_error(cmd, "Unable to create context: %s", SDL_GetError());
        SDL_GL_MakeCurrent(ctx->window, last_ctx);
        RETURN;
    }
    reload_gl_funcs(ctx);
    
    trc_gl_context_rev_t rev;
    rev.real = res;
    if (share_ctx) rev.namespace = share_ctx->namespace;
    else rev.namespace = trc_create_namespace(ctx->trace);
    uint64_t fake = trc_get_ptr(&cmd->ret)[0];
    trc_obj_t* cur_ctx = trc_create_named_obj(global_ns, TrcContext, fake, &rev);
    
    trc_obj_t* prev_ctx = trc_get_current_gl_context(ctx->trace, -1);
    size_t end = ctx->trace->inspection.cur_ctx_revision_count - 1;
    ctx->trace->inspection.cur_ctx_revisions[end].context.obj = cur_ctx; //TODO: A hack
    init_context(ctx);
    ctx->trace->inspection.cur_ctx_revisions[end].context.obj = prev_ctx;
    
    SDL_GL_MakeCurrent(ctx->window, last_ctx);
    reload_gl_funcs(ctx);
    
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, last_major);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, last_minor);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, last_flags);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, last_profile);

glXQueryExtensionsString: //Display* p_dpy, int p_screen
    ;

glXDestroyContext: //Display* p_dpy, GLXContext p_ctx
    trc_namespace_t* global_ns = &ctx->trace->inspection.global_namespace;
    SDL_GLContext glctx = ((trc_gl_context_rev_t*)trc_get_obj(global_ns, TrcContext, (uint64_t)p_ctx))->real;
    if (!glctx) ERROR("Invalid context name");
    
    SDL_GL_DeleteContext(glctx);
    
    delete_obj(global_ns, p_ctx, TrcContext);

glXSwapBuffers: //Display* p_dpy, GLXDrawable p_drawable
    if (!trc_get_current_gl_context(ctx->trace, -1)) ERROR("No current OpenGL context");
    SDL_GL_SwapWindow(ctx->window);
    replay_update_fb0_buffers(ctx, false, true, false, false);

wip15DrawableSize: //GLsizei p_width, GLsizei p_height
    if (p_width < 0) p_width = 100;
    if (p_height < 0) p_height = 100;
    
    SDL_SetWindowSize(ctx->window, p_width, p_height);
    
    trc_gl_context_rev_t state = *trc_get_context(ctx->trace);
    if (state.drawable_width==p_width && state.drawable_height==p_height) return;
    state.drawable_width = p_width;
    state.drawable_height = p_height;
    trc_set_context(ctx->trace, &state);
    replay_update_fb0_buffers(ctx, true, true, true, true);

glClear: //GLbitfield p_mask
    real(p_mask);
    update_buffers(ctx, trc_gl_state_get_draw_framebuffer(ctx->trace), p_mask);

glGenTextures: //GLsizei p_n, GLuint* p_textures
    if (p_n < 0) ERROR("Invalid texture name count");
    GLuint* textures = replay_alloc(p_n*sizeof(GLuint));
    real(p_n, textures);
    gen_textures(ctx, p_n, textures, p_textures, false, 0);

glCreateTextures: //GLenum p_target, GLsizei p_n, GLuint* p_textures
    if (p_n < 0) ERROR("Invalid texture name count");
    GLuint* textures = replay_alloc(p_n*sizeof(GLuint));
    real(p_target, p_n, textures);
    gen_textures(ctx, p_n, textures, p_textures, true, p_target);

glDeleteTextures: //GLsizei p_n, const GLuint* p_textures
    GLuint* textures = replay_alloc(p_n*sizeof(GLuint));
    for (size_t i = 0; i < p_n; ++i)
        if (!(textures[i] = get_real_texture(ctx->ns, p_textures[i])) && p_textures[i]) {
            trc_add_warning(cmd, "Invalid texture name");
        } else {
            trc_obj_t* obj = get_texture(ctx->ns, p_textures[i])->head.obj;
            
            //Reset targets
            GLenum targets[11] = {
                GL_TEXTURE_1D, GL_TEXTURE_2D, GL_TEXTURE_3D, GL_TEXTURE_1D_ARRAY,
                GL_TEXTURE_2D_ARRAY, GL_TEXTURE_RECTANGLE, GL_TEXTURE_CUBE_MAP,
                GL_TEXTURE_CUBE_MAP_ARRAY, GL_TEXTURE_BUFFER, GL_TEXTURE_2D_MULTISAMPLE,
                GL_TEXTURE_2D_MULTISAMPLE_ARRAY};
            for (size_t j = 0; j < 11; j++) {
                for (size_t k = 0; k < trc_gl_state_get_bound_textures_size(ctx->trace, targets[j]); k++) {
                    if (trc_gl_state_get_bound_textures(ctx->trace, targets[j], k) == obj)
                        trc_gl_state_set_bound_textures(ctx->trace, targets[j], k, NULL);
                }
            }
            
            //TODO: Remove from framebuffers?
            
            delete_obj(ctx->ns, p_textures[i], TrcTexture);
        }
    real(p_n, textures);

glActiveTexture: //GLenum p_texture
    if (p_texture<GL_TEXTURE0 || p_texture-GL_TEXTURE0>=trc_gl_state_get_state_int(ctx->trace, GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS, 0))
        ERROR("No such texture unit");
    trc_gl_state_set_active_texture_unit(ctx->trace, p_texture-GL_TEXTURE0);
    real(p_texture);

glBindTexture: //GLenum p_target, GLuint p_texture
    const trc_gl_texture_rev_t* rev = get_texture(ctx->ns, p_texture);
    if (!rev && p_texture) ERROR("Invalid texture name");
    real(p_target, p_texture?rev->real:0);
    if (rev && !rev->has_object) {
        trc_gl_texture_rev_t newrev = *rev;
        newrev.has_object = true;
        newrev.type = p_target;
        newrev.images = trc_create_data(ctx->trace, 0, NULL, TRC_DATA_IMMUTABLE);
        newrev.sample_params.min_filter = p_target==GL_TEXTURE_RECTANGLE ? GL_LINEAR : GL_NEAREST_MIPMAP_LINEAR;
        GLenum wrap_mode = p_target==GL_TEXTURE_RECTANGLE ? GL_CLAMP_TO_EDGE : GL_REPEAT;
        newrev.sample_params.wrap_s = wrap_mode;
        newrev.sample_params.wrap_t = wrap_mode;
        newrev.sample_params.wrap_r = wrap_mode;
        set_texture(&newrev);
    } else if (rev && rev->type!=p_target) {
        ERROR("Invalid target for texture object");
    }
    uint unit = trc_gl_state_get_active_texture_unit(ctx->trace);
    trc_gl_state_set_bound_textures(ctx->trace, p_target, unit, rev?rev->head.obj:NULL);

glBindTextureUnit: //GLuint p_unit, GLuint p_texture
    if (p_unit>=trc_gl_state_get_state_int(ctx->trace, GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS, 0))
        ERROR("Invalid unit");
    if (!p_texture_rev) ERROR("Invalid texture name");
    if (!p_texture_rev->has_object) ERROR("Texture name has no object");
    real(p_unit, p_texture_rev->real);
    if (p_texture) {
        trc_gl_state_set_bound_textures(ctx->trace, p_texture_rev->type, p_unit, p_texture_rev->head.obj);
    } else {
        GLenum targets[] = {GL_TEXTURE_1D, GL_TEXTURE_2D, GL_TEXTURE_3D,
                            GL_TEXTURE_1D_ARRAY, GL_TEXTURE_2D_ARRAY,
                            GL_TEXTURE_RECTANGLE, GL_TEXTURE_BUFFER,
                            GL_TEXTURE_CUBE_MAP, GL_TEXTURE_CUBE_MAP_ARRAY,
                            GL_TEXTURE_2D_MULTISAMPLE, GL_TEXTURE_2D_MULTISAMPLE_ARRAY};
        for (size_t i = 0; i < sizeof(targets)/sizeof(targets[0]); i++)
            trc_gl_state_set_bound_textures(ctx->trace, targets[i], p_unit, NULL);
    }

glBindTextures: //GLuint p_first, GLsizei p_count, const GLuint* p_textures
    if (p_first+p_count>trc_gl_state_get_state_int(ctx->trace, GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS, 0) || p_count<0)
        ERROR("Invalid range");
    GLuint* real_tex = replay_alloc(p_count*sizeof(GLuint));
    //TODO: This probably creates a new trc_data_t for each texture
    for (size_t i = p_first; i < p_first+p_count; i++) {
        const trc_gl_texture_rev_t* rev = get_texture(ctx->ns, p_textures[i]);
        if (!rev) ERROR("Invalid texture name at index %zu", i);
        if (!rev->has_object) ERROR("Texture name at index %zu has no object", i);
        real_tex[i] = rev->real;
        
        if (p_textures[i]) {
            trc_gl_state_set_bound_textures(ctx->trace, rev->type, i, trc_lookup_name(ctx->ns, TrcTexture, p_textures[i], -1));
        } else {
            GLenum targets[] = {GL_TEXTURE_1D, GL_TEXTURE_2D, GL_TEXTURE_3D,
                                GL_TEXTURE_1D_ARRAY, GL_TEXTURE_2D_ARRAY,
                                GL_TEXTURE_RECTANGLE, GL_TEXTURE_BUFFER,
                                GL_TEXTURE_CUBE_MAP, GL_TEXTURE_CUBE_MAP_ARRAY,
                                GL_TEXTURE_2D_MULTISAMPLE, GL_TEXTURE_2D_MULTISAMPLE_ARRAY};
            for (size_t j = 0; j < sizeof(targets)/sizeof(targets[0]); j++)
                trc_gl_state_set_bound_textures(ctx->trace, targets[j], i, NULL);
        }
    }
    real(p_first, p_count, real_tex);

//TODO: Support malsized data in gl*Image*D
glTexImage1D: //GLenum p_target, GLint p_level, GLint p_internalformat, GLsizei p_width, GLint p_border, GLenum p_format, GLenum p_type, const void* p_pixels
    if (tex_image(ctx, cmd, false, p_target, p_level, p_internalformat, p_border, p_format, p_type, false, 1, p_width)) {
        real(p_target, p_level, p_internalformat, p_width, p_border, p_format, p_type, p_pixels);
        replay_update_bound_tex_image(ctx, cmd, p_target, p_level);
    }

glCompressedTexImage1D: //GLenum p_target, GLint p_level, GLenum p_internalformat, GLsizei p_width, GLint p_border, GLsizei p_imageSize, const void* p_data
    real(p_target, p_level, p_internalformat, p_width, p_border, p_imageSize, p_data);
    replay_update_bound_tex_image(ctx, cmd, p_target, p_level);

glTexSubImage1D: //GLenum p_target, GLint p_level, GLint p_xoffset, GLsizei p_width, GLenum p_format, GLenum p_type, const void* p_pixels
    if (tex_image(ctx, cmd, false, p_target, p_level, 0, 0, p_format, p_type, true, 1, p_width, p_xoffset)) {
        real(p_target, p_level, p_xoffset, p_width, p_format, p_type, p_pixels);
        replay_update_bound_tex_image(ctx, cmd, p_target, p_level);
    }

glCompressedTexSubImage1D: //GLenum p_target, GLint p_level, GLint p_xoffset, GLsizei p_width, GLenum p_format, GLsizei p_imageSize, const void* p_data
    real(p_target, p_level, p_xoffset, p_width, p_format, p_imageSize, p_data);
    replay_update_bound_tex_image(ctx, cmd, p_target, p_level);

glTexImage2D: //GLenum p_target, GLint p_level, GLint p_internalformat, GLsizei p_width, GLsizei p_height, GLint p_border, GLenum p_format, GLenum p_type, const void* p_pixels
    if (tex_image(ctx, cmd, false, p_target, p_level, p_internalformat, p_border, p_format, p_type, false, 2, p_width, p_height)) {
        real(p_target, p_level, p_internalformat, p_width, p_height, p_border, p_format, p_type, p_pixels);
        replay_update_bound_tex_image(ctx, cmd, p_target, p_level);
    }

glCompressedTexImage2D: //GLenum p_target, GLint p_level, GLenum p_internalformat, GLsizei p_width, GLsizei p_height, GLint p_border, GLsizei p_imageSize, const void* p_data
    real(p_target, p_level, p_internalformat, p_width, p_height, p_border, p_imageSize, p_data);
    replay_update_bound_tex_image(ctx, cmd, p_target, p_level);

glTexSubImage2D: //GLenum p_target, GLint p_level, GLint p_xoffset, GLint p_yoffset, GLsizei p_width, GLsizei p_height, GLenum p_format, GLenum p_type, const void* p_pixels
    if (tex_image(ctx, cmd, false, p_target, p_level, 0, 0, p_format, p_type, true, 2, p_width, p_height, p_xoffset, p_yoffset)) {
        real(p_target, p_level, p_xoffset, p_yoffset, p_width, p_height, p_format, p_type, p_pixels);
        replay_update_bound_tex_image(ctx, cmd, p_target, p_level);
    }

glCompressedTexSubImage2D: //GLenum p_target, GLint p_level, GLint p_xoffset, GLint p_yoffset, GLsizei p_width, GLsizei p_height, GLenum p_format, GLsizei p_imageSize, const void* p_data
    real(p_target, p_level, p_xoffset, p_yoffset, p_width, p_height, p_format, p_imageSize, p_data);
    replay_update_bound_tex_image(ctx, cmd, p_target, p_level);

glTexImage3D: //GLenum p_target, GLint p_level, GLint p_internalformat, GLsizei p_width, GLsizei p_height, GLsizei p_depth, GLint p_border, GLenum p_format, GLenum p_type, const void* p_pixels
    //TODO: Array textures?
    if (tex_image(ctx, cmd, false, p_target, p_level, p_internalformat, p_border, p_format, p_type, false, 3, p_width, p_height, p_depth)) {
        real(p_target, p_level, p_internalformat, p_width, p_height, p_depth, p_border, p_format, p_type, p_pixels);
        replay_update_bound_tex_image(ctx, cmd, p_target, p_level);
    }

glCompressedTexImage3D: //GLenum p_target, GLint p_level, GLenum p_internalformat, GLsizei p_width, GLsizei p_height, GLsizei p_depth, GLint p_border, GLsizei p_imageSize, const void* p_data
    //TODO: Array textures
    real(p_target, p_level, p_internalformat, p_width, p_height, p_depth, p_border, p_imageSize, p_data);
    replay_update_bound_tex_image(ctx, cmd, p_target, p_level);

glTexSubImage3D: //GLenum p_target, GLint p_level, GLint p_xoffset, GLint p_yoffset, GLint p_zoffset, GLsizei p_width, GLsizei p_height, GLsizei p_depth, GLenum p_format, GLenum p_type, const void* p_pixels
    if (tex_image(ctx, cmd, false, p_target, p_level, 0, 0, p_format, p_type, true, 3, p_width, p_height, p_depth, p_xoffset, p_yoffset, p_zoffset)) {
        real(p_target, p_level, p_xoffset, p_yoffset, p_zoffset, p_width, p_height, p_depth, p_format, p_type, p_pixels);
        replay_update_bound_tex_image(ctx, cmd, p_target, p_level);
    }

glCompressedTexSubImage3D: //GLenum p_target, GLint p_level, GLint p_xoffset, GLint p_yoffset, GLint p_zoffset, GLsizei p_width, GLsizei p_height, GLsizei p_depth, GLenum p_format, GLsizei p_imageSize, const void* p_data
    real(p_target, p_level, p_xoffset, p_yoffset, p_zoffset, p_width, p_height, p_depth, p_format, p_imageSize, p_data);
    replay_update_bound_tex_image(ctx, cmd, p_target, p_level);

glTexImage2DMultisample: //GLenum p_target, GLsizei p_samples, GLenum p_internalformat, GLsizei p_width, GLsizei p_height, GLboolean p_fixedsamplelocations
    real(p_target, p_samples, p_internalformat, p_width, p_height, p_fixedsamplelocations);
    //TODO
    //replay_get_tex_params(ctx, cmd, target);

glTexImage3DMultisample: //GLenum p_target, GLsizei p_samples, GLenum p_internalformat, GLsizei p_width, GLsizei p_height, GLsizei p_depth, GLboolean p_fixedsamplelocations
    real(p_target, p_samples, p_internalformat, p_width, p_height, p_depth, p_fixedsamplelocations);
    //TODO
    //replay_get_tex_params(ctx, cmd, target);

glCopyTexImage1D: //GLenum p_target, GLint p_level, GLenum p_internalformat, GLint p_x, GLint p_y, GLsizei p_width, GLint p_border
    real(p_target, p_level, p_internalformat, p_x, p_y, p_width, p_border);
    replay_update_bound_tex_image(ctx, cmd, p_target, p_level);

glCopyTexImage2D: //GLenum p_target, GLint p_level, GLenum p_internalformat, GLint p_x, GLint p_y, GLsizei p_width, GLsizei p_height, GLint p_border
    real(p_target, p_level, p_internalformat, p_x, p_y, p_width, p_height, p_border);
    replay_update_bound_tex_image(ctx, cmd, p_target, p_level);

glCopyTexSubImage1D: //GLenum p_target, GLint p_level, GLint p_xoffset, GLint p_x, GLint p_y, GLsizei p_width
    real(p_target, p_level, p_xoffset, p_x, p_y, p_width);
    replay_update_bound_tex_image(ctx, cmd, p_target, p_level);

glCopyTexSubImage2D: //GLenum p_target, GLint p_level, GLint p_xoffset, GLint p_yoffset, GLint p_x, GLint p_y, GLsizei p_width, GLsizei p_height
    real(p_target, p_level, p_xoffset, p_yoffset, p_x, p_y, p_width, p_height);
    replay_update_bound_tex_image(ctx, cmd, p_target, p_level);

glCopyTexSubImage3D: //GLenum p_target, GLint p_level, GLint p_xoffset, GLint p_yoffset, GLint p_zoffset, GLint p_x, GLint p_y, GLsizei p_width, GLsizei p_height
    real(p_target, p_level, p_xoffset, p_yoffset, p_zoffset, p_x, p_y, p_width, p_height);
    replay_update_bound_tex_image(ctx, cmd, p_target, p_level);

glTexBuffer: //GLenum p_target, GLenum p_internalformat, GLuint p_buffer
    if (tex_buffer(ctx, cmd, p_target, false, p_internalformat, p_buffer, 0, -1))
        real(p_target, p_internalformat, p_buffer?p_buffer_rev->real:0);

glTexBufferRange: //GLenum p_target, GLenum p_internalformat, GLuint p_buffer, GLintptr p_offset, GLsizeiptr p_size
    if (tex_buffer(ctx, cmd, p_target, false, p_internalformat, p_buffer, p_offset, p_size))
        real(p_target, p_internalformat, p_buffer?p_buffer_rev->real:0, p_offset, p_size);

glTextureBuffer: //GLuint p_texture, GLenum p_internalformat, GLuint p_buffer
    if (tex_buffer(ctx, cmd, p_texture, true, p_internalformat, p_buffer, 0, -1))
        real(p_texture_rev->real, p_internalformat, p_buffer?p_buffer_rev->real:0);

glTextureBufferRange: //GLuint p_texture, GLenum p_internalformat, GLuint p_buffer, GLintptr p_offset, GLsizeiptr p_size
    if (tex_buffer(ctx, cmd, p_texture, true, p_internalformat, p_buffer, p_offset, p_size))
        real(p_texture_rev->real, p_internalformat, p_buffer?p_buffer_rev->real:0, p_offset, p_size);

glGenerateMipmap: //GLenum p_target
    real(p_target);
    
    uint unit = trc_gl_state_get_active_texture_unit(ctx->trace);
    trc_obj_t* tex = trc_gl_state_get_bound_textures(ctx->trace, p_target, unit);
    if (!tex) ERROR("No texture bound");
    
    GLint base;
    F(glGetTexParameteriv)(p_target, GL_TEXTURE_BASE_LEVEL, &base);
    GLint w, h, d;
    F(glGetTexLevelParameteriv)(p_target, base, GL_TEXTURE_WIDTH, &w);
    F(glGetTexLevelParameteriv)(p_target, base, GL_TEXTURE_HEIGHT, &h);
    F(glGetTexLevelParameteriv)(p_target, base, GL_TEXTURE_DEPTH, &d);
    uint level = base;
    while (w || h || d) {
        if (level != base)
            replay_update_bound_tex_image(ctx, cmd, p_target, level);
        w /= 2;
        h /= 2;
        d /= 2;
        level++;
    }

glTexParameterf: //GLenum p_target, GLenum p_pname, GLfloat p_param
    GLdouble double_param = p_param;
    if (!texture_param_double(ctx, cmd, false, p_target, p_pname, 1, &double_param))
        real(p_target, p_pname, p_param);

glTexParameteri: //GLenum p_target, GLenum p_pname, GLint p_param
    GLdouble double_param = p_param;
    if (!texture_param_double(ctx, cmd, false, p_target, p_pname, 1, &double_param))
        real(p_target, p_pname, p_param);

glTexParameterfv: //GLenum p_target, GLenum p_pname, const GLfloat* p_params
    const double* paramsd = trc_get_double(arg_params);
    if (!texture_param_double(ctx, cmd, false, p_target, p_pname, arg_params->count, paramsd))
        real(p_target, p_pname, p_params);

glTexParameteriv: //GLenum p_target, GLenum p_pname, const GLint* p_params
    const int64_t* params64 = trc_get_int(arg_params);
    double* double_params = replay_alloc(arg_params->count*sizeof(double));
    if (p_pname == GL_TEXTURE_BORDER_COLOR)
        conv_from_signed_norm_array_i64(ctx->trace, arg_params->count, double_params, params64, 32);
    else
        for (size_t i = 0; i < arg_params->count; i++) double_params[i] = params64[i];
    if (!texture_param_double(ctx, cmd, false, p_target, p_pname, arg_params->count, double_params))
        real(p_target, p_pname, p_params);

glTexParameterIiv: //GLenum p_target, GLenum p_pname, const GLint* p_params
    const int64_t* params64 = trc_get_int(arg_params);
    double* double_params = replay_alloc(arg_params->count*sizeof(double));
    for (size_t i = 0; i < arg_params->count; i++) double_params[i] = params64[i];
    if (!texture_param_double(ctx, cmd, false, p_target, p_pname, arg_params->count, double_params))
        real(p_target, p_pname, p_params);

glTexParameterIuiv: //GLenum p_target, GLenum p_pname, const GLuint* p_params
    const uint64_t* params64 = trc_get_uint(arg_params);
    double* double_params = replay_alloc(arg_params->count*sizeof(double));
    for (size_t i = 0; i < arg_params->count; i++) double_params[i] = params64[i];
    if (!texture_param_double(ctx, cmd, false, p_target, p_pname, arg_params->count, double_params))
        real(p_target, p_pname, p_params);

glTextureParameterf: //GLuint p_texture, GLenum p_pname, GLfloat p_param
    GLdouble double_param = p_param;
    if (!texture_param_double(ctx, cmd, true, p_texture, p_pname, 1, &double_param))
        real(p_texture_rev->real, p_pname, p_param);

glTextureParameteri: //GLuint p_texture, GLenum p_pname, GLint p_param
    GLdouble double_param = p_param;
    if (!texture_param_double(ctx, cmd, true, p_texture, p_pname, 1, &double_param))
        real(p_texture_rev->real, p_pname, p_param);

glTextureParameterfv: //GLuint p_texture, GLenum p_pname, const GLfloat* p_param
    const double* paramsd = trc_get_double(arg_param);
    if (!texture_param_double(ctx, cmd, true, p_texture, p_pname, arg_param->count, paramsd))
        real(p_texture_rev->real, p_pname, p_param);

glTextureParameteriv: //GLuint p_texture, GLenum p_pname, const GLint* p_param
    double* double_params = replay_alloc(arg_param->count*sizeof(double));
    if (p_pname == GL_TEXTURE_BORDER_COLOR)
        conv_from_signed_norm_array_i32(ctx->trace, arg_param->count, double_params, p_param, 32);
    else
        for (size_t i = 0; i < arg_param->count; i++) double_params[i] = p_param[i];
    if (!texture_param_double(ctx, cmd, true, p_texture, p_pname, arg_param->count, double_params))
        real(p_texture_rev->real, p_pname, p_param);

glTextureParameterIiv: //GLuint p_texture, GLenum p_pname, const GLint* p_params
    double* double_params = replay_alloc(arg_params->count*sizeof(double));
    for (size_t i = 0; i < arg_params->count; i++) double_params[i] = p_params[i];
    if (!texture_param_double(ctx, cmd, true, p_texture, p_pname, arg_params->count, double_params))
        real(p_texture_rev->real, p_pname, p_params);

glTextureParameterIuiv: //GLuint p_texture, GLenum p_pname, const GLuint* p_params
    double* double_params = replay_alloc(arg_params->count*sizeof(double));
    for (size_t i = 0; i < arg_params->count; i++) double_params[i] = p_params[i];
    if (!texture_param_double(ctx, cmd, true, p_texture, p_pname, arg_params->count, double_params))
        real(p_texture_rev->real, p_pname, p_params);

glGenBuffers: //GLsizei p_n, GLuint* p_buffers
    if (p_n < 0) ERROR("Invalid buffer name count");
    GLuint* buffers = replay_alloc(p_n*sizeof(GLuint));
    real(p_n, buffers);
    gen_buffers(ctx, p_n, buffers, p_buffers, false);

glCreateBuffers: //GLsizei p_n, GLuint* p_buffers
    if (p_n < 0) ERROR("Invalid buffer name count");
    GLuint* buffers = replay_alloc(p_n*sizeof(GLuint));
    real(p_n, buffers);
    gen_buffers(ctx, p_n, buffers, p_buffers, true);

glDeleteBuffers: //GLsizei p_n, const GLuint* p_buffers
    GLuint* buffers = replay_alloc(p_n*sizeof(GLuint));
    for (size_t i = 0; i < p_n; ++i) {
        if (!(buffers[i] = get_real_buffer(ctx->ns, p_buffers[i])) && p_buffers[i])
            trc_add_warning(cmd, "Invalid buffer name");
        else {
            trc_obj_t* obj = get_buffer(ctx->ns, p_buffers[i])->head.obj;
            
            //Reset targets
            GLenum targets[14] = {
                GL_ARRAY_BUFFER, GL_ATOMIC_COUNTER_BUFFER, GL_COPY_READ_BUFFER,
                GL_COPY_WRITE_BUFFER, GL_DISPATCH_INDIRECT_BUFFER, GL_DRAW_INDIRECT_BUFFER,
                GL_ELEMENT_ARRAY_BUFFER, GL_PIXEL_PACK_BUFFER, GL_PIXEL_UNPACK_BUFFER,
                GL_QUERY_BUFFER, GL_SHADER_STORAGE_BUFFER, GL_TEXTURE_BUFFER,
                GL_TRANSFORM_FEEDBACK_BUFFER, GL_UNIFORM_BUFFER};
            for (size_t j = 0; j < 14; j++) {
                if (trc_gl_state_get_bound_buffer(ctx->trace, targets[j]) == obj)
                    bind_buffer(ctx, targets[j], 0);
            }
            
            GLenum indexed_targets[4] = {
                GL_TRANSFORM_FEEDBACK_BUFFER, GL_UNIFORM_BUFFER,
                GL_SHADER_STORAGE_BUFFER, GL_ATOMIC_COUNTER_BUFFER};
            for (size_t j = 0; j < 4; j++) {
                size_t count = trc_gl_state_get_bound_buffer_indexed_size(ctx->trace, indexed_targets[j]);
                for (size_t k = 0; k < count; k++) {
                    if (trc_gl_state_get_bound_buffer_indexed(ctx->trace, indexed_targets[j], k).buf.obj == obj)
                        bind_buffer_indexed(ctx, indexed_targets[j], k, 0);
                }
            }
            
            delete_obj(ctx->ns, p_buffers[i], TrcBuffer);
        }
    }
    real(p_n, buffers);

glBindBuffer: //GLenum p_target, GLuint p_buffer
    if (get_current_tf(ctx)->active_not_paused && p_target==GL_TRANSFORM_FEEDBACK_BUFFER)
        ERROR("Cannot modify GL_TRANSFORM_FEEDBACK_BUFFER when transform feedback is active and not paused");
    const trc_gl_buffer_rev_t* rev = get_buffer(ctx->ns, p_buffer);
    if (!rev && p_buffer) ERROR("Invalid buffer name");
    real(p_target, p_buffer?rev->real:0);
    bind_buffer(ctx, p_target, p_buffer);

glBindBufferBase: //GLenum p_target, GLuint p_index, GLuint p_buffer
    if (get_current_tf(ctx)->active_not_paused && p_target==GL_TRANSFORM_FEEDBACK_BUFFER)
        ERROR("Cannot modify GL_TRANSFORM_FEEDBACK_BUFFER when transform feedback is active and not paused");
    if (p_index >= trc_gl_state_get_bound_buffer_indexed_size(ctx->trace, p_target))
        ERROR("Invalid index");
    const trc_gl_buffer_rev_t* rev = get_buffer(ctx->ns, p_buffer);
    if (!rev && p_buffer) ERROR("Invalid buffer name");
    real(p_target, p_index, p_buffer?rev->real:0);
    bind_buffer(ctx, p_target, p_buffer);
    bind_buffer_indexed(ctx, p_target, p_index, p_buffer);

glBindBufferRange: //GLenum p_target, GLuint p_index, GLuint p_buffer, GLintptr p_offset, GLsizeiptr p_size
    if (get_current_tf(ctx)->active_not_paused && p_target==GL_TRANSFORM_FEEDBACK_BUFFER)
        ERROR("Cannot modify GL_TRANSFORM_FEEDBACK_BUFFER when transform feedback is active and not paused");
    if (p_index >= trc_gl_state_get_bound_buffer_indexed_size(ctx->trace, p_target))
        ERROR("Invalid index");
    const trc_gl_buffer_rev_t* rev = get_buffer(ctx->ns, p_buffer);
    if (!rev && p_buffer) ERROR("Invalid buffer name");
    if (rev && (p_size<=0 || p_offset+p_size>rev->data->size))
        ERROR("Invalid range");
    //TODO: Check alignment of offset
    real(p_target, p_index, p_buffer?rev->real:0, p_offset, p_size);
    
    bind_buffer(ctx, p_target, p_buffer);
    bind_buffer_indexed_ranged(ctx, p_target, p_index, p_buffer, p_offset, p_size);

glBufferData: //GLenum p_target, GLsizeiptr p_size, const void* p_data, GLenum p_usage
    if (buffer_data(ctx, cmd, false, get_bound_buffer(ctx, p_target), p_size, p_data, p_usage))
        real(p_target, p_size, p_data, p_usage);

glNamedBufferData: //GLuint p_buffer, GLsizeiptr p_size, const void* p_data, GLenum p_usage
    if (buffer_data(ctx, cmd, true, p_buffer_rev?p_buffer_rev->head.obj:NULL, p_size, p_data, p_usage))
        real(p_buffer_rev->real, p_size, p_data, p_usage);

glBufferSubData: //GLenum p_target, GLintptr p_offset, GLsizeiptr p_size, const void* p_data
    trc_obj_t* buf = get_bound_buffer(ctx, p_target);
    if (!buf) ERROR("No buffer bound to target");
    if (buffer_sub_data(ctx, cmd, false, buf, p_offset, p_size, p_data))
        real(p_target, p_offset, p_size, p_data);

glNamedBufferSubData: //GLuint p_buffer, GLintptr p_offset, GLsizeiptr p_size, const void* p_data
    if (buffer_sub_data(ctx, cmd, true, trc_lookup_name(ctx->ns, TrcBuffer, p_buffer, -1), p_offset, p_size, p_data))
        real(p_buffer_rev->real, p_offset, p_size, p_data);

glCopyBufferSubData: //GLenum p_readTarget, GLenum p_writeTarget, GLintptr p_readOffset, GLintptr p_writeOffset, GLsizeiptr p_size
    trc_obj_t* read = get_bound_buffer(ctx, p_readTarget);
    trc_obj_t* write = get_bound_buffer(ctx, p_writeTarget);
    if (copy_buffer_data(ctx, cmd, false, read, write, p_readOffset, p_writeOffset, p_size))
        real(p_readTarget, p_writeTarget, p_readOffset, p_writeOffset, p_size);

glCopyNamedBufferSubData: //GLuint p_readBuffer, GLuint p_writeBuffer, GLintptr p_readOffset, GLintptr p_writeOffset, GLsizeiptr p_size
    trc_obj_t* read = p_readBuffer_rev ? p_readBuffer_rev->head.obj : NULL;
    trc_obj_t* write = p_writeBuffer_rev ? p_writeBuffer_rev->head.obj : NULL;
    if (copy_buffer_data(ctx, cmd, true, read, write, p_readOffset, p_writeOffset, p_size))
        real(p_readBuffer_rev->real, p_writeBuffer_rev->real, p_readOffset, p_writeOffset, p_size);

glMapBuffer: //GLenum p_target, GLenum p_access
    const trc_gl_buffer_rev_t* rev = trc_obj_get_rev(get_bound_buffer(ctx, p_target), -1);
    map_buffer(ctx, cmd, false, rev?rev->head.name:0, p_access);

glMapNamedBuffer: //GLuint p_buffer, GLenum p_access
    map_buffer(ctx, cmd, true, p_buffer, p_access);

glMapBufferRange: //GLenum p_target, GLintptr p_offset, GLsizeiptr p_length, GLbitfield p_access
    const trc_gl_buffer_rev_t* rev = trc_obj_get_rev(get_bound_buffer(ctx, p_target), -1);
    map_buffer_range(ctx, cmd, false, rev?rev->head.name:0, p_offset, p_length, p_access);

glMapNamedBufferRange: //GLuint p_buffer, GLintptr p_offset, GLsizeiptr p_length, GLbitfield p_access
    map_buffer_range(ctx, cmd, true, p_buffer, p_offset, p_length, p_access);

glUnmapBuffer: //GLenum p_target
    unmap_buffer(ctx, cmd, false, p_target);

glUnmapNamedBuffer: //GLuint p_buffer
    unmap_buffer(ctx, cmd, true, p_buffer);

glFlushMappedBufferRange: //GLenum p_target, GLintptr p_offset, GLsizeiptr p_length
    flush_mapped_buffer_range(ctx, cmd, false, p_target, p_offset, p_length);

glFlushMappedNamedBufferRange: //GLuint p_buffer, GLintptr p_offset, GLsizeiptr p_length
    flush_mapped_buffer_range(ctx, cmd, true, p_buffer, p_offset, p_length);

glCreateShader: //GLenum p_type
    GLuint real_shdr = F(glCreateShader)(p_type);
    GLuint fake = trc_get_uint(&cmd->ret)[0];
    trc_gl_shader_rev_t rev;
    rev.real = real_shdr;
    rev.sources = trc_create_data(ctx->trace, 0, NULL, TRC_DATA_IMMUTABLE);
    rev.info_log = trc_create_data(ctx->trace, 1, "", TRC_DATA_IMMUTABLE);
    rev.type = p_type;
    trc_create_named_obj(ctx->ns, TrcShader, fake, &rev);

glDeleteShader: //GLuint p_shader
    if (p_shader == 0) RETURN;
    GLuint real_shdr = get_real_shader(ctx->ns, p_shader);
    if (!real_shdr) ERROR("Invalid shader name");
    
    real(real_shdr);
    
    delete_obj(ctx->ns, p_shader, TrcShader);

glShaderSource: //GLuint p_shader, GLsizei p_count, const GLchar*const* p_string, const GLint* p_length
    GLuint shader = get_real_shader(ctx->ns, p_shader);
    if (!shader) ERROR("Invalid shader name");
    
    size_t res_sources_size = 0;
    char* res_sources = NULL;
    if (arg_string->count == 0) {
        real(shader, p_count, p_string, NULL);
        for (GLsizei i = 0; i < p_count; i++) {
            res_sources = realloc(res_sources, res_sources_size+strlen(p_string[i])+1);
            memset(res_sources+res_sources_size, 0, strlen(p_string[i])+1);
            strcpy(res_sources+res_sources_size, p_string[i]);
            res_sources_size += strlen(p_string[i]) + 1;
        }
    } else {
        real(shader, p_count, p_string, p_length);
        for (GLsizei i = 0; i < p_count; i++) {
            res_sources = realloc(res_sources, res_sources_size+p_length[i]+1);
            memset(res_sources+res_sources_size, 0, p_length[i]+1);
            memcpy(res_sources+res_sources_size, p_string[i], p_length[i]);
            res_sources[res_sources_size+p_length[i]+1] = 0;
            res_sources_size += p_length[i] + 1;
        }
    }
    
    trc_gl_shader_rev_t shdr = *get_shader(ctx->ns, p_shader);
    
    shdr.sources = trc_create_data_no_copy(ctx->trace, res_sources_size, res_sources, TRC_DATA_IMMUTABLE);
    
    set_shader(&shdr);

glCompileShader: //GLuint p_shader
    GLuint real_shdr = get_real_shader(ctx->ns, p_shader);
    if (!real_shdr) ERROR("Invalid shader name");
    
    real(real_shdr);
    
    trc_gl_shader_rev_t shdr = *get_shader(ctx->ns, p_shader);
    
    GLint len;
    F(glGetShaderiv)(real_shdr, GL_INFO_LOG_LENGTH, &len);
    shdr.info_log = trc_create_data(ctx->trace, len+1, NULL, 0);
    F(glGetShaderInfoLog)(real_shdr, len+1, NULL, trc_map_data(shdr.info_log, TRC_MAP_REPLACE));
    trc_unmap_freeze_data(ctx->trace, shdr.info_log);
    
    set_shader(&shdr);
    
    GLint status;
    F(glGetShaderiv)(real_shdr, GL_COMPILE_STATUS, &status);
    if (!status) ERROR("Failed to compile shader");

glCreateProgram: //
    GLuint real_program = F(glCreateProgram)();
    GLuint fake = trc_get_uint(&cmd->ret)[0];
    trc_gl_program_rev_t rev;
    rev.real = real_program;
    trc_data_t* empty_data = trc_create_data(ctx->trace, 0, NULL, TRC_DATA_IMMUTABLE);
    rev.root_uniform_count = 0;
    rev.uniforms = empty_data;
    rev.uniform_data = empty_data;
    rev.vertex_attribs = empty_data;
    rev.uniform_blocks = empty_data;
    for (size_t i = 0; i < 6; i++) rev.subroutines[i] = empty_data;
    for (size_t i = 0; i < 6; i++) rev.subroutine_uniforms[i] = empty_data;
    rev.shaders = empty_data;
    rev.linked = empty_data;
    rev.info_log = trc_create_data(ctx->trace, 1, "", TRC_DATA_IMMUTABLE);
    rev.binary_retrievable_hint = -1;
    rev.separable = false;
    trc_create_named_obj(ctx->ns, TrcProgram, fake, &rev);

glDeleteProgram: //GLuint p_program
    if (p_program == 0) RETURN;
    trc_gl_program_rev_t rev = *get_program(ctx->ns, p_program);
    if (!rev.real) ERROR("Invalid program name");
    real(rev.real);
    
    size_t shader_count = rev.shaders->size / sizeof(trc_gl_program_shader_t);
    trc_gl_program_shader_t* shaders = trc_map_data(rev.shaders, TRC_MAP_READ);
    for (size_t i = 0; i < shader_count; i++)
        trc_del_obj_ref(shaders[i].shader);
    trc_unmap_data(rev.shaders);
    
    rev.shaders = trc_create_data(ctx->trace, 0, NULL, TRC_DATA_IMMUTABLE);
    set_program(&rev);
    
    delete_obj(ctx->ns, p_program, TrcProgram);

glProgramParameteri: //GLuint p_program, GLenum p_pname, GLint p_value
    if (!p_program_rev) ERROR("Invalid program name");
    trc_gl_program_rev_t newrev = *p_program_rev;
    switch (p_pname) {
    case GL_PROGRAM_BINARY_RETRIEVABLE_HINT: newrev.binary_retrievable_hint = p_value ? 1 : 0; break;
    case GL_PROGRAM_SEPARABLE: newrev.separable = p_value; break;
    }
    set_program(&newrev);
    real(p_program, p_pname, p_value);

glAttachShader: //GLuint p_program, GLuint p_shader
    GLuint real_program = get_real_program(ctx->ns, p_program);
    if (!real_program) ERROR("Invalid program name");
    
    const trc_gl_shader_rev_t* shader_rev = trc_obj_get_rev(trc_lookup_name(ctx->ns, TrcShader, p_shader, -1), -1);
    if (!shader_rev) ERROR("Invalid shader name");
    
    real(real_program, shader_rev->real);
    
    trc_gl_program_rev_t program = *get_program(ctx->ns, p_program);
    trc_gl_program_rev_t old = program;
    const trc_gl_shader_rev_t* shader = get_shader(ctx->ns, p_shader);
    
    size_t shader_count = program.shaders->size / sizeof(trc_gl_program_shader_t);
    trc_gl_program_shader_t* src = trc_map_data(old.shaders, TRC_MAP_READ);
    
    for (size_t i = 0; i < shader_count; i++) {
        if (src[i].shader.obj == shader_rev->head.obj) ERROR("Shader is already attached");
    }
    
    program.shaders = trc_create_data(ctx->trace, (shader_count+1)*sizeof(trc_gl_program_shader_t), NULL, TRC_DATA_NO_ZERO);
    
    trc_gl_program_shader_t* dest = trc_map_data(program.shaders, TRC_MAP_REPLACE);
    memcpy(dest, src, shader_count*sizeof(trc_gl_program_shader_t));
    memset(&dest[shader_count], 0, sizeof(trc_gl_program_shader_t));
    dest[shader_count].shader.obj = shader_rev->head.obj;
    trc_grab_obj(shader_rev->head.obj);
    dest[shader_count].shader_revision = shader->head.revision;
    trc_unmap_data(old.shaders);
    trc_unmap_freeze_data(ctx->trace, program.shaders);
    
    set_program(&program);

glDetachShader: //GLuint p_program, GLuint p_shader
    GLuint real_program = get_real_program(ctx->ns, p_program);
    if (!real_program) ERROR("Invalid program name");
    
    const trc_gl_shader_rev_t* shader_rev = get_shader(ctx->ns, p_shader);
    if (!shader_rev) ERROR("Invalid shader name");
    
    real(real_program, shader_rev->real);
    
    trc_gl_program_rev_t program = *get_program(ctx->ns, p_program);
    trc_gl_program_rev_t old = program;
    
    size_t shader_count = program.shaders->size / sizeof(trc_gl_program_shader_t);
    program.shaders = trc_create_data(ctx->trace, (shader_count-1)*sizeof(trc_gl_program_shader_t), NULL, TRC_DATA_NO_ZERO);
    
    trc_gl_program_shader_t* dest = trc_map_data(program.shaders, TRC_MAP_REPLACE);
    trc_gl_program_shader_t* src = trc_map_data(old.shaders, TRC_MAP_READ);
    size_t next = 0;
    bool found = false;
    for (size_t i = 0; i < shader_count; i++) {
        if (src[i].shader.obj == shader_rev->head.obj) {
            found = true;
            trc_del_obj_ref(src[i].shader);
            continue;
        }
        dest[next++] = src[i];
    }
    trc_unmap_data(old.shaders);
    trc_unmap_freeze_data(ctx->trace, program.shaders);
    if (!found) ERROR("Shader is not attached to program");
    
    set_program(&program);

typedef struct link_program_extra_t {
    char* name;
    uint32_t val;
    GLenum stage;
    uint32_t stage_idx;
} link_program_extra_t;

static uint link_program_extra(trace_command_t* cmd, const char* name, size_t* i, link_program_extra_t* res) {
    trace_extra_t* extra = trc_get_extrai(cmd, name, (*i)++);
    if (!extra) return -1; //-1=End
    
    if (extra->size < 12) ERROR2(1, "Invalid %s extra", name); //1=skip
    void* data = extra->data;
    res->val = le32toh(((uint32_t*)data)[0]);
    res->stage = le32toh(((uint32_t*)data)[1]);
    uint32_t len = le32toh(((uint32_t*)data)[2]);
    if (extra->size < 12+len) ERROR2(1, "Invalid %s extra", name); //1=skip
    
    switch (res->stage) {
    case GL_VERTEX_SHADER: res->stage_idx = 0; break;
    case GL_TESS_CONTROL_SHADER: res->stage_idx = 1; break;
    case GL_TESS_EVALUATION_SHADER: res->stage_idx = 2; break;
    case GL_GEOMETRY_SHADER: res->stage_idx = 3; break;
    case GL_FRAGMENT_SHADER: res->stage_idx = 4; break;
    case GL_COMPUTE_SHADER: res->stage_idx = 5; break;
    case 0: res->stage_idx = 0; break;
    default: ERROR2(1, "Invalid %s extra", name); //1=skip
    }
    
    res->name = calloc(1, len+1);
    memcpy(res->name, (uint8_t*)data+12, len);
    
    return 0; //0=Use
}

//TODO: The get_program_* functions are all very similar
static trc_data_t* get_program_vertex_attribs(trace_command_t* cmd, trc_replay_context_t* ctx, GLuint real_program) {
    size_t vertex_attrib_count = 0;
    uint* vertex_attribs = NULL;
    size_t i = 0;
    int res;
    link_program_extra_t extra;
    while ((res=link_program_extra(cmd, "replay/program/vertex_attrib", &i, &extra))!=-1) {
        if (res != 0) continue;
        GLint real_idx = F(glGetAttribLocation)(real_program, extra.name);
        if (real_idx < 0) {
            trc_add_error(cmd, "Nonexistent or inactive vertex attribute while adding vertex attribute %s", extra.name);
        } else {
            vertex_attribs = realloc(vertex_attribs, (vertex_attrib_count+1)*sizeof(uint)*2);
            vertex_attribs[vertex_attrib_count*2] = real_idx;
            vertex_attribs[vertex_attrib_count++*2+1] = extra.val;
        }
        free(extra.name);
    }
    
    return trc_create_data_no_copy(ctx->trace, vertex_attrib_count*2*sizeof(uint), vertex_attribs, TRC_DATA_IMMUTABLE);
}

static trc_data_t* get_program_uniform_blocks(trace_command_t* cmd, trc_replay_context_t* ctx, GLuint real_program) {
    size_t uniform_block_count = 0;
    trc_gl_program_uniform_block_t* uniform_blocks = NULL;
    size_t i = 0;
    int res;
    link_program_extra_t extra;
    while ((res=link_program_extra(cmd, "replay/program/uniform_block", &i, &extra))!=-1) {
        if (res != 0) continue;
        GLint real_idx = F(glGetUniformBlockIndex)(real_program, extra.name);
        if (real_idx < 0) {
            trc_add_error(cmd, "Nonexistent or inactive uniform block while adding uniform block %s", extra.name);
        } else {
            uniform_blocks = realloc(uniform_blocks, (uniform_block_count+1)*sizeof(trc_gl_program_uniform_block_t));
            trc_gl_program_uniform_block_t block;
            memset(&block, 0, sizeof(block)); //initialize padding to zero - it might be compressed
            block.real = real_idx;
            block.fake = extra.val;
            block.binding = 0;
            uniform_blocks[uniform_block_count++] = block;
        }
        free(extra.name);
    }
    
    return trc_create_data_no_copy(ctx->trace, uniform_block_count*sizeof(trc_gl_program_uniform_block_t), uniform_blocks, TRC_DATA_IMMUTABLE);
}

static void get_program_subroutines(trace_command_t* cmd, trc_replay_context_t* ctx, GLuint real_program, trc_data_t** datas) {
    //TODO: Check for extensions
    bool compute_supported = trc_gl_state_get_ver(ctx->trace) >= 430;
    bool tesselation_supported = trc_gl_state_get_ver(ctx->trace) >= 400;
    
    size_t subroutine_count[6] = {0};
    uint* subroutines[6] = {0};
    size_t i = 0;
    int res;
    link_program_extra_t extra;
    while ((res=link_program_extra(cmd, "replay/program/subroutine", &i, &extra))!=-1) {
        if (res != 0) continue;
        if ((extra.stage==GL_COMPUTE_SHADER && !compute_supported) ||
            ((extra.stage==GL_TESS_CONTROL_SHADER||extra.stage==GL_TESS_EVALUATION_SHADER) && !tesselation_supported)) {
            trc_add_error(cmd, "Unsupported shader stage while adding subroutine %s", extra.name);
            continue;
        }
        GLint real_idx = F(glGetSubroutineIndex)(real_program, extra.stage, extra.name);
        if (real_idx == GL_INVALID_INDEX) {
            trc_add_error(cmd, "Nonexistent or inactive subroutine while adding subroutine %s", extra.name);
        } else {
            subroutines[extra.stage_idx] = realloc(subroutines[extra.stage_idx], (subroutine_count[extra.stage_idx]+1)*sizeof(uint)*2);
            subroutines[extra.stage_idx][subroutine_count[extra.stage_idx]*2] = real_idx;
            subroutines[extra.stage_idx][subroutine_count[extra.stage_idx]++*2+1] = extra.val;
        }
        free(extra.name);
    }
    
    for (size_t i = 0; i < 6; i++)
        datas[i] = trc_create_data_no_copy(ctx->trace, subroutine_count[i]*2*sizeof(uint), subroutines[i], TRC_DATA_IMMUTABLE);
}

static void get_program_subroutine_uniforms(trace_command_t* cmd, trc_replay_context_t* ctx, GLuint real_program, trc_data_t** datas) {
    //TODO: Check for extensions
    bool compute_supported = trc_gl_state_get_ver(ctx->trace) >= 430;
    bool tesselation_supported = trc_gl_state_get_ver(ctx->trace) >= 400;
    
    //"_uniform" not included to keep names short
    size_t subroutine_count[6] = {0};
    uint* subroutines[6] = {0};
    size_t i = 0;
    int res;
    link_program_extra_t extra;
    while ((res=link_program_extra(cmd, "replay/program/subroutine_uniform", &i, &extra))!=-1) {
        if (res != 0) continue;
        if ((extra.stage==GL_COMPUTE_SHADER && !compute_supported) ||
            ((extra.stage==GL_TESS_CONTROL_SHADER||extra.stage==GL_TESS_EVALUATION_SHADER) && !tesselation_supported)) {
            trc_add_error(cmd, "Unsupported shader stage while adding subroutine uniform %s", extra.name);
            continue;
        }
        GLint real_idx = F(glGetSubroutineUniformLocation)(real_program, extra.stage, extra.name);
        if (real_idx == GL_INVALID_INDEX) {
            trc_add_error(cmd, "Nonexistent or inactive subroutine while adding subroutine uniform %s", extra.name);
        } else {
            subroutines[extra.stage_idx] = realloc(subroutines[extra.stage_idx], (subroutine_count[extra.stage_idx]+1)*sizeof(uint)*2);
            subroutines[extra.stage_idx][subroutine_count[extra.stage_idx]*2] = real_idx;
            subroutines[extra.stage_idx][subroutine_count[extra.stage_idx]++*2+1] = extra.val;
        }
        free(extra.name);
    }
    
    for (size_t i = 0; i < 6; i++)
        datas[i] = trc_create_data_no_copy(ctx->trace, subroutine_count[i]*2*sizeof(uint), subroutines[i], TRC_DATA_IMMUTABLE);
}

bool read_uint32(size_t* data_size, uint8_t** data, uint32_t* val) {
    if (*data_size < 4) return false;
    memcpy(val, *data, 4);
    *val = le32toh(*val);
    *data += 4;
    return true;
}

//TODO: Validation
//TODO: Cleanup
static bool read_uniform_spec(trc_replay_context_t* ctx, trc_gl_uniform_t* uniforms, size_t* data_size,
                              uint8_t** data, size_t* next_index, size_t* storage_used, size_t spec_count,
                              uint specindex, GLuint program, const char* var_name_base, uint parent) {
    uint32_t name_length;
    if (!read_uint32(data_size, data, &name_length)) return false;
    if (*data_size < name_length) return false;
    char* name = calloc(name_length+1, 1);
    memcpy(name, *data, name_length);
    *data += name_length;
    *data_size -= name_length;
    
    trc_gl_uniform_t* spec = &uniforms[specindex];
    memset(spec, 0, sizeof(trc_gl_uniform_t)); //To fix usage of unintialized data because of compression
    spec->parent = parent;
    spec->name = trc_create_data_no_copy(ctx->trace, name_length+1, name, TRC_DATA_IMMUTABLE);
    spec->next = 0xffffffff;
    spec->dtype.dim[0] = 1;
    spec->dtype.dim[1] = 1;
    spec->first_child = 0xffffffff;
    
    if (*data_size < 1) return false;
    spec->dtype.base = (trc_gl_uniform_base_dtype_t)**data;
    (*data_size)--;
    (*data)++;
    
    if ((int)spec->dtype.base <= 8) {
        uint32_t loc;
        if (!read_uint32(data_size, data, &loc)) return false;
        spec->fake_loc = loc;
    }
    
    if ((int)spec->dtype.base <= 6) {
        if (*data_size < 2) return false;
        spec->dtype.dim[0] = (*data)[0];
        spec->dtype.dim[1] = (*data)[1];
        (*data_size) -= 2;
        (*data) += 2;
    } else if ((int)spec->dtype.base <= 8) {
        spec->dtype.tex_type = (*data)[0];
        spec->dtype.tex_shadow = (*data)[1];
        spec->dtype.tex_array = (*data)[2];
        spec->dtype.tex_multisample = (*data)[3];
        spec->dtype.tex_dtype = (*data)[4];
        (*data_size) -= 5;
        (*data) += 5;
    } else if ((int)spec->dtype.base==10 || (int)spec->dtype.base==11) {
        uint32_t child_count;
        if (!read_uint32(data_size, data, &child_count)) return false;
        
        size_t prev_index;
        for (size_t i = 0; i < child_count; i++) {
            size_t index = (*next_index)++;
            if (index >= spec_count) return false;
            
            char* base = calloc(strlen(var_name_base)+strlen(name)+8, 1);
            if (spec->dtype.base == TrcUniformBaseType_Struct)
                sprintf(base, "%s%s.", var_name_base, name);
            else
                sprintf(base, "%s%s[%zu]", var_name_base, name, i);
            if (!read_uniform_spec(ctx, uniforms, data_size, data, next_index, storage_used, spec_count, index, program, base, parent)) {
                free(base);
                return false;
            }
            free(base);
            
            if (i == 0) spec->first_child = index;
            else uniforms[prev_index].next = index;
            prev_index = index;
        }
    }
    
    if ((int)spec->dtype.base <= 8) {
        char* full_name = calloc(strlen(var_name_base)+strlen(name)+1, 1);
        strcpy(full_name, var_name_base);
        strcat(full_name, name);
        spec->real_loc = F(glGetUniformLocation)(program, full_name);
        free(full_name);
        if (spec->real_loc < 0) return false;
        
        size_t dtype_sizes[] = {4, 8, 4, 4, 8, 8, 1, 4, 4};
        spec->data_offset = *storage_used;
        *storage_used += dtype_sizes[(int)spec->dtype.base] * spec->dtype.dim[0] * spec->dtype.dim[1];
    }
    
    return true;
}

static trc_gl_uniform_t* read_uniform_specs(trc_replay_context_t* ctx, size_t* data_size, uint8_t** data,
                                            size_t* root_count, size_t* count, GLuint program, size_t* storage_needed) {
    uint32_t spec_count;
    if (!read_uint32(data_size, data, &spec_count)) return NULL;
    *count = spec_count;
    
    uint32_t root_spec_count;
    if (!read_uint32(data_size, data, &root_spec_count)) return NULL;
    *root_count = root_spec_count;
    
    *storage_needed = 0;
    
    trc_gl_uniform_t* uniforms = malloc(spec_count*sizeof(trc_gl_uniform_t));
    size_t next_index = root_spec_count;
    for (size_t i = 0; i < root_spec_count; i++) {
        if (!read_uniform_spec(ctx, uniforms, data_size, data, &next_index,
                               storage_needed, spec_count, i, program, "", 0xffffffff)) {
            return NULL;
        }
    }
    return uniforms;
}

static bool init_uniforms(trace_command_t* cmd, trc_replay_context_t* ctx, trc_gl_program_rev_t* rev) {
    trace_extra_t* extra = trc_get_extrai(cmd, "replay/program/uniforms", 0);
    if (!extra) return false;
    
    size_t data_size = extra->size;
    uint8_t* data = extra->data;
    size_t root_count, count, storage_needed;
    trc_gl_uniform_t* uniforms = read_uniform_specs(ctx, &data_size, &data, &root_count, &count, rev->real, &storage_needed);
    if (!uniforms) {
        free(uniforms);
        return false;
    }
    
    uint8_t* uniform_data = malloc(storage_needed);
    //Initialize data
    for (size_t i = 0; i < count; i++) {
        trc_gl_uniform_t u = uniforms[i];
        if ((int)u.dtype.base > 8) continue;
        uint8_t* data = uniform_data + u.data_offset;
        
        size_t size = u.dtype.dim[0] * u.dtype.dim[1];
        switch (u.dtype.base) {
        #define D(e, st, dt, f) case e: {\
            st vals[16];\
            F(f)(rev->real, u.real_loc, vals);\
            for (size_t j = 0; j < size; j++) ((dt*)data)[j] = vals[j];\
            break;\
        }
        D(TrcUniformBaseType_Float, float, float, glGetUniformfv)
        D(TrcUniformBaseType_Double, double, double, glGetUniformdv)
        D(TrcUniformBaseType_Uint, uint32_t, uint32_t, glGetUniformuiv)
        D(TrcUniformBaseType_Int, int32_t, int32_t, glGetUniformiv)
        D(TrcUniformBaseType_Bool, int32_t, bool, glGetUniformiv)
        D(TrcUniformBaseType_Sampler, int32_t, int32_t, glGetUniformiv)
        D(TrcUniformBaseType_Image, int32_t, int32_t, glGetUniformiv)
        //TODO
        //D(TrcUniformBaseType_Uint64, uint64_t, uint64_t, glGetUniformi64vARB)
        //D(TrcUniformBaseType_Int64, int64_t, int64_t, glGetUniformi64vARB)
        #undef D
        default: break;
        }
    }
    
    rev->root_uniform_count = root_count;
    rev->uniforms = trc_create_data_no_copy(ctx->trace, count*sizeof(trc_gl_uniform_t), uniforms, TRC_DATA_IMMUTABLE);
    rev->uniform_data = trc_create_data_no_copy(ctx->trace, storage_needed, uniform_data, TRC_DATA_IMMUTABLE);
    
    return true;
}

glLinkProgram: //GLuint p_program
    trc_obj_t* program = trc_lookup_name(ctx->ns, TrcProgram, p_program, -1);
    if (!program) ERROR("Invalid program name");
    
    //TODO: Also test if it is part of the current program pipeline
    if (get_current_tf(ctx)->active_not_paused && program==trc_gl_state_get_bound_program(ctx->trace))
        ERROR("The bound program cannot be modified while transform feedback is active and unpaused");
    
    trc_gl_program_rev_t rev = *(const trc_gl_program_rev_t*)trc_obj_get_rev(program, -1);
    
    real(rev.real);
    
    GLint status;
    F(glGetProgramiv)(rev.real, GL_LINK_STATUS, &status);
    if (!status) ERROR("Failed to link program");
    
    GLint len;
    F(glGetProgramiv)(rev.real, GL_INFO_LOG_LENGTH, &len);
    rev.info_log = trc_create_data(ctx->trace, len+1, NULL, 0);
    F(glGetProgramInfoLog)(rev.real, len+1, NULL, trc_map_data(rev.info_log, TRC_MAP_REPLACE));
    trc_unmap_freeze_data(ctx->trace, rev.info_log);
    
    if (!init_uniforms(cmd, ctx, &rev)) {
        trc_data_t* empty_data = trc_create_data(ctx->trace, 0, NULL, TRC_DATA_IMMUTABLE);
        rev.root_uniform_count = 0;
        rev.uniforms = empty_data;
        rev.uniform_data = empty_data;
        ERROR("Failed to initialize uniform storage");
    }
    rev.vertex_attribs = get_program_vertex_attribs(cmd, ctx, rev.real);
    rev.uniform_blocks = get_program_uniform_blocks(cmd, ctx, rev.real);
    if (trc_gl_state_get_ver(ctx->trace) >= 400) { //TODO: Check for the extension
        get_program_subroutines(cmd, ctx, rev.real, rev.subroutines);
        get_program_subroutine_uniforms(cmd, ctx, rev.real, rev.subroutine_uniforms);
    }
    
    size_t linked_count = rev.shaders->size / sizeof(trc_gl_program_shader_t);
    trc_gl_program_linked_shader_t* linked = calloc(linked_count, sizeof(trc_gl_program_shader_t));
    trc_gl_program_shader_t* shaders = trc_map_data(rev.shaders, TRC_MAP_READ);
    for (size_t i = 0; i < linked_count; i++) {
        linked[i].shader = shaders[i].shader.obj;
        linked[i].shader_revision = shaders[i].shader_revision;
    }
    trc_unmap_data(rev.shaders);
    rev.linked = trc_create_data(ctx->trace, linked_count*sizeof(trc_gl_program_linked_shader_t),
                                 linked, TRC_DATA_IMMUTABLE);
    free(linked);
    
    set_program(&rev);

glValidateProgram: //GLuint p_program
    GLuint real_program = get_real_program(ctx->ns, p_program);
    if (!real_program) ERROR("Invalid program name");
    
    real(real_program);
    
    trc_gl_program_rev_t rev = *get_program(ctx->ns, p_program);
    
    GLint len;
    F(glGetProgramiv)(real_program, GL_INFO_LOG_LENGTH, &len);
    rev.info_log = trc_create_data(ctx->trace, len+1, NULL, 0);
    F(glGetProgramInfoLog)(real_program, len, NULL, trc_map_data(rev.info_log, TRC_MAP_REPLACE));
    trc_unmap_freeze_data(ctx->trace, rev.info_log);
    
    set_program(&rev);
    
    GLint status;
    F(glGetProgramiv)(real_program, GL_LINK_STATUS, &status);
    if (!status) ERROR("Program validation failed");

glUseProgram: //GLuint p_program
    const trc_gl_program_rev_t* program_rev = get_program(ctx->ns, p_program);
    if (!program_rev && p_program) ERROR("Invalid program name");
    if (get_current_tf(ctx)->active_not_paused)
        ERROR("The program binding cannot be modified while transform feedback is active and unpaused");
    
    trc_gl_state_set_bound_program(ctx->trace, program_rev?program_rev->head.obj:NULL);
    
    real(program_rev->real);

glGenProgramPipelines: //GLsizei p_n, GLuint* p_pipelines
    GLuint* pipelines = replay_alloc(p_n*sizeof(GLuint));
    real(p_n, pipelines);
    
    trc_gl_program_pipeline_rev_t rev;
    for (size_t i = 0; i < p_n; ++i) {
        rev.real = pipelines[i];
        rev.has_object = false;
        rev.active_program = (trc_obj_ref_t){NULL};
        rev.vertex_program = (trc_obj_ref_t){NULL};
        rev.fragment_program = (trc_obj_ref_t){NULL};
        rev.geometry_program = (trc_obj_ref_t){NULL};
        rev.tess_control_program = (trc_obj_ref_t){NULL};
        rev.tess_eval_program = (trc_obj_ref_t){NULL};
        rev.compute_program = (trc_obj_ref_t){NULL};
        trc_create_named_obj(ctx->ns, TrcProgramPipeline, p_pipelines[i], &rev);
    }

glDeleteProgramPipelines: //GLsizei p_n, const GLuint* p_pipelines
    GLuint* pipelines = replay_alloc(p_n*sizeof(GLuint));
    for (size_t i = 0; i < p_n; ++i) {
        if (!(pipelines[i] = get_real_program_pipeline(ctx->ns, p_pipelines[i])) && p_pipelines[i]) {
            trc_add_warning(cmd, "Invalid program pipeline name");
        } else {
            trc_obj_t* obj = get_program_pipeline(ctx->ns, p_pipelines[i])->head.obj;
            if (trc_gl_state_get_bound_pipeline(ctx->trace) == obj)
                trc_gl_state_set_bound_pipeline(ctx->trace, NULL);
            delete_obj(ctx->ns, p_pipelines[i], TrcProgramPipeline);
        }
    }
    real(p_n, pipelines);

//TODO: Do transform feedback checks in glBindProgramPipeline
glBindProgramPipeline: //GLuint p_pipeline
    if (p_pipeline && !p_pipeline_rev) ERROR("Invalid program pipeline name");
    if (p_pipeline_rev && !p_pipeline_rev->has_object) {
        trc_gl_program_pipeline_rev_t newrev = *p_pipeline_rev;
        newrev.has_object = true;
        set_program_pipeline(&newrev);
    }
    trc_gl_state_set_bound_pipeline(ctx->trace, p_pipeline_rev->head.obj);

glUseProgramStages: //GLuint p_pipeline, GLbitfield p_stages, GLuint p_program
    if (!p_pipeline_rev) ERROR("Invalid program pipeline name");
    if (!p_pipeline_rev->has_object) ERROR("Program pipeline name has no object");
    if (!p_program_rev) ERROR("Invalid program name");
    if (get_current_tf(ctx)->active_not_paused &&
        p_pipeline_rev->head.obj==trc_gl_state_get_bound_pipeline(ctx->trace)) {
        ERROR("The bound program pipeline object cannot be modified while transform feedback is active and unpaused");
    }
    real(p_pipeline_rev->real, p_stages, p_program_rev->real);
    trc_gl_program_pipeline_rev_t newrev = *p_pipeline_rev;
    if (p_stages & GL_VERTEX_SHADER_BIT)
        trc_set_obj_ref(&newrev.vertex_program, p_program_rev->head.obj);
    if (p_stages & GL_FRAGMENT_SHADER_BIT)
        trc_set_obj_ref(&newrev.fragment_program, p_program_rev->head.obj);
    if (p_stages & GL_GEOMETRY_SHADER_BIT)
        trc_set_obj_ref(&newrev.geometry_program, p_program_rev->head.obj);
    if (p_stages & GL_TESS_CONTROL_SHADER_BIT)
        trc_set_obj_ref(&newrev.tess_control_program, p_program_rev->head.obj);
    if (p_stages & GL_TESS_EVALUATION_SHADER_BIT)
        trc_set_obj_ref(&newrev.tess_eval_program, p_program_rev->head.obj);
    if (p_stages & GL_COMPUTE_SHADER_BIT)
        trc_set_obj_ref(&newrev.compute_program, p_program_rev->head.obj);
    set_program_pipeline(&newrev);

glActiveShaderProgram: //GLuint p_pipeline, GLuint p_program
    if (!p_pipeline_rev) ERROR("Invalid program pipeline name");
    if (!p_pipeline_rev->has_object) ERROR("Program pipeline name has no object");
    if (!p_program_rev) ERROR("Invalid program name");
    if (get_current_tf(ctx)->active_not_paused &&
        p_pipeline_rev->head.obj==trc_gl_state_get_bound_pipeline(ctx->trace)) {
        ERROR("The bound program pipeline object cannot be modified while transform feedback is active and unpaused");
    }
    real(p_pipeline_rev->real, p_program_rev->real);
    trc_gl_program_pipeline_rev_t newrev = *p_pipeline_rev;
    trc_set_obj_ref(&newrev.active_program, p_program_rev->head.obj);
    set_program_pipeline(&newrev);

glFlush: //
    real();

glFinish: //
    real();

glIsEnabled: //GLenum p_cap
    ;

glIsEnabledi: //GLenum p_target, GLuint p_index
    ; //TODO: Validation

glIsBuffer: //GLuint p_buffer
    ;

glIsProgram: //GLuint p_program
    ;

glIsQuery: //GLuint p_id
    ;

glIsShader: //GLuint p_shader
    ;

glIsTexture: //GLuint p_texture
    ;

glIsVertexArray: //GLuint p_array
    ;

glIsProgramPipeline: //GLuint p_pipeline
    ;

glIsFramebuffer: //GLuint p_framebuffer
    ;

glIsRenderbuffer: //GLuint p_renderbuffer
    ;

glIsSampler: //GLuint p_sampler
    ;

glIsSync: //GLsync p_sync
    ;

glIsTransformFeedback: //GLuint p_id
    ;

glTransformFeedbackVaryings: //GLuint p_program, GLsizei p_count, const GLchar*const* p_varyings, GLenum p_bufferMode
    if (!p_program) ERROR("Invalid program name");
    real(p_program_rev->real, p_count, p_varyings, p_bufferMode);
    //TODO: Store this in the program object

glBindAttribLocation: //GLuint p_program, GLuint p_index, const GLchar* p_name
    GLuint real_program = get_real_program(ctx->ns, p_program);
    if (!real_program) ERROR("Invalid program name");
    real(real_program, p_index, p_name);

glGetAttribLocation: //GLuint p_program, const GLchar* p_name
    GLuint real_program = get_real_program(ctx->ns, p_program);
    if (!real_program) ERROR("Invalid program name");
    real(real_program, p_name);

glGetUniformLocation: //GLuint p_program, const GLchar* p_name
    GLuint real_program = get_real_program(ctx->ns, p_program);
    if (!real_program) ERROR("Invalid program name");
    real(real_program, p_name);

glGetShaderiv: //GLuint p_shader, GLenum p_pname, GLint* p_params
    GLuint real_shdr = get_real_shader(ctx->ns, p_shader);
    if (!real_shdr) ERROR("Invalid shader name");

glGetShaderInfoLog: //GLuint p_shader, GLsizei p_bufSize, GLsizei* p_length, GLchar* p_infoLog
    GLuint real_shdr = get_real_shader(ctx->ns, p_shader);
    if (!real_shdr) ERROR("Invalid shader name");

glGetShaderSource: //GLuint p_shader, GLsizei p_bufSize, GLsizei* p_length, GLchar* p_source
    GLuint real_shdr = get_real_shader(ctx->ns, p_shader);
    if (!real_shdr) ERROR("Invalid shader name");

glGetQueryiv: //GLenum p_target, GLenum p_pname, GLint* p_params
    ; //TODO: Validation

glGetQueryObjectiv: //GLuint p_id, GLenum p_pname, GLint* p_params
    GLuint real_query = get_real_query(ctx->ns, p_id);
    if (!real_query) ERROR("Invalid query name");

glGetQueryObjectuiv: //GLuint p_id, GLenum p_pname, GLuint* p_params
    GLuint real_query = get_real_query(ctx->ns, p_id);
    if (!real_query) ERROR("Invalid query name");

glGetQueryObjecti64v: //GLuint p_id, GLenum p_pname, GLint64* p_params
    GLuint real_query = get_real_query(ctx->ns, p_id);
    if (!real_query) ERROR("Invalid query name");

glGetQueryObjectui64v: //GLuint p_id, GLenum p_pname, GLuint64* p_params
    GLuint real_query = get_real_query(ctx->ns, p_id);
    if (!real_query) ERROR("Invalid query name");

glGetProgramInfoLog: //GLuint p_program, GLsizei p_bufSize, GLsizei* p_length, GLchar* p_infoLog
    GLuint real_prog = get_real_program(ctx->ns, p_program);
    if (!real_prog) ERROR("Invalid program name");

glGetProgramiv: //GLuint p_program, GLenum p_pname, GLint* p_params
    GLuint real_prog = get_real_program(ctx->ns, p_program);
    if (!real_prog) ERROR("Invalid program name");

glGetFramebufferParameteriv: //GLenum p_target, GLenum p_pname, GLint* p_params
    trc_obj_t* fb = get_bound_framebuffer(ctx, p_target);
    if (!fb && not_one_of(p_pname, GL_DOUBLEBUFFER, GL_IMPLEMENTATION_COLOR_READ_FORMAT,
                          GL_IMPLEMENTATION_COLOR_READ_TYPE, GL_SAMPLES, GL_SAMPLE_BUFFERS, GL_STEREO, -1))
        ERROR("Invalid parameter name");

glGetNamedFramebufferParameteriv: //GLuint p_framebuffer, GLenum p_pname, GLint  * p_param
    if (!p_framebuffer && not_one_of(p_pname, GL_DOUBLEBUFFER, GL_IMPLEMENTATION_COLOR_READ_FORMAT,
                                     GL_IMPLEMENTATION_COLOR_READ_TYPE, GL_SAMPLES, GL_SAMPLE_BUFFERS, GL_STEREO, -1))
        ERROR("Invalid parameter name");
    if (!p_framebuffer_rev && p_framebuffer) ERROR("Invalid framebuffer name");
    if (p_framebuffer_rev && !p_framebuffer_rev->has_object)
        ERROR("Framebuffer name has no object");

glGetObjectLabel: //GLenum p_identifier, GLuint p_name, GLsizei p_bufSize, GLsizei* p_length, GLchar* p_label
    if (p_bufSize < 0) ERROR("Invalid buffer size");
    #define A(gf, t) {\
        const t* rev = gf(ctx->ns, p_name);\
        if (!rev) ERROR("Invalid object name");\
        if (!rev->has_object) ERROR("Name has no object");\
        break;\
    }
    #define B(gf, t) {\
        const t* rev = gf(ctx->ns, p_name);\
        if (!rev) ERROR("Invalid object name");\
        break;\
    }
    switch (p_identifier) {
    case GL_BUFFER: A(get_buffer, trc_gl_buffer_rev_t)
    case GL_SHADER: B(get_shader, trc_gl_shader_rev_t)
    case GL_PROGRAM: B(get_program, trc_gl_program_rev_t)
    case GL_VERTEX_ARRAY: A(get_vao, trc_gl_vao_rev_t)
    case GL_QUERY: A(get_query, trc_gl_query_rev_t)
    case GL_PROGRAM_PIPELINE: A(get_program_pipeline, trc_gl_program_pipeline_rev_t)
    case GL_TRANSFORM_FEEDBACK: A(get_transform_feedback, trc_gl_transform_feedback_rev_t)
    case GL_SAMPLER: B(get_sampler, trc_gl_sampler_rev_t)
    case GL_TEXTURE: A(get_texture, trc_gl_texture_rev_t)
    case GL_RENDERBUFFER: A(get_renderbuffer, trc_gl_renderbuffer_rev_t)
    case GL_FRAMEBUFFER: A(get_framebuffer, trc_gl_framebuffer_rev_t)
    }
    #undef B
    #undef A

glGetError: //
    ;

//TODO: Validation for these
glGetTexLevelParameterfv: //GLenum p_target, GLint p_level, GLenum p_pname, GLfloat* p_params
    ;

glGetTexLevelParameteriv: //GLenum p_target, GLint p_level, GLenum p_pname, GLint* p_params
    ;

glGetTexParameterfv: //GLenum p_target, GLenum p_pname, GLfloat* p_params
    ;

glGetTexParameteriv: //GLenum p_target, GLenum p_pname, GLint* p_params
    ;

glGetTexParameterIiv: //GLenum p_target, GLenum p_pname, GLint* p_params
    ;

glGetTexParameterIuiv: //GLenum p_target, GLenum p_pname, GLuint* p_params
    ;

glGetTextureLevelParameterfv: //GLuint p_texture, GLint p_level, GLenum p_pname, GLfloat* p_params
    ;

glGetTextureLevelParameteriv: //GLuint p_texture, GLint p_level, GLenum p_pname, GLint* p_params
    ;

glGetTextureParameterfv: //GLuint p_texture, GLenum p_pname, GLfloat* p_params
    ;

glGetTextureParameteriv: //GLuint p_texture, GLenum p_pname, GLint* p_params
    ;

glGetTextureParameterIiv: //GLuint p_texture, GLenum p_pname, GLint* p_params
    ;

glGetTextureParameterIuiv: //GLuint p_texture, GLenum p_pname, GLuint* p_params
    ;

glGetTransformFeedbackVarying: //GLuint p_program, GLuint p_index, GLsizei p_bufSize, GLsizei* p_length GLsizei* p_size, GLenum* p_type, GLchar* p_name
    ;

glCheckFramebufferStatus: //GLenum p_target
    ;

glGetPointerv: //GLenum p_pname, void** p_params
    ;

glGetPolygonStipple: //GLubyte* p_mask
    ;

glGetMinmax: //GLenum p_target, GLboolean p_reset, GLenum p_format, GLenum p_type, void * p_values
    ;

glGetMinmaxParameterfv: //GLenum p_target, GLenum p_pname, GLfloat* p_params
    ;

glGetMinmaxParameteriv: //GLenum p_target, GLenum p_pname, GLint* p_params
    ;

glGetPixelMapfv: //GLenum p_map, GLfloat* p_values
    ;

glGetPixelMapuiv: //GLenum p_map, GLuint* p_values
    ;

glGetPixelMapusv: //GLenum p_map, GLushort* p_values
    ;

glGetSeparableFilter: //GLenum p_target, GLenum p_format, GLenum p_type, void * p_row, void * p_column, void * p_span
    ;

glGetProgramPipelineiv: //GLuint p_pipeline, GLenum p_pname, GLint  * p_params
    ;

glGetBufferParameteriv: //GLenum p_target, GLenum p_pname, GLint* p_params
    if (!get_bound_buffer(ctx, p_target)) ERROR("No buffer bound to target");

glGetBufferParameteri64v: //GLenum p_target, GLenum p_pname, GLint64* p_params
    if (!get_bound_buffer(ctx, p_target)) ERROR("No buffer bound to target");

glGetNamedBufferParameteriv: //GLuint p_buffer, GLenum p_pname, GLint* p_params
    if (!p_buffer_rev) ERROR("Invalid buffer name");
    if (!p_buffer_rev->has_object) ERROR("Buffer name has no object");

glGetNamedBufferParameteri64v: //GLuint p_buffer, GLenum p_pname, GLint64* p_params
    if (!p_buffer_rev) ERROR("Invalid buffer name");
    if (!p_buffer_rev->has_object) ERROR("Buffer name has no object");

glGetBufferPointerv: //GLenum p_target, GLenum p_pname, void ** p_params
    if (!get_bound_buffer(ctx, p_target)) ERROR("No buffer bound to target");

glGetNamedBufferPointerv: //GLuint p_buffer, GLenum p_pname, void ** p_params
    if (!p_buffer_rev) ERROR("Invalid buffer name");
    if (!p_buffer_rev->has_object) ERROR("Buffer name has no object");

glGetBufferSubData: //GLenum p_target, GLintptr p_offset, GLsizeiptr p_size, void* p_data
    get_buffer_sub_data(ctx, cmd, false, get_bound_buffer(ctx, p_target), p_offset, p_size);

glGetNamedBufferSubData: //GLuint p_buffer, GLintptr p_offset, GLsizeiptr p_size, void* p_data
    get_buffer_sub_data(ctx, cmd, true, trc_lookup_name(ctx->ns, TrcBuffer, p_buffer, -1), p_offset, p_size);

glGetTexImage: //GLenum p_target, GLint p_level, GLenum p_format, GLenum p_type, void * p_pixels
    ;

glGetBooleanv: //GLenum p_pname, GLboolean* p_data
    ;

glGetDoublev: //GLenum p_pname, GLdouble* p_data
    ;

glGetFloatv: //GLenum p_pname, GLfloat* p_data
    ;

glGetIntegerv: //GLenum p_pname, GLint* p_data
    ;

glGetString: //GLenum p_name
    ;

glGetStringi: //GLenum p_name, GLuint p_index
    ;

glGetDoublei_v: //GLenum p_target, GLuint p_index, GLdouble* p_data
    ;

glGetVertexAttribdv: //GLuint p_index, GLenum p_pname, GLdouble* p_params
    ;

glGetVertexAttribfv: //GLuint p_index, GLenum p_pname, GLfloat* p_params
    ;

glGetVertexAttribiv: //GLuint p_index, GLenum p_pname, GLint* p_params
    ;

glGetVertexAttribIiv: //GLuint p_index, GLenum p_pname, GLint* p_params
    ;

glGetVertexAttribIuiv: //GLuint p_index, GLenum p_pname, GLuint* p_params
    ;

glGetVertexAttribLdv: //GLuint p_index, GLenum p_pname, GLdouble* p_params
    ;

glGetVertexAttribPointerv: //GLuint p_index, GLenum p_pname, void ** p_pointer
    ;

glGetCompressedTexImage: //GLenum p_target, GLint p_level, void * p_img
    ;

glGetAttachedShaders: //GLuint p_program, GLsizei p_maxCount, GLsizei* p_count, GLuint* p_shaders
    ;

glGetActiveUniform: //GLuint p_program, GLuint p_index, GLsizei p_bufSize, GLsizei* p_length, GLint* p_size, GLenum* p_type, GLchar* p_name
    if (!get_real_program(ctx->ns, p_program)) ERROR("Invalid program name");

glGetActiveAttrib: //GLuint p_program, GLuint p_index, GLsizei p_bufSize, GLsizei* p_length, GLint* p_size, GLenum* p_type, GLchar* p_name
    if (!get_real_program(ctx->ns, p_program)) ERROR("Invalid program name");

glGetBooleanv: //GLenum p_pname, GLboolean* p_data
    ;

glGetDoublev: //GLenum p_pname, GLdouble* p_data
    ;

glGetFloatv: //GLenum p_pname, GLfloat* p_data
    ;

glGetIntegerv: //GLenum p_pname, GLint* p_data
    ;

glGetInteger64v: //GLenum p_pname, GLint64* p_data
    ;

glGetBooleani_v: //GLenum p_target, GLuint p_index, GLboolean* p_data
    ;

glGetIntegeri_v: //GLenum p_target, GLuint p_index, GLint* p_data
    ;

glGetFloati_v: //GLenum p_target, GLuint p_index, GLfloat* p_data
    ;

glGetDoublev: //GLenum p_pname, GLdouble* p_data
    ;

glGetInteger64i_v: //GLenum p_target, GLuint p_index, GLint64* p_data
    ;

glReadPixels: //GLint p_x, GLint p_y, GLsizei p_width, GLsizei p_height, GLenum p_format, GLenum p_type, void * p_pixels
    ;

glGetSamplerParameterfv: //GLuint p_sampler, GLenum p_pname, GLfloat* p_params
    if (!get_real_sampler(ctx->ns, p_sampler))
        ERROR("Invalid sampler name");

glGetSamplerParameteriv: //GLuint p_sampler, GLenum p_pname, GLint* p_params
    if (!get_real_sampler(ctx->ns, p_sampler))
        ERROR("Invalid sampler name");

glGetSamplerParameterIiv: //GLuint p_sampler, GLenum p_pname, GLint* p_params
    if (!get_real_sampler(ctx->ns, p_sampler))
        ERROR("Invalid sampler name");

glGetSamplerParameterIuiv: //GLuint p_sampler, GLenum p_pname, GLuint* p_params
    if (!get_real_sampler(ctx->ns, p_sampler))
        ERROR("Invalid sampler name");

glUniformBlockBinding: //GLuint p_program, GLuint p_uniformBlockIndex, GLuint p_uniformBlockBinding
    const trc_gl_program_rev_t* rev_ptr = get_program(ctx->ns, p_program);
    if (!rev_ptr) ERROR("Invalid program name");
    trc_gl_program_rev_t rev = *rev_ptr;
    if (p_uniformBlockBinding >= trc_gl_state_get_state_int(ctx->trace, GL_MAX_UNIFORM_BUFFER_BINDINGS, 0))
        ERROR("Invalid binding");
    uint uniform_block_count = rev.uniform_blocks->size / sizeof(trc_gl_program_uniform_block_t);
    trc_gl_program_uniform_block_t* blocks = trc_map_data(rev.uniform_blocks, TRC_MAP_READ);
    for (uint i = 0; i < uniform_block_count; i++) {
        if (blocks[i].fake == p_uniformBlockIndex) {
            real(p_program, blocks[i].real, p_uniformBlockBinding);
            trc_gl_program_rev_t newrev = rev;
            newrev.uniform_blocks = trc_create_data(ctx->trace, rev.uniform_blocks->size, blocks, 0);
            ((trc_gl_program_uniform_block_t*)trc_map_data(newrev.uniform_blocks, TRC_MAP_MODIFY))[i].binding = p_uniformBlockBinding;
            trc_unmap_data(newrev.uniform_blocks);
            set_program(&newrev);
            goto success;
        }
    }
    trc_add_error(cmd, "No such uniform block");
    success:
    trc_unmap_data(rev.uniform_blocks);

glUniform1f: //GLint p_location, GLfloat p_v0
    GLint loc;
    if ((loc=uniform(ctx, cmd, false, false, 1, 1, GL_FLOAT, NULL, NULL))<0) RETURN;
    real(loc, p_v0);

glUniform2f: //GLint p_location, GLfloat p_v0, GLfloat p_v1
    GLint loc;
    if ((loc=uniform(ctx, cmd, false, false, 2, 1, GL_FLOAT, NULL, NULL))<0) RETURN;
    real(loc, p_v0, p_v1);

glUniform3f: //GLint p_location, GLfloat p_v0, GLfloat p_v1, GLfloat p_v2
    GLint loc;
    if ((loc=uniform(ctx, cmd, false, false, 3, 1, GL_FLOAT, NULL, NULL))<0) RETURN;
    real(loc, p_v0, p_v1, p_v2);

glUniform4f: //GLint p_location, GLfloat p_v0, GLfloat p_v1, GLfloat p_v2, GLfloat p_v3
    GLint loc;
    if ((loc=uniform(ctx, cmd, false, false, 4, 1, GL_FLOAT, NULL, NULL))<0) RETURN;
    real(loc, p_v0, p_v1, p_v2, p_v3);

glUniform1i: //GLint p_location, GLint p_v0
    GLint loc;
    if ((loc=uniform(ctx, cmd, false, false, 1, 1, GL_INT, NULL, NULL))<0) RETURN;
    real(loc, p_v0);

glUniform2i: //GLint p_location, GLint p_v0, GLint p_v1
    GLint loc;
    if ((loc=uniform(ctx, cmd, false, false, 2, 1, GL_INT, NULL, NULL))<0) RETURN;
    real(loc, p_v0, p_v1);

glUniform3i: //GLint p_location, GLint p_v0, GLint p_v1, GLint p_v2
    GLint loc;
    if ((loc=uniform(ctx, cmd, false, false, 3, 1, GL_INT, NULL, NULL))<0) RETURN;
    real(loc, p_v0, p_v1, p_v2);

glUniform4i: //GLint p_location, GLint p_v0, GLint p_v1, GLint p_v2, GLint p_v3
    GLint loc;
    if ((loc=uniform(ctx, cmd, false, false, 4, 1, GL_INT, NULL, NULL))<0) RETURN;
    real(loc, p_v0, p_v1, p_v2, p_v3);

glUniform1ui: //GLint p_location, GLuint p_v0
    GLint loc;
    if ((loc=uniform(ctx, cmd, false, false, 1, 1, GL_UNSIGNED_INT, NULL, NULL))<0) RETURN;
    real(loc, p_v0);

glUniform2ui: //GLint p_location, GLuint p_v0, GLuint p_v1
    GLint loc;
    if ((loc=uniform(ctx, cmd, false, false, 2, 1, GL_UNSIGNED_INT, NULL, NULL))<0) RETURN;
    real(loc, p_v0, p_v1);

glUniform3ui: //GLint p_location, GLuint p_v0, GLuint p_v1, GLuint p_v2
    GLint loc;
    if ((loc=uniform(ctx, cmd, false, false, 3, 1, GL_UNSIGNED_INT, NULL, NULL))<0) RETURN;
    real(loc, p_v0, p_v1, p_v2);

glUniform4ui: //GLint p_location, GLuint p_v0, GLuint p_v1, GLuint p_v2, GLuint p_v3
    GLint loc;
    if ((loc=uniform(ctx, cmd, false, false, 4, 1, GL_UNSIGNED_INT, NULL, NULL))<0) RETURN;
    real(loc, p_v0, p_v1, p_v2, p_v3);

glUniform1d: //GLint p_location, GLdouble p_x
    GLint loc;
    if ((loc=uniform(ctx, cmd, false, false, 1, 1, GL_DOUBLE, NULL, NULL))<0) RETURN;
    real(loc, p_x);

glUniform2d: //GLint p_location, GLdouble p_x, GLdouble p_y
    GLint loc;
    if ((loc=uniform(ctx, cmd, false, false, 2, 1, GL_DOUBLE, NULL, NULL))<0) RETURN;
    real(loc, p_x, p_y);

glUniform3d: //GLint p_location, GLdouble p_x, GLdouble p_y, GLdouble p_z
    GLint loc;
    if ((loc=uniform(ctx, cmd, false, false, 3, 1, GL_DOUBLE, NULL, NULL))<0) RETURN;
    real(loc, p_x, p_y, p_z);

glUniform4d: //GLint p_location, GLdouble p_x, GLdouble p_y, GLdouble p_z, GLdouble p_w
    GLint loc;
    if ((loc=uniform(ctx, cmd, false, false, 4, 1, GL_DOUBLE, NULL, NULL))<0) RETURN;
    real(loc, p_x, p_y, p_z, p_w);

glUniform1fv: //GLint p_location, GLsizei p_count, const GLfloat* p_value
    GLfloat* values = replay_alloc(p_count*sizeof(GLfloat));
    GLint loc;
    if ((loc=uniform(ctx, cmd, false, true, 1, 1, GL_FLOAT, values, NULL))<0) RETURN;
    real(loc, p_count, values);

glUniform2fv: //GLint p_location, GLsizei p_count, const GLfloat* p_value
    GLfloat* values = replay_alloc(p_count*2*sizeof(GLfloat));
    GLint loc;
    if ((loc=uniform(ctx, cmd, false, true, 2, 1, GL_FLOAT, values, NULL))<0) RETURN;
    real(loc, p_count, values);

glUniform3fv: //GLint p_location, GLsizei p_count, const GLfloat* p_value
    GLfloat* values = replay_alloc(p_count*3*sizeof(GLfloat));
    GLint loc;
    if ((loc=uniform(ctx, cmd, false, true, 3, 1, GL_FLOAT, values, NULL))<0) RETURN;
    real(loc, p_count, values);

glUniform4fv: //GLint p_location, GLsizei p_count, const GLfloat* p_value
    GLfloat* values = replay_alloc(p_count*4*sizeof(GLfloat));
    GLint loc;
    if ((loc=uniform(ctx, cmd, false, true, 4, 1, GL_FLOAT, values, NULL))<0) RETURN;
    real(loc, p_count, values);

glUniform1iv: //GLint p_location, GLsizei p_count, const GLint* p_value
    GLint* values = replay_alloc(p_count*sizeof(GLint));
    GLint loc;
    if ((loc=uniform(ctx, cmd, false, true, 1, 1, GL_INT, values, NULL))<0) RETURN;
    real(loc, p_count, values);

glUniform2iv: //GLint p_location, GLsizei p_count, const GLint* p_value
    GLint* values = replay_alloc(p_count*2*sizeof(GLint));
    GLint loc;
    if ((loc=uniform(ctx, cmd, false, true, 2, 1, GL_INT, values, NULL))<0) RETURN;
    real(loc, p_count, values);

glUniform3iv: //GLint p_location, GLsizei p_count, const GLint* p_value
    GLint* values = replay_alloc(p_count*3*sizeof(GLint));
    GLint loc;
    if ((loc=uniform(ctx, cmd, false, true, 3, 1, GL_INT, values, NULL))<0) RETURN;
    real(loc, p_count, values);

glUniform4iv: //GLint p_location, GLsizei p_count, const GLint* p_value
    GLint* values = replay_alloc(p_count*4*sizeof(GLint));
    GLint loc;
    if ((loc=uniform(ctx, cmd, false, true, 4, 1, GL_INT, values, NULL))<0) RETURN;
    real(loc, p_count, values);

glUniform1uiv: //GLint p_location, GLsizei p_count, const GLuint* p_value
    GLuint* values = replay_alloc(p_count*sizeof(GLuint));
    GLint loc;
    if ((loc=uniform(ctx, cmd, false, true, 1, 1, GL_UNSIGNED_INT, values, NULL))<0) RETURN;
    real(loc, p_count, values);

glUniform2uiv: //GLint p_location, GLsizei p_count, const GLuint* p_value
    GLuint* values = replay_alloc(p_count*2*sizeof(GLuint));
    GLint loc;
    if ((loc=uniform(ctx, cmd, false, true, 2, 1, GL_UNSIGNED_INT, values, NULL))<0) RETURN;
    real(loc, p_count, values);

glUniform3uiv: //GLint p_location, GLsizei p_count, const GLuint* p_value
    GLuint* values = replay_alloc(p_count*3*sizeof(GLuint));
    GLint loc;
    if ((loc=uniform(ctx, cmd, false, true, 3, 1, GL_UNSIGNED_INT, values, NULL))<0) RETURN;
    real(loc, p_count, values);

glUniform4uiv: //GLint p_location, GLsizei p_count, const GLuint* p_value
    GLuint* values = replay_alloc(p_count*4*sizeof(GLuint));
    GLint loc;
    if ((loc=uniform(ctx, cmd, false, true, 4, 1, GL_UNSIGNED_INT, values, NULL))<0) RETURN;
    real(loc, p_count, values);

glUniform1dv: //GLint p_location, GLsizei p_count, const GLdouble* p_value
    GLdouble* values = replay_alloc(p_count*sizeof(GLdouble));
    GLint loc;
    if ((loc=uniform(ctx, cmd, false, true, 1, 1, GL_DOUBLE, values, NULL))<0) RETURN;
    real(loc, p_count, values);

glUniform2dv: //GLint p_location, GLsizei p_count, const GLdouble* p_value
    GLdouble* values = replay_alloc(p_count*2*sizeof(GLdouble));
    GLint loc;
    if ((loc=uniform(ctx, cmd, false, true, 2, 1, GL_DOUBLE, values, NULL))<0) RETURN;
    real(loc, p_count, values);

glUniform3dv: //GLint p_location, GLsizei p_count, const GLdouble* p_value
    GLdouble* values = replay_alloc(p_count*3*sizeof(GLdouble));
    GLint loc;
    if ((loc=uniform(ctx, cmd, false, true, 3, 1, GL_UNSIGNED_INT, values, NULL))<0) RETURN;
    real(loc, p_count, values);

glUniform4dv: //GLint p_location, GLsizei p_count, const GLdouble* p_value
    GLdouble* values = replay_alloc(p_count*4*sizeof(GLdouble));
    GLint loc;
    if ((loc=uniform(ctx, cmd, false, true, 4, 1, GL_UNSIGNED_INT, values, NULL))<0) RETURN;
    real(loc, p_count, values);

glUniformMatrix2fv: //GLint p_location, GLsizei p_count, GLboolean p_transpose, const GLfloat* p_value
    GLfloat* values = replay_alloc(p_count*4*sizeof(GLfloat));
    GLint loc;
    if ((loc=uniform(ctx, cmd, false, true, 2, 2, GL_FLOAT, values, NULL))<0) RETURN;
    real(loc, p_count, p_transpose, values);

glUniformMatrix3fv: //GLint p_location, GLsizei p_count, GLboolean p_transpose, const GLfloat* p_value
    GLfloat* values = replay_alloc(p_count*9*sizeof(GLfloat));
    GLint loc;
    if ((loc=uniform(ctx, cmd, false, true, 3, 3, GL_FLOAT, values, NULL))<0) RETURN;
    real(loc, p_count, p_transpose, values);

glUniformMatrix4fv: //GLint p_location, GLsizei p_count, GLboolean p_transpose, const GLfloat* p_value
    GLfloat* values = replay_alloc(p_count*16*sizeof(GLfloat));
    GLint loc;
    if ((loc=uniform(ctx, cmd, false, true, 4, 4, GL_FLOAT, values, NULL))<0) RETURN;
    real(loc, p_count, p_transpose, values);

glUniformMatrix2x3fv: //GLint p_location, GLsizei p_count, GLboolean p_transpose, const GLfloat* p_value
    GLfloat* values = replay_alloc(p_count*6*sizeof(GLfloat));
    GLint loc;
    if ((loc=uniform(ctx, cmd, false, true, 2, 3, GL_FLOAT, values, NULL))<0) RETURN;
    real(loc, p_count, p_transpose, values);

glUniformMatrix3x2fv: //GLint p_location, GLsizei p_count, GLboolean p_transpose, const GLfloat* p_value
    GLfloat* values = replay_alloc(p_count*6*sizeof(GLfloat));
    GLint loc;
    if ((loc=uniform(ctx, cmd, false, true, 3, 2, GL_FLOAT, values, NULL))<0) RETURN;
    real(loc, p_count, p_transpose, values);

glUniformMatrix2x4fv: //GLint p_location, GLsizei p_count, GLboolean p_transpose, const GLfloat* p_value
    GLfloat* values = replay_alloc(p_count*8*sizeof(GLfloat));
    GLint loc;
    if ((loc=uniform(ctx, cmd, false, true, 2, 4, GL_FLOAT, values, NULL))<0) RETURN;
    real(loc, p_count, p_transpose, values);

glUniformMatrix4x2fv: //GLint p_location, GLsizei p_count, GLboolean p_transpose, const GLfloat* p_value
    GLfloat* values = replay_alloc(p_count*8*sizeof(GLfloat));
    GLint loc;
    if ((loc=uniform(ctx, cmd, false, true, 4, 2, GL_FLOAT, values, NULL))<0) RETURN;
    real(loc, p_count, p_transpose, values);

glUniformMatrix3x4fv: //GLint p_location, GLsizei p_count, GLboolean p_transpose, const GLfloat* p_value
    GLfloat* values = replay_alloc(p_count*12*sizeof(GLfloat));
    GLint loc;
    if ((loc=uniform(ctx, cmd, false, true, 3, 4, GL_FLOAT, values, NULL))<0) RETURN;
    real(loc, p_count, p_transpose, values);

glUniformMatrix4x3fv: //GLint p_location, GLsizei p_count, GLboolean p_transpose, const GLfloat* p_value
    GLfloat* values = replay_alloc(p_count*12*sizeof(GLfloat));
    GLint loc;
    if ((loc=uniform(ctx, cmd, false, true, 4, 3, GL_FLOAT, values, NULL))<0) RETURN;
    real(loc, p_count, p_transpose, values);

glUniformMatrix2dv: //GLint p_location, GLsizei p_count, GLboolean p_transpose, const GLdouble* p_value
    GLdouble* values = replay_alloc(p_count*4*sizeof(GLdouble));
    GLint loc;
    if ((loc=uniform(ctx, cmd, false, true, 2, 2, GL_DOUBLE, values, NULL))<0) RETURN;
    real(loc, p_count, p_transpose, values);

glUniformMatrix3dv: //GLint p_location, GLsizei p_count, GLboolean p_transpose, const GLdouble* p_value
    GLdouble* values = replay_alloc(p_count*9*sizeof(GLdouble));
    GLint loc;
    if ((loc=uniform(ctx, cmd, false, true, 3, 3, GL_DOUBLE, values, NULL))<0) RETURN;
    real(loc, p_count, p_transpose, values);

glUniformMatrix4dv: //GLint p_location, GLsizei p_count, GLboolean p_transpose, const GLdouble* p_value
    GLdouble* values = replay_alloc(p_count*16*sizeof(GLdouble));
    GLint loc;
    if ((loc=uniform(ctx, cmd, false, true, 4, 4, GL_DOUBLE, values, NULL))<0) RETURN;
    real(loc, p_count, p_transpose, values);

glUniformMatrix2x3dv: //GLint p_location, GLsizei p_count, GLboolean p_transpose, const GLdouble* p_value
    GLdouble* values = replay_alloc(p_count*6*sizeof(GLdouble));
    GLint loc;
    if ((loc=uniform(ctx, cmd, false, true, 2, 3, GL_DOUBLE, values, NULL))<0) RETURN;
    real(loc, p_count, p_transpose, values);

glUniformMatrix3x2dv: //GLint p_location, GLsizei p_count, GLboolean p_transpose, const GLdouble* p_value
    GLdouble* values = replay_alloc(p_count*6*sizeof(GLdouble));
    GLint loc;
    if ((loc=uniform(ctx, cmd, false, true, 3, 2, GL_DOUBLE, values, NULL))<0) RETURN;
    real(loc, p_count, p_transpose, values);

glUniformMatrix2x4dv: //GLint p_location, GLsizei p_count, GLboolean p_transpose, const GLdouble* p_value
    GLdouble* values = replay_alloc(p_count*8*sizeof(GLdouble));
    GLint loc;
    if ((loc=uniform(ctx, cmd, false, true, 2, 4, GL_DOUBLE, values, NULL))<0) RETURN;
    real(loc, p_count, p_transpose, values);

glUniformMatrix4x2dv: //GLint p_location, GLsizei p_count, GLboolean p_transpose, const GLdouble* p_value
    GLdouble* values = replay_alloc(p_count*8*sizeof(GLdouble));
    GLint loc;
    if ((loc=uniform(ctx, cmd, false, true, 4, 2, GL_DOUBLE, values, NULL))<0) RETURN;
    real(loc, p_count, p_transpose, values);

glUniformMatrix3x4dv: //GLint p_location, GLsizei p_count, GLboolean p_transpose, const GLdouble* p_value
    GLdouble* values = replay_alloc(p_count*12*sizeof(GLdouble));
    GLint loc;
    if ((loc=uniform(ctx, cmd, false, true, 3, 4, GL_DOUBLE, values, NULL))<0) RETURN;
    real(loc, p_count, p_transpose, values);

glUniformMatrix4x3dv: //GLint p_location, GLsizei p_count, GLboolean p_transpose, const GLdouble* p_value
    GLdouble* values = replay_alloc(p_count*12*sizeof(GLdouble));
    GLint loc;
    if ((loc=uniform(ctx, cmd, false, true, 4, 3, GL_DOUBLE, values, NULL))<0) RETURN;
    real(loc, p_count, p_transpose, values);

glProgramUniform1f: //GLuint p_program, GLint p_location, GLfloat p_v0
    GLint loc; GLuint program;
    if ((loc=uniform(ctx, cmd, true, false, 1, 1, GL_FLOAT, NULL, &program))<0) RETURN;
    real(program, loc, p_v0);

glProgramUniform2f: //GLuint p_program, GLint p_location, GLfloat p_v0, GLfloat p_v1
    GLint loc; GLuint program;
    if ((loc=uniform(ctx, cmd, true, false, 2, 1, GL_FLOAT, NULL, &program))<0) RETURN;
    real(program, loc, p_v0, p_v1);

glProgramUniform3f: //GLuint p_program, GLint p_location, GLfloat p_v0, GLfloat p_v1, GLfloat p_v2
    GLint loc; GLuint program;
    if ((loc=uniform(ctx, cmd, true, false, 3, 1, GL_FLOAT, NULL, &program))<0) RETURN;
    real(program, loc, p_v0, p_v1, p_v2);

glProgramUniform4f: //GLuint p_program, GLint p_location, GLfloat p_v0, GLfloat p_v1, GLfloat p_v2, GLfloat p_v3
    GLint loc; GLuint program;
    if ((loc=uniform(ctx, cmd, true, false, 4, 1, GL_FLOAT, NULL, &program))<0) RETURN;
    real(program, loc, p_v0, p_v1, p_v2, p_v3);

glProgramUniform1i: //GLuint p_program, GLint p_location, GLint p_v0
    GLint loc; GLuint program;
    if ((loc=uniform(ctx, cmd, true, false, 1, 1, GL_INT, NULL, &program))<0) RETURN;
    real(program, loc, p_v0);

glProgramUniform2i: //GLuint p_program, GLint p_location, GLint p_v0, GLint p_v1
    GLint loc; GLuint program;
    if ((loc=uniform(ctx, cmd, true, false, 2, 1, GL_INT, NULL, &program))<0) RETURN;
    real(program, loc, p_v0, p_v1);

glProgramUniform3i: //GLuint p_program, GLint p_location, GLint p_v0, GLint p_v1, GLint p_v2
    GLint loc; GLuint program;
    if ((loc=uniform(ctx, cmd, true, false, 3, 1, GL_INT, NULL, &program))<0) RETURN;
    real(program, loc, p_v0, p_v1, p_v2);

glProgramUniform4i: //GLuint p_program, GLint p_location, GLint p_v0, GLint p_v1, GLint p_v2, GLint p_v3
    GLint loc; GLuint program;
    if ((loc=uniform(ctx, cmd, true, false, 4, 1, GL_INT, NULL, &program))<0) RETURN;
    real(program, loc, p_v0, p_v1, p_v2, p_v3);

glProgramUniform1ui: //GLuint p_program, GLint p_location, GLuint p_v0
    GLint loc; GLuint program;
    if ((loc=uniform(ctx, cmd, true, false, 1, 1, GL_UNSIGNED_INT, NULL, &program))<0) RETURN;
    real(program, loc, p_v0);

glProgramUniform2ui: //GLuint p_program, GLint p_location, GLuint p_v0, GLuint p_v1
    GLint loc; GLuint program;
    if ((loc=uniform(ctx, cmd, true, false, 2, 1, GL_UNSIGNED_INT, NULL, &program))<0) RETURN;
    real(program, loc, p_v0, p_v1);

glProgramUniform3ui: //GLuint p_program, GLint p_location, GLuint p_v0, GLuint p_v1, GLuint p_v2
    GLint loc; GLuint program;
    if ((loc=uniform(ctx, cmd, true, false, 3, 1, GL_UNSIGNED_INT, NULL, &program))<0) RETURN;
    real(program, loc, p_v0, p_v1, p_v2);

glProgramUniform4ui: //GLuint p_program, GLint p_location, GLuint p_v0, GLuint p_v1, GLuint p_v2, GLuint p_v3
    GLint loc; GLuint program;
    if ((loc=uniform(ctx, cmd, true, false, 4, 1, GL_UNSIGNED_INT, NULL, &program))<0) RETURN;
    real(program, loc, p_v0, p_v1, p_v2, p_v3);

glProgramUniform1d: //GLuint p_program, GLint p_location, GLdouble p_v0
    GLint loc; GLuint program;
    if ((loc=uniform(ctx, cmd, true, false, 1, 1, GL_DOUBLE, NULL, &program))<0) RETURN;
    real(program, loc, p_v0);

glProgramUniform2d: //GLuint p_program, GLint p_location, GLdouble p_v0, GLdouble p_v1
    GLint loc; GLuint program;
    if ((loc=uniform(ctx, cmd, true, false, 2, 1, GL_DOUBLE, NULL, &program))<0) RETURN;
    real(program, loc, p_v0, p_v1);

glProgramUniform3d: //GLuint p_program, GLint p_location, GLdouble p_v0, GLdouble p_v1, GLdouble p_v2
    GLint loc; GLuint program;
    if ((loc=uniform(ctx, cmd, true, false, 3, 1, GL_DOUBLE, NULL, &program))<0) RETURN;
    real(program, loc, p_v0, p_v1, p_v2);

glProgramUniform4d: //GLuint p_program, GLint p_location, GLdouble p_v0, GLdouble p_v1, GLdouble p_v2, GLdouble p_v3
    GLint loc; GLuint program;
    if ((loc=uniform(ctx, cmd, true, false, 4, 1, GL_DOUBLE, NULL, &program))<0) RETURN;
    real(program, loc, p_v0, p_v1, p_v2, p_v3);

glProgramUniform1fv: //GLuint p_program, GLint p_location, GLsizei p_count, const GLfloat* p_value
    GLfloat* values = replay_alloc(p_count*sizeof(GLfloat));
    GLint loc; GLuint program;
    if ((loc=uniform(ctx, cmd, true, true, 1, 1, GL_FLOAT, values, &program))<0) RETURN;
    real(program, loc, p_count, values);

glProgramUniform2fv: //GLuint p_program, GLint p_location, GLsizei p_count, const GLfloat* p_value
    GLfloat* values = replay_alloc(p_count*2*sizeof(GLfloat));
    GLint loc; GLuint program;
    if ((loc=uniform(ctx, cmd, true, true, 2, 1, GL_FLOAT, values, &program))<0) RETURN;
    real(program, loc, p_count, values);

glProgramUniform3fv: //GLuint p_program, GLint p_location, GLsizei p_count, const GLfloat* p_value
    GLfloat* values = replay_alloc(p_count*3*sizeof(GLfloat));
    GLint loc; GLuint program;
    if ((loc=uniform(ctx, cmd, true, true, 3, 1, GL_FLOAT, values, &program))<0) RETURN;
    real(program, loc, p_count, values);

glProgramUniform4fv: //GLuint p_program, GLint p_location, GLsizei p_count, const GLfloat* p_value
    GLfloat* values = replay_alloc(p_count*4*sizeof(GLfloat));
    GLint loc; GLuint program;
    if ((loc=uniform(ctx, cmd, true, true, 4, 1, GL_FLOAT, values, &program))<0) RETURN;
    real(program, loc, p_count, values);

glProgramUniform1iv: //GLuint p_program, GLint p_location, GLsizei p_count, const GLint* p_value
    GLint* values = replay_alloc(p_count*sizeof(GLint));
    GLint loc; GLuint program;
    if ((loc=uniform(ctx, cmd, true, true, 1, 1, GL_INT, values, &program))<0) RETURN;
    real(program, loc, p_count, values);

glProgramUniform2iv: //GLuint p_program, GLint p_location, GLsizei p_count, const GLint* p_value
    GLint* values = replay_alloc(p_count*2*sizeof(GLint));
    GLint loc; GLuint program;
    if ((loc=uniform(ctx, cmd, true, true, 2, 1, GL_INT, values, &program))<0) RETURN;
    real(program, loc, p_count, values);

glProgramUniform3iv: //GLuint p_program, GLint p_location, GLsizei p_count, const GLint* p_value
    GLint* values = replay_alloc(p_count*3*sizeof(GLint));
    GLint loc; GLuint program;
    if ((loc=uniform(ctx, cmd, true, true, 3, 1, GL_INT, values, &program))<0) RETURN;
    real(program, loc, p_count, values);

glProgramUniform4iv: //GLuint p_program, GLint p_location, GLsizei p_count, const GLint* p_value
    GLint* values = replay_alloc(p_count*4*sizeof(GLint));
    GLint loc; GLuint program;
    if ((loc=uniform(ctx, cmd, true, true, 4, 1, GL_INT, values, &program))<0) RETURN;
    real(program, loc, p_count, values);

glProgramUniform1uiv: //GLuint p_program, GLint p_location, GLsizei p_count, const GLuint* p_value
    GLuint* values = replay_alloc(p_count*sizeof(GLuint));
    GLint loc; GLuint program;
    if ((loc=uniform(ctx, cmd, true, true, 1, 1, GL_UNSIGNED_INT, values, &program))<0) RETURN;
    real(program, loc, p_count, values);

glProgramUniform2uiv: //GLuint p_program, GLint p_location, GLsizei p_count, const GLuint* p_value
    GLuint* values = replay_alloc(p_count*2*sizeof(GLuint));
    GLint loc; GLuint program;
    if ((loc=uniform(ctx, cmd, true, true, 2, 1, GL_UNSIGNED_INT, values, &program))<0) RETURN;
    real(program, loc, p_count, values);

glProgramUniform3uiv: //GLuint p_program, GLint p_location, GLsizei p_count, const GLuint* p_value
    GLuint* values = replay_alloc(p_count*3*sizeof(GLuint));
    GLint loc; GLuint program;
    if ((loc=uniform(ctx, cmd, true, true, 3, 1, GL_UNSIGNED_INT, values, &program))<0) RETURN;
    real(program, loc, p_count, values);

glProgramUniform4uiv: //GLuint p_program, GLint p_location, GLsizei p_count, const GLuint* p_value
    GLuint* values = replay_alloc(p_count*4*sizeof(GLuint));
    GLint loc; GLuint program;
    if ((loc=uniform(ctx, cmd, true, true, 4, 1, GL_UNSIGNED_INT, values, &program))<0) RETURN;
    real(program, loc, p_count, values);

glProgramUniform1dv: //GLuint p_program, GLint p_location, GLsizei p_count, const GLdouble* p_value
    GLdouble* values = replay_alloc(p_count*sizeof(GLdouble));
    GLint loc; GLuint program;
    if ((loc=uniform(ctx, cmd, true, true, 1, 1, GL_DOUBLE, values, &program))<0) RETURN;
    real(program, loc, p_count, values);

glProgramUniform2dv: //GLuint p_program, GLint p_location, GLsizei p_count, const GLdouble* p_value
    GLdouble* values = replay_alloc(p_count*2*sizeof(GLdouble));
    GLint loc; GLuint program;
    if ((loc=uniform(ctx, cmd, true, true, 2, 1, GL_DOUBLE, values, &program))<0) RETURN;
    real(program, loc, p_count, values);

glProgramUniform3dv: //GLuint p_program, GLint p_location, GLsizei p_count, const GLdouble* p_value
    GLdouble* values = replay_alloc(p_count*3*sizeof(GLdouble));
    GLint loc; GLuint program;
    if ((loc=uniform(ctx, cmd, true, true, 3, 1, GL_UNSIGNED_INT, values, &program))<0) RETURN;
    real(program, loc, p_count, values);

glProgramUniform4dv: //GLuint p_program, GLint p_location, GLsizei p_count, const GLdouble* p_value
    GLdouble* values = replay_alloc(p_count*4*sizeof(GLdouble));
    GLint loc; GLuint program;
    if ((loc=uniform(ctx, cmd, true, true, 4, 1, GL_UNSIGNED_INT, values, &program))<0) RETURN;
    real(program, loc, p_count, values);

glProgramUniformMatrix2fv: //GLuint p_program, GLint p_location, GLsizei p_count, GLboolean p_transpose, const GLfloat* p_value
    GLfloat* values = replay_alloc(p_count*4*sizeof(GLfloat));
    GLint loc; GLuint program;
    if ((loc=uniform(ctx, cmd, true, true, 2, 2, GL_FLOAT, values, &program))<0) RETURN;
    real(program, loc, p_count, p_transpose, values);

glProgramUniformMatrix3fv: //GLuint p_program, GLint p_location, GLsizei p_count, GLboolean p_transpose, const GLfloat* p_value
    GLfloat* values = replay_alloc(p_count*9*sizeof(GLfloat));
    GLint loc; GLuint program;
    if ((loc=uniform(ctx, cmd, true, true, 3, 3, GL_FLOAT, values, &program))<0) RETURN;
    real(program, loc, p_count, p_transpose, values);

glProgramUniformMatrix4fv: //GLuint p_program, GLint p_location, GLsizei p_count, GLboolean p_transpose, const GLfloat* p_value
    GLfloat* values = replay_alloc(p_count*16*sizeof(GLfloat));
    GLint loc; GLuint program;
    if ((loc=uniform(ctx, cmd, true, true, 4, 4, GL_FLOAT, values, &program))<0) RETURN;
    real(program, loc, p_count, p_transpose, values);

glProgramUniformMatrix2x3fv: //GLuint p_program, GLint p_location, GLsizei p_count, GLboolean p_transpose, const GLfloat* p_value
    GLfloat* values = replay_alloc(p_count*6*sizeof(GLfloat));
    GLint loc; GLuint program;
    if ((loc=uniform(ctx, cmd, true, true, 2, 3, GL_FLOAT, values, &program))<0) RETURN;
    real(program, loc, p_count, p_transpose, values);

glProgramUniformMatrix3x2fv: //GLuint p_program, GLint p_location, GLsizei p_count, GLboolean p_transpose, const GLfloat* p_value
    GLfloat* values = replay_alloc(p_count*6*sizeof(GLfloat));
    GLint loc; GLuint program;
    if ((loc=uniform(ctx, cmd, true, true, 3, 2, GL_FLOAT, values, &program))<0) RETURN;
    real(program, loc, p_count, p_transpose, values);

glProgramUniformMatrix2x4fv: //GLuint p_program, GLint p_location, GLsizei p_count, GLboolean p_transpose, const GLfloat* p_value
    GLfloat* values = replay_alloc(p_count*8*sizeof(GLfloat));
    GLint loc; GLuint program;
    if ((loc=uniform(ctx, cmd, true, true, 2, 4, GL_FLOAT, values, &program))<0) RETURN;
    real(program, loc, p_count, p_transpose, values);

glProgramUniformMatrix4x2fv: //GLuint p_program, GLint p_location, GLsizei p_count, GLboolean p_transpose, const GLfloat* p_value
    GLfloat* values = replay_alloc(p_count*8*sizeof(GLfloat));
    GLint loc; GLuint program;
    if ((loc=uniform(ctx, cmd, true, true, 4, 2, GL_FLOAT, values, &program))<0) RETURN;
    real(program, loc, p_count, p_transpose, values);

glProgramUniformMatrix3x4fv: //GLuint p_program, GLint p_location, GLsizei p_count, GLboolean p_transpose, const GLfloat* p_value
    GLfloat* values = replay_alloc(p_count*12*sizeof(GLfloat));
    GLint loc; GLuint program;
    if ((loc=uniform(ctx, cmd, true, true, 3, 4, GL_FLOAT, values, &program))<0) RETURN;
    real(program, loc, p_count, p_transpose, values);

glProgramUniformMatrix4x3fv: //GLuint p_program, GLint p_location, GLsizei p_count, GLboolean p_transpose, const GLfloat* p_value
    GLfloat* values = replay_alloc(p_count*12*sizeof(GLfloat));
    GLint loc; GLuint program;
    if ((loc=uniform(ctx, cmd, true, true, 4, 3, GL_FLOAT, values, &program))<0) RETURN;
    real(program, loc, p_count, p_transpose, values);

glProgramUniformMatrix2dv: //GLuint p_program, GLint p_location, GLsizei p_count, GLboolean p_transpose, const GLdouble* p_value
    GLdouble* values = replay_alloc(p_count*4*sizeof(GLdouble));
    GLint loc; GLuint program;
    if ((loc=uniform(ctx, cmd, true, true, 2, 2, GL_DOUBLE, values, &program))<0) RETURN;
    real(program, loc, p_count, p_transpose, values);

glProgramUniformMatrix3dv: //GLuint p_program, GLint p_location, GLsizei p_count, GLboolean p_transpose, const GLdouble* p_value
    GLdouble* values = replay_alloc(p_count*9*sizeof(GLdouble));
    GLint loc; GLuint program;
    if ((loc=uniform(ctx, cmd, true, true, 3, 3, GL_DOUBLE, values, &program))<0) RETURN;
    real(program, loc, p_count, p_transpose, values);

glProgramUniformMatrix4dv: //GLuint p_program, GLint p_location, GLsizei p_count, GLboolean p_transpose, const GLdouble* p_value
    GLdouble* values = replay_alloc(p_count*16*sizeof(GLdouble));
    GLint loc; GLuint program;
    if ((loc=uniform(ctx, cmd, true, true, 4, 4, GL_DOUBLE, values, &program))<0) RETURN;
    real(program, loc, p_count, p_transpose, values);

glProgramUniformMatrix2x3dv: //GLuint p_program, GLint p_location, GLsizei p_count, GLboolean p_transpose, const GLdouble* p_value
    GLdouble* values = replay_alloc(p_count*6*sizeof(GLdouble));
    GLint loc; GLuint program;
    if ((loc=uniform(ctx, cmd, true, true, 2, 3, GL_DOUBLE, values, &program))<0) RETURN;
    real(program, loc, p_count, p_transpose, values);

glProgramUniformMatrix3x2dv: //GLuint p_program, GLint p_location, GLsizei p_count, GLboolean p_transpose, const GLdouble* p_value
    GLdouble* values = replay_alloc(p_count*6*sizeof(GLdouble));
    GLint loc; GLuint program;
    if ((loc=uniform(ctx, cmd, true, true, 3, 2, GL_DOUBLE, values, &program))<0) RETURN;
    real(program, loc, p_count, p_transpose, values);

glProgramUniformMatrix2x4dv: //GLuint p_program, GLint p_location, GLsizei p_count, GLboolean p_transpose, const GLdouble* p_value
    GLdouble* values = replay_alloc(p_count*8*sizeof(GLdouble));
    GLint loc; GLuint program;
    if ((loc=uniform(ctx, cmd, true, true, 2, 4, GL_DOUBLE, values, &program))<0) RETURN;
    real(program, loc, p_count, p_transpose, values);

glProgramUniformMatrix4x2dv: //GLuint p_program, GLint p_location, GLsizei p_count, GLboolean p_transpose, const GLdouble* p_value
    GLdouble* values = replay_alloc(p_count*8*sizeof(GLdouble));
    GLint loc; GLuint program;
    if ((loc=uniform(ctx, cmd, true, true, 4, 2, GL_DOUBLE, values, &program))<0) RETURN;
    real(program, loc, p_count, p_transpose, values);

glProgramUniformMatrix3x4dv: //GLuint p_program, GLint p_location, GLsizei p_count, GLboolean p_transpose, const GLdouble* p_value
    GLdouble* values = replay_alloc(p_count*12*sizeof(GLdouble));
    GLint loc; GLuint program;
    if ((loc=uniform(ctx, cmd, true, true, 3, 4, GL_DOUBLE, values, &program))<0) RETURN;
    real(program, loc, p_count, p_transpose, values);

glProgramUniformMatrix4x3dv: //GLuint p_program, GLint p_location, GLsizei p_count, GLboolean p_transpose, const GLdouble* p_value
    GLdouble* values = replay_alloc(p_count*12*sizeof(GLdouble));
    GLint loc; GLuint program;
    if ((loc=uniform(ctx, cmd, true, true, 4, 3, GL_DOUBLE, values, &program))<0) RETURN;
    real(program, loc, p_count, p_transpose, values);

//TODO: There is some duplicate code among glVertexAttrib*Pointer and gl*VertexAttribArray
glVertexAttribPointer: //GLuint p_index, GLint p_size, GLenum p_type, GLboolean p_normalized, GLsizei p_stride, const void* p_pointer
    //if (p_pointer > UINTPTR_MAX) //TODO
    if (!trc_gl_state_get_bound_vao(ctx->trace)) RETURN;
    trc_gl_vao_rev_t rev = *(const trc_gl_vao_rev_t*)trc_obj_get_rev(trc_gl_state_get_bound_vao(ctx->trace), -1);
    if (p_index < rev.attribs->size/sizeof(trc_gl_vao_attrib_t)) {
        trc_data_t* newattribs = trc_create_data(ctx->trace, rev.attribs->size, trc_map_data(rev.attribs, TRC_MAP_READ), 0);
        trc_unmap_data(rev.attribs);
        trc_gl_vao_attrib_t* a = &((trc_gl_vao_attrib_t*)trc_map_data(newattribs, TRC_MAP_MODIFY))[p_index];
        a->normalized = p_normalized;
        a->integer = false;
        a->size = p_size;
        a->stride = p_stride;
        a->offset = p_pointer;
        a->type = p_type;
        trc_set_obj_ref(&a->buffer, trc_gl_state_get_bound_buffer(ctx->trace, GL_ARRAY_BUFFER));
        trc_unmap_freeze_data(ctx->trace, newattribs);
        rev.attribs = newattribs;
        set_vao(&rev);
    }

glVertexAttribIPointer: //GLuint p_index, GLint p_size, GLenum p_type, GLsizei p_stride, const void* p_pointer
    //if (p_pointer > UINTPTR_MAX) //TODO
    if (!trc_gl_state_get_bound_vao(ctx->trace)) RETURN;
    trc_gl_vao_rev_t rev = *(const trc_gl_vao_rev_t*)trc_obj_get_rev(trc_gl_state_get_bound_vao(ctx->trace), -1);
    if (p_index < rev.attribs->size/sizeof(trc_gl_vao_attrib_t)) {
        trc_data_t* newattribs = trc_create_data(ctx->trace, rev.attribs->size, trc_map_data(rev.attribs, TRC_MAP_READ), 0);
        trc_unmap_data(rev.attribs);
        trc_gl_vao_attrib_t* a = &((trc_gl_vao_attrib_t*)trc_map_data(newattribs, TRC_MAP_MODIFY))[p_index];
        a->integer = true;
        a->size = p_size;
        a->stride = p_stride;
        a->offset = p_pointer;
        a->type = p_type;
        trc_set_obj_ref(&a->buffer, trc_gl_state_get_bound_buffer(ctx->trace, GL_ARRAY_BUFFER));
        trc_unmap_freeze_data(ctx->trace, newattribs);
        rev.attribs = newattribs;
        set_vao(&rev);
    }

glEnableVertexAttribArray: //GLuint p_index
    if (!trc_gl_state_get_bound_vao(ctx->trace)) RETURN;
    trc_gl_vao_rev_t rev = *(const trc_gl_vao_rev_t*)trc_obj_get_rev(trc_gl_state_get_bound_vao(ctx->trace), -1);
    if (p_index < rev.attribs->size/sizeof(trc_gl_vao_attrib_t)) {
        trc_data_t* newattribs = trc_create_data(ctx->trace, rev.attribs->size, trc_map_data(rev.attribs, TRC_MAP_READ), 0);
        trc_unmap_data(rev.attribs);
        trc_gl_vao_attrib_t* a = &((trc_gl_vao_attrib_t*)trc_map_data(newattribs, TRC_MAP_MODIFY))[p_index];
        a->enabled = true;
        trc_unmap_freeze_data(ctx->trace, newattribs);
        rev.attribs = newattribs;
        set_vao(&rev);
    }

glDisableVertexAttribArray: //GLuint p_index
    if (!trc_gl_state_get_bound_vao(ctx->trace)) RETURN;
    trc_gl_vao_rev_t rev = *(const trc_gl_vao_rev_t*)trc_obj_get_rev(trc_gl_state_get_bound_vao(ctx->trace), -1);
    if (p_index < rev.attribs->size/sizeof(trc_gl_vao_attrib_t)) {
        trc_data_t* newattribs = trc_create_data(ctx->trace, rev.attribs->size, trc_map_data(rev.attribs, TRC_MAP_READ), 0);
        trc_unmap_data(rev.attribs);
        trc_gl_vao_attrib_t* a = &((trc_gl_vao_attrib_t*)trc_map_data(newattribs, TRC_MAP_MODIFY))[p_index];
        a->enabled = true;
        trc_unmap_freeze_data(ctx->trace, newattribs);
        rev.attribs = newattribs;
        set_vao(&rev);
    }

glVertexAttribDivisor: //GLuint p_index, GLuint p_divisor
    if (!trc_gl_state_get_bound_vao(ctx->trace)) RETURN;
    trc_gl_vao_rev_t rev = *(const trc_gl_vao_rev_t*)trc_obj_get_rev(trc_gl_state_get_bound_vao(ctx->trace), -1);
    if (p_index < rev.attribs->size/sizeof(trc_gl_vao_attrib_t)) {
        trc_data_t* newattribs = trc_create_data(ctx->trace, rev.attribs->size, trc_map_data(rev.attribs, TRC_MAP_READ), 0);
        trc_unmap_data(rev.attribs);
        trc_gl_vao_attrib_t* a = &((trc_gl_vao_attrib_t*)trc_map_data(newattribs, TRC_MAP_MODIFY))[p_index];
        a->divisor = p_divisor;
        trc_unmap_freeze_data(ctx->trace, newattribs);
        rev.attribs = newattribs;
        set_vao(&rev);
    }

glVertexAttrib1f: //GLuint p_index, GLfloat p_v0
    vertex_attrib(ctx, cmd, 1, GL_FLOAT, false, false, GL_FLOAT);

glVertexAttrib1s: //GLuint p_index, GLshort p_v0
    vertex_attrib(ctx, cmd, 1, GL_INT, false, false, GL_FLOAT);

glVertexAttrib1d: //GLuint p_index, GLdouble p_v0
    vertex_attrib(ctx, cmd, 1, GL_DOUBLE, false, false, GL_FLOAT);

glVertexAttribI1i: //GLuint p_index, GLint p_v0
    vertex_attrib(ctx, cmd, 1, GL_INT, false, false, GL_INT);

glVertexAttribI1ui: //GLuint p_index, GLuint p_v0
    vertex_attrib(ctx, cmd, 1, GL_UNSIGNED_INT, false, false, GL_UNSIGNED_INT);

glVertexAttribL1d: //GLuint p_index, GLdouble p_v0
    vertex_attrib(ctx, cmd, 1, GL_DOUBLE, false, false, GL_DOUBLE);

glVertexAttrib2f: //GLuint p_index, GLfloat p_v0, GLfloat p_v1
    vertex_attrib(ctx, cmd, 2, GL_FLOAT, false, false, GL_FLOAT);

glVertexAttrib2s: //GLuint p_index, GLshort p_v0, GLshort p_v1
    vertex_attrib(ctx, cmd, 2, GL_INT, false, false, GL_FLOAT);

glVertexAttrib2d: //GLuint p_index, GLdouble p_v0, GLdouble p_v1
    vertex_attrib(ctx, cmd, 2, GL_DOUBLE, false, false, GL_FLOAT);

glVertexAttribI2i: //GLuint p_index, GLint p_v0, GLint p_v1
    vertex_attrib(ctx, cmd, 2, GL_INT, false, false, GL_INT);

glVertexAttribI2ui: //GLuint p_index, GLuint p_v0, GLuint p_v1
    vertex_attrib(ctx, cmd, 2, GL_UNSIGNED_INT, false, false, GL_UNSIGNED_INT);

glVertexAttribL2d: //GLuint p_index, GLdouble p_v0, GLdouble p_v1
    vertex_attrib(ctx, cmd, 2, GL_DOUBLE, false, false, GL_DOUBLE);

glVertexAttrib3f: //GLuint p_index, GLfloat p_v0, GLfloat p_v1, GLfloat p_v2
    vertex_attrib(ctx, cmd, 3, GL_FLOAT, false, false, GL_FLOAT);

glVertexAttrib3s: //GLuint p_index, GLshort p_v0, GLshort p_v1, GLshort p_v2
    vertex_attrib(ctx, cmd, 3, GL_INT, false, false, GL_FLOAT);

glVertexAttrib3d: //GLuint p_index, GLdouble p_v0, GLdouble p_v1, GLdouble p_v2
    vertex_attrib(ctx, cmd, 3, GL_DOUBLE, false, false, GL_FLOAT);

glVertexAttribI3i: //GLuint p_index, GLint p_v0, GLint p_v1, GLint p_v2
    vertex_attrib(ctx, cmd, 3, GL_INT, false, false, GL_INT);

glVertexAttribI3ui: //GLuint p_index, GLuint p_v0, GLuint p_v1, GLuint p_v2
    vertex_attrib(ctx, cmd, 3, GL_UNSIGNED_INT, false, false, GL_UNSIGNED_INT);

glVertexAttribL3d: //GLuint p_index, GLdouble p_v0, GLdouble p_v1, GLdouble p_v2
    vertex_attrib(ctx, cmd, 3, GL_DOUBLE, false, false, GL_DOUBLE);

glVertexAttrib4f: //GLuint p_index, GLfloat p_v0, GLfloat p_v1, GLfloat p_v2, GLfloat p_v3
    vertex_attrib(ctx, cmd, 4, GL_FLOAT, false, false, GL_FLOAT);

glVertexAttrib4s: //GLuint p_index, GLshort p_v0, GLshort p_v1, GLshort p_v2, GLshort p_v3
    vertex_attrib(ctx, cmd, 4, GL_INT, false, false, GL_FLOAT);

glVertexAttrib4d: //GLuint p_index, GLdouble p_v0, GLdouble p_v1, GLdouble p_v2, GLdouble p_v3
    vertex_attrib(ctx, cmd, 4, GL_DOUBLE, false, false, GL_FLOAT);

glVertexAttribI4i: //GLuint p_index, GLint p_v0, GLint p_v1, GLint p_v2, GLint p_v3
    vertex_attrib(ctx, cmd, 4, GL_INT, false, false, GL_INT);

glVertexAttribI4ui: //GLuint p_index, GLuint p_v0, GLuint p_v1, GLuint p_v2, GLuint p_v3
    vertex_attrib(ctx, cmd, 4, GL_UNSIGNED_INT, false, false, GL_UNSIGNED_INT);

glVertexAttribL4d: //GLuint p_index, GLdouble p_v0, GLdouble p_v1, GLdouble p_v2, GLdouble p_v3
    vertex_attrib(ctx, cmd, 4, GL_DOUBLE, false, false, GL_DOUBLE);

glVertexAttrib1dv: //GLuint p_index, const GLdouble* p_v
    vertex_attrib(ctx, cmd, 1, GL_DOUBLE, true, false, GL_FLOAT);

glVertexAttrib1fv: //GLuint p_index, const GLfloat* p_v
    vertex_attrib(ctx, cmd, 1, GL_FLOAT, true, false, GL_FLOAT);

glVertexAttrib1sv: //GLuint p_index, const GLshort* p_v
    vertex_attrib(ctx, cmd, 1, GL_SHORT, true, false, GL_FLOAT);

glVertexAttrib2dv: //GLuint p_index, const GLdouble* p_v
    vertex_attrib(ctx, cmd, 2, GL_DOUBLE, true, false, GL_FLOAT);

glVertexAttrib2fv: //GLuint p_index, const GLfloat* p_v
    vertex_attrib(ctx, cmd, 2, GL_FLOAT, true, false, GL_FLOAT);

glVertexAttrib2sv: //GLuint p_index, const GLshort* p_v
    vertex_attrib(ctx, cmd, 2, GL_SHORT, true, false, GL_FLOAT);

glVertexAttrib3dv: //GLuint p_index, const GLdouble* p_v
    vertex_attrib(ctx, cmd, 3, GL_DOUBLE, true, false, GL_FLOAT);

glVertexAttrib3fv: //GLuint p_index, const GLfloat* p_v
    vertex_attrib(ctx, cmd, 3, GL_FLOAT, true, false, GL_FLOAT);

glVertexAttrib3sv: //GLuint p_index, const GLshort* p_v
    vertex_attrib(ctx, cmd, 3, GL_SHORT, true, false, GL_FLOAT);

glVertexAttrib4Nbv: //GLuint p_index, const GLbyte* p_v
    vertex_attrib(ctx, cmd, 4, GL_BYTE, true, true, GL_FLOAT);

glVertexAttrib4Niv: //GLuint p_index, const GLint* p_v
    vertex_attrib(ctx, cmd, 4, GL_INT, true, true, GL_FLOAT);

glVertexAttrib4Nsv: //GLuint p_index, const GLshort* p_v
    vertex_attrib(ctx, cmd, 4, GL_SHORT, true, true, GL_FLOAT);

glVertexAttrib4Nubv: //GLuint p_index, const GLubyte* p_v
    vertex_attrib(ctx, cmd, 4, GL_UNSIGNED_BYTE, true, true, GL_FLOAT);

glVertexAttrib4Nuiv: //GLuint p_index, const GLuint* p_v
    vertex_attrib(ctx, cmd, 4, GL_UNSIGNED_INT, true, true, GL_FLOAT);

glVertexAttrib4Nusv: //GLuint p_index, const GLushort* p_v
    vertex_attrib(ctx, cmd, 4, GL_UNSIGNED_SHORT, true, false, GL_DOUBLE);

glVertexAttrib4bv: //GLuint p_index, const GLbyte* p_v
    vertex_attrib(ctx, cmd, 4, GL_BYTE, true, false, GL_DOUBLE);

glVertexAttrib4dv: //GLuint p_index, const GLdouble* p_v
    vertex_attrib(ctx, cmd, 4, GL_DOUBLE, true, false, GL_DOUBLE);

glVertexAttrib4fv: //GLuint p_index, const GLfloat* p_v
    vertex_attrib(ctx, cmd, 4, GL_FLOAT, true, false, GL_DOUBLE);

glVertexAttrib4iv: //GLuint p_index, const GLint* p_v
    vertex_attrib(ctx, cmd, 4, GL_INT, true, false, GL_DOUBLE);

glVertexAttrib4sv: //GLuint p_index, const GLshort* p_v
    vertex_attrib(ctx, cmd, 4, GL_SHORT, true, false, GL_DOUBLE);

glVertexAttrib4ubv: //GLuint p_index, const GLubyte* p_v
    vertex_attrib(ctx, cmd, 4, GL_UNSIGNED_BYTE, true, false, GL_DOUBLE);

glVertexAttrib4uiv: //GLuint p_index, const GLuint* p_v
    vertex_attrib(ctx, cmd, 4, GL_UNSIGNED_INT, true, false, GL_DOUBLE);

glVertexAttrib4usv: //GLuint p_index, const GLushort* p_v
    vertex_attrib(ctx, cmd, 4, GL_UNSIGNED_SHORT, true, false, GL_DOUBLE);

glVertexAttrib4Nub: //GLuint p_index, GLubyte p_x, GLubyte p_y, GLubyte p_z, GLubyte p_w
    vertex_attrib(ctx, cmd, 4, GL_UNSIGNED_BYTE, true, true, GL_FLOAT);

glVertexAttribI4ubv: //GLuint p_index, const GLubyte* p_v
    vertex_attrib(ctx, cmd, 4, GL_UNSIGNED_SHORT, true, false, GL_UNSIGNED_INT);

glVertexAttribI4usv: //GLuint p_index, const GLushort* p_v
    vertex_attrib(ctx, cmd, 4, GL_UNSIGNED_SHORT, true, false, GL_UNSIGNED_INT);

glVertexAttribI4sv: //GLuint p_index, const GLshort* p_v
    vertex_attrib(ctx, cmd, 4, GL_SHORT, true, false, GL_INT);

glVertexAttribI3iv: //GLuint p_index, const GLint* p_v
    vertex_attrib(ctx, cmd, 3, GL_INT, true, false, GL_INT);

glVertexAttribI4iv: //GLuint p_index, const GLint* p_v
    vertex_attrib(ctx, cmd, 4, GL_INT, true, false, GL_INT);

glVertexAttribI2uiv: //GLuint p_index, const GLuint* p_v
    vertex_attrib(ctx, cmd, 2, GL_UNSIGNED_INT, true, false, GL_UNSIGNED_INT);

glVertexAttribI4uiv: //GLuint p_index, const GLuint* p_v
    vertex_attrib(ctx, cmd, 4, GL_UNSIGNED_INT, true, false, GL_UNSIGNED_INT);

glVertexAttribI4bv: //GLuint p_index, const GLbyte* p_v
    vertex_attrib(ctx, cmd, 4, GL_BYTE, true, false, GL_INT);

glVertexAttribI1iv: //GLuint p_index, const GLint* p_v
    vertex_attrib(ctx, cmd, 1, GL_INT, true, false, GL_INT);

glVertexAttribI2iv: //GLuint p_index, const GLint* p_v
    vertex_attrib(ctx, cmd, 2, GL_INT, true, false, GL_INT);

glVertexAttribI3uiv: //GLuint p_index, const GLuint* p_v
    vertex_attrib(ctx, cmd, 3, GL_UNSIGNED_INT, true, false, GL_UNSIGNED_INT);

glVertexAttribI1uiv: //GLuint p_index, const GLuint* p_v
    vertex_attrib(ctx, cmd, 1, GL_UNSIGNED_INT, true, false, GL_UNSIGNED_INT);

glVertexAttribL1dv: //GLuint p_index, const GLdouble* p_v
    vertex_attrib(ctx, cmd, 1, GL_DOUBLE, true, false, GL_DOUBLE);

glVertexAttribL2dv: //GLuint p_index, const GLdouble* p_v
    vertex_attrib(ctx, cmd, 2, GL_DOUBLE, true, false, GL_DOUBLE);

glVertexAttribL3dv: //GLuint p_index, const GLdouble* p_v
    vertex_attrib(ctx, cmd, 3, GL_DOUBLE, true, false, GL_DOUBLE);

glVertexAttribL4dv: //GLuint p_index, const GLdouble* p_v
    vertex_attrib(ctx, cmd, 4, GL_DOUBLE, true, false, GL_DOUBLE);

glVertexAttribP1ui: //GLuint p_index, GLenum p_type, GLboolean p_normalized, GLuint p_value
    vertex_attrib_packed(ctx, cmd, p_index, p_type, 1, p_normalized, p_value);

glVertexAttribP2ui: //GLuint p_index, GLenum p_type, GLboolean p_normalized, GLuint p_value
    vertex_attrib_packed(ctx, cmd, p_index, p_type, 2, p_normalized, p_value);

glVertexAttribP3ui: //GLuint p_index, GLenum p_type, GLboolean p_normalized, GLuint p_value
    vertex_attrib_packed(ctx, cmd, p_index, p_type, 3, p_normalized, p_value);

glVertexAttribP4ui: //GLuint p_index, GLenum p_type, GLboolean p_normalized, GLuint p_value
    vertex_attrib_packed(ctx, cmd, p_index, p_type, 4, p_normalized, p_value);

glVertexAttribP1uiv: //GLuint p_index, GLenum p_type, GLboolean p_normalized, GLuint* p_value
    vertex_attrib_packed(ctx, cmd, p_index, p_type, 1, p_normalized, *p_value);

glVertexAttribP2uiv: //GLuint p_index, GLenum p_type, GLboolean p_normalized, GLuint* p_value
    vertex_attrib_packed(ctx, cmd, p_index, p_type, 2, p_normalized, *p_value);

glVertexAttribP3uiv: //GLuint p_index, GLenum p_type, GLboolean p_normalized, GLuint* p_value
    vertex_attrib_packed(ctx, cmd, p_index, p_type, 3, p_normalized, *p_value);

glVertexAttribP4uiv: //GLuint p_index, GLenum p_type, GLboolean p_normalized, GLuint* p_value
    vertex_attrib_packed(ctx, cmd, p_index, p_type, 4, p_normalized, *p_value);

glBeginConditionalRender: //GLuint p_id, GLenum p_mode
    uint real_id = get_real_query(ctx->ns, p_id);
    if (!real_id) ERROR("Invalid query name");
    real(real_id, p_mode);

glEndConditionalRender: //
    real();

glDrawArrays: //GLenum p_mode, GLint p_first, GLsizei p_count
    if (p_count<0) ERROR("Invalid count parameters");
    if (!begin_draw(ctx, cmd, p_mode)) RETURN;
    real(p_mode, p_first, p_count);
    end_draw(ctx, cmd);

glDrawArraysIndirect: //GLenum p_mode, const void* p_indirect
    trc_obj_t* buf = trc_gl_state_get_bound_buffer(ctx->trace, GL_DRAW_INDIRECT_BUFFER);
    if (!buf) ERROR("No buffer bound to GL_DRAW_INDIRECT_BUFFER");
    if (((trc_gl_buffer_rev_t*)trc_obj_get_rev(buf, -1))->mapped) ERROR("GL_DRAW_INDIRECT_BUFFER mapped");
    if (!begin_draw(ctx, cmd, p_mode)) RETURN;
    real(p_mode, (const GLvoid*)p_indirect);
    end_draw(ctx, cmd);

glDrawArraysInstanced: //GLenum p_mode, GLint p_first, GLsizei p_count, GLsizei p_instancecount
    if (p_count<0 || p_instancecount<0)
        ERROR("Invalid count or instance count parameters");
    if (!begin_draw(ctx, cmd, p_mode)) RETURN;
    real(p_mode, p_first, p_count, p_instancecount);
    end_draw(ctx, cmd);

glMultiDrawArrays: //GLenum p_mode, const GLint* p_first, const GLsizei* p_count, GLsizei p_drawcount
    if (p_drawcount < 0) ERROR("Invalid draw count parameter");
    for (size_t i = 0; i < p_drawcount; i++) {
        if (p_count[i]<0) ERROR("Invalid count parameter at index %zu", i);
    }
    if (!begin_draw(ctx, cmd, p_mode)) RETURN;
    real(p_mode, p_first, p_count, p_drawcount);
    end_draw(ctx, cmd);

glMultiDrawElements: //GLenum p_mode, const GLsizei* p_count, GLenum p_type, const void*const* p_indices, GLsizei p_drawcount
    if (p_drawcount < 0) ERROR("Invalid draw count parameter");
    for (size_t i = 0; i < p_drawcount; i++) {
        if (p_count[i] < 0) ERROR("Invalid count parameter at index %zu", i);
    }
    if (!begin_draw(ctx, cmd, p_mode)) RETURN;
    real(p_mode, p_count, p_type, p_indices, p_drawcount);
    end_draw(ctx, cmd);

glMultiDrawElementsBaseVertex: //GLenum p_mode, const GLsizei* p_count, GLenum p_type, const void*const* p_indices, GLsizei p_drawcount, const GLint* p_basevertex
    if (p_drawcount < 0) ERROR("Invalid draw count parameter");
    for (size_t i = 0; i < p_drawcount; i++) {
        if (p_count[i] < 0) ERROR("Invalid count parameter at index %zu", i);
    }
    if (!begin_draw(ctx, cmd, p_mode)) RETURN;
    real(p_mode, p_count, p_type, p_indices, p_drawcount, p_basevertex);
    end_draw(ctx, cmd);

glDrawElements: //GLenum p_mode, GLsizei p_count, GLenum p_type, const void* p_indices
    if (p_count < 0) ERROR("Invalid count parameter");
    if (!begin_draw(ctx, cmd, p_mode)) RETURN;
    real(p_mode, p_count, p_type, (const void*)p_indices);
    end_draw(ctx, cmd);

glDrawElementsIndirect: //GLenum p_mode, GLenum p_type, const void* indirect
    trc_obj_t* buf = trc_gl_state_get_bound_buffer(ctx->trace, GL_DRAW_INDIRECT_BUFFER);
    if (!buf) ERROR("No buffer bound to GL_DRAW_INDIRECT_BUFFER");
    if (((trc_gl_buffer_rev_t*)trc_obj_get_rev(buf, -1))->mapped) ERROR("GL_DRAW_INDIRECT_BUFFER mapped");
    if (!begin_draw(ctx, cmd, p_mode)) RETURN;
    real(p_mode, p_type, (const void*)p_indirect);
    end_draw(ctx, cmd);

glDrawElementsBaseVertex: //GLenum p_mode, GLsizei p_count, GLenum p_type, const void* p_indices, GLint p_basevertex
    if (p_count < 0) ERROR("Invalid count parameter");
    if (!begin_draw(ctx, cmd, p_mode)) RETURN;
    real(p_mode, p_count, p_type, (const GLvoid*)p_indices, p_basevertex);
    end_draw(ctx, cmd);

glDrawElementsInstanced: //GLenum p_mode, GLsizei p_count, GLenum p_type, const void* p_indices, GLsizei p_instancecount
    if (p_count<0 || p_instancecount<0) ERROR("Invalid count or instance count parameters");
    if (!begin_draw(ctx, cmd, p_mode)) RETURN;
    real(p_mode, p_count, p_type, (const GLvoid*)p_indices, p_instancecount);
    end_draw(ctx, cmd);

glDrawElementsInstancedBaseVertex: //GLenum p_mode, GLsizei p_count, GLenum p_type, const void* p_indices, GLsizei p_instancecount, GLint p_basevertex
    if (p_count<0 || p_instancecount<0) ERROR("Invalid count or instance count parameters");
    if (!begin_draw(ctx, cmd, p_mode)) RETURN;
    real(p_mode, p_count, p_type, (const GLvoid*)p_indices, p_instancecount, p_basevertex);
    end_draw(ctx, cmd);

glDrawElementsInstancedBaseInstance: //GLenum p_mode, GLsizei p_count, GLenum p_type, const void* p_indices, GLsizei p_instancecount, GLuint p_baseinstance
    if (p_count<0 || p_instancecount<0) ERROR("Invalid count or instance count parameters");
    if (!begin_draw(ctx, cmd, p_mode)) RETURN;
    real(p_mode, p_count, p_type, (const GLvoid*)p_indices, p_instancecount, p_baseinstance);
    end_draw(ctx, cmd);

glDrawElementsInstancedBaseVertexBaseInstance: //GLenum p_mode, GLsizei p_count, GLenum p_type, const void* p_indices, GLsizei p_instancecount, GLint p_basevertex, GLuint p_baseinstance
    if (p_count<0 || p_instancecount<0) ERROR("Invalid count or instance count parameters");
    if (!begin_draw(ctx, cmd, p_mode)) RETURN;
    real(p_mode, p_count, p_type, (const GLvoid*)p_indices, p_instancecount, p_basevertex, p_baseinstance);
    end_draw(ctx, cmd);

glDrawArraysInstancedBaseInstance: //GLenum p_mode, GLint p_first, GLsizei p_count, GLsizei p_instancecount, GLuint p_baseinstance
    if (p_count<0 || p_instancecount<0) ERROR("Invalid count or instance count parameters");
    if (!begin_draw(ctx, cmd, p_mode)) RETURN;
    real(p_mode, p_first, p_count, p_instancecount, p_baseinstance);
    end_draw(ctx, cmd);

//TODO: Check if indices are in-range
glDrawRangeElements: //GLenum p_mode, GLuint p_start, GLuint p_end, GLsizei p_count, GLenum p_type, const void* p_indices
    if (p_count<0 || p_end<p_start) ERROR("Invalid count or range parameters");
    if (!begin_draw(ctx, cmd, p_mode)) RETURN;
    real(p_mode, p_start, p_end, p_count, p_type, (const GLvoid*)p_indices);
    end_draw(ctx, cmd);

//TODO: Check if indices are in-range
glDrawRangeElementsBaseVertex: //GLenum p_mode, GLuint p_start, GLuint p_end, GLsizei p_count, GLenum p_type, const void* p_indices, GLint p_basevertex
    if (p_count<0 || p_end<p_start) ERROR("Invalid count or range parameters");
    if (!begin_draw(ctx, cmd, p_mode)) RETURN;
    real(p_mode, p_start, p_end, p_count, p_type, (const GLvoid*)p_indices, p_basevertex);
    end_draw(ctx, cmd);

glGenVertexArrays: //GLsizei p_n, GLuint* p_arrays
    if (p_n < 0) ERROR("Invalid vertex array object name count");
    GLuint* arrays = replay_alloc(p_n*sizeof(GLuint));
    real(p_n, arrays);
    gen_vertex_arrays(ctx, p_n, arrays, p_arrays, false);

glCreateVertexArrays: //GLsizei p_n, GLuint* p_arrays
    if (p_n < 0) ERROR("Invalid vertex array object name count");
    GLuint* arrays = replay_alloc(p_n*sizeof(GLuint));
    real(p_n, arrays);
    gen_vertex_arrays(ctx, p_n, arrays, p_arrays, true);

glDeleteVertexArrays: //GLsizei p_n, const GLuint* p_arrays
    GLuint* arrays = replay_alloc(p_n*sizeof(GLuint));
    for (size_t i = 0; i < p_n; ++i) {
        trc_obj_t* vao = trc_lookup_name(ctx->ns, TrcVAO, p_arrays[i], -1);
        if (vao && vao==trc_gl_state_get_bound_vao(ctx->trace))
            trc_gl_state_set_bound_vao(ctx->trace, 0);
        if (!(arrays[i]=get_real_vao(ctx->ns, p_arrays[i])) && p_arrays[i])
            trc_add_warning(cmd, "Invalid vertex array name");
        else
            delete_obj(ctx->ns, p_arrays[i], TrcVAO);
    }
    real(p_n, arrays);

glBindVertexArray: //GLuint p_array
    const trc_gl_vao_rev_t* rev = get_vao(ctx->ns, p_array);
    if (!rev && p_array) ERROR("Invalid vertex array name");
    if (rev && !rev->has_object) {
        trc_gl_vao_rev_t newrev = *rev;
        newrev.has_object = true;
        set_vao(&newrev);
    }
    trc_gl_state_set_bound_vao(ctx->trace, rev?rev->head.obj:NULL);

glGetFragDataIndex: //GLuint p_program, const GLchar* p_name
    GLuint real_program = get_real_program(ctx->ns, p_program);
    if (!real_program) ERROR("Invalid program name");

glGetFragDataLocation: //GLuint p_program, const GLchar* p_name
    GLuint real_program = get_real_program(ctx->ns, p_program);
    if (!real_program) ERROR("Invalid program name");

glGetUniformBlockIndex: //GLuint p_program, const GLchar* p_uniformBlockName
    GLuint real_program = get_real_program(ctx->ns, p_program);
    if (!real_program) ERROR("Invalid program name");

glGetUniformIndices: //GLuint p_program, GLsizei p_uniformCount, const GLchar  *const* p_uniformNames, GLuint* p_uniformIndices
    GLuint real_program = get_real_program(ctx->ns, p_program);
    if (!real_program) ERROR("Invalid program name");

glGetUniformfv: //GLuint p_program, GLint p_location, GLfloat* p_params
    validate_get_uniform(ctx, cmd);

glGetUniformiv: //GLuint p_program, GLint p_location, GLint* p_params
    validate_get_uniform(ctx, cmd);

glGetUniformuiv: //GLuint p_program, GLint p_location, GLuint* p_params
    validate_get_uniform(ctx, cmd);

glGetUniformdv: //GLuint p_program, GLint p_location, GLdouble* p_params
    validate_get_uniform(ctx, cmd);

glGetnUniformfv: //GLuint p_program, GLint p_location, GLsizei p_bufSize, GLfloat* p_params
    validate_get_uniform(ctx, cmd);

glGetnUniformiv: //GLuint p_program, GLint p_location, GLsizei p_bufSize, GLint* p_params
    validate_get_uniform(ctx, cmd);

glGetnUniformuiv: //GLuint p_program, GLint p_location, GLsizei p_bufSize, GLuint* p_params
    validate_get_uniform(ctx, cmd);

glGetnUniformdv: //GLuint p_program, GLint p_location, GLsizei p_bufSize, GLdouble* p_params
    validate_get_uniform(ctx, cmd);

glGetMultisamplefv: //GLenum p_pname, GLuint p_index, GLfloat* p_val
    ; //TODO: More validation should be done

glGetInternalformativ: //GLenum p_target, GLenum p_internalformat, GLenum p_pname, GLsizei p_bufSize, GLint* p_params
    GLint* params = replay_alloc(p_bufSize*sizeof(GLint));
    real(p_target, p_internalformat, p_pname, p_bufSize, params);

glGetInternalformati64v: //GLenum p_target, GLenum p_internalformat, GLenum p_pname, GLsizei p_bufSize, GLint64* p_params
    GLint64* params = replay_alloc(p_bufSize*sizeof(GLint64));
    real(p_target, p_internalformat, p_pname, p_bufSize, params);

glGetBufferParameteriv: //GLenum p_target, GLenum p_pname, GLint* p_params
    GLint i;
    real(p_target, p_pname, &i);

glGetBufferPointerv: //GLenum p_target, GLenum p_pname, void ** p_params
    GLvoid* p;
    real(p_target, p_pname, &p);

glGenSamplers: //GLsizei p_count, GLuint* p_samplers
    GLuint* samplers = replay_alloc(p_count*sizeof(GLuint));
    real(p_count, samplers);
    gen_samplers(ctx, p_count, samplers, p_samplers);

glCreateSamplers: //GLsizei p_n, GLuint* p_samplers
    GLuint* samplers = replay_alloc(p_n*sizeof(GLuint));
    real(p_n, samplers);
    gen_samplers(ctx, p_n, samplers, p_samplers);

glDeleteSamplers: //GLsizei p_count, const GLuint* p_samplers
    GLuint* samplers = replay_alloc(p_count*sizeof(GLuint));
    for (size_t i = 0; i < p_count; ++i) {
        if (!(samplers[i] = get_real_sampler(ctx->ns, p_samplers[i])) && p_samplers[i]) {
            trc_add_warning(cmd, "Invalid sampler name");
        } else {
            trc_obj_t* obj = get_sampler(ctx->ns, p_samplers[i])->head.obj;
            for (size_t i = 0; i < trc_gl_state_get_bound_samplers_size(ctx->trace); i++) {
                if (trc_gl_state_get_bound_samplers(ctx->trace, i) == obj)
                    trc_gl_state_set_bound_samplers(ctx->trace, i, NULL);
            }
            
            delete_obj(ctx->ns, p_samplers[i], TrcSampler);
        }
    }
    
    real(p_count, samplers);

glBindSampler: //GLuint p_unit, GLuint p_sampler
    const trc_gl_sampler_rev_t* rev = get_sampler(ctx->ns, p_sampler);
    if (!rev && p_sampler) ERROR("Invalid sampler name");
    real(p_unit, p_sampler?rev->real:0);
    trc_gl_state_set_bound_samplers(ctx->trace, p_unit, rev?rev->head.obj:NULL);

glBindSamplers: //GLuint p_first, GLsizei p_count, const GLuint* p_samplers
    if (p_first+p_count>trc_gl_state_get_state_int(ctx->trace, GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS, 0) || p_count<0)
        ERROR("Invalid range");
    GLuint* real_samplers = replay_alloc(p_count*sizeof(GLuint));
    for (size_t i = 0; i < p_count; i++) {
        const trc_gl_sampler_rev_t* rev = get_sampler(ctx->ns, p_samplers[i]);
        if (!rev) ERROR("Invalid sampler name at index %zu", i);
        real_samplers[i] = rev->real;
        trc_gl_state_set_bound_samplers(ctx->trace, p_first+i, rev?rev->head.obj:NULL);
    }
    real(p_first, p_count, real_samplers);

glGetSynciv: //GLsync p_sync, GLenum p_pname, GLsizei p_bufSize, GLsizei* p_length, GLint* p_values
    if (!get_real_sync(ctx->ns, p_sync)) ERROR("Invalid sync name");
    //TODO: More validation should be done

glSamplerParameterf: //GLuint p_sampler, GLenum p_pname, GLfloat p_param
    if (!p_sampler_rev) ERROR("Invalid sampler name");
    double double_param = p_param;
    trc_gl_sampler_rev_t newrev = *p_sampler_rev;
    if (!sample_param_double(cmd, &newrev.params, p_pname, 1, &double_param)) {
        real(p_sampler_rev->real, p_pname, p_param);
        set_sampler(&newrev);
    }

glSamplerParameteri: //GLuint p_sampler, GLenum p_pname, GLint p_param
    if (!p_sampler_rev) ERROR("Invalid sampler name");
    double double_param = p_param;
    trc_gl_sampler_rev_t newrev = *p_sampler_rev;
    if (!sample_param_double(cmd, &newrev.params, p_pname, 1, &double_param)) {
        real(p_sampler_rev->real, p_pname, p_param);
        set_sampler(&newrev);
    }

glSamplerParameterfv: //GLuint p_sampler, GLenum p_pname, const GLfloat* p_param
    if (!p_sampler_rev) ERROR("Invalid sampler name");
    trc_gl_sampler_rev_t newrev = *p_sampler_rev;
    if (!sample_param_double(cmd, &newrev.params, p_pname, arg_param->count, trc_get_double(arg_param))) {
        real(p_sampler_rev->real, p_pname, p_param);
        set_sampler(&newrev);
    }

glSamplerParameteriv: //GLuint p_sampler, GLenum p_pname, const GLint* p_param
    if (!p_sampler_rev) ERROR("Invalid sampler name");
    double* double_params = replay_alloc(arg_param->count*sizeof(double));
    if (p_pname == GL_TEXTURE_BORDER_COLOR)
        conv_from_signed_norm_array_i32(ctx->trace, arg_param->count, double_params, p_param, 32);
    else
        for (size_t i = 0; i < arg_param->count; i++) double_params[i] = p_param[i];
    trc_gl_sampler_rev_t newrev = *p_sampler_rev;
    if (!sample_param_double(cmd, &newrev.params, p_pname, arg_param->count, double_params)) {
        real(p_sampler_rev->real, p_pname, p_param);
        set_sampler(&newrev);
    }

glSamplerParameterIiv: //GLuint p_sampler, GLenum p_pname, const GLint* p_param
    if (!p_sampler_rev) ERROR("Invalid sampler name");
    double* double_params = replay_alloc(arg_param->count*sizeof(double));
    for (size_t i = 0; i < arg_param->count; i++) double_params[i] = p_param[i];
    trc_gl_sampler_rev_t newrev = *p_sampler_rev;
    if (!sample_param_double(cmd, &newrev.params, p_pname, arg_param->count, double_params)) {
        real(p_sampler_rev->real, p_pname, p_param);
        set_sampler(&newrev);
    }

glSamplerParameterIuiv: //GLuint p_sampler, GLenum p_pname, const GLuint* p_param
    if (!p_sampler_rev) ERROR("Invalid sampler name");
    double* double_params = replay_alloc(arg_param->count*sizeof(double));
    for (size_t i = 0; i < arg_param->count; i++) double_params[i] = p_param[i];
    trc_gl_sampler_rev_t newrev = *p_sampler_rev;
    if (!sample_param_double(cmd, &newrev.params, p_pname, arg_param->count, double_params)) {
        real(p_sampler_rev->real, p_pname, p_param);
        set_sampler(&newrev);
    }

glGenFramebuffers: //GLsizei p_n, GLuint* p_framebuffers
    if (p_n < 0) ERROR("Invalid framebuffer name count");
    GLuint* fbs = replay_alloc(p_n*sizeof(GLuint));
    real(p_n, fbs);
    gen_framebuffers(ctx, p_n, fbs, p_framebuffers, false);

glCreateFramebuffers: //GLsizei p_n, GLuint* p_framebuffers
    if (p_n < 0) ERROR("Invalid framebuffer name count");
    GLuint* fbs = replay_alloc(p_n*sizeof(GLuint));
    real(p_n, fbs);
    gen_framebuffers(ctx, p_n, fbs, p_framebuffers, true);

glDeleteFramebuffers: //GLsizei p_n, const GLuint* p_framebuffers
    GLuint* fbs = replay_alloc(p_n*sizeof(GLuint));
    for (size_t i = 0; i < p_n; ++i) {
        trc_obj_t* fb = trc_lookup_name(ctx->ns, TrcFramebuffer, p_framebuffers[i], -1);
        if (fb && fb==trc_gl_state_get_read_framebuffer(ctx->trace))
            trc_gl_state_set_read_framebuffer(ctx->trace, NULL);
        if (fb && fb==trc_gl_state_get_draw_framebuffer(ctx->trace))
            trc_gl_state_set_draw_framebuffer(ctx->trace, NULL);
        if (!(fbs[i] = get_real_framebuffer(ctx->ns, p_framebuffers[i])) && p_framebuffers[i]) {
            trc_add_warning(cmd, "Invalid framebuffer name");
        } else {
            delete_obj(ctx->ns, p_framebuffers[i], TrcFramebuffer);
        }
    }
    real(p_n, fbs);

glBindFramebuffer: //GLenum p_target, GLuint p_framebuffer
    const trc_gl_framebuffer_rev_t* rev = get_framebuffer(ctx->ns, p_framebuffer);
    if (!rev && p_framebuffer) ERROR("Invalid framebuffer name");
    real(p_target, p_framebuffer?rev->real:0);
    
    if (rev && !rev->has_object) {
        trc_gl_framebuffer_rev_t newrev = *rev;
        newrev.has_object = true;
        set_framebuffer(&newrev);
    }
    
    bool read = true;
    bool draw = true;
    switch (p_target) {
    case GL_READ_FRAMEBUFFER: draw = false; break;
    case GL_DRAW_FRAMEBUFFER: read = false; break;
    }
    
    if (read) trc_gl_state_set_read_framebuffer(ctx->trace, rev?rev->head.obj:NULL);
    if (draw) trc_gl_state_set_draw_framebuffer(ctx->trace, rev?rev->head.obj:NULL);

glGenRenderbuffers: //GLsizei p_n, GLuint* p_renderbuffers
    if (p_n < 0) ERROR("Invalid renderbuffer name count");
    GLuint* rbs = replay_alloc(p_n*sizeof(GLuint));
    real(p_n, rbs);
    gen_renderbuffers(ctx, p_n, rbs, p_renderbuffers, false);

glCreateRenderbuffers: //GLsizei p_n, GLuint* p_renderbuffers
    if (p_n < 0) ERROR("Invalid renderbuffer name count");
    GLuint* rbs = replay_alloc(p_n*sizeof(GLuint));
    real(p_n, rbs);
    gen_renderbuffers(ctx, p_n, rbs, p_renderbuffers, true);

glDeleteRenderbuffers: //GLsizei p_n, const GLuint* p_renderbuffers
    GLuint* rbs = replay_alloc(p_n*sizeof(GLuint));
    for (size_t i = 0; i < p_n; ++i) {
        trc_obj_t* rb = trc_lookup_name(ctx->ns, TrcRenderbuffer, p_renderbuffers[i], -1);
        if (p_renderbuffers[i] && rb==trc_gl_state_get_bound_renderbuffer(ctx->trace))
            trc_gl_state_set_bound_renderbuffer(ctx->trace, NULL);
        //TODO: Detach from bound framebuffers
        //TODO: What to do with renderbuffers attached to non-bound framebuffers?
        if (!(rbs[i] = get_real_renderbuffer(ctx->ns, p_renderbuffers[i])) && p_renderbuffers[i])
            trc_add_warning(cmd, "Invalid renderbuffer name");
        else delete_obj(ctx->ns, p_renderbuffers[i], TrcRenderbuffer);
    }
    
    real(p_n, rbs);

glBindRenderbuffer: //GLenum p_target, GLuint p_renderbuffer
    const trc_gl_renderbuffer_rev_t* rev = get_renderbuffer(ctx->ns, p_renderbuffer);
    if (!rev && p_renderbuffer) ERROR("Invalid renderbuffer name");
    real(p_target, p_renderbuffer?rev->real:0);
    if (rev && !rev->has_object) {
        trc_gl_renderbuffer_rev_t newrev = *rev;
        newrev.has_object = true;
        set_renderbuffer(&newrev);
    }
    trc_gl_state_set_bound_renderbuffer(ctx->trace, rev?rev->head.obj:NULL);

glGetActiveUniformBlockiv: //GLuint p_program, GLuint p_uniformBlockIndex, GLenum p_pname, GLint* p_params
    GLuint program = get_real_program(ctx->ns, p_program);
    if (!program) ERROR("Invalid program ");
    
    if (p_pname == GL_UNIFORM_BLOCK_ACTIVE_UNIFORM_INDICES) {
        GLint count;
        real(program, p_uniformBlockIndex, GL_UNIFORM_BLOCK_ACTIVE_UNIFORMS, &count);
        
        GLint* vals = malloc(sizeof(GLint)*count);
        real(program, p_uniformBlockIndex, p_pname, vals);
        free(vals);
    } else {
        GLint v;
        real(program, p_uniformBlockIndex, p_pname, &v);
    }

glGetActiveUniformBlockName: //GLuint p_program, GLuint p_uniformBlockIndex, GLsizei p_bufSize, GLsizei* p_length, GLchar* p_uniformBlockName
    GLuint program = get_real_program(ctx->ns, p_program);
    if (!program) ERROR("Invalid program name");
    GLchar buf[64];
    real(program, p_uniformBlockIndex, 64, NULL, buf);

glGetActiveUniformName: //GLuint p_program, GLuint p_uniformIndex, GLsizei p_bufSize, GLsizei* p_length, GLchar* p_uniformName
    GLuint program = get_real_program(ctx->ns, p_program);
    if (!program) ERROR("Invalid program name");
    GLchar buf[64];
    real(program, p_uniformIndex, 64, NULL, buf);

glGetActiveUniformsiv: //GLuint p_program, GLsizei p_uniformCount, const GLuint* p_uniformIndices, GLenum p_pname, GLint* p_params
    GLuint program = get_real_program(ctx->ns, p_program);
    if (!program) ERROR("Invalid program name");
    GLint* params = replay_alloc(p_uniformCount*sizeof(GLint));
    real(program, p_uniformCount, p_uniformIndices, p_pname, params);

glGetFramebufferAttachmentParameteriv: //GLenum p_target, GLenum p_attachment, GLenum p_pname, GLint* p_params
    GLint params;
    real(p_target, p_attachment, p_pname, &params);

glGetRenderbufferParameteriv: //GLenum p_target, GLenum p_pname, GLint* p_params
    const trc_gl_renderbuffer_rev_t* rev = trc_obj_get_rev(trc_gl_state_get_bound_renderbuffer(ctx->trace), -1);
    if (!rev) ERROR("No renderbuffer bound");
    if (!rev->has_object) ERROR("Renderbuffer name has no object");

glGetNamedRenderbufferParameteriv: //GLuint p_renderbuffer, GLenum p_pname, GLint* p_params
    if (!p_renderbuffer_rev) ERROR("Invalid renderbuffer name");
    if (!p_renderbuffer_rev->has_object) ERROR("Renderbuffer name has no object");

glFramebufferRenderbuffer: //GLenum p_target, GLenum p_attachment, GLenum p_renderbuffertarget, GLuint p_renderbuffer
    trc_obj_t* fb = get_bound_framebuffer(ctx, p_target);
    if (add_fb_attachment_rb(ctx, cmd, false, fb, p_attachment, p_renderbuffer, p_renderbuffer_rev))
        real(p_target, p_attachment, p_renderbuffertarget, p_renderbuffer?p_renderbuffer_rev->real:0);

glNamedFramebufferRenderbuffer: //GLuint p_framebuffer, GLenum p_attachment, GLenum p_renderbuffertarget, GLuint p_renderbuffer
    trc_obj_t* fb = trc_lookup_name(ctx->ns, TrcFramebuffer, p_framebuffer, -1);
    if (add_fb_attachment_rb(ctx, cmd, true, fb, p_attachment, p_renderbuffer, p_renderbuffer_rev))
        real(p_framebuffer_rev->real, p_attachment, p_renderbuffertarget, p_renderbuffer?p_renderbuffer_rev->real:0);

glFramebufferTexture: //GLenum p_target, GLenum p_attachment, GLuint p_texture, GLint p_level
    trc_obj_t* fb = get_bound_framebuffer(ctx, p_target);
    if (add_fb_attachment(ctx, cmd, fb, p_attachment, false, p_texture, p_texture_rev, p_texture_rev?p_texture_rev->type:0, p_level, 0))
        real(p_target, p_attachment, p_texture?p_texture_rev->real:0, p_level);

glNamedFramebufferTexture: //GLuint p_framebuffer, GLenum p_attachment, GLuint p_texture, GLint p_level
    trc_obj_t* fb = trc_lookup_name(ctx->ns, TrcFramebuffer, p_framebuffer, -1);
    if (add_fb_attachment(ctx, cmd, fb, p_attachment, true, p_texture, p_texture_rev, p_texture_rev?p_texture_rev->type:0, p_level, 0))
        real(p_framebuffer_rev->real, p_attachment, p_texture?p_texture_rev->real:0, p_level);

glFramebufferTextureLayer: //GLenum p_target, GLenum p_attachment GLuint p_texture, GLint p_level, GLint p_layer
    trc_obj_t* fb = get_bound_framebuffer(ctx, p_target);
    if (add_fb_attachment(ctx, cmd, fb, p_attachment, false, p_texture, p_texture_rev, p_texture_rev?p_texture_rev->type:0, p_level, p_layer))
        real(p_target, p_attachment, p_texture?p_texture_rev->real:0, p_level, p_layer);

glNamedFramebufferTextureLayer: //GLuint p_framebuffer, GLenum p_attachment GLuint p_texture, GLint p_level, GLint p_layer
    trc_obj_t* fb = trc_lookup_name(ctx->ns, TrcFramebuffer, p_framebuffer, -1);
    if (add_fb_attachment(ctx, cmd, fb, p_attachment, true, p_texture, p_texture_rev, p_texture_rev?p_texture_rev->type:0, p_level, p_layer))
        real(p_framebuffer_rev->real, p_attachment, p_texture?p_texture_rev->real:0, p_level, p_layer);

glFramebufferTexture1D: //GLenum p_target, GLenum p_attachment, GLenum p_textarget, GLuint p_texture, GLint p_level
    trc_obj_t* fb = get_bound_framebuffer(ctx, p_target);
    if (add_fb_attachment(ctx, cmd, fb, p_attachment, false, p_texture, p_texture_rev, p_textarget, p_level, 0))
        real(p_target, p_attachment, p_textarget, p_texture?p_texture_rev->real:0, p_level);

glFramebufferTexture2D: //GLenum p_target, GLenum p_attachment, GLenum p_textarget, GLuint p_texture, GLint p_level
    trc_obj_t* fb = get_bound_framebuffer(ctx, p_target);
    if (add_fb_attachment(ctx, cmd, fb, p_attachment, false, p_texture, p_texture_rev, p_textarget, p_level, 0))
        real(p_target, p_attachment, p_textarget, p_texture?p_texture_rev->real:0, p_level);

glFramebufferTexture3D: //GLenum p_target, GLenum p_attachment, GLenum p_textarget, GLuint p_texture, GLint p_level, GLint p_zoffset
    trc_obj_t* fb = get_bound_framebuffer(ctx, p_target);
    if (add_fb_attachment(ctx, cmd, fb, p_attachment, false, p_texture, p_texture_rev, p_textarget, p_level, p_zoffset))
        real(p_target, p_attachment, p_textarget, p_texture?p_texture_rev->real:0, p_level, p_zoffset);

glRenderbufferStorage: //GLenum p_target, GLenum p_internalformat, GLsizei p_width, GLsizei p_height
    trc_obj_t* rb = trc_gl_state_get_bound_renderbuffer(ctx->trace);
    const trc_gl_renderbuffer_rev_t* rb_rev = trc_obj_get_rev(rb, -1);
    if (renderbuffer_storage(ctx, cmd, rb_rev, false, p_internalformat, p_width, p_height, 1)) {
        real(p_target, p_internalformat, p_width, p_height);
        replay_update_renderbuffer(ctx, rb_rev, p_width, p_height, p_internalformat, 1);
    }

glNamedRenderbufferStorage: //GLuint p_renderbuffer, GLenum p_internalformat, GLsizei p_width, GLsizei p_height
    if (renderbuffer_storage(ctx, cmd, p_renderbuffer_rev, false, p_internalformat, p_width, p_height, 1)) {
        real(p_renderbuffer_rev->real, p_internalformat, p_width, p_height);
        replay_update_renderbuffer(ctx, p_renderbuffer_rev, p_width, p_height, p_internalformat, 1);
    }

glRenderbufferStorageMultisample: //GLenum p_target, GLsizei p_samples, GLenum p_internalformat, GLsizei p_width, GLsizei p_height
    trc_obj_t* rb = trc_gl_state_get_bound_renderbuffer(ctx->trace);
    const trc_gl_renderbuffer_rev_t* rb_rev = trc_obj_get_rev(rb, -1);
    if (renderbuffer_storage(ctx, cmd, rb_rev, false, p_internalformat, p_width, p_height, p_samples)) {
        real(p_target, p_samples, p_internalformat, p_width, p_height);
        replay_update_renderbuffer(ctx, rb_rev, p_width, p_height, p_internalformat, p_samples);
    }

glNamedRenderbufferStorageMultisample: //GLuint p_renderbuffer, GLsizei p_samples, GLenum p_internalformat, GLsizei p_width, GLsizei p_height
    if (renderbuffer_storage(ctx, cmd, p_renderbuffer_rev, false, p_internalformat, p_width, p_height, p_samples)) {
        real(p_renderbuffer, p_samples, p_internalformat, p_width, p_height);
        replay_update_renderbuffer(ctx, p_renderbuffer_rev, p_width, p_height, p_internalformat, p_samples);
    }

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
    if (p_sync) delete_obj(ctx->ns, p_sync, TrcSync);

glWaitSync: //GLsync p_sync, GLbitfield p_flags, GLuint64 p_timeout
    if (!p_sync_rev) ERROR("Invalid sync name");
    real((GLsync)p_sync_rev->real, p_flags, p_timeout);
    //TODO: grab the object and release it once the wait is over

glClientWaitSync: //GLsync p_sync, GLbitfield p_flags, GLuint64 p_timeout
    if (!p_sync_rev) ERROR("Invalid sync name");
    real((GLsync)p_sync_rev->real, p_flags, p_timeout);

glGenQueries: //GLsizei p_n, GLuint* p_ids
    if (p_n < 0) ERROR("Invalid name query count");
    GLuint* queries = replay_alloc(p_n*sizeof(GLuint));
    real(p_n, queries);
    gen_queries(ctx, p_n, queries, p_ids, false, 0);

glCreateQueries: //GLenum p_target, GLsizei p_n, GLuint* p_ids
    if (p_n < 0) ERROR("Invalid name query count");
    GLuint* queries = replay_alloc(p_n*sizeof(GLuint));
    real(p_target, p_n, queries);
    gen_queries(ctx, p_n, queries, p_ids, false, p_target);

glDeleteQueries: //GLsizei p_n, const GLuint* p_ids
    GLuint* queries = replay_alloc(p_n*sizeof(GLuint));
    for (size_t i = 0; i < p_n; ++i) {
        //TODO: Handle when queries are in use
        if (!(queries[i] = get_real_query(ctx->ns, p_ids[i])) && p_ids[i])
            trc_add_warning(cmd, "Invalid query name");
        else delete_obj(ctx->ns, p_ids[i], TrcQuery);
    }
    real(p_n, queries);

glBeginQuery: //GLenum p_target, GLuint p_id
    if (begin_query(ctx, cmd, p_target, 0, p_id))
        real(p_target, p_id_rev->real);

glEndQuery: //GLenum p_target
    if (end_query(ctx, cmd, p_target, 0)) real(p_target);

glBeginQueryIndexed: //GLenum p_target, GLuint p_index, GLuint p_id
    if (begin_query(ctx, cmd, p_target, p_index, p_id))
        real(p_target, p_index, p_id_rev->real);

glEndQueryIndexed: //GLenum p_target, GLuint p_index
    if (end_query(ctx, cmd, p_target, p_index)) real(p_target, p_index);

glQueryCounter: //GLuint p_id, GLenum p_target
    const trc_gl_query_rev_t* rev = get_query(ctx->ns, p_id);
    if (!rev) ERROR("Invalid query name");
    
    bool bound = false;
    GLenum targets[6] = {
        GL_SAMPLES_PASSED, GL_ANY_SAMPLES_PASSED, GL_ANY_SAMPLES_PASSED_CONSERVATIVE,
        GL_PRIMITIVES_GENERATED, GL_TRANSFORM_FEEDBACK_PRIMITIVES_WRITTEN, GL_TIME_ELAPSED};
    for (size_t i = 0; i < 6; i++) {
        for (size_t j = 0; j < trc_gl_state_get_bound_queries_size(ctx->trace, targets[j]); j++) {
            if (trc_gl_state_get_bound_queries(ctx->trace, targets[j], i) == rev->head.obj)
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
    
    update_query(ctx, cmd, p_target, p_id, rev->real);

glSampleMaski: //GLuint p_maskNumber, GLbitfield p_mask
    if (p_maskNumber >= trc_gl_state_get_state_int(ctx->trace, GL_MAX_SAMPLE_MASK_WORDS, 0))
        ERROR("Invalid mask number");
    real(p_maskNumber, p_mask);
    union {int32_t maski; uint32_t masku;} u;
    u.masku = p_mask;
    trc_gl_state_set_state_int(ctx->trace, GL_SAMPLE_MASK_VALUE, p_maskNumber, u.maski);

glDrawBuffer: //GLenum p_buf
    real(p_buf);
    //TODO: Set state

glDrawBuffers: //GLsizei p_n, const GLenum* p_bufs
    if (p_n < 0) ERROR("buffer count is less than zero");
    if (p_n > trc_gl_state_get_state_int(ctx->trace, GL_MAX_DRAW_BUFFERS, 0))
        ERROR("The buffer count is greater than GL_MAX_DRAW_BUFFERS");
    
    trc_obj_t* fb = trc_gl_state_get_draw_framebuffer(ctx->trace);
    
    uint color_min = GL_COLOR_ATTACHMENT0;
    uint color_max = GL_COLOR_ATTACHMENT0 + trc_gl_state_get_state_int(ctx->trace, GL_MAX_COLOR_ATTACHMENTS, 0);
    for (uint i = 0; i < p_n; i++) {
        if (fb==NULL && not_one_of(p_bufs[i], GL_NONE, GL_FRONT_LEFT, GL_FRONT_RIGHT, GL_BACK_LEFT, GL_BACK_RIGHT, -1))
            ERROR("Invalid buffer");
        else if (fb && (p_bufs[i]<color_min||p_bufs[i]>color_max) && p_bufs[i]!=GL_NONE)
            ERROR("Invalid buffer");
        for (uint j = 0; j < p_n; j++) {
            if (p_bufs[j]==p_bufs[i] && i!=j && p_bufs[i]!=GL_NONE)
                ERROR("Buffer %u appears more than once", j);
        }
        if (p_bufs[i]==GL_BACK && p_n!=1)
            ERROR("GL_BACK can only be a buffer is the buffer count is one");
    }
    //TODO: From reference:
    //    GL_INVALID_OPERATION is generated if any of the entries in bufs (other than GL_NONE)
    //    indicates a color buffer that does not exist in the current GL context.
    
    if (!fb) {
        trc_gl_state_state_enum_init(ctx->trace, GL_DRAW_BUFFER, p_n, p_bufs);
    } else {
        trc_gl_framebuffer_rev_t rev = *(const trc_gl_framebuffer_rev_t*)trc_obj_get_rev(fb, -1);
        rev.draw_buffers = trc_create_data(ctx->trace, p_n*sizeof(GLenum), p_bufs, TRC_DATA_IMMUTABLE);
        set_framebuffer(&rev);
    }
    
    real(p_n, p_bufs);

glReadBuffer: //GLenum p_src
    real(p_src);
    //TODO

glClearBufferiv: //GLenum p_buffer, GLint p_drawbuffer, const GLint* p_value
    //if (not_one_of(p_buffer, GL_COLOR, GL_STENCIL, -1))
    //    ERROR("Buffer is not one of GL_COLOR or GL_STENCIL");
    PARTIAL_VALIDATE_CLEAR_BUFFER;
    real(p_buffer, p_drawbuffer, p_value);
    update_drawbuffer(ctx, p_buffer, p_drawbuffer);

glClearBufferuiv: //GLenum p_buffer, GLint p_drawbuffer, const GLuint* p_value
    //if (p_buffer != GL_COLOR) ERROR("Buffer is not GL_COLOR");
    PARTIAL_VALIDATE_CLEAR_BUFFER;
    real(p_buffer, p_drawbuffer, p_value);
    update_drawbuffer(ctx, p_buffer, p_drawbuffer);

glClearBufferfv: //GLenum p_buffer, GLint p_drawbuffer, const GLfloat* p_value
    //if (not_one_of(p_buffer, GL_COLOR, GL_DEPTH, -1))
    //    ERROR("Buffer is not one of GL_COLOR or GL_DEPTH");
    PARTIAL_VALIDATE_CLEAR_BUFFER;
    real(p_buffer, p_drawbuffer, p_value);
    update_drawbuffer(ctx, p_buffer, p_drawbuffer);

glClearBufferfi: //GLenum p_buffer, GLint p_drawbuffer, GLfloat p_depth, GLint p_stencil
    //if (p_buffer != GL_DEPTH_STENCIL) ERROR("Buffer is not GL_DEPTH_STENCIL");
    PARTIAL_VALIDATE_CLEAR_BUFFER;
    real(p_buffer, p_drawbuffer, p_depth, p_stencil);
    update_drawbuffer(ctx, GL_DEPTH, p_drawbuffer);
    update_drawbuffer(ctx, GL_STENCIL, p_drawbuffer);

glBindFragDataLocation: //GLuint p_program, GLuint p_color, const GLchar* p_name
    GLuint real_program = get_real_program(ctx->ns, p_program);
    if (!real_program) ERROR("Invalid program name");
    real(real_program, p_color, p_name);

glBindFragDataLocationIndexed: //GLuint p_program, GLuint p_colorNumber, GLuint p_index, const GLchar* p_name
    GLuint real_program = get_real_program(ctx->ns, p_program);
    if (!real_program) ERROR("Invalid program name");
    real(real_program, p_colorNumber, p_index, p_name);

glEnable: //GLenum p_cap
    //GL_CLIP_DISTANCEi are the only constants in the form of 0x3xxx accepted by glEnable and glDisable
    if ((p_cap&0x3000)==0x3000 && p_cap-0x3000<trc_gl_state_get_state_int(ctx->trace, GL_MAX_CLIP_DISTANCES, 0))
        trc_gl_state_set_enabled(ctx->trace, GL_CLIP_DISTANCE0, p_cap-0x3000, true);
    else
        trc_gl_state_set_enabled(ctx->trace, p_cap, 0, true);
    real(p_cap);

glDisable: //GLenum p_cap
    if ((p_cap&0x3000)==0x3000 && p_cap-0x3000<trc_gl_state_get_state_int(ctx->trace, GL_MAX_CLIP_DISTANCES, 0))
        trc_gl_state_set_enabled(ctx->trace, GL_CLIP_DISTANCE0, p_cap-0x3000, false);
    else
        trc_gl_state_set_enabled(ctx->trace, p_cap, 0, true);
    real(p_cap);

glEnablei: //GLenum p_target, GLuint p_index
    if ((p_target&0x3000) == 0x3000) {
        if (p_index==0 && p_target-0x3000<trc_gl_state_get_state_int(ctx->trace, GL_MAX_CLIP_DISTANCES, 0))
            trc_gl_state_set_enabled(ctx->trace, GL_CLIP_DISTANCE0, p_target-0x3000, true);
    } else if (p_index < trc_gl_state_get_enabled_size(ctx->trace, p_target)) {
        trc_gl_state_set_enabled(ctx->trace, p_target, p_index, true);
    }
    real(p_target, p_index);

glDisablei: //GLenum p_target, GLuint p_index
    if ((p_target&0x3000) == 0x3000) {
        if (p_index==0 && p_target-0x3000<trc_gl_state_get_state_int(ctx->trace, GL_MAX_CLIP_DISTANCES, 0))
            trc_gl_state_set_enabled(ctx->trace, GL_CLIP_DISTANCE0, p_target-0x3000, false);
    } else if (p_index < trc_gl_state_get_enabled_size(ctx->trace, p_target)) {
        trc_gl_state_set_enabled(ctx->trace, p_target, p_index, false);
    }
    real(p_target, p_index);

glDepthMask: //GLboolean p_flag
    trc_gl_state_set_state_bool(ctx->trace, GL_DEPTH_WRITEMASK, 0, p_flag);
    real(p_flag);

glColorMask: //GLboolean p_red, GLboolean p_green, GLboolean p_blue, GLboolean p_alpha
    for (size_t i = 0; i < trc_gl_state_get_state_int(ctx->trace, GL_MAX_DRAW_BUFFERS, 0); i+=4) {
        trc_gl_state_set_state_bool(ctx->trace, GL_COLOR_WRITEMASK, i+0, p_red);
        trc_gl_state_set_state_bool(ctx->trace, GL_COLOR_WRITEMASK, i+1, p_green);
        trc_gl_state_set_state_bool(ctx->trace, GL_COLOR_WRITEMASK, i+2, p_blue);
        trc_gl_state_set_state_bool(ctx->trace, GL_COLOR_WRITEMASK, i+3, p_alpha);
    }
    real(p_red, p_green, p_blue, p_alpha);

glColorMaski: //GLuint p_index, GLboolean p_r, GLboolean p_g, GLboolean p_b, GLboolean p_a
    if (p_index < trc_gl_state_get_state_int(ctx->trace, GL_MAX_DRAW_BUFFERS, 0)) {
        trc_gl_state_set_state_bool(ctx->trace, GL_COLOR_WRITEMASK, p_index*4+0, p_r);
        trc_gl_state_set_state_bool(ctx->trace, GL_COLOR_WRITEMASK, p_index*4+1, p_g);
        trc_gl_state_set_state_bool(ctx->trace, GL_COLOR_WRITEMASK, p_index*4+2, p_b);
        trc_gl_state_set_state_bool(ctx->trace, GL_COLOR_WRITEMASK, p_index*4+3, p_a);
    }
    real(p_index, p_r, p_g, p_b, p_a);

glStencilMask: //GLuint mask
    union {int32_t maski; uint32_t masku;} u;
    u.masku = p_mask;
    trc_gl_state_set_state_int(ctx->trace, GL_STENCIL_WRITEMASK, 0, u.maski);
    trc_gl_state_set_state_int(ctx->trace, GL_STENCIL_BACK_WRITEMASK, 0, u.maski);
    real(p_mask);

glStencilMaskSeparate: //GLenum face, GLuint mask
    union {int32_t maski; uint32_t masku;} u;
    u.masku = p_mask;
    if (p_face==GL_FRONT || p_face==GL_FRONT_AND_BACK)
        trc_gl_state_set_state_int(ctx->trace, GL_STENCIL_WRITEMASK, 0, u.maski);
    if (p_face==GL_BACK || p_face==GL_FRONT_AND_BACK)
        trc_gl_state_set_state_int(ctx->trace, GL_STENCIL_BACK_WRITEMASK, 0, u.maski);
    real(p_face, p_mask);

glPixelStoref: //GLenum p_pname, GLfloat p_param
    replay_pixel_store(ctx, cmd, p_pname, p_param);

glPixelStorei: //GLenum p_pname, GLint p_param
    replay_pixel_store(ctx, cmd, p_pname, p_param);

glClearStencil: //GLint p_s
    //TODO: Mask p_s?
    trc_gl_state_set_state_int(ctx->trace, GL_STENCIL_CLEAR_VALUE, 0, p_s);
    real(p_s);

glClearColor: //GLfloat p_red, GLfloat p_green, GLfloat p_blue, GLfloat p_alpha
    trc_gl_state_set_state_float(ctx->trace, GL_COLOR_CLEAR_VALUE, 0, p_red);
    trc_gl_state_set_state_float(ctx->trace, GL_COLOR_CLEAR_VALUE, 1, p_green);
    trc_gl_state_set_state_float(ctx->trace, GL_COLOR_CLEAR_VALUE, 2, p_blue);
    trc_gl_state_set_state_float(ctx->trace, GL_COLOR_CLEAR_VALUE, 3, p_alpha);
    real(p_red, p_green, p_blue, p_alpha);

glClearDepth: //GLdouble p_depth
    trc_gl_state_set_state_float(ctx->trace, GL_DEPTH_CLEAR_VALUE, 0, fmin(fmax(p_depth, 0.0f), 1.0f));
    real(p_depth);

glClearDepthf: //GLfloat p_d
    trc_gl_state_set_state_float(ctx->trace, GL_DEPTH_CLEAR_VALUE, 0, fminf(fmaxf(p_d, 0.0f), 1.0f));
    real(p_d);

glDepthRange: //GLdouble p_near, GLdouble p_far
    for (int i = 0; i < trc_gl_state_get_state_int(ctx->trace, GL_MAX_VIEWPORTS, 0); i++) {
        trc_gl_state_set_state_float(ctx->trace, GL_DEPTH_RANGE, i*2+0, p_near);
        trc_gl_state_set_state_float(ctx->trace, GL_DEPTH_RANGE, i*2+1, p_far);
    }
    real(p_near, p_far);

glDepthRangef: //GLfloat p_n, GLfloat p_f
    for (int i = 0; i < trc_gl_state_get_state_int(ctx->trace, GL_MAX_VIEWPORTS, 0); i++) {
        trc_gl_state_set_state_float(ctx->trace, GL_DEPTH_RANGE, i*2+0, p_n);
        trc_gl_state_set_state_float(ctx->trace, GL_DEPTH_RANGE, i*2+1, p_f);
    }
    real(p_n, p_f);

glDepthRangeArrayv: //GLuint p_first, GLsizei p_count, const GLdouble* p_v
    int max_viewports = trc_gl_state_get_state_int(ctx->trace, GL_MAX_VIEWPORTS, 0);
    if (p_count<0 || p_first+p_count>max_viewports) ERROR("Invalid first and count");
    for (GLuint i = p_first*2; i < (p_first+p_count)*2; i++)
        trc_gl_state_set_state_float(ctx->trace, GL_DEPTH_RANGE, i, p_v[i]);
    real(p_first, p_count, p_v);

glPointSize: //GLfloat p_size
    if (p_size <= 0.0f) ERROR("size is not greater than zero");
    trc_gl_state_set_state_float(ctx->trace, GL_POINT_SIZE, 0, p_size);
    real(p_size);

glLineWidth: //GLfloat p_width
    if (p_width <= 0.0f) ERROR("width is not greater than zero");
    trc_gl_state_set_state_float(ctx->trace, GL_LINE_WIDTH, 0, p_width);
    real(p_width);

glPolygonOffset: //GLfloat p_factor, GLfloat p_units
    trc_gl_state_set_state_float(ctx->trace, GL_POLYGON_OFFSET_UNITS, 0, p_units);
    trc_gl_state_set_state_float(ctx->trace, GL_POLYGON_OFFSET_FACTOR, 0, p_factor);
    real(p_factor, p_units);

glPolygonOffsetClamp: //GLfloat p_factor, GLfloat p_units, GLfloat p_clamp
    trc_gl_state_set_state_float(ctx->trace, GL_POLYGON_OFFSET_UNITS, 0, p_units);
    trc_gl_state_set_state_float(ctx->trace, GL_POLYGON_OFFSET_FACTOR, 0, p_factor);
    trc_gl_state_set_state_float(ctx->trace, GL_POLYGON_OFFSET_CLAMP, 0, p_clamp);
    real(p_factor, p_units, p_clamp);

glSampleCoverage: //GLfloat p_value, GLboolean p_invert
    trc_gl_state_set_state_float(ctx->trace, GL_SAMPLE_COVERAGE_VALUE, 0, p_value);
    trc_gl_state_set_state_bool(ctx->trace, GL_SAMPLE_COVERAGE_INVERT, 0, p_invert);
    real(p_value, p_invert);

glStencilFunc: //GLenum p_func, GLint p_ref, GLuint p_mask
    union {int32_t maski; uint32_t masku;} u;
    u.masku = p_mask;
    trc_gl_state_set_state_enum(ctx->trace, GL_STENCIL_FUNC, 0, p_func);
    trc_gl_state_set_state_int(ctx->trace, GL_STENCIL_REF, 0, p_ref);
    trc_gl_state_set_state_int(ctx->trace, GL_STENCIL_VALUE_MASK, 0, u.maski);
    trc_gl_state_set_state_enum(ctx->trace, GL_STENCIL_BACK_FUNC, 0, p_func);
    trc_gl_state_set_state_int(ctx->trace, GL_STENCIL_BACK_REF, 0, p_ref);
    trc_gl_state_set_state_int(ctx->trace, GL_STENCIL_BACK_VALUE_MASK, 0, u.maski);
    real(p_func, p_ref, p_mask);

glStencilFuncSeparate: //GLenum p_face, GLenum p_func, GLint p_ref, GLuint p_mask
    union {int32_t maski; uint32_t masku;} u;
    u.masku = p_mask;
    if (p_face==GL_FRONT || p_face==GL_FRONT_AND_BACK) {
        trc_gl_state_set_state_enum(ctx->trace, GL_STENCIL_FUNC, 0, p_func);
        trc_gl_state_set_state_int(ctx->trace, GL_STENCIL_REF, 0, p_ref);
        trc_gl_state_set_state_int(ctx->trace, GL_STENCIL_VALUE_MASK, 0, u.maski);
    }
    if (p_face==GL_BACK || p_face==GL_FRONT_AND_BACK) {
        trc_gl_state_set_state_enum(ctx->trace, GL_STENCIL_BACK_FUNC, 0, p_func);
        trc_gl_state_set_state_int(ctx->trace, GL_STENCIL_BACK_REF, 0, p_ref);
        trc_gl_state_set_state_int(ctx->trace, GL_STENCIL_BACK_VALUE_MASK, 0, u.maski);
    }
    real(p_face, p_func, p_ref, p_mask);

glStencilOp: //GLenum p_fail, GLenum p_zfail, GLenum p_zpass
    trc_gl_state_set_state_enum(ctx->trace, GL_STENCIL_FAIL, 0, p_fail);
    trc_gl_state_set_state_enum(ctx->trace, GL_STENCIL_PASS_DEPTH_FAIL, 0, p_zfail);
    trc_gl_state_set_state_enum(ctx->trace, GL_STENCIL_PASS_DEPTH_PASS, 0, p_zpass);
    trc_gl_state_set_state_enum(ctx->trace, GL_STENCIL_BACK_FAIL, 0, p_fail);
    trc_gl_state_set_state_enum(ctx->trace, GL_STENCIL_BACK_PASS_DEPTH_FAIL, 0, p_zfail);
    trc_gl_state_set_state_enum(ctx->trace, GL_STENCIL_BACK_PASS_DEPTH_PASS, 0, p_zpass);
    real(p_fail, p_zfail, p_zpass);

glStencilOpSeparate: //GLenum p_face, GLenum p_sfail, GLenum p_dpfail, GLenum p_dppass
    if (p_face==GL_FRONT || p_face==GL_FRONT_AND_BACK) {
        trc_gl_state_set_state_enum(ctx->trace, GL_STENCIL_FAIL, 0, p_sfail);
        trc_gl_state_set_state_enum(ctx->trace, GL_STENCIL_PASS_DEPTH_FAIL, 0, p_dpfail);
        trc_gl_state_set_state_enum(ctx->trace, GL_STENCIL_PASS_DEPTH_PASS, 0, p_dppass);
    }
    if (p_face==GL_BACK || p_face==GL_FRONT_AND_BACK) {
        trc_gl_state_set_state_enum(ctx->trace, GL_STENCIL_BACK_FAIL, 0, p_sfail);
        trc_gl_state_set_state_enum(ctx->trace, GL_STENCIL_BACK_PASS_DEPTH_FAIL, 0, p_dpfail);
        trc_gl_state_set_state_enum(ctx->trace, GL_STENCIL_BACK_PASS_DEPTH_PASS, 0, p_dppass);
    }
    real(p_face, p_sfail, p_dpfail, p_dppass);

glBlendFunc: //GLenum p_sfactor, GLenum p_dfactor
    for (size_t i = 0; i < trc_gl_state_get_state_int(ctx->trace, GL_MAX_DRAW_BUFFERS, 0); i++) {
        trc_gl_state_set_state_enum(ctx->trace, GL_BLEND_SRC_RGB, i, p_sfactor);
        trc_gl_state_set_state_enum(ctx->trace, GL_BLEND_DST_RGB, i, p_dfactor);
        trc_gl_state_set_state_enum(ctx->trace, GL_BLEND_SRC_ALPHA, i, p_sfactor);
        trc_gl_state_set_state_enum(ctx->trace, GL_BLEND_DST_ALPHA, i, p_dfactor);
    }
    real(p_sfactor, p_dfactor);

glBlendFuncSeparate: //GLenum p_sfactorRGB, GLenum p_dfactorRGB, GLenum p_sfactorAlpha, GLenum p_dfactorAlpha
    for (size_t i = 0; i < trc_gl_state_get_state_int(ctx->trace, GL_MAX_DRAW_BUFFERS, 0); i++) {
        trc_gl_state_set_state_enum(ctx->trace, GL_BLEND_SRC_RGB, i, p_sfactorRGB);
        trc_gl_state_set_state_enum(ctx->trace, GL_BLEND_DST_RGB, i, p_dfactorRGB);
        trc_gl_state_set_state_enum(ctx->trace, GL_BLEND_SRC_ALPHA, i, p_sfactorAlpha);
        trc_gl_state_set_state_enum(ctx->trace, GL_BLEND_DST_ALPHA, i, p_dfactorAlpha);
    }
    real(p_sfactorRGB, p_dfactorRGB, p_sfactorAlpha, p_dfactorAlpha);

glBlendFunci: //GLuint p_buf, GLenum p_src, GLenum p_dst
    if (p_buf < trc_gl_state_get_state_int(ctx->trace, GL_MAX_DRAW_BUFFERS, 0)) {
        trc_gl_state_set_state_enum(ctx->trace, GL_BLEND_SRC_RGB, p_buf, p_src);
        trc_gl_state_set_state_enum(ctx->trace, GL_BLEND_DST_RGB, p_buf, p_dst);
        trc_gl_state_set_state_enum(ctx->trace, GL_BLEND_SRC_ALPHA, p_buf, p_src);
        trc_gl_state_set_state_enum(ctx->trace, GL_BLEND_DST_ALPHA, p_buf, p_dst);
    }
    real(p_buf, p_src, p_dst);

glBlendFuncSeparatei: //GLuint p_buf, GLenum p_srcRGB, GLenum p_dstRGB, GLenum p_srcAlpha, GLenum p_dstAlpha
    if (p_buf < trc_gl_state_get_state_int(ctx->trace, GL_MAX_DRAW_BUFFERS, 0)) {
        trc_gl_state_set_state_enum(ctx->trace, GL_BLEND_SRC_RGB, p_buf, p_srcRGB);
        trc_gl_state_set_state_enum(ctx->trace, GL_BLEND_DST_RGB, p_buf, p_dstRGB);
        trc_gl_state_set_state_enum(ctx->trace, GL_BLEND_SRC_ALPHA, p_buf, p_srcAlpha);
        trc_gl_state_set_state_enum(ctx->trace, GL_BLEND_DST_ALPHA, p_buf, p_dstAlpha);
    }
    real(p_buf, p_srcRGB, p_dstRGB, p_srcAlpha, p_dstAlpha);

glBlendColor: //GLfloat p_red, GLfloat p_green, GLfloat p_blue, GLfloat p_alpha
    trc_gl_state_set_state_float(ctx->trace, GL_BLEND_COLOR, 0, p_red);
    trc_gl_state_set_state_float(ctx->trace, GL_BLEND_COLOR, 1, p_green);
    trc_gl_state_set_state_float(ctx->trace, GL_BLEND_COLOR, 2, p_blue);
    trc_gl_state_set_state_float(ctx->trace, GL_BLEND_COLOR, 3, p_alpha);
    real(p_red, p_green, p_blue, p_alpha);

glBlendEquation: //GLenum p_mode
    for (size_t i = 0; i < trc_gl_state_get_state_int(ctx->trace, GL_MAX_DRAW_BUFFERS, 0); i++) {
        trc_gl_state_set_state_enum(ctx->trace, GL_BLEND_EQUATION_RGB, i, p_mode);
        trc_gl_state_set_state_enum(ctx->trace, GL_BLEND_EQUATION_ALPHA, i, p_mode);
    }
    real(p_mode);

glBlendEquationSeparate: //GLenum p_modeRGB, GLenum p_modeAlpha
    for (size_t i = 0; i < trc_gl_state_get_state_int(ctx->trace, GL_MAX_DRAW_BUFFERS, 0); i++) {
        trc_gl_state_set_state_enum(ctx->trace, GL_BLEND_EQUATION_RGB, i, p_modeRGB);
        trc_gl_state_set_state_enum(ctx->trace, GL_BLEND_EQUATION_ALPHA, i, p_modeAlpha);
    }
    real(p_modeRGB, p_modeAlpha);

glBlendEquationi: //GLuint p_buf, GLenum p_mode
    if (p_buf < trc_gl_state_get_state_int(ctx->trace, GL_MAX_DRAW_BUFFERS, 0)) {
        trc_gl_state_set_state_enum(ctx->trace, GL_BLEND_EQUATION_RGB, p_buf, p_mode);
        trc_gl_state_set_state_enum(ctx->trace, GL_BLEND_EQUATION_ALPHA, p_buf, p_mode);
    }
    real(p_buf, p_mode);

glBlendEquationSeparatei: //GLuint p_buf, GLenum p_modeRGB, GLenum p_modeAlpha
    if (p_buf < trc_gl_state_get_state_int(ctx->trace, GL_MAX_DRAW_BUFFERS, 0)) {
        trc_gl_state_set_state_enum(ctx->trace, GL_BLEND_EQUATION_RGB, p_buf, p_modeRGB);
        trc_gl_state_set_state_enum(ctx->trace, GL_BLEND_EQUATION_ALPHA, p_buf, p_modeAlpha);
    }
    real(p_buf, p_modeRGB, p_modeAlpha);

glViewport: //GLint p_x, GLint p_y, GLsizei p_width, GLsizei p_height
    trc_gl_state_set_state_float(ctx->trace, GL_VIEWPORT, 0, p_x);
    trc_gl_state_set_state_float(ctx->trace, GL_VIEWPORT, 1, p_y);
    trc_gl_state_set_state_float(ctx->trace, GL_VIEWPORT, 2, p_width);
    trc_gl_state_set_state_float(ctx->trace, GL_VIEWPORT, 3, p_height);
    real(p_x, p_y, p_width, p_height);

glViewportIndexedf: //GLuint p_index, GLfloat p_x, GLfloat p_y, GLfloat p_w, GLfloat p_h
    trc_gl_state_set_state_float(ctx->trace, GL_VIEWPORT, p_index*4+0, p_x);
    trc_gl_state_set_state_float(ctx->trace, GL_VIEWPORT, p_index*4+1, p_y);
    trc_gl_state_set_state_float(ctx->trace, GL_VIEWPORT, p_index*4+2, p_w);
    trc_gl_state_set_state_float(ctx->trace, GL_VIEWPORT, p_index*4+3, p_h);
    real(p_index, p_x, p_y, p_w, p_h);

glViewportArrayv: //GLuint p_first, GLsizei p_count, const GLfloat* p_v
    for (size_t i = p_first*4; i < (p_first+p_count)*4; i++)
        trc_gl_state_set_state_float(ctx->trace, GL_VIEWPORT, i, p_v[i]);
    real(p_first, p_count, p_v);

glViewportIndexedfv: //GLuint p_index, const GLfloat* p_v
    for (size_t i = 0; i < 4; i++)
        trc_gl_state_set_state_float(ctx->trace, GL_VIEWPORT, p_index*4+i, p_v[i]);
    real(p_index, p_v);

glScissor: //GLint p_x, GLint p_y, GLsizei p_width, GLsizei p_height
    trc_gl_state_set_state_int(ctx->trace, GL_SCISSOR_BOX, 0, p_x);
    trc_gl_state_set_state_int(ctx->trace, GL_SCISSOR_BOX, 1, p_y);
    trc_gl_state_set_state_int(ctx->trace, GL_SCISSOR_BOX, 2, p_width);
    trc_gl_state_set_state_int(ctx->trace, GL_SCISSOR_BOX, 3, p_height);
    real(p_x, p_y, p_width, p_height);

glScissorIndexed: //GLuint p_index, GLint p_left, GLint p_bottom, GLsizei p_width, GLsizei p_height
    trc_gl_state_set_state_int(ctx->trace, GL_SCISSOR_BOX, p_index*4+0, p_left);
    trc_gl_state_set_state_int(ctx->trace, GL_SCISSOR_BOX, p_index*4+1, p_bottom);
    trc_gl_state_set_state_int(ctx->trace, GL_SCISSOR_BOX, p_index*4+2, p_width);
    trc_gl_state_set_state_int(ctx->trace, GL_SCISSOR_BOX, p_index*4+3, p_height);
    real(p_index, p_left, p_bottom, p_width, p_height);

glScissorArrayv: //GLuint p_first, GLsizei p_count, const GLint* p_v
    for (size_t i = p_first*4; i < (p_first+p_count)*4; i++)
        trc_gl_state_set_state_float(ctx->trace, GL_SCISSOR_BOX, i, p_v[i]);
    real(p_first, p_count, p_v);

glScissorIndexedv: //GLuint p_index, const GLint* p_v
    for (size_t i = 0; i < 4; i++)
        trc_gl_state_set_state_int(ctx->trace, GL_SCISSOR_BOX, p_index*4+i, p_v[i]);
    real(p_index, p_v);

glHint: //GLenum p_target, GLenum p_mode
    trc_gl_state_set_hints(ctx->trace, p_target, p_mode);
    real(p_target, p_mode);

glProvokingVertex: //GLenum p_mode
    trc_gl_state_set_state_enum(ctx->trace, GL_PROVOKING_VERTEX, 0, p_mode);
    real(p_mode);

glLogicOp: //GLenum p_opcode
    trc_gl_state_set_state_enum(ctx->trace, GL_LOGIC_OP_MODE, 0, p_opcode);
    real(p_opcode);

glPrimitiveRestartIndex: //GLuint p_index
    trc_gl_state_set_state_int(ctx->trace, GL_PRIMITIVE_RESTART_INDEX, 0, p_index);
    real(p_index);

glPolygonMode: //GLenum p_face, GLenum p_mode
    trc_gl_state_set_state_enum(ctx->trace, GL_POLYGON_MODE, 0, p_mode);
    real(p_face, p_mode);

glCullFace: //GLenum p_mode
    trc_gl_state_set_state_enum(ctx->trace, GL_CULL_FACE_MODE, 0, p_mode);
    real(p_mode);

glFrontFace: //GLenum p_mode
    trc_gl_state_set_state_enum(ctx->trace, GL_FRONT_FACE, 0, p_mode);
    real(p_mode);

glDepthFunc: //GLenum p_func
    trc_gl_state_set_state_enum(ctx->trace, GL_DEPTH_FUNC, 0, p_func);
    real(p_func);

glClampColor: //GLenum p_target, GLenum p_clamp
    real(p_target, p_clamp);

glPointParameterf: //GLenum p_pname, GLfloat p_param
    switch (p_pname) {
    case GL_POINT_FADE_THRESHOLD_SIZE: trc_gl_state_set_state_float(ctx->trace, p_pname, 0, p_param);
    case GL_POINT_SPRITE_COORD_ORIGIN: trc_gl_state_set_state_enum(ctx->trace, p_pname, 0, p_param);
    }
    F(glPointParameterf)(p_pname, p_param);

glPointParameteri: //GLenum p_pname, GLint p_param
    switch (p_pname) {
    case GL_POINT_FADE_THRESHOLD_SIZE: trc_gl_state_set_state_float(ctx->trace, p_pname, 0, p_param);
    case GL_POINT_SPRITE_COORD_ORIGIN: trc_gl_state_set_state_enum(ctx->trace, p_pname, 0, p_param);
    }
    F(glPointParameteri)(p_pname, p_param);

glPointParameterfv: //GLenum p_pname, const GLfloat* p_params
    switch (p_pname) {
    case GL_POINT_FADE_THRESHOLD_SIZE: trc_gl_state_set_state_float(ctx->trace, p_pname, 0, p_params[0]);
    case GL_POINT_SPRITE_COORD_ORIGIN: trc_gl_state_set_state_enum(ctx->trace, p_pname, 0, p_params[0]);
    }
    real(p_pname, p_params);

glPointParameteriv: //GLenum p_pname, const GLint* p_params
    switch (p_pname) {
    case GL_POINT_FADE_THRESHOLD_SIZE: trc_gl_state_set_state_float(ctx->trace, p_pname, 0, p_params[0]);
    case GL_POINT_SPRITE_COORD_ORIGIN: trc_gl_state_set_state_enum(ctx->trace, p_pname, 0, p_params[0]);
    }
    real(p_pname, p_params);

glClipControl: //GLenum p_origin, GLenum p_depth
    trc_gl_state_set_state_enum(ctx->trace, GL_CLIP_ORIGIN, 0, p_origin);
    trc_gl_state_set_state_enum(ctx->trace, GL_CLIP_DEPTH_MODE, 0, p_depth);

glMinSampleShading: //GLfloat p_value
    trc_gl_state_set_state_float(ctx->trace, GL_MIN_SAMPLE_SHADING_VALUE, 0, p_value);
    real(p_value);

glDebugMessageCallback: //GLDEBUGPROC p_callback, const void* p_userParam
    ;

glPatchParameteri: //GLenum p_pname, GLint p_value
    if (p_value<=0 || p_value>trc_gl_state_get_state_int(ctx->trace, GL_MAX_PATCH_VERTICES, 0))
        ERROR("Invalid value");
    real(p_pname, p_value);

glPatchParameterfv: //GLenum p_pname, const GLfloat* p_values
    real(p_pname, p_values);

glGetUniformSubroutineuiv: //GLenum p_shadertype, GLint p_location, GLuint* p_params
    ;

glGetSubroutineIndex: //GLuint p_program, GLenum p_shadertype, const GLchar* p_name
    ;

glGetActiveSubroutineName: //GLuint p_program, GLenum p_shadertype, GLuint p_index, GLsizei p_bufsize, GLsizei* p_length, GLchar* p_name
    ;

glGetProgramStageiv: //GLuint p_program, GLenum p_shadertype, GLenum p_pname, GLint* p_values
    ;

glGetActiveSubroutineUniformName: //GLuint p_program, GLenum p_shadertype, GLuint p_index, GLsizei p_bufsize, GLsizei* p_length, GLchar* p_name
    ;

glGetSubroutineUniformLocation: //GLuint p_program, GLenum p_shadertype, const GLchar* p_name
    ;

glGetQueryIndexediv: //GLenum p_target, GLuint p_index, GLenum p_pname, GLint* p_params
    ;

//TODO: More validation
#define VALIDATE_BLIT_FRAMEBUFFER\
    if (((p_mask&GL_DEPTH_BUFFER_BIT)||(p_mask&GL_STENCIL_BUFFER_BIT)) && p_filter!=GL_NEAREST)\
        ERROR("Invalid filter for mask");

glBlitFramebuffer: //GLint p_srcX0, GLint p_srcY0, GLint p_srcX1, GLint p_srcY1, GLint p_dstX0, GLint p_dstY0, GLint p_dstX1, GLint p_dstY1, GLbitfield p_mask, GLenum p_filter
    real(p_srcX0, p_srcY0, p_srcX1, p_srcY1, p_dstX0, p_dstY0, p_dstX1, p_dstY1, p_mask, p_filter);
    VALIDATE_BLIT_FRAMEBUFFER
    update_buffers(ctx, trc_gl_state_get_draw_framebuffer(ctx->trace), p_mask);

glBlitNamedFramebuffer: //GLuint p_readFramebuffer, GLuint p_drawFramebuffer, GLint p_srcX0, GLint p_srcY0, GLint p_srcX1, GLint p_srcY1, GLint p_dstX0, GLint p_dstY0, GLint p_dstX1, GLint p_dstY1, GLbitfield p_mask, GLenum p_filter
    if (!p_readFramebuffer_rev || !p_drawFramebuffer_rev) ERROR("Invalid framebuffer name");
    if (!p_readFramebuffer_rev->has_object || !p_drawFramebuffer_rev->has_object)
        ERROR("Framebuffer name has no object");
    VALIDATE_BLIT_FRAMEBUFFER
    real(p_readFramebuffer_rev->real, p_drawFramebuffer_rev->real, p_srcX0, p_srcY0, p_srcX1, p_srcY1, p_dstX0, p_dstY0, p_dstX1, p_dstY1, p_mask, p_filter);
    update_buffers(ctx, p_drawFramebuffer_rev->head.obj, p_mask);

glGenTransformFeedbacks: //GLsizei p_n, GLuint* p_ids
    if (p_n < 0) ERROR("Invalid transform feedback name count");
    GLuint* tf = replay_alloc(p_n*sizeof(GLuint));
    real(p_n, tf);
    gen_transform_feedbacks(ctx, p_n, tf, p_ids, false);

glCreateTransformFeedbacks: //GLsizei p_n, GLuint* p_ids
    if (p_n < 0) ERROR("Invalid transform feedback name count");
    GLuint* tf = replay_alloc(p_n*sizeof(GLuint));
    real(p_n, tf);
    gen_transform_feedbacks(ctx, p_n, tf, p_ids, true);

glBindTransformFeedback: //GLenum p_target, GLuint p_id
    if (get_current_tf(ctx)->active_not_paused)
        ERROR("The current transform feedback object is active and not paused");
    
    const trc_gl_transform_feedback_rev_t* rev = get_transform_feedback(ctx->ns, p_id);
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
    
    trc_gl_state_set_current_tf(ctx->trace, rev->head.obj);

glDeleteTransformFeedbacks: //GLsizei p_n, const GLuint* p_ids
    GLuint* tf = replay_alloc(p_n*sizeof(GLuint));
    memset(tf, 0, p_n*sizeof(GLuint));
    for (size_t i = 0; i < p_n; ++i) {
        if (!p_ids[i]) continue;
        
        if (!(tf[i] = get_real_transform_feedback(ctx->ns, p_ids[i]))) {
            trc_add_error(cmd, "Invalid transform feedback object name");
        } else {
            const trc_gl_transform_feedback_rev_t* tf = get_transform_feedback(ctx->ns, p_ids[i]);
            if (tf->active) {
                trc_add_error(cmd, "Transform feedback object at index %zu is active", i);
                continue;
            }
            if (trc_gl_state_get_current_tf(ctx->trace) == tf->head.obj)
                trc_gl_state_set_current_tf(ctx->trace, NULL);
            delete_obj(ctx->ns, p_ids[i], TrcTransformFeedback);
        }
    }
    real(p_n, tf);

static void bind_tf_buffer(trc_replay_context_t* ctx, trace_command_t* cmd,
                           GLuint tf, GLuint index, GLuint buffer,
                           bool ranged, GLintptr offset, GLsizeiptr size) {
    const trc_gl_transform_feedback_rev_t* tf_rev = get_transform_feedback(ctx->ns, tf);
    if (!tf_rev) ERROR2(, "Invalid transform feedback name");
    if (tf_rev->active) ERROR2(, "The specified transform feedback object is active");
    size_t count = tf_rev->bindings->size / sizeof(trc_gl_buffer_binding_point_t);
    if (index >= count) ERROR2(, "Invalid index");
    
    const trc_gl_buffer_rev_t* buf_rev = get_buffer(ctx->ns, buffer);
    if (!buf_rev && buffer) ERROR2(, "Invalid buffer name");
    if (ranged && buf_rev && (size<=0 || offset+size>buf_rev->data->size))
        ERROR2(, "Invalid range");
    //TODO: Check alignment of offset?
    
    if (!ranged) F(glTransformFeedbackBufferBase)(tf_rev->real, index, buf_rev?buf_rev->real:0);
    else F(glTransformFeedbackBufferRange)(tf_rev->real, index, buf_rev?buf_rev->real:0, offset, size);
    
    trc_gl_transform_feedback_rev_t newrev = *tf_rev;
    newrev.bindings = trc_copy_data(ctx->trace, newrev.bindings, 0);
    trc_gl_buffer_binding_point_t* bindings = trc_map_data(newrev.bindings, TRC_MAP_MODIFY);
    trc_set_obj_ref(&bindings[index].buf, buf_rev?buf_rev->head.obj:NULL);
    bindings[index].offset = ranged ? offset : 0;
    bindings[index].size = ranged ? size : 0;
    trc_unmap_data(newrev.bindings);
    set_transform_feedback(&newrev);
}

glTransformFeedbackBufferBase: //GLuint p_xfb, GLuint p_index, GLuint p_buffer
    bind_tf_buffer(ctx, cmd, p_xfb, p_index, p_buffer, false, 0, 0);

glTransformFeedbackBufferRange: //GLuint p_xfb, GLuint p_index, GLuint p_buffer, GLintptr p_offset, GLsizeiptr p_size
    bind_tf_buffer(ctx, cmd, p_xfb, p_index, p_buffer, true, p_offset, p_size);

glBeginTransformFeedback: //GLenum p_primitiveMode
    trc_gl_transform_feedback_rev_t rev = *get_current_tf(ctx);
    if (rev.active) ERROR("Transform feedback is already active");
    //TODO: Check that needed binding points have buffers
    //TODO: Check to see if a program or pipeline object is bound and that it has varying variables
    real(p_primitiveMode);
    rev.active = true;
    rev.active_not_paused = true;
    set_transform_feedback(&rev);
    trc_gl_state_set_tf_primitive(ctx->trace, p_primitiveMode);
    on_activate_tf(ctx, cmd);

glEndTransformFeedback: //
    trc_gl_transform_feedback_rev_t rev = *get_current_tf(ctx);
    if (!rev.active) ERROR("Transform feedback is not active");
    real();
    rev.active = false;
    rev.paused = false;
    rev.active_not_paused = false;
    set_transform_feedback(&rev);

glPauseTransformFeedback: //
    trc_gl_transform_feedback_rev_t rev = *get_current_tf(ctx);
    if (!rev.active) ERROR("Transform feedback is not active");
    if (rev.paused) ERROR("Transform feedback is already paused");
    real();
    rev.paused = true;
    rev.active_not_paused = false;
    set_transform_feedback(&rev);

glResumeTransformFeedback: //
    trc_gl_transform_feedback_rev_t rev = *get_current_tf(ctx);
    if (!rev.active) ERROR("Transform feedback is not active");
    if (!rev.paused) ERROR("Transform feedback is not paused");
    real();
    rev.paused = false;
    rev.active_not_paused = true;
    set_transform_feedback(&rev);
    on_activate_tf(ctx, cmd);

glUniformSubroutinesuiv: //GLenum p_shadertype, GLsizei p_count, const GLuint* p_indices
    trc_obj_t* program = get_active_program_for_stage(ctx, p_shadertype);
    if (!program) ERROR("No program is current for stage");
    GLint reqCount, subroutineCount;
    const trc_gl_program_rev_t* program_rev = trc_obj_get_rev(program, -1);
    F(glGetProgramStageiv)(program_rev->real, p_shadertype, GL_ACTIVE_SUBROUTINE_UNIFORM_LOCATIONS, &reqCount);
    F(glGetProgramStageiv)(program_rev->real, p_shadertype, GL_ACTIVE_SUBROUTINES, &subroutineCount);
    if (p_count != reqCount) ERROR("Count must be GL_ACTIVE_SUBROUTINE_UNIFORM_LOCATIONS");
    for (GLsizei i = 0; i < p_count; i++) {
        if (p_indices[i] >= subroutineCount)
            ERROR("Index at %d is not a valid subroutine\n", i);
    }
    real(p_shadertype, p_count, p_indices);
    //TODO: Store the indices
