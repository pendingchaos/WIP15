#include "libinspect/libinspect.h"
#include "utils.h"

#include <gtk/gtk.h>
#include <string.h>

extern GtkBuilder* builder;
extern inspector_t* inspector;

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
    
    inspect_fb_vec_t fbs = inspector->framebuffers;
    for (inspect_fb_t* fb = fbs->data; !vec_end(fbs, fb); fb++) {
        char str[64];
        memset(str, 0, 64);
        snprintf(str, 64, "%u", fb->fake);
        
        gtk_tree_store_append(store, &row, NULL);
        gtk_tree_store_set(store, &row, 0, str, -1);
    }
}

static void init_framebuffer_tree(GtkTreeView* tree) {
    GtkTreeStore* store = GTK_TREE_STORE(gtk_tree_view_get_model(tree));
    gtk_tree_store_clear(store);
    
    inspect_image_t* front = inspector->front_buf;
    if (front && front->data) {
        GdkPixbuf* front_buf = gdk_pixbuf_new(GDK_COLORSPACE_RGB,
                                              TRUE,
                                              8,
                                              front->width,
                                              front->height);
        uint32_t* data = (uint32_t*)gdk_pixbuf_get_pixels(front_buf);
        for (size_t y = 0; y < front->height; y++) {
            for (size_t x = 0; x < front->width; x++) {
                data[(front->height-1-y)*front->width+x] = ((uint32_t*)front->data)[y*front->width+x];
            }
        }
        
        GtkTreeIter row;
        gtk_tree_store_append(store, &row, NULL);
        gtk_tree_store_set(store, &row, 0, "Front", 1, front_buf, -1);
        
        g_object_unref(front_buf);
    }
    
    inspect_image_t* back = inspector->back_buf;
    if (back && back->data) {
        GdkPixbuf* back_buf = gdk_pixbuf_new(GDK_COLORSPACE_RGB,
                                             TRUE,
                                             8,
                                             back->width,
                                             back->height);
        uint32_t* data = (uint32_t*)gdk_pixbuf_get_pixels(back_buf);
        for (size_t y = 0; y < back->height; y++) {
            for (size_t x = 0; x < back->width; x++) {
                data[(back->height-1-y)*back->width+x] = ((uint32_t*)back->data)[y*back->width+x];
            }
        }
        
        GtkTreeIter row;
        gtk_tree_store_append(store, &row, NULL);
        gtk_tree_store_set(store, &row, 0, "Back", 1, back_buf, -1);
        
        g_object_unref(back_buf);
    }
    
    inspect_image_t* depth = inspector->depth_buf;
    if (depth && depth->data) {
        GdkPixbuf* depth_buf = gdk_pixbuf_new(GDK_COLORSPACE_RGB,
                                              FALSE,
                                              8,
                                              depth->width,
                                              depth->height);
        
        uint8_t* data = (uint8_t*)gdk_pixbuf_get_pixels(depth_buf);
        for (size_t y = 0; y < depth->height; y++) {
            for (size_t x = 0; x < depth->width; x++) {
                size_t index = (depth->height-1-y)*depth->width + x;
                
                uint32_t val = ((uint32_t*)depth->data)[y*depth->width+x];
                val = val / 4294967296.0 * 16777216.0;
                data[index*3] = val % 256;
                data[index*3+1] = val % 65536 / 256;
                data[index*3+2] = val / 65536;
            }
        }
        
        GtkTreeIter row;
        gtk_tree_store_append(store, &row, NULL);
        gtk_tree_store_set(store, &row, 0, "Depth", 1, depth_buf, -1);
        
        g_object_unref(depth_buf);
    }
}

static void add_fb_attachment(GtkTreeStore* store, const char* name, const inspect_fb_attach_t* attach) {
    GtkTreeIter parent;
    gtk_tree_store_append(store, &parent, NULL);
    gtk_tree_store_set(store, &parent, 0, name, -1);
    
    GtkTreeIter row;
    gtk_tree_store_append(store, &row, &parent);
    gtk_tree_store_set(store, &row, 0, "Texture", 1, static_format("%u", attach->tex), -1);
    
    gtk_tree_store_append(store, &row, &parent);
    gtk_tree_store_set(store, &row, 0, "Level", 1, static_format("%u", attach->level), -1);
}

