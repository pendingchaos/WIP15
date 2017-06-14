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
    
    TRC_ITER_OBJECTS_BEGIN(TrcQuery, trc_gl_query_rev_t)
        char str[64];
        memset(str, 0, 64);
        snprintf(str, 64, "%u", (uint)rev->fake);
        
        GtkTreeIter row;
        gtk_tree_store_append(store, &row, NULL);
        gtk_tree_store_set(store, &row, 0, str, -1);
    TRC_ITER_OBJECTS_END
}

void query_select_callback(GObject* obj, gpointer user_data) {
    GtkTreePath* path;
    gtk_tree_view_get_cursor(GTK_TREE_VIEW(obj), &path, NULL);
    if (!path) return;
    
    size_t index = gtk_tree_path_get_indices(path)[0];
    
    size_t count = 0;
    const trc_gl_query_rev_t* query = NULL;
    TRC_ITER_OBJECTS_BEGIN(TrcQuery, trc_gl_query_rev_t)
        if (count == index+1) {
            query = rev;
            break;
        }
    TRC_ITER_OBJECTS_END
    
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
