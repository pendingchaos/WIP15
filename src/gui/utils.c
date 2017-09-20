#include "gui.h"
#include "utils.h"

#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
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

void cat_str(char* buf, const char* src, size_t buf_size) {
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
            bool first = true;
            for (size_t i = 0; i < group->entry_count; i++) {
                const glapi_group_entry_t* entry = group->entries[i];
                if (entry->value & val) {
                    if (!first) cat_str(str, "|", n);
                    cat_str(str, entry->name, n);
                    first = false;
                    val &= ~entry->value;
                }
            }
            if (val && !first) cat_str(str, "|", n);
            if (val) cat_str(str, static_format("0x"PRIx64"%", val), n);
            return;
        } else {
            for (size_t i = 0; i < group->entry_count; i++) {
                const glapi_group_entry_t* entry = group->entries[i];
                
                if (entry->value == val) {
                    cat_str(str, entry->name, n);
                    return;
                }
            }
        }
    }
    
    if (value.is_array) cat_str(str, "[", n);
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
            cat_str(str, static_format("%g", trc_get_double(&value)[i]), n);
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
    if (value.is_array) cat_str(str, "]", n);
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

void init_treeview(void* view_, size_t column_count) {
    GtkTreeView* view = GTK_TREE_VIEW(view_);
    
    GType types[column_count];
    for (size_t i = 0; i < column_count; i++)
        types[i] = G_TYPE_STRING;
    
    GtkTreeStore* store = gtk_tree_store_newv(column_count, types);
    
    gtk_tree_view_set_model(view, GTK_TREE_MODEL(store));
    g_object_unref(store);
    
    GtkCellRenderer* renderer = gtk_cell_renderer_text_new();
    
    for (size_t i = 0; i < column_count; i++) {
        GtkTreeViewColumn* column = gtk_tree_view_get_column(view, i);
        gtk_tree_view_column_pack_start(column, renderer, FALSE);
        gtk_tree_view_column_set_attributes(column, renderer, "text", i, NULL);
    }
}

void init_combobox(void* box_, const char** labels) {
    GtkComboBox* box = GTK_COMBO_BOX(box_);
    
    GtkTreeStore* store = gtk_tree_store_new(2, G_TYPE_STRING, G_TYPE_POINTER);
    gtk_combo_box_set_model(box, GTK_TREE_MODEL(store));
    GtkCellRenderer* renderer = gtk_cell_renderer_text_new();
    gtk_cell_layout_pack_start(GTK_CELL_LAYOUT(box), renderer, TRUE);
    gtk_cell_layout_set_attributes(GTK_CELL_LAYOUT(box), renderer, "text", 0, NULL);
    GtkTreeIter row;
    for (size_t i = 0; ; i++) {
        if (!labels[i]) break;
        gtk_tree_store_append(store, &row, NULL);
        gtk_tree_store_set(store, &row, 0, labels[i], 1, NULL, -1);
    }
    g_object_unref(store);
    gtk_combo_box_set_active(box, 0);
}

void fmt_object_id(char* buf, size_t buf_size, const trc_obj_rev_head_t* rev) {
    memset(buf, 0, buf_size);
    if (!buf_size) return;
    if (rev->has_name) {
        if (rev->obj->type==TrcContext || rev->obj->type==TrcSync)
            snprintf(buf, buf_size-1, "0x%lx", rev->name);
        else
            snprintf(buf, buf_size-1, "%lu", rev->name);
    } else if (rev->has_had_name) {
        if (rev->obj->type==TrcContext || rev->obj->type==TrcSync)
            snprintf(buf, buf_size-1, "<%p> (was 0x%lx)", rev->obj, rev->old_name);
        else
            snprintf(buf, buf_size-1, "<%p> (was %lu)", rev->obj, rev->old_name);
    } else {
        snprintf(buf, buf_size-1, "<%p>", rev->obj);
    }
}