void framebuffer_select_callback(GObject* obj, gpointer user_data) {
    GtkTreePath* path;
    gtk_tree_view_get_cursor(GTK_TREE_VIEW(obj), &path, NULL);
    
    if (!path)
        return;
    
    size_t index = gtk_tree_path_get_indices(path)[0];
    
    GValue page = G_VALUE_INIT;
    g_value_init(&page, G_TYPE_INT);
    
    if (!index) { //The first one (framebuffer 0) is special
        init_framebuffer_tree(GTK_TREE_VIEW(gtk_builder_get_object(builder, "framebuffer0_treeview")));
        g_value_set_int(&page, 0);
    } else {
        g_value_set_int(&page, 1);
        index -= 1;
        inspect_fb_t* fb = get_inspect_fb_vec(inspector->framebuffers, index);
        
        if (!fb)
            return;
        
        GtkTreeView* tree = GTK_TREE_VIEW(gtk_builder_get_object(builder, "framebuffer_attachments"));
        GtkTreeStore* store = GTK_TREE_STORE(gtk_tree_view_get_model(tree));
        gtk_tree_store_clear(store);
        
        if (fb->depth.tex)
            add_fb_attachment(store, "GL_DEPTH_ATTACHMENT", &fb->depth);
        
        if (fb->stencil.tex)
            add_fb_attachment(store, "GL_STENCIL_ATTACHMENT", &fb->stencil);
        
        if (fb->depth_stencil.tex)
            add_fb_attachment(store, "GL_DEPTH_STENCIL_ATTACHMENT", &fb->depth_stencil);
        
        inspect_fb_attach_vec_t atts = fb->color;
        for (inspect_fb_attach_t* att = atts->data; !vec_end(atts, att); att++) {
            char name[256];
            memset(name, 0, 256);
            snprintf(name, 256, "GL_COLOR_ATTACHMENT%u", (uint)(att-(inspect_fb_attach_t*)atts->data));
            
            add_fb_attachment(store, name, att);
        }
    }
    
    GObject* notebook = gtk_builder_get_object(builder, "framebuffer_notebook");
    g_object_set_property(notebook, "page", &page);
}

void init_renderbuffers_list(GtkTreeView* tree) {
    GtkTreeView* content = GTK_TREE_VIEW(gtk_builder_get_object(builder, "renderbuffer_treeview"));
    GtkTreeStore* store = GTK_TREE_STORE(gtk_tree_view_get_model(content));
    gtk_tree_store_clear(store);
    
    store = GTK_TREE_STORE(gtk_tree_view_get_model(tree));
    gtk_tree_store_clear(store);
    
    inspect_rb_vec_t rbs = inspector->renderbuffers;
    for (inspect_rb_t* rb = rbs->data; !vec_end(rbs, rb); rb++) {
        char str[64];
        memset(str, 0, 64);
        snprintf(str, 64, "%u", rb->fake);
        
        GtkTreeIter row;
        gtk_tree_store_append(store, &row, NULL);
        gtk_tree_store_set(store, &row, 0, str, -1);
    }
}

void renderbuffer_select_callback(GObject* obj, gpointer user_data) {
    GtkTreePath* path;
    gtk_tree_view_get_cursor(GTK_TREE_VIEW(obj), &path, NULL);
    
    if (!path)
        return;
    
    size_t index = gtk_tree_path_get_indices(path)[0];
    
    inspect_rb_t* rb = get_inspect_rb_vec(inspector->renderbuffers, index);
    if (!rb)
        return;
    
    GtkTreeView* tree = GTK_TREE_VIEW(gtk_builder_get_object(builder, "renderbuffer_treeview"));
    GtkTreeStore* store = GTK_TREE_STORE(gtk_tree_view_get_model(tree));
    gtk_tree_store_clear(store);
    
    if (rb->internal_format) { //The internal format is 0 when the renderbuffer has not been initialized with glRenderbufferStorage*
        GtkTreeIter row;
        gtk_tree_store_append(store, &row, NULL);
        gtk_tree_store_set(store, &row, 0, "Width", 1, static_format("%zu", rb->width), -1);
        gtk_tree_store_append(store, &row, NULL);
        gtk_tree_store_set(store, &row, 0, "Height", 1, static_format("%zu", rb->height), -1);
        gtk_tree_store_append(store, &row, NULL);
        gtk_tree_store_set(store, &row, 0, "Internal Format", 1, get_enum_str(NULL, rb->internal_format), -1);
        gtk_tree_store_append(store, &row, NULL);
        gtk_tree_store_set(store, &row, 0, "Sample Count", 1, static_format("%zu", rb->sample_count), -1);
        gtk_tree_store_append(store, &row, NULL);
        gtk_tree_store_set(store, &row, 0, "Red Size", 1, static_format("%zu", rb->red_size), -1);
        gtk_tree_store_append(store, &row, NULL);
        gtk_tree_store_set(store, &row, 0, "Green Size", 1, static_format("%zu", rb->green_size), -1);
        gtk_tree_store_append(store, &row, NULL);
        gtk_tree_store_set(store, &row, 0, "Blue Size", 1, static_format("%zu", rb->blue_size), -1);
        gtk_tree_store_append(store, &row, NULL);
        gtk_tree_store_set(store, &row, 0, "Alpha Size", 1, static_format("%zu", rb->alpha_size), -1);
        gtk_tree_store_append(store, &row, NULL);
        gtk_tree_store_set(store, &row, 0, "Depth Size", 1, static_format("%zu", rb->depth_size), -1);
        gtk_tree_store_append(store, &row, NULL);
        gtk_tree_store_set(store, &row, 0, "Stencil Size", 1, static_format("%zu", rb->stencil_size), -1);
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
