#include "../gui.h"
#include "../utils.h"

#include <math.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include "shared/glcorearb.h"

typedef struct texture_data_t {
    object_tab_t* tab;
    image_viewer_t* viewer;
    GtkWidget* layer_box;
    GtkWidget* face_box;
    GtkSpinButton* level;
    GtkSpinButton* layer;
    GtkComboBox* face;
    GtkEntry* internal_format;
} texture_data_t;

static void do_update(void* _, texture_data_t* data) {
    update_tab(data->tab->tab);
}

static void open_images_tab(texture_data_t* data) {
    data->level = create_integral_spin_button(0, INT64_MAX, &do_update, data);
    data->layer = create_integral_spin_button(0, INT64_MAX, &do_update, data);
    
    data->face = create_combobox(0, (const char*[]){
        "Positive X", "Negative X", "Positive Y", "Negative Y",
        "Positive Z", "Negative Z", NULL});
    g_signal_connect(data->face, "changed", G_CALLBACK(do_update), data);
    
    data->internal_format = GTK_ENTRY(gtk_entry_new());
    gtk_editable_set_editable(GTK_EDITABLE(data->internal_format), false);
    
    data->layer_box = create_box(false, 2, create_dim_label("Layer"), data->layer);
    data->face_box = create_box(false, 2, create_dim_label("Face"), data->face);
    
    GtkWidget* header_box = create_box(false, 6,
        create_dim_label("Level"), data->level, data->layer_box, data->face_box,
        create_dim_label("Internal Format"), data->internal_format);
    
    data->viewer = create_image_viewer();
    gtk_widget_set_vexpand(data->viewer->widget, true);
    
    GtkWidget* tab_content = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
    gtk_box_pack_start(GTK_BOX(tab_content), header_box, false, false, 0);
    gtk_box_pack_start(GTK_BOX(tab_content), data->viewer->widget, true, true, 0);
    
    add_object_notebook_tab(
        data->tab->obj_notebook, "Images", false, tab_content, NULL, data);
}

static void init(object_tab_t* tab) {
    texture_data_t* data = malloc(sizeof(texture_data_t));
    data->viewer = NULL;
    data->tab = tab;
    tab->data = data;
    add_obj_common_to_info_box(tab->info_box);
    
    add_multiple_to_info_box(tab->info_box,
        "Type", "Minification Filter", "Magnification Filter", "Minimum LOD",
        "Maximum LOD", "Wrap S", "Wrap T", "Wrap R", "Max Anisotropy",
        "Border Color", "Compare Mode", "Compare Function", "Depth Stencil Mode",
        "Base Level", "Max Level", "LOD Bias", "Swizzle", NULL);
    add_separator_to_info_box(tab->info_box);
    add_to_info_box(tab->info_box, "Internal Format");
    add_multiple_to_info_box(tab->info_box, //For normal textures
        "Width", "Height", "Depth", "Layers", "Mipmap Count", "Buffer", NULL);
    add_multiple_to_info_box(tab->info_box, //For multisample textures
        "Samples", "Fixed Sample Locations", NULL);
    add_multiple_to_info_box(tab->info_box, //For buffer textures
        "Buffer", "Offset", "Size", NULL);
    
    open_images_tab(data);
}

static void deinit(object_tab_t* tab) {
    free(tab->data);
}

static trc_gl_texture_image_t find_image(const trc_gl_texture_rev_t* rev, uint face, uint level, bool* found) {
    const trc_gl_texture_image_t* images = trc_map_data(rev->images, TRC_MAP_READ);
    size_t img_count = rev->images->size / sizeof(trc_gl_texture_image_t);
    
    for (size_t i = 0; i < img_count; i++) {
        if (images[i].face != face) continue;
        if (images[i].level != level) continue;
        trc_gl_texture_image_t res = images[i];
        trc_unmap_data(images);
        *found = true;
        return res;
    }
    
    trc_unmap_data(images);
    
    *found = false;
    trc_gl_texture_image_t res;
    memset(&res, 0, sizeof(res));
    return res;
}

static size_t get_pixel_size(trc_image_format_t format) {
    size_t pixel_sizes[] = {
        [TrcImageFormat_Red_U32] = 4,
        [TrcImageFormat_RedGreen_U32] = 8,
        [TrcImageFormat_RGB_U32] = 12,
        [TrcImageFormat_RGBA_U32] = 16,
        [TrcImageFormat_Red_I32] = 4,
        [TrcImageFormat_RedGreen_I32] = 8,
        [TrcImageFormat_RGB_I32] = 12,
        [TrcImageFormat_RGBA_I32] = 16,
        [TrcImageFormat_Red_F32] = 4,
        [TrcImageFormat_RedGreen_F32] = 8,
        [TrcImageFormat_RGB_F32] = 12,
        [TrcImageFormat_RGBA_F32] = 16,
        [TrcImageFormat_SRGB_U8] = 3,
        [TrcImageFormat_SRGBA_U8] = 4,
        [TrcImageFormat_F32_U24_U8] = 8,
        [TrcImageFormat_RGBA_U8] = 4};
    return pixel_sizes[format];
}

