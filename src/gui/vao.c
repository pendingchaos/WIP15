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
    
    for (size_t i = 0; i < trace->inspection.gl_obj_history_count[TrcGLObj_VAO]; i++) {
        trc_gl_obj_history_t* h = &trace->inspection.gl_obj_history[TrcGLObj_VAO][i];
        const trc_gl_obj_rev_t* vao = trc_lookup_gl_obj(trace, revision, h->fake, TrcGLObj_VAO);
        if (vao && vao->ref_count) {
            char str[64];
            memset(str, 0, 64);
            snprintf(str, 64, "%u", (uint)h->fake);
            
            GtkTreeIter row;
            gtk_tree_store_append(store, &row, NULL);
            gtk_tree_store_set(store, &row, 0, str, -1);
        }
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
    
    size_t count = 0;
    trc_gl_vao_rev_t* vao = NULL;
    for (size_t i = 0; i <= trace->inspection.gl_obj_history_count[TrcGLObj_VAO]; i++) {
        trc_gl_obj_history_t* h = &trace->inspection.gl_obj_history[TrcGLObj_VAO][i];
        vao = (trc_gl_vao_rev_t*)trc_lookup_gl_obj(trace, revision, h->fake, TrcGLObj_VAO);
        if (vao && vao->ref_count) count++;
        if (count == index+1) break;
    }
    
    if (!vao) return; //TODO: Is this possible?
    
    for (size_t i = 0; i < vao->attrib_count; i++) {
        trc_gl_vao_attrib_t* attr = &vao->attribs[i];
        
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
        if (attr->buffer) snprintf(buffer_str, 64, "%u", attr->buffer);
        
        char value_strs[128][4];
        for (size_t j = 0; j < 4; j++)
            strncpy(value_strs[j], format_float(attr->value[j]), 128);
        
        char value_str[1024];
        memset(value_str, 0, 1024);
        if (attr->buffer == 0) {
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
