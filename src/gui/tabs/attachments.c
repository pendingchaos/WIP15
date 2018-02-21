#include "attachments.h"
#include "../tree_store.h"

#include <inttypes.h>

static void row_activated(GtkTreeView* tree_view, GtkTreePath* path,
                          GtkTreeViewColumn* column, gui_tab_t* gtab) {
    GtkTreeModel* model = gtk_tree_view_get_model(tree_view);
    
    GtkTreeIter iter;
    gtk_tree_model_get_iter(model, &iter, path);
    
    uint64_t revision;
    gtk_tree_model_get(model, &iter, 2, &revision, -1);
    goto_revision(revision);
}

static void update_attachments(attachments_tab_t* tab) {
    for (size_t i = 0; i < 3; i++) tab->filter[i] = false;
    switch (gtk_combo_box_get_active(tab->filter_combobox)) {
    case 0: tab->filter[0] = tab->filter[1] = tab->filter[2] = true; break;
    case 1: tab->filter[0] = true; break;
    case 2: tab->filter[0] = tab->filter[1] = true; break;
    case 3: tab->filter[1] = true; break;
    case 4: tab->filter[1] = tab->filter[2] = true; break;
    case 5: tab->filter[2] = true; break;
    }
    
    GtkTreeStore* store = GTK_TREE_STORE(gtk_tree_view_get_model(tab->treeview));
    gtk_tree_store_clear(store);
    
    for (size_t frame = 0; frame < state.trace->frame_count; frame++) {
        for (size_t cmd = 0; cmd < state.trace->frames[frame].command_count; cmd++) {
            trace_command_t* cur_cmd = &state.trace->frames[frame].commands[cmd];
            
            trc_attachment_t* attachment = cur_cmd->attachments;
            while (attachment) {
                if (attachment->type==TrcAttachType_Error && !tab->filter[0])
                    goto cont;
                else if (attachment->type==TrcAttachType_Warning && !tab->filter[1])
                    goto cont;
                else if (attachment->type==TrcAttachType_Info && !tab->filter[2])
                    goto cont;
                
                GdkPixbuf* pixbuf = NULL;
                switch (attachment->type) {
                case TrcAttachType_Error: pixbuf = state.error_pixbuf; break;
                case TrcAttachType_Warning: pixbuf = state.warning_pixbuf; break;
                case TrcAttachType_Info: pixbuf = state.info_pixbuf; break;
                }
                
                char text[1024];
                snprintf(text, sizeof(text), "%"PRIu64"(%s): %s",
                         cur_cmd->revision, state.trace->func_names[cur_cmd->func_index],
                         attachment->message);
                
                gtk_tree_store_insert_with_values(
                    store, NULL, NULL, -1, 0, pixbuf, 1, text, 2, cur_cmd->revision, -1);
                
                cont: attachment = attachment->next;
            }
        }
    }
}

static void filter_changed(GtkComboBox* widget, attachments_tab_t* tab) {
    if (gtk_combo_box_get_active(widget) == -1) return;
    if (!state.trace) return;
    update_attachments(tab);
}

static void deinit_attachments_tab(gui_tab_t* gtab) {
    attachments_tab_t* tab = gtab->data;
    free_info_box(tab->info_box);
}

static void update_attachments_tab(gui_tab_t* gtab) {
    update_attachments(gtab->data);
}

gui_tab_t* open_attachments_tab() {
    attachments_tab_t* tab = malloc(sizeof(attachments_tab_t));
    
    //Create combo box
    tab->filter_combobox =
        create_combobox(2, (const char*[]){"All", "Errors", "Errors and Warnings", "Warnings", "Warnings and Info", "Info", "None", NULL});
    g_signal_connect(tab->filter_combobox, "changed", G_CALLBACK(filter_changed), tab);
    
    //Create tree store
    GtkTreeStore* store = gtk_tree_store_new(3, GDK_TYPE_PIXBUF, G_TYPE_STRING, G_TYPE_UINT64);
    
    //Create tree view
    tab->treeview = GTK_TREE_VIEW(gtk_tree_view_new());
    gtk_tree_view_set_headers_visible(tab->treeview, false);
    gtk_tree_view_set_search_column(tab->treeview, 1);
    gtk_widget_set_vexpand(GTK_WIDGET(tab->treeview), true);
    g_signal_connect(tab->treeview, "row-activated", G_CALLBACK(row_activated), tab);
    gtk_tree_view_set_model(tab->treeview, GTK_TREE_MODEL(store));
    
    GtkCellRenderer* renderer = gtk_cell_renderer_pixbuf_new();
    GtkTreeViewColumn* column = gtk_tree_view_column_new();
    gtk_tree_view_append_column(tab->treeview, column);
    gtk_tree_view_column_pack_start(column, renderer, FALSE);
    gtk_tree_view_column_set_attributes(column, renderer, "pixbuf", 0, NULL);
    
    renderer = gtk_cell_renderer_text_new();
    column = gtk_tree_view_column_new();
    gtk_tree_view_append_column(tab->treeview, column);
    gtk_tree_view_column_pack_start(column, renderer, FALSE);
    gtk_tree_view_column_set_attributes(column, renderer, "text", 1, NULL);
    
    g_object_unref(store);
    
    //Create tab
    tab->info_box = create_info_box();
    add_custom_to_info_box(tab->info_box, "Filter", GTK_WIDGET(tab->filter_combobox));
    gtk_widget_set_halign(GTK_WIDGET(tab->filter_combobox), GTK_ALIGN_FILL);
    add_custom_to_info_box(tab->info_box, NULL, create_scrolled_window(GTK_WIDGET(tab->treeview)));
    
    gui_tab_t* gtab = open_gui_tab(false, tab->info_box->widget);
    gtk_label_set_text(GTK_LABEL(gtab->tab_label), "Attachments");
    
    tab->tab = gtab;
    gtab->data = tab;
    gtab->deinit = &deinit_attachments_tab;
    gtab->update = &update_attachments_tab;
    
    update_tab(gtab);
    
    return gtab;
}
