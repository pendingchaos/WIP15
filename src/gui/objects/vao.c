#include "../gui.h"
#include "../utils.h"

#include <stdlib.h>

typedef struct vao_data_t {
    GtkTreeStore* attributes;
} vao_data_t;

static void init(object_tab_t* tab) {
    vao_data_t* data = malloc(sizeof(vao_data_t));
    tab->data = data;
    add_obj_common_to_info_box(tab->info_box);
    
    add_to_info_box(tab->info_box, "Element Buffer");
    
    add_custom_to_info_box(tab->info_box, "Attributes", NULL);
    
    GtkTreeView* view = create_tree_view(
        10, "Index", "Enabled", "Size", "Stride", "Offset",
        "Type", "Normalized", "Integer", "Divisor", "Buffer");
    data->attributes = GTK_TREE_STORE(gtk_tree_view_get_model(view));
    add_custom_to_info_box(tab->info_box, NULL, create_scrolled_window(GTK_WIDGET(view)));
}

static void deinit(object_tab_t* tab) {
    free(tab->data);
}

static void update(object_tab_t* tab, const trc_obj_rev_head_t* rev_head, uint64_t revision) {
    const trc_gl_vao_rev_t* rev = (const trc_gl_vao_rev_t*)rev_head;
    vao_data_t* data = tab->data;
    
    if (!set_obj_common_at_info_box(tab->info_box, rev_head, revision)) return;
    
    const char* buffer_str = static_format_obj(rev->element_buffer.obj, revision);
    set_at_info_box(tab->info_box, "Element Buffer", buffer_str);
    
    const trc_gl_vao_attrib_t* attribs = trc_map_data(rev->attribs, TRC_MAP_READ);
    for (size_t i = 0; i < rev->attribs->size/sizeof(trc_gl_vao_attrib_t); i++) {
        const trc_gl_vao_attrib_t* attr = &attribs[i];
        
        const char* type_str = get_enum_str("VertexAttribType", attr->type);
        char index_str[64] = {0};
        snprintf(index_str, sizeof(index_str)-1, "%u", (uint)i);
        char size_str[64] = {0};
        snprintf(size_str, sizeof(size_str)-1, "%u", attr->size);
        char stride_str[64] = {0};
        snprintf(stride_str, sizeof(stride_str)-1, "%u", attr->stride);
        char offset_str[64] = {0};
        snprintf(offset_str, sizeof(offset_str)-1, "%lu", attr->offset);
        char divisor_str[64] = {0};
        snprintf(divisor_str, sizeof(divisor_str-1), "%u", attr->divisor);
        const char* buffer_str = static_format_obj(attr->buffer.obj, revision);
        
        GtkTreeIter row;
        gtk_tree_store_append(data->attributes, &row, NULL);
        gtk_tree_store_set(data->attributes, &row,
                           0, index_str,
                           1, attr->enabled ? "True" : "False",
                           2, size_str,
                           3, stride_str,
                           4, offset_str,
                           5, type_str,
                           6, attr->normalized ? "True" : "False",
                           7, attr->integer ? "True" : "False",
                           8, divisor_str,
                           9, buffer_str,
                           -1);
    }
    trc_unmap_data(rev->attribs);
}

static __attribute__((constructor)) void init_callbacks() {
    object_tab_init_callbacks[(int)TrcVAO] = &init;
    object_tab_deinit_callbacks[(int)TrcVAO] = &deinit;
    object_tab_update_callbacks[(int)TrcVAO] = (object_tab_update_callback_t)&update;
}
