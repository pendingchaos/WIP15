#include "../gui.h"
#include "../utils.h"

#include <stdlib.h>

typedef struct program_data_t {
    GtkTextView* info_log;
    GtkTreeStore* attached_shaders;
    GtkTreeStore* linked_shaders;
    GtkTreeStore* uniforms;
    GtkTreeStore* uniform_blocks;
} program_data_t;

static void init(object_tab_t* tab) {
    program_data_t* data = malloc(sizeof(program_data_t));
    tab->data = data;
    add_obj_common_to_info_box(tab->info_box);
    add_multiple_to_info_box(tab->info_box,
        "Separable", "Binary Retreivable Hint", "Link Status", "Validation Status", NULL);
    
    add_separator_to_info_box(tab->info_box);
    add_custom_to_info_box(tab->info_box, "Info Log", NULL);
    
    data->info_log = GTK_TEXT_VIEW(gtk_text_view_new());
    gtk_text_view_set_editable(data->info_log, false);
    gtk_widget_set_vexpand(GTK_WIDGET(data->info_log), true);
    add_custom_to_info_box(tab->info_box, NULL, create_scrolled_window(GTK_WIDGET(data->info_log)));
    
    add_separator_to_info_box(tab->info_box);
    add_custom_to_info_box(tab->info_box, "Attached Shaders", NULL);
    
    GtkTreeView* view = create_tree_view(1, 1, "Shader", G_TYPE_POINTER);
    data->attached_shaders = GTK_TREE_STORE(gtk_tree_view_get_model(view));
    add_custom_to_info_box(tab->info_box, NULL, create_scrolled_window(GTK_WIDGET(view)));
    init_object_column(view, tab, 0, 1, -1);
    
    add_separator_to_info_box(tab->info_box);
    add_custom_to_info_box(tab->info_box, "Linked Shaders", NULL);
    
    view = create_tree_view(2, 2, "Shader", "Revision", G_TYPE_POINTER, G_TYPE_UINT64);
    data->linked_shaders = GTK_TREE_STORE(gtk_tree_view_get_model(view));
    add_custom_to_info_box(tab->info_box, NULL, create_scrolled_window(GTK_WIDGET(view)));
    init_object_column(view, tab, 0, 2, 3);
    
    add_separator_to_info_box(tab->info_box);
    add_custom_to_info_box(tab->info_box, "Uniforms", NULL);
    
    view = create_tree_view(3, 0, "Name", "Location", "Value");
    data->uniforms = GTK_TREE_STORE(gtk_tree_view_get_model(view));
    add_custom_to_info_box(tab->info_box, NULL, create_scrolled_window(GTK_WIDGET(view)));
    
    add_separator_to_info_box(tab->info_box);
    add_custom_to_info_box(tab->info_box, "Uniform Blocks", NULL);
    
    view = create_tree_view(3, 0, "Name", "Index", "Binding");
    data->uniform_blocks = GTK_TREE_STORE(gtk_tree_view_get_model(view));
    add_custom_to_info_box(tab->info_box, NULL, create_scrolled_window(GTK_WIDGET(view)));
}

static void deinit(object_tab_t* tab) {
    free(tab->data);
}

static void fill_uniform_tree(const trc_gl_uniform_t* uniforms, const uint8_t* uniform_data,
                              size_t uniform_, GtkTreeStore* store, GtkTreeIter* parent, int index) {
    const trc_gl_uniform_t* uniform = &uniforms[uniform_];
    
    char* name = index<0 ? trc_map_data(uniform->name, TRC_MAP_READ) : NULL;
    if (index >= 0) {
        name = malloc(16);
        sprintf(name, "[%d]", index);
    }
    
    if (uniform->dtype.base==TrcUniformBaseType_Struct ||
        uniform->dtype.base==TrcUniformBaseType_Array) {
        GtkTreeIter row;
        gtk_tree_store_append(store, &row, parent);
        gtk_tree_store_set(store, &row, 0, name, -1);
        
        size_t counter = 0;
        for (uint child = uniform->first_child; child!=0xffffffff; child = uniforms[child].next) {
            int new_index = uniform->dtype.base==TrcUniformBaseType_Array ? counter : -1;
            fill_uniform_tree(uniforms, uniform_data, child, store, &row, new_index);
            counter++;
        }
    } else if (uniform->dtype.base==TrcUniformBaseType_AtomicCounter) {
        GtkTreeIter row;
        gtk_tree_store_append(store, &row, parent);
        gtk_tree_store_set(store, &row, 0, name, -1);
    } else {
        const uint8_t* val = uniform_data + uniform->data_offset;
        
        char value_str[128] = {0};
        if (uniform->dtype.dim[0]!=1 || uniform->dtype.dim[1]!=1)
            cat_str(value_str, "[", sizeof(value_str));
        for (size_t i = 0; i < uniform->dtype.dim[0]*uniform->dtype.dim[1]; i++) {
            if (i != 0) cat_str(value_str, ", ", sizeof(value_str));
            switch (uniform->dtype.base) {
            case TrcUniformBaseType_Float:
                cat_str(value_str, static_format("%g", ((float*)val)[i]), sizeof(value_str));
                break;
            case TrcUniformBaseType_Double:
                cat_str(value_str, static_format("%g", ((double*)val)[i]), sizeof(value_str));
                break;
            case TrcUniformBaseType_Uint:
            case TrcUniformBaseType_Sampler:
            case TrcUniformBaseType_Image:
                cat_str(value_str, static_format("%u", ((uint32_t*)val)[i]), sizeof(value_str));
                break;
            case TrcUniformBaseType_Int:
                cat_str(value_str, static_format("%d", ((int32_t*)val)[i]), sizeof(value_str));
                break;
            case TrcUniformBaseType_Uint64:
                cat_str(value_str, static_format("%lu", ((uint64_t*)val)[i]), sizeof(value_str));
                break;
            case TrcUniformBaseType_Int64:
                cat_str(value_str, static_format("%ld", ((int64_t*)val)[i]), sizeof(value_str));
                break;
            case TrcUniformBaseType_Bool:
                cat_str(value_str, static_format("%s", ((uint32_t*)val)[i]?"True":"False"), sizeof(value_str));
                break;
            default:
                break;
            }
        }
        if (uniform->dtype.dim[0]!=1 || uniform->dtype.dim[1]!=1)
            cat_str(value_str, "]", sizeof(value_str));
        
        char location_str[16] = {0};
        snprintf(location_str, sizeof(location_str)-1, "%u", uniform->fake_loc);
        
        GtkTreeIter row;
        gtk_tree_store_append(store, &row, parent);
        gtk_tree_store_set(store, &row, 0, name, 1, location_str, 2, value_str, -1);
    }
    
    if (index < 0) trc_unmap_data(name);
    else free(name);
}

