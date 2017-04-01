#include "libtrace/libtrace.h"
#include "utils.h"

#include <endian.h>
#include <gtk/gtk.h>
#include <stdint.h>
#include <stdbool.h>
#include <assert.h>
#include <string.h>
#include <inttypes.h>

#define TYPE_FLOAT16 0
#define TYPE_FLOAT32 1
#define TYPE_FLOAT64 2
#define TYPE_UINT8 3
#define TYPE_INT8 4
#define TYPE_UINT16 5
#define TYPE_INT16 6
#define TYPE_UINT32 7
#define TYPE_INT32 8
#define TYPE_UINT64 9
#define TYPE_INT64 10

extern GtkBuilder* builder;
extern trace_t* trace;
extern int64_t revision;

static void update_buffer_view(size_t buf_index) {
    GtkSpinButton* stride_button = GTK_SPIN_BUTTON(gtk_builder_get_object(builder, "stride"));
    GtkSpinButton* offset_button = GTK_SPIN_BUTTON(gtk_builder_get_object(builder, "offset"));
    GtkSpinButton* components_button = GTK_SPIN_BUTTON(gtk_builder_get_object(builder, "components"));
    GtkComboBox* type_combobox = GTK_COMBO_BOX(gtk_builder_get_object(builder, "type_combobox"));
    
    uint32_t stride = gtk_spin_button_get_value(GTK_SPIN_BUTTON(stride_button));
    uint32_t offset = gtk_spin_button_get_value(GTK_SPIN_BUTTON(offset_button));
    uint32_t components = gtk_spin_button_get_value(GTK_SPIN_BUTTON(components_button));
    gint type = gtk_combo_box_get_active(GTK_COMBO_BOX(type_combobox));
    
    GtkTreeView* content = GTK_TREE_VIEW(gtk_builder_get_object(builder, "buffer_content_treeview"));
    GtkTreeStore* store = GTK_TREE_STORE(gtk_tree_view_get_model(content));
    gtk_tree_store_clear(store);
    
    if (type < 0) return;
    
    size_t type_size = 0;
    switch (type) {
    case TYPE_UINT8:
    case TYPE_INT8: {
        stride = stride ? stride : components;
        type_size = 1;
        break;
    }
    case TYPE_UINT16:
    case TYPE_INT16:
    case TYPE_FLOAT16: {
        stride = stride ? stride : components*2;
        type_size = 2;
        break;
    }
    case TYPE_UINT32:
    case TYPE_INT32:
    case TYPE_FLOAT32: {
        stride = stride ? stride : components*4;
        type_size = 4;
        break;
    }
    case TYPE_UINT64:
    case TYPE_INT64:
    case TYPE_FLOAT64: {
        stride = stride ? stride : components*8;
        type_size = 8;
        break;
    }
    default: {
        assert(false && "Unreachable code has been reached.");
    }
    }
    
    size_t count = 0;
    trc_gl_buffer_rev_t* buf = NULL;
    for (size_t i = 0; i <= trace->inspection.gl_obj_history_count[TrcGLObj_Buffer]; i++) {
        trc_gl_obj_history_t* h = &trace->inspection.gl_obj_history[TrcGLObj_Buffer][i];
        buf = (trc_gl_buffer_rev_t*)trc_lookup_gl_obj(trace, revision, h->fake, TrcGLObj_Buffer);
        if (buf && buf->ref_count) count++;
        if (count == buf_index+1) break;
    }
    
    if (!buf) return; //TODO: Is this possible?
    if (!buf->data) return;
    
    uint8_t* data = trc_lock_data(buf->data, true, false);
    size_t buf_size = buf->data->uncompressed_size;
    while (offset+type_size <= buf_size) {
        GtkTreeIter row;
        gtk_tree_store_append(store, &row, NULL);
        
        char str[1024];
        memset(str, 0, 1024);
        
        char* cur = str;
        char* end = str+1024;
        
        if (components>1)
            cur += snprintf(cur, end-cur, "[");
        
        for (size_t i = 0; i < components; i++) {
            bool at_end = offset+type_size*i+type_size > buf_size;
            if (at_end) break;
            if ((i != 0)) cur += snprintf(cur, end-cur, ", ");
            switch (type) {
            case TYPE_UINT8: {
                cur += snprintf(cur, end-cur, "%"PRIu8, ((uint8_t*)(data+offset))[i]);
                break;
            }
            case TYPE_INT8: {
                cur += snprintf(cur, end-cur, "%"PRId8, ((int8_t*)(data+offset))[i]);
                break;
            }
            case TYPE_UINT16: {
                cur += snprintf(cur, end-cur, "%"PRIu16, ((uint16_t*)(data+offset))[i]);
                break;
            }
            case TYPE_INT16: {
                cur += snprintf(cur, end-cur, "%"PRId16, ((int16_t*)(data+offset))[i]);
                break;
            }
            case TYPE_UINT32: {
                cur += snprintf(cur, end-cur, "%"PRIu32, ((uint32_t*)(data+offset))[i]);
                break;
            }
            case TYPE_INT32: {
                cur += snprintf(cur, end-cur, "%"PRId32, ((int32_t*)(data+offset))[i]);
                break;
            }
            case TYPE_UINT64: {
                cur += snprintf(cur, end-cur, "%"PRIu64, ((uint64_t*)(data+offset))[i]);
                break;
            }
            case TYPE_INT64: {
                cur += snprintf(cur, end-cur, "%"PRId64, ((int64_t*)(data+offset))[i]);
                break;
            }
            case TYPE_FLOAT16: {
                //TODO
                break;
            }
            case TYPE_FLOAT32: {
                cur += snprintf(cur, end-cur, "%s", format_float(((float*)(data+offset))[i]));
                break;
            }
            case TYPE_FLOAT64: {
                cur += snprintf(cur, end-cur, "%s", format_float(((double*)(data+offset))[i]));
                break;
            }
            }
        }
        
        offset += stride;
        
        if (components>1)
            cur += snprintf(cur, end-cur, "]");
        
        gtk_tree_store_set(store, &row, 0, str, -1);
    }
    
    trc_unlock_data(buf->data);
}

