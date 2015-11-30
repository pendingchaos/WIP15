#define GL_GLEXT_PROTOTYPES
#include "libinspect.h"
#include "libtrace.h"
#include "glapi.h"

#include <gtk/gtk.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <inttypes.h>
#include <assert.h>
#include <math.h>
#include <endian.h>
#include <GL/gl.h>

#define TYPE_FLOAT16 0
#define TYPE_FLOAT32 1
#define TYPE_FLOAT64 2
#define TYPE_UINT8 3
#define TYPE_INT8 4
#define TYPE_UINT16 5
#define TYPE_INT16 6
#define TYPE_UINT32 7
#define TYPE_INT32 8
#define TYPE_UINT64 9
#define TYPE_INT64 10

static GtkWidget* main_window;
static GtkBuilder* builder;
static GdkPixbuf* info_pixbuf;
static GdkPixbuf* warning_pixbuf;
static GdkPixbuf* error_pixbuf;
static inspector_t* inspector;
static trace_t* trace;

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

static char* static_format(const char* format, ...) {
    static char data[1024];
    memset(data, 0, 1024);
    
    va_list list;
    va_start(list, format);
    vsnprintf(data, 1024, format, list);
    va_end(list);
    
    return data;
}

static char* format_float(double val) {
    static char data[128];
    memset(data, 0, 128);
    
    snprintf(data, 128, "%f", val);
    
    char* c = data+127;
    while (true) {
        if (*c == '0') {
            *c = 0;
        } else if (*c == '.') {
            *c = 0;
            break;
        } else if (*c) {
            break;
        }
        c--;
    }
    
    return data;
}

static void format_value(char* str, trace_value_t value) {
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
                    strcat(str, static_format("%s(%zu)", entry->name, val));
                    return;
                }
            }
        }
    }
    
    if (value.count > 1) strcat(str, static_format("["));
    for (size_t i = 0; i < value.count; ++i) {
        switch (value.type) {
        case Type_Void: {
            strcat(str, static_format("void"));
            break;
        }
        case Type_UInt: {
            strcat(str, static_format("%"PRIu64, trace_get_uint(&value)[i]));
            break;
        }
        case Type_Int: {
            strcat(str, static_format("%"PRId64, trace_get_int(&value)[i]));
            break;
        }
        case Type_Double: {
            strcat(str, static_format("%s", format_float(trace_get_double(&value)[i])));
            break;
        }
        case Type_Boolean: {
            strcat(str, static_format(trace_get_bool(&value)[i] ? "true" : "false"));
            break;
        }
        case Type_Str: {
            strcat(str, static_format("'%s'", trace_get_str(&value)[i]));
            break;
        }
        case Type_Bitfield: {
            strcat(str, static_format("%u", trace_get_bitfield(&value)[i]));
            break;
        }
        case Type_FunctionPtr: {
            strcat(str, static_format("<function pointer>"));
            break;
        }
        case Type_Ptr: {
            strcat(str, static_format("0x%"PRIx64, trace_get_ptr(&value)[i]));
            break;
        }
        case Type_Data: {
            strcat(str, static_format("<data>"));
            break;
        }
        }
        
        if (i != value.count-1)
            strcat(str, static_format(", "));
    }
    if (value.count > 1) strcat(str, static_format("]"));
}

static void format_command(char* str, inspect_command_t* command) {
    trace_command_t* trace_cmd = command->trace_cmd;
    
    strcat(str, static_format("%s(", command->name));
    
    size_t count = get_vec_size(command->trace_cmd->args) / sizeof(trace_arg_t);
    for (size_t i = 0; i < count; ++i) {
        trace_arg_t* arg = ((trace_arg_t*)get_vec_data(command->trace_cmd->args)) + i;
        
        format_value(str, arg->val);
        if (i != count-1) {
            strcat(str, static_format(", "));
        }
    }
    
    strcat(str, static_format(")"));
    
    if (trace_cmd->ret.type != Type_Void) {
        strcat(str, static_format(" = "));
        format_value(str, trace_cmd->ret);
    }
}

static void init_trace_tree(GtkTreeView* tree,
                            inspection_t* inspection) {
    GtkTreeStore* store = gtk_tree_store_new(2, GDK_TYPE_PIXBUF, G_TYPE_STRING);
    
    for (size_t i = 0; i < inspection->frame_count; ++i) {
        inspect_frame_t* frame = inspection->frames + i;
        
        char frame_str[1024];
        memset(frame_str, 0, 1024);
        snprintf(frame_str, 1024, "Frame %zu", i);
        
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
        
        GdkPixbuf* pixbuf = NULL;
        if (error)
            pixbuf = error_pixbuf;
        else if (warning)
            pixbuf = warning_pixbuf;
        else if (info)
            pixbuf = info_pixbuf;
        
        GtkTreeIter frame_row;
        gtk_tree_store_append(store, &frame_row, NULL);
        gtk_tree_store_set(store, &frame_row, 0, pixbuf, 1, frame_str, -1);
        
        for (size_t j = 0; j < frame->command_count; ++j) {
            GtkTreeIter cmd_row;
            gtk_tree_store_append(store, &cmd_row, &frame_row);
            
            inspect_command_t* cmd = frame->commands + j;
            char cmd_str[1024];
            memset(cmd_str, 0, 1024);
            format_command(cmd_str, cmd);
            
            bool error = false;
            bool warning = false;
            bool info = false;
            inspect_attachment_t* attachment = cmd->attachments;
            while (attachment) {
                error = error || attachment->type == AttachType_Error;
                warning = warning || attachment->type == AttachType_Warning;
                info = info || attachment->type == AttachType_Info;
                attachment = attachment->next;
            }
            
            pixbuf = NULL;
            if (error)
                pixbuf = error_pixbuf;
            else if (warning)
                pixbuf = warning_pixbuf;
            else if (info)
                pixbuf = info_pixbuf;
            
            gtk_tree_store_set(store, &cmd_row, 0, pixbuf, 1, cmd_str, -1);
        }
    }
    
    gtk_tree_view_set_model(tree, GTK_TREE_MODEL(store));
    
    GtkCellRenderer* renderer = gtk_cell_renderer_pixbuf_new();
    GtkTreeViewColumn* column = gtk_tree_view_get_column(tree, 0);
    gtk_tree_view_column_pack_start(column, renderer, FALSE);
    gtk_tree_view_column_set_attributes(column, renderer, "pixbuf", 0, NULL);
    
    renderer = gtk_cell_renderer_text_new();
    column = gtk_tree_view_get_column(tree, 1);
    gtk_tree_view_column_pack_start(column, renderer, FALSE);
    gtk_tree_view_column_set_attributes(column, renderer, "text", 1, NULL);
}

