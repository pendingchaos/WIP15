#ifndef UTILS_H
#define UTILS_H
#include "widgets/image_viewer.h"
#include "widgets/info_box.h"
#include "widgets/object_notebook.h"
#include "libtrace/libtrace.h"
#include "shared/glapi.h"

#include <stdbool.h>
#include <gtk/gtk.h>

#define VISIBLE __attribute__((visibility("default")))

const glapi_group_t* find_group(const char* name);
const char* get_enum_str(const char* group_name, uint val);
const char* static_format(const char* format, ...);
void cat_str(char* buf, const char* src, size_t buf_size);
void format_value(trace_t* trace, char* str, trace_value_t value, size_t n);
void format_command(trace_t* trace, char* str, trace_command_t* command, size_t n);
void format_command_ext(trace_t* trace, char* str, trace_command_t* command, size_t n);
void init_treeview(void* view, size_t column_count);
void init_combobox(void* box, const char** labels);
void fmt_object_id(char* buf, size_t buf_size, const trc_obj_rev_head_t* rev);
const char* static_format_obj(trc_obj_t* obj, uint64_t revision);
void display_error_dialog(const char* message);
char* run_file_dialog(const char* title, const char* button, GtkFileChooserAction action);

GtkComboBox* create_combobox(int initial_value, const char** labels);
GtkSpinButton* create_integral_spin_button(int64_t min, int64_t max, void* callback, void* data);
GtkWidget* create_button(const char* label, void* callback, void* data);
GtkWidget* create_dim_label(const char* text);
GtkWidget* create_box(bool vertical, size_t count, ...);
GtkTreeView* create_tree_view(size_t column_count, ...);
GtkWidget* create_scrolled_window(GtkWidget* widget);
#endif
