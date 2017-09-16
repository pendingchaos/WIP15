#include "tabs/tabs.h"
#include "utils.h"
#include "gui.h"

#include <gtk/gtk.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

gui_state_t state;

static void fill_trace_view() {
    GtkTreeView* tree = GTK_TREE_VIEW(gtk_builder_get_object(state.builder, "trace_view"));
    GtkTreeStore* store = GTK_TREE_STORE(gtk_tree_view_get_model(tree));
    gtk_tree_store_clear(store);
    
    if (!state.trace) return;
    
    for (size_t i = 0; i < state.trace->frame_count; ++i) {
        trace_frame_t* frame = state.trace->frames + i;
        
        GtkTreeIter frame_row;
        gtk_tree_store_append(store, &frame_row, NULL);
        
        bool frame_error = false;
        bool frame_warning = false;
        bool frame_info = false;
        for (size_t j = 0; j < frame->command_count; ++j) {
            GtkTreeIter cmd_row;
            gtk_tree_store_append(store, &cmd_row, &frame_row);
            
            trace_command_t* cmd = frame->commands + j;
            char cmd_str[1024] = {0};
            format_command(state.trace, cmd_str, cmd, 1024);
            
            bool error = false;
            bool warning = false;
            bool info = false;
            trc_attachment_t* attachment = cmd->attachments;
            while (attachment) {
                error = error || attachment->type == TrcAttachType_Error;
                warning = warning || attachment->type == TrcAttachType_Warning;
                info = info || attachment->type == TrcAttachType_Info;
                attachment = attachment->next;
            }
            frame_error = frame_error || error;
            frame_warning = frame_warning || warning;
            frame_info = frame_info || info;
            
            GdkPixbuf* pixbuf = NULL;
            if (error) pixbuf = state.error_pixbuf;
            else if (warning) pixbuf = state.warning_pixbuf;
            else if (info) pixbuf = state.info_pixbuf;
            
            char rev_str[64] = {0};
            sprintf(rev_str, "%lu", cmd->revision);
            
            gtk_tree_store_set(store, &cmd_row, 0, pixbuf, 1, rev_str, 2, cmd_str, -1);
        }
        
        char frame_str[32] = {0};
        snprintf(frame_str, 32, "Frame %zu", i);
        
        GdkPixbuf* pixbuf = NULL;
        if (frame_error) pixbuf = state.error_pixbuf;
        else if (frame_warning) pixbuf = state.warning_pixbuf;
        else if (frame_info) pixbuf = state.info_pixbuf;
        
        gtk_tree_store_set(store, &frame_row, 0, pixbuf, 1, "", 2, frame_str, -1);
    }
}

void close_tab(gui_tab_t* tab) {
    if (tab->prev) tab->prev->next = tab->next;
    else state.tabs = tab->next;
    if (tab->next) tab->next->prev = tab->prev;
    
    if (tab->deinit) tab->deinit(tab);
    gtk_container_remove(GTK_CONTAINER(tab->notebook), tab->tab_child);
    
    free(tab);
}

void update_tab(gui_tab_t* tab) {
    if (tab->update) tab->update(tab);
}

static void tab_close_callback(GtkButton* button, gui_tab_t* tab) {
    close_tab(tab);
}

gui_tab_t* open_gui_tab(bool closeable, GtkWidget* content) {
    gui_tab_t* tab = malloc(sizeof(gui_tab_t));
    tab->prev = NULL;
    tab->next = state.tabs;
    if (tab->next) tab->next->prev = tab;
    state.tabs = tab;
    
    tab->tab_label = gtk_label_new("");
    
    GtkWidget* label = tab->tab_label;
    if (closeable) {
        GtkWidget* label_button = gtk_button_new_from_icon_name("window-close", GTK_ICON_SIZE_MENU);
        g_signal_connect(label_button, "clicked", G_CALLBACK(tab_close_callback), tab);
        gtk_button_set_relief(GTK_BUTTON(label_button), GTK_RELIEF_NONE);
        label = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
        gtk_box_pack_start(GTK_BOX(label), tab->tab_label, true, true, 0);
        gtk_box_pack_start(GTK_BOX(label), label_button, false, false, 0);
    }
    
    tab->tab_child = content;
    
    tab->notebook = GTK_NOTEBOOK(gtk_builder_get_object(state.builder, "notebook"));
    gtk_notebook_append_page(tab->notebook, tab->tab_child, label);
    gtk_notebook_set_tab_reorderable(tab->notebook, tab->tab_child, true);
    gtk_widget_show_all(label);
    gtk_widget_show_all(tab->tab_child);
    
    tab->deinit = NULL;
    tab->update = NULL;
    
    return tab;
}

static void update_gui_for_revision() {
    if (!state.trace) {
        for (gui_tab_t* tab = state.tabs; tab;) {
            gui_tab_t* next = tab->next;
            close_tab(tab);
            tab = next;
        }
        state.tabs = NULL;
    } else {
        for (gui_tab_t* tab = state.tabs; tab; tab = tab->next)
            update_tab(tab);
    }
}

