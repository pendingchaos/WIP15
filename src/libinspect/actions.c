#include "libinspect/actions.h"

#include <stdlib.h>
#include <string.h>

static void apply_gen_tex(inspector_t* inspector, inspect_action_t* action) {
    inspect_texture_t tex;
    tex.fake = action->obj;
    tex.params.texture = action->obj;
    tex.params.width = 0;
    tex.params.height = 0;
    tex.params.type = 0;
    tex.mipmap_count = 0;
    tex.mipmaps = NULL;
    append_inspect_tex_vec(inspector->textures, &tex);
}

static void apply_del_tex(inspector_t* inspector, inspect_action_t* action) {
    inspect_texture_t* tex = inspect_find_tex_ptr(inspector, action->obj);
    if (!tex)
        return;
    
    for (size_t j = 0; j < tex->mipmap_count; ++j)
        free(tex->mipmaps[j]);
    free(tex->mipmaps);
    
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
    free_vec(prog->shaders);
    
    size_t index = prog - get_inspect_prog_vec_data(inspector->programs);
    remove_inspect_prog_vec(inspector->programs, index, 1);
}

typedef struct {
    unsigned int obj;
    unsigned int type;
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
    unsigned int prog;
    unsigned int shdr;
} prog_tach_t;

static void apply_attach_shdr(inspector_t* inspector, inspect_action_t* action) {
    prog_tach_t* data = action->data;
    
    inspect_program_t* prog = inspect_find_prog_ptr(inspector, data->prog);
    if (!prog)
        return;
    
    size_t count = prog->shaders->size/sizeof(unsigned int);
    for (size_t i = 0; i < count; i++)
        if (((unsigned int *)prog->shaders->data)[i] == data->shdr)
            goto end;
    
    append_vec(prog->shaders, sizeof(unsigned int), &data->shdr);
    
    end:;
}

static void apply_detach_shdr(inspector_t* inspector, inspect_action_t* action) {
    prog_tach_t* data = action->data;
    
    inspect_program_t* prog = inspect_find_prog_ptr(inspector, data->prog);
    if (!prog)
        return;
    
    size_t count = prog->shaders->size/sizeof(unsigned int);
    for (size_t i = 0; i < count; i++)
        if (((unsigned int *)prog->shaders->data)[i] == data->shdr) {
            remove_vec(prog->shaders, i*sizeof(unsigned int), sizeof(unsigned int));
            return;
        }
}

typedef struct {
    unsigned int buf;
    size_t size;
    unsigned int usage;
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
    unsigned int buf;
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
    
    if (tex->params.width != params->width || tex->params.height != params->height) {
        size_t mipmap_count = 1;
        size_t w = params->width;
        size_t h = params->height;
        while ((w > 1) && (h > 1)) {
            ++mipmap_count;
            w /= 2;
            h /= 2;
        }
        
        for (size_t j = 0; j < tex->mipmap_count; ++j)
            free(tex->mipmaps[j]);
        free(tex->mipmaps);
        
        tex->mipmap_count = mipmap_count;
        tex->mipmaps = malloc(tex->mipmap_count*sizeof(void*));
        for (size_t j = 0; j < mipmap_count; ++j)
            tex->mipmaps[j] = NULL;
    }
    
    tex->params = *params;
}

typedef struct {
    unsigned int obj;
    size_t mipmap;
    size_t data_size;
} tex_data_t;

static void apply_tex_data(inspector_t* inspector, inspect_action_t* action) {
    tex_data_t* data = action->data;
    
    inspect_texture_t* tex = inspect_find_tex_ptr(inspector, data->obj);
    if (!tex)
        return;
    if (!tex->mipmaps[data->mipmap])
        tex->mipmaps[data->mipmap] = malloc(data->data_size);
    
    memcpy(tex->mipmaps[data->mipmap], data+1, data->data_size);
}

