#include "simple_store.h"

#include <gtk/gtk.h>
#include <stdbool.h>
#include <stddef.h>

struct _GuiSimpleStore {
    GObject parent_instance;
    int stamp;
    
    size_t size; //row count
    size_t column_count;
    void (*get_func)(size_t index, size_t row, GValue* val, void* user_data);
    GType column_types[32];
    void* user_data;
};

static size_t unpack_iter(GtkTreeModel* model, GtkTreeIter* iter) {
    return (uintptr_t)iter->user_data;
}

static GtkTreeIter pack_iter(GtkTreeModel* model, size_t iter) {
    GtkTreeIter res = {.stamp=GUI_SIMPLE_STORE(model)->stamp,
                       .user_data=(gpointer)(uintptr_t)iter};
    return res;
}

static bool iter_valid(GtkTreeModel* model, GtkTreeIter* iter) {
    GuiSimpleStore* store = GUI_SIMPLE_STORE(model);
    
    if (iter->stamp != store->stamp) return false;
    size_t i = unpack_iter(model, iter);
    
    return i < store->size;
}

GuiSimpleStore* gui_simple_store_new(trace_t* trace) {
    GuiSimpleStore* self = (GuiSimpleStore*)g_object_new(GUI_TYPE_SIMPLE_STORE, NULL);
    
    do {
        self->stamp = ((gint64)g_random_int()) - G_MININT;
    } while (self->stamp == 0);
    
    self->size = 0;
    self->column_count = 0;
    self->get_func = NULL;
    for (size_t i = 0; i < sizeof(self->column_types)/sizeof(GType); i++)
        self->column_types[i] = G_TYPE_INVALID;
    self->user_data = NULL;
    
    return self;
}

void gui_simple_store_update(GuiSimpleStore* self, GuiSimpleStoreUpdate up) {
    GtkTreeModel* model = GTK_TREE_MODEL(self);
    
    //Run deleted signals
    GtkTreePath* path = gtk_tree_path_new_from_indices(self->size-1, -1);
    for (ptrdiff_t i = self->size-1; i >= 0; i--) {
        gtk_tree_model_row_deleted(model, path);
        gtk_tree_path_prev(path);
        self->size--;
    }
    gtk_tree_path_free(path);
    
    //Update store
    self->size = 0;
    self->column_count = up.column_count;
    self->get_func = up.get_func;
    self->user_data = up.user_data;
    
    for (size_t i = 0; i < sizeof(self->column_types)/sizeof(GType); i++)
        self->column_types[i] = G_TYPE_INVALID;
    for (size_t i = 0; i < self->column_count; i++)
        self->column_types[i] = up.column_types[i];
    
    //Run inserted signals
    path = gtk_tree_path_new_from_indices(0, -1);
    for (size_t i = 0; i < up.size; i++) {
        GtkTreeIter packed = pack_iter(model, i);
        gtk_tree_model_row_inserted(model, path, &packed);
        
        gtk_tree_path_next(path);
        self->size++;
    }
    gtk_tree_path_free(path);
}

static GtkTreeModelFlags get_flags(GtkTreeModel* self) {
    return 0;
}

static gint get_n_columns(GtkTreeModel* self) {
    return GUI_SIMPLE_STORE(self)->column_count;
}

static GType get_column_type(GtkTreeModel* self, gint index) {
    GuiSimpleStore* store = GUI_SIMPLE_STORE(self);
    if (index >= store->column_count) return G_TYPE_INVALID;
    return store->column_types[index];
}

static gboolean get_iter(GtkTreeModel* self, GtkTreeIter* iter, GtkTreePath* path) {
    if (gtk_tree_path_get_depth(path) != 1) return FALSE;
    gint index = gtk_tree_path_get_indices(path)[0];
    if (index >= GUI_SIMPLE_STORE(self)->size) return FALSE;
    *iter = pack_iter(self, index);
    return TRUE;
}

