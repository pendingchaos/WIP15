#include "libinspect/actions.h"

#include <stdlib.h>
#include <string.h>
#include <GL/gl.h>

static void apply_gen_tex(inspector_t* inspector, inspect_action_t* action) {
    inspect_texture_t tex;
    tex.fake = action->obj;
    tex.params.texture = action->obj;
    tex.type = tex.width = tex.height = tex.depth = tex.mipmap_count
    = tex.layer_count = 0;
    tex.mipmaps = NULL;
    append_inspect_tex_vec(inspector->textures, &tex);
}

static void apply_del_tex(inspector_t* inspector, inspect_action_t* action) {
    inspect_texture_t* tex = inspect_find_tex_ptr(inspector, action->obj);
    if (!tex)
        return;
    
    inspect_free_tex_mipmaps(tex);
    
    size_t index = tex - get_inspect_tex_vec_data(inspector->textures);
    remove_inspect_tex_vec(inspector->textures, index, 1);
}

static void apply_gen_buf(inspector_t* inspector, inspect_action_t* action) {
    inspect_buffer_t buf;
    buf.fake = action->obj;
    buf.usage = 0;
    buf.size = 0;
    buf.data = NULL;
    append_inspect_buf_vec(inspector->buffers, &buf);
}

static void apply_del_buf(inspector_t* inspector, inspect_action_t* action) {
    inspect_buffer_t* buf = inspect_find_buf_ptr(inspector, action->obj);
    if (!buf)
        return;
    
    free(buf->data);
    
    size_t index = buf - get_inspect_buf_vec_data(inspector->buffers);
    remove_inspect_buf_vec(inspector->buffers, index, 1);
}

static void apply_del_shdr(inspector_t* inspector, inspect_action_t* action) {
    inspect_shader_t* shdr = inspect_find_shdr_ptr(inspector, action->obj);
    if (!shdr)
        return;
    
    free(shdr->info_log);
    free(shdr->source);
    
    size_t index = shdr - get_inspect_shdr_vec_data(inspector->shaders);
    remove_inspect_shdr_vec(inspector->shaders, index, 1);
}

static void apply_new_prog(inspector_t* inspector, inspect_action_t* action) {
    inspect_program_t prog;
    prog.fake = action->obj;
    prog.shaders = alloc_vec(0);
    prog.info_log = NULL;
    append_inspect_prog_vec(inspector->programs, &prog);
}

static void apply_del_prog(inspector_t* inspector, inspect_action_t* action) {
    inspect_program_t* prog = inspect_find_prog_ptr(inspector, action->obj);
    if (!prog)
        return;
    
    free(prog->info_log);
    free_inspect_prog_shdr_vec(prog->shaders);
    
    size_t index = prog - get_inspect_prog_vec_data(inspector->programs);
    remove_inspect_prog_vec(inspector->programs, index, 1);
}

static void apply_link_prog(inspector_t* inspector, inspect_action_t* action) {
    inspect_program_t* prog = inspect_find_prog_ptr(inspector, action->obj);
    if (!prog)
        return;
    
    inspect_prog_shdr_vec_t shaders = prog->shaders;
    for (inspect_prog_shdr_t* attach = shaders->data; !vec_end(shaders, attach); attach++) {
        inspect_shader_t* shdr = inspect_find_shdr_ptr(inspector, attach->shader);
        if (!shdr)
            continue;
        
        attach->type = shdr->type;
        attach->source = malloc(strlen(shdr->source)+1);
        strcpy(attach->source, shdr->source);
    }
}

typedef struct {
    uint obj;
    uint type;
} new_shdr_t;

static void apply_new_shdr(inspector_t* inspector, inspect_action_t* action) {
    new_shdr_t* new_shdr = action->data;
    inspect_shader_t shdr;
    shdr.fake = new_shdr->obj;
    shdr.type = new_shdr->type;
    shdr.source = NULL;
    shdr.info_log = NULL;
    append_inspect_shdr_vec(inspector->shaders, &shdr);
}

