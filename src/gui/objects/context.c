#include "../gui.h"
#include "../utils.h"

#include "../gui.h"
#include "../utils.h"

#include <string.h>
#include <stdlib.h>

typedef struct context_data_t {
    object_tab_t* tab;
    GtkTreeStore* store;
    GtkWidget* open_back_buffer;
    GtkWidget* open_front_buffer;
    GtkWidget* open_depth_buffer;
    GtkWidget* open_stencil_buffer;
    image_viewer_t* back_viewer;
    image_viewer_t* front_viewer;
    image_viewer_t* depth_viewer;
    image_viewer_t* stencil_viewer;
} context_data_t;

static void on_close_back_buffer(context_data_t* data) {
    data->back_viewer = NULL;
    gtk_widget_set_sensitive(data->open_back_buffer, true);
    update_tab(data->tab->tab);
}

static void on_close_front_buffer(context_data_t* data) {
    data->front_viewer = NULL;
    gtk_widget_set_sensitive(data->open_front_buffer, true);
    update_tab(data->tab->tab);
}

static void on_close_depth_buffer(context_data_t* data) {
    data->depth_viewer = NULL;
    gtk_widget_set_sensitive(data->open_depth_buffer, true);
    update_tab(data->tab->tab);
}

static void on_close_stencil_buffer(context_data_t* data) {
    data->stencil_viewer = NULL;
    gtk_widget_set_sensitive(data->open_stencil_buffer, true);
    update_tab(data->tab->tab);
}

static void open_back_buffer(GtkButton* _, context_data_t* data) {
    data->back_viewer = create_image_viewer();
    gtk_widget_set_vexpand(data->back_viewer->widget, true);
    add_object_notebook_tab(data->tab->obj_notebook, "Back Buffer", true, data->back_viewer->widget,
                            (void(*)(void*))on_close_back_buffer, data);
    gtk_widget_set_sensitive(data->open_back_buffer, false);
    update_tab(data->tab->tab);
}

static void open_front_buffer(GtkButton* _, context_data_t* data) {
    data->front_viewer = create_image_viewer();
    gtk_widget_set_vexpand(data->front_viewer->widget, true);
    add_object_notebook_tab(data->tab->obj_notebook, "Front Buffer", true, data->front_viewer->widget,
                            (void(*)(void*))on_close_front_buffer, data);
    gtk_widget_set_sensitive(data->open_front_buffer, false);
    update_tab(data->tab->tab);
}

static void open_depth_buffer(GtkButton* _, context_data_t* data) {
    data->depth_viewer = create_image_viewer();
    gtk_widget_set_vexpand(data->depth_viewer->widget, true);
    add_object_notebook_tab(data->tab->obj_notebook, "Depth Buffer", true, data->depth_viewer->widget,
                            (void(*)(void*))on_close_depth_buffer, data);
    gtk_widget_set_sensitive(data->open_depth_buffer, false);
    update_tab(data->tab->tab);
}

static void open_stencil_buffer(GtkButton* _, context_data_t* data) {
    data->stencil_viewer = create_image_viewer();
    gtk_widget_set_vexpand(data->stencil_viewer->widget, true);
    add_object_notebook_tab(data->tab->obj_notebook, "Stencil Buffer", true, data->stencil_viewer->widget,
                            (void(*)(void*))on_close_stencil_buffer, data);
    gtk_widget_set_sensitive(data->open_stencil_buffer, false);
    update_tab(data->tab->tab);
}

static void init(object_tab_t* tab) {
    context_data_t* data = calloc(1, sizeof(context_data_t));
    data->tab = tab;
    tab->data = data;
    add_obj_common_to_info_box(tab->info_box);
    
    add_custom_to_info_box(tab->info_box, "State", NULL);
    
    data->open_back_buffer = create_button("Back", &open_back_buffer, data);
    data->open_depth_buffer = create_button("Depth", &open_depth_buffer, data);
    data->open_stencil_buffer = create_button("Stencil", &open_stencil_buffer, data);
    data->open_front_buffer = create_button("Front", &open_front_buffer, data);
    
    GtkWidget* box = create_box(false, 4,
        data->open_back_buffer, data->open_depth_buffer,
        data->open_stencil_buffer, data->open_front_buffer);
    
    add_custom_to_info_box(tab->info_box, "Open Buffer", box);
    
    GtkTreeView* view = create_tree_view(2, "Name", "Value");
    gtk_tree_view_set_search_column(view, 0);
    data->store = GTK_TREE_STORE(gtk_tree_view_get_model(view));
    add_custom_to_info_box(tab->info_box, NULL, create_scrolled_window(GTK_WIDGET(view)));
}

static void deinit(object_tab_t* tab) {
    free(tab->data);
}

typedef struct value_tree_state_t {
    GtkTreeStore* store;
    size_t category_stack_size;
    GtkTreeIter category_stack[64];
} value_tree_state_t;

static void begin_category(value_tree_state_t* state, const char* format, ...) {
    va_list list;
    va_start(list, format);
    char name[1024];
    vsnprintf(name, sizeof(name), format, list);
    va_end(list);
    
    GtkTreeIter* parent = NULL;
    if (state->category_stack_size > 0)
        parent = &state->category_stack[state->category_stack_size-1];
    GtkTreeIter* row = &state->category_stack[state->category_stack_size++];
    gtk_tree_store_append(state->store, row, parent);
    gtk_tree_store_set(state->store, row, 0, name, 1, "", -1);
}