void command_select_callback(GObject* obj, gpointer user_data) {
    GtkTreePath* path;
    gtk_tree_view_get_cursor(GTK_TREE_VIEW(obj), &path, NULL);
    if (!path) return;
    
    GObject* cmd_view = gtk_builder_get_object(state.builder, "selected_command_attachments");
    GtkTreeStore* cmd_store = GTK_TREE_STORE(gtk_tree_view_get_model(GTK_TREE_VIEW(cmd_view)));
    gtk_tree_store_clear(cmd_store);
    
    if (!state.trace) return;
    
    if (gtk_tree_path_get_depth(path) == 2) {
        gint* indices = gtk_tree_path_get_indices(path);
        
        trace_frame_t* frame = state.trace->frames + indices[0];
        trace_command_t* cmd = frame->commands + indices[1];
        state.revision = cmd->revision;
        state.selected_cmd = cmd;
        
        update_gui_for_revision();
        
        char cmd_str[1024] = {0};
        format_command_ext(state.trace, cmd_str, cmd, 1024);
        GtkTreeIter row;
        gtk_tree_store_append(cmd_store, &row, NULL);
        gtk_tree_store_set(cmd_store, &row, 0, cmd_str, -1);
        
        trc_attachment_t* attachment = cmd->attachments;
        while (attachment) {
            GtkTreeIter row;
            gtk_tree_store_append(cmd_store, &row, NULL);
            gtk_tree_store_set(cmd_store, &row, 0, attachment->message, -1);
            attachment = attachment->next;
        }
    } else {
        state.revision = -1;
        state.selected_cmd = NULL;
        update_gui_for_revision();
    }
}

static void free_open_trace() {
    trace_t* trace = state.trace;
    state.trace = NULL;
    state.revision = -1;
    state.selected_cmd = NULL;
    
    if (state.gtk_was_init) {
        fill_trace_view();
        update_gui_for_revision();
    }
    
    if (trace) free_trace(state.trace);
}

static void open_trace(const char* filename) {
    trace_t* trace = load_trace(filename);
    trace_error_t error = trc_get_error();
    if (error != TraceError_None) {
        display_error_dialog(static_format("Failed to open trace file: %s", trc_get_error_desc()));
        free_trace(trace);
        return;
    }
    
    trc_run_inspection(trace);
    
    free_open_trace();
    state.trace = trace;
    
    if (state.gtk_was_init) {
        fill_trace_view();
        update_gui_for_revision();
        open_object_list_tab();
    }
}

static void cleanup() {
    free_open_trace();
    if (state.builder) g_object_unref(G_OBJECT(state.builder));
    if (state.error_pixbuf) g_object_unref(state.error_pixbuf);
    if (state.warning_pixbuf) g_object_unref(state.warning_pixbuf);
    if (state.info_pixbuf) g_object_unref(state.info_pixbuf);
    if (state.icon_theme) g_object_unref(state.icon_theme);
}

void quit_callback(GObject* obj, gpointer user_data) {
    cleanup();
    gtk_main_quit();
}

void about_callback(GObject* obj, gpointer user_data) {
    gtk_show_about_dialog(GTK_WINDOW(state.main_window),
                          "program-name", "WIP15 Inspector",
                          "version", "0.0.0",
                          "website", "https://gitlab.com/pendingchaos/WIP15",
                          "logo-icon-name", "image-missing",
                          //"license-type", GTK_LICENSE_GPL_3_0_ONLY,
                          NULL);
}

//TODO: Support escaping of arguments
char** parse_arguments(char* program, const char* args, size_t* count) {
    *count = 0;
    for (const char* c = args; *c; c++) {
        if (*c!=' ' && *c!='\t') {
            (*count)++;
            while (*c!=' ' && *c!='\t' && *c!=0) c++;
        }
    }
    
    char** program_args = calloc(*count+2, sizeof(const char*));
    program_args[0] = program;
    const char* cur_arg = args;
    for (size_t i = 0; i < *count; i++) {
        const char* c = cur_arg;
        while (*c!=' ' && *c!='\t' && *c!=0) c++;
        size_t len = c - cur_arg;
        program_args[i+1] = calloc(len+1, 1);
        memcpy(program_args[i+1], cur_arg, len);
        cur_arg += len;
        while (*cur_arg==' ' || *cur_arg=='\t') cur_arg++;
    }
    
    return program_args;
}

