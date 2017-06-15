#include "libtrace/libtrace.h"
#include "utils.h"

#include <gtk/gtk.h>
#include <string.h>
#include <stdlib.h>
#include <GL/gl.h>

extern GtkBuilder* builder;
extern trace_t* trace;
extern int64_t revision;

void init_framebuffers_list(GtkTreeView* tree) {
    GtkTreeView* content = GTK_TREE_VIEW(gtk_builder_get_object(builder, "framebuffer0_treeview"));
    GtkTreeStore* store = GTK_TREE_STORE(gtk_tree_view_get_model(content));
    gtk_tree_store_clear(store);
    
    content = GTK_TREE_VIEW(gtk_builder_get_object(builder, "framebuffer_attachments"));
    store = GTK_TREE_STORE(gtk_tree_view_get_model(content));
    gtk_tree_store_clear(store);
    
    store = GTK_TREE_STORE(gtk_tree_view_get_model(tree));
    gtk_tree_store_clear(store);
    
    GtkTreeIter row;
    gtk_tree_store_append(store, &row, NULL);
    gtk_tree_store_set(store, &row, 0, "0", -1);
    
    const trc_gl_framebuffer_rev_t* rev;
    for (size_t i = 0; trc_iter_objects(trace, TrcFramebuffer, &i, revision, (const void**)&rev);) {
        char str[64];
        memset(str, 0, 64);
        snprintf(str, 64, "%u", (uint)rev->fake);
        
        GtkTreeIter row;
        gtk_tree_store_append(store, &row, NULL);
        gtk_tree_store_set(store, &row, 0, str, -1);
    }
}

static GdkPixbuf* get_pixbuf(const trc_gl_context_rev_t* state, trc_data_t* data, bool depth) {
    uint width = state->drawable_width;
    uint height = state->drawable_height;
    
    uint32_t* img = trc_map_data(data, TRC_MAP_READ);
    
    GdkPixbuf* buf = gdk_pixbuf_new(GDK_COLORSPACE_RGB, TRUE, 8, width, height);
    if (depth) {
        uint8_t* data = (uint8_t*)gdk_pixbuf_get_pixels(buf);
        for (size_t y = 0; y < height; y++) {
            for (size_t x = 0; x < width; x++) {
                size_t index = (height-1-y)*width + x;
                uint32_t val = img[y*width+x];
                val = val / 4294967296.0 * 16777216.0;
                data[index*4] = val % 256;
                data[index*4+1] = val % 65536 / 256;
                data[index*4+2] = val / 65536;
                data[index*4+3] = 255;
            }
        }
    } else {
        uint32_t* data = (uint32_t*)gdk_pixbuf_get_pixels(buf);
        for (size_t y = 0; y < height; y++) {
            for (size_t x = 0; x < width; x++) {
                size_t index = (height-1-y)*width + x;
                data[index] = img[y*width+x] | *(const uint32_t*)"\x00\x00\x00\xff";
            }
        }
    }
    
    trc_unmap_data(data);
    
    return buf;
}

static void init_framebuffer_tree(GtkTreeView* tree) {
    GtkTreeStore* store = GTK_TREE_STORE(gtk_tree_view_get_model(tree));
    gtk_tree_store_clear(store);
    
    uint64_t fake = trc_lookup_current_fake_gl_context(trace, revision);
    const trc_gl_context_rev_t* state = trc_obj_get_rev(trc_lookup_name(trace, TrcContext, fake, revision), revision);
    
    GdkPixbuf* buf = get_pixbuf(state, state->front_color_buffer, false);
    GtkTreeIter row;
    gtk_tree_store_append(store, &row, NULL);
    gtk_tree_store_set(store, &row, 0, "Front", 1, buf, -1);
    g_object_unref(buf);
    
    buf = get_pixbuf(state, state->back_color_buffer, false);
    gtk_tree_store_append(store, &row, NULL);
    gtk_tree_store_set(store, &row, 0, "Back", 1, buf, -1);
    g_object_unref(buf);
    
    buf = get_pixbuf(state, state->back_depth_buffer, true);
    gtk_tree_store_append(store, &row, NULL);
    gtk_tree_store_set(store, &row, 0, "Depth", 1, buf, -1);
    g_object_unref(buf);
    
    //TODO: Stencil buffer
}