static void end_category(value_tree_state_t* state) {
    state->category_stack_size--;
}

static void value(value_tree_state_t* state, const char* name, const char* format, ...) {
    va_list list;
    va_start(list, format);
    char value[1024] = {0};
    vsnprintf(value, sizeof(value), format, list);
    va_end(list);
    
    GtkTreeIter* parent = NULL;
    if (state->category_stack_size > 0)
        parent = &state->category_stack[state->category_stack_size-1];
    GtkTreeIter row;
    gtk_tree_store_append(state->store, &row, parent);
    gtk_tree_store_set(state->store, &row, 0, name, 1, value, -1);
}

static void object_list(value_tree_state_t* state, const char* name, trc_data_t* list,
                        trc_obj_type_t type, uint64_t revision) {
    trc_obj_ref_t* objs = trc_map_data(list, TRC_MAP_READ);
    begin_category(state, "%s", name);
    for (size_t i = 0;i < list->size/sizeof(trc_obj_ref_t);i++) {
        trc_obj_t* obj = objs[i].obj;
        const char* null = type==TrcContext||type==TrcSync ? "NULL" : "0";
        value(state, static_format("%zu", i), "%s", obj?static_format_obj(obj, revision):null);
    }
    end_category(state);
    trc_unmap_data(list);
}

static void value_obj(value_tree_state_t* state, const char* name, trc_obj_ref_t obj, uint64_t revision) {
    value(state, name, static_format_obj(obj.obj, revision));
}

static void value_enums(value_tree_state_t* state, const char* group, const char* name, trc_data_t* list) {
    uint* vals = trc_map_data(list, TRC_MAP_READ);
    if (list->size == sizeof(uint)) {
        value(state, name, "%s", get_enum_str(group, vals[0]));
    } else {
        begin_category(state, "%s", name);
        for (size_t i = 0; i < list->size/sizeof(uint);i++)
            value(state, static_format("%zu", i), "%s", get_enum_str(group, vals[i]));
        end_category(state);
    }
    trc_unmap_data(list);
}

static void value_bools(value_tree_state_t* state, const char* name, trc_data_t* list) {
    bool* vals = trc_map_data(list, TRC_MAP_READ);
    if (list->size == sizeof(bool)) {
        value(state, name, "%s", vals[0]?"True":"False");
    } else {
        begin_category(state, "%s", name);
        for (size_t i = 0; i < list->size/sizeof(bool);i++)
            value(state, static_format("%zu", i), "%s", vals[i]?"True":"False");
        end_category(state);
    }
    trc_unmap_data(list);
}

static void value_floats(value_tree_state_t* state, const char* name, trc_data_t* list) {
    float* vals = trc_map_data(list, TRC_MAP_READ);
    if (list->size == sizeof(float)) {
        value(state, name, "%g", vals[0]);
    } else {
        begin_category(state, "%s", name);
        for (size_t i = 0; i < list->size/sizeof(float);i++)
            value(state, static_format("%zu", i), "%g", vals[i]);
        end_category(state);
    }
    trc_unmap_data(list);
}

static void value_ints(value_tree_state_t* state, const char* name, trc_data_t* list) {
    int* vals = trc_map_data(list, TRC_MAP_READ);
    if (list->size == sizeof(int)) {
        value(state, name, "%d", vals[0]);
    } else {
        begin_category(state, "%s", name);
        for (size_t i = 0;i < list->size/sizeof(int);i++)
            value(state, static_format("%zu", i), "%d", vals[i]);
        end_category(state);
    }
    trc_unmap_data(list);
}

static void value_indexed_bufs(value_tree_state_t* state, const char* name,
                               trc_data_t* data, uint64_t revision) {
    begin_category(state, "%s", name);
    size_t count = data->size / sizeof(trc_gl_buffer_binding_point_t);
    const trc_gl_buffer_binding_point_t* bindings = trc_map_data(data, TRC_MAP_READ);
    for (size_t i = 0; i < count; i++) {
        trc_gl_buffer_binding_point_t binding = bindings[i];
        if (!binding.buf.obj) continue;
        if (!binding.size) {
            const trc_gl_buffer_rev_t* buf = trc_obj_get_rev(binding.buf.obj, revision);
            binding.size = buf->data->size;
        }
        value(state, static_format("%zu", i), "%s (%zu-%zu)",
              static_format_obj(binding.buf.obj, revision),
              binding.offset, binding.offset+binding.size);
    }
    trc_unmap_data(data);
    end_category(state);
}

