#include "libtrace/libtrace.h"
#include "../gui.h"
#include "vertexdata.h"
#include "vertexdata_utils.h"
#include "shared/glcorearb.h"

#include <gtk/gtk.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>

#define MODE_VERTICES 0
#define MODE_PRIMITIVES 1

static void deinit_vertex_data_tab(gui_tab_t* gtab) {
    vertex_data_tab_t* tab = gtab->data;
    free(tab->attrib_columns);
    free(tab->column_attribs);
    free_info_box(tab->info_box);
}

static void create_column_info(vertex_data_tab_t* tab, const trc_gl_vao_rev_t* vao) {
    bool index = gtk_combo_box_get_active(tab->mode) == MODE_PRIMITIVES;
    tab->attrib_column_start = index ? 1 : 0;
    
    size_t attrib_count = vao->attribs->size / sizeof(trc_gl_vao_attrib_t);
    const trc_gl_vao_attrib_t* attribs = trc_map_data(vao->attribs, TRC_MAP_READ);
    
    tab->column_count = index ? 1 : 0;
    free(tab->attrib_columns);
    free(tab->column_attribs);
    tab->attrib_columns = malloc(sizeof(int)*attrib_count);
    tab->column_attribs = malloc(sizeof(int)*attrib_count);
    for (size_t i = 0; i < attrib_count; i++) tab->attrib_columns[i] = -1;
    for (size_t i = 0; i < attrib_count; i++) tab->column_attribs[i] = -1;
    for (size_t i = 0; i < attrib_count; i++) {
        if (attribs[i].enabled) {
            tab->attrib_columns[i] = tab->column_count++;
            tab->column_attribs[tab->attrib_columns[i]] = i;
        }
    }
    
    trc_unmap_data(attribs);
}

static trc_obj_t* get_vertex_program(const trc_gl_context_rev_t* ctx) {
    if (ctx->bound_program.obj) return ctx->bound_program.obj;
    trc_obj_t* pipeline = ctx->bound_pipeline.obj;
    if (!pipeline) return NULL;
    const trc_gl_program_pipeline_rev_t* rev = trc_obj_get_rev(pipeline, -1); 
    return rev->vertex_program.obj;
}

const char* format_column_title(int attrib, size_t attrib_count,
                                const trc_gl_program_vertex_attrib_t* attribs) {
    size_t index = 0;
    int rel_loc;
    for (; index < attrib_count; index++) {
        rel_loc = attribs[index].fake - attrib;
        if (rel_loc>=0 && rel_loc<attribs[index].locations_used)
            goto found;
    }
    
    return static_format("%d", attrib);
    found: ;
    const char* res;
    const char* name = trc_map_data(attribs[index].name, TRC_MAP_READ);
    if (rel_loc != 0)
        res = static_format("%s+%d(%d)", name, rel_loc, attrib);
    else
        res = static_format("%s(%d)", name, attrib);
    trc_unmap_data(name);
    return res;
}

static void create_treeview(vertex_data_tab_t* tab, const trc_gl_vao_rev_t* vao,
                            const trc_gl_context_rev_t* ctx) {
    create_column_info(tab, vao);
    
    bool index = gtk_combo_box_get_active(tab->mode) == MODE_PRIMITIVES;
    
    tab->data_view = GTK_TREE_VIEW(gtk_tree_view_new());
    gtk_widget_set_vexpand(GTK_WIDGET(tab->data_view), true);
    
    //include one extra column to ensure there is always at least one
    GType types[tab->column_count+1];
    for (size_t i = 0; i < tab->column_count+1; i++) types[i] = G_TYPE_STRING;
    GtkTreeStore* store = gtk_tree_store_newv(tab->column_count+1, types);
    
    gtk_tree_view_set_model(tab->data_view, GTK_TREE_MODEL(store));
    
    trc_obj_t* program_obj = get_vertex_program(ctx);
    const trc_gl_program_rev_t* program_rev = NULL;
    size_t attrib_count = 0;
    const trc_gl_program_vertex_attrib_t* attribs = NULL;
    if (program_obj) {
        program_rev = trc_obj_get_rev(program_obj, state.revision);
        attrib_count = program_rev->vertex_attribs->size;
        attrib_count /= sizeof(trc_gl_program_vertex_attrib_t);
        attribs = trc_map_data(program_rev->vertex_attribs, TRC_MAP_READ);
    }
    
    GtkCellRenderer* renderer = gtk_cell_renderer_text_new();
    for (size_t i = 0; i < tab->column_count; i++) {
        GtkTreeViewColumn* column = gtk_tree_view_column_new();
        const char* title = "Index";
        if (!index || i!=0) {
            int attrib = tab->column_attribs[i];
            title = format_column_title(attrib, attrib_count, attribs);
        }
        gtk_tree_view_column_set_title(column, title);
        gtk_tree_view_column_set_resizable(column, true);
        gtk_tree_view_append_column(tab->data_view, column);
        gtk_tree_view_column_pack_start(column, renderer, FALSE);
        gtk_tree_view_column_set_attributes(column, renderer, "text", i, NULL);
    }
    
    if (program_rev) trc_unmap_data(attribs);
}

