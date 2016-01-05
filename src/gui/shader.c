#include "libinspect/libinspect.h"
#include "utils.h"

#include <GL/gl.h>
#include <gtk/gtk.h>
#include <string.h>

extern GtkBuilder* builder;
extern inspector_t* inspector;

void init_shader_list(GtkTreeView* tree) {
    GtkTreeStore* store = GTK_TREE_STORE(gtk_tree_view_get_model(tree));
    gtk_tree_store_clear(store);
    
    inspect_shdr_vec_t shaders = inspector->shaders;
    for (inspect_shader_t* shdr = shaders->data; !vec_end(shaders, shdr); shdr++) {
        char str[64];
        memset(str, 0, 64);
        snprintf(str, 64, "%u", shdr->fake);
        
        const char* type = "Unknown";
        switch (shdr->type) {
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
    
    GtkTextView* info_log_view = GTK_TEXT_VIEW(gtk_builder_get_object(builder, "shader_info_log"));
    GtkTextBuffer* info_log_buffer = gtk_text_view_get_buffer(info_log_view);
    gtk_text_buffer_set_text(info_log_buffer, "", -1);
}

void init_program_list(GtkTreeView* tree) {
    GtkTreeView* view = GTK_TREE_VIEW(gtk_builder_get_object(builder, "program_shaders_view"));
    GtkTreeStore* store = GTK_TREE_STORE(gtk_tree_view_get_model(view));
    gtk_tree_store_clear(store);
    
    store = GTK_TREE_STORE(gtk_tree_view_get_model(tree));
    gtk_tree_store_clear(store);
    
    inspect_prog_vec_t programs = inspector->programs;
    for (inspect_program_t* prog = programs->data; !vec_end(programs, prog); prog++) {
        char str[64];
        memset(str, 0, 64);
        snprintf(str, 64, "%u", prog->fake);
        
        GtkTreeIter row;
        gtk_tree_store_append(store, &row, NULL);
        gtk_tree_store_set(store, &row, 0, str, -1);
    }
    
    GtkTextView* info_log_view = GTK_TEXT_VIEW(gtk_builder_get_object(builder, "program_info_log"));
    GtkTextBuffer* info_log_buffer = gtk_text_view_get_buffer(info_log_view);
    gtk_text_buffer_set_text(info_log_buffer, "", -1);
}

void shader_select_callback(GObject* obj, gpointer user_data) {
    GtkTreePath* path;
    gtk_tree_view_get_cursor(GTK_TREE_VIEW(obj), &path, NULL);
    
    if (!path)
        return;
    
    size_t index = gtk_tree_path_get_indices(path)[0];
    inspect_shader_t* shdr = get_inspect_shdr_vec(inspector->shaders, index);
    if (!shdr)
        return;
    
    GtkTextView* source_view = GTK_TEXT_VIEW(gtk_builder_get_object(builder, "selected_shader_textview"));
    GtkTextBuffer* source_buffer = gtk_text_view_get_buffer(source_view);
    
    gtk_text_buffer_set_text(source_buffer, shdr->source?shdr->source:"", -1);
    
    GtkTextView* info_log_view = GTK_TEXT_VIEW(gtk_builder_get_object(builder, "shader_info_log"));
    GtkTextBuffer* info_log_buffer = gtk_text_view_get_buffer(info_log_view);
    
    gtk_text_buffer_set_text(info_log_buffer, shdr->info_log?shdr->info_log:"", -1);
}

void program_select_callback(GObject* obj, gpointer user_data) {
    GtkTreePath* path;
    gtk_tree_view_get_cursor(GTK_TREE_VIEW(obj), &path, NULL);
    
    if (!path)
        return;
    
    size_t index = gtk_tree_path_get_indices(path)[0];
    inspect_program_t* prog = get_inspect_prog_vec(inspector->programs, index);
    if (!prog)
        return;
    
    GtkTreeView* view = GTK_TREE_VIEW(gtk_builder_get_object(builder, "program_shaders_view"));
    GtkTreeStore* store = GTK_TREE_STORE(gtk_tree_view_get_model(view));
    gtk_tree_store_clear(store);
    
    size_t count = prog->shaders->size / sizeof(unsigned int);
    for (size_t i = 0; i < count; i++) {
        char str[64];
        memset(str, 0, 64);
        snprintf(str, 64, "%u", ((unsigned int*)prog->shaders->data)[i]);
        
        GtkTreeIter row;
        gtk_tree_store_append(store, &row, NULL);
        gtk_tree_store_set(store, &row, 0, str, -1);
    }
    
    GtkTextView* info_log_view = GTK_TEXT_VIEW(gtk_builder_get_object(builder, "program_info_log"));
    GtkTextBuffer* info_log_buffer = gtk_text_view_get_buffer(info_log_view);
    
    gtk_text_buffer_set_text(info_log_buffer, prog->info_log?prog->info_log:"", -1);
}

void shader_init() {
    init_treeview(builder, "shader_list_treeview", 2);
    init_treeview(builder, "program_list_view", 1);
    init_treeview(builder, "program_shaders_view", 1);
}