static void add_fb_attachment(GtkTreeStore* store, const char* name, const trc_gl_framebuffer_attachment_t* attach) {
    GtkTreeIter parent;
    gtk_tree_store_append(store, &parent, NULL);
    gtk_tree_store_set(store, &parent, 0, name, -1);
    
    GtkTreeIter row;
    if (attach->has_renderbuffer) {
        gtk_tree_store_append(store, &row, &parent);
        gtk_tree_store_set(store, &row, 0, "Renderbuffer", 1, static_format("%u", attach->fake_renderbuffer), -1);
    } else {
        gtk_tree_store_append(store, &row, &parent);
        gtk_tree_store_set(store, &row, 0, "Texture", 1, static_format("%u", attach->fake_texture), -1);
        
        gtk_tree_store_append(store, &row, &parent);
        gtk_tree_store_set(store, &row, 0, "Level", 1, static_format("%u", attach->level), -1);
        
        //TODO: Hide this for non-layered textures
        gtk_tree_store_append(store, &row, &parent);
        gtk_tree_store_set(store, &row, 0, "Layer", 1, static_format("%u", attach->level), -1);
        
        //TODO: Hide this for non-cube textures
        gtk_tree_store_append(store, &row, &parent);
        gtk_tree_store_set(store, &row, 0, "Face", 1, static_format("%u", attach->face), -1);
    }
}

void framebuffer_select_callback(GObject* obj, gpointer user_data) {
    GtkTreePath* path;
    gtk_tree_view_get_cursor(GTK_TREE_VIEW(obj), &path, NULL);
    if (!path) return;
    
    size_t index = gtk_tree_path_get_indices(path)[0];
    GtkNotebook* notebook = GTK_NOTEBOOK(gtk_builder_get_object(builder, "framebuffer_notebook"));
    
    if (!index) { //framebuffer 0 is special
        init_framebuffer_tree(GTK_TREE_VIEW(gtk_builder_get_object(builder, "framebuffer0_treeview")));
        gtk_notebook_set_current_page(notebook, 0);
    } else {
        gtk_notebook_set_current_page(notebook, 1);
        
        size_t count = 0;
        const trc_gl_framebuffer_rev_t* fb = NULL;
        for (size_t i = 0; trc_iter_objects(trace, TrcFramebuffer, &i, revision, (const void**)&fb);) {
            if (count == index-1) break;
            count++;
        }
        
        GtkTreeView* tree = GTK_TREE_VIEW(gtk_builder_get_object(builder, "framebuffer_attachments"));
        GtkTreeStore* store = GTK_TREE_STORE(gtk_tree_view_get_model(tree));
        gtk_tree_store_clear(store);
        
        size_t attach_count = fb->attachments->size / sizeof(trc_gl_framebuffer_attachment_t);
        trc_gl_framebuffer_attachment_t* attachs = trc_map_data(fb->attachments, TRC_MAP_READ);
        for (size_t i = 0; i < attach_count; i++) {
            switch (attachs[i].attachment) {
            case GL_DEPTH_ATTACHMENT: {
                add_fb_attachment(store, "GL_DEPTH_ATTACHMENT", &attachs[i]);
                break;
            }
            case GL_STENCIL_ATTACHMENT: {
                add_fb_attachment(store, "GL_STENCIL_ATTACHMENT", &attachs[i]);
                break;
            }
            case GL_DEPTH_STENCIL_ATTACHMENT: {
                add_fb_attachment(store, "GL_DEPTH_STENCIL_ATTACHMENT", &attachs[i]);
                break;
            }
            default: {
                uint index = attachs[i].attachment-GL_COLOR_ATTACHMENT0;
                add_fb_attachment(store, static_format("GL_COLOR_ATTACHMENT%u", index), &attachs[i]);
                break;
            }
            }
        }
        trc_unmap_data(fb->attachments);
    }
}

void init_renderbuffers_list(GtkTreeView* tree) {
    GtkTreeView* content = GTK_TREE_VIEW(gtk_builder_get_object(builder, "renderbuffer_treeview"));
    GtkTreeStore* store = GTK_TREE_STORE(gtk_tree_view_get_model(content));
    gtk_tree_store_clear(store);
    
    store = GTK_TREE_STORE(gtk_tree_view_get_model(tree));
    gtk_tree_store_clear(store);
    
    const trc_gl_renderbuffer_rev_t* rev;
    for (size_t i = 0; trc_iter_objects(trace, TrcRenderbuffer, &i, revision, (const void**)&rev);) {
        char str[64];
        memset(str, 0, 64);
        snprintf(str, 64, "%u", (uint)rev->fake);
        
        GtkTreeIter row;
        gtk_tree_store_append(store, &row, NULL);
        gtk_tree_store_set(store, &row, 0, str, -1);
    }
}