static void init_state_tree(GtkTreeView* tree,
                            inspect_gl_state_t* state) {
    GtkTreeStore* store = GTK_TREE_STORE(gtk_tree_view_get_model(tree));
    gtk_tree_store_clear(store);
    
    vec_t entries = state->entries;
    size_t count = get_vec_size(entries)/sizeof(inspect_gl_state_entry_t);
    for (size_t i = 0; i < count; ++i) {
        inspect_gl_state_entry_t* entry = ((inspect_gl_state_entry_t*)get_vec_data(entries)) + i;
        
        char val_str[1024];
        memset(val_str, 0, 1024);
        format_value(val_str, entry->val);
        
        GtkTreeIter row;
        gtk_tree_store_append(store, &row, NULL);
        gtk_tree_store_set(store, &row, 0, entry->name, 1, val_str, -1);
    }
}

void update_buffer_view(size_t buf_index) {
    GtkSpinButton* stride_button = GTK_SPIN_BUTTON(gtk_builder_get_object(builder, "stride"));
    GtkSpinButton* offset_button = GTK_SPIN_BUTTON(gtk_builder_get_object(builder, "offset"));
    GtkSpinButton* components_button = GTK_SPIN_BUTTON(gtk_builder_get_object(builder, "components"));
    GtkComboBox* type_combobox = GTK_COMBO_BOX(gtk_builder_get_object(builder, "type_combobox"));
    
    uint32_t stride = gtk_spin_button_get_value(GTK_SPIN_BUTTON(stride_button));
    uint32_t offset = gtk_spin_button_get_value(GTK_SPIN_BUTTON(offset_button));
    uint32_t components = gtk_spin_button_get_value(GTK_SPIN_BUTTON(components_button));
    gint type = gtk_combo_box_get_active(GTK_COMBO_BOX(type_combobox));
    
    GtkTreeView* content = GTK_TREE_VIEW(gtk_builder_get_object(builder, "buffer_content_treeview"));
    GtkTreeStore* store = GTK_TREE_STORE(gtk_tree_view_get_model(content));
    gtk_tree_store_clear(store);
    
    if (type < 0)
        return;
    
    size_t type_size = 0;
    switch (type) {
    case TYPE_UINT8:
    case TYPE_INT8: {
        stride = stride?stride:1;
        type_size = 1;
        break;
    }
    case TYPE_UINT16:
    case TYPE_INT16:
    case TYPE_FLOAT16: {
        stride = stride?stride:2;
        type_size = 2;
        break;
    }
    case TYPE_UINT32:
    case TYPE_INT32:
    case TYPE_FLOAT32: {
        stride = stride?stride:4;
        type_size = 4;
        break;
    }
    case TYPE_UINT64:
    case TYPE_INT64:
    case TYPE_FLOAT64: {
        stride = stride?stride:8;
        type_size = 8;
        break;
    }
    default: {
        assert(false && "Unreachable code has been reached.");
    }
    }
    
    uint8_t* data = NULL;
    if (!inspect_get_buf_data(inspector, buf_index, (void**)&data))
        return;
    
    size_t buf_size = inspect_get_buf_size(inspector, buf_index);
    
    if (!data)
        return;
    
    while (offset+type_size <= buf_size) {
        GtkTreeIter row;
        gtk_tree_store_append(store, &row, NULL);
        
        char str[1024];
        memset(str, 0, 1024);
        
        char* cur = str;
        char* end = str+1024;
        
        if (components>1)
            cur += snprintf(cur, end-cur, "[");
        
        for (size_t i = 0; i < components; i++) {
            #define CE(v, fle, fbe) (trace->little_endian ? fle(v) : fbe(v))
            switch (type) {
            case TYPE_UINT8: {
                cur += snprintf(cur, end-cur, "%"PRIu8, *(uint8_t*)(data+offset));
                break;
            }
            case TYPE_INT8: {
                cur += snprintf(cur, end-cur, "%"PRId8, *(int8_t*)(data+offset));
                break;
            }
            case TYPE_UINT16: {
                uint16_t v = *(uint16_t*)(data+offset);
                cur += snprintf(cur, end-cur, "%"PRIu16, CE(v, le16toh, be16toh));
                break;
            }
            case TYPE_INT16: {
                int16_t v = *(int16_t*)(data+offset);
                cur += snprintf(cur, end-cur, "%"PRId16, CE(v, le16toh, be16toh));
                break;
            }
            case TYPE_UINT32: {
                uint32_t v = *(uint32_t*)(data+offset);
                cur += snprintf(cur, end-cur, "%"PRIu32, CE(v, le32toh, be32toh));
                break;
            }
            case TYPE_INT32: {
                int32_t v = *(int32_t*)(data+offset);
                cur += snprintf(cur, end-cur, "%"PRId32, CE(v, le32toh, be32toh));
                break;
            }
            case TYPE_UINT64: {
                uint64_t v = *(uint64_t*)(data+offset);
                cur += snprintf(cur, end-cur, "%"PRIu64, CE(v, le64toh, be64toh));
                break;
            }
            case TYPE_INT64: {
                int64_t v = *(int64_t*)(data+offset);
                cur += snprintf(cur, end-cur, "%"PRId64, CE(v, le64toh, be64toh));
                break;
            }
            case TYPE_FLOAT16: {
                //TODO
                break;
            }
            case TYPE_FLOAT32: {
                cur += snprintf(cur, end-cur, "%s", format_float(*(float*)(data+offset)));
                break;
            }
            case TYPE_FLOAT64: {
                cur += snprintf(cur, end-cur, "%s", format_float(*(double*)(data+offset)));
                break;
            }
            }
            
            offset += stride;
            
            bool at_end = offset+type_size > buf_size;
            
            if ((i != components-1) && !at_end)
                cur += snprintf(cur, end-cur, ", ");
            
            if (at_end)
                break;
        }
        
        if (components>1)
            cur += snprintf(cur, end-cur, "]");
        
        gtk_tree_store_set(store, &row, 0, str, -1);
    }
}

