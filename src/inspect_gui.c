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

static GtkWidget* main_window;
static GtkBuilder* builder;
static GdkPixbuf* info_pixbuf;
static GdkPixbuf* warning_pixbuf;
static GdkPixbuf* error_pixbuf;
static tex_inspector_t* tex_inspector;

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

static char* format_float(float val) {
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

static void format_value(char* str, trace_value_t value, const trace_t* trace) {
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

static void format_command(char* str, inspect_command_t* command, const trace_t* trace) {
    trace_command_t* trace_cmd = command->trace_cmd;
    
    strcat(str, static_format("%s(", command->name));
    
    size_t count = get_vec_size(command->trace_cmd->args) / sizeof(trace_arg_t);
    for (size_t i = 0; i < count; ++i) {
        trace_arg_t* arg = ((trace_arg_t*)get_vec_data(command->trace_cmd->args)) + i;
        
        format_value(str, arg->val, trace);
        if (i != count-1) {
            strcat(str, static_format(", "));
        }
    }
    
    strcat(str, static_format(")"));
    
    if (trace_cmd->ret.type != Type_Void) {
        strcat(str, static_format(" = "));
        format_value(str, trace_cmd->ret, trace);
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
            format_command(cmd_str, cmd, inspection->trace);
            
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
                            inspect_gl_state_t* state,
                            const trace_t* trace) {
    GtkTreeStore* store = GTK_TREE_STORE(gtk_tree_view_get_model(tree));
    gtk_tree_store_clear(store);
    
    vec_t entries = state->entries;
    size_t count = get_vec_size(entries)/sizeof(inspect_gl_state_entry_t);
    for (size_t i = 0; i < count; ++i) {
        inspect_gl_state_entry_t* entry = ((inspect_gl_state_entry_t*)get_vec_data(entries)) + i;
        
        char val_str[1024];
        memset(val_str, 0, 1024);
        format_value(val_str, entry->val, trace);
        
        GtkTreeIter row;
        gtk_tree_store_append(store, &row, NULL);
        gtk_tree_store_set(store, &row, 0, entry->name, 1, val_str, -1);
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
    if (!inspect_get_tex_params(tex_inspector, tex_index, &params))
        return;
    
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
        inspect_get_tex_data(tex_inspector, tex_index, level, &data);
        
        GtkTreeIter row;
        gtk_tree_store_append(image_store, &row, NULL);
        
        if (!data) {
            gtk_tree_store_set(image_store, &row, 0, static_format("%u", level), 1, NULL, -1);
        } else {
            GdkPixbuf* pixbuf = gdk_pixbuf_new(GDK_COLORSPACE_RGB, TRUE, 8, w, h);
            memcpy(gdk_pixbuf_get_pixels(pixbuf), data, w*h*4);
            gtk_tree_store_set(image_store, &row, 0, static_format("%u", level), 1, pixbuf, -1);
        }
        
        level++;
        w /= 2;
        h /= 2;
    }
}

static void init_texture_list(GtkTreeView* tree) {
    GtkTreeStore* store = GTK_TREE_STORE(gtk_tree_view_get_model(tree));
    gtk_tree_store_clear(store);
    
    for (size_t i = 0; i < inspect_get_tex_count(tex_inspector); ++i) {
        char str[64];
        memset(str, 0, 64);
        snprintf(str, 64, "%u", inspect_get_tex(tex_inspector, i));
        
        GtkTreeIter row;
        gtk_tree_store_append(store, &row, NULL);
        gtk_tree_store_set(store, &row, 0, str, -1);
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
        
        seek_tex_inspector(tex_inspector, indices[0], indices[1]);
        init_texture_list(GTK_TREE_VIEW(gtk_builder_get_object(builder, "texture_list_treeview")));
        
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
                        &cmd->state,
                        inspection->trace);
    } else {
        GtkTreeView* tree = GTK_TREE_VIEW(gtk_builder_get_object(builder, "state_treeview"));
        GtkTreeStore* store = GTK_TREE_STORE(gtk_tree_view_get_model(tree));
        gtk_tree_store_clear(store);
    }
}

int main(int argc, char** argv) {
    if (argc != 2) {
        fprintf(stderr, "Expected one argument. Got %d.\n", argc);
        fprintf(stderr, "Usage: inspect-gui <trace file>\n");
        return EXIT_FAILURE;
    }
    
    trace_t* trace = load_trace(argv[1]);
    
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
    
    tex_inspector = create_tex_inspector(inspection);
    
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
    gtk_tree_view_set_model(GTK_TREE_VIEW(attachments_view),
                            GTK_TREE_MODEL(gtk_tree_store_new(1, G_TYPE_STRING)));
    GtkCellRenderer* renderer = gtk_cell_renderer_text_new();
    GtkTreeViewColumn* column = gtk_tree_view_get_column(GTK_TREE_VIEW(attachments_view), 0);
    gtk_tree_view_column_pack_start(column, renderer, FALSE);
    gtk_tree_view_column_set_attributes(column, renderer, "text", 0, NULL);
    
    //Initialize state view
    GObject* state_view = gtk_builder_get_object(builder, "state_treeview");
    gtk_tree_view_set_model(GTK_TREE_VIEW(state_view),
                            GTK_TREE_MODEL(gtk_tree_store_new(2, G_TYPE_STRING, G_TYPE_STRING)));
    renderer = gtk_cell_renderer_text_new();
    column = gtk_tree_view_get_column(GTK_TREE_VIEW(state_view), 0);
    gtk_tree_view_column_pack_start(column, renderer, FALSE);
    gtk_tree_view_column_set_attributes(column, renderer, "text", 0, NULL);
    column = gtk_tree_view_get_column(GTK_TREE_VIEW(state_view), 1);
    gtk_tree_view_column_pack_start(column, renderer, FALSE);
    gtk_tree_view_column_set_attributes(column, renderer, "text", 1, NULL);
    
    //Initialize texture list view
    GObject* tex_list_view = gtk_builder_get_object(builder, "texture_list_treeview");
    gtk_tree_view_set_model(GTK_TREE_VIEW(tex_list_view),
                            GTK_TREE_MODEL(gtk_tree_store_new(1, G_TYPE_STRING)));
    renderer = gtk_cell_renderer_text_new();
    column = gtk_tree_view_get_column(GTK_TREE_VIEW(tex_list_view), 0);
    gtk_tree_view_column_pack_start(column, renderer, FALSE);
    gtk_tree_view_column_set_attributes(column, renderer, "text", 0, NULL);
    
    //Initialize texture state view
    GObject* tex_state_view = gtk_builder_get_object(builder, "selected_texture_treeview");
    gtk_tree_view_set_model(GTK_TREE_VIEW(tex_state_view),
                            GTK_TREE_MODEL(gtk_tree_store_new(2, G_TYPE_STRING, G_TYPE_STRING)));
    renderer = gtk_cell_renderer_text_new();
    column = gtk_tree_view_get_column(GTK_TREE_VIEW(tex_state_view), 0);
    gtk_tree_view_column_pack_start(column, renderer, FALSE);
    gtk_tree_view_column_set_attributes(column, renderer, "text", 0, NULL);
    column = gtk_tree_view_get_column(GTK_TREE_VIEW(tex_state_view), 1);
    gtk_tree_view_column_pack_start(column, renderer, FALSE);
    gtk_tree_view_column_set_attributes(column, renderer, "text", 1, NULL);
    
    //Initialize texture image view
    GObject* tex_image_view = gtk_builder_get_object(builder, "selected_texture_images");
    gtk_tree_view_set_model(GTK_TREE_VIEW(tex_image_view),
                            GTK_TREE_MODEL(gtk_tree_store_new(2, G_TYPE_STRING, GDK_TYPE_PIXBUF)));
    renderer = gtk_cell_renderer_text_new();
    column = gtk_tree_view_get_column(GTK_TREE_VIEW(tex_image_view), 0);
    gtk_tree_view_column_pack_start(column, renderer, FALSE);
    gtk_tree_view_column_set_attributes(column, renderer, "text", 0, NULL);
    renderer = gtk_cell_renderer_pixbuf_new();
    column = gtk_tree_view_get_column(GTK_TREE_VIEW(tex_image_view), 1);
    gtk_tree_view_column_pack_start(column, renderer, FALSE);
    gtk_tree_view_column_set_attributes(column, renderer, "pixbuf", 1, NULL);
    
    main_window = GTK_WIDGET(gtk_builder_get_object(builder, "main_window"));
    gtk_widget_show_all(main_window);
    
    gtk_main();
    
    g_object_unref(G_OBJECT(builder));
    
    g_object_unref(error_pixbuf);
    g_object_unref(warning_pixbuf);
    g_object_unref(info_pixbuf);
    g_object_unref(icon_theme);
    
    free_tex_inspector(tex_inspector);
    free_inspection(inspection);
    free_trace(trace);
    
    return EXIT_SUCCESS;
}
