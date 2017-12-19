#include "../gui.h"
#include "../utils.h"

#include <stdlib.h>

typedef struct tf_data_t {
    GtkTreeStore* buffers;
} tf_data_t;

static void init(object_tab_t* tab) {
    tf_data_t* data = malloc(sizeof(tf_data_t));
    tab->data = data;
    add_obj_common_to_info_box(tab->info_box);
    add_to_info_box(tab->info_box, "Active");
    add_to_info_box(tab->info_box, "Paused");
    
    add_separator_to_info_box(tab->info_box);
    add_custom_to_info_box(tab->info_box, "Buffers", NULL);
    
    GtkTreeView* view = create_tree_view(
        4, 1, "Index", "Buffer", "Offset", "Size", G_TYPE_POINTER);
    data->buffers = GTK_TREE_STORE(gtk_tree_view_get_model(view));
    add_custom_to_info_box(tab->info_box, NULL, create_scrolled_window(GTK_WIDGET(view)));
    
    init_object_column(view, tab, 1, 4, -1);
}

static void deinit(object_tab_t* tab) {
    free(tab->data);
}

static void update(object_tab_t* tab, const trc_obj_rev_head_t* rev_head, uint64_t revision) {
    if (!set_obj_common_at_info_box(tab->info_box, rev_head, revision)) return;
    
    tf_data_t* data = tab->data;
    const trc_gl_transform_feedback_rev_t* rev =
        (const trc_gl_transform_feedback_rev_t*)rev_head;
    
    info_box_t* box = tab->info_box;
    if (!set_obj_common_at_info_box(box, rev_head, revision)) return;
    set_at_info_box(box, "Active", rev->active?"True":"False");
    set_at_info_box(box, "Paused", rev->paused?"True":"False");
    
    gtk_tree_store_clear(data->buffers);
    size_t buffer_count = rev->bindings->size / sizeof(trc_gl_buffer_binding_point_t);
    const trc_gl_buffer_binding_point_t* bufs = trc_map_data(rev->bindings, TRC_MAP_READ);
    for (size_t i = 0; i < buffer_count; i++) {
        trc_gl_buffer_binding_point_t binding = bufs[i];
        if (!binding.buf.obj) continue;
        
        char off[64] = {0};
        snprintf(off, sizeof(off)-1, "%lu", binding.offset);
        
        char size[64] = {0};
        if (binding.size) {
            snprintf(size, sizeof(size)-1, "%lu", binding.size);
        } else {
            const trc_gl_buffer_rev_t* rev = trc_obj_get_rev(binding.buf.obj, revision);
            snprintf(size, sizeof(size)-1, "%zu", rev->data.size);
        }
        
        GtkTreeIter row;
        gtk_tree_store_append(data->buffers, &row, NULL);
        gtk_tree_store_set(data->buffers, &row,
                           0, static_format("%zu", i),
                           1, static_format_obj(binding.buf.obj, revision),
                           2, off, 3, size, 4, binding.buf.obj, -1);
    }
    trc_unmap_data(bufs);
}

static __attribute__((constructor)) void init_callbacks() {
    object_tab_init_callbacks[(int)TrcTransformFeedback] = &init;
    object_tab_deinit_callbacks[(int)TrcTransformFeedback] = &deinit;
    object_tab_update_callbacks[(int)TrcTransformFeedback] = (object_tab_update_callback_t)&update;
}
