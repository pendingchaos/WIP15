#include "widgets/info_box.h"
#include "tree_store.h"
#include "tabs/tabs.h"
#include "utils.h"
#include "gui.h"

#include <gtk/gtk.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>

gui_state_t state;

static void fill_trace_view() {
    GtkTreeView* tree = GTK_TREE_VIEW(gtk_builder_get_object(state.builder, "trace_view"));
    GuiTreeStore* store = GUI_TREE_STORE(gtk_tree_view_get_model(tree));
    gui_tree_store_set_trace(store, state.trace);
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

VISIBLE void command_select_callback(GtkTreeView* view, gpointer user_data) {
    GtkTreePath* path;
    gtk_tree_view_get_cursor(view, &path, NULL);
    if (!path) return;
    
    GObject* cmd_view = gtk_builder_get_object(state.builder, "selected_command_attachments");
    GtkTreeStore* cmd_store = GTK_TREE_STORE(gtk_tree_view_get_model(GTK_TREE_VIEW(cmd_view)));
    gtk_tree_store_clear(cmd_store);
    
    if (!state.trace) return;
    
    if (gtk_tree_path_get_depth(path) == 2) {
        GtkTreeModel* model = gtk_tree_view_get_model(view);
        
        GtkTreeIter iter;
        gtk_tree_model_get_iter(model, &iter, path);
        
        uint64_t frame_index, cmd_index;
        gtk_tree_model_get(model, &iter, 3, &frame_index, -1);
        gtk_tree_model_get(model, &iter, 4, &cmd_index, -1);
        
        trace_command_t* cmd = &(state.trace->frames[frame_index].commands[cmd_index]);
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

VISIBLE void quit_callback(GObject* obj, gpointer user_data) {
    cleanup();
    gtk_main_quit();
}

VISIBLE void about_callback(GObject* obj, gpointer user_data) {
    gtk_show_about_dialog(GTK_WINDOW(state.main_window),
                          "program-name", "WIP15 Inspector",
                          "version", "0.0.0",
                          "website", "https://gitlab.com/pendingchaos/WIP15",
                          "logo-icon-name", "image-missing",
                          //"license-type", GTK_LICENSE_GPL_3_0_ONLY,
                          NULL);
}

//TODO: Support escaping of arguments
static char** parse_arguments(const char* program, const char* args) {
    size_t count = 0;
    for (const char* c = args; *c; c++) {
        if (*c!=' ' && *c!='\t') {
            count++;
            while (*c!=' ' && *c!='\t' && *c!=0) c++;
        }
    }
    
    char** program_args = calloc(count+2, sizeof(const char*));
    program_args[0] = calloc(strlen(program)+1, 1);
    strcpy(program_args[0], program);
    const char* cur_arg = args;
    for (size_t i = 0; i < count; i++) {
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

static char* get_directory(const char* path_, size_t extra) {
    char* path = canonicalize_file_name(path_);
    char* dir = calloc(strlen(path)+1+extra, 1);
    strcpy(dir, path);
    for (ptrdiff_t i = strlen(dir)-1; i>=0; i--) {
        if (dir[i] != '/') continue;
        dir[i] = 0;
        free(path);
        return dir;
    }
    dir[0] = 0;
    free(path);
    return dir;
}

static char* get_abs_path(const char* path, bool exists, const char* cwd) {
    char* prev = get_current_dir_name();
    if (cwd) chdir(cwd);
    if (!exists) fclose(fopen(path, "a"));
    char* res = canonicalize_file_name(path);
    chdir(prev);
    free(prev);
    return res;
}

static void create_new_trace(const char* args, const char* program, const char* config, int compression, const char* cwd, const char* output) {
    char* lib_path = realpath("libgl.so", NULL);
    if (!lib_path) {
        display_error_dialog("Unable to find libgl.so\n");
        return;
    }
    
    char** program_args = parse_arguments(program, args);
    
    char* program_dir = get_directory(program, 0);
    char* abs_output = output[0] ? get_abs_path(output, false, program_dir) : NULL;
    char* abs_cwd = cwd[0] ? get_abs_path(cwd, true, program_dir) : NULL;
    
    int exitcode;
    bool success = trace_program(&exitcode, 6,
        TrcProgramArguments, program_args,
        TrcOutputFilename, output[0]?abs_output:static_format("%s.trace", program),
        TrcConfigFilename, config ? config : "configs/this.config.txt",
        TrcCompression, compression,
        TrcLibGL, lib_path,
        TrcCurrentWorkingDirectory, cwd[0]?abs_cwd:NULL);
    if (success && exitcode!=EXIT_SUCCESS) {
        display_error_dialog(
            static_format("Unable to execute command: Process returned %d\n", exitcode));
    } else if (!success) {
        display_error_dialog("Failed to start process\n");
    } else {
        open_trace(static_format("%s.trace", program));
    }
    
    for (size_t i = 0; program_args[i]; i++) free(program_args[i]);
    free(program_args);
    free(lib_path);
    free(program_dir);
    free(abs_output);
}

VISIBLE void new_callback(GObject* obj, gpointer user_data) {
    GtkDialog* dialog = GTK_DIALOG(
    gtk_dialog_new_with_buttons("New Trace",
                                GTK_WINDOW(state.main_window),
                                GTK_DIALOG_DESTROY_WITH_PARENT,
                                "OK",
                                GTK_RESPONSE_ACCEPT,
                                "Cancel",
                                GTK_RESPONSE_REJECT,
                                NULL));
    gtk_box_set_spacing(GTK_BOX(gtk_dialog_get_content_area(dialog)), 5);
    
    GtkWidget* prog_button =
        gtk_file_chooser_button_new("Select a Program", GTK_FILE_CHOOSER_ACTION_OPEN);
    GtkWidget* config_button =
        gtk_file_chooser_button_new("Select a Configuration", GTK_FILE_CHOOSER_ACTION_OPEN);
    GtkEntry* output_entry = GTK_ENTRY(gtk_entry_new()); //
    GtkEntry* cwd_entry = GTK_ENTRY(gtk_entry_new()); //
    GtkEntry* arg_entry = GTK_ENTRY(gtk_entry_new());
    GtkRange* compression_range = GTK_RANGE(
        gtk_scale_new_with_range(GTK_ORIENTATION_HORIZONTAL, 0.0, 100.0, 1.0));
    gtk_range_set_value(compression_range, 60.0);
    
    gtk_entry_set_placeholder_text(output_entry, "<unset>");
    gtk_entry_set_placeholder_text(cwd_entry, "<unset>");
    
    info_box_t* info_box = create_info_box();
    
    add_custom_to_info_box(info_box, "Program", prog_button);
    add_custom_to_info_box(info_box, "Arguments", GTK_WIDGET(arg_entry));
    add_custom_to_info_box(info_box, "Configuration", config_button);
    add_custom_to_info_box(info_box, "Output", GTK_WIDGET(output_entry));
    add_custom_to_info_box(info_box, "Working Directory", GTK_WIDGET(cwd_entry));
    add_custom_to_info_box(info_box, "Compression", GTK_WIDGET(compression_range));
    gtk_widget_set_halign(prog_button, GTK_ALIGN_FILL);
    gtk_widget_set_halign(GTK_WIDGET(arg_entry), GTK_ALIGN_FILL);
    gtk_widget_set_halign(GTK_WIDGET(output_entry), GTK_ALIGN_FILL);
    gtk_widget_set_halign(GTK_WIDGET(cwd_entry), GTK_ALIGN_FILL);
    gtk_widget_set_halign(config_button, GTK_ALIGN_FILL);
    gtk_widget_set_halign(GTK_WIDGET(compression_range), GTK_ALIGN_FILL);
    
    gtk_container_add(GTK_CONTAINER(gtk_dialog_get_content_area(dialog)), info_box->widget);
    
    gtk_widget_show_all(GTK_WIDGET(dialog));
    if (gtk_dialog_run(dialog) == GTK_RESPONSE_ACCEPT) {
        const char* args = gtk_entry_get_text(arg_entry);
        const char* cwd = gtk_entry_get_text(cwd_entry);
        const char* output = gtk_entry_get_text(output_entry);
        char* program = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(prog_button));
        char* config = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(config_button));
        
        const char* real_config = config ? config : "configs/this.config.txt";
        int compression = gtk_range_get_value(compression_range);
        create_new_trace(args, program, real_config, compression, cwd, output);
        
        g_free(config);
        g_free(program);
    }
    
    free_info_box(info_box);
    
    gtk_widget_destroy(GTK_WIDGET(compression_range));
    gtk_widget_destroy(GTK_WIDGET(arg_entry));
    gtk_widget_destroy(config_button);
    gtk_widget_destroy(prog_button);
    gtk_widget_destroy(GTK_WIDGET(dialog));
}

VISIBLE void open_callback(GObject* obj, gpointer user_data) {
    char* filename = run_file_dialog("Open Trace", "Open", GTK_FILE_CHOOSER_ACTION_OPEN);
    if (!filename) return;
    open_trace(filename);
    g_free(filename);
}

VISIBLE void vertex_data_callback(GObject* obj, gpointer user_data) {
    open_vertex_data_tab();
}

static bool get_command_for_revision(uint64_t revision, size_t* frame, size_t* cmd) {
    if (!state.trace) return false;
    
    for (size_t frame2 = 0; frame2 < state.trace->frame_count; frame2++) {
        for (size_t cmd2 = 0; cmd2 < state.trace->frames[frame2].command_count; cmd2++) {
            uint64_t cmd_rev = state.trace->frames[frame2].commands[cmd2].revision;
            if (cmd_rev < revision) continue;
            if (cmd_rev > revision) {
                if (frame2==0 && cmd2==0) {
                    return false;
                } else if (cmd2 == 0) {
                    frame2--;
                    cmd2 = state.trace->frames[frame2].command_count - 1;
                } else {
                    cmd2--;
                }
            }
            *frame = frame2;
            *cmd = cmd2;
            return true;
        }
    }
    
    return false;
}

VISIBLE void goto_callback(GObject* obj, gpointer user_data) {
    GtkDialog* dialog = GTK_DIALOG(
    gtk_dialog_new_with_buttons("Goto Revision",
                                GTK_WINDOW(state.main_window),
                                GTK_DIALOG_DESTROY_WITH_PARENT,
                                "OK",
                                GTK_RESPONSE_ACCEPT,
                                "Cancel",
                                GTK_RESPONSE_REJECT,
                                NULL));
    gtk_box_set_spacing(GTK_BOX(gtk_dialog_get_content_area(dialog)), 5);
    
    //any number over 9007199254740992.0 (or 2^53) would have too low precision
    GtkSpinButton* revision_button = GTK_SPIN_BUTTON(
        gtk_spin_button_new_with_range(-1, 9007199254740992.0, 1.0));
    gtk_spin_button_set_value(revision_button, state.revision);
    
    info_box_t* info_box = create_info_box();
    
    add_custom_to_info_box(info_box, "Revision", GTK_WIDGET(revision_button));
    gtk_widget_set_halign(GTK_WIDGET(revision_button), GTK_ALIGN_FILL);
    
    gtk_container_add(GTK_CONTAINER(gtk_dialog_get_content_area(dialog)), info_box->widget);
    
    gtk_widget_show_all(GTK_WIDGET(dialog));
    if (gtk_dialog_run(dialog) == GTK_RESPONSE_ACCEPT) {
        int64_t revision = gtk_spin_button_get_value(revision_button);
        
        GtkTreeView* tree = GTK_TREE_VIEW(gtk_builder_get_object(state.builder, "trace_view"));
        
        size_t frame, cmd;
        if (revision>=0 && get_command_for_revision(revision, &frame, &cmd)) {
            GtkTreePath* path = gtk_tree_path_new_from_indices(frame, cmd, -1);
            GtkTreePath* frame_path = gtk_tree_path_new_from_indices(frame, -1);
            gtk_tree_view_expand_row(tree, frame_path, FALSE);
            gtk_tree_view_set_cursor(tree, path, NULL, FALSE);
            gtk_tree_path_free(frame_path);
            gtk_tree_path_free(path);
        } else {
            gtk_tree_selection_unselect_all(gtk_tree_view_get_selection(tree));
            state.revision = -1;
            state.selected_cmd = NULL;
            update_gui_for_revision();
        }
    }
    
    free_info_box(info_box);
    
    gtk_widget_destroy(GTK_WIDGET(revision_button));
    gtk_widget_destroy(GTK_WIDGET(dialog));
}

static gboolean tick_callback(GtkWidget* _0, GdkFrameClock* _1, gpointer trace) {
    static size_t countdown = 2;
    if (!--countdown && trace) open_trace((const char*)trace);
    return countdown ? G_SOURCE_CONTINUE : G_SOURCE_REMOVE;
}

static void load_css() {
    const char* css = ".object_button {padding: 0px;}";
    
    GdkScreen* screen = gdk_screen_get_default();
    GtkCssProvider* provider = gtk_css_provider_new();
    gtk_css_provider_load_from_data(provider, css, -1, NULL);
    guint priority = GTK_STYLE_PROVIDER_PRIORITY_APPLICATION;
    gtk_style_context_add_provider_for_screen(screen, GTK_STYLE_PROVIDER(provider), priority);
    
    g_object_unref(provider);
}

int run_gui(const char* trace, int argc, char** argv) {
    memset(&state, 0, sizeof(state));
    state.revision = -1;
    
    gtk_init(&argc, &argv);
    state.gtk_was_init = true;
    
    load_css();
    
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
    GuiTreeStore* store = gui_tree_store_new(NULL);
    gui_tree_store_set_error_pixbuf(store, state.error_pixbuf);
    gui_tree_store_set_warning_pixbuf(store, state.warning_pixbuf);
    gui_tree_store_set_info_pixbuf(store, state.info_pixbuf);
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
