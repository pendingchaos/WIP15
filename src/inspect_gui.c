#include "libinspect.h"
#include "libtrace.h"

#include <gtk/gtk.h>
#include <stdlib.h>
#include <stdio.h>

static GtkWidget* main_window;

typedef struct tree_item_t {
    const char* val;
    size_t child_count;
    const struct tree_item_t* children;
} tree_item_t;

static const tree_item_t _children2[] = {{"Child 3", 0, NULL}};
static const tree_item_t _children[] = {{"Child 1", 0, NULL}, {"Child 2", 1, _children2}};
static const tree_item_t items[] = {{"Hello world!", 0, NULL}, {"Hello world again!", 2, _children}, {"Goodbye world!", 0, NULL}};

static void populate_tree(GtkTreeStore* store,
                          GtkTreeIter* parent,
                          size_t count,
                          const tree_item_t* items) {
    for (size_t i = 0; i < count; ++i) {
        GtkTreeIter row;
        gtk_tree_store_append(store, &row, parent);
        gtk_tree_store_set(store, &row, 0, items[i].val, -1);
        populate_tree(store, &row, items[i].child_count, items[i].children);
    }
}

static GtkWidget* create_tree(size_t count, const tree_item_t* items) {
    GtkTreeStore* tree_store = gtk_tree_store_new(1, G_TYPE_STRING);
    
    populate_tree(tree_store, NULL, count, items);
    
    GtkWidget* tree = gtk_tree_view_new_with_model(GTK_TREE_MODEL(tree_store));
    gtk_tree_view_set_headers_visible(GTK_TREE_VIEW(tree), FALSE);
    g_object_unref(G_OBJECT(tree_store));
    
    GtkCellRenderer* renderer = gtk_cell_renderer_text_new();
    GtkTreeViewColumn* column = gtk_tree_view_column_new();
    gtk_tree_view_column_pack_start(column, renderer, FALSE);
    gtk_tree_view_column_set_attributes(column, renderer, "text", 0, NULL);
    gtk_tree_view_append_column(GTK_TREE_VIEW(tree), column);
    
    GtkWidget* window = gtk_scrolled_window_new(NULL, NULL);
    gtk_container_add(GTK_CONTAINER(window), tree);
    
    return window;
}

void quit_callback(GObject* obj, gpointer user_data) {
    gtk_main_quit();
}

void about_callback(GObject* obj, gpointer user_data) {
    gtk_show_about_dialog(GTK_WINDOW(main_window),
                          "program-name", "WIP15 Inspector",
                          "version", "0.0.0",
                          "website", "https://www.gitlab.com/u/pendingchaos/WIP15",
                          "logo-icon-name", "image-missing",
                          //"license-type", GTK_LICENSE_GPL_3_0_ONLY,
                          NULL);
}

int main(int argc, char** argv) {
    if (argc != 2) {
        fprintf(stderr, "Expected one argument. Got %d.\n", argc);
        fprintf(stderr, "Usage: inspect-gui <trace file>\n");
        return EXIT_FAILURE;
    }
    
    trace_t* trace = load_trace(argv[1]);
    
    trace_error_t error = get_trace_error();
    if (error == TraceError_Invalid) {
        fprintf(stderr, "Invalid trace file: %s\n", get_trace_error_desc());
        return EXIT_FAILURE;
    } else if (error == TraceError_UnableToOpen) {
        fprintf(stderr, "Unable to open trace file.\n");
        return EXIT_FAILURE;
    }
    
    inspection_t* inspection = create_inspection(trace);
    inspect(inspection);
    
    gtk_init(&argc, &argv);
    
    /*GtkWidget* window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(window), "WIP15 Inspector");
    gtk_window_set_default_size(GTK_WINDOW(window), 500, 500);
    g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);
    
    GtkWidget* tree = create_tree(3, items);
    gtk_container_add(GTK_CONTAINER(window), tree);
    
    gtk_widget_show_all(window);*/
    
    GtkBuilder* builder = gtk_builder_new_from_file("ui.glade");
    gtk_builder_connect_signals(builder, inspection);
    
    main_window = GTK_WIDGET(gtk_builder_get_object(builder, "main_window"));
    gtk_widget_show_all(main_window);
    
    gtk_main();
    
    g_object_unref(G_OBJECT(builder));
    
    free_inspection(inspection);
    free_trace(trace);
    
    return EXIT_SUCCESS;
}