typedef struct {
    uint prog;
    uint shdr;
} prog_tach_t;

static void apply_attach_shdr(inspector_t* inspector, inspect_action_t* action) {
    prog_tach_t* data = action->data;
    
    inspect_program_t* prog = inspect_find_prog_ptr(inspector, data->prog);
    if (!prog)
        return;
    
    inspect_prog_shdr_vec_t shaders = prog->shaders;
    for (inspect_prog_shdr_t* shdr = shaders->data; !vec_end(shaders, shdr); shdr++)
        if (shdr->shader == data->shdr) {
            shdr->type = 0;
            shdr->source = NULL;
            return;
        }
    
    inspect_prog_shdr_t shdr;
    shdr.shader = data->shdr;
    shdr.type = 0;
    shdr.source = NULL;
    
    append_inspect_prog_shdr_vec(prog->shaders, &shdr);
}

static void apply_detach_shdr(inspector_t* inspector, inspect_action_t* action) {
    prog_tach_t* data = action->data;
    
    inspect_program_t* prog = inspect_find_prog_ptr(inspector, data->prog);
    if (!prog)
        return;
    
    inspect_prog_shdr_vec_t shaders = prog->shaders;
    for (inspect_prog_shdr_t* shdr = shaders->data; !vec_end(shaders, shdr); shdr++)
        if (shdr->shader == data->shdr) {
            free(shdr->source);
            remove_inspect_prog_shdr_vec(shaders, shdr-(inspect_prog_shdr_t*)shaders->data, 1);
            return;
        }
}

typedef struct {
    uint buf;
    size_t size;
    uint usage;
} buf_data_t;

static void apply_set_buf_data(inspector_t* inspector, inspect_action_t* action) {
    buf_data_t* data = action->data;
    
    inspect_buffer_t* buf = inspect_find_buf_ptr(inspector, data->buf);
    if (!buf)
        return;
    if (buf->data)
        free(buf->data);
    
    buf->usage = data->usage;
    buf->size = data->size;
    buf->data = malloc(data->size);
    memcpy(buf->data, data+1, data->size);
}

typedef struct {
    uint buf;
    size_t offset;
    size_t size;
} buf_sub_data_t;

static void apply_set_buf_sub_data(inspector_t* inspector, inspect_action_t* action) {
    buf_sub_data_t* data = action->data;
    
    inspect_buffer_t* buf = inspect_find_buf_ptr(inspector, data->buf);
    if (!buf)
        return;
    if (!buf->data)
        return;
    if (data->offset+data->size > buf->size)
        return;
    
    memcpy((uint8_t*)buf->data+data->offset, data+1, data->size);
}

static void apply_tex_params(inspector_t* inspector, inspect_action_t* action) {
    inspect_gl_tex_params_t* params = action->data;
    
    inspect_texture_t* tex = inspect_find_tex_ptr(inspector, params->texture);
    if (!tex)
        return;
    
    tex->params = *params;
}

typedef struct {
     uint obj, type;
} tex_type_t;

static void apply_tex_type(inspector_t* inspector, inspect_action_t* action) {
    tex_type_t* data = action->data;
    
    inspect_texture_t* tex = inspect_find_tex_ptr(inspector, data->obj);
    if (!tex) return;
    tex->type = data->type;
}

typedef struct {
    uint obj;
    size_t mipmap;
    size_t layer;
    size_t face;
    inspect_image_t image;
} tex_data_t;

static void apply_tex_data(inspector_t* inspector, inspect_action_t* action) {
    tex_data_t* data = action->data;
    
    inspect_texture_t* tex = inspect_find_tex_ptr(inspector, data->obj);
    if (!tex) return;
    if (!tex->mipmaps) return;
    
    void* idata = malloc(data->image.width*data->image.height*4);
    inspect_get_image_data(&data->image, idata);
    
    inspect_replace_image(inspect_get_tex_mipmap(tex, data->mipmap, data->layer, data->face), data->image.width, data->image.height, idata);
    
    free(idata);
}

