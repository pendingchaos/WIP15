#include "libinspect/replay.h"
#include "shared/uint.h"

#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <dlfcn.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <SDL2/SDL.h>

void init_replay_gl(replay_context_t* ctx);
void deinit_replay_gl(replay_context_t* ctx);

typedef struct {
    uint real;
    uint fake;
} uniform_t;
TYPED_VEC(uniform_t, uni)

typedef struct {
    uint real;
    uint fake;
} attrib_t;
TYPED_VEC(attrib_t, attrib)

typedef struct {
    uni_vec_t uniforms;
    uni_vec_t attribs;
} program_data_t;

typedef struct {
    uint attachment;
    uint tex;
    size_t level;
} fb_attach_t;
TYPED_VEC(fb_attach_t, attach)

typedef struct {
    attach_vec_t attachments;
    size_t depth_level;
    uint depth_texture;
    size_t stencil_level;
    uint stencil_texture;
    size_t depth_stencil_level;
    unsigned int depth_stencil_texture;
} fb_data_t;

typedef struct {
    unsigned int type;
} tex_data_t;

typedef struct replay_obj_t {
    uint64_t real;
    uint64_t fake;
    
    union {
        program_data_t prog;
        fb_data_t fb;
        tex_data_t tex;
    };
} replay_obj_t;
TYPED_VEC(replay_obj_t, obj)

typedef struct {
    obj_vec_t objects[ReplayObjType_Max];
} replay_internal_t;

replay_context_t* create_replay_context(inspection_t* inspection) {
    replay_context_t* ctx = malloc(sizeof(replay_context_t));
    replay_internal_t* internal = malloc(sizeof(replay_internal_t));
    
    ctx->inspection = inspection;
    ctx->_current_context = NULL;
    
    for (size_t i = 0; i < ReplayObjType_Max; ++i)
        internal->objects[i] = alloc_obj_vec(0);
    ctx->_internal = internal;
    
    init_replay_gl(ctx);
    
    const trace_t* trace = inspection->trace;
    
    ctx->funcs = malloc(trace->func_name_count * sizeof(replay_func_t));
    
    for (size_t i = 0; i < trace->func_name_count; ++i) {
        if (!trace->func_names[i] || !strlen(trace->func_names[i]))
            continue;
        
        char name[strlen(trace->func_names[i])+8];
        strcpy(name, "replay_");
        strcat(name, trace->func_names[i]);
        ctx->funcs[i] = dlsym(RTLD_DEFAULT, name);
        if (!ctx->funcs[i]) {
            fprintf(stderr, "Unable to find \"%s\".\n", name); //TODO: Handle
            fflush(stderr);
        }
    }
    
    ctx->current_test_name = "Unnamed";
    
    return ctx;
}

void free_obj(replay_obj_type_t type, replay_obj_t* obj) {
    switch (type) {
    case ReplayObjType_GLProgram:
        free_uni_vec(obj->prog.uniforms);
        free_attrib_vec(obj->prog.attribs);
        break;
    case ReplayObjType_GLFramebuffer:
        free_attach_vec(obj->fb.attachments);
        break;
    default:
        break;
    }
}

void destroy_replay_context(replay_context_t* context) {
    free(context->funcs);
    
    deinit_replay_gl(context);
    
    replay_internal_t* internal = context->_internal;
    for (size_t i = 0; i < ReplayObjType_Max; ++i) {
        obj_vec_t objs = internal->objects[i];
        for (replay_obj_t* obj = objs->data; !vec_end(objs, obj); obj++)
            free_obj(i, obj);
        free_obj_vec(objs);
    }
    free(internal);
    
    free(context);
}

uint64_t replay_get_real_object(replay_context_t* ctx, replay_obj_type_t type, uint64_t fake) {
    replay_internal_t* internal = ctx->_internal;
    
    obj_vec_t objs = internal->objects[type];
    for (replay_obj_t* obj = objs->data; !vec_end(objs, obj); obj++)
        if (obj->fake == fake)
            return obj->real;
    
    return 0;
}