static void fill_in_data_view(vertex_data_tab_t* tab,
                              const trc_gl_vao_rev_t* vao,
                              const trc_gl_context_rev_t* ctx) {
    int mode = gtk_combo_box_get_active(tab->mode);
    uint64_t start = gtk_spin_button_get_value(tab->start);
    uint64_t count = gtk_spin_button_get_value(tab->count);
    uint64_t instance = gtk_spin_button_get_value(tab->instance);
    int64_t base_instance = gtk_spin_button_get_value(tab->base_instance);
    
    GtkTreeStore* store = GTK_TREE_STORE(gtk_tree_view_get_model(tab->data_view));
    
    //Initialize each row
    for (size_t i = 0; i < count; i++) {
        GtkTreeIter row;
        gtk_tree_store_append(store, &row, NULL);
    }
    
    size_t attrib_count = vao->attribs->size / sizeof(trc_gl_vao_attrib_t);
    const trc_gl_vao_attrib_t* attribs = trc_map_data(vao->attribs, TRC_MAP_READ);
    const trc_gl_vao_buffer_t* buffers = trc_map_data(vao->buffers, TRC_MAP_READ);
    
    //Primitive-mode setup
    index_list_state_t il_state;
    int64_t base_vertex = gtk_spin_button_get_value(tab->base_vertex);
    if (mode == MODE_PRIMITIVES) {
        int index_type = gtk_combo_box_get_active(tab->index_type);
        GLenum type = (GLenum[]){
            GL_UNSIGNED_BYTE, GL_UNSIGNED_SHORT, GL_UNSIGNED_INT}[index_type];
        uint64_t offset = gtk_spin_button_get_value(tab->index_offset);
        begin_index_list(&il_state, type, offset, vao, ctx);
    }
    
    //Fill in each column
    for (size_t i = 0; i < attrib_count; i++) {
        const trc_gl_vao_attrib_t* attrib = &attribs[i];
        if (!attrib->enabled) continue;
        
        GtkTreeIter iter;
        gtk_tree_model_get_iter_first(GTK_TREE_MODEL(store), &iter);
        
        attrib_list_state_t al_state;
        begin_attrib_list(&al_state, i, ctx, *attrib, buffers[attrib->buffer_index]);
        int column = tab->attrib_columns[i];
        for (size_t j = 0; j < count; j++) {
            const char* value;
            if (mode == 0) {
                value = get_attrib(&al_state, base_instance+instance, start+j);
            } else {
                int64_t index = get_index(&il_state, j) + base_vertex;
                if (index < 0) break;
                value = get_attrib(&al_state, base_instance+instance, index);
            }
            if (value)
                gtk_tree_store_set(store, &iter, column, value, -1);
            gtk_tree_model_iter_next(GTK_TREE_MODEL(store), &iter);
        }
        end_attrib_list(&al_state);
    }
    
    //Primitive-mode teardown
    if (mode == MODE_PRIMITIVES) {
        GtkTreeIter iter;
        gtk_tree_model_get_iter_first(GTK_TREE_MODEL(store), &iter);
        for (size_t j = 0; j < count; j++) {
            int64_t index = get_index(&il_state, j);
            if (index < 0) break;
            const char* index_str = static_format("%"PRIi64, index);
            gtk_tree_store_set(store, &iter, 0, index_str, -1);
            gtk_tree_model_iter_next(GTK_TREE_MODEL(store), &iter);
        }
        
        end_index_list(&il_state);
    }
    
    trc_unmap_data(buffers);
    trc_unmap_data(attribs);
}

