#ifndef GUI_H
#define GUI_H
#include <gtk/gtk.h>
#include <stdint.h>

#include "libtrace/libtrace.h"
#include "utils.h"

typedef struct gui_state_t gui_state_t;

typedef struct object_tab_t {
    struct object_tab_t* prev;
    struct object_tab_t* next;
    GtkNotebook* notebook; //Containing notebook
    GtkWidget* revision_checkbox;
    GtkWidget* revision_entry;
    GtkWidget* revision_box;
    GtkWidget* tab_label;
    GtkWidget* tab_label_button;
    GtkWidget* tab_label_box;
    GtkWidget* tab_child;
    GtkWidget* problem_label;
    trc_obj_t* obj;
    trc_obj_type_t type;
    void* data;
    
    object_notebook_t* obj_notebook;
    info_box_t* info_box;
} object_tab_t;

typedef void (*object_tab_callback_t)(object_tab_t* tab);
typedef void (*object_tab_update_callback_t)(object_tab_t* tab, const trc_obj_rev_head_t* rev, uint64_t revision);

extern object_tab_callback_t object_tab_init_callbacks[];
extern object_tab_callback_t object_tab_deinit_callbacks[];
extern object_tab_update_callback_t object_tab_update_callbacks[];

struct gui_state_t {
    bool gtk_was_init;
    GtkWidget* main_window;
    GtkBuilder* builder;
    GtkIconTheme* icon_theme;
    GdkPixbuf* info_pixbuf;
    GdkPixbuf* warning_pixbuf;
    GdkPixbuf* error_pixbuf;
    
    GtkTreeIter object_parents[(int)Trc_ObjMax];
    
    trace_t* trace;
    int64_t revision;
    object_tab_t* object_tabs;
};

extern gui_state_t state;

int run_gui(const char* trace, int argc, char** argv);
void update_object_tab(object_tab_t* tab);
#endif