typedef struct {
    uint obj;
    size_t mipmaps, layers, width, height, depth;
} tex_alloc_t;

static void apply_tex_alloc(inspector_t* inspector, inspect_action_t* action) {
    tex_alloc_t* data = action->data;
    
    inspect_texture_t* tex = inspect_find_tex_ptr(inspector, data->obj);
    if (!tex) return;
    
    if (tex->mipmap_count!=data->mipmaps ||
        tex->layer_count!=data->layers ||
        tex->width!=data->width ||
        tex->height!=data->height ||
        tex->depth!=data->depth) {
        tex->mipmap_count = data->mipmaps;
        tex->layer_count = data->layers;
        tex->width = data->width;
        tex->height = data->height;
        tex->depth = data->depth;
        inspect_free_tex_mipmaps(tex);
        inspect_init_tex_mipmaps(tex);
    }
}

static void tex_data_free(inspect_action_t* action) {
    inspect_destroy_image(&((tex_data_t*)action->data)->image);
    free(action->data);
}

typedef struct {
    uint obj;
    size_t count;
} shdr_source_t;

static void apply_shdr_source(inspector_t* inspector, inspect_action_t* action) {
    shdr_source_t* data = action->data;
    char** sources = (char**)(data+1);
    
    inspect_shader_t* shdr = inspect_find_shdr_ptr(inspector, data->obj);
    if (!shdr)
        return;
    
    free(shdr->source);
    
    size_t lens[data->count];
    size_t len = 0;
    for (size_t i = 0; i < data->count; i++) {
        lens[i] = strlen(sources[i]);
        len += lens[i];
    }
    
    shdr->source = malloc(len+1);
    shdr->source[len] = 0;
    
    size_t offset = 0;
    for (size_t i = 0; i < data->count; i++) {
        memcpy(shdr->source+offset, sources[i], lens[i]);
        offset += lens[i];
    }
}

static void free_shdr_source(inspect_action_t* action) {
    shdr_source_t* data = action->data;
    char** sources = (char**)(data+1);
    
    for (size_t i = 0; i < data->count; i++)
        free(sources[i]);
    
    free(data);
}

typedef struct {
    uint obj;
} info_log_t;

static void apply_set_shdr_info_log(inspector_t* inspector, inspect_action_t* action) {
    info_log_t* data = action->data;
    
    inspect_shader_t* shdr = inspect_find_shdr_ptr(inspector, data->obj);
    if (!shdr)
        return;
    
    free(shdr->info_log);
    
    char* info_log = (char*)(data+1);
    
    size_t len = strlen(info_log);
    shdr->info_log = malloc(len+1);
    memcpy(shdr->info_log, info_log, len+1);
}

static void apply_set_prog_info_log(inspector_t* inspector, inspect_action_t* action) {
    info_log_t* data = action->data;
    
    inspect_program_t* prog = inspect_find_prog_ptr(inspector, data->obj);
    if (!prog)
        return;
    
    free(prog->info_log);
    
    char* info_log = (char*)(data+1);
    
    size_t len = strlen(info_log);
    prog->info_log = malloc(len+1);
    memcpy(prog->info_log, info_log, len+1);
}

static void apply_gen_vao(inspector_t* inspector, inspect_action_t* action) {
    inspect_vao_t vao;
    vao.fake = action->obj;
    vao.attrib_count = 0;
    vao.attribs = NULL;
    append_inspect_vao_vec(inspector->vaos, &vao);
}

static void apply_del_vao(inspector_t* inspector, inspect_action_t* action) {
    inspect_vao_t* vao = inspect_find_vao_ptr(inspector, action->obj);
    if (!vao)
        return;
    
    free(vao->attribs);
    
    size_t index = vao - get_inspect_vao_vec_data(inspector->vaos);
    remove_inspect_vao_vec(inspector->vaos, index, 1);
}