void quit_callback(GObject* obj, gpointer user_data) {
    gtk_main_quit();
}

void about_callback(GObject* obj, gpointer user_data) {
    gtk_show_about_dialog(GTK_WINDOW(main_window),
                          "program-name", "WIP15 Inspector",
                          "version", "0.0.0",
                          "website", "https://gitlab.com/pendingchaos/WIP15",
                          "logo-icon-name", "image-missing",
                          //"license-type", GTK_LICENSE_GPL_3_0_ONLY,
                          NULL);
}

void texture_select_callback(GObject* obj, gpointer user_data) {
    GtkTreeView* param_tree = GTK_TREE_VIEW(gtk_builder_get_object(builder, "selected_texture_treeview"));
    GtkTreeStore* param_store = GTK_TREE_STORE(gtk_tree_view_get_model(param_tree));
    
    GtkTreeView* image_tree = GTK_TREE_VIEW(gtk_builder_get_object(builder, "selected_texture_images"));
    GtkTreeStore* image_store = GTK_TREE_STORE(gtk_tree_view_get_model(image_tree));
    
    if (!param_store)
        return;
    if (!image_store)
        return;
    
    gtk_tree_store_clear(image_store);
    gtk_tree_store_clear(param_store);
    
    GtkTreePath* path;
    gtk_tree_view_get_cursor(GTK_TREE_VIEW(obj), &path, NULL);
    if (!path)
        return;
    
    //Initialize params
    size_t tex_index = gtk_tree_path_get_indices(path)[0];
    
    inspect_gl_tex_params_t params;
    if (!inspect_get_tex_params(inspector, tex_index, &params))
        return;
    
    if (params.type) {
        GtkTreeIter row;
        #define VAL(name, val) gtk_tree_store_append(param_store, &row, NULL);\
    gtk_tree_store_set(param_store, &row, 0, (name), 1, (val), -1);
        VAL("Type", static_format("%s", get_enum_str("TextureTarget", params.type)));
        VAL("Min Filter", static_format("%s", get_enum_str("TextureMinFilter", params.min_filter)));
        VAL("Mag Filter", static_format("%s", get_enum_str("TextureMagFilter", params.mag_filter)));
        VAL("Min LOD", static_format("%s", format_float(params.min_lod)));
        VAL("Max LOD", static_format("%s", format_float(params.max_lod)));
        VAL("Base Level", static_format("%d", params.base_level));
        VAL("Max Level", static_format("%d", params.max_level));
        VAL("Wrap S", static_format("%s", get_enum_str("TextureWrapMode", params.wrap_s)));
        VAL("Wrap T", static_format("%s", get_enum_str("TextureWrapMode", params.wrap_t)));
        VAL("Wrap R", static_format("%s", get_enum_str("TextureWrapMode", params.wrap_r)));
        VAL("Priority", static_format("%s", format_float(params.priority)));
        VAL("Compare Mode", static_format("%s", get_enum_str(NULL, params.compare_mode)));
        VAL("Compare Func", static_format("%s", get_enum_str("DepthFunction", params.compare_func)));
        VAL("Depth Texture Mode", static_format("%s", get_enum_str(NULL, params.depth_texture_mode)));
        VAL("Generate Mipmap", static_format("%s", params.generate_mipmap ? "true" : "false"));
        VAL("Depth Stencil Mode", static_format("%s", get_enum_str(NULL, params.depth_stencil_mode)));
        VAL("LOD bias", static_format("%s", format_float(params.lod_bias)));
        VAL("Swizzle", static_format("[%s, %s, %s, %s]",
                                     get_enum_str(NULL, params.swizzle[0]),
                                     get_enum_str(NULL, params.swizzle[1]),
                                     get_enum_str(NULL, params.swizzle[2]),
                                     get_enum_str(NULL, params.swizzle[3])));
        VAL("Border Color", static_format("[%s, %s, %s, %s]",
                                         format_float(params.border_color[0]),
                                         format_float(params.border_color[1]),
                                         format_float(params.border_color[2]),
                                         format_float(params.border_color[3])));
        VAL("Width", static_format("%u", params.width));
        VAL("Height", static_format("%u", params.height));
        VAL("Depth", static_format("%u", params.depth));
        VAL("Internal format", static_format("%s", get_enum_str(NULL, params.internal_format)));
        #undef VAL
        
        //Initialize images
        size_t level = 0;
        size_t w = params.width;
        size_t h = params.height;
        while ((w > 1) && (h > 1)) {
            void* data;
            inspect_get_tex_data(inspector, tex_index, level, &data);
            
            GtkTreeIter row;
            gtk_tree_store_append(image_store, &row, NULL);
            
            if (!data) {
                gtk_tree_store_set(image_store, &row, 0, static_format("%u", level), 1, NULL, -1);
            } else {
                GdkPixbuf* pixbuf = gdk_pixbuf_new(GDK_COLORSPACE_RGB, TRUE, 8, w, h);
                uint32_t* dest = (uint32_t*)gdk_pixbuf_get_pixels(pixbuf);
                for (size_t y = 0; y < h; y++) {
                    for (size_t x = 0; x < w; x++) {
                        dest[(h-1-y)*w+x] = ((uint32_t*)data)[y*w+x];
                    }
                }
                gtk_tree_store_set(image_store, &row, 0, static_format("%u", level), 1, pixbuf, -1);
                g_object_unref(pixbuf);
            }
            
            level++;
            w /= 2;
            h /= 2;
        }
    }
}