static void packing_state(value_tree_state_t* state, const trc_gl_context_rev_t* rev) {
    begin_category(state, "Image Packing");
    value_bools(state, "GL_PACK_SWAP_BYTES", rev->state_bool_GL_PACK_SWAP_BYTES);
    value_bools(state, "GL_PACK_LSB_FIRST", rev->state_bool_GL_PACK_LSB_FIRST);
    value_ints(state, "GL_PACK_ROW_LENGTH", rev->state_int_GL_PACK_ROW_LENGTH);
    value_ints(state, "GL_PACK_IMAGE_HEIGHT", rev->state_int_GL_PACK_IMAGE_HEIGHT);
    value_ints(state, "GL_PACK_SKIP_ROWS", rev->state_int_GL_PACK_SKIP_ROWS);
    value_ints(state, "GL_PACK_SKIP_PIXELS", rev->state_int_GL_PACK_SKIP_PIXELS);
    value_ints(state, "GL_PACK_SKIP_IMAGES", rev->state_int_GL_PACK_SKIP_IMAGES);
    value_ints(state, "GL_PACK_ALIGNMENT", rev->state_int_GL_PACK_ALIGNMENT);
    end_category(state);
}

static void unpacking_state(value_tree_state_t* state, const trc_gl_context_rev_t* rev) {
    begin_category(state, "Image Unpacking");
    value_bools(state, "GL_UNPACK_SWAP_BYTES", rev->state_bool_GL_UNPACK_SWAP_BYTES);
    value_bools(state, "GL_UNPACK_LSB_FIRST", rev->state_bool_GL_UNPACK_LSB_FIRST);
    value_ints(state, "GL_UNPACK_ROW_LENGTH", rev->state_int_GL_UNPACK_ROW_LENGTH);
    value_ints(state, "GL_UNPACK_IMAGE_HEIGHT", rev->state_int_GL_UNPACK_IMAGE_HEIGHT);
    value_ints(state, "GL_UNPACK_SKIP_ROWS", rev->state_int_GL_UNPACK_SKIP_ROWS);
    value_ints(state, "GL_UNPACK_SKIP_PIXELS", rev->state_int_GL_UNPACK_SKIP_PIXELS);
    value_ints(state, "GL_UNPACK_SKIP_IMAGES", rev->state_int_GL_UNPACK_SKIP_IMAGES);
    value_ints(state, "GL_UNPACK_ALIGNMENT", rev->state_int_GL_UNPACK_ALIGNMENT);
    end_category(state);
}

static void tesselation_state(value_tree_state_t* state, const trc_gl_context_rev_t* rev) {
    begin_category(state, "Tesselation");
    value_ints(state, "GL_PATCH_VERTICES", rev->state_int_GL_PATCH_VERTICES);
    value_floats(state, "GL_PATCH_DEFAULT_OUTER_LEVEL",
                 rev->state_float_GL_PATCH_DEFAULT_OUTER_LEVEL);
    value_floats(state, "GL_PATCH_DEFAULT_INNER_LEVEL",
                 rev->state_float_GL_PATCH_DEFAULT_INNER_LEVEL);
    end_category(state);
}

static void polygon_offset_state(value_tree_state_t* state, const trc_gl_context_rev_t* rev) {
    begin_category(state, "Polygon Offset");
    value_bools(state, "GL_POLYGON_OFFSET_FILL", rev->enabled_GL_POLYGON_OFFSET_FILL);
    value_bools(state, "GL_POLYGON_OFFSET_LINE", rev->enabled_GL_POLYGON_OFFSET_LINE);
    value_bools(state, "GL_POLYGON_OFFSET_POINT", rev->enabled_GL_POLYGON_OFFSET_POINT);
    value_floats(state, "GL_POLYGON_OFFSET_UNITS", rev->state_float_GL_POLYGON_OFFSET_UNITS);
    value_floats(state, "GL_POLYGON_OFFSET_FACTOR", rev->state_float_GL_POLYGON_OFFSET_FACTOR);
    value_floats(state, "GL_POLYGON_OFFSET_CLAMP", rev->state_float_GL_POLYGON_OFFSET_CLAMP);
    end_category(state);
}

static void msaa_state(value_tree_state_t* state, const trc_gl_context_rev_t* rev) {
    begin_category(state, "Multi-Sampling");
    value_bools(state, "GL_MULTISAMPLE", rev->enabled_GL_MULTISAMPLE);
    value_bools(state, "GL_SAMPLE_ALPHA_TO_COVERAGE",
                rev->enabled_GL_SAMPLE_ALPHA_TO_COVERAGE);
    value_bools(state, "GL_SAMPLE_ALPHA_TO_ONE", rev->enabled_GL_SAMPLE_ALPHA_TO_ONE);
    value_bools(state, "GL_SAMPLE_COVERAGE", rev->enabled_GL_SAMPLE_COVERAGE);
    value_bools(state, "GL_SAMPLE_COVERAGE_INVERT", rev->state_bool_GL_SAMPLE_COVERAGE_INVERT);
    value_floats(state, "GL_SAMPLE_COVERAGE_VALUE", rev->state_float_GL_SAMPLE_COVERAGE_VALUE);
    value_bools(state, "GL_SAMPLE_SHADING", rev->enabled_GL_SAMPLE_SHADING);
    value_floats(state, "GL_MIN_SAMPLE_SHADING_VALUE",
                 rev->state_float_GL_MIN_SAMPLE_SHADING_VALUE);
    
    value_bools(state, "GL_SAMPLE_MASK", rev->enabled_GL_SAMPLE_MASK);
    
    char sample_mask[1024] = {0};
    const uint32_t* sample_mask_ints =
        trc_map_data(rev->state_int_GL_SAMPLE_MASK_VALUE, TRC_MAP_READ);
    for (size_t i = 0; i < rev->state_int_GL_SAMPLE_MASK_VALUE->size/sizeof(int); i++) {
        if (i > 0) cat_str(sample_mask, ", ", sizeof(sample_mask));
        char src[33] = {0};
        for (size_t j = 0; j < 32; j++)
            src[j] = "01"[sample_mask_ints[i]>>j & 1];
        cat_str(sample_mask, src, sizeof(sample_mask));
    }
    trc_unmap_data(rev->state_int_GL_SAMPLE_MASK_VALUE);
    value(state, "GL_SAMPLE_MASK_WORDS", "%s", sample_mask);
    end_category(state);
}

