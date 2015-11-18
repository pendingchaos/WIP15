#include "libinspect.h"
#include "libtrace.h"
#include "glapi.h"

#include <string.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <GL/gl.h>

static const glapi_group_t* find_group(const char *name) {
    for (size_t i = 0; i < glapi.group_count; i++) {
        if (strcmp(glapi.groups[i]->name, name) == 0) {
            return glapi.groups[i];
        }
    }
    
    return NULL;
}

static void write_value(FILE* file, trace_value_t value) {
    if (value.group == NULL ? false : (value.group[0] != 0)) {
        const glapi_group_t* group = find_group(value.group);
        
        if (!group) {
        } else if (group->bitmask) {
            //TODO
        } else {
            for (size_t i = 0; i < group->entry_count; i++) {
                const glapi_group_entry_t *entry = group->entries[i];
                if (entry->value == value.u64[0]) {
                    fprintf(file, entry->name);
                    fprintf(file, "(%d)", value.u64[0]);
                    return;
                }
            }
        }
    }
    
    if (value.count > 1) fprintf(file, "[");
    for (size_t i = 0; i < value.count; ++i) {
        switch (value.type) {
        case Type_Void: {
            fprintf(file, "void");
            break;
        }
        case Type_UInt: {
            fprintf(file, "%llu", value.u64[i]);
            break;
        }
        case Type_Int: {
            fprintf(file, "%lli", value.i64[i]);
            break;
        }
        case Type_Double: {
            fprintf(file, "%f", value.dbl[i]);
            break;
        }
        case Type_Boolean: {
            fprintf(file, value.bl[i] ? "true" : "false");
            break;
        }
        case Type_Str: {
            fprintf(file, "'%s'", value.str[i]);
            break;
        }
        case Type_Bitfield: {
            fprintf(file, "%u", value.bitfield[i]);
            break;
        }
        case Type_FunctionPtr: {
            fprintf(file, "<fn ptr>");
            break;
        }
        case Type_Ptr: {
            fprintf(file, "0x%llx", value.ptr[i]);
            break;
        }
        }
        
        if (i != value.count-1) {
            fprintf(file, ", ");
        }
    }
    if (value.count > 1) fprintf(file, "]");
}

static void write_command(FILE* file, inspect_command_t* command) {
    trace_command_t* trace_cmd = command->trace_cmd;
    
    fprintf(file, "%s(", command->name);
    
    size_t count = get_vec_size(command->trace_cmd->args) / sizeof(trace_arg_t);
    for (size_t i = 0; i < count; ++i) {
        trace_arg_t* arg = ((trace_arg_t*)get_vec_data(command->trace_cmd->args)) + i;
        
        write_value(file, arg->val);
        if (i != count-1) {
            fprintf(file, ", ");
        }
    }
    
    fprintf(file, ")");
    
    if (trace_cmd->ret.type != Type_Void) {
        fprintf(file, " = ");
        write_value(file, trace_cmd->ret);
    }
}

static void write_state(FILE* file, const inspect_gl_state_t* state) {
    fprintf(file, "<ul>");
    
    inspect_gl_state_entry_t* entry = state->entries;
    while (entry) {
        fprintf(file, "<li>%s = ", entry->name);
        write_value(file, entry->val);
        fprintf(file, "</li>");
        entry = entry->next;
    }
    
    fprintf(file, "</ul>");
}

