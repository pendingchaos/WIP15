#include "libtrace.h"
#include "libinspect.h"
#include "replay.h"
#include "glapi.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stddef.h>
#include <stdarg.h>
#include <unistd.h>

typedef struct {
    unsigned int fake;
    inspect_gl_tex_params_t params;
    size_t mipmap_count;
    void** mipmaps;
} texture_t;

struct tex_inspector_t {
    inspection_t* inspection;
    vec_t textures;
};

static void update_context(uint64_t *context,
                           const trace_t* trace,
                           trace_command_t* trace_cmd) {
    const char *name = trace->func_names[trace_cmd->func_index];
    
    if (strcmp(name, "glXMakeCurrent") == 0) {
        *context = *trace_get_ptr(&trace_get_arg(trace_cmd, 2)->val);
    }
}

inspection_t* create_inspection(const trace_t* trace) {
    inspection_t* result = malloc(sizeof(inspection_t));
    result->trace = trace;
    result->frame_count = 0;
    
    trace_frame_t* frame = trace->frames;
    while (frame) {
        ++result->frame_count;
        frame = frame->next;
    }
    
    result->frames = malloc(sizeof(inspect_frame_t)*result->frame_count);
    
    uint64_t context = 0;
    size_t i = 0;
    frame = trace->frames;
    while (frame) {
        inspect_frame_t* new_frame = result->frames + i;
        new_frame->trace_frame = frame;
        new_frame->command_count = get_vec_size(frame->commands) / sizeof(trace_command_t);
        new_frame->commands = malloc(sizeof(inspect_command_t)*new_frame->command_count);
        
        for (size_t j = 0; j < new_frame->command_count; ++j) {
            trace_command_t* command = trace_get_cmd(frame, j);
            
            update_context(&context, trace, command);
            
            inspect_command_t* new_command = new_frame->commands + j;
            
            new_command->trace_cmd = command;
            new_command->attachments = NULL;
            new_command->name = trace->func_names[command->func_index];
            new_command->gl_context = context;
            new_command->state.entries = alloc_vec(0);
            new_command->state.color.width = 0;
            new_command->state.color.height = 0;
            new_command->state.color.data = NULL;
            new_command->state.depth.width = 0;
            new_command->state.depth.height = 0;
            new_command->state.depth.data = NULL;
            new_command->state.texture_params = alloc_vec(0);
            new_command->state.texture_data = alloc_vec(0);
        }
        
        frame = frame->next;
        ++i;
    }
    
    return result;
}

void free_inspection(inspection_t* inspection) {
    for (size_t i = 0; i < inspection->frame_count; ++i) {
        inspect_frame_t* frame = inspection->frames + i;
        for (size_t j = 0; j < frame->command_count; ++j) {
            inspect_command_t* command = frame->commands + j;
            
            inspect_attachment_t* attach = command->attachments;
            while (attach) {
                inspect_attachment_t* next_attach = attach->next;
                free(attach->message);
                free(attach);
                attach = next_attach;
            }
            
            vec_t entries = command->state.entries;
            size_t count = get_vec_size(entries)/sizeof(inspect_gl_state_entry_t);
            for (size_t k = 0; k < count; ++k) {
                inspect_gl_state_entry_t* entry = ((inspect_gl_state_entry_t*)get_vec_data(entries)) + k;
                trace_free_value(entry->val);
            }
            free_vec(entries);
            
            free(command->state.color.data);
            free(command->state.depth.data);
            
            free_vec(command->state.texture_params);
            
            vec_t tex_data = command->state.texture_data;
            count = get_vec_size(tex_data)/sizeof(inspect_gl_tex_data_t);
            for (size_t k = 0; k < count; ++k) {
                inspect_gl_tex_data_t* data = ((inspect_gl_tex_data_t*)get_vec_data(tex_data)) + k;
                free(data->data);
            }
            free_vec(tex_data);
        }
        
        free(frame->commands);
    }
    
    free(inspection->frames);
    free(inspection);
}

static const glapi_group_t* find_group(const char *name) {
    for (size_t i = 0; i < glapi.group_count; i++) {
        if (strcmp(glapi.groups[i]->name, name) == 0) {
            return glapi.groups[i];
        }
    }
    
    return NULL;
}

