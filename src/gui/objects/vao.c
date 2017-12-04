#include "../gui.h"
#include "../utils.h"

#include <stdlib.h>

typedef struct vao_data_t {
    GtkTreeStore* attributes;
    GtkTreeStore* bindings;
    object_button_t* element_buffer;
} vao_data_t;

static void init(object_tab_t* tab) {
    vao_data_t* data = malloc(sizeof(vao_data_t));
    tab->data = data;
    add_obj_common_to_info_box(tab->info_box);
    
    data->element_buffer = create_object_button();
    add_custom_to_info_box(tab->info_box, "Element Buffer", data->element_buffer->widget);
    
    add_custom_to_info_box(tab->info_box, "Attributes", NULL);
    
    GtkTreeView* view = create_tree_view(
        8, 0, "Index", "Enabled", "Size", "Relative Offset",
        "Type", "Normalized", "Integer", "Binding");
    data->attributes = GTK_TREE_STORE(gtk_tree_view_get_model(view));
    add_custom_to_info_box(tab->info_box, NULL, create_scrolled_window(GTK_WIDGET(view)));
    
    add_custom_to_info_box(tab->info_box, "Bindings", NULL);
    
    view = create_tree_view(
        5, 1, "Index", "Offset", "Stride", "Divisor", "Buffer", G_TYPE_POINTER);
    data->bindings = GTK_TREE_STORE(gtk_tree_view_get_model(view));
    add_custom_to_info_box(tab->info_box, NULL, create_scrolled_window(GTK_WIDGET(view)));
    
    init_object_column(view, tab, 4, 5);
}

static void deinit(object_tab_t* tab) {
    free(tab->data);
}

static void update(object_tab_t* tab, const trc_obj_rev_head_t* rev_head, uint64_t revision) {
    const trc_gl_vao_rev_t* rev = (const trc_gl_vao_rev_t*)rev_head;
    vao_data_t* data = tab->data;
    
    if (!set_obj_common_at_info_box(tab->info_box, rev_head, revision)) return;
    
    update_object_button(data->element_buffer, rev->element_buffer.obj, revision);
    
    const trc_gl_vao_attrib_t* attribs = trc_map_data(rev->attribs, TRC_MAP_READ);
    gtk_tree_store_clear(data->attributes);
    for (size_t i = 0; i < rev->attribs->size/sizeof(trc_gl_vao_attrib_t); i++) {
        const trc_gl_vao_attrib_t* attr = &attribs[i];
        
        const char* type_str = get_enum_str("VertexAttribType", attr->type);
        char index_str[64] = {0};
        snprintf(index_str, sizeof(index_str)-1, "%u", (uint)i);
        char size_str[64] = {0};
        snprintf(size_str, sizeof(size_str)-1, "%u", attr->size);
        char offset_str[64] = {0};
        snprintf(offset_str, sizeof(offset_str)-1, "%lu", attr->offset);
        char binding_str[64] = {0};
        snprintf(binding_str, sizeof(binding_str)-1, "%u", attr->buffer_index);
        
        GtkTreeIter row;
        gtk_tree_store_append(data->attributes, &row, NULL);
        gtk_tree_store_set(data->attributes, &row,
                           0, index_str,
                           1, attr->enabled ? "True" : "False",
                           2, size_str,
                           3, offset_str,
                           4, type_str,
                           5, attr->normalized ? "True" : "False",
                           6, attr->integer ? "True" : "False",
                           7, binding_str,
                           -1);
    }
    trc_unmap_data(attribs);
    
    const trc_gl_vao_buffer_t* buffers = trc_map_data(rev->buffers, TRC_MAP_READ);
    gtk_tree_store_clear(data->bindings);
    for (size_t i = 0; i < rev->buffers->size/sizeof(trc_gl_vao_buffer_t); i++) {
        const trc_gl_vao_buffer_t* buf = &buffers[i];
        
        char index_str[64] = {0};
        snprintf(index_str, sizeof(index_str)-1, "%u", (uint)i);
        char offset_str[64] = {0};
        snprintf(offset_str, sizeof(offset_str)-1, "%lu", buf->offset);
        char stride_str[64] = {0};
        snprintf(stride_str, sizeof(stride_str)-1, "%u", buf->stride);
        char divisor_str[64] = {0};
        snprintf(divisor_str, sizeof(divisor_str-1), "%u", buf->divisor);
        const char* buffer_str = static_format_obj(buf->buffer.obj, revision);
        
        GtkTreeIter row;
        gtk_tree_store_append(data->bindings, &row, NULL);
        gtk_tree_store_set(data->bindings, &row,
                           0, index_str,
                           1, offset_str,
                           2, stride_str,
                           3, divisor_str,
                           4, buffer_str,
                           5, buf->buffer.obj,
                           -1);
    }
    trc_unmap_data(buffers);
}

static __attribute__((constructor)) void init_callbacks() {
    object_tab_init_callbacks[(int)TrcVAO] = &init;
    object_tab_deinit_callbacks[(int)TrcVAO] = &deinit;
    object_tab_update_callbacks[(int)TrcVAO] = (object_tab_update_callback_t)&update;
}