typedef struct {
    uint vao;
    size_t count;
} set_vao_t;

static void apply_set_vao(inspector_t* inspector, inspect_action_t* action) {
    set_vao_t* data = action->data;
    inspect_vertex_attrib_t* attribs = (inspect_vertex_attrib_t*)(data+1);
    
    inspect_vao_t* vao = inspect_find_vao_ptr(inspector, data->vao);
    if (!vao)
        return;
    
    free(vao->attribs);
    vao->attrib_count = data->count;
    vao->attribs = malloc(data->count*sizeof(inspect_vertex_attrib_t));
    memcpy(vao->attribs, attribs, data->count*sizeof(inspect_vertex_attrib_t));
}

static void apply_gen_fb(inspector_t* inspector, inspect_action_t* action) {
    inspect_fb_t fb;
    fb.fake = action->obj;
    fb.depth.tex = fb.stencil.tex = fb.depth_stencil.tex = 0;
    fb.color = alloc_inspect_fb_attach_vec(0);
    append_inspect_fb_vec(inspector->framebuffers, &fb);
}

static void apply_del_fb(inspector_t* inspector, inspect_action_t* action) {
    inspect_fb_t* fb = inspect_find_fb_ptr(inspector, action->obj);
    if (!fb)
        return;
    
    free_inspect_fb_attach_vec(fb->color);
    
    size_t index = fb - get_inspect_fb_vec_data(inspector->framebuffers);
    remove_inspect_fb_vec(inspector->framebuffers, index, 1);
}

typedef struct {
    uint fb;
    uint attach;
    uint tex;
    uint level;
} fb_attach_t;

static void apply_fb_attach(inspector_t* inspector, inspect_action_t* action) {
    fb_attach_t* data = action->data;
    inspect_fb_t* fb = inspect_find_fb_ptr(inspector, data->fb);
    if (!fb)
        return;
    
    switch (data->attach) {
    case 0: {
        fb->depth.tex = data->tex;
        fb->depth.level = data->level;
        break;
    }
    case 1: {
        fb->stencil.tex = data->tex;
        fb->stencil.level = data->level;
        break;
    }
    case 2: {
        fb->depth_stencil.tex = data->tex;
        fb->depth_stencil.level = data->level;
        break;
    }
    default: {
        inspect_fb_attach_vec_t atts = fb->color;
        for (inspect_fb_attach_t* att = atts->data; !vec_end(atts, att); att++) {
            if (att->attachment == data->attach-3) {
                att->tex = data->tex;
                att->level = data->level;
                break;
            }
        }
        
        inspect_fb_attach_t att;
        att.attachment = data->attach - 3;
        att.tex = data->tex;
        att.level = data->level;
        
        append_inspect_fb_attach_vec(atts, &att);
        break;
    }
    }
}

static void apply_gen_rb(inspector_t* inspector, inspect_action_t* action) {
    inspect_rb_t rb;
    memset(&rb, 0, sizeof(rb));
    rb.fake = action->obj;
    append_inspect_rb_vec(inspector->renderbuffers, &rb);
}

static void apply_del_rb(inspector_t* inspector, inspect_action_t* action) {
    inspect_rb_t* rb = inspect_find_rb_ptr(inspector, action->obj);
    if (!rb)
        return;
    
    size_t index = rb - get_inspect_rb_vec_data(inspector->renderbuffers);
    remove_inspect_rb_vec(inspector->renderbuffers, index, 1);
}

static void apply_set_rb(inspector_t* inspector, inspect_action_t* action) {
    inspect_rb_t* data = action->data;
    
    inspect_rb_t* rb = inspect_find_rb_ptr(inspector, data->fake);
    if (!rb)
        return;
    
    *rb = *data;
}

