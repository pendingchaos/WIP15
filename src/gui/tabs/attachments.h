#ifndef ATTACHMENTS_H
#define ATTACHMENTS_H
#include "libtrace/libtrace.h"
#include "../utils.h"
#include "../gui.h"

typedef struct attachments_tab_t {
    gui_tab_t* tab;
    GtkComboBox* filter_combobox;
    GtkTreeView* treeview;
    info_box_t* info_box;
    bool filter[3]; //Show Error, Show Warning, Show Info
} attachments_tab_t;

gui_tab_t* open_attachments_tab();
#endif
