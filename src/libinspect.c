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

typedef struct {
    unsigned int fake;
    unsigned int usage;
    size_t size;
    void* data;
} buffer_t;

typedef struct {
    unsigned int fake;
    unsigned int type;
    char* source;
} shader_t;

struct inspector_t {
    inspection_t* inspection;
    vec_t textures; //vec_t of texture_t
    vec_t buffers; //vec_t of buffer_t
    vec_t shaders; //vec_t of shader_t
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
            new_command->state.back.width = 0;
            new_command->state.back.height = 0;
            new_command->state.back.data = NULL;
            new_command->state.front.width = 0;
            new_command->state.front.height = 0;
            new_command->state.front.data = NULL;
            new_command->state.depth.width = 0;
            new_command->state.depth.height = 0;
            new_command->state.depth.data = NULL;
            new_command->state.actions = alloc_vec(0);
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
            
            free(command->state.back.data);
            free(command->state.front.data);
            free(command->state.depth.data);
            
            vec_t actions = command->state.actions;
            count = get_vec_size(actions)/sizeof(inspect_action_t);
            for (size_t k = 0; k < count; ++k) {
                inspect_action_t* action = (inspect_action_t*)get_vec_data(actions) + k;
                switch (action->type) {
                case InspectAction_TexParams: {
                    break;
                }
                case InspectAction_TexData: {
                    free(action->tex_data.data);
                    break;
                }
                case InspectAction_GenTexture: {
                    break;
                }
                case InspectAction_DelTexture: {
                    break;
                }
                case InspectAction_GenBuffer: {
                    break;
                }
                case InspectAction_DelBuffer: {
                    break;
                }
                case InspectAction_BufferData: {
                    free(action->buf_data.data);
                    break;
                }
                case InspectAction_BufferSubData: {
                    free(action->buf_sub_data.data);
                    break;
                }
                case InspectAction_NewShader: {
                    break;
                }
                case InspectAction_DelShader: {
                    break;
                }
                case InspectAction_ShaderSource: {
                    for (size_t i = 0; i < action->shader_source.count; i++) {
                        free(action->shader_source.sources[i]);
                    }
                    free(action->shader_source.sources);
                    break;
                }
                }
            }
            free_vec(actions);
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


inspector_t* create_inspector(inspection_t* inspection) {
    inspector_t* res = malloc(sizeof(inspector_t));
    res->textures = alloc_vec(0);
    res->buffers = alloc_vec(0);
    res->shaders = alloc_vec(0);
    res->inspection = inspection;
    return res;
}

static void free_textures(inspector_t* inspector) {
    size_t count = get_vec_size(inspector->textures)/sizeof(texture_t);
    for (size_t i = 0; i < count; ++i) {
        texture_t* tex = (texture_t*)get_vec_data(inspector->textures) + i;
        for (size_t j = 0; j < tex->mipmap_count; ++j)
            free(tex->mipmaps[j]);
        free(tex->mipmaps);
    }
}

static void free_buffers(inspector_t* inspector) {
    size_t count = get_vec_size(inspector->buffers)/sizeof(buffer_t);
    for (size_t i = 0; i < count; ++i) {
        buffer_t* buf = (buffer_t*)get_vec_data(inspector->buffers) + i;
        free(buf->data);
    }
}

static void free_shaders(inspector_t* inspector) {
    size_t count = get_vec_size(inspector->shaders)/sizeof(shader_t);
    for (size_t i = 0; i < count; ++i) {
        shader_t* shdr = (shader_t*)get_vec_data(inspector->shaders) + i;
        free(shdr->source);
    }
}

void free_inspector(inspector_t* inspector) {
    free_buffers(inspector);
    free_textures(inspector);
    free_vec(inspector->shaders);
    free_vec(inspector->buffers);
    free_vec(inspector->textures);
    free(inspector);
}

static texture_t* find_tex(inspector_t* inspector, unsigned int fake) {
    int tex_index = inspect_find_tex(inspector, fake);
    if (tex_index == -1) {
        return NULL;
    }
    
    return (texture_t*)get_vec_data(inspector->textures) + tex_index;
}

static buffer_t* find_buf(inspector_t* inspector, unsigned int fake) {
    int buf_index = inspect_find_buf(inspector, fake);
    if (buf_index == -1) {
        return NULL;
    }
    
    return (buffer_t*)get_vec_data(inspector->buffers) + buf_index;
}

static shader_t* find_shdr(inspector_t* inspector, unsigned int fake) {
    int shdr_index = inspect_find_shdr(inspector, fake);
    if (shdr_index == -1) {
        return NULL;
    }
    
    return (shader_t*)get_vec_data(inspector->shaders) + shdr_index;
}

static void update_inspection(inspector_t* inspector, inspect_gl_state_t* state) {
    vec_t actions = state->actions;
    size_t count = get_vec_size(actions)/sizeof(inspect_action_t);
    for (size_t i = 0; i < count; ++i) {
        inspect_action_t* action = (inspect_action_t*)get_vec_data(actions) + i;
        
        switch (action->type) {
        case InspectAction_TexParams: {
            inspect_gl_tex_params_t* params = &action->tex_params;
            texture_t* tex = find_tex(inspector, params->texture);
            
            if (!tex)
                break;
            
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
            break;
        }
        case InspectAction_TexData: {
            inspect_gl_tex_data_t* data = &action->tex_data;
            texture_t* tex = find_tex(inspector, data->texture);
            if (!tex)
                break;
            if (!tex->mipmaps[data->mipmap])
                tex->mipmaps[data->mipmap] = malloc(data->data_size);
            
            memcpy(tex->mipmaps[data->mipmap], data->data, data->data_size);
            break;
        }
        case InspectAction_GenTexture: {
            texture_t tex;
            tex.fake = action->texture;
            tex.params.texture = action->texture;
            tex.params.width = 0;
            tex.params.height = 0;
            tex.params.type = 0;
            tex.mipmap_count = 0;
            tex.mipmaps = NULL;
            append_vec(inspector->textures, sizeof(texture_t), &tex);
            break;
        }
        case InspectAction_DelTexture: {
            texture_t* tex = find_tex(inspector, action->texture);
            if (!tex)
                break;
            
            for (size_t j = 0; j < tex->mipmap_count; ++j)
                free(tex->mipmaps[j]);
            free(tex->mipmaps);
            
            size_t offset = tex - (texture_t*)get_vec_data(inspector->textures);
            offset *= sizeof(texture_t);
            remove_vec(inspector->textures, offset, sizeof(texture_t));
            break;
        }
        case InspectAction_GenBuffer: {
            buffer_t buf;
            buf.fake = action->buffer;
            buf.usage = 0;
            buf.size = 0;
            buf.data = NULL;
            append_vec(inspector->buffers, sizeof(buffer_t), &buf);
            break;
        }
        case InspectAction_DelBuffer: {
            buffer_t* buf = find_buf(inspector, action->buffer);
            if (!buf)
                break;
            
            free(buf->data);
            
            size_t offset = buf - (buffer_t*)get_vec_data(inspector->buffers);
            offset *= sizeof(buffer_t);
            remove_vec(inspector->buffers, offset, sizeof(buffer_t));
            break;
        }
        case InspectAction_BufferData: {
            inspect_gl_buffer_data_t* data = &action->buf_data;
            buffer_t* buf = find_buf(inspector, action->buf_data.buffer);
            if (!buf)
                break;
            if (buf->data)
                free(buf->data);
            
            buf->usage = data->usage;
            buf->size = data->size;
            buf->data = malloc(data->size);
            memcpy(buf->data, data->data, data->size);
            break;
        }
        case InspectAction_BufferSubData: {
            inspect_gl_buffer_sub_data_t* data = &action->buf_sub_data;
            buffer_t* buf = find_buf(inspector, action->buf_sub_data.buffer);
            if (!buf)
                break;
            if (!buf->data)
                break;
            if (data->offset+data->size > buf->size)
                break;
            
            memcpy((uint8_t*)buf->data+data->offset, data->data, data->size);
            break;
        }
        case InspectAction_NewShader: {
            shader_t shdr;
            shdr.fake = action->new_shader.shader;
            shdr.type = action->new_shader.type;
            shdr.source = NULL;
            append_vec(inspector->shaders, sizeof(shader_t), &shdr);
            break;
        }
        case InspectAction_DelShader: {
            shader_t* shdr = find_shdr(inspector, action->del_shader);
            if (!shdr)
                break;
            
            free(shdr->source);
            
            size_t offset = shdr - (shader_t*)get_vec_data(inspector->shaders);
            offset *= sizeof(shader_t);
            remove_vec(inspector->shaders, offset, sizeof(shader_t));
            break;
        }
        case InspectAction_ShaderSource: {
            shader_t* shdr = find_shdr(inspector, action->shader_source.shader);
            if (!shdr)
                break;
            
            free(shdr->source);
            
            size_t lens[action->shader_source.count];
            size_t len = 0;
            for (size_t i = 0; i < action->shader_source.count; i++) {
                lens[i] = strlen(action->shader_source.sources[i]);
                len += lens[i];
            }
            
            shdr->source = malloc(len+1);
            shdr->source[len] = 0;
            
            size_t offset = 0;
            for (size_t i = 0; i < action->shader_source.count; i++) {
                memcpy(shdr->source+offset, action->shader_source.sources[i], lens[i]);
                offset += lens[i];
            }
            break;
        }
        }
    }
}

void seek_inspector(inspector_t* inspector, size_t frame_index, size_t cmd_index) {
    free_buffers(inspector);
    free_textures(inspector);
    free_shaders(inspector);
    resize_vec(inspector->buffers, 0);
    resize_vec(inspector->textures, 0);
    resize_vec(inspector->shaders, 0);
    
    inspection_t* inspection = inspector->inspection;
    
    if (frame_index >= inspection->frame_count)
        return;
    
    for (size_t i = 0; i <= frame_index; ++i) {
        inspect_frame_t* frame = inspection->frames + i;
        
        if ((cmd_index >= frame->command_count) && (i == frame_index))
            return;
        
        size_t count = (i == frame_index) ? cmd_index+1 : frame->command_count;
        for (size_t j = 0; j < count; ++j)
            update_inspection(inspector, &frame->commands[j].state);
    }
}

size_t inspect_get_tex_count(inspector_t* inspector) {
    return get_vec_size(inspector->textures)/sizeof(texture_t);
}

size_t inspect_get_buf_count(inspector_t* inspector) {
    return get_vec_size(inspector->buffers)/sizeof(buffer_t);
}

size_t inspect_get_shdr_count(inspector_t* inspector) {
    return get_vec_size(inspector->shaders)/sizeof(shader_t);
}

unsigned int inspect_get_tex(inspector_t* inspector, size_t index) {
    vec_t textures = inspector->textures;
    size_t count = get_vec_size(textures)/sizeof(texture_t);
    
    if (index >= count)
        return false;
    
    return ((texture_t*)get_vec_data(textures))[index].fake;
}

int inspect_find_tex(inspector_t* inspector, unsigned int tex) {
    vec_t textures = inspector->textures;
    size_t count = get_vec_size(textures)/sizeof(texture_t);
    for (size_t i = 0; i < count; ++i)
        if (((texture_t*)get_vec_data(textures))[i].fake == tex)
            return i;
    
    return -1;
}

bool inspect_get_tex_params(inspector_t* inspector, size_t index, inspect_gl_tex_params_t* dest) {
    vec_t textures = inspector->textures;
    size_t count = get_vec_size(textures)/sizeof(texture_t);
    
    if (index >= count)
        return false;
    
    *dest = ((texture_t*)get_vec_data(textures))[index].params;
    
    return true;
}

bool inspect_get_tex_data(inspector_t* inspector, size_t index, size_t level, void** dest) {
    vec_t textures = inspector->textures;
    size_t count = get_vec_size(textures)/sizeof(texture_t);
    
    if (index >= count)
        return false;
    
    texture_t* tex = (texture_t*)get_vec_data(textures) + index;
    
    if (level > tex->mipmap_count)
        return false;
    
    *dest = tex->mipmaps[level];
    
    return true;
}

unsigned int inspect_get_buf(inspector_t* inspector, size_t index) {
    vec_t buffers = inspector->buffers;
    size_t count = get_vec_size(buffers)/sizeof(buffer_t);
    
    if (index >= count)
        return false;
    
    return ((buffer_t*)get_vec_data(buffers))[index].fake;
}

int inspect_find_buf(inspector_t* inspector, unsigned int buf) {
    vec_t buffers = inspector->buffers;
    size_t count = get_vec_size(buffers)/sizeof(buffer_t);
    for (size_t i = 0; i < count; ++i)
        if (((buffer_t*)get_vec_data(buffers))[i].fake == buf)
            return i;
    
    return -1;
}

int inspect_get_buf_size(inspector_t* inspector, size_t index) {
    vec_t buffers = inspector->buffers;
    size_t count = get_vec_size(buffers)/sizeof(buffer_t);
    
    if (index >= count)
        return -1;
    
    return ((buffer_t*)get_vec_data(buffers))[index].size;
}

bool inspect_get_buf_data(inspector_t* inspector, size_t index, void** data) {
    vec_t buffers = inspector->buffers;
    size_t count = get_vec_size(buffers)/sizeof(buffer_t);
    
    if (index >= count)
        return false;
    
    *data = ((buffer_t*)get_vec_data(buffers))[index].data;
    
    return true;
}

unsigned int inspect_get_shdr(inspector_t* inspector, size_t index) {
    vec_t shaders = inspector->shaders;
    size_t count = get_vec_size(shaders)/sizeof(shader_t);
    
    if (index >= count)
        return false;
    
    return ((shader_t*)get_vec_data(shaders))[index].fake;
}

int inspect_find_shdr(inspector_t* inspector, unsigned int shdr) {
    vec_t shaders = inspector->shaders;
    size_t count = get_vec_size(shaders)/sizeof(shader_t);
    for (size_t i = 0; i < count; ++i)
        if (((shader_t*)get_vec_data(shaders))[i].fake == shdr)
            return i;
    
    return -1;
}

int inspect_get_shdr_type(inspector_t* inspector, size_t index) {
    vec_t shaders = inspector->shaders;
    size_t count = get_vec_size(shaders)/sizeof(shader_t);
    
    if (index >= count)
        return -1;
    
    return ((shader_t*)get_vec_data(shaders))[index].type;
}

bool inspect_get_shdr_source(inspector_t* inspector, size_t index, char** source) {
    vec_t shaders = inspector->shaders;
    size_t count = get_vec_size(shaders)/sizeof(shader_t);
    
    if (index >= count)
        return false;
    
    *source = ((shader_t*)get_vec_data(shaders))[index].source;
    
    return true;
}
