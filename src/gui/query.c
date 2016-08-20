#include "libtrace/libtrace.h"
#include "utils.h"

#include <gtk/gtk.h>
#include <string.h>

extern GtkBuilder* builder;
extern trace_t* trace;
extern int64_t revision;

void init_queries_list(GtkTreeView* tree) {
    GtkTreeView* content = GTK_TREE_VIEW(gtk_builder_get_object(builder, "query_treeview"));
    GtkTreeStore* store = GTK_TREE_STORE(gtk_tree_view_get_model(content));
    gtk_tree_store_clear(store);
    
    store = GTK_TREE_STORE(gtk_tree_view_get_model(tree));
    gtk_tree_store_clear(store);
    
    for (size_t i = 0; i < trace->inspection.gl_obj_history_count[TrcGLObj_Query]; i++) {
        trc_gl_obj_history_t* h = &trace->inspection.gl_obj_history[TrcGLObj_Query][i];
        trc_gl_obj_rev_t* query = trc_lookup_gl_obj(trace, revision, h->fake, TrcGLObj_Query);
        if (query && query->ref_count) {
            char str[64];
            memset(str, 0, 64);
            snprintf(str, 64, "%lx", h->fake);
            
            GtkTreeIter row;
            gtk_tree_store_append(store, &row, NULL);
            gtk_tree_store_set(store, &row, 0, str, -1);
        }
    }
}

void query_select_callback(GObject* obj, gpointer user_data) {
    GtkTreePath* path;
    gtk_tree_view_get_cursor(GTK_TREE_VIEW(obj), &path, NULL);
    if (!path) return;
    
    size_t index = gtk_tree_path_get_indices(path)[0];
    
    size_t count = 0;
    trc_gl_query_rev_t* query = NULL;
    for (size_t i = 0; i <= trace->inspection.gl_obj_history_count[TrcGLObj_Query]; i++) {
        trc_gl_obj_history_t* h = &trace->inspection.gl_obj_history[TrcGLObj_Query][i];
        query = (trc_gl_query_rev_t*)trc_lookup_gl_obj(trace, revision, h->fake, TrcGLObj_Query);
        if (query && query->ref_count) count++;
        if (count == index+1) break;
    }
    
    if (!query) return; //TODO: Is this possible?
    
    GtkTreeView* tree = GTK_TREE_VIEW(gtk_builder_get_object(builder, "query_treeview"));
    GtkTreeStore* store = GTK_TREE_STORE(gtk_tree_view_get_model(tree));
    gtk_tree_store_clear(store);
    
    if (query->type) {
        GtkTreeIter row;
        gtk_tree_store_append(store, &row, NULL);
        gtk_tree_store_set(store, &row, 0, "Type", 1, get_enum_str(NULL, query->type), -1); //TODO: The group
        gtk_tree_store_append(store, &row, NULL);
        gtk_tree_store_set(store, &row, 0, "Result", 1, static_format("%llu", query->result), -1);
    }
}

void query_init() {
    init_treeview(builder, "query_list", 1);
    init_treeview(builder, "query_treeview", 2);
}
