#ifndef REPLAY_CONFIG_DEFS
#define REPLAY_CONFIG_DEFS
typedef struct trc_replay_config_t {
    int version;
    int max_vertex_streams;
    int max_clip_distances;
    int max_draw_buffers;
    int max_viewports;
    int max_vertex_attribs;
    int max_vertex_attrib_bindings;
    int max_color_attachments;
    int max_combined_texture_units;
    int max_patch_vertices;
    int max_renderbuffer_size;
    int max_texture_size;
    int max_xfb_buffers;
    int max_ubo_bindings;
    int max_atomic_counter_buffer_bindings;
    int max_ssbo_bindings;
    int max_sample_mask_words;
    //Bug where transform feedback objects don't store their bindings
    int nvidia_xfb_object_bindings_bug;
} trc_replay_config_t;
#endif

#ifdef REPLAY_CONFIG_FUNCS
static void init_host_config(void* ctx, trc_replay_config_t* cfg) {
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
    if (ver >= 450)
        RC_F(glGetIntegerv)(GL_MAX_VERTEX_ATTRIB_BINDINGS, &cfg->max_vertex_attrib_bindings);
    else
        cfg->max_vertex_attrib_bindings = 0;
    RC_F(glGetIntegerv)(GL_MAX_COLOR_ATTACHMENTS, &cfg->max_color_attachments);
    RC_F(glGetIntegerv)(GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS, &cfg->max_combined_texture_units);
    if (ver>=400) RC_F(glGetIntegerv)(GL_MAX_PATCH_VERTICES, &cfg->max_patch_vertices);
    else cfg->max_patch_vertices = 0;
    RC_F(glGetIntegerv)(GL_MAX_RENDERBUFFER_SIZE, &cfg->max_renderbuffer_size);
    RC_F(glGetIntegerv)(GL_MAX_TEXTURE_SIZE, &cfg->max_texture_size);
    RC_F(glGetIntegerv)(GL_MAX_TRANSFORM_FEEDBACK_BUFFERS, &cfg->max_xfb_buffers);
    RC_F(glGetIntegerv)(GL_MAX_UNIFORM_BUFFER_BINDINGS, &cfg->max_ubo_bindings);
    if (ver >= 420) RC_F(glGetIntegerv)(GL_MAX_ATOMIC_COUNTER_BUFFER_BINDINGS, &cfg->max_atomic_counter_buffer_bindings);
    else cfg->max_atomic_counter_buffer_bindings = 0;
    if (ver >= 430) RC_F(glGetIntegerv)(GL_MAX_SHADER_STORAGE_BUFFER_BINDINGS, &cfg->max_ssbo_bindings);
    else cfg->max_ssbo_bindings = 0;
    RC_F(glGetIntegerv)(GL_MAX_SAMPLE_MASK_WORDS, &cfg->max_sample_mask_words);
    
    cfg->nvidia_xfb_object_bindings_bug = false; //TODO
}
#endif