const char* static_format_obj(trc_obj_t* obj, uint64_t revision) {
    static char id_str[64];
    id_str[0] = 0;
    if (obj) {
        const void* rev = trc_obj_get_rev(obj, revision);
        if (rev) fmt_object_id(id_str, 64, rev);
        else snprintf(id_str, sizeof(id_str)-1, "<%p>", obj);
    }
    return id_str;
}

void display_error_dialog(const char* message) {
    GtkWidget* dialog = gtk_message_dialog_new(GTK_WINDOW(state.main_window),
                                               GTK_DIALOG_DESTROY_WITH_PARENT|GTK_DIALOG_MODAL,
                                               GTK_MESSAGE_ERROR,
                                               GTK_BUTTONS_CLOSE,
                                               "%s",
                                               message);
    gtk_widget_show_all(dialog);
    g_signal_connect_swapped(dialog, "response", G_CALLBACK(gtk_widget_destroy), dialog);
}

char* run_file_dialog(const char* title, const char* button, GtkFileChooserAction action) {
    GtkWidget* dialog = gtk_file_chooser_dialog_new(
        title, GTK_WINDOW(state.main_window), action,
        "Cancel", GTK_RESPONSE_CANCEL, button, GTK_RESPONSE_ACCEPT, NULL);
    char* filename = NULL;
    if (gtk_dialog_run(GTK_DIALOG(dialog)) == GTK_RESPONSE_ACCEPT)
        filename = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(dialog));
    gtk_widget_destroy(dialog);
    return filename;
}

GtkComboBox* create_combobox(int initial_value, const char** labels) {
    GtkComboBox* res = GTK_COMBO_BOX(gtk_combo_box_new());
    init_combobox(res, labels);
    gtk_combo_box_set_active(res, initial_value);
    return res;
}

GtkSpinButton* create_integral_spin_button(int64_t min, int64_t max, void* callback, void* data) {
    GtkSpinButton* res = GTK_SPIN_BUTTON(gtk_spin_button_new_with_range(min, max, 1.0));
    if (callback) g_signal_connect(res, "value-changed", G_CALLBACK(callback), data);
    return res;
}

GtkWidget* create_button(const char* label, void* callback, void* data) {
    GtkWidget* res = gtk_button_new_with_label(label);
    if (callback) g_signal_connect(res, "clicked", G_CALLBACK(callback), data);
    return res;
}

GtkWidget* create_dim_label(const char* text) {
    GtkWidget* label = gtk_label_new(text);
    gtk_style_context_add_class(gtk_widget_get_style_context(label), "dim-label");
    return label;
}

GtkWidget* create_box(bool vertical, size_t count, ...) {
    GtkWidget* res;
    if (vertical) res = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
    else res = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 12);
    
    va_list list;
    va_start(list, count);
    for (size_t i = 0; i < count; i++) {
        GtkWidget* child = GTK_WIDGET(va_arg(list, void*));
        gtk_box_pack_start(GTK_BOX(res), child, false, false, 0);
    }
    va_end(list);
    
    return res;
}

GtkTreeView* create_tree_view(size_t column_count, ...) {
    GtkTreeView* view = GTK_TREE_VIEW(gtk_tree_view_new());
    va_list list;
    va_start(list, column_count);
    for (size_t i = 0; i < column_count; i++) {
        GtkTreeViewColumn* column = gtk_tree_view_column_new();
        gtk_tree_view_column_set_title(column, va_arg(list, const char*));
        gtk_tree_view_column_set_resizable(column, true);
        gtk_tree_view_append_column(view, column);
    }
    va_end(list);
    gtk_widget_set_vexpand(GTK_WIDGET(view), true);
    init_treeview(view, column_count);
    return view;
}

GtkWidget* create_scrolled_window(GtkWidget* widget) {
    GtkWidget* scrolled_window = gtk_scrolled_window_new(NULL, NULL);
    gtk_container_add(GTK_CONTAINER(scrolled_window), widget);
    return scrolled_window;
}