static void vertex_array_state(value_tree_state_t* state, const trc_gl_context_rev_t* rev, uint64_t revision) {
    begin_category(state, "Vertex Array");
    value_obj(state, "GL_VERTEX_ARRAY_BINDING", rev->bound_vao, revision);
    begin_category(state, "GL_CURRENT_VERTEX_ATTRIB");
    double* cur_vertex_attrib = trc_map_data(rev->state_double_GL_CURRENT_VERTEX_ATTRIB,
                                             TRC_MAP_READ);
    size_t count = rev->state_double_GL_CURRENT_VERTEX_ATTRIB->size / sizeof(double);
    for (uint i = 0; i < count; i+=4) {
        double* v = &cur_vertex_attrib[i];
        value(state, static_format("%u", i/4+1), "[%g, %g, %g, %g]", v[0], v[1], v[2], v[3]);
    }
    trc_unmap_data(rev->state_double_GL_CURRENT_VERTEX_ATTRIB);
    end_category(state);
    end_category(state);
}

static void primitive_restart_state(value_tree_state_t* state, const trc_gl_context_rev_t* rev) {
    begin_category(state, "Primitive Restart");
    value_bools(state, "GL_PRIMITIVE_RESTART", rev->enabled_GL_PRIMITIVE_RESTART);
    value_bools(state, "GL_PRIMITIVE_RESTART_FIXED_INDEX",
                rev->enabled_GL_PRIMITIVE_RESTART_FIXED_INDEX);
    value_ints(state, "GL_PRIMITIVE_RESTART_INDEX", rev->state_int_GL_PRIMITIVE_RESTART_INDEX);
    end_category(state);
}

static void viewport_scissor_state(value_tree_state_t* state, const trc_gl_context_rev_t* rev) {
    begin_category(state, "Viewport and Scissor");
    
    float* viewport = trc_map_data(rev->state_float_GL_VIEWPORT, TRC_MAP_READ);
    size_t count = rev->state_float_GL_VIEWPORT->size;
    if (count != 4) begin_category(state, "GL_VIEWPORT");
    for (size_t i = 0; i < rev->state_float_GL_VIEWPORT->size/sizeof(float); i+=4) {
        const char* name = count==4 ? "GL_VIEWPORT" : static_format("%u", i/4);
        value(state, name, "[%g, %g, %g, %g]",
              viewport[i], viewport[i+1], viewport[i+2], viewport[i+3]);
    }
    if (count != 4) end_category(state);
    trc_unmap_data(rev->state_float_GL_VIEWPORT);
    
    value_bools(state, "GL_SCISSOR_TEST", rev->enabled_GL_SCISSOR_TEST);
    
    int* scissor = trc_map_data(rev->state_int_GL_SCISSOR_BOX, TRC_MAP_READ);
    count = rev->state_int_GL_SCISSOR_BOX->size;
    if (count != 4) begin_category(state, "GL_SCISSOR_BOX");
    for (size_t i = 0; i < rev->state_int_GL_SCISSOR_BOX->size/sizeof(int); i+=4) {
        const char* name = count==4 ? "GL_SCISSOR_BOX" : static_format("%u", i/4);
        value(state, name, "[%d, %d, %d, %d]",
              scissor[i], scissor[i+1],
              scissor[i+2], scissor[i+3]);
    }
    if (count != 4) end_category(state);
    trc_unmap_data(rev->state_int_GL_SCISSOR_BOX);
    
    end_category(state);
}

static void rasterization_state(value_tree_state_t* state, const trc_gl_context_rev_t* rev) {
    begin_category(state, "Rasterization");
    value_bools(state, "GL_DEPTH_CLAMP", rev->enabled_GL_DEPTH_CLAMP);
    value_bools(state, "GL_RASTERIZER_DISCARD", rev->enabled_GL_RASTERIZER_DISCARD);
    value_enums(state, "PolygonMode", "GL_POLYGON_MODE", rev->state_enum_GL_POLYGON_MODE);
    value_bools(state, "GL_CULL_FACE", rev->enabled_GL_CULL_FACE);
    value_enums(state, "CullFaceMode", "GL_CULL_FACE_MODE", rev->state_enum_GL_CULL_FACE_MODE);
    value_enums(state, "FrontFaceDirection", "GL_FRONT_FACE", rev->state_enum_GL_FRONT_FACE);
    value_floats(state, "GL_POINT_SIZE", rev->state_float_GL_POINT_SIZE);
    value_floats(state, "GL_LINE_WIDTH", rev->state_float_GL_LINE_WIDTH);
    end_category(state);
}

