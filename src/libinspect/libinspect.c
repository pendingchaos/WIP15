#include "libtrace/libtrace.h"
#include "libinspect/libinspect.h"
#include "libinspect/replay.h"
#include "shared/glapi.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stddef.h>
#include <stdarg.h>
#include <unistd.h>

static void update_context(uint64_t *context,
                           const trace_t* trace,
                           trace_command_t* trace_cmd) {
    const char *name = trace->func_names[trace_cmd->func_index];
    
    if (strcmp(name, "glXMakeCurrent") == 0) {
        *context = *trace_get_ptr(trace_get_arg(trace_cmd, 2));
    }
}

inspection_t* create_inspection(const trace_t* trace) {
    inspection_t* result = malloc(sizeof(inspection_t));
    result->trace = trace;
    result->frame_count = get_trace_frame_vec_count(trace->frames);
    result->frames = malloc(sizeof(inspect_frame_t)*result->frame_count);
    
    uint64_t context = 0;
    for (size_t i = 0; i < result->frame_count; i++) {
        trace_frame_t* frame = get_trace_frame_vec(trace->frames, i);
        
        inspect_frame_t* new_frame = result->frames + i;
        new_frame->trace_frame = frame;
        new_frame->command_count = get_trace_cmd_vec_count(frame->commands);
        new_frame->commands = malloc(sizeof(inspect_command_t)*new_frame->command_count);
        
        for (size_t j = 0; j < new_frame->command_count; ++j) {
            trace_command_t* command = trace_get_cmd(frame, j);
            
            update_context(&context, trace, command);
            
            inspect_command_t* new_command = new_frame->commands + j;
            
            new_command->trace_cmd = command;
            new_command->attachments = NULL;
            new_command->name = trace->func_names[command->func_index];
            new_command->gl_context = context;
            new_command->state.entries = alloc_inspect_gl_state_vec(0);
            new_command->state.back.width = 0;
            new_command->state.back.height = 0;
            new_command->state.back.data = NULL;
            new_command->state.front.width = 0;
            new_command->state.front.height = 0;
            new_command->state.front.data = NULL;
            new_command->state.depth.width = 0;
            new_command->state.depth.height = 0;
            new_command->state.depth.data = NULL;
            new_command->state.actions = alloc_inspect_act_vec(0);
        }
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
            
            inspect_gl_state_vec_t entries = command->state.entries;
            for (inspect_gl_state_entry_t* entry = entries->data;
                 !vec_end(entries, entry);
                 entry++)
                trace_free_value(entry->val);
            free_inspect_gl_state_vec(entries);
            
            free(command->state.back.data);
            free(command->state.front.data);
            free(command->state.depth.data);
            
            inspect_act_vec_t actions = command->state.actions;
            for (inspect_action_t* act = actions->data; !vec_end(actions, act); act++)
                if (act->free_func)
                    act->free_func(act);
            free_inspect_act_vec(actions);
        }
        
        free(frame->commands);
    }
    
    free(inspection->frames);
    free(inspection);
}

static const glapi_group_t* find_group(const char *name) {
    for (size_t i = 0; i < glapi.group_count; i++)
        if (strcmp(glapi.groups[i]->name, name) == 0)
            return glapi.groups[i];
    
    return NULL;
}


