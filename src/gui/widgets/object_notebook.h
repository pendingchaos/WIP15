#ifndef OBJECT_NOTEBOOK_H
#define OBJECT_NOTEBOOK_H
#include <stdbool.h>
#include <gtk/gtk.h>

typedef struct object_notebook_t object_notebook_t;

typedef struct object_notebook_tab_t {
    char title[256];
    bool closeable;
    object_notebook_t* notebook;
    struct object_notebook_tab_t* prev;
    struct object_notebook_tab_t* next;
    GtkWidget* widget;
    void (*on_close)(void* udata);
    void* udata;
} object_notebook_tab_t;

struct object_notebook_t {
    GtkWidget* widget;
    GtkWidget* container_contents;
    GtkNotebook* notebook;
    object_notebook_tab_t* tabs;
};

object_notebook_t* create_object_notebook();
void destroy_object_notebook(object_notebook_t* notebook);
//gtk_widget_show_all(widget) will be called upon tab creation and closing
void add_object_notebook_tab(object_notebook_t* notebook, const char* title,
                             bool closeable, GtkWidget* widget,
                             void (*on_close)(void* udata), void* udata);
#endif