void update_buffer_view_callback(GObject* obj, gpointer user_data) {
    GObject* buf_tree_view = gtk_builder_get_object(builder, "buffers_treeview");
    GtkTreePath* path;
    gtk_tree_view_get_cursor(GTK_TREE_VIEW(buf_tree_view), &path, NULL);
    if (!path) return;
    
    //Initialize params
    size_t index = gtk_tree_path_get_indices(path)[0];
    
    update_buffer_view(index);
}

void init_buffer_list(GtkTreeView* tree) {
    GtkTreeView* content = GTK_TREE_VIEW(gtk_builder_get_object(builder, "buffer_content_treeview"));
    GtkTreeStore* store = GTK_TREE_STORE(gtk_tree_view_get_model(content));
    gtk_tree_store_clear(store);
    
    store = GTK_TREE_STORE(gtk_tree_view_get_model(tree));
    gtk_tree_store_clear(store);
    
    for (size_t i = 0; i < trace->inspection.gl_obj_history_count[TrcGLObj_Buffer]; i++) {
        trc_gl_obj_history_t* h = &trace->inspection.gl_obj_history[TrcGLObj_Buffer][i];
        const trc_gl_obj_rev_t* buf = trc_lookup_gl_obj(trace, revision, h->fake, TrcGLObj_Buffer);
        if (buf && buf->ref_count) {
            char str[64];
            memset(str, 0, 64);
            snprintf(str, 64, "%u", (uint)h->fake);
            
            GtkTreeIter row;
            gtk_tree_store_append(store, &row, NULL);
            gtk_tree_store_set(store, &row, 0, str, -1);
        }
    }
}

void buffer_init() {
    init_treeview(builder, "buffers_treeview", 1);
    init_treeview(builder, "buffer_content_treeview", 1);
    
    //Initialize buffer type combobox
    GObject* buf_type = gtk_builder_get_object(builder, "type_combobox");
    GtkTreeStore* buf_type_store = gtk_tree_store_new(1, G_TYPE_STRING);
    gtk_combo_box_set_model(GTK_COMBO_BOX(buf_type), GTK_TREE_MODEL(buf_type_store));
    GtkCellRenderer* renderer = gtk_cell_renderer_text_new();
    gtk_cell_layout_pack_start(GTK_CELL_LAYOUT(buf_type), renderer, TRUE);
    gtk_cell_layout_set_attributes(GTK_CELL_LAYOUT(buf_type), renderer, "text", 0, NULL);
    GtkTreeIter row;
    const char* strs[] = {"Float16", "Float32", "Float64", "Uint8", "Int8", "Uint16", "Int16", "Uint32", "Int32", "Uint64", "Int64"};
    for (size_t i = 0; i < sizeof(strs)/sizeof(strs[0]); i++) {
        gtk_tree_store_append(buf_type_store, &row, NULL);
        gtk_tree_store_set(buf_type_store, &row, 0, strs[i], -1);
    }
    g_object_unref(buf_type_store);
    gtk_combo_box_set_active(GTK_COMBO_BOX(buf_type), TYPE_FLOAT32);
    
    //Initialize buffer stride and offset
    GtkSpinButton* button = GTK_SPIN_BUTTON(gtk_builder_get_object(builder, "stride"));
    gtk_adjustment_set_upper(gtk_spin_button_get_adjustment(button), 4294967295);
    button = GTK_SPIN_BUTTON(gtk_builder_get_object(builder, "offset"));
    gtk_adjustment_set_upper(gtk_spin_button_get_adjustment(button), 4294967295);
    button = GTK_SPIN_BUTTON(gtk_builder_get_object(builder, "components"));
    gtk_adjustment_set_lower(gtk_spin_button_get_adjustment(button), 1);
    gtk_adjustment_set_upper(gtk_spin_button_get_adjustment(button), 4294967295);
}
