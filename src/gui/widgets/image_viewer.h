#ifndef IMAGE_VIEWER_H
#define IMAGE_VIEWER_H
#include "libtrace/libtrace.h"
#include <gtk/gtk.h>
#include <stdbool.h>
#include <stddef.h>

typedef struct image_viewer_t {
    size_t data_offset;
    trc_chunked_data_t* data;
    uint8_t* data_data;
    trc_image_format_t format;
    GtkWidget* widget;
    GtkCheckButton* flip_y;
    GtkCheckButton* srgb;
    GtkCheckButton* show_border;
    GtkSpinButton* zoom;
    GtkLabel* current_pixel;
    GtkGLArea* gl_area;
    GtkTextView* shader_editor;
    GtkTextView* shader_info_log;
    double view_offset[2];
    int viewport[4];
    int image_width;
    int image_height;
    
    bool program_dirty;
    bool texture_dirty;
    uint textures[2];
    uint program;
    
    bool dragging;
    gdouble drag_start[2];
} image_viewer_t;

image_viewer_t* create_image_viewer();
void clear_image_viewer(image_viewer_t* viewer);
void update_image_viewer(image_viewer_t* viewer, size_t offset,
                         trc_chunked_data_t data,
                         int dim[2], trc_image_format_t format);
#endif
