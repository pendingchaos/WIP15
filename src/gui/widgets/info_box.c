#include "info_box.h"
#include "../utils.h"

#include <string.h>
#include <stdlib.h>

info_box_t* create_info_box() {
    info_box_t* box = malloc(sizeof(info_box_t));
    box->widget = gtk_grid_new();
    gtk_grid_set_row_spacing(GTK_GRID(box->widget), 5);
    gtk_grid_set_column_spacing(GTK_GRID(box->widget), 12);
    box->entry_count = 0;
    return box;
}

void free_info_box(info_box_t* box) {
    free(box);
}

void clear_info_box(info_box_t* box) {
    for (size_t i = 0; i < box->entry_count; i++)
        gtk_grid_remove_row(GTK_GRID(box->widget), 0);
    box->entry_count = 0;
}

void add_to_info_box(info_box_t* box, const char* name) {
    GtkWidget* name_widget = create_dim_label(name);
    GtkWidget* value_widget = gtk_label_new("");
    
    gtk_widget_set_hexpand(value_widget, true);
    
    gtk_widget_set_halign(name_widget, GTK_ALIGN_END);
    gtk_widget_set_halign(value_widget, GTK_ALIGN_START);
    
    gtk_grid_attach(GTK_GRID(box->widget), name_widget, 0, box->entry_count, 1, 1);
    gtk_grid_attach(GTK_GRID(box->widget), value_widget, 1, box->entry_count, 1, 1);
    
    box->entry_count++;
}

void add_multiple_to_info_box(info_box_t* box, ...) {
    va_list list;
    va_start(list, box);
    while (true) {
        const char* name = va_arg(list, const char*);
        if (!name) break;
        add_to_info_box(box, name);
    }
    va_end(list);
}

void add_separator_to_info_box(info_box_t* box) {
    GtkWidget* sep = gtk_separator_new(GTK_ORIENTATION_HORIZONTAL);
    gtk_grid_attach(GTK_GRID(box->widget), sep, 0, box->entry_count, 2, 1);
    box->entry_count++;
    
    gtk_widget_set_hexpand(sep, true);
    //gtk_widget_set_vexpand(sep, true);
}

void set_at_info_box(info_box_t* box, const char* name, const char* fmt, ...) {
    va_list list, list2;
    va_start(list, fmt);
    va_copy(list2, list);
    size_t value_size = vsnprintf(NULL, 0, fmt, list) + 1;
    char* value = calloc(value_size, 1);
    vsnprintf(value, value_size, fmt, list2);
    va_end(list);
    va_end(list2);
    
    for (size_t i = 0; i < box->entry_count; i++) {
        GtkWidget* label = gtk_grid_get_child_at(GTK_GRID(box->widget), 0, i);
        if (!GTK_IS_LABEL(label)) continue;
        if (strcmp(gtk_label_get_text(GTK_LABEL(label)), name) != 0) continue;
        label = gtk_grid_get_child_at(GTK_GRID(box->widget), 1, i);
        gtk_label_set_text(GTK_LABEL(label), value);
        break;
    }
    
    free(value);
}

void set_enum_at_info_box(info_box_t* box, const char* name, const char* group, uint val) {
    set_at_info_box(box, name, "%s", get_enum_str(group, val));
}

void set_visible_at_info_box(info_box_t* box, const char* name, bool visible) {
    for (size_t i = 0; i < box->entry_count; i++) {
        GtkWidget* label = gtk_grid_get_child_at(GTK_GRID(box->widget), 0, i);
        if (!GTK_IS_LABEL(label)) continue;
        if (strcmp(gtk_label_get_text(GTK_LABEL(label)), name) != 0) continue;
        gtk_widget_set_visible(label, visible);
        label = gtk_grid_get_child_at(GTK_GRID(box->widget), 1, i);
        gtk_widget_set_visible(label, visible);
    }
}

void add_obj_common_to_info_box(info_box_t* box) {
    add_to_info_box(box, "Revision");
    add_to_info_box(box, "Reference Count");
    add_to_info_box(box, "Replay Pointer");
    add_to_info_box(box, "Name");
    add_to_info_box(box, "Label");
    add_separator_to_info_box(box);
    add_custom_to_info_box(box, "This name has not yet been bound", NULL);
}

bool set_obj_common_at_info_box(info_box_t* box, const trc_obj_rev_head_t* head, uint64_t revision) {
    set_at_info_box(box, "Revision", "%lu", head->revision);
    set_at_info_box(box, "Reference Count", "%u", head->ref_count);
    set_at_info_box(box, "Replay Pointer", "%p", head->obj);
    if (head->has_name)
        set_at_info_box(box, "Name", static_format_obj(head->obj, revision));
    else
        set_at_info_box(box, "Name", "");
    
    bool has_obj = true;
    switch (head->obj->type) {
    case TrcBuffer: has_obj = ((trc_gl_buffer_rev_t*)head)->has_object; break;
    case TrcTexture: has_obj = ((trc_gl_texture_rev_t*)head)->has_object; break;
    case TrcQuery: has_obj = ((trc_gl_query_rev_t*)head)->has_object; break;
    case TrcFramebuffer: has_obj = ((trc_gl_framebuffer_rev_t*)head)->has_object; break;
    case TrcRenderbuffer: has_obj = ((trc_gl_renderbuffer_rev_t*)head)->has_object; break;
    case TrcProgramPipeline: has_obj = ((trc_gl_program_pipeline_rev_t*)head)->has_object; break;
    case TrcVAO: has_obj = ((trc_gl_vao_rev_t*)head)->has_object; break;
    case TrcTransformFeedback: has_obj = ((trc_gl_transform_feedback_rev_t*)head)->has_object; break;
    default: break;
    }
    
    //the widget at index 6 is the "This name has not yet been bound" label
    for (size_t i = 6; i < box->entry_count; i++) {
        GtkWidget* widget = gtk_grid_get_child_at(GTK_GRID(box->widget), 0, i);
        gtk_widget_set_visible(widget, i==6?!has_obj:has_obj);
        widget = gtk_grid_get_child_at(GTK_GRID(box->widget), 1, i);
        gtk_widget_set_visible(widget, i==6?!has_obj:has_obj);
    }
    
    return has_obj;
}

void add_custom_to_info_box(info_box_t* box, const char* name, GtkWidget* widget) {
    if (widget) gtk_widget_set_hexpand(widget, true);
    
    if (name) {
        GtkWidget* name_widget = create_dim_label(name);
        
        if (widget) gtk_widget_set_halign(name_widget, GTK_ALIGN_END);
        if (widget) gtk_widget_set_halign(widget, GTK_ALIGN_START);
        
        gtk_grid_attach(GTK_GRID(box->widget), name_widget, 0, box->entry_count, widget?1:2, 1);
        if (widget) gtk_grid_attach(GTK_GRID(box->widget), widget, 1, box->entry_count, 1, 1);
    } else {
        gtk_grid_attach(GTK_GRID(box->widget), widget, 0, box->entry_count, 2, 1);
    }
    
    box->entry_count++;
}