void update_buffer_view_callback(GObject* obj, gpointer user_data) {
    GObject* buf_tree_view = gtk_builder_get_object(builder, "buffers_treeview");
    GtkTreePath* path;
    gtk_tree_view_get_cursor(GTK_TREE_VIEW(buf_tree_view), &path, NULL);
    if (!path)
        return;
    
    //Initialize params
    size_t index = gtk_tree_path_get_indices(path)[0];
    
    update_buffer_view(index);
}

static void init_texture_list(GtkTreeView* tree) {
    GtkTreeStore* store = GTK_TREE_STORE(gtk_tree_view_get_model(tree));
    gtk_tree_store_clear(store);
    
    for (size_t i = 0; i < inspect_get_tex_count(inspector); ++i) {
        char str[64];
        memset(str, 0, 64);
        snprintf(str, 64, "%u", inspect_get_tex(inspector, i));
        
        GtkTreeIter row;
        gtk_tree_store_append(store, &row, NULL);
        gtk_tree_store_set(store, &row, 0, str, -1);
    }
}

static void init_buffer_list(GtkTreeView* tree) {
    GtkTreeView* content = GTK_TREE_VIEW(gtk_builder_get_object(builder, "buffer_content_treeview"));
    GtkTreeStore* store = GTK_TREE_STORE(gtk_tree_view_get_model(content));
    gtk_tree_store_clear(store);
    
    store = GTK_TREE_STORE(gtk_tree_view_get_model(tree));
    gtk_tree_store_clear(store);
    
    for (size_t i = 0; i < inspect_get_buf_count(inspector); ++i) {
        char str[64];
        memset(str, 0, 64);
        snprintf(str, 64, "%u", inspect_get_buf(inspector, i));
        
        GtkTreeIter row;
        gtk_tree_store_append(store, &row, NULL);
        gtk_tree_store_set(store, &row, 0, str, -1);
    }
}

static void init_shader_list(GtkTreeView* tree) {
    GtkTreeStore* store = GTK_TREE_STORE(gtk_tree_view_get_model(tree));
    gtk_tree_store_clear(store);
    
    for (size_t i = 0; i < inspect_get_shdr_count(inspector); ++i) {
        char str[64];
        memset(str, 0, 64);
        snprintf(str, 64, "%u", inspect_get_shdr(inspector, i));
        
        const char* type = "Unknown";
        switch (inspect_get_shdr_type(inspector, i)) {
        case GL_VERTEX_SHADER: {
            type = "Vertex";
            break;
        }
        case GL_FRAGMENT_SHADER: {
            type = "Fragment";
            break;
        }
        case GL_GEOMETRY_SHADER: {
            type = "Geometry";
            break;
        }
        case GL_TESS_CONTROL_SHADER: {
            type = "Tesselation Control";
            break;
        }
        case GL_TESS_EVALUATION_SHADER: {
            type = "Tesselation Evaluation";
            break;
        }
        case GL_COMPUTE_SHADER: {
            type = "Compute";
            break;
        }
        }
        
        GtkTreeIter row;
        gtk_tree_store_append(store, &row, NULL);
        gtk_tree_store_set(store, &row, 0, str, 1, type, -1);
    }
    
    GtkTextView* source_view = GTK_TEXT_VIEW(gtk_builder_get_object(builder, "selected_shader_textview"));
    GtkTextBuffer* source_buffer = gtk_text_view_get_buffer(source_view);
    gtk_text_buffer_set_text(source_buffer, "", -1);
}

static void init_program_list(GtkTreeView* tree) {
    GtkTreeView* view = GTK_TREE_VIEW(gtk_builder_get_object(builder, "program_shaders_view"));
    GtkTreeStore* store = GTK_TREE_STORE(gtk_tree_view_get_model(view));
    gtk_tree_store_clear(store);
    
    store = GTK_TREE_STORE(gtk_tree_view_get_model(tree));
    gtk_tree_store_clear(store);
    
    for (size_t i = 0; i < inspect_get_prog_count(inspector); ++i) {
        char str[64];
        memset(str, 0, 64);
        snprintf(str, 64, "%u", inspect_get_prog(inspector, i));
        
        GtkTreeIter row;
        gtk_tree_store_append(store, &row, NULL);
        gtk_tree_store_set(store, &row, 0, str, -1);
    }
}