static void validate_command(inspect_command_t* command, const trace_t* trace) {
    //Validate enum argument values
    vec_t args = command->trace_cmd->args;
    for (size_t i = 0; i < get_vec_size(args)/sizeof(trace_arg_t); ++i) {
        trace_arg_t* arg = ((trace_arg_t*)get_vec_data(args)) + i;
        
        if (arg->val.group_index < 0 ? false : (trace->group_names[arg->val.group_index][0] != 0)) {
            const glapi_group_t* group = find_group(trace->group_names[arg->val.group_index]);
            
            if (!group) {
            } else if (group->bitmask) {
                //TODO
            } else {
                bool valid = false;
                for (size_t j = 0; j < group->entry_count; ++j) {
                    const glapi_group_entry_t *entry = group->entries[j];
                    //TODO: Requirements
                    if (entry->value == *trace_get_uint(&arg->val)) {
                        valid = true;
                    }
                }
                
                if (!valid) {
                    inspect_add_error(command,
                                      "Invalid enum value %d for enum \"%s\".",
                                      *trace_get_uint(&arg->val),
                                      group->name);
                }
            }
        }
    }
}

void inspect(inspection_t* inspection) {
    for (size_t i = 0; i < inspection->frame_count; ++i) {
        inspect_frame_t* frame = inspection->frames + i;
        for (size_t j = 0; j < frame->command_count; ++j) {
            validate_command(frame->commands + j, inspection->trace);
        }
    }
    
    replay_context_t* ctx = create_replay_context(inspection);
    replay(ctx);
    destroy_replay_context(ctx);
}

static char *format_str(const char *format, va_list list) {
    va_list list2;
    va_copy(list2, list);
    char dummy_buf[1];
    int length = vsnprintf(dummy_buf, 0, format, list2);
    va_end(list2);
    
    if (length < 0) {
        char *result = malloc(1);
        result[0] = 0;
        return result;
    } else {
        char *result = malloc(length+1);
        vsnprintf(result, length+1, format, list);
        result[length] = 0;
        return result;
    }
}

void inspect_add_info(inspect_command_t* command, const char* format, ...) {
    inspect_attachment_t* attach = malloc(sizeof(inspect_attachment_t));
    attach->type = AttachType_Info;
    attach->next = NULL;
    
    va_list list;
    va_start(list, format);
    attach->message = format_str(format, list);
    va_end(list);
    
    inspect_add_attachment(command, attach);
}

void inspect_add_warning(inspect_command_t* command, const char* format, ...) {
    inspect_attachment_t* attach = malloc(sizeof(inspect_attachment_t));
    attach->type = AttachType_Warning;
    attach->next = NULL;
    
    va_list list;
    va_start(list, format);
    attach->message = format_str(format, list);
    va_end(list);
    
    inspect_add_attachment(command, attach);
}

void inspect_add_error(inspect_command_t* command, const char* format, ...) {
    inspect_attachment_t* attach = malloc(sizeof(inspect_attachment_t));
    attach->type = AttachType_Error;
    attach->next = NULL;
    
    va_list list;
    va_start(list, format);
    attach->message = format_str(format, list);
    va_end(list);
    
    inspect_add_attachment(command, attach);
}

void inspect_add_attachment(inspect_command_t* command, inspect_attachment_t* attach) {
    if (!command->attachments) {
        command->attachments = attach;
    } else {
        inspect_attachment_t* current = command->attachments;
        while (current->next) current = current->next;
        current->next = attach;
    }
}


tex_inspector_t* create_tex_inspector(inspection_t* inspection) {
    tex_inspector_t* res = malloc(sizeof(tex_inspector_t));
    res->textures = alloc_vec(0);
    res->inspection = inspection;
    return res;
}

static void free_textures(tex_inspector_t* inspector) {
    size_t count = get_vec_size(inspector->textures)/sizeof(texture_t);
    for (size_t i = 0; i < count; ++i) {
        texture_t* tex = (texture_t*)get_vec_data(inspector->textures) + i;
        for (size_t j = 0; j < tex->mipmap_count; ++j)
            free(tex->mipmaps[j]);
        free(tex->mipmaps);
    }
}

