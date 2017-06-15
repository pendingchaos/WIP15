#include "libtrace/libtrace.h"
#include "utils.h"

#include <gtk/gtk.h>
#include <string.h>

extern GtkBuilder* builder;
extern trace_t* trace;
extern int64_t revision;

void init_syncs_list(GtkTreeView* tree) {
    GtkTreeView* content = GTK_TREE_VIEW(gtk_builder_get_object(builder, "sync_treeview"));
    GtkTreeStore* store = GTK_TREE_STORE(gtk_tree_view_get_model(content));
    gtk_tree_store_clear(store);
    
    store = GTK_TREE_STORE(gtk_tree_view_get_model(tree));
    gtk_tree_store_clear(store);
    
    const trc_gl_sync_rev_t* rev;
    for (size_t i = 0; trc_iter_objects(trace, TrcSync, &i, revision, (const void**)&rev);) {
        char str[64];
        memset(str, 0, 64);
        snprintf(str, 64, "%u", (uint)rev->fake);
        
        GtkTreeIter row;
        gtk_tree_store_append(store, &row, NULL);
        gtk_tree_store_set(store, &row, 0, str, -1);
    }
}

void sync_select_callback(GObject* obj, gpointer user_data) {
    GtkTreePath* path;
    gtk_tree_view_get_cursor(GTK_TREE_VIEW(obj), &path, NULL);
    
    if (!path) return;
    
    size_t index = gtk_tree_path_get_indices(path)[0];
    
    size_t count = 0;
    const trc_gl_sync_rev_t* sync = NULL;
    for (size_t i = 0; trc_iter_objects(trace, TrcSync, &i, revision, (const void**)&sync);) {
        if (count == index-1) break;
        count++;
    }
    
    GtkTreeView* tree = GTK_TREE_VIEW(gtk_builder_get_object(builder, "sync_treeview"));
    GtkTreeStore* store = GTK_TREE_STORE(gtk_tree_view_get_model(tree));
    gtk_tree_store_clear(store);
    
    GtkTreeIter row;
    gtk_tree_store_append(store, &row, NULL);
    gtk_tree_store_set(store, &row, 0, "Type", 1, get_enum_str(NULL, sync->type), -1);
    gtk_tree_store_append(store, &row, NULL);
    gtk_tree_store_set(store, &row, 0, "Condition", 1, get_enum_str(NULL, sync->condition), -1);
    gtk_tree_store_append(store, &row, NULL);
    gtk_tree_store_set(store, &row, 0, "Flags", 1, static_format("%u", sync->flags), -1);
}

void sync_init() {
    init_treeview(builder, "sync_list", 1);
    init_treeview(builder, "sync_treeview", 2);
}
