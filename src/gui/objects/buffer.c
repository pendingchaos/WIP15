#include "../gui.h"
#include "../utils.h"
#include "../simple_store.h"

#include <stdlib.h>
#include "shared/glcorearb.h"
#include <math.h>

//Matches the type combobox
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

typedef struct buffer_data_t {
    GtkSpinButton* data_offset;
    GtkSpinButton* data_stride;
    GtkSpinButton* data_components;
    GtkComboBox* data_type;
    GtkTreeView* data_view;
    GtkTreeModel* data_store;
    
    int view_type;
    size_t view_type_size;
    size_t view_components;
    size_t view_stride;
    size_t data_size;
    uint8_t* data;
} buffer_data_t;

static const char* format_component(int type, const void* data) {
    switch (type) {
    case TYPE_UINT8:
        return static_format("%"PRIu8, *(uint8_t*)data);
    case TYPE_INT8:
        return static_format("%"PRId8, *(int8_t*)data);
    case TYPE_UINT16:
        return static_format("%"PRIu16, *(uint16_t*)data);
    case TYPE_INT16:
        return static_format("%"PRId16, *(int16_t*)data);
    case TYPE_UINT32:
        return static_format("%"PRIu32, *(uint32_t*)data);
    case TYPE_INT32:
        return static_format("%"PRId32, *(int32_t*)data);
    case TYPE_UINT64:
        return static_format("%"PRIu64, *(uint64_t*)data);
    case TYPE_INT64:
        return static_format("%"PRId64, *(int64_t*)data);
    case TYPE_FLOAT16:
        return static_format("%g", parse_f16(*(uint16_t*)data));
    case TYPE_FLOAT32:
        return static_format("%g", *(float*)data);
    case TYPE_FLOAT64:
        return static_format("%g", *(double*)data);
    }
    return "";
}

static void get_function(size_t index, size_t row, GValue* val, void* udata) {
    if (index != 0) return;
    
    buffer_data_t* buf_data = udata;
    
    int type = buf_data->view_type;
    size_t type_size = buf_data->view_type_size;
    size_t components = buf_data->view_components;
    const uint8_t* data = buf_data->data + buf_data->view_stride*row;
    
    char str[1024] = {0};
    
    if (components>1) cat_str(str, "[", sizeof(str));
    
    for (size_t i = 0; i < components; i++) {
        size_t left = data - buf_data->data;
        if (left+type_size > buf_data->data_size) break;
        if (i) cat_str(str, ",", sizeof(str));
        cat_str(str, format_component(type, data), sizeof(str));
        data += type_size;
    }
    
    if (components>1) cat_str(str, "]", sizeof(str));
    
    g_value_set_string(val, str);
}

static void changed_callback(void* widget, object_tab_t* tab) {
    update_tab(tab->tab);
}

static void init(object_tab_t* tab) {
    buffer_data_t* data = malloc(sizeof(buffer_data_t));
    tab->data = data;
    add_obj_common_to_info_box(tab->info_box);
    add_to_info_box(tab->info_box, "Mapped");
    add_to_info_box(tab->info_box, "Map Offset");
    add_to_info_box(tab->info_box, "Map Length");
    add_to_info_box(tab->info_box, "Map Access");
    add_separator_to_info_box(tab->info_box);
    add_to_info_box(tab->info_box, "Data Usage");
    add_to_info_box(tab->info_box, "Data Size");
    add_separator_to_info_box(tab->info_box);
    add_custom_to_info_box(tab->info_box, "Data View", NULL);
    
    data->data_offset = create_integral_spin_button(0, INT64_MAX, &changed_callback, tab);
    data->data_stride = create_integral_spin_button(0, INT64_MAX, &changed_callback, tab);
    data->data_components = create_integral_spin_button(1, INT64_MAX, &changed_callback, tab);
    
    add_custom_to_info_box(tab->info_box, "Offset", GTK_WIDGET(data->data_offset));
    add_custom_to_info_box(tab->info_box, "Stride", GTK_WIDGET(data->data_stride));
    add_custom_to_info_box(tab->info_box, "Components", GTK_WIDGET(data->data_components));
    
    data->data_type = create_combobox(TYPE_FLOAT32, (const char*[]){
        "Float16", "Float32", "Float64", "Uint8", "Int8", "Uint16", "Int16",
        "Uint32", "Int32", "Uint64", "Int64", NULL});
    g_signal_connect(data->data_type, "changed", G_CALLBACK(changed_callback), tab);
    add_custom_to_info_box(tab->info_box, "Data Type", GTK_WIDGET(data->data_type));
    
    data->data_view = create_tree_view(1, 0, "");
    gtk_tree_view_set_headers_visible(data->data_view, false);
    data->data_store = GTK_TREE_MODEL(gui_simple_store_new());
    gtk_tree_view_set_model(data->data_view, data->data_store);
    
    data->view_type = 0;
    data->view_type_size = 0;
    data->view_components = 0;
    data->view_stride = 0;
    data->data_size = 0;
    data->data = NULL;
    
    add_custom_to_info_box(tab->info_box, NULL, create_scrolled_window(GTK_WIDGET(data->data_view)));
}