void new_callback(GObject* obj, gpointer user_data) {
    GtkDialog* dialog = GTK_DIALOG(
    gtk_dialog_new_with_buttons("New Trace",
                                GTK_WINDOW(state.main_window),
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
        
        char* lib_path = realpath("libgl.so", NULL);
        if (!lib_path) {
            display_error_dialog("Unable to find libgl.so\n");
            goto failure;
        }
        
        size_t arg_count;
        char** program_args = parse_arguments(program, args, &arg_count);
        
        int exitcode;
        //TODO: Allow the configuration to be specified in the GUI
        bool success = trace_program(&exitcode, 5,
            TrcProgramArguments, program_args,
            TrcOutputFilename, static_format("%s.trace", program),
            TrcConfigFilename, "configs/this.config.txt",
            TrcCompression, 60,
            TrcLibGL, lib_path);
        if (success && exitcode!=EXIT_SUCCESS) {
            display_error_dialog(
                static_format("Unable to execute command: Process returned %d\n", exitcode));
        } else if (!success) {
            display_error_dialog("Failed to start process\n");
        } else {
            open_trace(static_format("%s.trace", program));
        }
        
        for (size_t i = 1; i < arg_count+1; i++) free(program_args[i]);
        free(program_args);
        free(lib_path);
        
        failure:
        
        g_free(program);
    }
    
    gtk_widget_destroy(prog_button);
    gtk_widget_destroy(GTK_WIDGET(arg_entry));
    gtk_widget_destroy(GTK_WIDGET(box));
    gtk_widget_destroy(GTK_WIDGET(dialog));
}

void open_callback(GObject* obj, gpointer user_data) {
    char* filename = run_file_dialog("Open Trace", "Open", GTK_FILE_CHOOSER_ACTION_OPEN);
    if (!filename) return;
    open_trace(filename);
    g_free(filename);
}

void vertex_data_callback(GObject* obj, gpointer user_data) {
    open_vertex_data_tab();
}

static gboolean tick_callback(GtkWidget* _0, GdkFrameClock* _1, gpointer trace) {
    static size_t countdown = 2;
    if (!--countdown && trace) open_trace((const char*)trace);
    return countdown ? G_SOURCE_CONTINUE : G_SOURCE_REMOVE;
}

int run_gui(const char* trace, int argc, char** argv) {
    memset(&state, 0, sizeof(state));
    state.revision = -1;
    
    gtk_init(&argc, &argv);
    state.gtk_was_init = true;
    
    //Load icons
    state.icon_theme = gtk_icon_theme_new();
    GdkPixbuf** dests[3] = {&state.info_pixbuf, &state.warning_pixbuf, &state.error_pixbuf};
    static const char* names[3] = {"dialog-information", "dialog-warning", "dialog-error"};
    for (size_t i = 0; i < 3; i++) {
        GError* error = NULL;
        *dests[i] = gtk_icon_theme_load_icon(state.icon_theme, names[i], 16, 0, &error);
        if (error) {
            fprintf(stderr, "Unable to load information icon: %s\n", error->message);
            g_error_free(error);
            cleanup();
            return EXIT_FAILURE;
        }
    }
    
    state.builder = gtk_builder_new_from_file("ui.glade");
    
    init_treeview(gtk_builder_get_object(state.builder, "selected_command_attachments"), 1);
    
    //Initialize the command list view
    GtkTreeView* tree = GTK_TREE_VIEW(gtk_builder_get_object(state.builder, "trace_view"));
    GtkTreeStore* store = gtk_tree_store_new(3, GDK_TYPE_PIXBUF, G_TYPE_STRING, G_TYPE_STRING);
    gtk_tree_view_set_model(tree, GTK_TREE_MODEL(store));
    
    GtkCellRenderer* renderer = gtk_cell_renderer_pixbuf_new();
    GtkTreeViewColumn* column = gtk_tree_view_get_column(tree, 0);
    gtk_tree_view_column_pack_start(column, renderer, FALSE);
    gtk_tree_view_column_set_attributes(column, renderer, "pixbuf", 0, NULL);
    
    renderer = gtk_cell_renderer_text_new();
    gtk_cell_renderer_set_alignment(renderer, 1.0, 0.5);
    column = gtk_tree_view_get_column(tree, 1);
    gtk_tree_view_column_pack_start(column, renderer, FALSE);
    gtk_tree_view_column_set_attributes(column, renderer, "text", 1, NULL);
    
    renderer = gtk_cell_renderer_text_new();
    column = gtk_tree_view_get_column(tree, 2);
    gtk_tree_view_column_pack_start(column, renderer, FALSE);
    gtk_tree_view_column_set_attributes(column, renderer, "text", 2, NULL);
    
    //Create main window
    gtk_builder_connect_signals(state.builder, NULL);
    state.main_window = GTK_WIDGET(gtk_builder_get_object(state.builder, "main_window"));
    gtk_widget_show_all(state.main_window);
    
    //It seems best to call open_trace once the gui has started
    //problems seem to occur otherwise (probably because of SDL)
    gtk_widget_add_tick_callback(
        state.main_window, &tick_callback, (gpointer)trace, NULL);
    
    gtk_main();
    
    return EXIT_SUCCESS;
}
