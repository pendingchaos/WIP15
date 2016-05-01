#include "libinspect/libinspect.h"
#include "utils.h"

#include <gtk/gtk.h>
#include <string.h>
#include <stdlib.h>

extern GtkBuilder* builder;
extern inspector_t* inspector;

void texture_select_callback(GObject* obj, gpointer user_data) {
    GtkTreeView* param_tree = GTK_TREE_VIEW(gtk_builder_get_object(builder, "selected_texture_treeview"));
    GtkTreeStore* param_store = GTK_TREE_STORE(gtk_tree_view_get_model(param_tree));
    
    GtkTreeView* image_tree = GTK_TREE_VIEW(gtk_builder_get_object(builder, "selected_texture_images"));
    GtkTreeStore* image_store = GTK_TREE_STORE(gtk_tree_view_get_model(image_tree));
    
    if (!param_store)
        return;
    if (!image_store)
        return;
    
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
    inspect_texture_t* tex = get_inspect_tex_vec(inspector->textures, tex_index);
    if (!tex) return;
    
    gtk_adjustment_set_upper(gtk_spin_button_get_adjustment(layer_spinbutton), tex->layer_count-1);
    
    inspect_gl_tex_params_t params = tex->params;
    
    GtkTreeIter row;
    #define VAL(name, val) gtk_tree_store_append(param_store, &row, NULL);\
    gtk_tree_store_set(param_store, &row, 0, (name), 1, (val), -1);
    if (tex->type) {
        VAL("Type", static_format("%s", get_enum_str("TextureTarget", tex->type)));
        VAL("Depth Stencil Mode", static_format("%s", get_enum_str(NULL, params.depth_stencil_mode)));
        VAL("Min Filter", static_format("%s", get_enum_str("TextureMinFilter", params.min_filter)));
        VAL("Mag Filter", static_format("%s", get_enum_str("TextureMagFilter", params.mag_filter)));
        VAL("Min LOD", static_format("%s", format_float(params.min_lod)));
        VAL("Max LOD", static_format("%s", format_float(params.max_lod)));
        VAL("LOD bias", static_format("%s", format_float(params.lod_bias)));
        VAL("Base Level", static_format("%d", params.base_level));
        VAL("Max Level", static_format("%d", params.max_level));
        VAL("Swizzle", static_format("[%s, %s, %s, %s]",
                                     get_enum_str(NULL, params.swizzle[0]),
                                     get_enum_str(NULL, params.swizzle[1]),
                                     get_enum_str(NULL, params.swizzle[2]),
                                     get_enum_str(NULL, params.swizzle[3])));
        VAL("Wrap S", static_format("%s", get_enum_str("TextureWrapMode", params.wrap[0])));
        VAL("Wrap T", static_format("%s", get_enum_str("TextureWrapMode", params.wrap[0])));
        VAL("Wrap R", static_format("%s", get_enum_str("TextureWrapMode", params.wrap[0])));
        VAL("Border Color", static_format("[%s, %s, %s, %s]",
                                          format_float(params.border_color[0]),
                                          format_float(params.border_color[1]),
                                          format_float(params.border_color[2]),
                                          format_float(params.border_color[3])));
        VAL("Compare Mode", static_format("%s", get_enum_str(NULL, params.compare_mode)));
        VAL("Compare Func", static_format("%s", get_enum_str("DepthFunction", params.compare_func)));
        VAL("View Min Level", static_format("%d", params.view_min_level));
        VAL("View Num Levels", static_format("%u", params.view_num_levels));
        VAL("View Min Layer", static_format("%d", params.view_min_layer));
        VAL("View Num Layers", static_format("%u", params.view_num_layers));
        VAL("Immutable levels", static_format("%u", params.immutable_levels));
        VAL("Image Format Compat Type", static_format("%s", get_enum_str(NULL, params.image_format_compatibility_type)));
        VAL("Immutable Format", static_format("%s", get_enum_str(NULL, params.immutable_format)));
    }
    
    if (tex->mipmaps) {
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
    }
}

void init_texture_list(GtkTreeView* tree) {
    GtkTreeStore* store = GTK_TREE_STORE(gtk_tree_view_get_model(tree));
    gtk_tree_store_clear(store);
    
    inspect_tex_vec_t textures = inspector->textures;
    for (inspect_texture_t* tex = textures->data; !vec_end(textures, tex); tex++) {
        char str[64];
        memset(str, 0, 64);
        snprintf(str, 64, "%u", tex->fake);
        
        GtkTreeIter row;
        gtk_tree_store_append(store, &row, NULL);
        gtk_tree_store_set(store, &row, 0, str, -1);
    }
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