static void deinit(object_tab_t* tab) {
    free(((buffer_data_t*)tab->data)->data);
    free(tab->data);
}

static void update_data_view(buffer_data_t* buf_data, const trc_gl_buffer_rev_t* rev) {
    //Clear it (to make error handling simpler)
    GuiSimpleStoreUpdate up;
    up.size = 0;
    up.column_count = 1;
    up.get_func = &get_function;
    up.column_types[0] = G_TYPE_STRING;
    up.user_data = buf_data;
    gui_simple_store_update(GUI_SIMPLE_STORE(buf_data->data_store), up);
    
    //Then fill it
    uint64_t offset = gtk_spin_button_get_value(buf_data->data_offset);
    uint64_t stride = gtk_spin_button_get_value(buf_data->data_stride);
    uint64_t components = gtk_spin_button_get_value(buf_data->data_components);
    gint type = gtk_combo_box_get_active(buf_data->data_type);
    if (type < 0) return;
    
    size_t type_size = ((size_t[]){
        [TYPE_UINT8]=1, [TYPE_INT8]=1,
        [TYPE_UINT16]=2, [TYPE_INT16]=2, [TYPE_FLOAT16]=2,
        [TYPE_UINT32]=4, [TYPE_INT32]=4, [TYPE_FLOAT32]=4,
        [TYPE_UINT64]=8, [TYPE_INT64]=8, [TYPE_FLOAT64]=8})[type];
    if (!stride) stride = components * type_size;
    if (!stride) return; //For the unlikely scenario when components == 0
    
    buf_data->view_type = type;
    buf_data->view_type_size = type_size;
    buf_data->view_components = components;
    buf_data->view_stride = stride;
    
    trc_read_chunked_data_t rinfo =
        {.data=rev->data, .start=offset, .size=rev->data.size-offset};
    rinfo.dest = malloc(rinfo.size);
    trc_read_chunked_data(rinfo);
    
    free(buf_data->data);
    buf_data->data = rinfo.dest;
    buf_data->data_size = rinfo.size;
    
    up.size = buf_data->data_size/stride + (buf_data->data_size%stride?1:0);
    gui_simple_store_update(GUI_SIMPLE_STORE(buf_data->data_store), up);
}

static void update(object_tab_t* tab, const trc_obj_rev_head_t* rev_head, uint64_t revision) {
    const trc_gl_buffer_rev_t* rev = (const trc_gl_buffer_rev_t*)rev_head;
    
    buffer_data_t* data = tab->data;
    
    info_box_t* box = tab->info_box;
    if (!set_obj_common_at_info_box(box, rev_head, revision)) return;
    set_at_info_box(box, "Mapped", "%s", rev->mapped?"True":"False");
    set_at_info_box(box, "Map Offset", "%lu", rev->map_offset);
    set_at_info_box(box, "Map Length", "%lu", rev->map_length);
    
    uint flags[8] = {
        GL_MAP_READ_BIT, GL_MAP_WRITE_BIT, GL_MAP_PERSISTENT_BIT,
        GL_MAP_COHERENT_BIT, GL_MAP_INVALIDATE_RANGE_BIT,
        GL_MAP_INVALIDATE_BUFFER_BIT, GL_MAP_FLUSH_EXPLICIT_BIT,
        GL_MAP_UNSYNCHRONIZED_BIT};
    const char* flag_names[8] = {
        "GL_MAP_READ_BIT", "GL_MAP_WRITE_BIT", "GL_MAP_PERSISTENT_BIT",
        "GL_MAP_COHERENT_BIT", "GL_MAP_INVALIDATE_RANGE_BIT",
        "GL_MAP_INVALIDATE_BUFFER_BIT", "GL_MAP_FLUSH_EXPLICIT_BIT",
        "GL_MAP_UNSYNCHRONIZED_BIT"};
    char access_str[256] = {0};
    for (size_t i = 0; i < 8; i++) {
        if (rev->map_access&flags[i]) {
            if (access_str[0]) cat_str(access_str, " | ", sizeof(access_str));
            cat_str(access_str, flag_names[i], sizeof(access_str));
        }
    }
    set_at_info_box(box, "Map Access", "%s", access_str);
    
    set_enum_at_info_box(box, "Data Usage", "BufferUsageARB", rev->data_usage);
    set_at_info_box(box, "Data Size", "%zu", rev->data.size);
    
    update_data_view(data, rev);
}

static __attribute__((constructor)) void init_callbacks() {
    object_tab_init_callbacks[(int)TrcBuffer] = &init;
    object_tab_deinit_callbacks[(int)TrcBuffer] = &deinit;
    object_tab_update_callbacks[(int)TrcBuffer] = (object_tab_update_callback_t)&update;
}
