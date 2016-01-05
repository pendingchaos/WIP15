#include "libinspect/libinspect.h"
#include "libtrace/libtrace.h"

#include <gtk/gtk.h>
#include <stdlib.h>
#include <stdio.h>

static GtkWidget* main_window;
GtkBuilder* builder;
GdkPixbuf* info_pixbuf;
GdkPixbuf* warning_pixbuf;
GdkPixbuf* error_pixbuf;
inspector_t* inspector;
trace_t* trace;

void buffer_init();
void vao_init();
void framebuffer_init();
void texture_init();
void shader_init();
void trace_init();

void quit_callback(GObject* obj, gpointer user_data) {
    gtk_main_quit();
}

void about_callback(GObject* obj, gpointer user_data) {
    gtk_show_about_dialog(GTK_WINDOW(main_window),
                          "program-name", "WIP15 Inspector",
                          "version", "0.0.0",
                          "website", "https://gitlab.com/pendingchaos/WIP15",
                          "logo-icon-name", "image-missing",
                          //"license-type", GTK_LICENSE_GPL_3_0_ONLY,
                          NULL);
}

int main(int argc, char** argv) {
    if (argc != 2) {
        fprintf(stderr, "Expected one argument. Got %d.\n", argc-1);
        fprintf(stderr, "Usage: inspect-gui <trace file>\n");
        return EXIT_FAILURE;
    }
    
    trace = load_trace(argv[1]);
    {
        trace_error_t error = get_trace_error();
        if (error == TraceError_Invalid) {
            fprintf(stderr, "Invalid trace file: %s\n", get_trace_error_desc());
            return EXIT_FAILURE;
        } else if (error == TraceError_UnableToOpen) {
            fprintf(stderr, "Unable to open trace file.\n");
            return EXIT_FAILURE;
        }
    }
    
    inspection_t* inspection = create_inspection(trace);
    inspect(inspection);
    
    inspector = create_inspector(inspection);
    
    gtk_init(&argc, &argv);
    
    GtkIconTheme* icon_theme = gtk_icon_theme_new();
    {
        GError* error = NULL;
        info_pixbuf = gtk_icon_theme_load_icon(icon_theme, "dialog-information", 16, 0, &error);
        if (error) {
            fprintf(stderr, "Unable to load information icon: %s\n", error->message);
            g_error_free(error);
            free_inspection(inspection);
            free_trace(trace);
            return EXIT_FAILURE;
        }
        warning_pixbuf = gtk_icon_theme_load_icon(icon_theme, "dialog-warning", 16, 0, &error);
        if (error) {
            fprintf(stderr, "Unable to load information icon: %s\n", error->message);
            g_object_unref(info_pixbuf);
            g_error_free(error);
            free_inspection(inspection);
            free_trace(trace);
            return EXIT_FAILURE;
        }
        error_pixbuf = gtk_icon_theme_load_icon(icon_theme, "dialog-error", 16, 0, &error);
        if (error) {
            fprintf(stderr, "Unable to load information icon: %s\n", error->message);
            g_object_unref(warning_pixbuf);
            g_object_unref(info_pixbuf);
            g_error_free(error);
            free_inspection(inspection);
            free_trace(trace);
            return EXIT_FAILURE;
        }
    }
    
    builder = gtk_builder_new_from_file("ui.glade");
    gtk_builder_connect_signals(builder, inspection);
    
    buffer_init();
    vao_init();
    framebuffer_init();
    texture_init();
    shader_init();
    trace_init();
    
    main_window = GTK_WIDGET(gtk_builder_get_object(builder, "main_window"));
    gtk_widget_show_all(main_window);
    
    gtk_main();
    
    g_object_unref(G_OBJECT(builder));
    
    g_object_unref(error_pixbuf);
    g_object_unref(warning_pixbuf);
    g_object_unref(info_pixbuf);
    g_object_unref(icon_theme);
    
    free_inspector(inspector);
    free_inspection(inspection);
    free_trace(trace);
    
    return EXIT_SUCCESS;
}
