#include "utils.h"
#include "gui.h"

#include <string.h>
#include <stdlib.h>
#include <stdio.h>

object_tab_callback_t object_tab_init_callbacks[(int)Trc_ObjMax] = {0};
object_tab_callback_t object_tab_deinit_callbacks[(int)Trc_ObjMax] = {0};
object_tab_update_callback_t object_tab_update_callbacks[(int)Trc_ObjMax] = {0};
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

static void update_object_list(bool context_list, bool object_list) {
    //Create object context combo box
    GtkComboBox* obj_ctx_combobox = GTK_COMBO_BOX(gtk_builder_get_object(state.builder, "object_context_combo_box"));    
    GtkTreeStore* obj_ctx_store = GTK_TREE_STORE(gtk_combo_box_get_model(obj_ctx_combobox));
    
    GtkTreeView* obj_list = GTK_TREE_VIEW(gtk_builder_get_object(state.builder, "objects_treeview"));    
    
    gtk_widget_set_sensitive(GTK_WIDGET(obj_ctx_combobox), state.revision>=0);    
    gtk_widget_set_sensitive(GTK_WIDGET(obj_list), state.revision>=0);
    
    if (context_list) {
        gtk_tree_store_clear(obj_ctx_store);
        GtkTreeIter row;
        
        trc_obj_t* cur_context = NULL;
        if (state.revision >= 0) {
            trc_obj_t* ctx = trc_get_current_gl_context(state.trace, state.revision);
            const char* cur_name =
                static_format("Current (%s)", ctx?static_format_obj(ctx, state.revision):"None");
            
            gtk_tree_store_append(obj_ctx_store, &row, NULL);
            gtk_tree_store_set(obj_ctx_store, &row, 0, cur_name, 1, NULL, -1);
            gtk_combo_box_set_active(obj_ctx_combobox, 0);
            
            GtkTreeIter ctx_iter;
            gtk_combo_box_get_active_iter(obj_ctx_combobox, &ctx_iter);
            gtk_tree_model_get(GTK_TREE_MODEL(obj_ctx_store), &ctx_iter, 1, &cur_context, -1);
        } else {
            gtk_combo_box_set_active(obj_ctx_combobox, -1);
        }
        
        const trc_obj_rev_head_t* rev;
        size_t item_index = 1;
        for (size_t i = 0; state.revision>=0 && trc_iter_objects(state.trace, TrcContext, &i, state.revision, (const void**)&rev);) {
            if (!rev->ref_count) continue;
            
            char str[64];
            fmt_object_id(str, 64, rev);
            
            gtk_tree_store_append(obj_ctx_store, &row, NULL);
            gtk_tree_store_set(obj_ctx_store, &row, 0, str, 1, rev->obj, -1);
            
            if (rev->obj == cur_context)
                gtk_combo_box_set_active(obj_ctx_combobox, item_index);
            item_index++;
        }
    }
    
    //Fill object list
    if (object_list) {
        GtkTreeStore* obj_list_store = GTK_TREE_STORE(gtk_tree_view_get_model(obj_list));
        
        const trc_gl_context_rev_t* cur_context;
        if (state.revision >= 0) {
            trc_obj_t* ctx_obj;
            GtkTreeIter ctx_iter;
            gtk_combo_box_get_active_iter(obj_ctx_combobox, &ctx_iter);
            gtk_tree_model_get(GTK_TREE_MODEL(obj_ctx_store), &ctx_iter, 1, &ctx_obj, -1);
            if (!ctx_obj) //Use current
                ctx_obj = trc_get_current_gl_context(state.trace, state.revision);
            cur_context = trc_obj_get_rev(ctx_obj, state.revision);
        }
        
        for (size_t i = 0; i < (int)Trc_ObjMax; i++) {
            GtkTreePath* parent_path = gtk_tree_model_get_path(GTK_TREE_MODEL(obj_list_store), &state.object_parents[i]);
            bool expanded = gtk_tree_view_row_expanded(obj_list, parent_path);
            
            GtkTreeIter iter;
            while (gtk_tree_model_iter_children(GTK_TREE_MODEL(obj_list_store), &iter, &state.object_parents[i]))
                gtk_tree_store_remove(obj_list_store, &iter);
            
            const trc_obj_rev_head_t* rev;
            for (size_t j = 0;
                 state.revision>=0 && trc_iter_objects(state.trace, (trc_obj_type_t)i, &j, state.revision, (const void**)&rev);) {
                if (i!=TrcContext && (cur_context==NULL||rev->namespace_!=cur_context->namespace))
                    continue;
                
                char str[64] = {0};
                fmt_object_id(str, 64, rev);
                
                GtkTreeIter row;
                gtk_tree_store_append(obj_list_store, &row, &state.object_parents[i]);
                gtk_tree_store_set(obj_list_store, &row, 0, str, 1, "", 2, rev->obj, -1);
            }
            
            if (expanded) gtk_tree_view_expand_row(obj_list, parent_path, false);            
            else gtk_tree_view_collapse_row(obj_list, parent_path);
            
            gtk_tree_path_free(parent_path);
        }
    }
}

