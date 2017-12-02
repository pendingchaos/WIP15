#include "libtrace/replay/textures.h"
#include "libtrace/replay/utils.h"

#define REPLAY_CONFIG_FUNCS
#define RC_F F
#include "shared/replay_config.h"
#undef RC_F
#undef REPLAY_CONFIG_FUNCS

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

static void create_context_buffers(trc_gl_context_rev_t* rev) {
    size_t size = rev->drawable_width * rev->drawable_height * 4;
    rev->front_color_buffer = trc_create_chunked_data(ctx->trace, size, NULL);
    rev->back_color_buffer = rev->front_color_buffer;
    rev->back_depth_buffer = rev->back_color_buffer;
    rev->back_stencil_buffer = rev->back_depth_buffer;
}

static void update_fb0_buffers(bool backcolor, bool frontcolor, bool depth, bool stencil);

static void init_context() {
    trace_t* trace = ctx->trace;
    
    trc_replay_config_t cfg = trc_get_context(trace)->trace_cfg;
    
    int w, h;
    SDL_GL_GetDrawableSize(ctx->window, &w, &h);
    trc_gl_state_set_drawable_width(trace, w);
    trc_gl_state_set_drawable_height(trace, h);
    trc_gl_state_bound_buffer_init(trace, GL_ARRAY_BUFFER, (trc_obj_ref_t){NULL});
    trc_gl_state_bound_buffer_init(trace, GL_ELEMENT_ARRAY_BUFFER, (trc_obj_ref_t){NULL});
    trc_gl_state_bound_buffer_init(trace, GL_ATOMIC_COUNTER_BUFFER, (trc_obj_ref_t){NULL});
    trc_gl_state_bound_buffer_init(trace, GL_COPY_READ_BUFFER, (trc_obj_ref_t){NULL});
    trc_gl_state_bound_buffer_init(trace, GL_COPY_WRITE_BUFFER, (trc_obj_ref_t){NULL});
    trc_gl_state_bound_buffer_init(trace, GL_DISPATCH_INDIRECT_BUFFER, (trc_obj_ref_t){NULL});
    trc_gl_state_bound_buffer_init(trace, GL_DRAW_INDIRECT_BUFFER, (trc_obj_ref_t){NULL});
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
    
    trc_gl_state_bound_queries_init(trace, GL_SAMPLES_PASSED, 1, NULL);
    trc_gl_state_bound_queries_init(trace, GL_ANY_SAMPLES_PASSED, 1, NULL);
    trc_gl_state_bound_queries_init(trace, GL_ANY_SAMPLES_PASSED_CONSERVATIVE, 1, NULL);
    trc_gl_state_bound_queries_init(trace, GL_PRIMITIVES_GENERATED, cfg.max_vertex_streams, NULL);
    trc_gl_state_bound_queries_init(trace, GL_TRANSFORM_FEEDBACK_PRIMITIVES_WRITTEN, cfg.max_vertex_streams, NULL);
    trc_gl_state_bound_queries_init(trace, GL_TIME_ELAPSED, 1, NULL);
    
    trc_gl_state_state_int_init1(trace, GL_MAX_CLIP_DISTANCES, cfg.max_clip_distances);
    trc_gl_state_state_int_init1(trace, GL_MAX_DRAW_BUFFERS, cfg.max_draw_buffers);
    trc_gl_state_state_int_init1(trace, GL_MAX_VIEWPORTS, cfg.max_viewports);
    trc_gl_state_state_int_init1(trace, GL_MAX_VERTEX_ATTRIBS, cfg.max_vertex_attribs);
    trc_gl_state_state_int_init1(trace, GL_MAX_COLOR_ATTACHMENTS, cfg.max_color_attachments);
    trc_gl_state_state_int_init1(trace, GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS, cfg.max_combined_texture_units);
    trc_gl_state_state_int_init1(trace, GL_MAX_TRANSFORM_FEEDBACK_BUFFERS, cfg.max_xfb_buffers);
    trc_gl_state_state_int_init1(trace, GL_MAX_UNIFORM_BUFFER_BINDINGS, cfg.max_ubo_bindings);
    trc_gl_state_state_int_init1(trace, GL_MAX_PATCH_VERTICES, cfg.max_patch_vertices);
    trc_gl_state_state_int_init1(trace, GL_MAX_RENDERBUFFER_SIZE, cfg.max_renderbuffer_size);
    trc_gl_state_state_int_init1(trace, GL_MAX_TEXTURE_SIZE, cfg.max_texture_size);
    trc_gl_state_state_int_init1(trace, GL_MAX_ATOMIC_COUNTER_BUFFER_BINDINGS, cfg.max_atomic_counter_buffer_bindings);
    trc_gl_state_state_int_init1(trace, GL_MAX_SHADER_STORAGE_BUFFER_BINDINGS, cfg.max_ssbo_bindings);
    trc_gl_state_state_int_init1(trace, GL_MAX_SAMPLE_MASK_WORDS, cfg.max_sample_mask_words);
    trc_gl_state_state_int_init1(trace, GL_MAJOR_VERSION, cfg.version/100);
    trc_gl_state_state_int_init1(trace, GL_MINOR_VERSION, cfg.version%100/10);
    trc_gl_state_set_ver(trace, cfg.version);
    
    trc_gl_state_bound_buffer_indexed_init(trace, GL_UNIFORM_BUFFER, cfg.max_ubo_bindings, NULL);
    trc_gl_state_bound_buffer_indexed_init(trace, GL_ATOMIC_COUNTER_BUFFER, cfg.max_atomic_counter_buffer_bindings, NULL);
    trc_gl_state_bound_buffer_indexed_init(trace, GL_SHADER_STORAGE_BUFFER, cfg.max_ssbo_bindings, NULL);
    
    trc_gl_context_rev_t rev = *trc_get_context(trace);
    
    trc_gl_transform_feedback_rev_t default_tf;
    default_tf.has_object = true;
    default_tf.real = 0;
    size_t size = cfg.max_xfb_buffers * sizeof(trc_gl_buffer_binding_point_t);
    default_tf.bindings = trc_create_data(trace, size, NULL, 0);
    default_tf.active = false;
    default_tf.paused = false;
    default_tf.active_not_paused = false;
    trc_obj_t* default_tf_obj = trc_create_named_obj(rev.priv_ns, TrcTransformFeedback, 0, &default_tf);
    
    rev.bound_buffer_indexed_GL_TRANSFORM_FEEDBACK_BUFFER = default_tf.bindings;
    trc_set_context(trace, &rev);
    
    trc_gl_state_set_tf_primitive(trace, 0);
    
    trc_gl_state_default_tf_init(trace, (trc_obj_ref_t){default_tf_obj});
    trc_grab_obj(default_tf_obj); //trc_gl_state_default_tf_init does not increase the reference count
    trc_gl_state_current_tf_init(trace, (trc_obj_ref_t){default_tf_obj});
    trc_grab_obj(default_tf_obj); //trc_gl_state_current_tf_init does not increase the reference count
    
    trc_gl_state_bound_textures_init(trace, GL_TEXTURE_1D, cfg.max_combined_texture_units, NULL);
    trc_gl_state_bound_textures_init(trace, GL_TEXTURE_2D, cfg.max_combined_texture_units, NULL);
    trc_gl_state_bound_textures_init(trace, GL_TEXTURE_3D, cfg.max_combined_texture_units, NULL);
    trc_gl_state_bound_textures_init(trace, GL_TEXTURE_1D_ARRAY, cfg.max_combined_texture_units, NULL);
    trc_gl_state_bound_textures_init(trace, GL_TEXTURE_2D_ARRAY, cfg.max_combined_texture_units, NULL);
    trc_gl_state_bound_textures_init(trace, GL_TEXTURE_RECTANGLE, cfg.max_combined_texture_units, NULL);
    trc_gl_state_bound_textures_init(trace, GL_TEXTURE_CUBE_MAP, cfg.max_combined_texture_units, NULL);
    trc_gl_state_bound_textures_init(trace, GL_TEXTURE_CUBE_MAP_ARRAY, cfg.max_combined_texture_units, NULL);
    trc_gl_state_bound_textures_init(trace, GL_TEXTURE_BUFFER, cfg.max_combined_texture_units, NULL);
    trc_gl_state_bound_textures_init(trace, GL_TEXTURE_2D_MULTISAMPLE, cfg.max_combined_texture_units, NULL);
    trc_gl_state_bound_textures_init(trace, GL_TEXTURE_2D_MULTISAMPLE_ARRAY, cfg.max_combined_texture_units, NULL);
    
    trc_gl_state_bound_samplers_init(trace, cfg.max_combined_texture_units, NULL);
    
    trc_gl_state_enabled_init(trace, GL_BLEND, cfg.max_draw_buffers, NULL);
    trc_gl_state_enabled_init(trace, GL_CLIP_DISTANCE0, cfg.max_clip_distances, NULL);
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
    trc_gl_state_enabled_init(trace, GL_SCISSOR_TEST, cfg.max_viewports, NULL);
    trc_gl_state_enabled_init1(trace, GL_STENCIL_TEST, false);
    trc_gl_state_enabled_init1(trace, GL_TEXTURE_CUBE_MAP_SEAMLESS, false);
    trc_gl_state_enabled_init1(trace, GL_PROGRAM_POINT_SIZE, false);
    
    trc_gl_state_state_bool_init1(trace, GL_DEPTH_WRITEMASK, GL_TRUE);
    bool color_mask[cfg.max_draw_buffers*4];
    for (size_t i = 0; i < cfg.max_draw_buffers*4; i++) color_mask[i] = GL_TRUE;
    trc_gl_state_state_bool_init(trace, GL_COLOR_WRITEMASK, cfg.max_draw_buffers*4, color_mask);
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
    
    GLenum blenddata[cfg.max_draw_buffers];
    for (size_t i = 0; i < cfg.max_draw_buffers; i++) blenddata[i] = GL_ONE;
    trc_gl_state_state_enum_init(trace, GL_BLEND_SRC_RGB, cfg.max_draw_buffers, blenddata);
    trc_gl_state_state_enum_init(trace, GL_BLEND_SRC_ALPHA, cfg.max_draw_buffers, blenddata);
    
    for (size_t i = 0; i < cfg.max_draw_buffers; i++) blenddata[i] = GL_ZERO;
    trc_gl_state_state_enum_init(trace, GL_BLEND_DST_RGB, cfg.max_draw_buffers, blenddata);
    trc_gl_state_state_enum_init(trace, GL_BLEND_DST_ALPHA, cfg.max_draw_buffers, blenddata);
    
    float blend_color[4] = {0.0f, 0.0f, 0.0f, 0.0f};
    trc_gl_state_state_float_init(trace, GL_BLEND_COLOR, 4, blend_color);
    
    for (size_t i = 0; i < cfg.max_draw_buffers; i++) blenddata[i] = GL_FUNC_ADD;
    trc_gl_state_state_enum_init(trace, GL_BLEND_EQUATION_RGB, cfg.max_draw_buffers, blenddata);
    trc_gl_state_state_enum_init(trace, GL_BLEND_EQUATION_ALPHA, cfg.max_draw_buffers, blenddata);
    
    trc_gl_state_set_hints(trace, GL_FRAGMENT_SHADER_DERIVATIVE_HINT, GL_DONT_CARE);
    trc_gl_state_set_hints(trace, GL_LINE_SMOOTH_HINT, GL_DONT_CARE);
    trc_gl_state_set_hints(trace, GL_POLYGON_SMOOTH_HINT, GL_DONT_CARE);
    trc_gl_state_set_hints(trace, GL_TEXTURE_COMPRESSION_HINT, GL_DONT_CARE);
    
    float zerof[cfg.max_viewports*4];
    for (size_t i = 0; i < cfg.max_viewports*4; i++) zerof[i] = 0.0f;
    trc_gl_state_state_float_init(trace, GL_VIEWPORT, cfg.max_viewports*4, zerof);
    trc_gl_state_state_int_init(trace, GL_SCISSOR_BOX, cfg.max_viewports*4, NULL);
    float depth_range[cfg.max_viewports*2];
    for (int i = 0; i < cfg.max_viewports*2; i++) depth_range[i] = (float[]){0.0f, 1.0f}[i%2];
    trc_gl_state_state_float_init(trace, GL_DEPTH_RANGE, cfg.max_viewports*2, depth_range);
    
    trc_gl_state_state_enum_init1(trace, GL_PROVOKING_VERTEX, GL_LAST_VERTEX_CONVENTION);
    trc_gl_state_state_enum_init1(trace, GL_LOGIC_OP_MODE, GL_COPY);
    trc_gl_state_state_int_init1(trace, GL_PRIMITIVE_RESTART_INDEX, 0);
    trc_gl_state_state_enum_init1(trace, GL_POLYGON_MODE, GL_FILL);
    trc_gl_state_state_enum_init1(trace, GL_CULL_FACE_MODE, GL_BACK);
    trc_gl_state_state_enum_init1(trace, GL_FRONT_FACE, GL_CCW);
    trc_gl_state_state_enum_init1(trace, GL_DEPTH_FUNC, GL_LESS);
    trc_gl_state_state_enum_init1(trace, GL_LOGIC_OP_MODE, GL_COPY);
    trc_gl_state_state_enum_init1(trace, GL_CLIP_ORIGIN, GL_LOWER_LEFT);
    trc_gl_state_state_enum_init1(trace, GL_CLIP_DEPTH_MODE, GL_NEGATIVE_ONE_TO_ONE);
    trc_gl_state_state_float_init1(trace, GL_POINT_FADE_THRESHOLD_SIZE, GL_UPPER_LEFT);
    trc_gl_state_state_enum_init1(trace, GL_POINT_SPRITE_COORD_ORIGIN, GL_UPPER_LEFT);
    trc_gl_state_state_float_init1(trace, GL_MIN_SAMPLE_SHADING_VALUE, 0.0f);
    
    trc_gl_state_state_int_init1(trace, GL_PATCH_VERTICES, 3);
    float one4[4] = {1.0f, 1.0f, 1.0f, 1.0f};
    trc_gl_state_state_float_init(trace, GL_PATCH_DEFAULT_OUTER_LEVEL, 4, one4);
    trc_gl_state_state_float_init(trace, GL_PATCH_DEFAULT_INNER_LEVEL, 2, one4);
    
    double* va = malloc(cfg.max_vertex_attribs*4*sizeof(double));
    for (size_t i = 0; i < cfg.max_vertex_attribs*4; i++) va[i] = i%4==3 ? 1 : 0;
    trc_gl_state_state_double_init(trace, GL_CURRENT_VERTEX_ATTRIB, cfg.max_vertex_attribs*4, va);
    free(va);
    
    GLenum draw_buffers[1] = {GL_BACK};
    trc_gl_state_state_enum_init(trace, GL_DRAW_BUFFER, 1, draw_buffers);
    
    int sample_mask_value[cfg.max_sample_mask_words];
    memset(sample_mask_value, 0xff, cfg.max_sample_mask_words*sizeof(int));
    trc_gl_state_state_int_init(trace, GL_SAMPLE_MASK_VALUE, cfg.max_sample_mask_words, sample_mask_value);
    
    uint draw_vao;
    F(glGenVertexArrays)(1, &draw_vao);
    F(glBindVertexArray)(draw_vao);
    trc_gl_state_set_draw_vao(trace, draw_vao);
    
    rev = *trc_get_context(trace);
    create_context_buffers(&rev);
    trc_set_context(ctx->trace, &rev);
    update_fb0_buffers(true, true, true, true);
    
    if (cfg.version >= 430) {
        F(glEnable)(GL_DEBUG_OUTPUT);
        F(glEnable)(GL_DEBUG_OUTPUT_SYNCHRONOUS);
        F(glDebugMessageCallback)(replay_debug_callback, ctx);
        F(glDebugMessageControl)(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, 0, GL_TRUE);
    }
}