uint64_t replay_get_fake_object(replay_context_t* ctx, replay_obj_type_t type, uint64_t real) {
    replay_internal_t* internal = ctx->_internal;
    
    obj_vec_t objs = internal->objects[type];
    for (replay_obj_t* obj = objs->data; !vec_end(objs, obj); obj++)
        if (obj->real == real)
            return obj->fake;
    
    return 0;
}

void replay_create_object(replay_context_t* ctx, replay_obj_type_t type, uint64_t real, uint64_t fake) {
    replay_internal_t* internal = ctx->_internal;
    
    obj_vec_t objs = internal->objects[type];
    
    replay_obj_t new_obj;
    new_obj.real = real;
    new_obj.fake = fake;
    
    switch (type) {
    case ReplayObjType_GLProgram:
        new_obj.prog.uniforms = alloc_uni_vec(0);
        new_obj.prog.attribs = alloc_attrib_vec(0);
        break;
    case ReplayObjType_GLFramebuffer:
        new_obj.fb.attachments = alloc_attach_vec(0);
        new_obj.fb.depth_level = new_obj.fb.stencil_level =
        new_obj.fb.depth_stencil_level = new_obj.fb.depth_texture =
        new_obj.fb.stencil_texture = new_obj.fb.depth_stencil_texture = 0;
        break;
    case ReplayObjType_GLTexture:
        new_obj.tex.type = 0;
        break;
    default:
        break;
    }
    
    append_obj_vec(objs, &new_obj);
}

void replay_destroy_object(replay_context_t* ctx, replay_obj_type_t type, uint64_t fake) {
    replay_internal_t* internal = ctx->_internal;
    
    obj_vec_t objs = internal->objects[type];
    for (replay_obj_t* obj = objs->data; !vec_end(objs, obj); obj++)
        if (obj->fake == fake) {
            free_obj(type, obj);
            remove_obj_vec(objs, obj-(replay_obj_t*)objs->data, 1);
            return;
        }
}

size_t replay_get_obj_count(replay_context_t* ctx, replay_obj_type_t type) {
    replay_internal_t* internal = ctx->_internal;
    
    return get_obj_vec_count(internal->objects[type]);
}

void replay_list_real_objects(replay_context_t* ctx, replay_obj_type_t type, uint64_t* real) {
    replay_internal_t* internal = ctx->_internal;
    
    obj_vec_t objs = internal->objects[type];
    for (replay_obj_t* obj = objs->data; !vec_end(objs, obj); obj++)
        real[obj-(replay_obj_t*)objs->data] = obj->real;
}

void replay_list_fake_objects(replay_context_t* ctx, replay_obj_type_t type, uint64_t* fake) {
    replay_internal_t* internal = ctx->_internal;
    
    obj_vec_t objs = internal->objects[type];
    for (replay_obj_t* obj = objs->data; !vec_end(objs, obj); obj++)
        fake[obj-(replay_obj_t*)objs->data] = obj->fake;
}

void replay(replay_context_t* ctx) {
    bool sdl_was_init = SDL_WasInit(SDL_INIT_VIDEO);
    if (!sdl_was_init)
        SDL_Init(SDL_INIT_VIDEO);
    
    ctx->window = SDL_CreateWindow("",
                                   SDL_WINDOWPOS_UNDEFINED,
                                   SDL_WINDOWPOS_UNDEFINED,
                                   100,
                                   100,
                                   SDL_WINDOW_SHOWN | //TODO: For some reason SDL_WINDOW_HIDDEN messes up the viewport and framebuffer display
                                   SDL_WINDOW_OPENGL);
    if (!ctx->window) {
        fprintf(stderr, "Unable to create a window: %sn", SDL_GetError()); //TODO: Handle
        fflush(stderr);
    }
    
    for (size_t i = 0; i < ctx->inspection->frame_count; ++i) {
        inspect_frame_t* frame = ctx->inspection->frames + i;
        for (size_t j = 0; j < frame->command_count; ++j) {
            inspect_command_t* command = frame->commands + j;
            ctx->funcs[command->trace_cmd->func_index](ctx, command->trace_cmd, command);
        }
    }
    
    SDL_DestroyWindow(ctx->window);
    
    if (!sdl_was_init)
        SDL_Quit();
}

