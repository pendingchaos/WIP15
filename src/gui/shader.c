#include "libtrace/libtrace.h"
#include "utils.h"
#include "shared/uint.h"

#include "shared/glcorearb.h"
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
    
    create_obj_list(store, TrcShader);
    
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
    
    create_obj_list(store, TrcProgram);
    
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
        if (count == index) break;
        count++;
    }
    
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

static void fill_uniform_tree(GtkTreeStore* store, trc_gl_uniform_t* uniforms, uint uniformindex,
                              uint8_t* data, GtkTreeIter* parent, int64_t array_index, const char* prefix) {
    trc_gl_uniform_t uniform = uniforms[uniformindex];
    
    char val[1024] = {0};
    
    size_t count = uniform.dtype.dim[0] * uniform.dtype.dim[1];
    if (count>1) strncat(val, "[", sizeof(val)-1);
    void* udata = data + uniform.data_offset;
    for (size_t i = 0; i < count; i++) {
        switch (uniform.dtype.base) {
        #define D(e, t, f) case e:\
            strncat(val, static_format(f, ((t*)udata)[i]), sizeof(val)-1);\
            break;
        D(TrcUniformBaseType_Float, float, "%g")
        D(TrcUniformBaseType_Double, double, "%g")
        D(TrcUniformBaseType_Uint, uint32_t, "%u")
        D(TrcUniformBaseType_Int, int32_t, "%d")
        D(TrcUniformBaseType_Uint64, uint64_t, "%"PRIu64)
        D(TrcUniformBaseType_Int64, int64_t, "%"PRId64)
        case TrcUniformBaseType_Bool:
            strncat(val, static_format("%s", ((bool*)udata)[i]?"true":"false"), sizeof(val)-1);
            break;
        D(TrcUniformBaseType_Sampler, int32_t, "%u")
        D(TrcUniformBaseType_Image, int32_t, "%u")
        #undef D
        default: break;
        }
        if (i != count-1) strncat(val, ", ", sizeof(val)-1);
    }
    if (count>1) strncat(val, "]", sizeof(val)-1);
    
    const char* name = trc_map_data(uniform.name, TRC_MAP_READ);
    char location[256] = {0};
    if ((int)uniform.dtype.base <= 8) strcpy(location, static_format("%u", uniform.fake_loc));
    
    GtkTreeIter row;
    gtk_tree_store_append(store, &row, parent);
    gtk_tree_store_set(store, &row,
                       0, static_format("%s%s", prefix, name),
                       1, location,
                       2, val, -1);
    
    switch (uniform.dtype.base) {
    case TrcUniformBaseType_Struct:
    case TrcUniformBaseType_Array: {
        size_t i = 0;
        for (uint cur = uniform.first_child; cur != 0xffffffff; cur = uniforms[cur].next) {
            char newprefix[256] = {0};
            if (uniform.dtype.base == TrcUniformBaseType_Struct)
                snprintf(newprefix, sizeof(newprefix)-1, "%s%s.", prefix, name);
            else
                snprintf(newprefix, sizeof(newprefix)-1, "%s%s[%"PRId64"]", prefix, name, i);
            fill_uniform_tree(store, uniforms, cur, data, &row, uniform.dtype.base==TrcUniformBaseType_Array?i:-1, newprefix);
            i++;
        }
        break;
    }
    default: {
        break;
    }
    }
    
    trc_unmap_data(uniform.name);
}

//TODO: Show GL_PROGRAM_BINARY_RETRIEVABLE_HINT and GL_PROGRAM_SEPARABLE
void program_select_callback(GObject* obj, gpointer user_data) {
    GtkTreePath* path;
    gtk_tree_view_get_cursor(GTK_TREE_VIEW(obj), &path, NULL);
    if (!path) return;
    
    size_t index = gtk_tree_path_get_indices(path)[0];
    
    size_t count = 0;
    const trc_gl_program_rev_t* program = NULL;
    for (size_t i = 0; trc_iter_objects(trace, TrcProgram, &i, revision, (const void**)&program);) {
        if (count == index) break;
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
        fmt_object_id(id, 64, &shdr->head);
        
        GtkTreeIter row;
        gtk_tree_store_append(store, &row, NULL);
        gtk_tree_store_set(store, &row, 0, id, 1, shader_type_str(shdr->type), -1);
    }
    trc_unmap_data(program->shaders);
    
    view = GTK_TREE_VIEW(gtk_builder_get_object(builder, "program_uniforms_view"));
    store = GTK_TREE_STORE(gtk_tree_view_get_model(view));
    gtk_tree_store_clear(store);
    
    trc_gl_uniform_t* uniforms = trc_map_data(program->uniforms, TRC_MAP_READ);
    uint8_t* data = trc_map_data(program->uniform_data, TRC_MAP_READ);
    for (size_t i = 0; i < program->root_uniform_count; i++)
        fill_uniform_tree(store, uniforms, i, data, NULL, -1, "");
    trc_unmap_data(program->uniform_data);
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