typedef struct {
    unsigned int obj;
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
    unsigned int obj;
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

static void simple_free(inspect_action_t* action) {
    free(action->data);
}

void inspect_act_gen_tex(inspect_gl_state_t* state, unsigned int id) {
    inspect_action_t action;
    action.apply_func = &apply_gen_tex;
    action.free_func = NULL;
    action.obj = id;
    append_inspect_act_vec(state->actions, &action);
}

void inspect_act_del_tex(inspect_gl_state_t* state, unsigned int id) {
    inspect_action_t action;
    action.apply_func = &apply_del_tex;
    action.free_func = NULL;
    action.obj = id;
    append_inspect_act_vec(state->actions, &action);
}

void inspect_act_tex_params(inspect_gl_state_t* state, unsigned int id, inspect_gl_tex_params_t* params) {
    inspect_gl_tex_params_t* data = malloc(sizeof(inspect_gl_tex_params_t));
    *data = *params;
    
    inspect_action_t action;
    action.apply_func = &apply_tex_params;
    action.free_func = &simple_free;
    action.data = data;
    append_inspect_act_vec(state->actions, &action);
}

void inspect_act_tex_data(inspect_gl_state_t* state, unsigned int id, size_t mipmap, size_t size, const void* data) {
    tex_data_t* act_data = malloc(sizeof(tex_data_t) + size);
    act_data->obj = id;
    act_data->mipmap = mipmap;
    act_data->data_size = size;
    memcpy(act_data+1, data, size);
    
    inspect_action_t action;
    action.apply_func = &apply_tex_data;
    action.free_func = &simple_free;
    action.data = act_data;
    append_inspect_act_vec(state->actions, &action);
}

void inspect_act_gen_buf(inspect_gl_state_t* state, unsigned int id) {
    inspect_action_t action;
    action.apply_func = &apply_gen_buf;
    action.free_func = NULL;
    action.obj = id;
    append_inspect_act_vec(state->actions, &action);
}

void inspect_act_del_buf(inspect_gl_state_t* state, unsigned int id) {
    inspect_action_t action;
    action.apply_func = &apply_del_buf;
    action.free_func = NULL;
    action.obj = id;
    append_inspect_act_vec(state->actions, &action);
}

void inspect_act_buf_data(inspect_gl_state_t* state, unsigned int id, size_t size, const void* data, unsigned int usage) {
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

void inspect_act_buf_sub_data(inspect_gl_state_t* state, unsigned int id, size_t offset, size_t size, const void* data) {
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

void inspect_act_new_shdr(inspect_gl_state_t* state, unsigned int id, unsigned int type) {
    new_shdr_t* data = malloc(sizeof(new_shdr_t));
    data->obj = id;
    data->type = type;
    
    inspect_action_t action;
    action.apply_func = &apply_new_shdr;
    action.free_func = &simple_free;
    action.data = data;
    append_inspect_act_vec(state->actions, &action);
}

void inspect_act_del_shdr(inspect_gl_state_t* state, unsigned int id) {
    inspect_action_t action;
    action.apply_func = &apply_del_shdr;
    action.free_func = NULL;
    action.obj = id;
    append_inspect_act_vec(state->actions, &action);
}

void inspect_act_shdr_source(inspect_gl_state_t* state, unsigned int id, size_t count, const char *const* sources) {
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

void inspect_act_new_prog(inspect_gl_state_t* state, unsigned int id) {
    inspect_action_t action;
    action.apply_func = &apply_new_prog;
    action.free_func = NULL;
    action.obj = id;
    append_inspect_act_vec(state->actions, &action);
}

void inspect_act_del_prog(inspect_gl_state_t* state, unsigned int id) {
    inspect_action_t action;
    action.apply_func = &apply_del_prog;
    action.free_func = NULL;
    action.obj = id;
    append_inspect_act_vec(state->actions, &action);
}

void inspect_act_attach_shdr(inspect_gl_state_t* state, unsigned int prog, unsigned int shdr) {
    prog_tach_t* data = malloc(sizeof(prog_tach_t));
    data->prog = prog;
    data->shdr = shdr;
    
    inspect_action_t action;
    action.apply_func = &apply_attach_shdr;
    action.free_func = &simple_free;
    action.data = data;
    append_inspect_act_vec(state->actions, &action);
}

void inspect_act_detach_shdr(inspect_gl_state_t* state, unsigned int prog, unsigned int shdr) {
    prog_tach_t* data = malloc(sizeof(prog_tach_t));
    data->prog = prog;
    data->shdr = shdr;
    
    inspect_action_t action;
    action.apply_func = &apply_detach_shdr;
    action.free_func = &simple_free;
    action.data = data;
    append_inspect_act_vec(state->actions, &action);
}

void inspect_act_set_shdr_info_log(inspect_gl_state_t* state, unsigned int id, const char* info_log) {
    info_log_t* data = malloc(sizeof(info_log_t) + strlen(info_log)+1);
    data->obj = id;
    memcpy(data+1, info_log, strlen(info_log)+1);
    
    inspect_action_t action;
    action.apply_func = &apply_set_shdr_info_log;
    action.free_func = &simple_free;
    action.data = data;
    append_inspect_act_vec(state->actions, &action);
}

void inspect_act_set_prog_info_log(inspect_gl_state_t* state, unsigned int id, const char* info_log) {
    info_log_t* data = malloc(sizeof(info_log_t) + strlen(info_log)+1);
    data->obj = id;
    memcpy(data+1, info_log, strlen(info_log)+1);
    
    inspect_action_t action;
    action.apply_func = &apply_set_prog_info_log;
    action.free_func = &simple_free;
    action.data = data;
    append_inspect_act_vec(state->actions, &action);
}
