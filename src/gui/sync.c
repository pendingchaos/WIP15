#include "libinspect/libinspect.h"
#include "utils.h"

#include <gtk/gtk.h>
#include <string.h>

extern GtkBuilder* builder;
extern inspector_t* inspector;

void init_syncs_list(GtkTreeView* tree) {
    GtkTreeView* content = GTK_TREE_VIEW(gtk_builder_get_object(builder, "sync_treeview"));
    GtkTreeStore* store = GTK_TREE_STORE(gtk_tree_view_get_model(content));
    gtk_tree_store_clear(store);
    
    store = GTK_TREE_STORE(gtk_tree_view_get_model(tree));
    gtk_tree_store_clear(store);
    
    inspect_sync_vec_t syncs = inspector->syncs;
    for (inspect_sync_t* sync = syncs->data; !vec_end(syncs, sync); sync++) {
        char str[64];
        memset(str, 0, 64);
        snprintf(str, 64, "0x%zx", sync->fake);
        
        GtkTreeIter row;
        gtk_tree_store_append(store, &row, NULL);
        gtk_tree_store_set(store, &row, 0, str, -1);
    }
}

void sync_select_callback(GObject* obj, gpointer user_data) {
    GtkTreePath* path;
    gtk_tree_view_get_cursor(GTK_TREE_VIEW(obj), &path, NULL);
    
    if (!path)
        return;
    
    size_t index = gtk_tree_path_get_indices(path)[0];
    
    inspect_sync_t* sync = get_inspect_sync_vec(inspector->syncs, index);
    if (!sync)
        return;
    
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
