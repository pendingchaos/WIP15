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

static const glapi_group_t* find_group(const char* name) {
    for (size_t i = 0; i < glapi.group_count; i++) {
        if (strcmp(glapi.groups[i]->name, name) == 0) {
            return glapi.groups[i];
        }
    }
    
    return NULL;
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
            strcat(str, static_format("%f", trace_get_double(&value)[i]));
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
            strcat(str, static_format("<fn ptr>"));
            break;
        }
        case Type_Ptr: {
            strcat(str, static_format("0x%"PRIx64, trace_get_ptr(&value)[i]));
            break;
        }
        case Type_Data: {
            strcat(str, static_format("(data)"));
            break;
        }
        }
        
        if (i != value.count-1) {
            strcat(str, static_format(", "));
        }
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
    
    main_window = GTK_WIDGET(gtk_builder_get_object(builder, "main_window"));
    gtk_widget_show_all(main_window);
    
    gtk_main();
    
    g_object_unref(G_OBJECT(builder));
    
    g_object_unref(error_pixbuf);
    g_object_unref(warning_pixbuf);
    g_object_unref(info_pixbuf);
    g_object_unref(icon_theme);
    
    free_inspection(inspection);
    free_trace(trace);
    
    return EXIT_SUCCESS;
}