static void depth_buffering_state(value_tree_state_t* state, const trc_gl_context_rev_t* rev) {
    begin_category(state, "Depth Buffering");
    value_bools(state, "GL_DEPTH_TEST", rev->enabled_GL_DEPTH_TEST);
    value_bools(state, "GL_DEPTH_WRITEMASK", rev->state_bool_GL_DEPTH_WRITEMASK);
    value_enums(state, "DepthFunction", "GL_DEPTH_FUNC", rev->state_enum_GL_DEPTH_FUNC);
    end_category(state);
}

static void blending_state(value_tree_state_t* state, const trc_gl_context_rev_t* rev) {
    begin_category(state, "Blending");
    value_bools(state, "GL_BLEND", rev->enabled_GL_BLEND);
    float* blend_color = trc_map_data(rev->state_float_GL_BLEND_COLOR, TRC_MAP_READ);
    value(state, "GL_BLEND_COLOR", "[%g, %g, %g, %g]",
          blend_color[0], blend_color[1], blend_color[2], blend_color[3]);
    trc_unmap_data(rev->state_float_GL_BLEND_COLOR);
    value_enums(state, "BlendingFactorDest", "GL_BLEND_DST_RGB",
                rev->state_enum_GL_BLEND_DST_RGB);
    value_enums(state, "BlendingFactorDest", "GL_BLEND_DST_ALPHA",
                rev->state_enum_GL_BLEND_DST_ALPHA);
    value_enums(state, "BlendingFactorSrc", "GL_BLEND_SRC_RGB",
                rev->state_enum_GL_BLEND_SRC_RGB);
    value_enums(state, "BlendingFactorSrc", "GL_BLEND_SRC_ALPHA",
                rev->state_enum_GL_BLEND_SRC_ALPHA);
    value_enums(state, "BlendEquation", "GL_BLEND_EQUATION_RGB",
                rev->state_enum_GL_BLEND_EQUATION_RGB);
    value_enums(state, "BlendEquation", "GL_BLEND_EQUATION_ALPHA",
                rev->state_enum_GL_BLEND_EQUATION_ALPHA);
    end_category(state);
}

static void stencil_state(value_tree_state_t* state, const trc_gl_context_rev_t* rev) {
    begin_category(state, "Stencil Buffering");
    value_bools(state, "GL_STENCIL_TEST", rev->enabled_GL_STENCIL_TEST);
    value_ints(state, "GL_STENCIL_CLEAR_VALUE", rev->state_int_GL_STENCIL_CLEAR_VALUE);
    value_ints(state, "GL_STENCIL_VALUE_MASK", rev->state_int_GL_STENCIL_VALUE_MASK);
    value_ints(state, "GL_STENCIL_REF", rev->state_int_GL_STENCIL_REF);
    value_ints(state, "GL_STENCIL_WRITEMASK", rev->state_int_GL_STENCIL_WRITEMASK);
    value_ints(state, "GL_STENCIL_BACK_VALUE_MASK", rev->state_int_GL_STENCIL_BACK_VALUE_MASK);
    value_ints(state, "GL_STENCIL_BACK_REF", rev->state_int_GL_STENCIL_BACK_REF);
    value_ints(state, "GL_STENCIL_BACK_WRITEMASK", rev->state_int_GL_STENCIL_BACK_WRITEMASK);
    value_enums(state, "StencilFunction", "GL_STENCIL_FUNC", rev->state_enum_GL_STENCIL_FUNC);
    value_enums(state, "StencilOp", "GL_STENCIL_FAIL", rev->state_enum_GL_STENCIL_FAIL);
    value_enums(state, "StencilOp", "GL_STENCIL_PASS_DEPTH_FAIL",
                rev->state_enum_GL_STENCIL_PASS_DEPTH_FAIL);
    value_enums(state, "StencilOp", "GL_STENCIL_PASS_DEPTH_PASS",
                rev->state_enum_GL_STENCIL_PASS_DEPTH_PASS);
    value_enums(state, "StencilFunction", "GL_STENCIL_BACK_FUNC",
                rev->state_enum_GL_STENCIL_BACK_FUNC);
    value_enums(state, "StencilOp", "GL_STENCIL_BACK_FAIL",
                rev->state_enum_GL_STENCIL_BACK_FAIL);
    value_enums(state, "StencilOp", "GL_STENCIL_BACK_PASS_DEPTH_FAIL",
                rev->state_enum_GL_STENCIL_BACK_PASS_DEPTH_FAIL);
    value_enums(state, "StencilOp", "GL_STENCIL_BACK_PASS_DEPTH_PASS",
                rev->state_enum_GL_STENCIL_BACK_PASS_DEPTH_PASS);
    end_category(state);
}

