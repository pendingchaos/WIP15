#include "libtrace/libtrace.h"
#include "utils.h"

#include <gtk/gtk.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include <GL/gl.h>

extern GtkBuilder* builder;
extern trace_t* trace;
extern int64_t revision;

void texture_select_callback(GObject* obj, gpointer user_data) {
    GtkTreeView* param_tree = GTK_TREE_VIEW(gtk_builder_get_object(builder, "selected_texture_treeview"));
    GtkTreeStore* param_store = GTK_TREE_STORE(gtk_tree_view_get_model(param_tree));
    
    GtkTreeView* image_tree = GTK_TREE_VIEW(gtk_builder_get_object(builder, "selected_texture_images"));
    GtkTreeStore* image_store = GTK_TREE_STORE(gtk_tree_view_get_model(image_tree));
    if (!param_store) return;
    if (!image_store) return;
    
    gtk_tree_store_clear(image_store);
    gtk_tree_store_clear(param_store);
    
    GtkTreePath* path;
    GtkSpinButton* layer_spinbutton = GTK_SPIN_BUTTON(gtk_builder_get_object(builder, "layer_spinbutton"));
    
    gtk_tree_view_get_cursor(GTK_TREE_VIEW(obj), &path, NULL);
    if (!path) {
        gtk_adjustment_set_upper(gtk_spin_button_get_adjustment(layer_spinbutton), 0);
        return;
    }
    
    //Initialize params
    size_t tex_index = gtk_tree_path_get_indices(path)[0];
    size_t count = 0;
    const trc_gl_texture_rev_t* tex = NULL;
    TRC_ITER_OBJECTS_BEGIN(TrcTexture, trc_gl_texture_rev_t)
        if (count == tex_index) {
            tex = rev;
            break;
        }
        count++;
    TRC_ITER_OBJECTS_END
    //TODO
    gtk_adjustment_set_upper(gtk_spin_button_get_adjustment(layer_spinbutton), /*tex->layer_count-1*/0);
    
    GtkTreeIter row;
    #define VAL(name, val) do {gtk_tree_store_append(param_store, &row, NULL);\
    gtk_tree_store_set(param_store, &row, 0, (name), 1, (val), -1);} while (0)
    if (!tex->has_object) return;
    
    VAL("Type", get_enum_str(NULL, tex->type));
    VAL("Depth Stencil Mode", static_format("%s", get_enum_str(NULL, tex->depth_stencil_mode)));
    VAL("Min Filter", static_format("%s", get_enum_str("TextureMinFilter", tex->sample_params.min_filter)));
    VAL("Mag Filter", static_format("%s", get_enum_str("TextureMagFilter", tex->sample_params.mag_filter)));
    VAL("Min LOD", static_format("%s", format_float(tex->sample_params.min_lod)));
    VAL("Max LOD", static_format("%s", format_float(tex->sample_params.max_lod)));
    VAL("LOD bias", static_format("%s", format_float(tex->lod_bias)));
    VAL("Base Level", static_format("%d", tex->base_level));
    VAL("Max Level", static_format("%d", tex->max_level));
    VAL("Swizzle", static_format("[%s, %s, %s, %s]",
                                 get_enum_str(NULL, tex->swizzle[0]), //TODO: The group
                                 get_enum_str(NULL, tex->swizzle[1]), //TODO: The group
                                 get_enum_str(NULL, tex->swizzle[2]), //TODO: The group
                                 get_enum_str(NULL, tex->swizzle[3]))); //TODO: The group
    VAL("Wrap S", static_format("%s", get_enum_str("TextureWrapMode", tex->sample_params.wrap_s)));
    VAL("Wrap T", static_format("%s", get_enum_str("TextureWrapMode", tex->sample_params.wrap_t)));
    VAL("Wrap R", static_format("%s", get_enum_str("TextureWrapMode", tex->sample_params.wrap_r)));
    VAL("Border Color", static_format("[%s, %s, %s, %s]",
                                      format_float(tex->sample_params.border_color[0]),
                                      format_float(tex->sample_params.border_color[1]),
                                      format_float(tex->sample_params.border_color[2]),
                                      format_float(tex->sample_params.border_color[3])));
    VAL("Compare Mode", static_format("%s", get_enum_str(NULL, tex->sample_params.compare_mode))); //TODO: The group
    VAL("Compare Func", static_format("%s", get_enum_str("DepthFunction", tex->sample_params.compare_func)));
    
    //TODO: The image should update when the layer spinbutton or face combobox update
    
    trc_gl_texture_image_t* images = trc_map_data(tex->images, TRC_MAP_READ);
    size_t img_count = tex->images->size / sizeof(trc_gl_texture_image_t);
    
    uint dims[3] = {0, 0, 0};
    uint max_level = 0;
    for (size_t i = 0; i < img_count; i++) {
        if (images[i].width > dims[0]) dims[0] = images[i].width;
        if (images[i].height > dims[1]) dims[1] = images[i].height;
        if (images[i].depth > dims[2]) dims[2] = images[i].depth;
        if (images[i].level > max_level) max_level = images[i].level;
    }
    
    int layers = -1;
    uint dim_count = 0;
    uint layers_div = 1;
    switch (tex->type) {
    case GL_TEXTURE_1D: dim_count = 1; break;
    case GL_TEXTURE_2D: dim_count = 2; break;
    case GL_TEXTURE_3D: dim_count = 3; break;
    case GL_TEXTURE_1D_ARRAY: dim_count = 1; layers = 1; break;
    case GL_TEXTURE_2D_ARRAY: dim_count = 2; layers = 2; break;
    case GL_TEXTURE_RECTANGLE: dim_count = 2; break;
    case GL_TEXTURE_CUBE_MAP: dim_count = 2; break;
    case GL_TEXTURE_CUBE_MAP_ARRAY: dim_count = 2; layers = 2; layers_div = 6; break;
    case GL_TEXTURE_BUFFER: assert(false); break; //TODO
    case GL_TEXTURE_2D_MULTISAMPLE: dim_count = 2; break;
    case GL_TEXTURE_2D_MULTISAMPLE_ARRAY: dim_count = 2; break;
    }
    if (dim_count > 0) VAL("Width", static_format("%zu", dims[0]));
    if (dim_count > 1) VAL("Height", static_format("%zu", dims[1]));
    if (dim_count > 2) VAL("Depth", static_format("%zu", dims[2]));
    if (layers != -1) VAL("Layers", static_format("%zu", dims[layers]/layers_div));
    VAL("Mipmap Count", static_format("%zu", max_level+1));
    
    //TODO: Currently assumes 2d floating point data
    for (size_t level = 0; level <= max_level; level++) {
        for (size_t i = 0; i < img_count; i++) {
            trc_gl_texture_image_t* img = &images[i];
            //if (img->face != ) continue; //TODO
            if (img->level != level) continue;
            
            float* imgdata = trc_map_data(img->data, TRC_MAP_READ);
            
            GtkTreeIter row;
            gtk_tree_store_append(image_store, &row, NULL);
            GdkPixbuf* pixbuf = gdk_pixbuf_new(GDK_COLORSPACE_RGB, TRUE, 8, img->width, img->height);
            uint8_t* dest = (uint8_t*)gdk_pixbuf_get_pixels(pixbuf);
            for (uint y = 0; y < img->height; y++) {
                for (uint x = 0; x < img->width; x++) {
                    for (size_t c = 0; c < 4; c++)
                        dest[((img->height-1-y)*img->width+x)*4+c] = imgdata[(y*img->width+x)*4+c] * 255;
                }
            }
            gtk_tree_store_set(image_store, &row, 0, static_format("%u", img->level), 1, pixbuf, -1);
            g_object_unref(pixbuf);
            
            trc_unmap_data(img->data);
        }
    }
    
    trc_unmap_data(tex->images);
    
    //TODO
    /*if (tex->mipmaps) {
        VAL("Width", static_format("%zu", tex->width));
        VAL("Height", static_format("%zu", tex->height));
        VAL("Depth", static_format("%zu", tex->depth));
        VAL("Mipmap Count", static_format("%zu", tex->mipmap_count));
        VAL("Layer Count", static_format("%zu", tex->layer_count));
        #undef VAL
        
        GtkSpinButton* layer_button = GTK_SPIN_BUTTON(gtk_builder_get_object(builder, "layer_spinbutton"));
        uint32_t layer = gtk_spin_button_get_value(GTK_SPIN_BUTTON(layer_button));
        
        GtkComboBox* face_combobox = GTK_COMBO_BOX(gtk_builder_get_object(builder, "face_combobox"));
        gint face = gtk_combo_box_get_active(GTK_COMBO_BOX(face_combobox));
        
        //Initialize images
        size_t w = tex->width;
        size_t h = tex->height;
        for (size_t level = 0; level < tex->mipmap_count; level++) {
            inspect_image_t* img = inspect_get_tex_mipmap(tex, level, layer, face);
            if (img->has_data) {
                uint32_t* data = (uint32_t*)malloc(w*h*4);
                inspect_get_image_data(img, data);
                
                GtkTreeIter row;
                gtk_tree_store_append(image_store, &row, NULL);
                GdkPixbuf* pixbuf = gdk_pixbuf_new(GDK_COLORSPACE_RGB, TRUE, 8, w, h);
                uint32_t* dest = (uint32_t*)gdk_pixbuf_get_pixels(pixbuf);
                for (size_t y = 0; y < h; y++) {
                    for (size_t x = 0; x < w; x++) {
                        dest[(h-1-y)*w+x] = data[y*w+x];
                    }
                }
                gtk_tree_store_set(image_store, &row, 0, static_format("%u", level), 1, pixbuf, -1);
                g_object_unref(pixbuf);
                
                free(data);
            }
            
            w /= 2;
            h /= 2;
        }
    }*/
}