static void write_frame(FILE* frame_file, char *output_dir, inspect_frame_t* frame) {
    static size_t command_counter = 0;
    
    fprintf(frame_file, "<ul>");
    
    inspect_command_t* command = frame->commands;
    while (command) {
        bool error = false;
        bool warning = false;
        bool info = false;
        
        inspect_attachment_t* attachment = command->attachments;
        while (attachment) {
            error = error || attachment->type == AttachType_Error;
            warning = warning || attachment->type == AttachType_Warning;
            info = info || attachment->type == AttachType_Info;
            attachment = attachment->next;
        }
        
        fprintf(frame_file, "<li><a href=\"command_%zu.html\">", command_counter);
        
        if (error)
            fprintf(frame_file, "<u style=\"background:red;color:red\">____</u>");
        else if (warning)
            fprintf(frame_file, "<u style=\"background:yellow;color:yellow\">____</u>");
        else if (info)
            fprintf(frame_file, "<u style=\"background:green;color:green\">____</u>");
        
        write_command(frame_file, command);
        
        fprintf(frame_file, "</a></li>");
        
        char command_filename[FILENAME_MAX];
        memset(command_filename, 0, FILENAME_MAX);
        snprintf(command_filename, FILENAME_MAX, "%s/command_%zu.html", output_dir, command_counter);
        
        FILE *command_file = fopen(command_filename, "w");
        fprintf(command_file, "<h1>");
        write_command(command_file, command);
        fprintf(command_file, "</h1>");
        
        fprintf(command_file,
                "<h3>CPU: %f ms<br/>GPU: %f ms</h3>",
                command->cpu_duration/1000000.0,
                command->gpu_duration/1000000.0);
        
        fprintf(command_file, "</h1><ul>");
        
        attachment = command->attachments;
        while (attachment) {
            fprintf(command_file, "<li>");
            
            if (attachment->type == AttachType_Error)
                fprintf(command_file, "<u style=\"background:red;color:red\">____</u>");
            else if (attachment->type == AttachType_Warning)
                fprintf(command_file, "<u style=\"background:yellow;color:yellow\">____</u>");
            else if (attachment->type == AttachType_Info)
                fprintf(command_file, "<u style=\"background:green;color:green\">____</u>");
            
            fprintf(command_file, attachment->message);
            
            fprintf(command_file, "</li>");
            
            attachment = attachment->next;
        }
        
        fprintf(command_file, "</ul>");
        
        write_state(command_file, &command->state);
        
        fclose(command_file);
        
        command = command->next;
        ++command_counter;
    }
    
    fprintf(frame_file, "</ul>");
}

int main(int argc, char** argv) {
    if (argc < 3) {
        fprintf(stderr, "Usage: inspect <trace> <output dir>\n");
        return EXIT_FAILURE;
    }
    
    trace_t* trace = load_trace(argv[1]);
    
    trace_error_t error = get_trace_error();
    if (error == TraceError_Invalid) {
        fprintf(stderr, "Invalid trace file: %s\n", get_trace_error_desc());
        return EXIT_FAILURE;
    } else if (error == TraceError_UnableToOpen) {
        fprintf(stderr, "Unable to open trace file.\n");
        return EXIT_FAILURE;
    }
    
    inspection_t* inspection = create_inspection(trace);
    inspect(inspection);
    
    char index_filename[FILENAME_MAX];
    strcpy(index_filename, argv[2]);
    strcat(index_filename, "/");
    strcat(index_filename, "index.html");
    
    FILE *index = fopen(index_filename, "w");
    if (index == NULL) {
        fprintf(stderr, "Unable to create file \"%s\"\n", index_filename);
        return EXIT_FAILURE;
    }
    
    fprintf(index, "<ul>");
    
    inspect_frame_t* frame = inspection->frames;
    size_t i = 0;
    while (frame) {
        char frame_filename[FILENAME_MAX];
        memset(frame_filename, 0, FILENAME_MAX);
        snprintf(frame_filename, FILENAME_MAX, "%s/frame_%zu.html", argv[2], i);
        
        bool error = false;
        bool warning = false;
        bool info = false;
        
        inspect_command_t* command = frame->commands;
        while (command) {
            inspect_attachment_t* attachment = command->attachments;
            while (attachment) {
                error = error || attachment->type == AttachType_Error;
                warning = warning || attachment->type == AttachType_Warning;
                info = info || attachment->type == AttachType_Info;
                attachment = attachment->next;
            }
            command = command->next;
        }
        
        fprintf(index, "<li>");
        
        if (error)
            fprintf(index, "<u style=\"background:red;color:red\">____</u>");
        else if (warning)
            fprintf(index, "<u style=\"background:yellow;color:yellow\">____</u>");
        else if (info)
            fprintf(index, "<u style=\"background:green;color:green\">____</u>");
        
        fprintf(index, "<a href=\"frame_%zu.html\">Frame %zu</a></li>", i, i);
        
        FILE *frame_file = fopen(frame_filename, "w");
        if (frame_file == NULL) {
            fprintf(stderr, "Unable to create file \"%s\"\n", frame_filename);
            return EXIT_FAILURE;
        }
        
        write_frame(frame_file, argv[2], frame);
        fclose(frame_file);
        
        frame = frame->next;
        i++;
    }
    
    fprintf(index, "</ul>");
    
    fclose(index);
    
    free_inspection(inspection);
    free_trace(trace);
    
    return EXIT_SUCCESS;
}