static void update(object_tab_t* tab, const trc_obj_rev_head_t* rev_head, uint64_t revision) {
    const trc_gl_program_rev_t* rev = (const trc_gl_program_rev_t*)rev_head;
    program_data_t* data = tab->data;
    
    info_box_t* box = tab->info_box;
    if (!set_obj_common_at_info_box(box, rev_head, revision)) return;
    set_at_info_box(box, "Separable", "%s", rev->separable?"True":"False");
    set_at_info_box(box, "Binary Retrievable Hint", "%s",
                    (const char*[]){"Unset", "False", "True"}[rev->binary_retrievable_hint+1]);
    
    set_at_info_box(tab->info_box, "Validation Status", "%s",
                    (const char*[]){"Not Done", "Failed", "Succeeded"}[rev->validation_status+1]);
    
    set_at_info_box(tab->info_box, "Link Status", "%s",
                    (const char*[]){"Not Done", "Failed", "Succeeded"}[rev->link_status+1]);
    
    //Info log
    const char* info_log = trc_map_data(rev->info_log, TRC_MAP_READ);
    gtk_text_buffer_set_text(gtk_text_view_get_buffer(data->info_log), info_log, -1);
    trc_unmap_data(info_log);
    
    //Attached shaders
    size_t shader_count = rev->shaders->size / sizeof(trc_obj_ref_t);
    const trc_obj_ref_t* shaders = trc_map_data(rev->shaders, TRC_MAP_READ);
    gtk_tree_store_clear(data->attached_shaders);
    for (size_t i = 0; i < shader_count; i++) {
        GtkTreeIter row;
        gtk_tree_store_append(data->attached_shaders, &row, NULL);
        gtk_tree_store_set(data->attached_shaders, &row,
                           0, static_format_obj(shaders[i].obj, revision),
                           1, shaders[i].obj, -1);
    }
    trc_unmap_data(shaders);
    
    //Linked shaders
    size_t linked_count = rev->linked->size / sizeof(trc_obj_t*);
    trc_obj_t*const* linked = trc_map_data(rev->linked, TRC_MAP_READ);
    gtk_tree_store_clear(data->linked_shaders);
    for (size_t i = 0; i < linked_count; i++) {
        const char* rev_str = static_format("%lu", rev->link_revision);
        GtkTreeIter row;
        gtk_tree_store_append(data->linked_shaders, &row, NULL);
        gtk_tree_store_set(data->linked_shaders, &row, 1, rev_str,
                           0, static_format_obj(linked[i], revision),
                           2, linked[i], 3, rev->link_revision, -1);
    }
    trc_unmap_data(linked);
    
    //Uniform blocks
    size_t block_count = rev->uniform_blocks->size / sizeof(trc_gl_program_uniform_block_t);
    const trc_gl_program_uniform_block_t* blocks = trc_map_data(rev->uniform_blocks, TRC_MAP_READ);
    gtk_tree_store_clear(data->uniform_blocks);
    for (size_t i = 0; i < block_count; i++) {
        char index_str[64] = {0};
        snprintf(index_str, sizeof(index_str)-1, "%u", blocks[i].fake);
        char binding_str[64] = {0};
        snprintf(binding_str, sizeof(index_str)-1, "%u", blocks[i].binding);
        
        const char* name = trc_map_data(blocks[i].name, TRC_MAP_READ);
        
        GtkTreeIter row;
        gtk_tree_store_append(data->uniform_blocks, &row, NULL);
        gtk_tree_store_set(data->uniform_blocks, &row, 0, name, 1, index_str, 2, binding_str, -1);
        
        trc_unmap_data(name);
    }
    trc_unmap_data(blocks);
    
    //Uniforms
    const trc_gl_uniform_t* uniforms = trc_map_data(rev->uniforms, TRC_MAP_READ);
    const uint8_t* uniform_data = trc_map_data(rev->uniform_data, TRC_MAP_READ);
    gtk_tree_store_clear(data->uniforms);
    for (size_t i = 0; i < rev->root_uniform_count; i++)
        fill_uniform_tree(uniforms, uniform_data, i, data->uniforms, NULL, -1);
    trc_unmap_data(uniform_data);
    trc_unmap_data(uniforms);
}

static __attribute__((constructor)) void init_callbacks() {
    object_tab_init_callbacks[(int)TrcProgram] = &init;
    object_tab_deinit_callbacks[(int)TrcProgram] = &deinit;
    object_tab_update_callbacks[(int)TrcProgram] = (object_tab_update_callback_t)&update;
}
