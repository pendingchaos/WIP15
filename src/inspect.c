#include "libinspect.h"
#include "libtrace.h"
#include "glapi.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

#include <string.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <inttypes.h>
#include <GL/gl.h>

static const glapi_group_t* find_group(const char* name) {
    for (size_t i = 0; i < glapi.group_count; i++) {
        if (strcmp(glapi.groups[i]->name, name) == 0) {
            return glapi.groups[i];
        }
    }
    
    return NULL;
}

static const char* get_enum_str(const char* group_name, unsigned int val) {
    if (!group_name) {
        for (size_t i = 0; i < glapi.group_count; i++) {
            const glapi_group_t* group = glapi.groups[i];
            
            for (size_t i = 0; i < group->entry_count; i++) {
                const glapi_group_entry_t *entry = group->entries[i];
                if (entry->value == val)
                    return entry->name;
            }
        }
        
        return "(unable to find string)";
    } else {
        const glapi_group_t* group = find_group(group_name);
        
        if (!group)
            return "(unable to find string)";
        
        for (size_t i = 0; i < group->entry_count; i++) {
            const glapi_group_entry_t *entry = group->entries[i];
            if (entry->value == val)
                return entry->name;
        }
        
        return "(unable to find string)";
    }
}

static void write_value(FILE* file, trace_value_t value, trace_t* trace) {
    if (value.group_index < 0 ? false : (trace->group_names[value.group_index][0] != 0)) {
        const glapi_group_t* group = find_group(trace->group_names[value.group_index]);
        
        if (!group) {
        } else if (group->bitmask) {
            //TODO
        } else {
            for (size_t i = 0; i < group->entry_count; i++) {
                const glapi_group_entry_t *entry = group->entries[i];
                uint64_t val = (value.type==Type_Boolean) ?
                               *trace_get_bool(&value) :
                               *trace_get_uint(&value);
                if (entry->value == val) {
                    fprintf(file, entry->name);
                    fprintf(file, "(%zu)", val);
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
            fprintf(file, "%"PRIu64, trace_get_uint(&value)[i]);
            break;
        }
        case Type_Int: {
            fprintf(file, "%"PRId64, trace_get_int(&value)[i]);
            break;
        }
        case Type_Double: {
            fprintf(file, "%f", trace_get_double(&value)[i]);
            break;
        }
        case Type_Boolean: {
            fprintf(file, trace_get_bool(&value)[i] ? "true" : "false");
            break;
        }
        case Type_Str: {
            fprintf(file, "'%s'", trace_get_str(&value)[i]);
            break;
        }
        case Type_Bitfield: {
            fprintf(file, "%u", trace_get_bitfield(&value)[i]);
            break;
        }
        case Type_FunctionPtr: {
            fprintf(file, "<fn ptr>");
            break;
        }
        case Type_Ptr: {
            fprintf(file, "0x%"PRIx64, trace_get_ptr(&value)[i]);
            break;
        }
        case Type_Data: {
            fprintf(file, "(data)");
            break;
        }
        }
        
        if (i != value.count-1) {
            fprintf(file, ", ");
        }
    }
    if (value.count > 1) fprintf(file, "]");
}

static void write_command(FILE* file, inspect_command_t* command, trace_t* trace) {
    trace_command_t* trace_cmd = command->trace_cmd;
    
    fprintf(file, "%s(", command->name);
    
    size_t count = get_vec_size(command->trace_cmd->args) / sizeof(trace_arg_t);
    for (size_t i = 0; i < count; ++i) {
        trace_arg_t* arg = ((trace_arg_t*)get_vec_data(command->trace_cmd->args)) + i;
        
        write_value(file, arg->val, trace);
        if (i != count-1) {
            fprintf(file, ", ");
        }
    }
    
    fprintf(file, ")");
    
    if (trace_cmd->ret.type != Type_Void) {
        fprintf(file, " = ");
        write_value(file, trace_cmd->ret, trace);
    }
}

static int current_color_image = -1;
static int current_depth_image = -1;
static unsigned int next_image_id = 0;

static void write_color_image(char *output_dir, const inspect_image_t* image, unsigned int id) {
    char filename[FILENAME_MAX];
    memset(filename, 0, FILENAME_MAX);
    snprintf(filename, FILENAME_MAX, "%s/image_%u.bmp", output_dir, id);
    
    stbi_write_bmp(filename, image->width, image->height, 4, image->data);
}

static void write_depth_image(char *output_dir, const inspect_image_t* image, unsigned int id) {
    write_color_image(output_dir, image, id);
}

typedef struct {
    GLuint fake;
    inspect_gl_tex_params_t params;
    size_t mipmap_count;
    void** mipmaps;
    char** mipmap_filenames;
} texture_t;

static vec_t textures; //texture_t

texture_t* find_texture(GLuint fake) {
    size_t count = get_vec_size(textures)/sizeof(texture_t);
    for (size_t i = 0; i < count; ++i) {
        texture_t* tex = (texture_t*)get_vec_data(textures) + i;
        if (tex->fake == fake)
            return tex;
    }
    
    texture_t tex;
    tex.fake = fake;
    tex.params.fake_texture = fake;
    tex.params.width = 0;
    tex.params.height = 0;
    tex.mipmap_count = 0;
    tex.mipmaps = NULL;
    tex.mipmap_filenames = NULL;
    append_vec(textures, sizeof(texture_t), &tex);
    return (texture_t*)get_vec_data(textures) + count;
}

static void write_state(FILE* file, char *output_dir, const inspect_gl_state_t* state, trace_t* trace) {
    fprintf(file, "<ul>");
    
    vec_t entries = state->entries;
    size_t count = get_vec_size(entries)/sizeof(inspect_gl_state_entry_t);
    for (size_t i = 0; i < count; ++i) {
        inspect_gl_state_entry_t* entry = ((inspect_gl_state_entry_t*)get_vec_data(entries)) + i;
        
        fprintf(file, "<li>%s = ", entry->name);
        write_value(file, entry->val, trace);
        fprintf(file, "</li>");
        entry = entry->next;
    }
    
    vec_t tex_params = state->texture_params;
    count = get_vec_size(tex_params)/sizeof(inspect_gl_tex_params_t);
    for (size_t i = 0; i < count; ++i) {
        inspect_gl_tex_params_t* params = (inspect_gl_tex_params_t*)get_vec_data(tex_params) + i;
        texture_t* tex = find_texture(params->fake_texture);
        
        if (tex->params.width != params->width || tex->params.height != params->height) {
            size_t mipmap_count = 0;
            size_t w = params->width;
            size_t h = params->height;
            while ((w > 1) && (h > 1)) {
                ++mipmap_count;
                w /= 2;
                h /= 2;
            }
            
            for (size_t i = 0; i < tex->mipmap_count; ++i) {
                free(tex->mipmaps[i]);
                free(tex->mipmap_filenames[i]);
            }
            free(tex->mipmaps);
            free(tex->mipmap_filenames);
            
            tex->mipmap_count = mipmap_count;
            tex->mipmaps = malloc(tex->mipmap_count*sizeof(void*));
            tex->mipmap_filenames = malloc(tex->mipmap_count*sizeof(char*));
            for (size_t i = 0; i < mipmap_count; ++i) {
                tex->mipmaps[i] = NULL;
                tex->mipmap_filenames[i] = NULL;
            }
        }
        
        tex->params = *params;
    }
    
    vec_t tex_data = state->texture_data;
    count = get_vec_size(tex_data)/sizeof(inspect_gl_tex_data_t);
    for (size_t i = 0; i < count; ++i) {
        inspect_gl_tex_data_t* data = (inspect_gl_tex_data_t*)get_vec_data(tex_data) + i;
        texture_t* tex = find_texture(data->fake_texture);
        
        if (!tex->mipmaps[data->mipmap])
            tex->mipmaps[data->mipmap] = malloc(data->data_size);
        
        memcpy(tex->mipmaps[data->mipmap], data->data, data->data_size);
        
        size_t width = tex->params.width;
        size_t height = tex->params.height;
        for (size_t j = 0; j < data->mipmap; ++j) {
            width /= 2;
            height /= 2;
        }
        
        char filename[FILENAME_MAX];
        memset(filename, 0, FILENAME_MAX);
        snprintf(filename, FILENAME_MAX, "%s/image_%u.bmp", output_dir, next_image_id);
        stbi_write_bmp(filename, width, height, 4, data->data);
        
        free(tex->mipmap_filenames[data->mipmap]);
        
        memset(filename, 0, FILENAME_MAX);
        snprintf(filename, FILENAME_MAX, "image_%u.bmp", next_image_id++);
        
        tex->mipmap_filenames[data->mipmap] = malloc(strlen(filename)+1);
        strcpy(tex->mipmap_filenames[data->mipmap], filename);
    }
    
    count = get_vec_size(textures)/sizeof(texture_t);
    for (size_t i = 0; i < count; ++i) {
        texture_t* tex = (texture_t*)get_vec_data(textures) + i;
        fprintf(file, "<h3>Texture %u</h3><ul>", tex->fake);
        
        fprintf(file, "<li>type = %s</li>", get_enum_str("TextureTarget", tex->params.type));
        fprintf(file, "<li>min filter = %s</li>", get_enum_str("TextureMinFilter", tex->params.min_filter));
        fprintf(file, "<li>mag_filter = %s</li>", get_enum_str("TextureMagFilter", tex->params.mag_filter));
        fprintf(file, "<li>min LOD = %f</li>", tex->params.min_lod);
        fprintf(file, "<li>max LOD = %f</li>", tex->params.max_lod);
        fprintf(file, "<li>base_level = %d</li>", tex->params.base_level);
        fprintf(file, "<li>max_level = %d</li>", tex->params.max_level);
        fprintf(file, "<li>wrap S = %s</li>", get_enum_str("TextureWrapMode", tex->params.wrap_s));
        fprintf(file, "<li>wrap T = %s</li>", get_enum_str("TextureWrapMode", tex->params.wrap_t));
        fprintf(file, "<li>wrap R = %s</li>", get_enum_str("TextureWrapMode", tex->params.wrap_r));
        fprintf(file, "<li>priority = %f</li>", tex->params.priority);
        fprintf(file, "<li>compare mode = %s</li>", get_enum_str(NULL, tex->params.compare_mode));
        fprintf(file, "<li>compare_func = %s</li>", get_enum_str("DepthFunction", tex->params.compare_func));
        fprintf(file, "<li>depth texture mode = %s</li>", get_enum_str(NULL, tex->params.depth_texture_mode));
        fprintf(file, "<li>generate mipmap = %s</li>", tex->params.generate_mipmap ? "true" : "false");
        fprintf(file, "<li>depth stencil mode = %s</li>", get_enum_str(NULL, tex->params.depth_stencil_mode));
        fprintf(file, "<li>LOD bias = %f</li>", tex->params.lod_bias);
        fprintf(file, "<li>swizzle = [%s, %s, %s, %s]</li>",
                get_enum_str(NULL, tex->params.swizzle[0]),
                get_enum_str(NULL, tex->params.swizzle[1]),
                get_enum_str(NULL, tex->params.swizzle[2]),
                get_enum_str(NULL, tex->params.swizzle[3]));
        fprintf(file, "<li>border color = [%f, %f, %f, %f]</li>",
                tex->params.border_color[0],
                tex->params.border_color[1],
                tex->params.border_color[2],
                tex->params.border_color[3]);
        fprintf(file, "<li>width = %u</li>", tex->params.width);
        fprintf(file, "<li>height = %u</li>", tex->params.height);
        fprintf(file, "<li>depth = %u</li>", tex->params.depth);
        fprintf(file, "<li>internal format = %s</li>", get_enum_str(NULL, tex->params.internal_format));
        
        fprintf(file, "</ul>");
        
        for (size_t j = 0; j < tex->mipmap_count; ++j) {
            fprintf(file, "<img src=\"%s\"/>", tex->mipmap_filenames[j]);
        }
        
        fprintf(file, "<br/>");
    }
    
    fprintf(file, "</ul>");
    
    if (state->color.data) {
        current_color_image = next_image_id++;
        write_color_image(output_dir, &state->color, current_color_image);
    }
    
    if (state->depth.data) {
        current_depth_image = next_image_id++;
        write_depth_image(output_dir, &state->depth, current_depth_image);
    }
    
    if (current_color_image >= 0)
        fprintf(file, "<img src=\"image_%d.bmp\"/>", current_color_image);
    
    if (current_depth_image >= 0)
        fprintf(file, "<img src=\"image_%d.bmp\"/>", current_depth_image);
}

static void write_frame(FILE* frame_file, char *output_dir, inspect_frame_t* frame, trace_t* trace) {
    static size_t command_counter = 0;
    
    fprintf(frame_file, "<ul>");
    
    for (size_t i = 0; i < frame->command_count; ++i) {
        inspect_command_t* command = frame->commands + i;
        
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
        
        write_command(frame_file, command, trace);
        
        fprintf(frame_file, "</a></li>");
        
        char command_filename[FILENAME_MAX];
        memset(command_filename, 0, FILENAME_MAX);
        snprintf(command_filename, FILENAME_MAX, "%s/command_%zu.html", output_dir, command_counter);
        
        FILE *command_file = fopen(command_filename, "w");
        fprintf(command_file, "<h1>");
        write_command(command_file, command, trace);
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
        
        write_state(command_file, output_dir, &command->state, trace);
        
        fclose(command_file);
        
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
    
    textures = alloc_vec(0);
    
    for (size_t i = 0; i < inspection->frame_count; ++i) {
        inspect_frame_t* frame = inspection->frames + i;
        
        char frame_filename[FILENAME_MAX];
        memset(frame_filename, 0, FILENAME_MAX);
        snprintf(frame_filename, FILENAME_MAX, "%s/frame_%zu.html", argv[2], i);
        
        bool error = false;
        bool warning = false;
        bool info = false;
        
        for (size_t j = 0; j < frame->command_count; ++j) {
            inspect_attachment_t* attachment = frame->commands[j].attachments;
            while (attachment) {
                error = error || attachment->type == AttachType_Error;
                warning = warning || attachment->type == AttachType_Warning;
                info = info || attachment->type == AttachType_Info;
                attachment = attachment->next;
            }
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
        
        write_frame(frame_file, argv[2], frame, trace);
        fclose(frame_file);
    }
    
    size_t count = get_vec_size(textures)/sizeof(texture_t);
    for (size_t i = 0; i < count; ++i) {
        texture_t* tex = (texture_t*)get_vec_data(textures) + i;
        for (size_t j = 0; j < tex->mipmap_count; ++j) {
            free(tex->mipmaps[j]);
            free(tex->mipmap_filenames[j]);
        }
        free(tex->mipmaps);
        free(tex->mipmap_filenames);
    }
    free_vec(textures);
    
    fprintf(index, "</ul>");
    
    fclose(index);
    
    free_inspection(inspection);
    free_trace(trace);
    
    return EXIT_SUCCESS;
}
