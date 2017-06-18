#include "libtrace/libtrace.h"
#include "utils.h"

#include <gtk/gtk.h>
#include <GL/gl.h>
#include <string.h>
#include <inttypes.h>

extern GtkBuilder* builder;
extern trace_t* trace;
extern int64_t revision;

void init_vao_list(GtkTreeView* tree) {
    GtkTreeView* content = GTK_TREE_VIEW(gtk_builder_get_object(builder, "vao_attributes"));
    GtkTreeStore* store = GTK_TREE_STORE(gtk_tree_view_get_model(content));
    gtk_tree_store_clear(store);
    
    store = GTK_TREE_STORE(gtk_tree_view_get_model(tree));
    gtk_tree_store_clear(store);
    
    const trc_gl_vao_rev_t* rev;
    for (size_t i = 0; trc_iter_objects(trace, TrcVAO, &i, revision, (const void**)&rev);) {
        char str[64];
        memset(str, 0, 64);
        snprintf(str, 64, "%u", (uint)rev->fake);
        
        GtkTreeIter row;
        gtk_tree_store_append(store, &row, NULL);
        gtk_tree_store_set(store, &row, 0, str, -1);
    }
}

void vao_select_callback(GObject* obj, gpointer user_data) {
    GtkTreeView* attr_tree = GTK_TREE_VIEW(gtk_builder_get_object(builder, "vao_attributes"));
    GtkTreeStore* attr_store = GTK_TREE_STORE(gtk_tree_view_get_model(attr_tree));
    
    if (!attr_store)
        return;
    
    gtk_tree_store_clear(attr_store);
    
    GtkTreePath* path;
    gtk_tree_view_get_cursor(GTK_TREE_VIEW(obj), &path, NULL);
    if (!path) return;
    
    size_t index = gtk_tree_path_get_indices(path)[0];
    
    size_t count = 0;
    const trc_gl_vao_rev_t* vao = NULL;
    for (size_t i = 0; trc_iter_objects(trace, TrcVAO, &i, revision, (const void**)&vao);) {
        if (count == index-1) break;
        count++;
    }
    
    trc_gl_vao_attrib_t* attribs = trc_map_data(vao->attribs, TRC_MAP_READ);
    for (size_t i = 0; i < vao->attribs->size/sizeof(trc_gl_vao_attrib_t); i++) {
        trc_gl_vao_attrib_t* attr = &attribs[i];
        
        const char* type_str = "Unknown";
        switch (attr->type) {
        case GL_BYTE: type_str = "GL_BYTE"; break;
        case GL_UNSIGNED_BYTE: type_str = "GL_UNSIGNED_BYTE"; break;
        case GL_SHORT: type_str = "GL_SHORT"; break;
        case GL_UNSIGNED_SHORT: type_str = "GL_UNSIGNED_SHORT"; break;
        case GL_INT: type_str = "GL_INT"; break;
        case GL_UNSIGNED_INT: type_str = "GL_UNSIGNED_INT"; break;
        case GL_FLOAT: type_str = "GL_FLOAT"; break;
        case GL_DOUBLE: type_str = "GL_DOUBLE"; break;
        }
        
        char index_str[64];
        memset(index_str, 0, 64);
        snprintf(index_str, 64, "%u", (uint)i);
        
        char size_str[64];
        memset(size_str, 0, 64);
        snprintf(size_str, 64, "%u", attr->size);
        
        char stride_str[64];
        memset(stride_str, 0, 64);
        snprintf(stride_str, 64, "%u", attr->stride);
        
        char offset_str[64];
        memset(offset_str, 0, 64);
        snprintf(offset_str, 64, "%"PRIu64, attr->offset);
        
        char divisor_str[64];
        memset(divisor_str, 0, 64);
        snprintf(divisor_str, 64, "%u", attr->divisor);
        
        char buffer_str[64];
        memset(buffer_str, 0, 64);
        if (attr->buffer.obj) {
            const trc_gl_buffer_rev_t* rev = trc_obj_get_rev(attr->buffer.obj, revision);
            snprintf(buffer_str, 64, "%lu", rev->fake); //TODO: Handle when the object has no name
        }
        
        GtkTreeIter row;
        gtk_tree_store_append(attr_store, &row, NULL);
        gtk_tree_store_set(attr_store, &row,
                           0, index_str,
                           1, attr->enabled ? "true" : "false",
                           2, size_str,
                           3, stride_str,
                           4, offset_str,
                           5, type_str,
                           6, attr->normalized ? "true" : "false",
                           7, attr->integer ? "true" : "false",
                           8, divisor_str,
                           9, buffer_str,
                           -1);
    }
    trc_unmap_data(vao->attribs);
}

void vao_init() {
    init_treeview(builder, "vao_treeview", 1);
    init_treeview(builder, "vao_attributes", 10);
}