static bool parse_uint(const char* text, int64_t* val) {
    *val = 0;
    if (*text == 0) return false;
    for (const char* c = text; *c; c++) {
        if (*c>='0' && *c<='9')
            *val = (*val)*10 + *c-'0';
        else
            return false;
    }
    return true;
}

void update_object_tab(object_tab_t* tab) {
    static const char* names[] = {
        [TrcBuffer] = "Buffer ",
        [TrcSampler] = "Sampler ",
        [TrcTexture] = "Texture ",
        [TrcQuery] = "Query ",
        [TrcFramebuffer] = "Framebuffer ",
        [TrcRenderbuffer] = "Renderbuffer ",
        [TrcSync] = "Sync ",
        [TrcProgram] = "Program ",
        [TrcProgramPipeline] = "Program Pipeline ",
        [TrcShader] = "Shader ",
        [TrcVAO] = "VAO ",
        [TrcTransformFeedback] = "Transform Feedback ",
        [TrcContext] = "Context "
    };
    
    const char* problem_text = NULL;
    
    int64_t revision = state.revision;
    if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(tab->revision_checkbox))) {
        const char* text = gtk_entry_get_text(GTK_ENTRY(tab->revision_entry));
        if (!parse_uint(text, &revision)) {
            problem_text = static_format("Invalid revision");
            revision = -1;
        }
    } else {
        char rev_text[64];
        sprintf(rev_text, "%ld", state.revision);
        gtk_entry_set_text(GTK_ENTRY(tab->revision_entry), rev_text);
    }
    
    const trc_obj_rev_head_t* rev = NULL;
    if (revision >= 0) rev = trc_obj_get_rev(tab->obj, revision);
    
    if (!rev && !problem_text) {
        problem_text = static_format(
            "This object was only created at revision %lu",
            ((trc_obj_rev_head_t*)tab->obj->revisions[0])->revision);
    }
    if (rev && rev->ref_count==0)
        problem_text = static_format("This object has been deleted at revision %lu", rev->revision);
    
    if (problem_text)
        gtk_label_set_text(GTK_LABEL(tab->problem_label), problem_text);
    
    gtk_widget_set_visible(tab->problem_label, problem_text!=NULL);
    gtk_widget_set_visible(tab->obj_notebook->widget, problem_text==NULL);
    
    const char* text = static_format("%s %s", names[tab->type], static_format_obj(tab->obj, revision));
    gtk_label_set_text(GTK_LABEL(tab->tab_label), text);
    
    object_tab_update_callback_t cb = object_tab_update_callbacks[(int)tab->type];
    if (rev && cb) cb(tab, rev, revision);
}

static void delete_object_tab(object_tab_t* tab) {
    if (tab->prev) tab->prev->next = tab->next;
    else state.object_tabs = tab->next;
    if (tab->next) tab->next->prev = tab->prev;
    
    object_tab_callback_t cb = object_tab_deinit_callbacks[(int)tab->type];
    if (cb) cb(tab);
    destroy_object_notebook(tab->obj_notebook);
    free_info_box(tab->info_box);
    gtk_container_remove(GTK_CONTAINER(tab->notebook), tab->tab_child);
    
    free(tab);
}

static void revision_checkbox_callback(GtkToggleButton* togglebutton, object_tab_t* tab) {
    bool sensitive = gtk_toggle_button_get_active(togglebutton);
    gtk_widget_set_sensitive(tab->revision_entry, sensitive);
    update_object_tab(tab);
}

