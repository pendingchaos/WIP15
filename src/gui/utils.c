#include "utils.h"

#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <inttypes.h>

const glapi_group_t* find_group(const char* name) {
    for (size_t i = 0; i < glapi.group_count; i++)
        if (strcmp(glapi.groups[i]->name, name) == 0)
            return glapi.groups[i];
    
    return NULL;
}

const char* get_enum_str(const char* group_name, uint val) {
    if (!group_name) {
        for (size_t i = 0; i < glapi.group_count; i++) {
            const glapi_group_t* group = glapi.groups[i];
            
            for (size_t i = 0; i < group->entry_count; i++) {
                const glapi_group_entry_t *entry = group->entries[i];
                if (entry->value == val)
                    return entry->name;
            }
        }
        
        return "(unable to find string)";
    } else {
        const glapi_group_t* group = find_group(group_name);
        
        if (!group)
            return "(unable to find string)";
        
        for (size_t i = 0; i < group->entry_count; i++) {
            const glapi_group_entry_t *entry = group->entries[i];
            if (entry->value == val)
                return entry->name;
        }
        
        return "(unable to find string)";
    }
}

char* static_format(const char* format, ...) {
    static char data[1024];
    memset(data, 0, 1024);
    
    va_list list;
    va_start(list, format);
    vsnprintf(data, 1024, format, list);
    va_end(list);
    
    return data;
}

char* format_float(double val) {
    static char data[128];
    memset(data, 0, 128);
    
    snprintf(data, 128, "%f", val);
    
    char* c = data+127;
    while (true) {
        if (*c == '0') {
            *c = 0;
        } else if (*c == '.') {
            *c = 0;
            break;
        } else if (*c) {
            break;
        }
        c--;
    }
    
    return data;
}

void format_value(trace_t* trace, char* str, trace_value_t value, size_t n) {
    if (value.group_index < 0 ? false : (trace->group_names[value.group_index][0] != 0)) {
        const glapi_group_t* group = find_group(trace->group_names[value.group_index]);
        
        uint64_t val = (value.type==Type_Boolean) ?
                        *trace_get_bool(&value) :
                        *trace_get_uint(&value);
        
        if (!group) {
        } else if (group->bitmask) {
            //TODO
        } else {
            for (size_t i = 0; i < group->entry_count; i++) {
                const glapi_group_entry_t *entry = group->entries[i];
                if (entry->value == val) {
                    strncat(str, static_format("%s(%zu)", entry->name, val), n);
                    return;
                }
            }
        }
    }
    
    if (value.count > 1) strncat(str, "[", n);
    for (size_t i = 0; i < value.count; ++i) {
        switch (value.type) {
        case Type_Void: {
            strncat(str, "void", n);
            break;
        }
        case Type_UInt: {
            strncat(str, static_format("%"PRIu64, trace_get_uint(&value)[i]), n);
            break;
        }
        case Type_Int: {
            strncat(str, static_format("%"PRId64, trace_get_int(&value)[i]), n);
            break;
        }
        case Type_Double: {
            strncat(str, static_format("%s", format_float(trace_get_double(&value)[i])), n);
            break;
        }
        case Type_Boolean: {
            strncat(str, static_format(trace_get_bool(&value)[i] ? "true" : "false"), n);
            break;
        }
        case Type_Str: {
            strncat(str, static_format("'%s'", trace_get_str(&value)[i]), n);
            break;
        }
        case Type_Bitfield: {
            strncat(str, static_format("%u", trace_get_bitfield(&value)[i]), n);
            break;
        }
        case Type_FunctionPtr: {
            strncat(str, static_format("<function pointer>"), n);
            break;
        }
        case Type_Ptr: {
            strncat(str, static_format("0x%"PRIx64, trace_get_ptr(&value)[i]), n);
            break;
        }
        case Type_Data: {
            strncat(str, static_format("<data>"), n);
            break;
        }
        }
        
        if (i != value.count-1)
            strncat(str, ", ", n);
    }
    if (value.count > 1) strncat(str, "]", n);
}

void format_command(trace_t* trace, char* str, inspect_command_t* command, size_t n) {
    trace_command_t* trace_cmd = command->trace_cmd;
    
    strncat(str, static_format("%s(", command->name), n);
    
    trace_val_vec_t args = command->trace_cmd->args;
    size_t count = get_trace_val_vec_count(args);
    for (size_t i = 0; i < count; i++) {
        format_value(trace, str, *get_trace_val_vec(args, i), n);
        if (i != count-1) {
            strncat(str, static_format(", "), n);
        }
    }
    
    strncat(str, ")", n);
    
    if (trace_cmd->ret.type != Type_Void) {
        strncat(str, " = ", n);
        format_value(trace, str, trace_cmd->ret, n);
    }
}

void init_treeview(GtkBuilder* builder, const char* name, size_t column_count) {
    GObject* view = gtk_builder_get_object(builder, name);
    
    GType types[column_count];
    for (size_t i = 0; i < column_count; i++)
        types[i] = G_TYPE_STRING;
    
    GtkTreeStore* store = gtk_tree_store_newv(column_count, types);
    
    gtk_tree_view_set_model(GTK_TREE_VIEW(view),
                            GTK_TREE_MODEL(store));
    g_object_unref(store);
    
    GtkCellRenderer* renderer = gtk_cell_renderer_text_new();
    
    for (size_t i = 0; i < column_count; i++) {
        GtkTreeViewColumn* column = gtk_tree_view_get_column(GTK_TREE_VIEW(view), i);
        gtk_tree_view_column_pack_start(column, renderer, FALSE);
        gtk_tree_view_column_set_attributes(column, renderer, "text", i, NULL);
    }
}