static void init_framebuffer_tree(GtkTreeView* tree,
                                  size_t frame_index,
                                  size_t cmd_index,
                                  inspection_t* inspection) {
    inspect_image_t front;
    inspect_image_t back;
    inspect_image_t depth;
    front.data = NULL;
    back.data = NULL;
    depth.data = NULL;
    
    if (frame_index >= inspection->frame_count)
        return;
    
    for (size_t i = 0; i <= frame_index; ++i) {
        inspect_frame_t* frame = inspection->frames + i;
        
        if ((cmd_index >= frame->command_count) && (i == frame_index))
            return;
        
        size_t count = (i == frame_index) ? cmd_index+1 : frame->command_count;
        for (size_t j = 0; j < count; ++j) {
            inspect_command_t* cmd = frame->commands + j;
            
            if (cmd->state.front.data)
                front = cmd->state.front;
            
            if (cmd->state.back.data)
                back = cmd->state.back;
            
            if (cmd->state.depth.data)
                depth = cmd->state.depth;
        }
    }
    
    GtkTreeStore* store = GTK_TREE_STORE(gtk_tree_view_get_model(tree));
    gtk_tree_store_clear(store);
    
    if (front.data) {
        GdkPixbuf* front_buf = gdk_pixbuf_new(GDK_COLORSPACE_RGB,
                                              TRUE,
                                              8,
                                              front.width,
                                              front.height);
        uint32_t* data = (uint32_t*)gdk_pixbuf_get_pixels(front_buf);
        for (size_t y = 0; y < front.height; y++) {
            for (size_t x = 0; x < front.width; x++) {
                data[(front.height-1-y)*front.width+x] = ((uint32_t*)front.data)[y*front.width+x];
            }
        }
        
        GtkTreeIter row;
        gtk_tree_store_append(store, &row, NULL);
        gtk_tree_store_set(store, &row, 0, "Front", 1, front_buf, -1);
        
        g_object_unref(front_buf);
    }
    
    if (back.data) {
        GdkPixbuf* back_buf = gdk_pixbuf_new(GDK_COLORSPACE_RGB,
                                             TRUE,
                                             8,
                                             back.width,
                                             back.height);
        uint32_t* data = (uint32_t*)gdk_pixbuf_get_pixels(back_buf);
        for (size_t y = 0; y < back.height; y++) {
            for (size_t x = 0; x < back.width; x++) {
                data[(back.height-1-y)*back.width+x] = ((uint32_t*)back.data)[y*back.width+x];
            }
        }
        
        GtkTreeIter row;
        gtk_tree_store_append(store, &row, NULL);
        gtk_tree_store_set(store, &row, 0, "Back", 1, back_buf, -1);
        
        g_object_unref(back_buf);
    }
    
    if (depth.data) {
        GdkPixbuf* depth_buf = gdk_pixbuf_new(GDK_COLORSPACE_RGB,
                                              FALSE,
                                              8,
                                              depth.width,
                                              depth.height);
        
        uint8_t* data = (uint8_t*)gdk_pixbuf_get_pixels(depth_buf);
        for (size_t y = 0; y < depth.height; y++) {
            for (size_t x = 0; x < depth.width; x++) {
                size_t index = (depth.height-1-y)*depth.width + x;
                
                uint32_t val = ((uint32_t*)depth.data)[y*depth.width+x];
                val = val / 4294967296.0 * 16777216.0;
                data[index*3] = val % 256;
                data[index*3+1] = val % 65536 / 256;
                data[index*3+2] = val / 65536;
            }
        }
        
        GtkTreeIter row;
        gtk_tree_store_append(store, &row, NULL);
        gtk_tree_store_set(store, &row, 0, "Depth", 1, depth_buf, -1);
        
        g_object_unref(depth_buf);
    }
}

void command_select_callback(GObject* obj, gpointer user_data) {
    GtkTreePath* path;
    gtk_tree_view_get_cursor(GTK_TREE_VIEW(obj), &path, NULL);
    
    if (!path)
        return;
    
    if (gtk_tree_path_get_depth(path) == 2) {
        gint* indices = gtk_tree_path_get_indices(path);
        
        inspection_t* inspection = user_data;
        assert(indices[0] < inspection->frame_count);
        inspect_frame_t* frame = inspection->frames + indices[0];
        assert(indices[1] < frame->command_count);
        inspect_command_t* cmd = frame->commands + indices[1];
        
        seek_inspector(inspector, indices[0], indices[1]);
        init_texture_list(GTK_TREE_VIEW(gtk_builder_get_object(builder, "texture_list_treeview")));
        init_buffer_list(GTK_TREE_VIEW(gtk_builder_get_object(builder, "buffers_treeview")));
        init_shader_list(GTK_TREE_VIEW(gtk_builder_get_object(builder, "shader_list_treeview")));
        init_program_list(GTK_TREE_VIEW(gtk_builder_get_object(builder, "program_list_view")));
        
        GObject* view = gtk_builder_get_object(builder, "selected_command_attachments");
        GtkTreeStore* store = GTK_TREE_STORE(gtk_tree_view_get_model(GTK_TREE_VIEW(view)));
        gtk_tree_store_clear(store);
        
        inspect_attachment_t* attachment = cmd->attachments;
        while (attachment) {
            GtkTreeIter row;
            gtk_tree_store_append(store, &row, NULL);
            gtk_tree_store_set(store, &row, 0, attachment->message, -1);
            
            attachment = attachment->next;
        }
        
        init_state_tree(GTK_TREE_VIEW(gtk_builder_get_object(builder, "state_treeview")),
                        &cmd->state);
        
        init_framebuffer_tree(GTK_TREE_VIEW(gtk_builder_get_object(builder, "framebuffer_treeview")),
                              indices[0],
                              indices[1],
                              inspection);
    } else {
        GtkTreeView* tree = GTK_TREE_VIEW(gtk_builder_get_object(builder, "state_treeview"));
        GtkTreeStore* store = GTK_TREE_STORE(gtk_tree_view_get_model(tree));
        gtk_tree_store_clear(store);
    }
}

