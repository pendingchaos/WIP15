#include "replay.h"

#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <dlfcn.h>
#include <X11/Xlib.h>

void init_replay_gl(replay_context_t* ctx);
void deinit_replay_gl(replay_context_t* ctx);

typedef struct replay_obj_t {
    void* real;
    void* fake;
    struct replay_obj_t* prev;
    struct replay_obj_t* next;
} replay_obj_t;

typedef struct {
    replay_obj_t *objects[19];
} replay_internal_t;

replay_context_t* create_replay_context(inspection_t* inspection) {
    replay_context_t* ctx = malloc(sizeof(replay_context_t));
    replay_internal_t* internal = malloc(sizeof(replay_internal_t));
    
    ctx->inspection = inspection;
    
    for (size_t i = 0; i < 19; ++i)
        internal->objects[i] = NULL;
    ctx->_internal = internal;
    
    init_replay_gl(ctx);
    
    //TODO: GLX stuff
    ctx->_display = XOpenDisplay(NULL);
    
    const trace_t* trace = inspection->trace;
    
    ctx->funcs = malloc(trace->func_name_count * sizeof(replay_func_t));
    
    for (size_t i = 0; i < trace->func_name_count; ++i) {
        char name[strlen(trace->func_names[i])+8];
        strcpy(name, "replay_");
        strcat(name, trace->func_names[i]);
        ctx->funcs = dlsym(RTLD_DEFAULT, name);
    }
    
    return ctx;
}

void destroy_replay_context(replay_context_t* context) {
    free(context->funcs);
    
    //TODO: GLX stuff
    XCloseDisplay(context->_display);
    
    deinit_replay_gl(context);
    
    replay_internal_t* internal = context->_internal;
    
    for (size_t i = 0; i < 19; ++i) {
        replay_obj_t* obj = internal->objects[i];
        while (obj) {
            replay_obj_t* next = obj->next;
            free(obj);
            obj = next;
        }
    }
    free(internal);
    
    free(context);
}

void* replay_get_real_object(replay_context_t* ctx, replay_obj_type_t type, void* fake) {
    replay_internal_t* internal = ctx->_internal;
    
    replay_obj_t* obj = internal->objects[type];
    while (obj) {
        if (obj->fake == fake)
            return obj->real;
        obj = obj->next;
    }
    
    return NULL;
}

void* replay_get_fake_object(replay_context_t* ctx, replay_obj_type_t type, void* real) {
    replay_internal_t* internal = ctx->_internal;
    
    replay_obj_t* obj = internal->objects[type];
    while (obj) {
        if (obj->real == real)
            return obj->fake;
        obj = obj->next;
    }
    
    return NULL;
}

void replay_create_object(replay_context_t* ctx, replay_obj_type_t type, void* real, void *fake) {
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
}

void replay_destroy_object(replay_context_t* ctx, replay_obj_type_t type, void* fake) {
    replay_internal_t* internal = ctx->_internal;
    
    replay_obj_t* obj = internal->objects[type];
    while (obj) {
        if (obj->fake == fake) {
            obj->prev->next = obj->next;
            obj->next->prev = obj->prev;
            free(obj);
            return;
        }
        
        obj = obj->next;
    }
}

void replay(replay_context_t* ctx) {
    inspect_frame_t* frame = ctx->inspection->frames;
    while (frame) {
        inspect_command_t* command = frame->commands;
        while (command) {
            ctx->funcs[command->trace_cmd->func_index](ctx, command->trace_cmd, command);
        }
        
        frame = frame->next;
    }
}
