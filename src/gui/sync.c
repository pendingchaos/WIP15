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
    
    for (size_t i = 0; i < trace->inspection.gl_obj_history_count[TrcGLObj_Sync]; i++) {
        trc_gl_obj_history_t* h = &trace->inspection.gl_obj_history[TrcGLObj_Sync][i];
        const trc_gl_obj_rev_t* sync = trc_lookup_gl_obj(trace, revision, h->fake, TrcGLObj_Sync);
        if (sync && sync->ref_count) {
            char str[64];
            memset(str, 0, 64);
            snprintf(str, 64, "%lx", h->fake);
            
            GtkTreeIter row;
            gtk_tree_store_append(store, &row, NULL);
            gtk_tree_store_set(store, &row, 0, str, -1);
        }
    }
}

void sync_select_callback(GObject* obj, gpointer user_data) {
    GtkTreePath* path;
    gtk_tree_view_get_cursor(GTK_TREE_VIEW(obj), &path, NULL);
    
    if (!path) return;
    
    size_t index = gtk_tree_path_get_indices(path)[0];
    
    size_t count = 0;
    trc_gl_sync_rev_t* sync = NULL;
    for (size_t i = 0; i <= trace->inspection.gl_obj_history_count[TrcGLObj_Sync]; i++) {
        trc_gl_obj_history_t* h = &trace->inspection.gl_obj_history[TrcGLObj_Sync][i];
        sync = (trc_gl_sync_rev_t*)trc_lookup_gl_obj(trace, revision, h->fake, TrcGLObj_Sync);
        if (sync && sync->ref_count) count++;
        if (count == index+1) break;
    }
    if (!sync) return; //TODO: Is this possible?
    
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