typedef struct draw_call_info_t {
    const char* name;
    bool indexed;
    int start;
    int count;
    int index_offset;
    int index_type;
    int base_vertex;
    int base_instance;
    bool instanced;
} draw_call_info_t;

static draw_call_info_t draw_call_infos[] = {
    (draw_call_info_t){"glDrawArrays", false, 1, 2, -1, -1, -1, -1, false},
    //TODO: glDrawArraysIndirect
    (draw_call_info_t){"glDrawArraysInstanced", false, 1, 2, -1, -1, -1, -1, true},
    (draw_call_info_t){"glDrawArraysInstancedBaseInstance", false, 1, 2, -1, -1, -1, 4, true},
    (draw_call_info_t){"glDrawElements", true, -1, 1, 3, 2, -1, -1, false},
    (draw_call_info_t){"glDrawElementsBaseVertex", true, -1, 1, 3, 2, 4, -1, false},
    //TODO: glDrawElementsIndirect
    (draw_call_info_t){"glDrawElementsInstanced", true, -1, 1, 3, 2, -1, -1, true},
    (draw_call_info_t){"glDrawElementsInstancedBaseInstance", true, -1, 1, 3, 2, -1, 5, true},
    (draw_call_info_t){"glDrawElementsInstancedBaseVertex", true, -1, 1, 3, 2, 5, -1, true},
    (draw_call_info_t){"glDrawElementsInstancedBaseVertexBaseInstance", true, -1, 1, 3, 2, 5, 6, true}
    //TODO: glDrawRangeElements
    //TODO: glDrawRangeElementsBaseVertex
    //TODO: glMultiDrawArrays
    //TODO: glMultiDrawArraysIndirect
    //TODO: glMultiDrawElements
    //TODO: glMultiDrawElementsBaseVertex
    //TODO: glMultiDrawElementsIndirect
    //TODO: glMultiDrawArraysIndirectCount (GL 4.6)
    //TODO: glMultiDrawElementsIndirectCount (GL 4.6)
};

static int64_t get_arg(trace_command_t* cmd, int index, int64_t default_, bool* success) {
    if (index < 0) return default_;
    
    trace_value_t* val = &cmd->args[index];
    switch (val->type) {
    case Type_UInt:
    case Type_Ptr: {
        uint64_t v = val->type==Type_Ptr ? trc_get_ptr(val)[0] : trc_get_uint(val)[0];
        *success = v <= (uint64_t)INT64_MAX;
        return v;
    }
    case Type_Int: {
        return trc_get_int(val)[0];
    }
    default: {
        *success = false;
        return -1;
    }
    }
}

typedef struct autofill_data_t {
    int64_t mode, start, count, base_instance, base_vertex, index_offset, index_type;
} autofill_data_t;

static bool fill_autofill_data(autofill_data_t* data, trace_command_t* cmd) {
    const char* name = state.trace->func_names[cmd->func_index];
    draw_call_info_t* info = NULL;
    for (size_t i = 0; i < sizeof(draw_call_infos)/sizeof(draw_call_info_t); i++) {
        if (strcmp(name, draw_call_infos[i].name) == 0)
            info = &draw_call_infos[i];
    }
    if (!info) return false;
    
    data->mode = info->indexed ? MODE_PRIMITIVES : MODE_VERTICES;
    bool success = true;
    data->start = get_arg(cmd, info->start, 0, &success);
    data->count = get_arg(cmd, info->count, 0, &success);
    data->base_instance = get_arg(cmd, info->base_instance, 0, &success);
    data->base_vertex = get_arg(cmd, info->base_vertex, 0, &success);
    data->index_offset = get_arg(cmd, info->index_offset, 0, &success);
    data->index_type = get_arg(cmd, info->index_type, GL_UNSIGNED_BYTE, &success);
    return success;
}

