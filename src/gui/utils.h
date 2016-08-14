#ifndef UTILS_H
#define UTILS_H
#include "libtrace/libtrace.h"
#include "shared/glapi.h"

#include <gtk/gtk.h>

const glapi_group_t* find_group(const char* name);
const char* get_enum_str(const char* group_name, uint val);
char* static_format(const char* format, ...);
char* format_float(double val);
void format_value(trace_t* trace, char* str, trace_value_t value, size_t n);
void format_command(trace_t* trace, char* str, trace_command_t* command, size_t n);
void init_treeview(GtkBuilder* builder, const char* name, size_t column_count);
#endif
