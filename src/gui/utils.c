#include "utils.h"

#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <inttypes.h>

extern trace_t* trace;
extern int64_t revision;

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
        if (!group) return "(unable to find string)";
        
        for (size_t i = 0; i < group->entry_count; i++) {
            const glapi_group_entry_t *entry = group->entries[i];
            if (entry->value == val)
                return entry->name;
        }
        
        return "(unable to find string)";
    }
}

const char* static_format(const char* format, ...) {
    static char data[1024];
    memset(data, 0, 1024);
    
    va_list list;
    va_start(list, format);
    vsnprintf(data, 1024, format, list);
    va_end(list);
    
    return data;
}

const char* format_float(double val) {
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

static void cat_str(char* buf, const char* src, size_t buf_size) {
    char* dest = buf;
    if (buf_size > 1) {
        while (*dest) dest++;
        while (*src && dest-buf < buf_size-1)
            *dest++ = *src++;
    }
    *dest++ = 0;
}

void format_value(trace_t* trace, char* str, trace_value_t value, size_t n) {
    if (value.group_index < 0 ? false : (trace->group_names[value.group_index][0] != 0)) {
        const glapi_group_t* group = find_group(trace->group_names[value.group_index]);
        
        uint64_t val = (value.type==Type_Boolean) ?
                        *trc_get_bool(&value) :
                        *trc_get_uint(&value);
        
        if (!group) {
        } else if (group->bitmask) {
            //TODO
        } else {
            for (size_t i = 0; i < group->entry_count; i++) {
                const glapi_group_entry_t *entry = group->entries[i];
                
                if (entry->value == val) {
                    cat_str(str, entry->name, n);
                    return;
                }
            }
        }
    }
    
    if (value.count != 1) cat_str(str, "[", n);
    for (size_t i = 0; i < value.count; ++i) {
        switch (value.type) {
        case Type_Void: {
            cat_str(str, "void", n);
            break;
        }
        case Type_UInt: {
            cat_str(str, static_format("%"PRIu64, trc_get_uint(&value)[i]), n);
            break;
        }
        case Type_Int: {
            cat_str(str, static_format("%"PRId64, trc_get_int(&value)[i]), n);
            break;
        }
        case Type_Double: {
            cat_str(str, static_format("%s", format_float(trc_get_double(&value)[i])), n);
            break;
        }
        case Type_Boolean: {
            cat_str(str, static_format(trc_get_bool(&value)[i] ? "true" : "false"), n);
            break;
        }
        case Type_Str: {
            const char* strval = trc_get_str(&value)[i];
            bool multiline = false;
            for (const char* c = strval; *c; c++) multiline |= *c == '\n';
            if (multiline) cat_str(str, "...", n);
            else cat_str(str, static_format("'%s'", strval), n);
            break;
        }
        case Type_FunctionPtr: {
            cat_str(str, static_format("<function pointer>"), n);
            break;
        }
        case Type_Ptr: {
            cat_str(str, static_format("0x%"PRIx64, trc_get_ptr(&value)[i]), n);
            break;
        }
        case Type_Data: {
            cat_str(str, static_format("<data>"), n);
            break;
        }
        }
        
        if (i != value.count-1) cat_str(str, ", ", n);
    }
    if (value.count != 1) cat_str(str, "]", n);
}

static void format_cmd(trace_t* trace, char* str, trace_command_t* cmd, size_t n, bool ext) {
    *str = 0;
    
    cat_str(str, static_format("%s(", trace->func_names[cmd->func_index]), n);
    
    char* func_name = trace->func_names[cmd->func_index];
    glapi_function_t* func = NULL;
    for (size_t i = 0; ext && i<glapi.function_count; i++) {
        if (strcmp((func=glapi.functions[i])->name, func_name) == 0)
            break;
    }
    
    for (size_t i = 0; i < cmd->arg_count; i++) {
        if (ext) {
            cat_str(str, func->args[i]->name, n);
            cat_str(str, "=", n);
        }
        format_value(trace, str, cmd->args[i], n);
        if (i != cmd->arg_count-1)
            cat_str(str, static_format(", "), n);
    }
    
    cat_str(str, ")", n);
    
    if (cmd->ret.type != Type_Void) {
        cat_str(str, " = ", n);
        format_value(trace, str, cmd->ret, n);
    }
}

void format_command(trace_t* trace, char* str, trace_command_t* cmd, size_t n) {
    format_cmd(trace, str, cmd, n, false);
}

void format_command_ext(trace_t* trace, char* str, trace_command_t* cmd, size_t n) {
    format_cmd(trace, str, cmd, n, true);
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

void fmt_object_id(char* buf, size_t buf_size, const trc_obj_rev_head_t* rev) {
    memset(buf, 0, buf_size);
    if (!buf_size) return;
    if (rev->has_name) snprintf(buf, buf_size-1, "%lu", rev->name);
    else snprintf(buf, buf_size-1, "0x%lx", (uint64_t)(uintptr_t)rev->obj); //TODO: Show past name
}

void create_obj_list(GtkTreeStore* store, trc_obj_type_t type) {
    const trc_obj_rev_head_t* rev;
    for (size_t i = 0; trc_iter_objects(trace, type, &i, revision, (const void**)&rev);) {
        char str[64];
        fmt_object_id(str, 64, rev);
        
        GtkTreeIter row;
        gtk_tree_store_append(store, &row, NULL);
        gtk_tree_store_set(store, &row, 0, str, -1);
    }
}
