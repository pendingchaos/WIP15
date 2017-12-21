#ifndef TREE_STORE_H
#define TREE_STORE_H
//A GtkTreeModel implementation wrapping the trace's command list
#include <gtk/gtk.h>

#include "libtrace/libtrace.h"

typedef bool (*GuiTreeStoreFilterFunc)(trace_command_t* cmd, void* udata);

#define GUI_TYPE_TREE_STORE gui_tree_store_get_type()
G_DECLARE_FINAL_TYPE(GuiTreeStore, gui_tree_store, GUI, TREE_STORE, GObject);

GuiTreeStore* gui_tree_store_new(trace_t* trace);
void gui_tree_store_set_trace(GuiTreeStore* self, trace_t* trace);
void gui_tree_store_set_filter_func(GuiTreeStore* self, GuiTreeStoreFilterFunc func, void* udata);
void gui_tree_store_refilter(GuiTreeStore* self);
void gui_tree_store_set_error_pixbuf(GuiTreeStore* self, GdkPixbuf* buf);
void gui_tree_store_set_warning_pixbuf(GuiTreeStore* self, GdkPixbuf* buf);
void gui_tree_store_set_info_pixbuf(GuiTreeStore* self, GdkPixbuf* buf);
#endif
