#include "libinspect/libinspect.h"
#include "libtrace/libtrace.h"
#include "utils.h"

#include <gtk/gtk.h>
#include <stdlib.h>
#include <stdio.h>

static GtkWidget* main_window;
GtkBuilder* builder;
GdkPixbuf* info_pixbuf;
GdkPixbuf* warning_pixbuf;
GdkPixbuf* error_pixbuf;
inspector_t* inspector = NULL;
inspection_t* inspection = NULL;
trace_t* trace = NULL;

void buffer_init();
void vao_init();
void framebuffer_init();
void renderbuffer_init();
void texture_init();
void shader_init();
void sync_init();
void query_init();
void trace_init();
void trace_fill();

static void reset_trace() {
    trace = malloc(sizeof(trace_t));
    trace->little_endian = true;
    trace->func_name_count = 0;
    trace->func_names = NULL;
    trace->group_name_count = 0;
    trace->group_names = NULL;
    trace->frames = alloc_trace_frame_vec(0);
    
    inspection = create_inspection(trace);
    inspector = create_inspector(inspection);
}

static void free_open_trace() {
    if (trace) {
        free_inspector(inspector);
        free_inspection(inspection);
        free_trace(trace);
    }
}

static void open_trace(const char* filename) {
    free_open_trace();
    
    trace = load_trace(filename);
    trace_error_t error = get_trace_error();
    if (error == TraceError_Invalid) {
        fprintf(stderr, "Invalid trace file: %s\n", get_trace_error_desc());
        reset_trace();
        trace_fill();
        return;
    } else if (error == TraceError_UnableToOpen) {
        fprintf(stderr, "Unable to open trace file.\n");
        reset_trace();
        trace_fill();
        return;
    }
    
    inspection = create_inspection(trace);
    inspect(inspection);
    
    inspector = create_inspector(inspection);
}

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

void new_callback(GObject* obj, gpointer user_data) {
    GtkDialog* dialog = GTK_DIALOG(
    gtk_dialog_new_with_buttons("New Trace",
                                GTK_WINDOW(main_window),
                                GTK_DIALOG_DESTROY_WITH_PARENT,
                                "OK",
                                GTK_RESPONSE_ACCEPT,
                                "Cancel",
                                GTK_RESPONSE_REJECT,
                                NULL));
    
    GtkWidget* content = gtk_dialog_get_content_area(dialog);
    
    GtkWidget* prog_button = gtk_file_chooser_button_new("Select a Program", GTK_FILE_CHOOSER_ACTION_OPEN);
    
    GtkEntry* arg_entry = GTK_ENTRY(gtk_entry_new());
    
    GtkBox* box = GTK_BOX(gtk_box_new(GTK_ORIENTATION_VERTICAL, 0));
    gtk_box_pack_end(box, GTK_WIDGET(arg_entry), FALSE, TRUE, 0);
    gtk_box_pack_end(box, GTK_WIDGET(prog_button), FALSE, TRUE, 0);
    
    gtk_container_add(GTK_CONTAINER(content), GTK_WIDGET(box));
    
    gtk_widget_show_all(GTK_WIDGET(dialog));
    if (gtk_dialog_run(dialog) == GTK_RESPONSE_ACCEPT) {
        const char* args = gtk_entry_get_text(arg_entry);
        char* program = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(prog_button));
        const char* cmd = static_format("./trace -l limits/this.limits.txt -o \"%s.trace\" %s %s", program, program, args);
        
        if (system(cmd)) {
            fprintf(stderr, "Unable to trace program\n");
        } else {
            open_trace(static_format("%s.trace", program));
            trace_fill();
        }
        
        g_free(program);
    }
    
    gtk_widget_destroy(prog_button);
    gtk_widget_destroy(GTK_WIDGET(arg_entry));
    gtk_widget_destroy(GTK_WIDGET(box));
    gtk_widget_destroy(GTK_WIDGET(dialog));
}

void open_callback(GObject* obj, gpointer user_data) {
    GtkWidget* dialog = gtk_file_chooser_dialog_new("Open trace",
                                                    GTK_WINDOW(main_window),
                                                    GTK_FILE_CHOOSER_ACTION_OPEN,
                                                    "Cancel",
                                                    GTK_RESPONSE_CANCEL,
                                                    "Open",
                                                    GTK_RESPONSE_ACCEPT,
                                                    NULL);
    
    if (gtk_dialog_run(GTK_DIALOG(dialog)) == GTK_RESPONSE_ACCEPT) {
        GtkFileChooser* chooser = GTK_FILE_CHOOSER(dialog);
        char* filename = gtk_file_chooser_get_filename(chooser);
        open_trace(filename);
        trace_fill();
        g_free(filename);
    }
    
    gtk_widget_destroy(dialog);
}

int main(int argc, char** argv) {
    if (argc > 2) {
        fprintf(stderr, "Expected one or two arguments. Got %d.\n", argc-1);
        fprintf(stderr, "Usage: inspect-gui <trace file (optional)>\n");
        return EXIT_FAILURE;
    }
    
    if (argc == 2)
        open_trace(argv[1]);
    else
        reset_trace();
    
    gtk_init(&argc, &argv);
    
    GtkIconTheme* icon_theme = gtk_icon_theme_new();
    GError* error = NULL;
    info_pixbuf = gtk_icon_theme_load_icon(icon_theme, "dialog-information", 16, 0, &error);
    if (error) {
        fprintf(stderr, "Unable to load information icon: %s\n", error->message);
        g_error_free(error);
        free_open_trace();
        return EXIT_FAILURE;
    }
    warning_pixbuf = gtk_icon_theme_load_icon(icon_theme, "dialog-warning", 16, 0, &error);
    if (error) {
        fprintf(stderr, "Unable to load information icon: %s\n", error->message);
        g_object_unref(info_pixbuf);
        g_error_free(error);
        free_open_trace();
        return EXIT_FAILURE;
    }
    error_pixbuf = gtk_icon_theme_load_icon(icon_theme, "dialog-error", 16, 0, &error);
    if (error) {
        fprintf(stderr, "Unable to load information icon: %s\n", error->message);
        g_object_unref(warning_pixbuf);
        g_object_unref(info_pixbuf);
        g_error_free(error);
        free_open_trace();
        return EXIT_FAILURE;
    }
    
    builder = gtk_builder_new_from_file("ui.glade");
    gtk_builder_connect_signals(builder, NULL);
    
    buffer_init();
    vao_init();
    framebuffer_init();
    renderbuffer_init();
    texture_init();
    shader_init();
    sync_init();
    query_init();
    trace_init();
    
    trace_fill();
    
    main_window = GTK_WIDGET(gtk_builder_get_object(builder, "main_window"));
    gtk_widget_show_all(main_window);
    
    gtk_main();
    
    g_object_unref(G_OBJECT(builder));
    
    g_object_unref(error_pixbuf);
    g_object_unref(warning_pixbuf);
    g_object_unref(info_pixbuf);
    g_object_unref(icon_theme);
    
    free_open_trace();
    
    return EXIT_SUCCESS;
}
