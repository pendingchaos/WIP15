#ifndef INFO_BOX_H
#define INFO_BOX_H
#include "libtrace/libtrace.h"

#include <gtk/gtk.h>
#include <stdbool.h>

typedef struct info_box_t {
    GtkWidget* widget;
    size_t entry_count;
} info_box_t;

info_box_t* create_info_box();
void free_info_box(info_box_t* box);
void clear_info_box(info_box_t* box);
void add_to_info_box(info_box_t* box, const char* name);
void add_multiple_to_info_box(info_box_t* box, ...);
void add_separator_to_info_box(info_box_t* box);
void set_at_info_box(info_box_t* box, const char* name, const char* fmt, ...);
void set_enum_at_info_box(info_box_t* box, const char* name, const char* group, uint val);
void set_visible_at_info_box(info_box_t* box, const char* name, bool visible);
void add_obj_common_to_info_box(info_box_t* box);
bool set_obj_common_at_info_box(info_box_t* box, const trc_obj_rev_head_t* head, uint64_t revision);
void add_custom_to_info_box(info_box_t* box, const char* name, GtkWidget* widget);
#endif
