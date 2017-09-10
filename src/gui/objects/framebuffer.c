#include "../gui.h"
#include "../utils.h"

#include <stdlib.h>
#include "shared/glcorearb.h"

typedef struct fb_data_t {
    GtkTreeView* draw_buffers_view;
    GtkTreeStore* draw_buffers_store;
    GtkTreeStore* attachments;
} fb_data_t;

static void init(object_tab_t* tab) {
    fb_data_t* data = malloc(sizeof(fb_data_t));
    tab->data = data;
    add_obj_common_to_info_box(tab->info_box);
    
    data->draw_buffers_view = create_tree_view(1, "");
    gtk_tree_view_set_headers_visible(data->draw_buffers_view, false);
    gtk_widget_set_vexpand(GTK_WIDGET(data->draw_buffers_view), false);
    data->draw_buffers_store = GTK_TREE_STORE(gtk_tree_view_get_model(data->draw_buffers_view));
    
    add_custom_to_info_box(tab->info_box, "Draw Buffers", NULL);
    add_custom_to_info_box(tab->info_box, NULL,
                           create_scrolled_window(GTK_WIDGET(data->draw_buffers_view)));
    
    add_separator_to_info_box(tab->info_box);
    add_custom_to_info_box(tab->info_box, "Attachments", NULL);
    
    GtkTreeView* view = create_tree_view(
        6, "Attachment", "Type", "Object", "Level", "Layer", "Face");
    data->attachments = GTK_TREE_STORE(gtk_tree_view_get_model(view));
    add_custom_to_info_box(tab->info_box, NULL, create_scrolled_window(GTK_WIDGET(view)));
}

static void deinit(object_tab_t* tab) {
    free(tab->data);
}

static void update(object_tab_t* tab, trc_obj_rev_head_t* rev_head, uint64_t revision) {
    const trc_gl_framebuffer_rev_t* rev = (const trc_gl_framebuffer_rev_t*)rev_head;
    
    fb_data_t* data = tab->data;
    if (!set_obj_common_at_info_box(tab->info_box, rev_head, revision)) return;
    
    gtk_tree_store_clear(data->draw_buffers_store);
    GLenum* draw_buffers = trc_map_data(rev->draw_buffers, TRC_MAP_READ);
    for (size_t i = 0; i < rev->draw_buffers->size/sizeof(GLenum); i++) {
        char str[64] = {0};
        snprintf(str, sizeof(str)-1, "[%zu]=%s", i, get_enum_str("DrawBuffersBuffer", draw_buffers[i]));
        GtkTreeIter row;
        gtk_tree_store_append(data->draw_buffers_store, &row, NULL);
        gtk_tree_store_set(data->draw_buffers_store, &row, 0, str, -1);
    }
    trc_unmap_data(draw_buffers);
    
    trc_gl_framebuffer_attachment_t* attachments = trc_map_data(rev->attachments, TRC_MAP_READ);
    size_t count = rev->attachments->size/sizeof(trc_gl_framebuffer_attachment_t);
    gtk_tree_store_clear(data->attachments);
    for (size_t i = 0; i < count; i++) {
        trc_gl_framebuffer_attachment_t* a = &attachments[i];
        const char* id;
        if (a->has_renderbuffer)
            id = static_format_obj(a->renderbuffer.obj, revision);
        else
            id = static_format_obj(a->texture.obj, revision);
        
        char level[16] = {0};
        char layer[16] = {0};
        const char* face = "";
        if (!a->has_renderbuffer) {
            snprintf(level, sizeof(level)-1, "%u", a->level);
            snprintf(layer, sizeof(layer)-1, "%u", a->layer);
            const trc_gl_texture_rev_t* tex_rev = trc_obj_get_rev(a->texture.obj, revision);
            if (tex_rev->type==GL_TEXTURE_CUBE_MAP || tex_rev->type==GL_TEXTURE_CUBE_MAP_ARRAY) {
                face = (const char*[]){
                    "Positive X", "Negative X", "Positive Y",
                    "Negative Y", "Positive Z", "Negative Z"}[a->face];
            }
        }
        
        GtkTreeIter row;
        gtk_tree_store_append(data->attachments, &row, NULL);
        gtk_tree_store_set(data->attachments, &row,
                           0, get_enum_str("Attachment", a->attachment),
                           1, a->has_renderbuffer?"Renderbuffer":"Texture",
                           2, id, 3, level, 4, layer, 5, face, -1);
    }
    trc_unmap_data(attachments);
}

static __attribute__((constructor)) void init_callbacks() {
    object_tab_init_callbacks[(int)TrcFramebuffer] = &init;
    object_tab_deinit_callbacks[(int)TrcFramebuffer] = &deinit;
    object_tab_update_callbacks[(int)TrcFramebuffer] = (object_tab_update_callback_t)&update;
}