static void pixel_store(GLenum pname, GLint param) {
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

static bool append_fb_attachment(bool dsa, trc_obj_t* fb, const trc_gl_framebuffer_attachment_t* attach) {
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
    trc_unmap_data(attachs);
    
    trc_gl_framebuffer_rev_t newrev = *rev;
    if (!replaced) newattachs[attach_count++] = *attach;
    
    size_t size = attach_count * sizeof(trc_gl_framebuffer_attachment_t);
    newrev.attachments = trc_create_data_no_copy(ctx->trace, size, newattachs, 0);
    
    set_framebuffer(&newrev);
    
    return true;
}

static bool add_fb_attachment(trc_obj_t* fb, uint attachment, bool dsa, uint fake_tex,
                              const trc_gl_texture_rev_t* tex, uint target, uint level, uint layer) {
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
        trc_unmap_data(images);
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
    return append_fb_attachment(dsa, fb, &attach);
}

static bool add_fb_attachment_rb(bool dsa, trc_obj_t* fb, uint attachment, uint fake_rb,
                                 const trc_gl_renderbuffer_rev_t* rb) {
    if (!rb && fake_rb) ERROR2(false, "Invalid renderbuffer name");
    if (fake_rb && !rb->has_object) ERROR2(false, "Renderbuffer name has no object");
    trc_gl_framebuffer_attachment_t attach;
    memset(&attach, 0, sizeof(attach));
    attach.has_renderbuffer = true;
    attach.attachment = attachment;
    trc_set_obj_ref(&attach.texture, NULL);
    trc_set_obj_ref(&attach.renderbuffer, rb->head.obj);
    return append_fb_attachment(dsa, fb, &attach);
}

static void update_renderbuffer(const trc_gl_renderbuffer_rev_t* rev, uint width,
                                uint height, uint internal_format, uint samples) {
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

static trc_obj_t* get_bound_framebuffer(GLenum target) {
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

static void update_query(GLenum target, GLuint fake_id, GLuint id) {
    if (!id) return;
    
    GLuint64 res = 0;
    if (target!=GL_TIME_ELAPSED && target!=GL_TIMESTAMP) { //TODO: Why is this branch here?
        F(glFinish)();
        while (!res) F(glGetQueryObjectui64v)(id, GL_QUERY_RESULT_AVAILABLE, &res);
        F(glGetQueryObjectui64v)(id, GL_QUERY_RESULT, &res);
    }
    
    trc_gl_query_rev_t query = *get_query(fake_id);
    query.result = res;
    set_query(&query);
}

static void begin_get_fb0_data(GLint prev[11]) {
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

static void end_get_fb0_data(const GLint prev[11]) {
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

static void get_fb0_buffer(trc_chunked_data_t* data, trc_gl_context_rev_t* state, GLenum buffer, GLenum format, GLenum type) {
    F(glReadBuffer)(buffer);
    
    size_t data_size = state->drawable_width * state->drawable_height * 4;
    void* buf = malloc(data_size);
    F(glReadPixels)(0, 0, state->drawable_width, state->drawable_height, format, type, buf);
    
    if (data_size != data->size) {
        *data = trc_create_chunked_data(ctx->trace, data_size, buf);
    } else {
        trc_chunked_data_mod_t mod = {.next=NULL, .start=0, .size=data_size, .data=buf};
        trc_modify_chunked_data_t minfo = {.base=*data, .mods=&mod};
        *data = trc_modify_chunked_data(ctx->trace, minfo);
    }
    
    free(buf);
}

static void store_and_bind_fb(GLint* prev, GLuint fb) {
    F(glGetIntegerv)(GL_DRAW_FRAMEBUFFER_BINDING, prev);
    F(glBindFramebuffer)(GL_DRAW_FRAMEBUFFER, fb);
}

static void update_fb0_buffers(bool backcolor, bool frontcolor, bool depth, bool stencil) {
    F(glFinish)();
    
    GLint prevfb;
    store_and_bind_fb(&prevfb, 0);
    GLint depth_size, stencil_size;
    F(glGetFramebufferAttachmentParameteriv)(GL_DRAW_FRAMEBUFFER, GL_DEPTH,
                                             GL_FRAMEBUFFER_ATTACHMENT_DEPTH_SIZE, &depth_size);
    F(glGetFramebufferAttachmentParameteriv)(GL_DRAW_FRAMEBUFFER, GL_STENCIL,
                                             GL_FRAMEBUFFER_ATTACHMENT_STENCIL_SIZE, &stencil_size);
    if (depth_size == 0) depth = false;
    if (stencil_size == 0) stencil = false;
    F(glBindFramebuffer)(GL_DRAW_FRAMEBUFFER, prevfb);
    
    GLint prev[11];
    begin_get_fb0_data(prev);
    trc_gl_context_rev_t state = *trc_get_context(ctx->trace);
    if (backcolor)
        get_fb0_buffer(&state.back_color_buffer, &state, GL_BACK, GL_RGBA, GL_UNSIGNED_BYTE);
    if (frontcolor)
        get_fb0_buffer(&state.front_color_buffer, &state, GL_FRONT, GL_RGBA, GL_UNSIGNED_BYTE);
    if (depth)
        get_fb0_buffer(&state.back_depth_buffer, &state, GL_BACK, GL_DEPTH_COMPONENT, GL_FLOAT);
    if (stencil)
        get_fb0_buffer(&state.back_stencil_buffer, &state, GL_BACK, GL_STENCIL_INDEX, GL_UNSIGNED_INT);
    trc_set_context(ctx->trace, &state);
    end_get_fb0_data(prev);
}

static trc_obj_t** get_tf_buffer_list(size_t* count) {
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
    if (!vao) ERROR2(false, "No VAO bound");
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
    
    for (size_t i = 0; i < vao->attribs->size/sizeof(trc_gl_vao_attrib_t); i++) {
        trc_gl_vao_attrib_t* a = &vao_attribs[i];
        if (!a->enabled) continue;
        const trc_gl_buffer_rev_t* buf = trc_obj_get_rev(a->buffer.obj, -1);
        if (buf->mapped) {
            trc_unmap_data(vao_attribs);
            ERROR2(false, "Buffer for vertex attribute %zu is mapped", i);
        }
    }
    
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
        
        F(glEnableVertexAttribArray)(real_loc);
        
        F(glBindBuffer)(GL_ARRAY_BUFFER, ((trc_gl_buffer_rev_t*)trc_obj_get_rev(a->buffer.obj, -1))->real);
        if (a->integer)
            F(glVertexAttribIPointer)(real_loc, a->size, a->type, a->stride, (const void*)(uintptr_t)a->offset);
        else
            F(glVertexAttribPointer)(real_loc, a->size, a->type, a->normalized, a->stride, (const void*)(uintptr_t)a->offset);
        
        if (trc_gl_state_get_ver(ctx->trace) > 330)
            F(glVertexAttribDivisor)(real_loc, a->divisor);
    }
    trc_unmap_data(vao_attribs);
    trc_unmap_data(prog_vertex_attribs);
    
    F(glBindBuffer)(GL_ARRAY_BUFFER, last_buf);
    
    return true;
}

#define PARTIAL_VALIDATE_CLEAR_BUFFER do {\
int max_draw_buffers = trc_gl_state_get_state_int(ctx->trace, GL_MAX_DRAW_BUFFERS, 0);\
if (p_drawbuffer<0 || (p_drawbuffer==0&&p_buffer!=GL_COLOR) || p_drawbuffer>=max_draw_buffers)\
    ERROR("Invalid buffer");\
} while (0)

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

static void gen_framebuffers(size_t count, const GLuint* real, const GLuint* fake, bool create) {
    trc_gl_framebuffer_rev_t rev;
    rev.has_object = false;
    trc_data_t* empty_data = trc_create_data(ctx->trace, 0, NULL, 0);
    rev.attachments = empty_data;
    rev.draw_buffers = empty_data;
    for (size_t i = 0; i < count; ++i) {
        rev.real = real[i];
        trc_create_named_obj(ctx->priv_ns, TrcFramebuffer, fake[i], &rev);
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

static void gen_renderbuffers(size_t count, const GLuint* real, const GLuint* fake, bool create) {
    trc_gl_renderbuffer_rev_t rev;
    rev.has_object = create;
    rev.has_storage = false;
    for (size_t i = 0; i < count; ++i) {
        rev.real = real[i];
        trc_create_named_obj(ctx->ns, TrcRenderbuffer, fake[i], &rev);
    }
}

static void gen_transform_feedbacks(size_t count, const GLuint* real, const GLuint* fake, bool create) {
    trc_gl_transform_feedback_rev_t rev;
    rev.has_object = create;
    size_t size = trc_gl_state_get_state_int(ctx->trace, GL_MAX_TRANSFORM_FEEDBACK_BUFFERS, 0);
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

static bool renderbuffer_storage(const trc_gl_renderbuffer_rev_t* rb, bool dsa, GLenum internal_format,
                                 GLsizei width, GLsizei height, GLsizei samples) {
    if (!rb) ERROR2(false, dsa?"Invalid renderbuffer name":"No renderbuffer bound");
    if (!rb->has_object) ERROR2(false, "Renderbuffer name has no obejct");
    int maxsize = trc_gl_state_get_state_int(ctx->trace, GL_MAX_RENDERBUFFER_SIZE, 0);
    if (width<0 || height<0 || width>maxsize || height>maxsize)
        ERROR2(false, "Invalid dimensions");
    //TODO: test if samples if valid
    //TODO: handle when internal_format is not renderable
    return true;
}

static bool begin_query(GLenum target, GLuint index, GLuint id) {
    if (index >= trc_gl_state_get_bound_queries_size(ctx->trace, target))
        ERROR2(false, "Index is greater than maximum");
    if (trc_gl_state_get_bound_queries(ctx->trace, target, index))
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
    
    trc_gl_state_set_bound_queries(ctx->trace, target, index, newrev.head.obj);
    
    return true;
}

static bool end_query(GLenum target, GLuint index) {
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

static void on_activate_tf() {
    size_t buf_count;
    trc_obj_t** bufs = get_tf_buffer_list(&buf_count);
    for (size_t i = 0; i < buf_count; i++) {
        if (((const trc_gl_buffer_rev_t*)trc_obj_get_rev(bufs[i], -1))->mapped)
            trc_add_warning(cmd, "Buffer bound to GL_TRANSFORM_FEEDBACK_BUFFER is mapped");
    }
}

static void test_host_config(const trc_replay_config_t* host, const trc_replay_config_t* trace) {
    typedef struct cap_info_t {
        const char* name;
        size_t offset;
    } cap_info_t;
    cap_info_t caps[] = {
        {"version", offsetof(trc_replay_config_t, version)},
        {"max_vertex_streams", offsetof(trc_replay_config_t, max_vertex_streams)},
        {"max_clip_distances", offsetof(trc_replay_config_t, max_clip_distances)},
        {"max_draw_buffers", offsetof(trc_replay_config_t, max_draw_buffers)},
        {"max_viewports", offsetof(trc_replay_config_t, max_viewports)},
        {"max_vertex_attribs", offsetof(trc_replay_config_t, max_vertex_attribs)},
        {"max_color_attachments", offsetof(trc_replay_config_t, max_color_attachments)},
        {"max_combined_texture_units", offsetof(trc_replay_config_t, max_combined_texture_units)},
        {"max_patch_vertices", offsetof(trc_replay_config_t, max_patch_vertices)},
        {"max_renderbuffer_size", offsetof(trc_replay_config_t, max_renderbuffer_size)},
        {"max_texture_size", offsetof(trc_replay_config_t, max_texture_size)},
        {"max_xfb_buffers", offsetof(trc_replay_config_t, max_xfb_buffers)},
        {"max_ubo_bindings", offsetof(trc_replay_config_t, max_ubo_bindings)},
        {"max_atomic_counter_buffer_bindings", offsetof(trc_replay_config_t, max_atomic_counter_buffer_bindings)},
        {"max_ssbo_bindings", offsetof(trc_replay_config_t, max_ssbo_bindings)}};
    size_t cap_count = sizeof(caps) / sizeof(caps[0]);
    
    for (size_t i = 0; i < cap_count; i++) {
        const char* name = caps[i].name;
        int host_val = *(const int*)((const uint8_t*)host+caps[i].offset);
        int trace_val = *(const int*)((const uint8_t*)trace+caps[i].offset);
        if (host_val < trace_val) {
            trc_add_warning(cmd, "Host value for capability '%s' is lower than that of trace: %d < %d",
                            name, host_val, trace_val);
        }
    }
}

static void handle_new_context_config(trc_gl_context_rev_t* rev, trace_extra_t* extra) {
    init_host_config(ctx, &rev->host_cfg);
    rev->trace_cfg = rev->host_cfg;
    trc_replay_config_t* cfg = &rev->trace_cfg;
    
    if (extra) {
        typedef struct option_info_t {
            const char* name;
            int* ptr;
        } option_info_t;
        
        //TODO: Use the array in test_host_config()
        const option_info_t options[] = {
            {"version", &cfg->version},
            {"max_vertex_streams", &cfg->max_vertex_streams},
            {"max_clip_distances", &cfg->max_clip_distances},
            {"max_draw_buffers", &cfg->max_draw_buffers},
            {"max_viewports", &cfg->max_viewports},
            {"max_vertex_attribs", &cfg->max_vertex_attribs},
            {"max_color_attachments", &cfg->max_color_attachments},
            {"max_combined_texture_units", &cfg->max_combined_texture_units},
            {"max_patch_vertices", &cfg->max_patch_vertices},
            {"max_renderbuffer_size", &cfg->max_renderbuffer_size},
            {"max_texture_size", &cfg->max_texture_size},
            {"max_xfb_buffers", &cfg->max_xfb_buffers},
            {"max_ubo_bindings", &cfg->max_ubo_bindings},
            {"max_atomic_counter_buffer_bindings", &cfg->max_atomic_counter_buffer_bindings},
            {"max_ssbo_bindings", &cfg->max_ssbo_bindings},
            {"nvidia_xfb_object_bindings_bug", &cfg->nvidia_xfb_object_bindings_bug}};
        size_t option_count = sizeof(options) / sizeof(options[0]);
        
        data_reader_t dr = dr_new(extra->size, extra->data);
        
        uint32_t count;
        if (!dr_read_le(&dr, 4, &count, -1))
            ERROR2(, "Invalid %s extra: End of data", extra->name);
        
        for (uint32_t i = 0; i < count; i++) {
            uint32_t name_len;
            if (!dr_read_le(&dr, 4, &name_len, -1))
                ERROR2(, "Invalid %s extra: End of data", extra->name);
            char* name = calloc(name_len+1, 1);
            if (!dr_read(&dr, name_len, name)) {
                free(name);
                ERROR2(, "Invalid %s extra: End of data", extra->name);
            }
            
            int32_t value;
            if (!dr_read_le(&dr, 4, &value, -1))
                ERROR2(, "Invalid %s extra: End of data", extra->name);
            
            for (size_t j = 0; j < option_count; j++) {
                if (strcmp(options[j].name, name)) continue;
                *options[j].ptr = value;
                goto done;
            }
            trc_add_error(cmd, "Unknown target option '%s'", ctx->target_option_names[i]);
            done: ;
            
            free(name);
        }
    }
    
    test_host_config(&rev->host_cfg, &rev->trace_cfg);
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
        reload_gl_funcs();
        trc_set_current_gl_context(ctx->trace, trc_lookup_name(global_ns, TrcContext, p_ctx, -1));
        if (!trc_get_context(ctx->trace)->made_current_before) {
            trc_gl_context_rev_t rev = *trc_get_context(ctx->trace);
            trace_extra_t* config_extra = trc_get_extrai(cmd, "replay/glXMakeCurrent/config", 0);
            //config_extra may be NULL but handle_new_context_config handles that
            handle_new_context_config(&rev, config_extra);
            rev.made_current_before = true;
            trc_set_context(ctx->trace, &rev);
            
            init_context();
            
            trace_extra_t* extra = trc_get_extra(cmd, "replay/glXMakeCurrent/drawable_size");
            if (!extra) ERROR("replay/glXMakeCurrent/drawable_size extra not found");
            if (extra->size != 8) ERROR("replay/glXMakeCurrent/drawable_size is not 8 bytes");
            int32_t width = ((int32_t*)extra->data)[0];
            int32_t height = ((int32_t*)extra->data)[1];
            if (width>=0 && height>=0) { //TODO: Move this into init_context()
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
    } else {
        reset_gl_funcs();
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

glXGetCurrentContext:
    ;

glXGetCurrentDisplay:
    ;

glXGetCurrentDrawable:
    ;

glXGetCurrentReadDrawable:
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
    reload_gl_funcs();
    
    trc_gl_context_rev_t rev;
    rev.real = res;
    if (shareList) rev.namespace = shareList->namespace;
    else rev.namespace = trc_create_namespace(ctx->trace);
    rev.priv_ns = trc_create_namespace(ctx->trace);
    rev.made_current_before = false;
    
    uint64_t fake = trc_get_ptr(&cmd->ret)[0];
    trc_obj_t* cur_ctx = trc_create_named_obj(global_ns, TrcContext, fake, &rev);
    
    SDL_GL_MakeCurrent(ctx->window, last_ctx);
    reload_gl_funcs();

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
    reload_gl_funcs();
    
    trc_gl_context_rev_t rev;
    rev.real = res;
    if (share_ctx) rev.namespace = share_ctx->namespace;
    else rev.namespace = trc_create_namespace(ctx->trace);
    rev.priv_ns = trc_create_namespace(ctx->trace);
    rev.made_current_before = false;
    
    uint64_t fake = trc_get_ptr(&cmd->ret)[0];
    trc_obj_t* cur_ctx = trc_create_named_obj(global_ns, TrcContext, fake, &rev);
    
    SDL_GL_MakeCurrent(ctx->window, last_ctx);
    reload_gl_funcs();
    
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
    
    delete_obj(p_ctx, TrcContext);

glXSwapBuffers: //Display* p_dpy, GLXDrawable p_drawable
    if (!trc_get_current_gl_context(ctx->trace, -1)) ERROR("No current OpenGL context");
    SDL_GL_SwapWindow(ctx->window);
    update_fb0_buffers(false, true, false, false);

wip15DrawableSize: //GLsizei p_width, GLsizei p_height
    if (p_width < 0) p_width = 100;
    if (p_height < 0) p_height = 100;
    
    SDL_SetWindowSize(ctx->window, p_width, p_height);
    
    trc_gl_context_rev_t state = *trc_get_context(ctx->trace);
    if (state.drawable_width==p_width && state.drawable_height==p_height) return;
    state.drawable_width = p_width;
    state.drawable_height = p_height;
    trc_set_context(ctx->trace, &state);
    update_fb0_buffers(true, true, true, true);

glClear: //GLbitfield p_mask
    real(p_mask);
    update_buffers(trc_gl_state_get_draw_framebuffer(ctx->trace), p_mask, true);

glFlush: //
    real();

glFinish: //
    real();

glIsEnabled: //GLenum p_cap
    ;

glIsEnabledi: //GLenum p_target, GLuint p_index
    ; //TODO: Validation

glIsQuery: //GLuint p_id
    ;

glIsFramebuffer: //GLuint p_framebuffer
    ;

glIsRenderbuffer: //GLuint p_renderbuffer
    ;

glIsSync: //GLsync p_sync
    ;

glIsTransformFeedback: //GLuint p_id
    ;

glTransformFeedbackVaryings: //GLuint p_program, GLsizei p_count, const GLchar*const* p_varyings, GLenum p_bufferMode
    if (!p_program) ERROR("Invalid program name");
    real(p_program_rev->real, p_count, p_varyings, p_bufferMode);
    //TODO: Store this in the program object

glGetQueryiv: //GLenum p_target, GLenum p_pname, GLint* p_params
    ;

glGetQueryObjectiv: //GLuint p_id, GLenum p_pname, GLint* p_params
    GLuint real_query = get_real_query(p_id);
    if (!real_query) ERROR("Invalid query name");

glGetQueryObjectuiv: //GLuint p_id, GLenum p_pname, GLuint* p_params
    GLuint real_query = get_real_query(p_id);
    if (!real_query) ERROR("Invalid query name");

glGetQueryObjecti64v: //GLuint p_id, GLenum p_pname, GLint64* p_params
    GLuint real_query = get_real_query(p_id);
    if (!real_query) ERROR("Invalid query name");

glGetQueryObjectui64v: //GLuint p_id, GLenum p_pname, GLuint64* p_params
    GLuint real_query = get_real_query(p_id);
    if (!real_query) ERROR("Invalid query name");

glGetFramebufferParameteriv: //GLenum p_target, GLenum p_pname, GLint* p_params
    trc_obj_t* fb = get_bound_framebuffer(p_target);
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
    #define A(gf, t, namespace) {\
        const t* rev = gf(p_name);\
        if (!rev) ERROR("Invalid object name");\
        if (!rev->has_object) ERROR("Name has no object");\
        break;\
    }
    #define B(gf, t, namespace) {\
        const t* rev = gf(p_name);\
        if (!rev) ERROR("Invalid object name");\
        break;\
    }
    switch (p_identifier) {
    case GL_BUFFER: A(get_buffer, trc_gl_buffer_rev_t, ns)
    case GL_SHADER: B(get_shader, trc_gl_shader_rev_t, ns)
    case GL_PROGRAM: B(get_program, trc_gl_program_rev_t, ns)
    case GL_VERTEX_ARRAY: A(get_vao, trc_gl_vao_rev_t, priv_ns)
    case GL_QUERY: A(get_query, trc_gl_query_rev_t, ns)
    case GL_PROGRAM_PIPELINE: A(get_program_pipeline, trc_gl_program_pipeline_rev_t, priv_ns)
    case GL_TRANSFORM_FEEDBACK: A(get_transform_feedback, trc_gl_transform_feedback_rev_t, priv_ns)
    case GL_SAMPLER: B(get_sampler, trc_gl_sampler_rev_t, ns)
    case GL_TEXTURE: A(get_texture, trc_gl_texture_rev_t, ns)
    case GL_RENDERBUFFER: A(get_renderbuffer, trc_gl_renderbuffer_rev_t, ns)
    case GL_FRAMEBUFFER: A(get_framebuffer, trc_gl_framebuffer_rev_t, priv_ns)
    }
    #undef B
    #undef A

glGetError: //
    ;

glGetTransformFeedbackVarying: //GLuint p_program, GLuint p_index, GLsizei p_bufSize, GLsizei* p_length, GLsizei* p_size, GLenum* p_type, GLchar* p_name
    if (!p_program_rev) ERROR("Invalid program name");
    if (!p_program_rev->has_been_linked) ERROR("Program has not been linked");
    GLint varying_count;
    F(glGetProgramiv)(p_program_rev->real, GL_TRANSFORM_FEEDBACK_VARYINGS, &varying_count);
    if (p_index >= varying_count) ERROR("Index is out of bounds");

glCheckFramebufferStatus: //GLenum p_target
    ;

glCheckNamedFramebufferStatus: //GLuint p_framebuffer
    if (!p_framebuffer_rev) ERROR2(, "Invalid framebuffer name");
    if (!p_framebuffer_rev->has_object) ERROR2(, "Framebuffer name has no object");

glGetPointerv: //GLenum p_pname, void** p_params
    ;

glGetProgramPipelineiv: //GLuint p_pipeline, GLenum p_pname, GLint  * p_params
    if (!p_pipeline_rev) ERROR2(, "Invalid program pipeline name");
    if (!p_pipeline_rev->has_object) ERROR2(, "Program pipeline name has no object");

//TODO: Validation for these two functions
glGetTexImage: //GLenum p_target, GLint p_level, GLenum p_format, GLenum p_type, void* p_pixels
    ;

glGetCompressedTexImage: //GLenum p_target, GLint p_level, void* p_img
    ;

glGetBooleanv: //GLenum p_pname, GLboolean* p_data
    ;

glGetFloatv: //GLenum p_pname, GLfloat* p_data
    ;

glGetIntegerv: //GLenum p_pname, GLint* p_data
    ;

glGetString: //GLenum p_name
    ;

glGetDoublev: //GLenum p_pname, GLdouble* p_data
    ;

//TODO: Validation for these
glGetInteger64v: //GLenum p_pname, GLint64* p_data
    ;

glGetBooleani_v: //GLenum p_target, GLuint p_index, GLboolean* p_data
    ;

glGetIntegeri_v: //GLenum p_target, GLuint p_index, GLint* p_data
    ;

glGetFloati_v: //GLenum p_target, GLuint p_index, GLfloat* p_data
    ;

glGetStringi: //GLenum p_name, GLuint p_index
    ;

glGetDoublei_v: //GLenum p_target, GLuint p_index, GLdouble* p_data
    ;

glGetInteger64i_v: //GLenum p_target, GLuint p_index, GLint64* p_data
    ;

glReadPixels: //GLint p_x, GLint p_y, GLsizei p_width, GLsizei p_height, GLenum p_format, GLenum p_type, void * p_pixels
    ;

glBeginConditionalRender: //GLuint p_id, GLenum p_mode
    uint real_id = get_real_query(p_id);
    if (!real_id) ERROR("Invalid query name");
    real(real_id, p_mode);

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

glGetMultisamplefv: //GLenum p_pname, GLuint p_index, GLfloat* p_val
    ; //TODO: More validation should be done

glGetInternalformativ: //GLenum p_target, GLenum p_internalformat, GLenum p_pname, GLsizei p_bufSize, GLint* p_params
    GLint* params = replay_alloc(p_bufSize*sizeof(GLint));
    real(p_target, p_internalformat, p_pname, p_bufSize, params);

glGetInternalformati64v: //GLenum p_target, GLenum p_internalformat, GLenum p_pname, GLsizei p_bufSize, GLint64* p_params
    GLint64* params = replay_alloc(p_bufSize*sizeof(GLint64));
    real(p_target, p_internalformat, p_pname, p_bufSize, params);

glGetSynciv: //GLsync p_sync, GLenum p_pname, GLsizei p_bufSize, GLsizei* p_length, GLint* p_values
    if (!get_real_sync(p_sync)) ERROR("Invalid sync name");
    //TODO: More validation should be done

glGenFramebuffers: //GLsizei p_n, GLuint* p_framebuffers
    if (p_n < 0) ERROR("Invalid framebuffer name count");
    GLuint* fbs = replay_alloc(p_n*sizeof(GLuint));
    real(p_n, fbs);
    gen_framebuffers(p_n, fbs, p_framebuffers, false);

glCreateFramebuffers: //GLsizei p_n, GLuint* p_framebuffers
    if (p_n < 0) ERROR("Invalid framebuffer name count");
    GLuint* fbs = replay_alloc(p_n*sizeof(GLuint));
    real(p_n, fbs);
    gen_framebuffers(p_n, fbs, p_framebuffers, true);

glDeleteFramebuffers: //GLsizei p_n, const GLuint* p_framebuffers
    GLuint* fbs = replay_alloc(p_n*sizeof(GLuint));
    for (size_t i = 0; i < p_n; ++i) {
        trc_obj_t* fb = trc_lookup_name(ctx->priv_ns, TrcFramebuffer, p_framebuffers[i], -1);
        if (fb && fb==trc_gl_state_get_read_framebuffer(ctx->trace))
            trc_gl_state_set_read_framebuffer(ctx->trace, NULL);
        if (fb && fb==trc_gl_state_get_draw_framebuffer(ctx->trace))
            trc_gl_state_set_draw_framebuffer(ctx->trace, NULL);
        if (!(fbs[i] = get_real_framebuffer(p_framebuffers[i])) && p_framebuffers[i]) {
            trc_add_warning(cmd, "Invalid framebuffer name");
        } else {
            delete_obj(p_framebuffers[i], TrcFramebuffer);
        }
    }
    real(p_n, fbs);

glBindFramebuffer: //GLenum p_target, GLuint p_framebuffer
    const trc_gl_framebuffer_rev_t* rev = get_framebuffer(p_framebuffer);
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
    gen_renderbuffers(p_n, rbs, p_renderbuffers, false);

glCreateRenderbuffers: //GLsizei p_n, GLuint* p_renderbuffers
    if (p_n < 0) ERROR("Invalid renderbuffer name count");
    GLuint* rbs = replay_alloc(p_n*sizeof(GLuint));
    real(p_n, rbs);
    gen_renderbuffers(p_n, rbs, p_renderbuffers, true);

glDeleteRenderbuffers: //GLsizei p_n, const GLuint* p_renderbuffers
    GLuint* rbs = replay_alloc(p_n*sizeof(GLuint));
    for (size_t i = 0; i < p_n; ++i) {
        trc_obj_t* rb = trc_lookup_name(ctx->ns, TrcRenderbuffer, p_renderbuffers[i], -1);
        if (p_renderbuffers[i] && rb==trc_gl_state_get_bound_renderbuffer(ctx->trace))
            trc_gl_state_set_bound_renderbuffer(ctx->trace, NULL);
        //TODO: Detach from bound framebuffers
        //TODO: What to do with renderbuffers attached to non-bound framebuffers?
        if (!(rbs[i] = get_real_renderbuffer(p_renderbuffers[i])) && p_renderbuffers[i])
            trc_add_warning(cmd, "Invalid renderbuffer name");
        else delete_obj(p_renderbuffers[i], TrcRenderbuffer);
    }
    
    real(p_n, rbs);

glBindRenderbuffer: //GLenum p_target, GLuint p_renderbuffer
    const trc_gl_renderbuffer_rev_t* rev = get_renderbuffer(p_renderbuffer);
    if (!rev && p_renderbuffer) ERROR("Invalid renderbuffer name");
    real(p_target, p_renderbuffer?rev->real:0);
    if (rev && !rev->has_object) {
        trc_gl_renderbuffer_rev_t newrev = *rev;
        newrev.has_object = true;
        set_renderbuffer(&newrev);
    }
    trc_gl_state_set_bound_renderbuffer(ctx->trace, rev?rev->head.obj:NULL);

glGetActiveUniformBlockiv: //GLuint p_program, GLuint p_uniformBlockIndex, GLenum p_pname, GLint* p_params
    GLuint program = get_real_program(p_program);
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
    GLuint program = get_real_program(p_program);
    if (!program) ERROR("Invalid program name");
    GLchar buf[64];
    real(program, p_uniformBlockIndex, 64, NULL, buf);

glGetActiveUniformName: //GLuint p_program, GLuint p_uniformIndex, GLsizei p_bufSize, GLsizei* p_length, GLchar* p_uniformName
    GLuint program = get_real_program(p_program);
    if (!program) ERROR("Invalid program name");
    GLchar buf[64];
    real(program, p_uniformIndex, 64, NULL, buf);

glGetActiveUniformsiv: //GLuint p_program, GLsizei p_uniformCount, const GLuint* p_uniformIndices, GLenum p_pname, GLint* p_params
    GLuint program = get_real_program(p_program);
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
    trc_obj_t* fb = get_bound_framebuffer(p_target);
    if (add_fb_attachment_rb(false, fb, p_attachment, p_renderbuffer, p_renderbuffer_rev))
        real(p_target, p_attachment, p_renderbuffertarget, p_renderbuffer?p_renderbuffer_rev->real:0);

glNamedFramebufferRenderbuffer: //GLuint p_framebuffer, GLenum p_attachment, GLenum p_renderbuffertarget, GLuint p_renderbuffer
    trc_obj_t* fb = trc_lookup_name(ctx->priv_ns, TrcFramebuffer, p_framebuffer, -1);
    if (add_fb_attachment_rb(true, fb, p_attachment, p_renderbuffer, p_renderbuffer_rev))
        real(p_framebuffer_rev->real, p_attachment, p_renderbuffertarget, p_renderbuffer?p_renderbuffer_rev->real:0);

glFramebufferTexture: //GLenum p_target, GLenum p_attachment, GLuint p_texture, GLint p_level
    trc_obj_t* fb = get_bound_framebuffer(p_target);
    if (add_fb_attachment(fb, p_attachment, false, p_texture, p_texture_rev, p_texture_rev?p_texture_rev->type:0, p_level, 0))
        real(p_target, p_attachment, p_texture?p_texture_rev->real:0, p_level);

glNamedFramebufferTexture: //GLuint p_framebuffer, GLenum p_attachment, GLuint p_texture, GLint p_level
    trc_obj_t* fb = trc_lookup_name(ctx->priv_ns, TrcFramebuffer, p_framebuffer, -1);
    if (add_fb_attachment(fb, p_attachment, true, p_texture, p_texture_rev, p_texture_rev?p_texture_rev->type:0, p_level, 0))
        real(p_framebuffer_rev->real, p_attachment, p_texture?p_texture_rev->real:0, p_level);

glFramebufferTextureLayer: //GLenum p_target, GLenum p_attachment GLuint p_texture, GLint p_level, GLint p_layer
    trc_obj_t* fb = get_bound_framebuffer(p_target);
    if (add_fb_attachment(fb, p_attachment, false, p_texture, p_texture_rev, p_texture_rev?p_texture_rev->type:0, p_level, p_layer))
        real(p_target, p_attachment, p_texture?p_texture_rev->real:0, p_level, p_layer);

glNamedFramebufferTextureLayer: //GLuint p_framebuffer, GLenum p_attachment GLuint p_texture, GLint p_level, GLint p_layer
    trc_obj_t* fb = trc_lookup_name(ctx->priv_ns, TrcFramebuffer, p_framebuffer, -1);
    if (add_fb_attachment(fb, p_attachment, true, p_texture, p_texture_rev, p_texture_rev?p_texture_rev->type:0, p_level, p_layer))
        real(p_framebuffer_rev->real, p_attachment, p_texture?p_texture_rev->real:0, p_level, p_layer);

glFramebufferTexture1D: //GLenum p_target, GLenum p_attachment, GLenum p_textarget, GLuint p_texture, GLint p_level
    trc_obj_t* fb = get_bound_framebuffer(p_target);
    if (add_fb_attachment(fb, p_attachment, false, p_texture, p_texture_rev, p_textarget, p_level, 0))
        real(p_target, p_attachment, p_textarget, p_texture?p_texture_rev->real:0, p_level);

glFramebufferTexture2D: //GLenum p_target, GLenum p_attachment, GLenum p_textarget, GLuint p_texture, GLint p_level
    trc_obj_t* fb = get_bound_framebuffer(p_target);
    if (add_fb_attachment(fb, p_attachment, false, p_texture, p_texture_rev, p_textarget, p_level, 0))
        real(p_target, p_attachment, p_textarget, p_texture?p_texture_rev->real:0, p_level);

glFramebufferTexture3D: //GLenum p_target, GLenum p_attachment, GLenum p_textarget, GLuint p_texture, GLint p_level, GLint p_zoffset
    trc_obj_t* fb = get_bound_framebuffer(p_target);
    if (add_fb_attachment(fb, p_attachment, false, p_texture, p_texture_rev, p_textarget, p_level, p_zoffset))
        real(p_target, p_attachment, p_textarget, p_texture?p_texture_rev->real:0, p_level, p_zoffset);

glRenderbufferStorage: //GLenum p_target, GLenum p_internalformat, GLsizei p_width, GLsizei p_height
    trc_obj_t* rb = trc_gl_state_get_bound_renderbuffer(ctx->trace);
    const trc_gl_renderbuffer_rev_t* rb_rev = trc_obj_get_rev(rb, -1);
    if (renderbuffer_storage(rb_rev, false, p_internalformat, p_width, p_height, 1)) {
        real(p_target, p_internalformat, p_width, p_height);
        update_renderbuffer(rb_rev, p_width, p_height, p_internalformat, 1);
    }

glNamedRenderbufferStorage: //GLuint p_renderbuffer, GLenum p_internalformat, GLsizei p_width, GLsizei p_height
    if (renderbuffer_storage(p_renderbuffer_rev, false, p_internalformat, p_width, p_height, 1)) {
        real(p_renderbuffer_rev->real, p_internalformat, p_width, p_height);
        update_renderbuffer(p_renderbuffer_rev, p_width, p_height, p_internalformat, 1);
    }

glRenderbufferStorageMultisample: //GLenum p_target, GLsizei p_samples, GLenum p_internalformat, GLsizei p_width, GLsizei p_height
    trc_obj_t* rb = trc_gl_state_get_bound_renderbuffer(ctx->trace);
    const trc_gl_renderbuffer_rev_t* rb_rev = trc_obj_get_rev(rb, -1);
    if (renderbuffer_storage(rb_rev, false, p_internalformat, p_width, p_height, p_samples)) {
        real(p_target, p_samples, p_internalformat, p_width, p_height);
        update_renderbuffer(rb_rev, p_width, p_height, p_internalformat, p_samples);
    }

glNamedRenderbufferStorageMultisample: //GLuint p_renderbuffer, GLsizei p_samples, GLenum p_internalformat, GLsizei p_width, GLsizei p_height
    if (renderbuffer_storage(p_renderbuffer_rev, false, p_internalformat, p_width, p_height, p_samples)) {
        real(p_renderbuffer, p_samples, p_internalformat, p_width, p_height);
        update_renderbuffer(p_renderbuffer_rev, p_width, p_height, p_internalformat, p_samples);
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
    if (p_sync) delete_obj(p_sync, TrcSync);

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
    
    update_query(p_target, p_id, rev->real);

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
        rev.draw_buffers = trc_create_data(ctx->trace, p_n*sizeof(GLenum), p_bufs, 0);
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

glBindFragDataLocation: //GLuint p_program, GLuint p_color, const GLchar* p_name
    GLuint real_program = get_real_program(p_program);
    if (!real_program) ERROR("Invalid program name");
    real(real_program, p_color, p_name);

glBindFragDataLocationIndexed: //GLuint p_program, GLuint p_colorNumber, GLuint p_index, const GLchar* p_name
    GLuint real_program = get_real_program(p_program);
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
    if (p_cap!=GL_DEBUG_OUTPUT && p_cap!=GL_DEBUG_OUTPUT_SYNCHRONOUS) //These are set by the replayer
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
    pixel_store(p_pname, p_param);

glPixelStorei: //GLenum p_pname, GLint p_param
    pixel_store(p_pname, p_param);

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

glGetQueryIndexediv: //GLenum p_target, GLuint p_index, GLenum p_pname, GLint* p_params
    ;

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
    
    trc_gl_state_set_current_tf(ctx->trace, rev->head.obj);

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
            if (trc_gl_state_get_current_tf(ctx->trace) == tf->head.obj)
                trc_gl_state_set_current_tf(ctx->trace, get_transform_feedback(0)->head.obj);
            delete_obj(p_ids[i], TrcTransformFeedback);
        }
    }
    real(p_n, tf);

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
    trc_gl_state_set_tf_primitive(ctx->trace, p_primitiveMode);
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