void shader_select_callback(GObject* obj, gpointer user_data) {
    GtkTreePath* path;
    gtk_tree_view_get_cursor(GTK_TREE_VIEW(obj), &path, NULL);
    
    if (!path)
        return;
    
    size_t index = gtk_tree_path_get_indices(path)[0];
    
    GtkTextView* source_view = GTK_TEXT_VIEW(gtk_builder_get_object(builder, "selected_shader_textview"));
    GtkTextBuffer* source_buffer = gtk_text_view_get_buffer(source_view);
    
    char* source;
    inspect_get_shdr_source(inspector, index, &source);
    
    gtk_text_buffer_set_text(source_buffer, source, -1);
}

void program_select_callback(GObject* obj, gpointer user_data) {
    GtkTreePath* path;
    gtk_tree_view_get_cursor(GTK_TREE_VIEW(obj), &path, NULL);
    
    if (!path)
        return;
    
    size_t index = gtk_tree_path_get_indices(path)[0];
    
    GtkTreeView* view = GTK_TREE_VIEW(gtk_builder_get_object(builder, "program_shaders_view"));
    GtkTreeStore* store = GTK_TREE_STORE(gtk_tree_view_get_model(view));
    gtk_tree_store_clear(store);
    
    vec_t shaders;
    inspect_get_prog_shaders(inspector, index, &shaders);
    
    size_t count = get_vec_size(shaders) / sizeof(unsigned int);
    for (size_t i = 0; i < count; i++) {
        char str[64];
        memset(str, 0, 64);
        snprintf(str, 64, "%u", ((unsigned int*)get_vec_data(shaders))[i]);
        
        GtkTreeIter row;
        gtk_tree_store_append(store, &row, NULL);
        gtk_tree_store_set(store, &row, 0, str, -1);
    }
}

