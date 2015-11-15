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

static void update_context(uint64_t *context,
                           const trace_t* trace,
                           trace_command_t* trace_cmd) {
    const char *name = trace->func_names[trace_cmd->func_index];
    
    if (strcmp(name, "glXMakeCurrent") == 0) {
        *context = trace_cmd->args->next->next->val.ptr[0];
    }
}

inspection_t* create_inspection(const trace_t* trace) {
    inspection_t* result = malloc(sizeof(inspection_t));
    result->frames = NULL;
    result->trace = trace;
    
    trace_frame_t* frame = trace->frames;
    uint64_t context = 0;
    while (frame) {
        inspect_frame_t* new_frame = malloc(sizeof(inspect_frame_t));
        new_frame->next = NULL;
        new_frame->commands = NULL;
        new_frame->trace_frame = frame;
        
        trace_command_t* command = frame->commands;
        while (command) {
            update_context(&context, trace, command);
            
            inspect_command_t* new_command = malloc(sizeof(inspect_command_t));
            
            new_command->trace_cmd = command;
            new_command->attachments = NULL;
            new_command->name = trace->func_names[command->func_index];
            new_command->gl_context = context;
            new_command->next = NULL;
            
            if (!new_frame->commands) {
                new_frame->commands = new_command;
            } else
            {
                inspect_command_t* current = new_frame->commands;
                while (current->next) current = current->next;
                current->next = new_command;
            }
            
            command = command->next;
        }
        
        if (!result->frames) {
            result->frames = new_frame;
        } else
        {
            inspect_frame_t* current = result->frames;
            while (current->next) current = current->next;
            current->next = new_frame;
        }
        
        frame = frame->next;
    }
    
    return result;
}

void free_inspection(inspection_t* inspection) {
    inspect_frame_t* frame = inspection->frames;
    while (frame) {
        inspect_command_t* command = frame->commands;
        while (command) {
            inspect_command_t* next_command = command->next;
            
            inspect_attachment_t* attach = command->attachments;
            while (attach) {
                inspect_attachment_t* next_attach = attach->next;
                free(attach->message);
                free(attach);
                attach = next_attach;
            }
            
            free(command);
            
            command = next_command;
        }
        
        inspect_frame_t* next_frame = frame->next;
        free(frame);
        frame = next_frame;
    }
    
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

static void validate_command(inspect_command_t* command) {
    //Validate enum argument values
    trace_arg_t* arg = command->trace_cmd->args;
    while (arg) {
        if (arg->val.group == NULL ? false : (arg->val.group[0] != 0)) {
            const glapi_group_t* group = find_group(arg->val.group);
            
            if (!group) {
            } else if (group->bitmask) {
                //TODO
            } else {
                bool valid = false;
                for (size_t i = 0; i < group->entry_count; i++) {
                    const glapi_group_entry_t *entry = group->entries[i];
                    //TODO: Requirements
                    if (entry->value == arg->val.u64[0]) {
                        valid = true;
                    }
                }
                
                if (!valid) {
                    inspect_add_error(command,
                                      "Invalid enum value %d for enum \"%s\".",
                                      arg->val.u64[0],
                                      group->name);
                }
            }
        }
        
        arg = arg->next;
    }
}

void inspect(inspection_t* inspection) {
    inspect_frame_t* frame = inspection->frames;
    while (frame) {
        inspect_command_t* command = frame->commands;
        while (command) {
            validate_command(command);
            command = command->next;
        }
        frame = frame->next;
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
