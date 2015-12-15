#include "libinspect/replay.h"

#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <dlfcn.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <GL/glx.h>

void init_replay_gl(replay_context_t* ctx);
void deinit_replay_gl(replay_context_t* ctx);

typedef struct {
    unsigned int real;
    unsigned int fake;
} uniform_t;
TYPED_VEC(uniform_t, uni)

typedef struct {
    unsigned int real;
    unsigned int fake;
} attrib_t;
TYPED_VEC(attrib_t, attrib)

typedef struct {
    uni_vec_t uniforms;
    uni_vec_t attribs;
} program_data_t;

typedef struct replay_obj_t {
    uint64_t real;
    uint64_t fake;
    
    program_data_t prog;
    
    struct replay_obj_t* prev;
    struct replay_obj_t* next;
} replay_obj_t;

typedef struct {
    replay_obj_t *objects[22];
    Window window;
} replay_internal_t;

replay_context_t* create_replay_context(inspection_t* inspection) {
    replay_context_t* ctx = malloc(sizeof(replay_context_t));
    replay_internal_t* internal = malloc(sizeof(replay_internal_t));
    
    ctx->inspection = inspection;
    ctx->_current_context = NULL;
    
    for (size_t i = 0; i < 22; ++i)
        internal->objects[i] = NULL;
    ctx->_internal = internal;
    
    init_replay_gl(ctx);
     
    ctx->_display = XOpenDisplay(NULL);
    
    static int attribs[] = {GLX_DRAWABLE_TYPE, GLX_WINDOW_BIT,
                            GLX_RENDER_TYPE, GLX_RGBA_BIT,
                            GLX_DOUBLEBUFFER, True,
                            None};
    int numConfigs;
    GLXFBConfig* configs = glXChooseFBConfig(ctx->_display, DefaultScreen(ctx->_display), attribs, &numConfigs);
    if (numConfigs < 1) {
        fprintf(stderr, "Unable to choose a framebuffer configuation.");
        fflush(stderr);
    }
    ctx->_fbconfig = configs[0];
    XFree(configs);
    
    XVisualInfo *vis_info = glXGetVisualFromFBConfig(ctx->_display, ctx->_fbconfig);
    ctx->_visual = vis_info;
    
    Window root = RootWindow(ctx->_display, vis_info->screen);
    
    ctx->_colormap = XCreateColormap(ctx->_display, root, vis_info->visual, AllocNone);
    XSetWindowAttributes swa;
    swa.border_pixel = 0;
    swa.event_mask = 0;
    swa.colormap = ctx->_colormap;
    ctx->_drawable = XCreateWindow(ctx->_display, root, 0, 0, 100, 100, 0,
                                   vis_info->depth, InputOutput, vis_info->visual,
                                   CWBorderPixel|CWEventMask|CWColormap, &swa);
    static char *argv = "Replay";
    XSetStandardProperties(ctx->_display, ctx->_drawable, "Replay", "Replay", None, &argv, 1, NULL);
    XMapWindow(ctx->_display, ctx->_drawable);
    
    ctx->_glx_drawable = glXCreateWindow(ctx->_display, ctx->_fbconfig, ctx->_drawable, NULL);
    
    const trace_t* trace = inspection->trace;
    
    ctx->funcs = malloc(trace->func_name_count * sizeof(replay_func_t));
    
    for (size_t i = 0; i < trace->func_name_count; ++i) {
        char name[strlen(trace->func_names[i])+8];
        strcpy(name, "replay_");
        strcat(name, trace->func_names[i]);
        ctx->funcs[i] = dlsym(RTLD_DEFAULT, name);
        if (!ctx->funcs[i]) {
            fprintf(stderr, "Unable to find \"%s\".\n", name); //TODO: Handle
            fflush(stderr);
        }
    }
    
    for (size_t i = 0; i < ReplayClientArr_Max; i++)
        ctx->client_arrays[i] = NULL;
    ctx->generic_client_array_count = 0;
    ctx->generic_client_arrays = NULL;
    
    return ctx;
}

void free_obj(replay_obj_type_t type, replay_obj_t* obj) {
    switch (type) {
    case ReplayObjType_GLProgram:
        free_uni_vec(obj->prog.uniforms);
        free_attrib_vec(obj->prog.attribs);
        break;
    default:
        break;
    }
    
    free(obj);
}

void destroy_replay_context(replay_context_t* context) {
    free(context->generic_client_arrays);
    
    for (size_t i = 0; i < ReplayClientArr_Max; i++)
        free(context->client_arrays[i]);
    
    free(context->funcs);
    
    glXDestroyWindow(context->_display, context->_glx_drawable);
    XDestroyWindow(context->_display, context->_drawable);
    XFreeColormap(context->_display, context->_colormap);
    XFree(context->_visual);
    
    XCloseDisplay(context->_display);
    
    deinit_replay_gl(context);
    
    replay_internal_t* internal = context->_internal;
    
    for (size_t i = 0; i < 22; ++i) {
        replay_obj_t* obj = internal->objects[i];
        while (obj) {
            replay_obj_t* next = obj->next;
            free_obj(i, obj);
            obj = next;
        }
    }
    free(internal);
    
    free(context);
}

uint64_t replay_get_real_object(replay_context_t* ctx, replay_obj_type_t type, uint64_t fake) {
    replay_internal_t* internal = ctx->_internal;
    
    replay_obj_t* obj = internal->objects[type];
    while (obj) {
        if (obj->fake == fake)
            return obj->real;
        obj = obj->next;
    }
    
    return 0;
}