static void revision_entry_callback(GtkEntry* entry, object_tab_t* tab) {
    update_object_tab(tab);
}

static void tab_close_callback(GtkButton* button, object_tab_t* tab) {
    delete_object_tab(tab);
}

static void open_object_tab(trc_obj_t* obj) {
    object_tab_t* tab = malloc(sizeof(object_tab_t));
    tab->prev = NULL;
    tab->next = state.object_tabs;
    if (tab->next) tab->next->prev = tab;
    state.object_tabs = tab;
    tab->obj = obj;
    tab->type = obj->type;
    tab->data = NULL;
    
    tab->obj_notebook = create_object_notebook();
    tab->info_box = create_info_box();
    add_object_notebook_tab(tab->obj_notebook, "Main", false, tab->info_box->widget, NULL, NULL);
    
    object_tab_callback_t cb = object_tab_init_callbacks[(int)tab->type];
    if (cb) {
        cb(tab);
    } else {
        GtkWidget* widget = gtk_image_new_from_icon_name("broken", GTK_ICON_SIZE_DIALOG);
        add_custom_to_info_box(tab->info_box, NULL, widget);
    }
    
    tab->revision_checkbox = gtk_check_button_new_with_label("Custom Revision");
    tab->revision_entry = gtk_entry_new();
    gtk_widget_set_sensitive(tab->revision_entry, false);
    gtk_entry_set_input_purpose(GTK_ENTRY(tab->revision_entry), GTK_INPUT_PURPOSE_DIGITS);
    
    g_signal_connect(tab->revision_checkbox, "toggled", G_CALLBACK(revision_checkbox_callback), tab);
    g_signal_connect(tab->revision_entry, "activate", G_CALLBACK(revision_entry_callback), tab);
    
    tab->revision_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
    gtk_box_pack_start(GTK_BOX(tab->revision_box), tab->revision_checkbox, false, false, 0);
    gtk_box_pack_start(GTK_BOX(tab->revision_box), tab->revision_entry, true, true, 0);
    
    tab->problem_label = gtk_label_new("");
    gtk_widget_set_visible(tab->problem_label, false);
    gtk_widget_set_visible(tab->obj_notebook->widget, false);
    
    tab->tab_child = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    gtk_box_pack_start(GTK_BOX(tab->tab_child), tab->revision_box, false, false, 0);
    gtk_box_pack_start(GTK_BOX(tab->tab_child), tab->problem_label, true, false, 0);
    GtkWidget* notebook_window = create_scrolled_window(tab->obj_notebook->widget);
    gtk_box_pack_start(GTK_BOX(tab->tab_child), notebook_window, true, true, 0);
    
    tab->tab_label = gtk_label_new("");
    tab->tab_label_button = gtk_button_new_from_icon_name("window-close", GTK_ICON_SIZE_MENU);
    g_signal_connect(tab->tab_label_button, "clicked", G_CALLBACK(tab_close_callback), tab);
    gtk_button_set_relief(GTK_BUTTON(tab->tab_label_button), GTK_RELIEF_NONE);
    tab->tab_label_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
    gtk_box_pack_start(GTK_BOX(tab->tab_label_box), tab->tab_label, true, true, 0);
    gtk_box_pack_start(GTK_BOX(tab->tab_label_box), tab->tab_label_button, false, false, 0);
    
    tab->notebook = GTK_NOTEBOOK(gtk_builder_get_object(state.builder, "notebook"));
    gtk_notebook_append_page(tab->notebook, tab->tab_child, tab->tab_label_box);
    gtk_notebook_set_tab_reorderable(tab->notebook, tab->tab_child, true);
    gtk_widget_show_all(tab->tab_label_box);
    gtk_widget_show_all(tab->tab_child);
    
    update_object_tab(tab);
}

void objects_treeview_row_activated_callback(
    GtkTreeView* tree_view, GtkTreePath* path, GtkTreeViewColumn* column, gpointer user_data) {
    GtkTreeSelection* sel = gtk_tree_view_get_selection(tree_view);
    GList* selected = gtk_tree_selection_get_selected_rows(sel, NULL);
    
    for (; selected; selected = selected->next) {
        GtkTreeIter iter;
        GtkTreeModel* model = gtk_tree_view_get_model(tree_view);
        gtk_tree_model_get_iter(model, &iter, selected->data);
        trc_obj_t* obj;
        gtk_tree_model_get(model, &iter, 2, &obj, -1);
        if (obj) open_object_tab(obj);
    }
    
    g_list_free_full(selected, (GDestroyNotify)gtk_tree_path_free);
}