static void buffer_state(value_tree_state_t* state, const trc_gl_context_rev_t* rev, uint64_t revision) {
    begin_category(state, "General Buffer Bindings");
    value_obj(state, "GL_ARRAY_BUFFER", rev->bound_buffer_GL_ARRAY_BUFFER, revision);
    value_obj(state, "GL_ELEMENT_ARRAY_BUFFER", rev->bound_buffer_GL_ELEMENT_ARRAY_BUFFER, revision);
    value_obj(state, "GL_UNIFORM_BUFFER", rev->bound_buffer_GL_UNIFORM_BUFFER, revision);
    value_obj(state, "GL_SHADER_STORAGE_BUFFER", rev->bound_buffer_GL_SHADER_STORAGE_BUFFER, revision);
    value_obj(state, "GL_ATOMIC_COUNTER_BUFFER", rev->bound_buffer_GL_ATOMIC_COUNTER_BUFFER, revision);
    value_obj(state, "GL_DRAW_INDIRECT_BUFFER", rev->bound_buffer_GL_DRAW_INDIRECT_BUFFER, revision);
    value_obj(state, "GL_COPY_READ_BUFFER", rev->bound_buffer_GL_COPY_READ_BUFFER, revision);
    value_obj(state, "GL_COPY_WRITE_BUFFER", rev->bound_buffer_GL_COPY_WRITE_BUFFER, revision);
    value_obj(state, "GL_DISPATCH_INDIRECT_BUFFER",
              rev->bound_buffer_GL_DISPATCH_INDIRECT_BUFFER, revision);
    value_obj(state, "GL_PIXEL_PACK_BUFFER", rev->bound_buffer_GL_PIXEL_PACK_BUFFER, revision);
    value_obj(state, "GL_PIXEL_UNPACK_BUFFER", rev->bound_buffer_GL_PIXEL_UNPACK_BUFFER, revision);
    value_obj(state, "GL_QUERY_BUFFER", rev->bound_buffer_GL_QUERY_BUFFER, revision);
    value_obj(state, "GL_TEXTURE_BUFFER", rev->bound_buffer_GL_TEXTURE_BUFFER, revision);
    value_obj(state, "GL_TRANSFORM_FEEDBACK_BUFFER", rev->bound_buffer_GL_TRANSFORM_FEEDBACK_BUFFER, revision);
    end_category(state);
    
    begin_category(state, "Indexed Buffer Bindings");
    value_indexed_bufs(state, "GL_UNIFORM_BUFFER",
                       rev->bound_buffer_indexed_GL_UNIFORM_BUFFER, revision);
    value_indexed_bufs(state, "GL_SHADER_STORAGE_BUFFER",
                       rev->bound_buffer_indexed_GL_SHADER_STORAGE_BUFFER, revision);
    value_indexed_bufs(state, "GL_ATOMIC_COUNTER_BUFFER",
                       rev->bound_buffer_indexed_GL_ATOMIC_COUNTER_BUFFER, revision);
    end_category(state);
}

static void query_state(value_tree_state_t* state, const trc_gl_context_rev_t* rev, uint64_t revision) {
    begin_category(state, "Query Bindings");
    object_list(state, "GL_SAMPLES_PASSED",
                rev->bound_queries_GL_SAMPLES_PASSED, TrcQuery, revision);
    object_list(state, "GL_ANY_SAMPLES_PASSED",
                rev->bound_queries_GL_ANY_SAMPLES_PASSED, TrcQuery, revision);
    object_list(state, "GL_ANY_SAMPLES_PASSED_CONSERVATIVE",
                rev->bound_queries_GL_ANY_SAMPLES_PASSED_CONSERVATIVE, TrcQuery, revision);
    object_list(state, "GL_PRIMITIVES_GENERATED",
                rev->bound_queries_GL_PRIMITIVES_GENERATED, TrcQuery, revision);
    object_list(state, "GL_TRANSFORM_FEEDBACK_PRIMITIVES_WRITTEN",
                rev->bound_queries_GL_TRANSFORM_FEEDBACK_PRIMITIVES_WRITTEN, TrcQuery, revision);
    object_list(state, "GL_TIME_ELAPSED",
                rev->bound_queries_GL_TIME_ELAPSED, TrcQuery, revision);
    end_category(state);
}

static void texturing_state(value_tree_state_t* state, const trc_gl_context_rev_t* rev, uint64_t revision) {
    value(state, "Active Texture Unit", "%u", rev->active_texture_unit);
    value_bools(state, "GL_TEXTURE_CUBE_MAP_SEAMLESS",
                rev->enabled_GL_TEXTURE_CUBE_MAP_SEAMLESS);
    begin_category(state, "Texture Bindings");
    object_list(state, "GL_TEXTURE_1D", rev->bound_textures_GL_TEXTURE_1D, TrcTexture, revision);
    object_list(state, "GL_TEXTURE_2D", rev->bound_textures_GL_TEXTURE_2D, TrcTexture, revision);
    object_list(state, "GL_TEXTURE_3D", rev->bound_textures_GL_TEXTURE_3D, TrcTexture, revision);
    object_list(state, "GL_TEXTURE_1D_ARRAY",
               rev->bound_textures_GL_TEXTURE_1D_ARRAY, TrcTexture, revision);
    object_list(state, "GL_TEXTURE_2D_ARRAY",
                rev->bound_textures_GL_TEXTURE_2D_ARRAY, TrcTexture, revision);
    object_list(state, "GL_TEXTURE_RECTANGLE",
                rev->bound_textures_GL_TEXTURE_RECTANGLE, TrcTexture, revision);
    object_list(state, "GL_TEXTURE_CUBE_MAP",
                rev->bound_textures_GL_TEXTURE_CUBE_MAP, TrcTexture, revision);
    object_list(state, "GL_TEXTURE_CUBE_MAP_ARRAY",
                rev->bound_textures_GL_TEXTURE_CUBE_MAP_ARRAY, TrcTexture, revision);
    object_list(state, "GL_TEXTURE_BUFFER",
                rev->bound_textures_GL_TEXTURE_BUFFER, TrcTexture, revision);
    object_list(state, "GL_TEXTURE_2D_MULTISAMPLE",
                rev->bound_textures_GL_TEXTURE_2D_MULTISAMPLE, TrcTexture, revision);
    object_list(state, "GL_TEXTURE_2D_MULTISAMPLE_ARRAY",
                rev->bound_textures_GL_TEXTURE_2D_MULTISAMPLE_ARRAY, TrcTexture, revision);
    end_category(state);
    
    object_list(state, "Sampler Bindings", rev->bound_samplers, TrcSampler, revision);
}