void free_tex_inspector(tex_inspector_t* inspector) {
    free_textures(inspector);
    free_vec(inspector->textures);
    free(inspector);
}

static texture_t* find_or_create_tex(tex_inspector_t* inspector, unsigned int fake) {
    int tex_index = inspect_find_tex(inspector, fake);
    if (tex_index == -1) {
        tex_index = get_vec_size(inspector->textures)/sizeof(texture_t);
        texture_t tex;
        tex.fake = fake;
        tex.params.fake_texture = fake;
        tex.params.width = 0;
        tex.params.height = 0;
        tex.mipmap_count = 0;
        tex.mipmaps = NULL;
        append_vec(inspector->textures, sizeof(texture_t), &tex);
    }
    
    return (texture_t*)get_vec_data(inspector->textures) + tex_index;
}

static void update_tex_inspection(tex_inspector_t* inspector, inspect_gl_state_t* state) {
    vec_t tex_params = state->texture_params;
    size_t count = get_vec_size(tex_params)/sizeof(inspect_gl_tex_params_t);
    for (size_t i = 0; i < count; ++i) {
        inspect_gl_tex_params_t* params = (inspect_gl_tex_params_t*)get_vec_data(tex_params) + i;
        texture_t* tex = find_or_create_tex(inspector, params->fake_texture);
        
        if (tex->params.width != params->width || tex->params.height != params->height) {
            size_t mipmap_count = 0;
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
    
    vec_t tex_data = state->texture_data;
    count = get_vec_size(tex_data)/sizeof(inspect_gl_tex_data_t);
    for (size_t i = 0; i < count; ++i) {
        inspect_gl_tex_data_t* data = (inspect_gl_tex_data_t*)get_vec_data(tex_data) + i;
        texture_t* tex = find_or_create_tex(inspector, data->fake_texture);
        
        if (!tex->mipmaps[data->mipmap])
            tex->mipmaps[data->mipmap] = malloc(data->data_size);
        
        memcpy(tex->mipmaps[data->mipmap], data->data, data->data_size);
    }
}

void seek_tex_inspector(tex_inspector_t* inspector, size_t frame_index, size_t cmd_index) {
    free_textures(inspector);
    resize_vec(inspector->textures, 0);
    
    inspection_t* inspection = inspector->inspection;
    
    if (frame_index >= inspection->frame_count)
        return;
    
    for (size_t i = 0; i <= frame_index; ++i) {
        inspect_frame_t* frame = inspection->frames;
        
        if ((cmd_index >= frame->command_count) && (i == frame_index))
            return;
        
        size_t count = i == frame_index ? frame->command_count : cmd_index+1;
        for (size_t j = 0; j < count; ++j)
            update_tex_inspection(inspector, &frame->commands[j].state);
    }
}

size_t inspect_get_tex_count(tex_inspector_t* inspector) {
    return get_vec_size(inspector->textures)/sizeof(texture_t);
}

unsigned int inspect_get_tex(tex_inspector_t* inspector, size_t index) {
    vec_t textures = inspector->textures;
    size_t count = get_vec_size(textures)/sizeof(texture_t);
    
    if (index >= count)
        return false;
    
    return ((texture_t*)get_vec_data(textures))[index].fake;
}

int inspect_find_tex(tex_inspector_t* inspector, unsigned int tex) {
    vec_t textures = inspector->textures;
    size_t count = get_vec_size(textures)/sizeof(texture_t);
    for (size_t i = 0; i < count; ++i)
        if (((texture_t*)get_vec_data(textures))[i].fake == tex)
            return i;
    
    return -1;
}

bool inspect_get_tex_params(tex_inspector_t* inspector, size_t index, inspect_gl_tex_params_t* dest) {
    vec_t textures = inspector->textures;
    size_t count = get_vec_size(textures)/sizeof(texture_t);
    
    if (index >= count)
        return false;
    
    *dest = ((texture_t*)get_vec_data(textures))[index].params;
    
    return true;
}