static void apply_gen_sync(inspector_t* inspector, inspect_action_t* action) {
    inspect_sync_t sync;
    memset(&sync, 0, sizeof(sync));
    sync.fake = action->obj;
    append_inspect_sync_vec(inspector->syncs, &sync);
}

static void apply_del_sync(inspector_t* inspector, inspect_action_t* action) {
    inspect_sync_t* sync = inspect_find_sync_ptr(inspector, action->obj);
    if (!sync)
        return;
    
    size_t index = sync - get_inspect_sync_vec_data(inspector->syncs);
    remove_inspect_sync_vec(inspector->syncs, index, 1);
}

static void apply_set_sync(inspector_t* inspector, inspect_action_t* action) {
    inspect_sync_t* data = action->data;
    
    inspect_sync_t* sync = inspect_find_sync_ptr(inspector, data->fake);
    if (!sync)
        return;
    
    *sync = *data;
}

static void apply_gen_query(inspector_t* inspector, inspect_action_t* action) {
    inspect_query_t query;
    memset(&query, 0, sizeof(query));
    query.fake = action->obj;
    append_inspect_query_vec(inspector->queries, &query);
}

static void apply_del_query(inspector_t* inspector, inspect_action_t* action) {
    inspect_query_t* query = inspect_find_query_ptr(inspector, action->obj);
    if (!query)
        return;
    
    size_t index = query - get_inspect_query_vec_data(inspector->queries);
    remove_inspect_query_vec(inspector->queries, index, 1);
}

static void apply_set_query(inspector_t* inspector, inspect_action_t* action) {
    inspect_query_t* data = action->data;
    
    inspect_query_t* query = inspect_find_query_ptr(inspector, data->fake);
    if (!query)
        return;
    
    *query = *data;
}

static void simple_free(inspect_action_t* action) {
    free(action->data);
}

void inspect_act_gen_tex(inspect_gl_state_t* state, uint id) {
    inspect_action_t action;
    action.apply_func = &apply_gen_tex;
    action.free_func = NULL;
    action.obj = id;
    append_inspect_act_vec(state->actions, &action);
}

void inspect_act_del_tex(inspect_gl_state_t* state, uint id) {
    inspect_action_t action;
    action.apply_func = &apply_del_tex;
    action.free_func = NULL;
    action.obj = id;
    append_inspect_act_vec(state->actions, &action);
}

void inspect_act_tex_params(inspect_gl_state_t* state, uint id, inspect_gl_tex_params_t* params) {
    inspect_gl_tex_params_t* data = malloc(sizeof(inspect_gl_tex_params_t));
    *data = *params;
    
    inspect_action_t action;
    action.apply_func = &apply_tex_params;
    action.free_func = &simple_free;
    action.data = data;
    append_inspect_act_vec(state->actions, &action);
}

void inspect_act_tex_type(inspect_gl_state_t* state, uint id, uint type) {
    tex_type_t* act_data = malloc(sizeof(tex_type_t));
    act_data->obj = id;
    act_data->type = type;
    
    inspect_action_t action;
    action.apply_func = &apply_tex_type;
    action.free_func = &simple_free;
    action.data = act_data;
    append_inspect_act_vec(state->actions, &action);
}

void inspect_act_tex_data(inspect_gl_state_t* state, uint id, size_t mipmap, size_t layer, size_t face, size_t w, size_t h, const void* data) {
    tex_data_t* act_data = malloc(sizeof(tex_data_t));
    act_data->obj = id;
    act_data->mipmap = mipmap;
    act_data->layer = layer;
    act_data->face = face;
    act_data->image.filename = NULL;
    inspect_replace_image(&act_data->image, w, h, data);
    
    inspect_action_t action;
    action.apply_func = &apply_tex_data;
    action.free_func = &tex_data_free;
    action.data = act_data;
    append_inspect_act_vec(state->actions, &action);
}

