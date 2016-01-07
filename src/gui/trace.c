#include "libinspect/libinspect.h"
#include "utils.h"

#include <gtk/gtk.h>
#include <string.h>
#include <assert.h>

extern GtkBuilder* builder;
extern GdkPixbuf* info_pixbuf;
extern GdkPixbuf* warning_pixbuf;
extern GdkPixbuf* error_pixbuf;
extern trace_t* trace;
extern inspector_t* inspector;
extern inspection_t* inspection;

void init_buffer_list(GtkTreeView* tree);
void init_vao_list(GtkTreeView* tree);
void init_framebuffers_list(GtkTreeView* tree);
void init_renderbuffers_list(GtkTreeView* tree);
void init_texture_list(GtkTreeView* tree);
void init_shader_list(GtkTreeView* tree);
void init_program_list(GtkTreeView* tree);
void init_syncs_list(GtkTreeView* tree);
void init_queries_list(GtkTreeView* tree);

static void init_trace_tree(GtkTreeView* tree) {
    GtkTreeStore* store = GTK_TREE_STORE(gtk_tree_view_get_model(tree));
    gtk_tree_store_clear(store);
    
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
            format_command(trace, cmd_str, cmd);
            
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
}

static void init_state_tree(GtkTreeView* tree,
                            inspect_gl_state_t* state) {
    GtkTreeStore* store = GTK_TREE_STORE(gtk_tree_view_get_model(tree));
    gtk_tree_store_clear(store);
    
    inspect_gl_state_vec_t entries = state->entries;
    for (inspect_gl_state_entry_t* entry = entries->data; !vec_end(entries, entry); entry++) {
        char val_str[1024];
        memset(val_str, 0, 1024);
        format_value(trace, val_str, entry->val);
        
        GtkTreeIter row;
        gtk_tree_store_append(store, &row, NULL);
        gtk_tree_store_set(store, &row, 0, entry->name, 1, val_str, -1);
    }
}

void command_select_callback(GObject* obj, gpointer user_data) {
    GtkTreePath* path;
    gtk_tree_view_get_cursor(GTK_TREE_VIEW(obj), &path, NULL);
    
    if (!path)
        return;
    
    if (gtk_tree_path_get_depth(path) == 2) {
        gint* indices = gtk_tree_path_get_indices(path);
        
        assert(indices[0] < inspection->frame_count);
        inspect_frame_t* frame = inspection->frames + indices[0];
        assert(indices[1] < frame->command_count);
        inspect_command_t* cmd = frame->commands + indices[1];
        
        seek_inspector(inspector, indices[0], indices[1]);
        init_texture_list(GTK_TREE_VIEW(gtk_builder_get_object(builder, "texture_list_treeview")));
        init_buffer_list(GTK_TREE_VIEW(gtk_builder_get_object(builder, "buffers_treeview")));
        init_shader_list(GTK_TREE_VIEW(gtk_builder_get_object(builder, "shader_list_treeview")));
        init_program_list(GTK_TREE_VIEW(gtk_builder_get_object(builder, "program_list_view")));
        init_vao_list(GTK_TREE_VIEW(gtk_builder_get_object(builder, "vao_treeview")));
        init_framebuffers_list(GTK_TREE_VIEW(gtk_builder_get_object(builder, "framebuffers_treeview")));
        init_renderbuffers_list(GTK_TREE_VIEW(gtk_builder_get_object(builder, "renderbuffer_list")));
        init_syncs_list(GTK_TREE_VIEW(gtk_builder_get_object(builder, "sync_list")));
        init_queries_list(GTK_TREE_VIEW(gtk_builder_get_object(builder, "query_list")));
        
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
    } else {
        GtkTreeView* tree = GTK_TREE_VIEW(gtk_builder_get_object(builder, "state_treeview"));
        GtkTreeStore* store = GTK_TREE_STORE(gtk_tree_view_get_model(tree));
        gtk_tree_store_clear(store);
    }
}

void trace_init() {
    init_treeview(builder, "selected_command_attachments", 1);
    init_treeview(builder, "state_treeview", 2);
    
    //Initialize the command list view
    GtkTreeView* tree = GTK_TREE_VIEW(gtk_builder_get_object(builder, "trace_view"));
    GtkTreeStore* store = gtk_tree_store_new(2, GDK_TYPE_PIXBUF, G_TYPE_STRING);
    
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

void trace_fill() {
    GObject* trace_view = gtk_builder_get_object(builder, "trace_view");
    init_trace_tree(GTK_TREE_VIEW(trace_view));
}