int main(int argc, char** argv) {
    if (argc != 2) {
        fprintf(stderr, "Expected one argument. Got %d.\n", argc);
        fprintf(stderr, "Usage: inspect-gui <trace file>\n");
        return EXIT_FAILURE;
    }
    
    trace = load_trace(argv[1]);
    {
        trace_error_t error = get_trace_error();
        if (error == TraceError_Invalid) {
            fprintf(stderr, "Invalid trace file: %s\n", get_trace_error_desc());
            return EXIT_FAILURE;
        } else if (error == TraceError_UnableToOpen) {
            fprintf(stderr, "Unable to open trace file.\n");
            return EXIT_FAILURE;
        }
    }
    
    inspection_t* inspection = create_inspection(trace);
    inspect(inspection);
    
    inspector = create_inspector(inspection);
    
    gtk_init(&argc, &argv);
    
    GtkIconTheme* icon_theme = gtk_icon_theme_new();
    {
        GError* error = NULL;
        info_pixbuf = gtk_icon_theme_load_icon(icon_theme, "dialog-information", 16, 0, &error);
        if (error) {
            fprintf(stderr, "Unable to load information icon: %s\n", error->message);
            g_error_free(error);
            free_inspection(inspection);
            free_trace(trace);
            return EXIT_FAILURE;
        }
        warning_pixbuf = gtk_icon_theme_load_icon(icon_theme, "dialog-warning", 16, 0, &error);
        if (error) {
            fprintf(stderr, "Unable to load information icon: %s\n", error->message);
            g_object_unref(info_pixbuf);
            g_error_free(error);
            free_inspection(inspection);
            free_trace(trace);
            return EXIT_FAILURE;
        }
        error_pixbuf = gtk_icon_theme_load_icon(icon_theme, "dialog-error", 16, 0, &error);
        if (error) {
            fprintf(stderr, "Unable to load information icon: %s\n", error->message);
            g_object_unref(warning_pixbuf);
            g_object_unref(info_pixbuf);
            g_error_free(error);
            free_inspection(inspection);
            free_trace(trace);
            return EXIT_FAILURE;
        }
    }
    
    builder = gtk_builder_new_from_file("ui.glade");
    gtk_builder_connect_signals(builder, inspection);
    
    //Initialize trace view
    GObject* trace_view = gtk_builder_get_object(builder, "trace_view");
    init_trace_tree(GTK_TREE_VIEW(trace_view), inspection);
    
    //Initialize attachments view
    GObject* attachments_view = gtk_builder_get_object(builder, "selected_command_attachments");
    GtkTreeStore* store = gtk_tree_store_new(1, G_TYPE_STRING);
    gtk_tree_view_set_model(GTK_TREE_VIEW(attachments_view),
                            GTK_TREE_MODEL(store));
    g_object_unref(store);
    GtkCellRenderer* renderer = gtk_cell_renderer_text_new();
    GtkTreeViewColumn* column = gtk_tree_view_get_column(GTK_TREE_VIEW(attachments_view), 0);
    gtk_tree_view_column_pack_start(column, renderer, FALSE);
    gtk_tree_view_column_set_attributes(column, renderer, "text", 0, NULL);
    
    //Initialize state view
    GObject* state_view = gtk_builder_get_object(builder, "state_treeview");
    store = gtk_tree_store_new(2, G_TYPE_STRING, G_TYPE_STRING);
    gtk_tree_view_set_model(GTK_TREE_VIEW(state_view),
                            GTK_TREE_MODEL(store));
    g_object_unref(store);
    renderer = gtk_cell_renderer_text_new();
    column = gtk_tree_view_get_column(GTK_TREE_VIEW(state_view), 0);
    gtk_tree_view_column_pack_start(column, renderer, FALSE);
    gtk_tree_view_column_set_attributes(column, renderer, "text", 0, NULL);
    column = gtk_tree_view_get_column(GTK_TREE_VIEW(state_view), 1);
    gtk_tree_view_column_pack_start(column, renderer, FALSE);
    gtk_tree_view_column_set_attributes(column, renderer, "text", 1, NULL);
    
    //Initialize texture list view
    GObject* tex_list_view = gtk_builder_get_object(builder, "texture_list_treeview");
    store = gtk_tree_store_new(1, G_TYPE_STRING);
    gtk_tree_view_set_model(GTK_TREE_VIEW(tex_list_view),
                            GTK_TREE_MODEL(store));
    g_object_unref(store);
    renderer = gtk_cell_renderer_text_new();
    column = gtk_tree_view_get_column(GTK_TREE_VIEW(tex_list_view), 0);
    gtk_tree_view_column_pack_start(column, renderer, FALSE);
    gtk_tree_view_column_set_attributes(column, renderer, "text", 0, NULL);
    
    //Initialize texture state view
    GObject* tex_state_view = gtk_builder_get_object(builder, "selected_texture_treeview");
    store = gtk_tree_store_new(2, G_TYPE_STRING, G_TYPE_STRING);
    gtk_tree_view_set_model(GTK_TREE_VIEW(tex_state_view),
                            GTK_TREE_MODEL(store));
    g_object_unref(store);
    renderer = gtk_cell_renderer_text_new();
    column = gtk_tree_view_get_column(GTK_TREE_VIEW(tex_state_view), 0);
    gtk_tree_view_column_pack_start(column, renderer, FALSE);
    gtk_tree_view_column_set_attributes(column, renderer, "text", 0, NULL);
    column = gtk_tree_view_get_column(GTK_TREE_VIEW(tex_state_view), 1);
    gtk_tree_view_column_pack_start(column, renderer, FALSE);
    gtk_tree_view_column_set_attributes(column, renderer, "text", 1, NULL);
    
    //Initialize texture image view
    GObject* tex_image_view = gtk_builder_get_object(builder, "selected_texture_images");
    store = gtk_tree_store_new(2, G_TYPE_STRING, GDK_TYPE_PIXBUF);
    gtk_tree_view_set_model(GTK_TREE_VIEW(tex_image_view),
                            GTK_TREE_MODEL(store));
    g_object_unref(store);
    renderer = gtk_cell_renderer_text_new();
    column = gtk_tree_view_get_column(GTK_TREE_VIEW(tex_image_view), 0);
    gtk_tree_view_column_pack_start(column, renderer, FALSE);
    gtk_tree_view_column_set_attributes(column, renderer, "text", 0, NULL);
    renderer = gtk_cell_renderer_pixbuf_new();
    column = gtk_tree_view_get_column(GTK_TREE_VIEW(tex_image_view), 1);
    gtk_tree_view_column_pack_start(column, renderer, FALSE);
    gtk_tree_view_column_set_attributes(column, renderer, "pixbuf", 1, NULL);
    
    //Initialize texture framebuffer view
    GObject* fb_view = gtk_builder_get_object(builder, "framebuffer_treeview");
    store = gtk_tree_store_new(2, G_TYPE_STRING, GDK_TYPE_PIXBUF);
    gtk_tree_view_set_model(GTK_TREE_VIEW(fb_view),
                            GTK_TREE_MODEL(store));
    g_object_unref(store);
    renderer = gtk_cell_renderer_text_new();
    column = gtk_tree_view_get_column(GTK_TREE_VIEW(fb_view), 0);
    gtk_tree_view_column_pack_start(column, renderer, FALSE);
    gtk_tree_view_column_set_attributes(column, renderer, "text", 0, NULL);
    renderer = gtk_cell_renderer_pixbuf_new();
    column = gtk_tree_view_get_column(GTK_TREE_VIEW(fb_view), 1);
    gtk_tree_view_column_pack_start(column, renderer, FALSE);
    gtk_tree_view_column_set_attributes(column, renderer, "pixbuf", 1, NULL);
    
    //Initialize buffer type combobox
    GObject* buf_type = gtk_builder_get_object(builder, "type_combobox");
    GtkTreeStore* buf_type_store = gtk_tree_store_new(1, G_TYPE_STRING);
    gtk_combo_box_set_model(GTK_COMBO_BOX(buf_type), GTK_TREE_MODEL(buf_type_store));
    //gtk_tree_view_set_model(GTK_TREE_VIEW(fb_view), GTK_TREE_MODEL(buf_type_store));
    //renderer = gtk_cell_renderer_text_new();
    //column = gtk_tree_view_get_column(GTK_TREE_VIEW(buf_type), 0);
    //gtk_tree_view_column_pack_start(column, renderer, FALSE);
    //gtk_tree_view_column_set_attributes(column, renderer, "text", 0, NULL);
    renderer = gtk_cell_renderer_text_new();
    gtk_cell_layout_pack_start(GTK_CELL_LAYOUT(buf_type), renderer, TRUE);
    gtk_cell_layout_set_attributes(GTK_CELL_LAYOUT(buf_type), renderer, "text", 0, NULL);
    GtkTreeIter row;
    gtk_tree_store_append(buf_type_store, &row, NULL);
    gtk_tree_store_set(buf_type_store, &row, 0, "Float16", -1);
    gtk_tree_store_append(buf_type_store, &row, NULL);
    gtk_tree_store_set(buf_type_store, &row, 0, "Float32", -1);
    gtk_tree_store_append(buf_type_store, &row, NULL);
    gtk_tree_store_set(buf_type_store, &row, 0, "Float64", -1);
    gtk_tree_store_append(buf_type_store, &row, NULL);
    gtk_tree_store_set(buf_type_store, &row, 0, "Uint8", -1);
    gtk_tree_store_append(buf_type_store, &row, NULL);
    gtk_tree_store_set(buf_type_store, &row, 0, "Int8", -1);
    gtk_tree_store_append(buf_type_store, &row, NULL);
    gtk_tree_store_set(buf_type_store, &row, 0, "Uint16", -1);
    gtk_tree_store_append(buf_type_store, &row, NULL);
    gtk_tree_store_set(buf_type_store, &row, 0, "Int16", -1);
    gtk_tree_store_append(buf_type_store, &row, NULL);
    gtk_tree_store_set(buf_type_store, &row, 0, "Uint32", -1);
    gtk_tree_store_append(buf_type_store, &row, NULL);
    gtk_tree_store_set(buf_type_store, &row, 0, "Int32", -1);
    gtk_tree_store_append(buf_type_store, &row, NULL);
    gtk_tree_store_set(buf_type_store, &row, 0, "Uint64", -1);
    gtk_tree_store_append(buf_type_store, &row, NULL);
    gtk_tree_store_set(buf_type_store, &row, 0, "Int64", -1);
    g_object_unref(buf_type_store);
    gtk_combo_box_set_active(GTK_COMBO_BOX(buf_type), TYPE_FLOAT32);
    
    //Initialize buffer stride and offset
    GtkSpinButton* button = GTK_SPIN_BUTTON(gtk_builder_get_object(builder, "stride"));
    gtk_adjustment_set_upper(gtk_spin_button_get_adjustment(button), 4294967295);
    button = GTK_SPIN_BUTTON(gtk_builder_get_object(builder, "offset"));
    gtk_adjustment_set_upper(gtk_spin_button_get_adjustment(button), 4294967295);
    button = GTK_SPIN_BUTTON(gtk_builder_get_object(builder, "components"));
    gtk_adjustment_set_lower(gtk_spin_button_get_adjustment(button), 1);
    gtk_adjustment_set_upper(gtk_spin_button_get_adjustment(button), 4294967295);
    
    //Initialize buffer list view
    GObject* buf_list_view = gtk_builder_get_object(builder, "buffers_treeview");
    store = gtk_tree_store_new(1, G_TYPE_STRING);
    gtk_tree_view_set_model(GTK_TREE_VIEW(buf_list_view),
                            GTK_TREE_MODEL(store));
    g_object_unref(store);
    renderer = gtk_cell_renderer_text_new();
    column = gtk_tree_view_get_column(GTK_TREE_VIEW(buf_list_view), 0);
    gtk_tree_view_column_pack_start(column, renderer, FALSE);
    gtk_tree_view_column_set_attributes(column, renderer, "text", 0, NULL);
    
    //Initialize buffer content view
    GObject* buf_content_view = gtk_builder_get_object(builder, "buffer_content_treeview");
    store = gtk_tree_store_new(1, G_TYPE_STRING);
    gtk_tree_view_set_model(GTK_TREE_VIEW(buf_content_view),
                            GTK_TREE_MODEL(store));
    g_object_unref(store);
    renderer = gtk_cell_renderer_text_new();
    column = gtk_tree_view_get_column(GTK_TREE_VIEW(buf_content_view), 0);
    gtk_tree_view_column_pack_start(column, renderer, FALSE);
    gtk_tree_view_column_set_attributes(column, renderer, "text", 0, NULL);
    
    //Initialize shader list view
    GObject* shdr_list_view = gtk_builder_get_object(builder, "shader_list_treeview");
    store = gtk_tree_store_new(2, G_TYPE_STRING, G_TYPE_STRING);
    gtk_tree_view_set_model(GTK_TREE_VIEW(shdr_list_view),
                            GTK_TREE_MODEL(store));
    g_object_unref(store);
    renderer = gtk_cell_renderer_text_new();
    column = gtk_tree_view_get_column(GTK_TREE_VIEW(shdr_list_view), 0);
    gtk_tree_view_column_pack_start(column, renderer, FALSE);
    gtk_tree_view_column_set_attributes(column, renderer, "text", 0, NULL);
    column = gtk_tree_view_get_column(GTK_TREE_VIEW(shdr_list_view), 1);
    gtk_tree_view_column_pack_start(column, renderer, FALSE);
    gtk_tree_view_column_set_attributes(column, renderer, "text", 1, NULL);
    
    //Initialize program list view
    GObject* prog_list_view = gtk_builder_get_object(builder, "program_list_view");
    store = gtk_tree_store_new(1, G_TYPE_STRING);
    gtk_tree_view_set_model(GTK_TREE_VIEW(prog_list_view),
                            GTK_TREE_MODEL(store));
    g_object_unref(store);
    renderer = gtk_cell_renderer_text_new();
    column = gtk_tree_view_get_column(GTK_TREE_VIEW(prog_list_view), 0);
    gtk_tree_view_column_pack_start(column, renderer, FALSE);
    gtk_tree_view_column_set_attributes(column, renderer, "text", 0, NULL);
    
    //Initialize program shader view
    GObject* prog_shdrs_view = gtk_builder_get_object(builder, "program_shaders_view");
    store = gtk_tree_store_new(1, G_TYPE_STRING);
    gtk_tree_view_set_model(GTK_TREE_VIEW(prog_shdrs_view),
                            GTK_TREE_MODEL(store));
    g_object_unref(store);
    renderer = gtk_cell_renderer_text_new();
    column = gtk_tree_view_get_column(GTK_TREE_VIEW(prog_shdrs_view), 0);
    gtk_tree_view_column_pack_start(column, renderer, FALSE);
    gtk_tree_view_column_set_attributes(column, renderer, "text", 0, NULL);
    
    main_window = GTK_WIDGET(gtk_builder_get_object(builder, "main_window"));
    gtk_widget_show_all(main_window);
    
    gtk_main();
    
    g_object_unref(G_OBJECT(builder));
    
    g_object_unref(error_pixbuf);
    g_object_unref(warning_pixbuf);
    g_object_unref(info_pixbuf);
    g_object_unref(icon_theme);
    
    free_inspector(inspector);
    free_inspection(inspection);
    free_trace(trace);
    
    return EXIT_SUCCESS;
}