void inspect_act_alloc_tex(inspect_gl_state_t* state, uint id, size_t mipmaps, size_t layers, size_t width, size_t height, size_t depth) {
    tex_alloc_t* act_data = malloc(sizeof(tex_alloc_t));
    act_data->obj = id;
    act_data->mipmaps = mipmaps;
    act_data->layers = layers;
    act_data->width = width;
    act_data->height = height;
    act_data->depth = depth;
    
    inspect_action_t action;
    action.apply_func = &apply_tex_alloc;
    action.free_func = &simple_free;
    action.data = act_data;
    append_inspect_act_vec(state->actions, &action);
}

void inspect_act_gen_buf(inspect_gl_state_t* state, uint id) {
    inspect_action_t action;
    action.apply_func = &apply_gen_buf;
    action.free_func = NULL;
    action.obj = id;
    append_inspect_act_vec(state->actions, &action);
}

void inspect_act_del_buf(inspect_gl_state_t* state, uint id) {
    inspect_action_t action;
    action.apply_func = &apply_del_buf;
    action.free_func = NULL;
    action.obj = id;
    append_inspect_act_vec(state->actions, &action);
}

void inspect_act_buf_data(inspect_gl_state_t* state, uint id, size_t size, const void* data, uint usage) {
    buf_data_t* act_data = malloc(sizeof(buf_data_t) + size);
    act_data->buf = id;
    act_data->size = size;
    act_data->usage = usage;
    memcpy(act_data+1, data, size);
    
    inspect_action_t action;
    action.apply_func = &apply_set_buf_data;
    action.free_func = &simple_free;
    action.data = act_data;
    append_inspect_act_vec(state->actions, &action);
}

void inspect_act_buf_sub_data(inspect_gl_state_t* state, uint id, size_t offset, size_t size, const void* data) {
    buf_sub_data_t* act_data = malloc(sizeof(buf_sub_data_t) + size);
    act_data->buf = id;
    act_data->offset = offset;
    act_data->size = size;
    memcpy(act_data+1, data, size);
    
    inspect_action_t action;
    action.apply_func = &apply_set_buf_sub_data;
    action.free_func = &simple_free;
    action.data = act_data;
    append_inspect_act_vec(state->actions, &action);
}

void inspect_act_new_shdr(inspect_gl_state_t* state, uint id, uint type) {
    new_shdr_t* data = malloc(sizeof(new_shdr_t));
    data->obj = id;
    data->type = type;
    
    inspect_action_t action;
    action.apply_func = &apply_new_shdr;
    action.free_func = &simple_free;
    action.data = data;
    append_inspect_act_vec(state->actions, &action);
}

void inspect_act_del_shdr(inspect_gl_state_t* state, uint id) {
    inspect_action_t action;
    action.apply_func = &apply_del_shdr;
    action.free_func = NULL;
    action.obj = id;
    append_inspect_act_vec(state->actions, &action);
}

void inspect_act_shdr_source(inspect_gl_state_t* state, uint id, size_t count, const char *const* sources) {
    shdr_source_t* data = malloc(sizeof(shdr_source_t) + count*sizeof(char*));
    data->obj = id;
    data->count = count;
    char** new_sources = (char**)(data+1);
    
    for (size_t i = 0; i < count; i++) {
        size_t len = strlen(sources[i]);
        new_sources[i] = malloc(len+1);
        memcpy(new_sources[i], sources[i], len+1);
    }
    
    inspect_action_t action;
    action.apply_func = &apply_shdr_source;
    action.free_func = &free_shdr_source;
    action.data = data;
    append_inspect_act_vec(state->actions, &action);
}

void inspect_act_new_prog(inspect_gl_state_t* state, uint id) {
    inspect_action_t action;
    action.apply_func = &apply_new_prog;
    action.free_func = NULL;
    action.obj = id;
    append_inspect_act_vec(state->actions, &action);
}

