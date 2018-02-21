#ifndef GUI_H
#define GUI_H
#include <gtk/gtk.h>
#include <stdint.h>

#include "libtrace/libtrace.h"
#include "utils.h"

typedef struct gui_state_t gui_state_t;

typedef struct gui_tab_t gui_tab_t;

typedef void (*gui_tab_callback_t)(gui_tab_t* tab);

struct gui_tab_t {
    struct gui_tab_t* prev;
    struct gui_tab_t* next;
    GtkNotebook* notebook; //Containing nodebook
    GtkWidget* tab_label;
    GtkWidget* tab_child;
    gui_tab_callback_t deinit;
    gui_tab_callback_t update;
    void* data;
};

#include "tabs/tabs.h"

struct gui_state_t {
    bool gtk_was_init;
    GtkWidget* main_window;
    GtkWidget* prefs_window;
    GtkBuilder* builder;
    GtkBuilder* prefs_builder;
    GtkIconTheme* icon_theme;
    GdkPixbuf* info_pixbuf;
    GdkPixbuf* warning_pixbuf;
    GdkPixbuf* error_pixbuf;
    
    GtkTreeIter object_parents[(int)Trc_ObjMax];
    
    bool pref_file_dirty;
    //These settings only take effect at startup
    trc_data_settings_t data_settings;
    
    trace_t* trace;
    int64_t revision;
    trace_command_t* selected_cmd;
    gui_tab_t* tabs;
};

extern gui_state_t state;

int run_gui(const char* trace, int argc, char** argv);
void fill_prefs_window();
void read_preferences();
void write_preferences();
gui_tab_t* open_gui_tab(bool closeable, GtkWidget* content);
void close_tab(gui_tab_t* tab);
void update_tab(gui_tab_t* tab);
void goto_revision(int64_t revision);
#endif
