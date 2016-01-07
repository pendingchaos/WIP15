#include "libinspect/libinspect.h"
#include "utils.h"

#include <gtk/gtk.h>
#include <string.h>

extern GtkBuilder* builder;
extern inspector_t* inspector;

void init_queries_list(GtkTreeView* tree) {
    GtkTreeView* content = GTK_TREE_VIEW(gtk_builder_get_object(builder, "query_treeview"));
    GtkTreeStore* store = GTK_TREE_STORE(gtk_tree_view_get_model(content));
    gtk_tree_store_clear(store);
    
    store = GTK_TREE_STORE(gtk_tree_view_get_model(tree));
    gtk_tree_store_clear(store);
    
    inspect_query_vec_t queries = inspector->queries;
    for (inspect_query_t* query = queries->data; !vec_end(queries, query); query++) {
        char str[64];
        memset(str, 0, 64);
        snprintf(str, 64, "%u", query->fake);
        
        GtkTreeIter row;
        gtk_tree_store_append(store, &row, NULL);
        gtk_tree_store_set(store, &row, 0, str, -1);
    }
}

void query_select_callback(GObject* obj, gpointer user_data) {
    GtkTreePath* path;
    gtk_tree_view_get_cursor(GTK_TREE_VIEW(obj), &path, NULL);
    
    if (!path)
        return;
    
    size_t index = gtk_tree_path_get_indices(path)[0];
    
    inspect_query_t* query = get_inspect_query_vec(inspector->queries, index);
    if (!query)
        return;
    
    GtkTreeView* tree = GTK_TREE_VIEW(gtk_builder_get_object(builder, "query_treeview"));
    GtkTreeStore* store = GTK_TREE_STORE(gtk_tree_view_get_model(tree));
    gtk_tree_store_clear(store);
    
    if (query->type) {
        GtkTreeIter row;
        gtk_tree_store_append(store, &row, NULL);
        gtk_tree_store_set(store, &row, 0, "Type", 1, get_enum_str(NULL, query->type), -1);
        gtk_tree_store_append(store, &row, NULL);
        gtk_tree_store_set(store, &row, 0, "Result", 1, static_format("%llu", query->result), -1);
    }
}

void query_init() {
    init_treeview(builder, "query_list", 1);
    init_treeview(builder, "query_treeview", 2);
}
