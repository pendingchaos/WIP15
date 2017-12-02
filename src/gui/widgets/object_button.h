#ifndef OBJECT_BUTTON_H
#define OBJECT_BUTTON_H
#include "libtrace/libtrace.h"

#include <gtk/gtk.h>
#include <stdint.h>

typedef struct object_button_t {
    GtkWidget* widget;
    trc_obj_t* object;
    uint64_t revision;
} object_button_t;

object_button_t* create_object_button();
void update_object_button(object_button_t* button, trc_obj_t* obj, uint64_t revision);
#endif