void renderbuffer_select_callback(GObject* obj, gpointer user_data) {
    GtkTreePath* path;
    gtk_tree_view_get_cursor(GTK_TREE_VIEW(obj), &path, NULL);
    if (!path) return;
    
    size_t index = gtk_tree_path_get_indices(path)[0];
    
    size_t count = 0;
    const trc_gl_renderbuffer_rev_t* rb = NULL;
    for (size_t i = 0; trc_iter_objects(trace, TrcRenderbuffer, &i, revision, (const void**)&rb);) {
        if (count == index-1) break;
        count++;
    }
    
    GtkTreeView* tree = GTK_TREE_VIEW(gtk_builder_get_object(builder, "renderbuffer_treeview"));
    GtkTreeStore* store = GTK_TREE_STORE(gtk_tree_view_get_model(tree));
    gtk_tree_store_clear(store);
    
    if (rb->has_storage) {
        GtkTreeIter row;
        gtk_tree_store_append(store, &row, NULL);
        gtk_tree_store_set(store, &row, 0, "Width", 1, static_format("%zu", rb->width), -1);
        gtk_tree_store_append(store, &row, NULL);
        gtk_tree_store_set(store, &row, 0, "Height", 1, static_format("%zu", rb->height), -1);
        gtk_tree_store_append(store, &row, NULL);
        gtk_tree_store_set(store, &row, 0, "Internal Format", 1, get_enum_str(NULL, rb->internal_format), -1);
        gtk_tree_store_append(store, &row, NULL);
        if (rb->sample_count > 1) {
            gtk_tree_store_set(store, &row, 0, "Sample Count", 1, static_format("%zu", rb->sample_count), -1);
            gtk_tree_store_append(store, &row, NULL);
        }
        const char* names[] = {"Red Bits", "Green Bits", "Blue Bits", "Alpha Bits"};
        for (size_t i = 0; i < 4; i++) {
            if (rb->rgba_bits[i]) {
                gtk_tree_store_set(store, &row, 0, names[i], 1, static_format("%zu", rb->rgba_bits[i]), -1);
                gtk_tree_store_append(store, &row, NULL);
            }
        }
        if (rb->depth_bits) {
            gtk_tree_store_set(store, &row, 0, "Depth Bits", 1, static_format("%zu", rb->depth_bits), -1);
            gtk_tree_store_append(store, &row, NULL);
        }
        if (rb->stencil_bits) {
            gtk_tree_store_set(store, &row, 0, "Stencil Bits", 1, static_format("%zu", rb->stencil_bits), -1);
            gtk_tree_store_append(store, &row, NULL);
        }
    }
}

void framebuffer_init() {
    init_treeview(builder, "framebuffers_treeview", 1);
    init_treeview(builder, "framebuffer_attachments", 2);
    
    //Initialize framebuffer 0 view
    GObject* fb0_view = gtk_builder_get_object(builder, "framebuffer0_treeview");
    GtkTreeStore* store = gtk_tree_store_new(2, G_TYPE_STRING, GDK_TYPE_PIXBUF);
    gtk_tree_view_set_model(GTK_TREE_VIEW(fb0_view),
                            GTK_TREE_MODEL(store));
    g_object_unref(store);
    GtkCellRenderer* renderer = gtk_cell_renderer_text_new();
    GtkTreeViewColumn* column = gtk_tree_view_get_column(GTK_TREE_VIEW(fb0_view), 0);
    gtk_tree_view_column_pack_start(column, renderer, FALSE);
    gtk_tree_view_column_set_attributes(column, renderer, "text", 0, NULL);
    renderer = gtk_cell_renderer_pixbuf_new();
    column = gtk_tree_view_get_column(GTK_TREE_VIEW(fb0_view), 1);
    gtk_tree_view_column_pack_start(column, renderer, FALSE);
    gtk_tree_view_column_set_attributes(column, renderer, "pixbuf", 1, NULL);
}

void renderbuffer_init() {
    init_treeview(builder, "renderbuffer_list", 1);
    init_treeview(builder, "renderbuffer_treeview", 2);
}