static GtkTreePath* get_path(GtkTreeModel* self, GtkTreeIter* iter) {
    g_return_val_if_fail(iter_valid(self, iter), NULL);
    return gtk_tree_path_new_from_indices(unpack_iter(self, iter), -1);
}

static void get_value(GtkTreeModel* self, GtkTreeIter* iter, gint column, GValue* value) {
    g_return_if_fail(iter_valid(self, iter));
    GuiSimpleStore* store = GUI_SIMPLE_STORE(self);
    g_value_init(value, get_column_type(self, column));
    store->get_func(column, unpack_iter(self, iter), value, store->user_data);
}

static gboolean iter_next(GtkTreeModel* self, GtkTreeIter* iter) {
    g_return_val_if_fail(iter_valid(self, iter), FALSE);
    
    iter->stamp = 0;
    size_t i = unpack_iter(self, iter) + 1;
    if (i >= GUI_SIMPLE_STORE(self)->size) return FALSE;
    *iter = pack_iter(self, i);
    return TRUE;
}

static gboolean iter_previous(GtkTreeModel* self, GtkTreeIter* iter) {
    g_return_val_if_fail(iter_valid(self, iter), FALSE);
    
    iter->stamp = 0;
    size_t i = unpack_iter(self, iter);
    if (i == 0) return FALSE;
    *iter = pack_iter(self, i - 1);
    return TRUE;
}

static gboolean iter_children(GtkTreeModel* self, GtkTreeIter* iter, GtkTreeIter* parent) {
    iter->stamp = 0;
    
    if (parent != NULL) return FALSE;
    
    if (GUI_SIMPLE_STORE(self)->size == 0)
        return FALSE;
    *iter = pack_iter(self, 0);
    return TRUE;
}

static gboolean iter_has_child(GtkTreeModel* self, GtkTreeIter* iter) {
    return FALSE;
}

static gint iter_n_children(GtkTreeModel* self, GtkTreeIter* iter) {
    return 0;
}

static gboolean iter_nth_child(GtkTreeModel* self, GtkTreeIter* iter, GtkTreeIter* parent, gint n) {
    iter->stamp = 0;
    return FALSE;
}

static gboolean iter_parent(GtkTreeModel* self, GtkTreeIter* iter, GtkTreeIter* child) {
    iter->stamp = 0;
    return FALSE;
}

static void ref_node(GtkTreeModel* self, GtkTreeIter* iter) {}

static void unref_node(GtkTreeModel* self, GtkTreeIter* iter) {}

static void gui_simple_store_interface_init(GtkTreeModelIface* iface) {
    iface->get_flags = get_flags;
    iface->get_n_columns = get_n_columns;
    iface->get_column_type = get_column_type;
    iface->get_iter = get_iter;
    iface->get_path = get_path;
    iface->get_value = get_value;
    iface->iter_next = iter_next;
    iface->iter_previous = iter_previous;
    iface->iter_children = iter_children;
    iface->iter_has_child = iter_has_child;
    iface->iter_n_children = iter_n_children;
    iface->iter_nth_child = iter_nth_child;
    iface->iter_parent = iter_parent;
    iface->ref_node = ref_node;
    iface->unref_node = unref_node;
}

G_DEFINE_TYPE_WITH_CODE(GuiSimpleStore, gui_simple_store, G_TYPE_OBJECT,
                        G_IMPLEMENT_INTERFACE(GTK_TYPE_TREE_MODEL,
                                              gui_simple_store_interface_init))

static void finalize(GObject* gobject) {
    G_OBJECT_CLASS(gui_simple_store_parent_class)->finalize(gobject);
}

static void gui_simple_store_class_init(GuiSimpleStoreClass* klass) {
    GObjectClass* obj_class = G_OBJECT_CLASS(klass);
    obj_class->finalize = finalize;
}

static void gui_simple_store_init(GuiSimpleStore* self) {
    //Everything is done in gui_simple_store_new()
}