static bool autofill(vertex_data_tab_t* tab) {
    autofill_data_t data;
    data.start = 0;
    data.count = 0;
    data.base_instance = 0;
    data.base_vertex = 0;
    data.index_offset = 0;
    data.index_type = GL_UNSIGNED_BYTE;
    
    if (!fill_autofill_data(&data, state.selected_cmd)) goto failure;
    
    if (data.start<0 || data.count<0 || data.base_vertex<0) goto failure;
    if (data.base_instance<0 || data.index_offset<0) goto failure;
    if (data.index_type==GL_UNSIGNED_BYTE) ;
    else if (data.index_type==GL_UNSIGNED_SHORT) ;
    else if (data.index_type==GL_UNSIGNED_INT) ;
    else goto failure;
    
    tab->disable_fill = true; //to avoid recursion
    
    gtk_combo_box_set_active(tab->mode, data.mode);
    gtk_spin_button_set_value(tab->start, data.start);
    gtk_spin_button_set_value(tab->count, data.count);
    gtk_spin_button_set_value(tab->base_instance, data.base_instance);
    gtk_spin_button_set_value(tab->base_vertex, data.base_vertex);
    gtk_spin_button_set_value(tab->index_offset, data.index_offset);
    switch (data.index_type) {
    case GL_UNSIGNED_BYTE: gtk_combo_box_set_active(tab->index_type, 0);
    case GL_UNSIGNED_SHORT: gtk_combo_box_set_active(tab->index_type, 1);
    case GL_UNSIGNED_INT: gtk_combo_box_set_active(tab->index_type, 2);
    }
    
    tab->disable_fill = false;
    return true;
    
    failure:
        return false;
}

static void update_vertex_data_tab(gui_tab_t* gtab) {
    vertex_data_tab_t* tab = gtab->data;
    
    if (tab->data_view) {
        gtk_container_remove(GTK_CONTAINER(tab->data_view_box), GTK_WIDGET(tab->data_view_window));
        tab->data_view_window = NULL;
        tab->data_view = NULL;
    }
    
    trace_t* trace = state.trace;
    int64_t revision = state.revision;
    if (!trace || revision<0 || tab->disable_fill) goto fail;
    
    gtk_widget_set_sensitive(GTK_WIDGET(tab->mode), true);
    gtk_widget_set_sensitive(GTK_WIDGET(tab->start), true);
    gtk_widget_set_sensitive(GTK_WIDGET(tab->count), true);
    gtk_widget_set_sensitive(GTK_WIDGET(tab->base_instance), true);
    gtk_widget_set_sensitive(GTK_WIDGET(tab->base_vertex), true);
    gtk_widget_set_sensitive(GTK_WIDGET(tab->index_offset), true);
    gtk_widget_set_sensitive(GTK_WIDGET(tab->index_type), true);
    if (gtk_toggle_button_get_active(tab->autofill)) {
        if (autofill(tab)) {
            gtk_widget_set_sensitive(GTK_WIDGET(tab->mode), false);
            gtk_widget_set_sensitive(GTK_WIDGET(tab->start), false);
            gtk_widget_set_sensitive(GTK_WIDGET(tab->count), false);
            gtk_widget_set_sensitive(GTK_WIDGET(tab->base_instance), false);
            gtk_widget_set_sensitive(GTK_WIDGET(tab->base_vertex), false);
            gtk_widget_set_sensitive(GTK_WIDGET(tab->index_offset), false);
            gtk_widget_set_sensitive(GTK_WIDGET(tab->index_type), false);
        }
    }
    
    //Get context and VAO
    trc_obj_t* ctx_obj = trc_get_current_gl_context(trace, revision);
    if (!ctx_obj) goto fail;
    const trc_gl_context_rev_t* ctx = trc_obj_get_rev(ctx_obj, revision);
    
    trc_obj_t* vao_obj = ctx->bound_vao.obj;
    if (!vao_obj) goto fail;
    const trc_gl_vao_rev_t* vao = trc_obj_get_rev(vao_obj, revision);
    
    //Update data view
    create_treeview(tab, vao, ctx);
    fill_in_data_view(tab, vao, ctx);
    
    tab->data_view_window = create_scrolled_window(GTK_WIDGET(tab->data_view));
    gtk_box_pack_start(tab->data_view_box, tab->data_view_window, true, true, 0);
    gtk_widget_show_all(GTK_WIDGET(tab->data_view_box));
    
    fail:
        ;
    
    //Update visiblities
    int mode = gtk_combo_box_get_active(tab->mode);
    set_visible_at_info_box(tab->info_box, "Start", mode==MODE_VERTICES);
    set_visible_at_info_box(tab->info_box, "Index Offset", mode==MODE_PRIMITIVES);
    set_visible_at_info_box(tab->info_box, "Index Type", mode==MODE_PRIMITIVES);
    set_visible_at_info_box(tab->info_box, "Base Vertex", mode==MODE_PRIMITIVES);
}