static void update(object_tab_t* tab, const trc_obj_rev_head_t* rev_head, uint64_t revision) {
    const trc_gl_context_rev_t* rev = (const trc_gl_context_rev_t*)rev_head;
    context_data_t* data = tab->data;
    
    info_box_t* box = tab->info_box;
    if (!set_obj_common_at_info_box(box, rev_head, revision)) return;
    
    gtk_tree_store_clear(data->store);
    
    value_tree_state_t state;
    state.category_stack_size = 0;
    state.store = data->store;
    
    value(&state, "Drawable Size", "%ux%u", rev->drawable_width, rev->drawable_height);
    
    buffer_state(&state, rev, revision);
    query_state(&state, rev, revision);
    texturing_state(&state, rev, revision);
    
    value_obj(&state, "GL_CURRENT_PROGRAM", rev->bound_program, revision);
    value_obj(&state, "GL_PROGRAM_PIPELINE_BINDING", rev->bound_pipeline, revision);
    value_obj(&state, "GL_RENDERBUFFER_BINDING", rev->bound_renderbuffer, revision);
    value_obj(&state, "GL_READ_FRAMEBUFFER_BINDING", rev->read_framebuffer, revision);
    value_obj(&state, "GL_DRAW_FRAMEBUFFER_BINDING", rev->draw_framebuffer, revision);
    value_enums(&state, "DrawBufferBuffer", "GL_DRAW_BUFFER", rev->state_enum_GL_DRAW_BUFFER);
    
    begin_category(&state, "Transform Feedback");
    value_obj(&state, "Binding", rev->current_tf, revision);
    //TODO: Use a better group
    value(&state, "Primitive", "%s",
          get_enum_str("PrimitiveType", rev->tf_primitive));
    end_category(&state);
    
    vertex_array_state(&state, rev, revision);
    primitive_restart_state(&state, rev);
    viewport_scissor_state(&state, rev);
    rasterization_state(&state, rev);
    depth_buffering_state(&state, rev);
    
    msaa_state(&state, rev);
    
    begin_category(&state, "Enabled");
    
    bool* clip_distances = trc_map_data(rev->enabled_GL_CLIP_DISTANCE0, TRC_MAP_READ);
    size_t count = rev->enabled_GL_CLIP_DISTANCE0->size / sizeof(bool);
    for (size_t i = 0;i < count;i++)
        value(&state, static_format("GL_CLIP_DISTANCE%zu", i), clip_distances[i]?"True":"False");
    trc_unmap_data(rev->enabled_GL_CLIP_DISTANCE0);
    
    value_bools(&state, "GL_COLOR_LOGIC_OP", rev->enabled_GL_COLOR_LOGIC_OP);
    value_bools(&state, "GL_DEBUG_OUTPUT", rev->enabled_GL_DEBUG_OUTPUT);
    value_bools(&state, "GL_DEBUG_OUTPUT_SYNCHRONOUS",
                rev->enabled_GL_DEBUG_OUTPUT_SYNCHRONOUS);
    value_bools(&state, "GL_DITHER", rev->enabled_GL_DITHER);
    value_bools(&state, "GL_FRAMEBUFFER_SRGB", rev->enabled_GL_FRAMEBUFFER_SRGB);
    value_bools(&state, "GL_LINE_SMOOTH", rev->enabled_GL_LINE_SMOOTH);
    value_bools(&state, "GL_POLYGON_SMOOTH", rev->enabled_GL_POLYGON_SMOOTH);
    value_bools(&state, "GL_PROGRAM_POINT_SIZE", rev->enabled_GL_PROGRAM_POINT_SIZE);
    end_category(&state);
    
    blending_state(&state, rev);
    stencil_state(&state, rev);
    packing_state(&state, rev);
    unpacking_state(&state, rev);
    tesselation_state(&state, rev);
    polygon_offset_state(&state, rev);
    
    value_bools(&state, "GL_COLOR_WRITEMASK", rev->state_bool_GL_COLOR_WRITEMASK);
    
    value_enums(&state, "LogicOp", "GL_LOGIC_OP_MODE", rev->state_enum_GL_LOGIC_OP_MODE);
    value_enums(&state, "ClipOrigin", "GL_CLIP_ORIGIN", rev->state_enum_GL_CLIP_ORIGIN);
    value_enums(&state, "ClipDepth", "GL_CLIP_DEPTH_MODE", rev->state_enum_GL_CLIP_DEPTH_MODE);
    value_enums(&state, "PointSpriteCoordOrigin", "GL_POINT_SPRITE_COORD_ORIGIN",
                rev->state_enum_GL_POINT_SPRITE_COORD_ORIGIN);
    value_enums(&state, "ProvokingVertex", "GL_PROVOKING_VERTEX",
                rev->state_enum_GL_PROVOKING_VERTEX);
    
    value_floats(&state, "GL_DEPTH_CLEAR_VALUE", rev->state_float_GL_DEPTH_CLEAR_VALUE);
    value_floats(&state, "GL_COLOR_CLEAR_VALUE", rev->state_float_GL_COLOR_CLEAR_VALUE);
    value_floats(&state, "GL_DEPTH_RANGE", rev->state_float_GL_DEPTH_RANGE);
    value_floats(&state, "GL_POINT_FADE_THRESHOLD_SIZE",
                 rev->state_float_GL_POINT_FADE_THRESHOLD_SIZE);
    
    begin_category(&state, "Hints");
    value(&state, "GL_FRAGMENT_SHADER_DERIVATIVE_HINT",
          get_enum_str("HintMode", rev->hints_GL_FRAGMENT_SHADER_DERIVATIVE_HINT));
    value(&state, "GL_LINE_SMOOTH_HINT",
          get_enum_str("HintMode", rev->hints_GL_LINE_SMOOTH_HINT));
    value(&state, "GL_POLYGON_SMOOTH_HINT",
          get_enum_str("HintMode", rev->hints_GL_POLYGON_SMOOTH_HINT));
    value(&state, "GL_TEXTURE_COMPRESSION_HINT",
          get_enum_str("HintMode", rev->hints_GL_TEXTURE_COMPRESSION_HINT));
    end_category(&state);
    
    begin_category(&state, "Capabilities");
    value_ints(&state, "GL_MAX_CLIP_DISTANCES", rev->state_int_GL_MAX_CLIP_DISTANCES);
    value_ints(&state, "GL_MAX_TEXTURE_SIZE", rev->state_int_GL_MAX_TEXTURE_SIZE);
    value_ints(&state, "GL_MAJOR_VERSION", rev->state_int_GL_MAJOR_VERSION);
    value_ints(&state, "GL_MINOR_VERSION", rev->state_int_GL_MINOR_VERSION);
    value_ints(&state, "GL_MAX_VIEWPORTS", rev->state_int_GL_MAX_VIEWPORTS);
    value_ints(&state, "GL_MAX_RENDERBUFFER_SIZE", rev->state_int_GL_MAX_RENDERBUFFER_SIZE);
    value_ints(&state, "GL_MAX_DRAW_BUFFERS", rev->state_int_GL_MAX_DRAW_BUFFERS);
    value_ints(&state, "GL_MAX_TRANSFORM_FEEDBACK_BUFFERS",
               rev->state_int_GL_MAX_TRANSFORM_FEEDBACK_BUFFERS);
    value_ints(&state, "GL_MAX_ATOMIC_COUNTER_BUFFER_BINDINGS",
               rev->state_int_GL_MAX_ATOMIC_COUNTER_BUFFER_BINDINGS);
    value_ints(&state, "GL_MAX_SHADER_STORAGE_BUFFER_BINDINGS",
               rev->state_int_GL_MAX_SHADER_STORAGE_BUFFER_BINDINGS);
    value_ints(&state, "GL_MAX_VERTEX_ATTRIBS",
               rev->state_int_GL_MAX_VERTEX_ATTRIBS);
    value_ints(&state, "GL_MAX_COLOR_ATTACHMENTS",
               rev->state_int_GL_MAX_COLOR_ATTACHMENTS);
    value_ints(&state, "GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS",
               rev->state_int_GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS);
    value_ints(&state, "GL_MAX_UNIFORM_BUFFER_BINDINGS",
               rev->state_int_GL_MAX_UNIFORM_BUFFER_BINDINGS);
    value_ints(&state, "GL_MAX_PATCH_VERTICES",
               rev->state_int_GL_MAX_PATCH_VERTICES);
    value_ints(&state, "GL_MAX_SAMPLE_MASK_WORDS",
               rev->state_int_GL_MAX_SAMPLE_MASK_WORDS);
    end_category(&state);
    
    int dim[2] = {rev->drawable_width, rev->drawable_height};
    if (data->back_viewer) {
        update_image_viewer(data->back_viewer, 0, rev->back_color_buffer,
                            dim, TrcImageFormat_SRGBA_U8);
    }
    if (data->front_viewer) {
        update_image_viewer(data->front_viewer, 0, rev->front_color_buffer,
                            dim, TrcImageFormat_SRGBA_U8);
    }
    if (data->depth_viewer) {
        update_image_viewer(data->depth_viewer, 0, rev->back_depth_buffer,
                            dim, TrcImageFormat_Red_F32);
    }
    if (data->stencil_viewer) {
        update_image_viewer(data->stencil_viewer, 0, rev->back_stencil_buffer,
                            dim, TrcImageFormat_Red_U32);
    }
}

static __attribute__((constructor)) void init_callbacks() {
    object_tab_init_callbacks[(int)TrcContext] = &init;
    object_tab_deinit_callbacks[(int)TrcContext] = &deinit;
    object_tab_update_callbacks[(int)TrcContext] = (object_tab_update_callback_t)&update;
}
