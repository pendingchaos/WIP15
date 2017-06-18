#include "libtrace/libtrace.h"
#include "utils.h"
#include "shared/uint.h"

#include <GL/gl.h>
#include <gtk/gtk.h>
#include <string.h>
#include <stdlib.h>

extern GtkBuilder* builder;
extern trace_t* trace;
extern int64_t revision;

static const char* shader_type_str(GLenum type) {
    switch (type) {
    case GL_VERTEX_SHADER:
        return "Vertex";
    case GL_FRAGMENT_SHADER:
        return "Fragment";
    case GL_GEOMETRY_SHADER:
        return "Geometry";
    case GL_TESS_CONTROL_SHADER:
        return "Tesselation Control";
    case GL_TESS_EVALUATION_SHADER:
        return "Tesselation Evaluation";
    case GL_COMPUTE_SHADER:
        return "Compute";
    default:
        return "Unknown";
    }
}

void init_shader_list(GtkTreeView* tree) {
    GtkTreeStore* store = GTK_TREE_STORE(gtk_tree_view_get_model(tree));
    gtk_tree_store_clear(store);
    
    const trc_gl_shader_rev_t* rev;
    for (size_t i = 0; trc_iter_objects(trace, TrcShader, &i, revision, (const void**)&rev);) {
        char str[64];
        memset(str, 0, 64);
        snprintf(str, 64, "%u", (uint)rev->fake);
        
        GtkTreeIter row;
        gtk_tree_store_append(store, &row, NULL);
        gtk_tree_store_set(store, &row, 0, str, -1);
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
    
    const trc_gl_program_rev_t* rev;
    for (size_t i = 0; trc_iter_objects(trace, TrcProgram, &i, revision, (const void**)&rev);) {
        char str[64];
        memset(str, 0, 64);
        snprintf(str, 64, "%u", (uint)rev->fake);
        
        GtkTreeIter row;
        gtk_tree_store_append(store, &row, NULL);
        gtk_tree_store_set(store, &row, 0, str, -1);
    }
    
    GtkTextView* info_log_view = GTK_TEXT_VIEW(gtk_builder_get_object(builder, "program_info_log"));
    GtkTextBuffer* info_log_buffer = gtk_text_view_get_buffer(info_log_view);
    gtk_text_buffer_set_text(info_log_buffer, "", -1);
}

static char* get_shader_source(const trc_gl_shader_rev_t* shdr) {
    char* source = calloc(shdr->sources->size==0?1:shdr->sources->size, 1);
    char* sourcesource = trc_map_data(shdr->sources, TRC_MAP_READ);
    for (size_t i = 0; i < shdr->sources->size; i++) {
        if (sourcesource[i] != 0)
            source[strlen(source)] = sourcesource[i];
    }
    trc_unmap_data(shdr->sources);
    
    return source;
}

void shader_select_callback(GObject* obj, gpointer user_data) {
    GtkTreePath* path;
    gtk_tree_view_get_cursor(GTK_TREE_VIEW(obj), &path, NULL);
    if (!path) return;
    
    size_t index = gtk_tree_path_get_indices(path)[0];
    
    size_t count = 0;
    const trc_gl_shader_rev_t* shdr = NULL;
    for (size_t i = 0; trc_iter_objects(trace, TrcShader, &i, revision, (const void**)&shdr);) {
        if (count == index-1) break;
        count++;
    }
    
    if (!shdr) return; //TODO: Is this possible?
    
    char* source = get_shader_source(shdr);
    
    GtkTextView* source_view = GTK_TEXT_VIEW(gtk_builder_get_object(builder, "selected_shader_textview"));
    GtkTextBuffer* source_buffer = gtk_text_view_get_buffer(source_view);
    
    gtk_text_buffer_set_text(source_buffer, source, -1);
    
    free(source);
    
    GtkTextView* info_log_view = GTK_TEXT_VIEW(gtk_builder_get_object(builder, "shader_info_log"));
    GtkTextBuffer* info_log_buffer = gtk_text_view_get_buffer(info_log_view);
    
    gtk_text_buffer_set_text(info_log_buffer, trc_map_data(shdr->info_log, TRC_MAP_READ), -1);
    trc_unmap_data(shdr->info_log);
}

static const trc_gl_program_rev_t* selected_program = NULL;

void prog_shdr_select_callback(GObject* obj, gpointer userdata);

//TODO: Show GL_PROGRAM_BINARY_RETRIEVABLE_HINT and GL_PROGRAM_SEPARABLE
void program_select_callback(GObject* obj, gpointer user_data) {
    GtkTreePath* path;
    gtk_tree_view_get_cursor(GTK_TREE_VIEW(obj), &path, NULL);
    if (!path) return;
    
    size_t index = gtk_tree_path_get_indices(path)[0];
    
    size_t count = 0;
    const trc_gl_program_rev_t* program = NULL;
    for (size_t i = 0; trc_iter_objects(trace, TrcProgram, &i, revision, (const void**)&program);) {
        if (count == index-1) break;
        count++;
    }
    selected_program = program;
    
    GtkTreeView* view = GTK_TREE_VIEW(gtk_builder_get_object(builder, "program_shaders_view"));
    GtkTreeStore* store = GTK_TREE_STORE(gtk_tree_view_get_model(view));
    gtk_tree_store_clear(store);
    size_t shader_count = program->shaders->size / sizeof(trc_gl_program_shader_t);
    trc_gl_program_shader_t* shaders = trc_map_data(program->shaders, TRC_MAP_READ);
    for (size_t i = 0; i < shader_count; i++) {
        const trc_gl_shader_rev_t* shdr = trc_obj_get_rev(shaders[i].shader.obj, revision);
        
        char id[64];
        memset(id, 0, 64);
        snprintf(id, 64, "%lu", shdr->fake);
        
        GtkTreeIter row;
        gtk_tree_store_append(store, &row, NULL);
        gtk_tree_store_set(store, &row, 0, id, 1, shader_type_str(shdr->type), -1);
    }
    trc_unmap_data(program->shaders);
    
    view = GTK_TREE_VIEW(gtk_builder_get_object(builder, "program_uniforms_view"));
    store = GTK_TREE_STORE(gtk_tree_view_get_model(view));
    gtk_tree_store_clear(store);
    size_t uniform_count = program->uniforms->size / sizeof(trc_gl_program_uniform_t);
    trc_gl_program_uniform_t* uniforms = trc_map_data(program->uniforms, TRC_MAP_READ);
    for (size_t i = 0; i < uniform_count; i++) {
        char val[1024] = {0};
        if (uniforms[i].dim[0] == 0) {
            strcpy(val, "<unset>");
        } else {
            size_t count = uniforms[i].value->size / sizeof(double);
            double* vals = trc_map_data(uniforms[i].value, TRC_MAP_READ);
            if (count>1 || uniforms[i].count!=0) strncat(val, "[", sizeof(val)-1);
            for (size_t i = 0; i < count; i++) {
                strncat(val, static_format("%g", vals[i]), sizeof(val)-1);
                if (i != count-1) strncat(val, ", ", sizeof(val)-1);
            }
            if (count>1 || uniforms[i].count!=0) strncat(val, "]", sizeof(val)-1);
            trc_unmap_data(uniforms[i].value);
        }
        
        const char* location = static_format("%u", uniforms[i].fake);
        
        GtkTreeIter row;
        gtk_tree_store_append(store, &row, NULL);
        gtk_tree_store_set(store, &row, 0, location, 1, "", 2, val, -1);
    }
    trc_unmap_data(program->uniforms);
    
    GtkTextView* info_log_view = GTK_TEXT_VIEW(gtk_builder_get_object(builder, "program_info_log"));
    GtkTextBuffer* info_log_buffer = gtk_text_view_get_buffer(info_log_view);
    gtk_text_buffer_set_text(info_log_buffer, trc_map_data(program->info_log, TRC_MAP_READ), -1);
    trc_unmap_data(program->info_log);
    
    prog_shdr_select_callback(gtk_builder_get_object(builder, "program_shaders_view"), NULL);
}

void prog_shdr_select_callback(GObject* obj, gpointer userdata) {
    if (!selected_program) return;
    
    GtkTreePath* path;
    gtk_tree_view_get_cursor(GTK_TREE_VIEW(obj), &path, NULL);
    if (!path) return;
    
    size_t shdr_index = gtk_tree_path_get_indices(path)[0];
    
    trc_gl_program_shader_t* shaders = trc_map_data(selected_program->shaders, TRC_MAP_READ);
    trc_gl_program_shader_t shader = shaders[shdr_index];
    const trc_gl_shader_rev_t* shdr = trc_obj_get_rev(shaders[shdr_index].shader.obj, revision);
    trc_unmap_data(selected_program->shaders);
    
    char* source = get_shader_source(shdr);
    
    GtkTextView* source_view = GTK_TEXT_VIEW(gtk_builder_get_object(builder, "shdr_src_textview"));
    GtkTextBuffer* source_buffer = gtk_text_view_get_buffer(source_view);
    gtk_text_buffer_set_text(source_buffer, source, -1);
    
    free(source);
}

void shader_init() {
    init_treeview(builder, "shader_list_treeview", 2);
    init_treeview(builder, "program_list_view", 1);
    init_treeview(builder, "program_shaders_view", 2);
    init_treeview(builder, "program_uniforms_view", 3);
}