void init_texture_list(GtkTreeView* tree) {
    GtkTreeStore* store = GTK_TREE_STORE(gtk_tree_view_get_model(tree));
    gtk_tree_store_clear(store);
    
    TRC_ITER_OBJECTS_BEGIN(TrcTexture, trc_gl_texture_rev_t)
        char str[64];
        memset(str, 0, 64);
        snprintf(str, 64, "%u", (uint)rev->fake);
        
        GtkTreeIter row;
        gtk_tree_store_append(store, &row, NULL);
        gtk_tree_store_set(store, &row, 0, str, -1);
    TRC_ITER_OBJECTS_END
}

void texture_init() {
    init_treeview(builder, "texture_list_treeview", 1);
    init_treeview(builder, "selected_texture_treeview", 2);
    
    //Initialize texture image view
    GObject* tex_image_view = gtk_builder_get_object(builder, "selected_texture_images");
    GtkTreeStore* store = gtk_tree_store_new(2, G_TYPE_STRING, GDK_TYPE_PIXBUF);
    gtk_tree_view_set_model(GTK_TREE_VIEW(tex_image_view),
                            GTK_TREE_MODEL(store));
    g_object_unref(store);
    GtkCellRenderer* renderer = gtk_cell_renderer_text_new();
    GtkTreeViewColumn* column = gtk_tree_view_get_column(GTK_TREE_VIEW(tex_image_view), 0);
    gtk_tree_view_column_pack_start(column, renderer, FALSE);
    gtk_tree_view_column_set_attributes(column, renderer, "text", 0, NULL);
    renderer = gtk_cell_renderer_pixbuf_new();
    column = gtk_tree_view_get_column(GTK_TREE_VIEW(tex_image_view), 1);
    gtk_tree_view_column_pack_start(column, renderer, FALSE);
    gtk_tree_view_column_set_attributes(column, renderer, "pixbuf", 1, NULL);
    
    //Initialize face combobox
    GObject* face = gtk_builder_get_object(builder, "face_combobox");
    GtkTreeStore* face_store = gtk_tree_store_new(1, G_TYPE_STRING);
    gtk_combo_box_set_model(GTK_COMBO_BOX(face), GTK_TREE_MODEL(face_store));
    renderer = gtk_cell_renderer_text_new();
    gtk_cell_layout_pack_start(GTK_CELL_LAYOUT(face), renderer, TRUE);
    gtk_cell_layout_set_attributes(GTK_CELL_LAYOUT(face), renderer, "text", 0, NULL);
    GtkTreeIter row;
    const char* strs[] = {"Positive X", "Negative X", "Positive Y", "Negative Y", "Positive Z", "Negative Z"};
    for (size_t i = 0; i < sizeof(strs)/sizeof(strs[0]); i++) {
        gtk_tree_store_append(face_store, &row, NULL);
        gtk_tree_store_set(face_store, &row, 0, strs[i], -1);
    }
    g_object_unref(face_store);
    gtk_combo_box_set_active(GTK_COMBO_BOX(face), 0);
    
    //Initialize layer
    GtkSpinButton* button = GTK_SPIN_BUTTON(gtk_builder_get_object(builder, "layer_spinbutton"));
    gtk_adjustment_set_upper(gtk_spin_button_get_adjustment(button), 0);
}
