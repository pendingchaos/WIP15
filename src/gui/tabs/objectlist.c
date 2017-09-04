#include "libtrace/libtrace.h"
#include "../gui.h"
#include "tabs.h"

#include <gtk/gtk.h>
#include <stdlib.h>

static void update_object_list(object_list_tab_t* tab,
                               bool context_list, bool object_list) {
    //Create object context combo box
    GtkTreeStore* obj_ctx_store = GTK_TREE_STORE(gtk_combo_box_get_model(tab->context_combobox));  
    
    gtk_widget_set_sensitive(GTK_WIDGET(tab->context_combobox), state.revision>=0);    
    gtk_widget_set_sensitive(GTK_WIDGET(tab->objects_treeview), state.revision>=0);
    
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
            gtk_combo_box_set_active(tab->context_combobox, 0);
            
            GtkTreeIter ctx_iter;
            gtk_combo_box_get_active_iter(tab->context_combobox, &ctx_iter);
            gtk_tree_model_get(GTK_TREE_MODEL(obj_ctx_store), &ctx_iter, 1, &cur_context, -1);
        } else {
            gtk_combo_box_set_active(tab->context_combobox, -1);
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
                gtk_combo_box_set_active(tab->context_combobox, item_index);
            item_index++;
        }
    }
    
    //Fill object list
    if (object_list) {
        GtkTreeStore* obj_list_store = GTK_TREE_STORE(gtk_tree_view_get_model(tab->objects_treeview));
        
        const trc_gl_context_rev_t* cur_context;
        if (state.revision >= 0) {
            trc_obj_t* ctx_obj;
            GtkTreeIter ctx_iter;
            gtk_combo_box_get_active_iter(tab->context_combobox, &ctx_iter);
            gtk_tree_model_get(GTK_TREE_MODEL(obj_ctx_store), &ctx_iter, 1, &ctx_obj, -1);
            if (!ctx_obj) //Use current
                ctx_obj = trc_get_current_gl_context(state.trace, state.revision);
            cur_context = trc_obj_get_rev(ctx_obj, state.revision);
        }
        
        for (size_t i = 0; i < (int)Trc_ObjMax; i++) {
            GtkTreePath* parent_path = gtk_tree_model_get_path(GTK_TREE_MODEL(obj_list_store), &tab->object_parents[i]);
            bool expanded = gtk_tree_view_row_expanded(tab->objects_treeview, parent_path);
            
            GtkTreeIter iter;
            while (gtk_tree_model_iter_children(GTK_TREE_MODEL(obj_list_store), &iter, &tab->object_parents[i]))
                gtk_tree_store_remove(obj_list_store, &iter);
            
            const trc_obj_rev_head_t* rev;
            for (size_t j = 0;
                 state.revision>=0 && trc_iter_objects(state.trace, (trc_obj_type_t)i, &j, state.revision, (const void**)&rev);) {
                if (i!=TrcContext && (cur_context==NULL||rev->namespace_!=cur_context->namespace))
                    continue;
                
                char str[64] = {0};
                fmt_object_id(str, 64, rev);
                
                GtkTreeIter row;
                gtk_tree_store_append(obj_list_store, &row, &tab->object_parents[i]);
                gtk_tree_store_set(obj_list_store, &row, 0, str, 1, "", 2, rev->obj, -1);
            }
            
            if (expanded) gtk_tree_view_expand_row(tab->objects_treeview, parent_path, false);            
            else gtk_tree_view_collapse_row(tab->objects_treeview, parent_path);
            
            gtk_tree_path_free(parent_path);
        }
    }
}

static void row_activated(GtkTreeView* tree_view, GtkTreePath* path,
                   GtkTreeViewColumn* column, gui_tab_t* gtab) {
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

static void context_changed(GtkComboBox* widget, object_list_tab_t* tab) {
    if (gtk_combo_box_get_active(widget) == -1) return;
    if (!state.trace) return;
    update_object_list(tab, false, true);
}

static void deinit_objct_list_tab(gui_tab_t* gtab) {
    object_list_tab_t* tab = gtab->data;
    free_info_box(tab->info_box);
}

static void update_object_list_tab(gui_tab_t* tab) {
    update_object_list(tab->data, true, true);
}

gui_tab_t* open_object_list_tab() {
    object_list_tab_t* tab = malloc(sizeof(object_list_tab_t));
    
    //Create combo box
    tab->context_combobox =
        create_combobox(0, (const char*[]){"Current", NULL});
    g_signal_connect(tab->context_combobox, "changed", G_CALLBACK(context_changed), tab);
    
    //Create object treeview
    tab->objects_treeview = GTK_TREE_VIEW(gtk_tree_view_new());
    gtk_widget_set_vexpand(GTK_WIDGET(tab->objects_treeview), true);
    g_signal_connect(tab->objects_treeview, "row-activated", G_CALLBACK(row_activated), tab);
    GtkTreeStore* store = gtk_tree_store_new(3, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_POINTER);
    gtk_tree_view_set_model(tab->objects_treeview, GTK_TREE_MODEL(store));
    
    GtkTreeSelection* sel = gtk_tree_view_get_selection(tab->objects_treeview);
    gtk_tree_selection_set_mode(sel, GTK_SELECTION_MULTIPLE);
    
    GtkCellRenderer* renderer = gtk_cell_renderer_text_new();
    for (size_t i = 0; i < 2; i++) {
        GtkTreeViewColumn* column = gtk_tree_view_column_new();
        gtk_tree_view_column_set_title(column, i?"Label":"Name");
        gtk_tree_view_column_set_resizable(column, true);
        gtk_tree_view_append_column(tab->objects_treeview, column);
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
        
        gtk_tree_store_append(store, &tab->object_parents[i], NULL);
        gtk_tree_store_set(store, &tab->object_parents[i], 0, labels[i], 1, "", 2, NULL, -1);
    }
    
    g_object_unref(store);
    
    gtk_tree_view_set_search_column(tab->objects_treeview, 0);
    
    //Create tab
    tab->info_box = create_info_box();
    add_custom_to_info_box(tab->info_box, "Context", GTK_WIDGET(tab->context_combobox));
    gtk_widget_set_halign(GTK_WIDGET(tab->context_combobox), GTK_ALIGN_FILL);
    add_custom_to_info_box(tab->info_box, NULL, create_scrolled_window(GTK_WIDGET(tab->objects_treeview)));
    
    gui_tab_t* gtab = open_gui_tab(false, tab->info_box->widget);
    gtk_label_set_text(GTK_LABEL(gtab->tab_label), "Objects");
    
    tab->tab = gtab;
    gtab->data = tab;
    gtab->deinit = &deinit_objct_list_tab;
    gtab->update = &update_object_list_tab;
    
    update_tab(gtab);
    
    return gtab;
}