static void changed_callback(void* widget, vertex_data_tab_t* tab) {
    update_vertex_data_tab(tab->tab);
}

gui_tab_t* open_vertex_data_tab() {
    vertex_data_tab_t* tab = malloc(sizeof(vertex_data_tab_t));
    tab->attrib_columns = NULL;
    tab->column_attribs = NULL;
    tab->disable_fill = false;
    
    //Create tab
    tab->info_box = create_info_box();
    
    tab->autofill = GTK_TOGGLE_BUTTON(gtk_check_button_new());
    gtk_toggle_button_set_active(tab->autofill, true);
    g_signal_connect(tab->autofill, "toggled", G_CALLBACK(changed_callback), tab);
    add_custom_to_info_box(tab->info_box, "Autofill", GTK_WIDGET(tab->autofill));
    
    tab->mode = create_combobox(MODE_VERTICES,
        (const char*[]){"Vertices", "Primitives", NULL});
    g_signal_connect(tab->mode, "changed", G_CALLBACK(changed_callback), tab);
    add_custom_to_info_box(tab->info_box, "Mode", GTK_WIDGET(tab->mode));
    
    tab->start = create_integral_spin_button(0, INT64_MAX, &changed_callback, tab);
    tab->count = create_integral_spin_button(0, INT64_MAX, &changed_callback, tab);
    tab->instance = create_integral_spin_button(0, INT64_MAX, &changed_callback, tab);
    tab->base_instance = create_integral_spin_button(0, INT64_MAX, &changed_callback, tab);
    
    add_custom_to_info_box(tab->info_box, "Start", GTK_WIDGET(tab->start));
    add_custom_to_info_box(tab->info_box, "Count", GTK_WIDGET(tab->count));
    add_custom_to_info_box(tab->info_box, "Instance", GTK_WIDGET(tab->instance));
    add_custom_to_info_box(tab->info_box, "Base Instance", GTK_WIDGET(tab->base_instance));
    
    tab->base_vertex = create_integral_spin_button(0, INT64_MAX, &changed_callback, tab);
    add_custom_to_info_box(tab->info_box, "Base Vertex", GTK_WIDGET(tab->base_vertex));
    
    tab->index_offset = create_integral_spin_button(0, INT64_MAX, &changed_callback, tab);
    add_custom_to_info_box(tab->info_box, "Index Offset", GTK_WIDGET(tab->index_offset));
    
    tab->index_type = create_combobox(2,
        (const char*[]){"UInt8", "Uint16", "Uint32", NULL});
    g_signal_connect(tab->index_type, "changed", G_CALLBACK(changed_callback), tab);
    add_custom_to_info_box(tab->info_box, "Index Type", GTK_WIDGET(tab->index_type));
    
    add_separator_to_info_box(tab->info_box);
    
    tab->data_view_box = GTK_BOX(gtk_box_new(GTK_ORIENTATION_VERTICAL, 0));
    tab->data_view = NULL;
    tab->data_view_window = NULL;
    add_custom_to_info_box(tab->info_box, NULL, GTK_WIDGET(tab->data_view_box));
    
    gui_tab_t* gtab = open_gui_tab(true, create_scrolled_window(tab->info_box->widget));
    gtk_label_set_text(GTK_LABEL(gtab->tab_label), "Vertex Data");
    
    tab->tab = gtab;
    gtab->data = tab;
    gtab->deinit = &deinit_vertex_data_tab;
    gtab->update = &update_vertex_data_tab;
    
    update_tab(gtab);
    
    return gtab;
}