static void update_gui_for_revision() {
    update_object_list(true, true);
    if (!state.trace) {
        for (object_tab_t* tab = state.object_tabs; tab;) {
            object_tab_t* next = tab->next;
            delete_object_tab(tab);
            tab = next;
        }
        state.object_tabs = NULL;
    } else {
        for (object_tab_t* tab = state.object_tabs; tab; tab = tab->next)
            update_object_tab(tab);
    }
}

void object_context_changed_callback(GtkComboBox* widget, gpointer user_data) {
    if (gtk_combo_box_get_active(widget) == -1) return;
    if (!state.trace) return;
    update_object_list(false, true);
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
        update_gui_for_revision();
    }
}

static void free_open_trace() {
    trace_t* trace = state.trace;
    state.trace = NULL;
    state.revision = -1;
    
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
        bool success = trace_program(&exitcode, 5,
            TrcProgramArguments, program_args,
            TrcOutputFilename, static_format("%s.trace", program),
            TrcLimitsFilename, "limits/this.limits.txt",
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

int run_gui(const char* trace, int argc, char** argv) {
    memset(&state, 0, sizeof(state));
    state.revision = -1;
    
    if (trace) open_trace(trace);
    
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
    
    init_combobox(gtk_builder_get_object(state.builder, "object_context_combo_box"),
                  (const char*[]){"Current", NULL});
    
    init_treeview(gtk_builder_get_object(state.builder, "selected_command_attachments"), 1);
    
    //Initialize the object list view
    GObject* obj_treeview = gtk_builder_get_object(state.builder, "objects_treeview");
    GtkTreeStore* store = gtk_tree_store_new(3, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_POINTER);
    gtk_tree_view_set_model(GTK_TREE_VIEW(obj_treeview),
                            GTK_TREE_MODEL(store));
    
    GtkTreeSelection* sel = gtk_tree_view_get_selection(GTK_TREE_VIEW(obj_treeview));
    gtk_tree_selection_set_mode(sel, GTK_SELECTION_MULTIPLE);
    
    GtkCellRenderer* renderer = gtk_cell_renderer_text_new();
    for (size_t i = 0; i < 2; i++) {
        GtkTreeViewColumn* column = gtk_tree_view_get_column(GTK_TREE_VIEW(obj_treeview), i);
        gtk_tree_view_column_pack_start(column, renderer, FALSE);
        gtk_tree_view_column_set_attributes(column, renderer, "text", i, NULL);
    }
    
    for (size_t i = 0; i < (int)Trc_ObjMax; i++) {
        static const char* labels[] = {
            [TrcBuffer] = "Buffers",
            [TrcSampler] = "Samplers",
            [TrcTexture] = "Textures",
            [TrcQuery] = "Queries",
            [TrcFramebuffer] = "Framebuffers",
            [TrcRenderbuffer] = "Renderbuffers",
            [TrcSync] = "Syncs",
            [TrcProgram] = "Programs",
            [TrcProgramPipeline] = "Program Pipelines",
            [TrcShader] = "Shaders",
            [TrcVAO] = "VAOs",
            [TrcTransformFeedback] = "Transform Feedbacks",
            [TrcContext] = "Contexts"
        };
        
        gtk_tree_store_append(store, &state.object_parents[i], NULL);
        gtk_tree_store_set(store, &state.object_parents[i], 0, labels[i], 1, "", 2, NULL, -1);
    }
    
    g_object_unref(store);
    
    //Initialize the command list view
    GtkTreeView* tree = GTK_TREE_VIEW(gtk_builder_get_object(state.builder, "trace_view"));
    store = gtk_tree_store_new(3, GDK_TYPE_PIXBUF, G_TYPE_STRING, G_TYPE_STRING);
    gtk_tree_view_set_model(tree, GTK_TREE_MODEL(store));
    
    renderer = gtk_cell_renderer_pixbuf_new();
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
    fill_trace_view();
    update_gui_for_revision();
    gtk_widget_show_all(state.main_window);
    
    gtk_main();
    
    return EXIT_SUCCESS;
}