static void update_image(const trc_gl_texture_rev_t* rev, texture_data_t* data) {
    int level = gtk_spin_button_get_value(data->level);
    int layer = gtk_spin_button_get_value(data->layer);
    int face = gtk_combo_box_get_active(data->face);
    
    gtk_entry_set_text(data->internal_format, "");
    clear_image_viewer(data->viewer);
    
    bool found;
    trc_gl_texture_image_t image = find_image(rev, face, level, &found);
    if (!found) return;
    
    size_t pixel_size = get_pixel_size(image.data_format);
    size_t offset = layer * image.width * image.height * pixel_size;
    if (offset + image.width*image.height*pixel_size > image.data.size)
        return;
    
    int dim[2] = {image.width, image.height};
    update_image_viewer(data->viewer, offset, image.data, dim, image.data_format);
    gtk_entry_set_text(data->internal_format,
                       get_enum_str("InternalFormat", image.internal_format));
}

static void get_texture_info(const trc_gl_texture_rev_t* rev, int* dims, int* layers,
                             int* mipmaps, bool* cubemap, const char** internal_format) {
    memset(dims, 0, sizeof(int)*3);
    *mipmaps = 0;
    size_t img_count = rev->images->size / sizeof(trc_gl_texture_image_t);
    const trc_gl_texture_image_t* images = trc_map_data(rev->images, TRC_MAP_READ);
    for (size_t i = 0; i < img_count; i++) {
        if (images[i].level+1 > *mipmaps) *mipmaps = images[i].level + 1;
        if (images[i].level == rev->base_level) {
            dims[0] = images[i].width;
            dims[1] = images[i].height;
            dims[2] = images[i].depth;
        }
    }
    
    GLenum ifmt = (GLenum)-1;
    for (size_t i = 0; i < img_count; i++) {
        if (ifmt==(GLenum)-1)
            ifmt = images[i].internal_format;
        else if (ifmt != images[i].internal_format)
            ifmt = (GLenum)-2;
    }
    if (ifmt == (GLenum)-1)
        *internal_format = "No Images";
    else if (ifmt == (GLenum)-2)
        *internal_format = "Mixed";
    else
        *internal_format = get_enum_str("InternalFormat", ifmt);
    
    trc_unmap_data(images);
    
    int layers_index = -1;
    uint dim_count = 0;
    switch (rev->type) {
    case GL_TEXTURE_1D: dim_count = 1; break;
    case GL_TEXTURE_2D: dim_count = 2; break;
    case GL_TEXTURE_3D: dim_count = 3; break;
    case GL_TEXTURE_1D_ARRAY: dim_count = 1; layers_index = 1; break;
    case GL_TEXTURE_2D_ARRAY: dim_count = 2; layers_index = 2; break;
    case GL_TEXTURE_RECTANGLE: dim_count = 2; break;
    case GL_TEXTURE_CUBE_MAP: dim_count = 2; break;
    case GL_TEXTURE_CUBE_MAP_ARRAY: dim_count = 2; layers_index = 2; break;
    case GL_TEXTURE_2D_MULTISAMPLE: dim_count = 2; break;
    case GL_TEXTURE_2D_MULTISAMPLE_ARRAY: dim_count = 2; layers_index = 2; break;
    }
    
    int layers_div = rev->type==GL_TEXTURE_CUBE_MAP_ARRAY ? 6 : 1;
    *layers = layers_index>=0 ? dims[layers_index]/layers_div : -1;
    
    if (dim_count < 3) dims[2] = -1;
    if (dim_count < 2) dims[1] = -1;
    if (dim_count < 1) dims[0] = -1;
    
    *cubemap = rev->type==GL_TEXTURE_CUBE_MAP_ARRAY;
    *cubemap = *cubemap || rev->type==GL_TEXTURE_CUBE_MAP;
}

