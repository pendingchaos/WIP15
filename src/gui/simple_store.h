#ifndef SIMPLE_STORE_H
#define SIMPLE_STORE_H
//A simple GtkTreeModel implementation for flat data
#include <gtk/gtk.h>

#include "libtrace/libtrace.h"

#define GUI_TYPE_SIMPLE_STORE gui_simple_store_get_type()
G_DECLARE_FINAL_TYPE(GuiSimpleStore, gui_simple_store, GUI, SIMPLE_STORE, GObject);

typedef struct GuiSimpleStoreUpdate {
    size_t size;
    size_t column_count;
    void (*get_func)(size_t index, size_t row, GValue* val, void* user_data);
    GType column_types[32];
    void* user_data;
} GuiSimpleStoreUpdate;

GuiSimpleStore* gui_simple_store_new();
void gui_simple_store_update(GuiSimpleStore* store, GuiSimpleStoreUpdate up);
#endif