void inspect_act_del_prog(inspect_gl_state_t* state, uint id) {
    inspect_action_t action;
    action.apply_func = &apply_del_prog;
    action.free_func = NULL;
    action.obj = id;
    append_inspect_act_vec(state->actions, &action);
}

void inspect_act_link_prog(inspect_gl_state_t* state, uint id) {
    inspect_action_t action;
    action.apply_func = &apply_link_prog;
    action.free_func = NULL;
    action.obj = id;
    append_inspect_act_vec(state->actions, &action);
}

void inspect_act_attach_shdr(inspect_gl_state_t* state, uint prog, uint shdr) {
    prog_tach_t* data = malloc(sizeof(prog_tach_t));
    data->prog = prog;
    data->shdr = shdr;
    
    inspect_action_t action;
    action.apply_func = &apply_attach_shdr;
    action.free_func = &simple_free;
    action.data = data;
    append_inspect_act_vec(state->actions, &action);
}

void inspect_act_detach_shdr(inspect_gl_state_t* state, uint prog, uint shdr) {
    prog_tach_t* data = malloc(sizeof(prog_tach_t));
    data->prog = prog;
    data->shdr = shdr;
    
    inspect_action_t action;
    action.apply_func = &apply_detach_shdr;
    action.free_func = &simple_free;
    action.data = data;
    append_inspect_act_vec(state->actions, &action);
}

void inspect_act_set_shdr_info_log(inspect_gl_state_t* state, uint id, const char* info_log) {
    info_log_t* data = malloc(sizeof(info_log_t) + strlen(info_log)+1);
    data->obj = id;
    memcpy(data+1, info_log, strlen(info_log)+1);
    
    inspect_action_t action;
    action.apply_func = &apply_set_shdr_info_log;
    action.free_func = &simple_free;
    action.data = data;
    append_inspect_act_vec(state->actions, &action);
}

void inspect_act_set_prog_info_log(inspect_gl_state_t* state, uint id, const char* info_log) {
    info_log_t* data = malloc(sizeof(info_log_t) + strlen(info_log)+1);
    data->obj = id;
    memcpy(data+1, info_log, strlen(info_log)+1);
    
    inspect_action_t action;
    action.apply_func = &apply_set_prog_info_log;
    action.free_func = &simple_free;
    action.data = data;
    append_inspect_act_vec(state->actions, &action);
}

void inspect_act_gen_vao(inspect_gl_state_t* state, uint id) {
    inspect_action_t action;
    action.apply_func = &apply_gen_vao;
    action.free_func = NULL;
    action.obj = id;
    append_inspect_act_vec(state->actions, &action);
}

void inspect_act_del_vao(inspect_gl_state_t* state, uint id) {
    inspect_action_t action;
    action.apply_func = &apply_del_vao;
    action.free_func = NULL;
    action.obj = id;
    append_inspect_act_vec(state->actions, &action);
}

void inspect_act_set_vao(inspect_gl_state_t* state, uint id, size_t count, inspect_vertex_attrib_t* attribs) {
    set_vao_t* act_data = malloc(sizeof(set_vao_t) + count*sizeof(inspect_vertex_attrib_t));
    act_data->vao = id;
    act_data->count = count;
    memcpy(act_data+1, attribs, count*sizeof(inspect_vertex_attrib_t));
    
    inspect_action_t action;
    action.apply_func = &apply_set_vao;
    action.free_func = &simple_free;
    action.data = act_data;
    append_inspect_act_vec(state->actions, &action);
}

void inspect_act_gen_fb(inspect_gl_state_t* state, uint id) {
    inspect_action_t action;
    action.apply_func = &apply_gen_fb;
    action.free_func = NULL;
    action.obj = id;
    append_inspect_act_vec(state->actions, &action);
}

void inspect_act_del_fb(inspect_gl_state_t* state, uint id) {
    inspect_action_t action;
    action.apply_func = &apply_del_fb;
    action.free_func = NULL;
    action.obj = id;
    append_inspect_act_vec(state->actions, &action);
}