int replay_conv_uniform_location(replay_context_t* ctx, uint64_t fake_prog, uint fake_loc) {
    replay_internal_t* internal = ctx->_internal;
    
    obj_vec_t objs = internal->objects[ReplayObjType_GLProgram];
    for (replay_obj_t* obj = objs->data; !vec_end(objs, obj); obj++)
        if (obj->fake == fake_prog) {
            uni_vec_t uniforms = obj->prog.uniforms;
            for (uniform_t* uni = uniforms->data; !vec_end(uniforms, uni); uni++)
                if (uni->fake == fake_loc)
                    return uni->real;
            
            return -1;
        }
    
    return -1;
}

void replay_add_uniform(replay_context_t* ctx, uint64_t fake_prog, uint fake, uint real) {
    replay_internal_t* internal = ctx->_internal;
    
    obj_vec_t objs = internal->objects[ReplayObjType_GLProgram];
    for (replay_obj_t* obj = objs->data; !vec_end(objs, obj); obj++)
        if (obj->fake == fake_prog) {
            uniform_t uni;
            uni.fake = fake;
            uni.real = real;
            append_uni_vec(obj->prog.uniforms, &uni);
            return;
        }
}

int replay_conv_attrib_index(replay_context_t* ctx, uint64_t fake_prog, uint fake_idx) {
    replay_internal_t* internal = ctx->_internal;
    
    obj_vec_t objs = internal->objects[ReplayObjType_GLProgram];
    for (replay_obj_t* obj = objs->data; !vec_end(objs, obj); obj++)
        if (obj->fake == fake_prog) {
            attrib_vec_t attribs = obj->prog.attribs;
            for (attrib_t* attrib = attribs->data; !vec_end(attribs, attrib); attrib++)
                if (attrib->fake == fake_idx)
                    return attrib->real;
            
            return -1;
        }
    
    return -1;
}

void replay_add_attrib(replay_context_t* ctx, uint64_t fake_prog, uint fake, uint real) {
    replay_internal_t* internal = ctx->_internal;
    
    obj_vec_t objs = internal->objects[ReplayObjType_GLProgram];
    for (replay_obj_t* obj = objs->data; !vec_end(objs, obj); obj++)
        if (obj->fake == fake_prog) {
            uniform_t uni;
            uni.fake = fake;
            uni.real = real;
            append_uni_vec(obj->prog.attribs, &uni);
            return;
        }
}

static fb_data_t* find_fb(replay_context_t* ctx, uint64_t fake_fb) {
    replay_internal_t* internal = ctx->_internal;
    obj_vec_t objs = internal->objects[ReplayObjType_GLFramebuffer];
    for (replay_obj_t* obj = objs->data; !vec_end(objs, obj); obj++)
        if (obj->fake == fake_fb)
            return &obj->fb;
    return NULL;
}

void replay_set_depth_tex(replay_context_t* ctx, uint64_t fake_fb, uint64_t fake_tex, size_t level) {
    fb_data_t* data = find_fb(ctx, fake_fb);
    if (data) {
        data->depth_texture = fake_tex;
        data->depth_level = level;
    }
}

void replay_set_stencil_tex(replay_context_t* ctx, uint64_t fake_fb, uint64_t fake_tex, size_t level) {
    fb_data_t* data = find_fb(ctx, fake_fb);
    if (data) {
        data->stencil_texture = fake_tex;
        data->stencil_level = level;
    }
}

void replay_set_depth_stencil_tex(replay_context_t* ctx, uint64_t fake_fb, uint64_t fake_tex, size_t level) {
    fb_data_t* data = find_fb(ctx, fake_fb);
    if (data) {
        data->depth_stencil_texture = fake_tex;
        data->depth_stencil_level = level;
    }
}

