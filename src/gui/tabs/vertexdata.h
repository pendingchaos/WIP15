#ifndef TAB_VERTEXDATA_H
#define TAB_VERTEXDATA_H
#include "../utils.h"
#include "../gui.h"

typedef struct vertex_data_tab_t {
    gui_tab_t* tab;
    info_box_t* info_box;
    GtkToggleButton* autofill;
    GtkComboBox* mode;
    GtkSpinButton* start;
    GtkSpinButton* count;
    GtkSpinButton* instance;
    GtkSpinButton* base_instance;
    GtkSpinButton* base_vertex;
    GtkSpinButton* index_offset;
    GtkComboBox* index_type;
    
    GtkBox* data_view_box;
    GtkTreeView* data_view;
    size_t column_count;
    size_t attrib_column_start;
    int* attrib_columns;
    int* column_attribs;
    
    bool disable_fill;
} vertex_data_tab_t;

gui_tab_t* open_vertex_data_tab();
#endif