static void validate_command(inspect_command_t* command, const trace_t* trace) {
    //Validate enum argument values
    vec_t args = command->trace_cmd->args;
    for (size_t i = 0; i < get_trace_val_vec_count(args); ++i) {
        trace_value_t* arg = get_trace_val_vec(args, i);
        
        if (arg->group_index < 0 ? false : (trace->group_names[arg->group_index][0] != 0)) {
            const glapi_group_t* group = find_group(trace->group_names[arg->group_index]);
            
            if (!group) {
            } else if (group->bitmask) {
                //TODO
            } else {
                uint64_t val = (arg->type==Type_Boolean) ?
                                *trace_get_bool(arg) :
                                *trace_get_uint(arg);
                
                bool valid = false;
                for (size_t j = 0; j < group->entry_count; ++j) {
                    const glapi_group_entry_t *entry = group->entries[j];
                    //TODO: Requirements
                    if (entry->value == val) {
                        valid = true;
                    }
                }
                
                if (!valid) {
                    inspect_add_error(command,
                                      "Invalid enum value %d for enum \"%s\".",
                                      *trace_get_uint(arg),
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
    res->programs = alloc_vec(0);
    res->vaos = alloc_vec(0);
    res->framebuffers = alloc_vec(0);
    res->renderbuffers = alloc_vec(0);
    res->inspection = inspection;
    
    inspect_vao_t vao;
    vao.fake = 0;
    vao.attrib_count = 0;
    vao.attribs = NULL;
    append_inspect_vao_vec(res->vaos, &vao);
    
    res->back_buf = res->front_buf = res->depth_buf = NULL;
    
    return res;
}

static void free_textures(inspector_t* inspector) {
    inspect_tex_vec_t textures = inspector->textures;
    for (inspect_texture_t* tex = textures->data; !vec_end(textures, tex); tex++) {
        for (size_t j = 0; j < tex->mipmap_count; j++)
            free(tex->mipmaps[j]);
        free(tex->mipmaps);
    }
}

static void free_buffers(inspector_t* inspector) {
    inspect_buf_vec_t buffers = inspector->buffers;
    for (inspect_buffer_t* buf = buffers->data; !vec_end(buffers, buf); buf++)
        free(buf->data);
}

static void free_shaders(inspector_t* inspector) {
    inspect_shdr_vec_t shaders = inspector->shaders;
    for (inspect_shader_t* shdr = shaders->data; !vec_end(shaders, shdr); shdr++) {
        free(shdr->source);
        free(shdr->info_log);
    }
}

static void free_programs(inspector_t* inspector) {
    inspect_prog_vec_t programs = inspector->programs;
    for (inspect_program_t* prog = programs->data; !vec_end(programs, prog); prog++) {
        free_vec(prog->shaders);
        free(prog->info_log);
    }
}

static void free_vaos(inspector_t* inspector) {
    inspect_vao_vec_t vaos = inspector->vaos;
    for (inspect_vao_t* vao = vaos->data; !vec_end(vaos, vao); vao++)
        free(vao->attribs);
}

static void free_framebuffers(inspector_t* inspector) {
    inspect_fb_vec_t fbs = inspector->framebuffers;
    for (inspect_fb_t* fb = fbs->data; !vec_end(fbs, fb); fb++)
        free_inspect_fb_attach_vec(fb->color);
}

void free_inspector(inspector_t* inspector) {
    free_buffers(inspector);
    free_textures(inspector);
    free_shaders(inspector);
    free_programs(inspector);
    free_vaos(inspector);
    free_framebuffers(inspector);
    free_inspect_prog_vec(inspector->programs);
    free_inspect_shdr_vec(inspector->shaders);
    free_inspect_buf_vec(inspector->buffers);
    free_inspect_tex_vec(inspector->textures);
    free_inspect_vao_vec(inspector->vaos);
    free_inspect_fb_vec(inspector->framebuffers);
    free_inspect_rb_vec(inspector->renderbuffers);
    free(inspector);
}

inspect_texture_t* inspect_find_tex_ptr(inspector_t* inspector, unsigned int fake) {
    int tex_index = inspect_find_tex(inspector, fake);
    if (tex_index == -1) {
        return NULL;
    }
    
    return get_inspect_tex_vec_data(inspector->textures) + tex_index;
}

inspect_buffer_t* inspect_find_buf_ptr(inspector_t* inspector, unsigned int fake) {
    int buf_index = inspect_find_buf(inspector, fake);
    if (buf_index == -1) {
        return NULL;
    }
    
    return get_inspect_buf_vec_data(inspector->buffers) + buf_index;
}

inspect_shader_t* inspect_find_shdr_ptr(inspector_t* inspector, unsigned int fake) {
    int shdr_index = inspect_find_shdr(inspector, fake);
    if (shdr_index == -1) {
        return NULL;
    }
    
    return get_inspect_shdr_vec_data(inspector->shaders) + shdr_index;
}

inspect_program_t* inspect_find_prog_ptr(inspector_t* inspector, unsigned int fake) {
    int prog_index = inspect_find_prog(inspector, fake);
    if (prog_index == -1) {
        return NULL;
    }
    
    return get_inspect_prog_vec_data(inspector->programs) + prog_index;
}

inspect_vao_t* inspect_find_vao_ptr(inspector_t* inspector, unsigned int fake) {
    int vao_index = inspect_find_vao(inspector, fake);
    if (vao_index == -1) {
        return NULL;
    }
    
    return get_inspect_vao_vec_data(inspector->vaos) + vao_index;
}

inspect_fb_t* inspect_find_fb_ptr(inspector_t* inspector, unsigned int fake) {
    int fb_index = inspect_find_fb(inspector, fake);
    if (fb_index == -1) {
        return NULL;
    }
    
    return get_inspect_fb_vec_data(inspector->framebuffers) + fb_index;
}

inspect_rb_t* inspect_find_rb_ptr(inspector_t* inspector, unsigned int fake) {
    int rb_index = inspect_find_rb(inspector, fake);
    if (rb_index == -1) {
        return NULL;
    }
    
    return get_inspect_rb_vec_data(inspector->renderbuffers) + rb_index;
}

static void update_inspection(inspector_t* inspector, inspect_gl_state_t* state) {
    inspect_act_vec_t actions = state->actions;
    for (inspect_action_t* action = actions->data; !vec_end(actions, action); action++)
        if (action->apply_func)
            action->apply_func(inspector, action);
    
    if (state->front.data)
        inspector->front_buf = &state->front;
    
    if (state->back.data)
        inspector->back_buf = &state->back;
    
    if (state->depth.data)
        inspector->depth_buf = &state->depth;
}

void seek_inspector(inspector_t* inspector, size_t frame_index, size_t cmd_index) {
    free_buffers(inspector);
    free_textures(inspector);
    free_shaders(inspector);
    free_programs(inspector);
    free_vaos(inspector);
    free_framebuffers(inspector);
    resize_vec(inspector->buffers, 0);
    resize_vec(inspector->textures, 0);
    resize_vec(inspector->shaders, 0);
    resize_vec(inspector->programs, 0);
    resize_vec(inspector->vaos, 0);
    resize_vec(inspector->framebuffers, 0);
    resize_vec(inspector->renderbuffers, 0);
    
    inspect_vao_t vao;
    vao.fake = 0;
    vao.attrib_count = 0;
    vao.attribs = NULL;
    append_inspect_vao_vec(inspector->vaos, &vao);
    
    inspector->back_buf = inspector->front_buf = inspector->depth_buf = NULL;
    
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

int inspect_find_tex(inspector_t* inspector, unsigned int tex) {
    inspect_tex_vec_t textures = inspector->textures;
    size_t count = get_inspect_tex_vec_count(textures);
    for (size_t i = 0; i < count; ++i)
        if (get_inspect_tex_vec(textures, i)->fake == tex)
            return i;
    
    return -1;
}

int inspect_find_buf(inspector_t* inspector, unsigned int buf) {
    inspect_buf_vec_t buffers = inspector->buffers;
    size_t count = get_inspect_buf_vec_count(buffers);
    for (size_t i = 0; i < count; ++i)
        if (get_inspect_buf_vec(buffers, i)->fake == buf)
            return i;
    
    return -1;
}

int inspect_find_shdr(inspector_t* inspector, unsigned int shdr) {
    inspect_shdr_vec_t shaders = inspector->shaders;
    size_t count = get_inspect_shdr_vec_count(shaders);
    for (size_t i = 0; i < count; ++i)
        if (get_inspect_shdr_vec(shaders, i)->fake == shdr)
            return i;
    
    return -1;
}

int inspect_find_prog(inspector_t* inspector, unsigned int prog) {
    inspect_prog_vec_t programs = inspector->programs;
    size_t count = get_inspect_prog_vec_count(programs);
    for (size_t i = 0; i < count; ++i)
        if (get_inspect_prog_vec(programs, i)->fake == prog)
            return i;
    
    return -1;
}

int inspect_find_vao(inspector_t* inspector, unsigned int vao) {
    inspect_vao_vec_t vaos = inspector->vaos;
    size_t count = get_inspect_vao_vec_count(vaos);
    for (size_t i = 0; i < count; ++i)
        if (get_inspect_vao_vec(vaos, i)->fake == vao)
            return i;
    
    return -1;
}

int inspect_find_fb(inspector_t* inspector, unsigned int fb) {
    inspect_fb_vec_t fbs = inspector->framebuffers;
    size_t count = get_inspect_fb_vec_count(fbs);
    for (size_t i = 0; i < count; ++i)
        if (get_inspect_fb_vec(fbs, i)->fake == fb)
            return i;
    
    return -1;
}

int inspect_find_rb(inspector_t* inspector, unsigned int rb) {
    inspect_rb_vec_t rbs = inspector->renderbuffers;
    size_t count = get_inspect_rb_vec_count(rbs);
    for (size_t i = 0; i < count; ++i)
        if (get_inspect_rb_vec(rbs, i)->fake == rb)
            return i;
    
    return -1;
}
