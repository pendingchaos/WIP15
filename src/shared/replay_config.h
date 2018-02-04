/*
nvidia_xfb_object_bindings_bug:
    Bug where transform feedback objects don't store their bindings
*/
#ifndef REPLAY_CONFIG_DEFS
#define REPLAY_CONFIG_DEFS
#include <stddef.h>

typedef enum trc_replay_config_option_type_t {
    TrcReplayCfgOpt_CapInt,
    TrcReplayCfgOpt_FeatureBool,
    TrcReplayCfgOpt_BugBool
} trc_replay_config_option_type_t;

typedef struct trc_replay_config_option_t {
    const char* name;
    trc_replay_config_option_type_t type;
    size_t offset;
} trc_replay_config_option_t;

#define _ \
CAP_INT(version)\
CAP_INT(max_vertex_streams)\
CAP_INT(max_clip_distances)\
CAP_INT(max_draw_buffers)\
CAP_INT(max_viewports)\
CAP_INT(max_vertex_attribs)\
CAP_INT(max_vertex_attrib_stride)\
CAP_INT(max_vertex_attrib_relative_offset)\
CAP_INT(max_vertex_attrib_bindings)\
CAP_INT(max_color_attachments)\
CAP_INT(max_combined_texture_units)\
CAP_INT(max_patch_vertices)\
CAP_INT(max_renderbuffer_size)\
CAP_INT(max_texture_size)\
CAP_INT(max_xfb_buffers)\
CAP_INT(max_ubo_bindings)\
CAP_INT(max_atomic_counter_buffer_bindings)\
CAP_INT(max_ssbo_bindings)\
CAP_INT(max_sample_mask_words)\
BUG_BOOL(nvidia_xfb_object_bindings_bug)

typedef struct trc_replay_config_t {
    #define CAP_INT(name) int name;
    #define FEATURE_BOOL(name) bool name;
    #define BUG_BOOL(name) bool name;
    _
    #undef CAP_INT
    #undef FEATURE_BOOL
    #undef BUG_BOOL
} trc_replay_config_t;

static trc_replay_config_option_t trc_replay_config_options[] = {
    #define CAP_INT(name) {#name, TrcReplayCfgOpt_CapInt, offsetof(trc_replay_config_t, name)},
    #define FEATURE_BOOL(name) {#name, TrcReplayCfgOpt_FeatureBool, offsetof(trc_replay_config_t, name)},
    #define BUG_BOOL(name) {#name, TrcReplayCfgOpt_BugBool, offsetof(trc_replay_config_t, name)},
    _
    #undef CAP_INT
    #undef FEATURE_BOOL
    #undef BUG_BOOL
};

#undef _
#endif

#ifdef REPLAY_CONFIG_FUNCS
static void init_host_config(void* ctx, trc_replay_config_t* cfg) {
    memset(cfg, 0, sizeof(*cfg));
    
    GLint major, minor;
    RC_F(glGetIntegerv)(GL_MAJOR_VERSION, &major);
    RC_F(glGetIntegerv)(GL_MINOR_VERSION, &minor);
    uint ver = major*100 + minor*10;
    cfg->version = ver;
    
    if (ver >= 400) RC_F(glGetIntegerv)(GL_MAX_VERTEX_STREAMS, &cfg->max_vertex_streams);
    else cfg->max_vertex_streams = 1;
    RC_F(glGetIntegerv)(GL_MAX_CLIP_DISTANCES, &cfg->max_clip_distances);
    RC_F(glGetIntegerv)(GL_MAX_DRAW_BUFFERS, &cfg->max_draw_buffers);
    if (ver >= 410) RC_F(glGetIntegerv)(GL_MAX_VIEWPORTS, &cfg->max_viewports);
    else cfg->max_viewports = 1;
    RC_F(glGetIntegerv)(GL_MAX_VERTEX_ATTRIBS, &cfg->max_vertex_attribs);
    if (ver >= 430)
        RC_F(glGetIntegerv)(GL_MAX_VERTEX_ATTRIB_BINDINGS, &cfg->max_vertex_attrib_bindings);
    if (ver >= 440)
        RC_F(glGetIntegerv)(GL_MAX_VERTEX_ATTRIB_STRIDE, &cfg->max_vertex_attrib_stride);
    if (ver >= 430)
        RC_F(glGetIntegerv)(GL_MAX_VERTEX_ATTRIB_RELATIVE_OFFSET, &cfg->max_vertex_attrib_relative_offset);
    RC_F(glGetIntegerv)(GL_MAX_COLOR_ATTACHMENTS, &cfg->max_color_attachments);
    RC_F(glGetIntegerv)(GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS, &cfg->max_combined_texture_units);
    if (ver>=400) RC_F(glGetIntegerv)(GL_MAX_PATCH_VERTICES, &cfg->max_patch_vertices);
    RC_F(glGetIntegerv)(GL_MAX_RENDERBUFFER_SIZE, &cfg->max_renderbuffer_size);
    RC_F(glGetIntegerv)(GL_MAX_TEXTURE_SIZE, &cfg->max_texture_size);
    RC_F(glGetIntegerv)(GL_MAX_TRANSFORM_FEEDBACK_BUFFERS, &cfg->max_xfb_buffers);
    RC_F(glGetIntegerv)(GL_MAX_UNIFORM_BUFFER_BINDINGS, &cfg->max_ubo_bindings);
    if (ver >= 420) RC_F(glGetIntegerv)(GL_MAX_ATOMIC_COUNTER_BUFFER_BINDINGS, &cfg->max_atomic_counter_buffer_bindings);
    if (ver >= 430) RC_F(glGetIntegerv)(GL_MAX_SHADER_STORAGE_BUFFER_BINDINGS, &cfg->max_ssbo_bindings);
    RC_F(glGetIntegerv)(GL_MAX_SAMPLE_MASK_WORDS, &cfg->max_sample_mask_words);
    
    cfg->nvidia_xfb_object_bindings_bug = false; //TODO
}
#endif
