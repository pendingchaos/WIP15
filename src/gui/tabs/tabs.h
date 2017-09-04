#ifndef TABS_H
#define TABS_H
#include "libtrace/libtrace.h"
#include "../utils.h"
#include "../gui.h"

typedef struct object_tab_t {
    gui_tab_t* tab;
    GtkWidget* revision_checkbox;
    GtkWidget* revision_entry;
    GtkWidget* revision_box;
    GtkWidget* problem_label;
    trc_obj_t* obj;
    trc_obj_type_t type;
    void* data;
    object_notebook_t* obj_notebook;
    info_box_t* info_box;
} object_tab_t;

typedef struct object_list_tab_t {
    gui_tab_t* tab;
    GtkTreeIter object_parents[(int)Trc_ObjMax];
    GtkComboBox* context_combobox;
    GtkTreeView* objects_treeview;
    info_box_t* info_box;
} object_list_tab_t;

typedef void (*object_tab_callback_t)(object_tab_t* tab);
typedef void (*object_tab_update_callback_t)(object_tab_t* tab, const trc_obj_rev_head_t* rev, uint64_t revision);

extern object_tab_callback_t object_tab_init_callbacks[];
extern object_tab_callback_t object_tab_deinit_callbacks[];
extern object_tab_update_callback_t object_tab_update_callbacks[];

gui_tab_t* open_object_tab(trc_obj_t* obj);
gui_tab_t* open_object_list_tab();
#endif
