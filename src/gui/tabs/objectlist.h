#ifndef TAB_OBJECT_LIST_H
#define TAB_OBJECT_LIST_H
#include "libtrace/libtrace.h"
#include "../utils.h"
#include "../gui.h"

typedef struct object_list_tab_t {
    gui_tab_t* tab;
    GtkTreeIter object_parents[(int)Trc_ObjMax];
    GtkComboBox* context_combobox;
    GtkTreeView* objects_treeview;
    info_box_t* info_box;
} object_list_tab_t;

gui_tab_t* open_object_list_tab();
#endif
