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
    
    create_obj_list(store, TrcQuery);
}

void query_select_callback(GObject* obj, gpointer user_data) {
    GtkTreePath* path;
    gtk_tree_view_get_cursor(GTK_TREE_VIEW(obj), &path, NULL);
    if (!path) return;
    
    size_t index = gtk_tree_path_get_indices(path)[0];
    
    size_t count = 0;
    const trc_gl_query_rev_t* query = NULL;
    for (size_t i = 0; trc_iter_objects(trace, TrcQuery, &i, revision, (const void**)&query);) {
        if (count == index-1) break;
        count++;
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