static void fb_attach(inspect_gl_state_t* state, uint fb, uint attach, uint tex, uint level) {
    fb_attach_t* act_data = malloc(sizeof(fb_attach_t));
    act_data->fb = fb;
    act_data->attach = attach;
    act_data->tex = tex;
    act_data->level = level;
    
    inspect_action_t action;
    action.apply_func = &apply_fb_attach;
    action.free_func = &simple_free;
    action.data = act_data;
    append_inspect_act_vec(state->actions, &action);
}

void inspect_act_fb_depth(inspect_gl_state_t* state, uint fb, uint tex, uint level) {
    fb_attach(state, fb, 0, tex, level);
}

void inspect_act_fb_stencil(inspect_gl_state_t* state, uint fb, uint tex, uint level) {
    fb_attach(state, fb, 1, tex, level);
}

void inspect_act_fb_depth_stencil(inspect_gl_state_t* state, uint fb, uint tex, uint level) {
    fb_attach(state, fb, 2, tex, level);
}

void inspect_act_fb_color(inspect_gl_state_t* state, uint fb, uint attachment, uint tex, uint level) {
    fb_attach(state, fb, attachment+3, tex, level);
}

void inspect_act_gen_rb(inspect_gl_state_t* state, uint id) {
    inspect_action_t action;
    action.apply_func = &apply_gen_rb;
    action.free_func = NULL;
    action.obj = id;
    append_inspect_act_vec(state->actions, &action);
}

void inspect_act_del_rb(inspect_gl_state_t* state, uint id) {
    inspect_action_t action;
    action.apply_func = &apply_del_rb;
    action.free_func = NULL;
    action.obj = id;
    append_inspect_act_vec(state->actions, &action);
}

void inspect_act_set_rb(inspect_gl_state_t* state, inspect_rb_t* data) {
    inspect_action_t action;
    action.apply_func = &apply_set_rb;
    action.free_func = &simple_free;
    action.data = malloc(sizeof(inspect_rb_t));
    *(inspect_rb_t*)action.data = *data;
    append_inspect_act_vec(state->actions, &action);
}

void inspect_act_gen_sync(inspect_gl_state_t* state, uint64_t id) {
    inspect_action_t action;
    action.apply_func = &apply_gen_sync;
    action.free_func = NULL;
    action.obj = id;
    append_inspect_act_vec(state->actions, &action);
}

void inspect_act_del_sync(inspect_gl_state_t* state, uint64_t id) {
    inspect_action_t action;
    action.apply_func = &apply_del_sync;
    action.free_func = NULL;
    action.obj = id;
    append_inspect_act_vec(state->actions, &action);
}

void inspect_act_set_sync(inspect_gl_state_t* state, inspect_sync_t* data) {
    inspect_action_t action;
    action.apply_func = &apply_set_sync;
    action.free_func = &simple_free;
    action.data = malloc(sizeof(inspect_sync_t));
    *(inspect_sync_t*)action.data = *data;
    append_inspect_act_vec(state->actions, &action);
}

void inspect_act_gen_query(inspect_gl_state_t* state, uint id) {
    inspect_action_t action;
    action.apply_func = &apply_gen_query;
    action.free_func = NULL;
    action.obj = id;
    append_inspect_act_vec(state->actions, &action);
}

void inspect_act_del_query(inspect_gl_state_t* state, uint id) {
    inspect_action_t action;
    action.apply_func = &apply_del_query;
    action.free_func = NULL;
    action.obj = id;
    append_inspect_act_vec(state->actions, &action);
}

void inspect_act_set_query(inspect_gl_state_t* state, inspect_query_t* data) {
    inspect_action_t action;
    action.apply_func = &apply_set_query;
    action.free_func = &simple_free;
    action.data = malloc(sizeof(inspect_query_t));
    *(inspect_query_t*)action.data = *data;
    append_inspect_act_vec(state->actions, &action);
}