uint64_t replay_get_fake_object(replay_context_t* ctx, replay_obj_type_t type, uint64_t real) {
    replay_internal_t* internal = ctx->_internal;
    
    replay_obj_t* obj = internal->objects[type];
    while (obj) {
        if (obj->real == real)
            return obj->fake;
        obj = obj->next;
    }
    
    return 0;
}

void replay_create_object(replay_context_t* ctx, replay_obj_type_t type, uint64_t real, uint64_t fake) {
    replay_internal_t* internal = ctx->_internal;
    
    replay_obj_t* objs = internal->objects[type];
    
    replay_obj_t* new_obj = malloc(sizeof(replay_obj_t));
    new_obj->real = real;
    new_obj->fake = fake;
    new_obj->next = NULL;
    
    if (objs == NULL) {
        new_obj->prev = NULL;
        internal->objects[type] = new_obj;
    } else
    {
        while (objs->next)
            objs = objs->next;
        new_obj->prev = objs;
        objs->next = new_obj;
    }
    
    switch (type) {
    case ReplayObjType_GLProgram:
        new_obj->prog.uniforms = alloc_uni_vec(0);
        new_obj->prog.attribs = alloc_attrib_vec(0);
        break;
    default:
        break;
    }
}

void replay_destroy_object(replay_context_t* ctx, replay_obj_type_t type, uint64_t fake) {
    replay_internal_t* internal = ctx->_internal;
    
    replay_obj_t* obj = internal->objects[type];
    
    if (!obj) {
    } else if (!obj->next && obj->fake == fake) {
        free_obj(type, obj);
        internal->objects[type] = NULL;
    } else {
        while (obj) {
            if (obj->fake == fake) {
                if (obj->prev) obj->prev->next = obj->next;
                if (obj->next) obj->next->prev = obj->prev;
                free_obj(type, obj);
                return;
            }
            
            obj = obj->next;
        }
    }
}

size_t replay_get_obj_count(replay_context_t* ctx, replay_obj_type_t type) {
    replay_internal_t* internal = ctx->_internal;
    size_t result = 0;
    
    replay_obj_t* obj = internal->objects[type];
    while (obj) {
        obj = obj->next;
        result++;
    }
    
    return result;
}

void replay_list_real_objects(replay_context_t* ctx, replay_obj_type_t type, uint64_t* real) {
    replay_internal_t* internal = ctx->_internal;
    
    replay_obj_t* obj = internal->objects[type];
    size_t i = 0;
    while (obj) {
        real[i] = obj->real;
        obj = obj->next;
        i++;
    }
}

void replay_list_fake_objects(replay_context_t* ctx, replay_obj_type_t type, uint64_t* fake) {
    replay_internal_t* internal = ctx->_internal;
    
    replay_obj_t* obj = internal->objects[type];
    size_t i = 0;
    while (obj) {
        fake[i] = obj->fake;
        obj = obj->next;
        i++;
    }
}

void replay(replay_context_t* ctx) {
    ctx->_in_begin_end = false;
    
    for (size_t i = 0; i < ctx->inspection->frame_count; ++i) {
        inspect_frame_t* frame = ctx->inspection->frames + i;
        for (size_t j = 0; j < frame->command_count; ++j) {
            inspect_command_t* command = frame->commands + j;
            ctx->funcs[command->trace_cmd->func_index](ctx, command->trace_cmd, command);
        }
    }
}

int replay_conv_uniform_location(replay_context_t* ctx, uint64_t fake_prog, unsigned int fake_loc) {
    replay_internal_t* internal = ctx->_internal;
    
    replay_obj_t* obj = internal->objects[ReplayObjType_GLProgram];
    while (obj) {
        if (obj->fake == fake_prog) {
            uni_vec_t uniforms = obj->prog.uniforms;
            for (uniform_t* uni = uniforms->data; !vec_end(uniforms, uni); uni++)
                if (uni->fake == fake_loc)
                    return uni->real;
            
            return -1;
        }
        
        obj = obj->next;
    }
    
    return -1;
}

void replay_add_uniform(replay_context_t* ctx, uint64_t fake_prog, unsigned int fake, unsigned int real) {
    replay_internal_t* internal = ctx->_internal;
    
    replay_obj_t* obj = internal->objects[ReplayObjType_GLProgram];
    while (obj) {
        if (obj->fake == fake_prog) {
            uniform_t uni;
            uni.fake = fake;
            uni.real = real;
            append_uni_vec(obj->prog.uniforms, &uni);
            return;
        }
        
        obj = obj->next;
    }
}

int replay_conv_attrib_index(replay_context_t* ctx, uint64_t fake_prog, unsigned int fake_idx) {
    replay_internal_t* internal = ctx->_internal;
    
    replay_obj_t* obj = internal->objects[ReplayObjType_GLProgram];
    while (obj) {
        if (obj->fake == fake_prog) {
            attrib_vec_t attribs = obj->prog.attribs;
            for (attrib_t* attrib = attribs->data; !vec_end(attribs, attrib); attrib++)
                if (attrib->fake == fake_idx)
                    return attrib->real;
            
            return -1;
        }
        
        obj = obj->next;
    }
    
    return -1;
}

void replay_add_attrib(replay_context_t* ctx, uint64_t fake_prog, unsigned int fake, unsigned int real) {
    replay_internal_t* internal = ctx->_internal;
    
    replay_obj_t* obj = internal->objects[ReplayObjType_GLProgram];
    while (obj) {
        if (obj->fake == fake_prog) {
            uniform_t uni;
            uni.fake = fake;
            uni.real = real;
            append_uni_vec(obj->prog.attribs, &uni);
            return;
        }
        
        obj = obj->next;
    }
}
