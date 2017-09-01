#include "object_notebook.h"
#include "../utils.h"

#include <string.h>
#include <stdlib.h>

object_notebook_t* create_object_notebook() {
    object_notebook_t* notebook = malloc(sizeof(object_notebook_t));
    notebook->widget = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    notebook->notebook = GTK_NOTEBOOK(gtk_notebook_new());
    notebook->container_contents = NULL;
    notebook->tabs = NULL;
    return notebook;
}

void destroy_object_notebook(object_notebook_t* notebook) {
    for (object_notebook_tab_t* tab = notebook->tabs; tab;) {
        object_notebook_tab_t* next = tab->next;
        free(tab);
        tab = next;
    }
    gtk_widget_destroy(notebook->widget);
    free(notebook);
}

static void update_object_notebook_box(object_notebook_t* notebook, GtkWidget* widget) {
    if (notebook->container_contents != widget) {
        if (notebook->container_contents) {
            g_object_ref(notebook->container_contents);
            gtk_container_remove(GTK_CONTAINER(notebook->widget), notebook->container_contents);
        }
        if (widget)
            gtk_box_pack_start(GTK_BOX(notebook->widget), GTK_WIDGET(widget), true, true, 0);
        notebook->container_contents = widget;
    }
    
    if (widget) gtk_widget_show_all(widget);
}

static void close_object_notebook_tab(GtkWidget* _, object_notebook_tab_t* tab) {
    object_notebook_t* notebook = tab->notebook;
    
    if (tab->prev) tab->prev->next = tab->next;
    else notebook->tabs = tab->next;
    if (tab->next) tab->next->prev = tab->prev;
    
    if (notebook->container_contents == GTK_WIDGET(notebook->notebook))
        gtk_container_remove(GTK_CONTAINER(notebook->notebook), tab->widget);
    
    if (notebook->tabs == NULL) {
        //Now no tabs
        update_object_notebook_box(notebook, NULL);
    } else if (notebook->tabs->next == NULL) {
        //Now one tab
        gtk_container_remove(GTK_CONTAINER(notebook->notebook), notebook->tabs->widget);
        update_object_notebook_box(notebook, notebook->tabs->widget);
        gtk_widget_show_all(GTK_WIDGET(notebook->notebook));
    } else {
        //Still multiple tabs
    }
    
    if (tab->on_close) tab->on_close(tab->udata);
    free(tab);
}

static GtkWidget* create_tab_label(object_notebook_tab_t* tab) {
    GtkWidget* label_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
    gtk_box_pack_start(GTK_BOX(label_box), gtk_label_new(tab->title), true, true, 0);
    if (tab->closeable) {
        GtkWidget* close_button = gtk_button_new_from_icon_name("window-close", GTK_ICON_SIZE_MENU);
        g_signal_connect(close_button, "clicked", G_CALLBACK(close_object_notebook_tab), tab);
        gtk_button_set_relief(GTK_BUTTON(close_button), GTK_RELIEF_NONE);
        gtk_box_pack_start(GTK_BOX(label_box), close_button, false, false, 0);
    }
    return label_box;
}

void add_object_notebook_tab(object_notebook_t* notebook, const char* title,
                             bool closeable, GtkWidget* widget,
                             void (*on_close)(void* udata), void* udata) {
    widget = create_scrolled_window(widget);
    object_notebook_tab_t* tab = calloc(1, sizeof(object_notebook_tab_t));
    strncpy(tab->title, title, sizeof(tab->title)-1);
    tab->notebook = notebook;
    tab->closeable = closeable;
    tab->prev = NULL;
    tab->next = notebook->tabs;
    if (tab->next) tab->next->prev = tab;
    notebook->tabs = tab;
    tab->widget = widget;
    tab->on_close = on_close;
    tab->udata = udata;
    
    if (tab->next != NULL) {
        if (notebook->container_contents != GTK_WIDGET(notebook->notebook)) {
            //When transitioning from 1 tab -> 2 tabs
            
            //ensure tab->next->widget has no parent
            update_object_notebook_box(notebook, GTK_WIDGET(notebook->notebook));
            
            GtkWidget* label = create_tab_label(tab->next);
            gtk_notebook_append_page(notebook->notebook, tab->next->widget, label);
            gtk_widget_show_all(label);
            gtk_notebook_set_tab_reorderable(notebook->notebook, tab->next->widget, true);
        }
        
        GtkWidget* label = create_tab_label(tab);
        gtk_notebook_append_page(notebook->notebook, widget, label);
        gtk_widget_show_all(label);
        gtk_notebook_set_tab_reorderable(notebook->notebook, widget, true);
        update_object_notebook_box(notebook, GTK_WIDGET(notebook->notebook));
    } else {
        update_object_notebook_box(notebook, widget);
    }
}