static void update(object_tab_t* tab, const trc_obj_rev_head_t* rev_head, uint64_t revision) {
    const trc_gl_texture_rev_t* rev = (const trc_gl_texture_rev_t*)rev_head;
    trc_gl_sample_params_t params = rev->sample_params;
    
    texture_data_t* data = tab->data;
    
    info_box_t* box = tab->info_box;
    if (!set_obj_common_at_info_box(box, rev_head, revision)) return;
    set_enum_at_info_box(box, "Type", "TextureTarget", rev->type);
    set_enum_at_info_box(box, "Minification Filter", "TextureMinFilter", params.min_filter);
    set_enum_at_info_box(box, "Magnification Filter", "TextureMagFilter", params.mag_filter);
    set_at_info_box(box, "Minimum LOD", "%g", params.min_lod);
    set_at_info_box(box, "Maximum LOD", "%g", params.max_lod);
    set_enum_at_info_box(box, "Wrap S", "TextureWrapMode", params.wrap_s);
    set_enum_at_info_box(box, "Wrap T", "TextureWrapMode", params.wrap_t);
    set_enum_at_info_box(box, "Wrap R", "TextureWrapMode", params.wrap_r);
    set_at_info_box(box, "Max Anisotropy", "%g", params.max_anisotropy);
    set_at_info_box(box, "Border Color", "[%g, %g, %g, %g]",
                    params.border_color[0], params.border_color[1],
                    params.border_color[2], params.border_color[3]);
    set_enum_at_info_box(box, "Compare Mode", "TexCompareMode", params.compare_mode);
    set_enum_at_info_box(box, "Compare Function", "DepthFunction", params.compare_func);
    
    set_enum_at_info_box(box, "Depth Stencil Mode", "DepthStencilTextureMode", rev->depth_stencil_mode);
    set_at_info_box(box, "Base Level", "%u", rev->base_level);
    set_at_info_box(box, "Max Level", "%u", rev->max_level);
    set_at_info_box(box, "LOD Bias", "%g", params.lod_bias);
    set_at_info_box(box, "Swizzle", "[%s, %s, %s, %s]",
                    get_enum_str("TextureSwizzle", rev->swizzle[0]),
                    get_enum_str("TextureSwizzle", rev->swizzle[1]),
                    get_enum_str("TextureSwizzle", rev->swizzle[2]),
                    get_enum_str("TextureSwizzle", rev->swizzle[3]));
    
    if (rev->type == GL_TEXTURE_BUFFER) {
        set_visible_at_info_box(box, "Width", false);
        set_visible_at_info_box(box, "Height", false);
        set_visible_at_info_box(box, "Depth", false);
        set_visible_at_info_box(box, "Layers", false);
        set_visible_at_info_box(box, "Mipmap Count", false);
        set_visible_at_info_box(box, "Samples", false);
        set_visible_at_info_box(box, "Fixed Sample Locations", false);
        set_visible_at_info_box(box, "Buffer", true);
        set_visible_at_info_box(box, "Offset", true);
        set_visible_at_info_box(box, "Size", true);
        
        set_at_info_box(box, "Buffer", "%u", rev->buffer);
        set_at_info_box(box, "Internal Format", "%s",
                        get_enum_str("InternalFormat", rev->buffer.internal_format));
        set_at_info_box(box, "Offset", "%zu", rev->buffer.offset);
        set_at_info_box(box, "Size", "%zu", rev->buffer.size);
    } else {
        set_visible_at_info_box(box, "Buffer", false);
        set_visible_at_info_box(box, "Offset", false);
        set_visible_at_info_box(box, "Size", false);
        
        int dims[3];
        int layers, mipmaps;
        bool cubemap;
        const char* internal_format;
        get_texture_info(rev, dims, &layers, &mipmaps, &cubemap, &internal_format);
        bool multisample = rev->type == GL_TEXTURE_2D_MULTISAMPLE_ARRAY;
        multisample = multisample || rev->type==GL_TEXTURE_2D_MULTISAMPLE;
        
        set_at_info_box(box, "Internal Format", "%s", internal_format);
        set_at_info_box(box, "Width", "%zu", dims[0]);
        set_visible_at_info_box(box, "Width", dims[0]>=0);
        set_at_info_box(box, "Height", "%zu", dims[1]);
        set_visible_at_info_box(box, "Height", dims[1]>=0);
        set_at_info_box(box, "Depth", "%zu", dims[2]);
        set_visible_at_info_box(box, "Depth", dims[2]>=0);
        set_at_info_box(box, "Layers", "%zu", layers);
        set_visible_at_info_box(box, "Layers", layers>=0);
        set_at_info_box(box, "Mipmap Count", "%zu", mipmaps);
        
        set_visible_at_info_box(box, "Samples", multisample);
        set_at_info_box(box, "Samples", "%u", rev->samples);
        set_visible_at_info_box(box, "Fixed Sample Locations", multisample);
        const char* fsl = rev->fixed_sample_locations ? "True" : "False";
        set_at_info_box(box, "Fixed Sample Locations", "%s", fsl);
        
        if (data->viewer) {
            gtk_widget_set_visible(data->layer_box, layers!=-1);
            gtk_widget_set_visible(data->face_box, cubemap);
            
            gtk_widget_set_sensitive(GTK_WIDGET(data->level), mipmaps>0);
            gtk_widget_set_sensitive(GTK_WIDGET(data->layer), layers>0);
            
            gtk_spin_button_set_range(data->level, 0.0, fmax(mipmaps-1, 0.0));
            gtk_spin_button_set_range(data->layer, 0.0, fmax(layers-1, 0.0));
            
            update_image(rev, data);
        }
    }
}

static __attribute__((constructor)) void init_callbacks() {
    object_tab_init_callbacks[(int)TrcTexture] = &init;
    object_tab_deinit_callbacks[(int)TrcTexture] = &deinit;
    object_tab_update_callbacks[(int)TrcTexture] = (object_tab_update_callback_t)&update;
}
