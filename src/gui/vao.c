#include "libinspect/libinspect.h"
#include "utils.h"

#include <gtk/gtk.h>
#include <GL/gl.h>
#include <string.h>

extern GtkBuilder* builder;
extern inspector_t* inspector;

void init_vao_list(GtkTreeView* tree) {
    GtkTreeView* content = GTK_TREE_VIEW(gtk_builder_get_object(builder, "vao_attributes"));
    GtkTreeStore* store = GTK_TREE_STORE(gtk_tree_view_get_model(content));
    gtk_tree_store_clear(store);
    
    store = GTK_TREE_STORE(gtk_tree_view_get_model(tree));
    gtk_tree_store_clear(store);
    
    inspect_vao_vec_t vaos = inspector->vaos;
    for (inspect_vao_t* vao = vaos->data; !vec_end(vaos, vao); vao++) {
        char str[64];
        memset(str, 0, 64);
        snprintf(str, 64, "%u", vao->fake);
        
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
    if (!path)
        return;
    
    size_t index = gtk_tree_path_get_indices(path)[0];
    inspect_vao_t* vao = get_inspect_vao_vec(inspector->vaos, index);
    if (!vao)
        return;
    
    for (size_t i = 0; i < vao->attrib_count; i++) {
        inspect_vertex_attrib_t* attr = vao->attribs + i;
        
        const char* type_str = "Unknown";
        switch (attr->type) {
        case GL_BYTE:
            type_str = "GL_BYTE";
            break;
        case GL_UNSIGNED_BYTE:
            type_str = "GL_UNSIGNED_BYTE";
            break;
        case GL_SHORT:
            type_str = "GL_SHORT";
            break;
        case GL_UNSIGNED_SHORT:
            type_str = "GL_UNSIGNED_SHORT";
            break;
        case GL_INT:
            type_str = "GL_INT";
            break;
        case GL_UNSIGNED_INT:
            type_str = "GL_UNSIGNED_INT";
            break;
        case GL_HALF_FLOAT:
            type_str = "GL_HALF_FLOAT";
            break;
        case GL_FLOAT:
            type_str = "GL_FLOAT";
            break;
        case GL_DOUBLE:
            type_str = "GL_DOUBLE";
            break;
        case GL_FIXED:
            type_str = "GL_FIXED";
            break;
        case GL_INT_2_10_10_10_REV:
            type_str = "GL_INT_2_10_10_10_REV";
            break;
        case GL_UNSIGNED_INT_2_10_10_10_REV:
            type_str = "GL_UNSIGNED_INT_2_10_10_10_REV";
            break;
        case GL_UNSIGNED_INT_10F_11F_11F_REV:
            type_str = "GL_UNSIGNED_INT_10F_11F_11F_REV";
            break;
        }
        
        char index_str[64];
        memset(index_str, 0, 64);
        snprintf(index_str, 64, "%u", attr->index);
        
        char size_str[64];
        memset(size_str, 0, 64);
        snprintf(size_str, 64, "%u", attr->size);
        
        char stride_str[64];
        memset(stride_str, 0, 64);
        snprintf(stride_str, 64, "%u", attr->stride);
        
        char offset_str[64];
        memset(offset_str, 0, 64);
        snprintf(offset_str, 64, "%u", attr->offset);
        
        char divisor_str[64];
        memset(divisor_str, 0, 64);
        snprintf(divisor_str, 64, "%u", attr->divisor);
        
        char buffer_str[64];
        memset(buffer_str, 0, 64);
        snprintf(buffer_str, 64, "%u", attr->buffer);
        
        char value_strs[128][4];
        for (size_t j = 0; j < 4; j++)
            strncpy(value_strs[j], format_float(attr->value[j]), 128);
        
        char value_str[1024];
        memset(value_str, 0, 1024);
        switch (attr->size) {
        case 1:
            snprintf(value_str,
                     1024,
                     "%s",
                     value_strs[0]);
            break;
        case 2:
            snprintf(value_str,
                     1024,
                     "[%s %s]",
                     value_strs[0],
                     value_strs[1]);
            break;
        case 3:
            snprintf(value_str,
                     1024,
                     "[%s %s %s]",
                     value_strs[0],
                     value_strs[1],
                     value_strs[2]);
            break;
        case 4:
            snprintf(value_str,
                     1024,
                     "[%s %s %s %s]",
                     value_strs[0],
                     value_strs[1],
                     value_strs[2],
                     value_strs[3]);
            break;
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
                           10, value_str,
                           -1);
    }
}

void vao_init() {
    init_treeview(builder, "vao_treeview", 1);
    init_treeview(builder, "vao_attributes", 11);
}