void replay_set_color_tex(replay_context_t* ctx, uint64_t fake_fb, unsigned int attachment, uint64_t fake_tex, size_t level) {
    fb_data_t* data = find_fb(ctx, fake_fb);
    if (data) {
        attach_vec_t v = data->attachments;
        for (fb_attach_t* attach = v->data; !vec_end(v, attach); attach++)
            if (attach->attachment == attachment) {
                attach->tex = fake_tex;
                attach->level = level;
                return;
            }
        
        fb_attach_t attach;
        attach.attachment = attachment;
        attach.tex = fake_tex;
        attach.level = level;
        append_attach_vec(v, &attach);
    }
}

uint64_t replay_get_depth_tex(replay_context_t* ctx, uint64_t fake_fb) {
    fb_data_t* data = find_fb(ctx, fake_fb);
    if (data)
        return data->depth_texture;
    return 0;
}

size_t replay_get_depth_level(replay_context_t* ctx, uint64_t fake_fb) {
    fb_data_t* data = find_fb(ctx, fake_fb);
    if (data)
        return data->depth_level;
    return 0;
}

uint64_t replay_get_stencil_tex(replay_context_t* ctx, uint64_t fake_fb) {
    fb_data_t* data = find_fb(ctx, fake_fb);
    if (data)
        return data->stencil_texture;
    return 0;
}

size_t replay_get_stencil_level(replay_context_t* ctx, uint64_t fake_fb) {
    fb_data_t* data = find_fb(ctx, fake_fb);
    if (data)
        return data->stencil_level;
    return 0;
}

uint64_t replay_get_depth_stencil_tex(replay_context_t* ctx, uint64_t fake_fb) {
    fb_data_t* data = find_fb(ctx, fake_fb);
    if (data)
        return data->depth_stencil_texture;
    return 0;
}

size_t replay_get_depth_stencil_level(replay_context_t* ctx, uint64_t fake_fb) {
    fb_data_t* data = find_fb(ctx, fake_fb);
    if (data)
        return data->depth_stencil_level;
    return 0;
}

size_t replay_get_color_tex_count(replay_context_t* ctx, uint64_t fake_fb) {
    fb_data_t* data = find_fb(ctx, fake_fb);
    if (data)
        return get_attach_vec_count(data->attachments);
    return 0;
}

uint64_t replay_get_color_tex(replay_context_t* ctx, uint64_t fake_fb, size_t index) {
    fb_data_t* data = find_fb(ctx, fake_fb);
    if (data)
        return get_attach_vec(data->attachments, index)->tex;
    return 0;
}

size_t replay_get_color_level(replay_context_t* ctx, uint64_t fake_fb, size_t index) {
    fb_data_t* data = find_fb(ctx, fake_fb);
    if (data)
        return get_attach_vec(data->attachments, index)->level;
    return 0;
}

uint replay_get_color_attach(replay_context_t* ctx, uint64_t fake_fb, size_t index) {
    fb_data_t* data = find_fb(ctx, fake_fb);
    if (data)
        return get_attach_vec(data->attachments, index)->attachment;
    return 0;
}

void replay_set_tex_type(replay_context_t* ctx, uint64_t fake_tex, unsigned int type) {
    replay_internal_t* internal = ctx->_internal;
    obj_vec_t objs = internal->objects[ReplayObjType_GLTexture];
    for (replay_obj_t* obj = objs->data; !vec_end(objs, obj); obj++)
        if (obj->fake == fake_tex) obj->tex.type = type;
}

unsigned int replay_get_tex_type(replay_context_t* ctx, uint64_t fake_tex) {
    replay_internal_t* internal = ctx->_internal;
    obj_vec_t objs = internal->objects[ReplayObjType_GLTexture];
    for (replay_obj_t* obj = objs->data; !vec_end(objs, obj); obj++)
        if (obj->fake == fake_tex) return obj->tex.type;
    return 0;
}
