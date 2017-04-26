#include "libtrace/libtrace.h"
#include "utils.h"

#include <gtk/gtk.h>
#include <string.h>
#include <assert.h>

extern GtkBuilder* builder;
extern GdkPixbuf* info_pixbuf;
extern GdkPixbuf* warning_pixbuf;
extern GdkPixbuf* error_pixbuf;
extern trace_t* trace;
extern int64_t revision;

void init_buffer_list(GtkTreeView* tree);
void init_vao_list(GtkTreeView* tree);
void init_framebuffers_list(GtkTreeView* tree);
void init_renderbuffers_list(GtkTreeView* tree);
void init_texture_list(GtkTreeView* tree);
void init_shader_list(GtkTreeView* tree);
void init_program_list(GtkTreeView* tree);
void init_syncs_list(GtkTreeView* tree);
void init_queries_list(GtkTreeView* tree);

static void init_trace_tree(GtkTreeView* tree) {
    GtkTreeStore* store = GTK_TREE_STORE(gtk_tree_view_get_model(tree));
    gtk_tree_store_clear(store);
    
    for (size_t i = 0; i < trace->frame_count; ++i) {
        trace_frame_t* frame = trace->frames + i;
        
        char frame_str[4096];
        memset(frame_str, 0, 4096);
        snprintf(frame_str, 4096, "Frame %zu", i);
        
        bool error = false;
        bool warning = false;
        bool info = false;
        for (size_t j = 0; j < frame->command_count; ++j) {
            trc_attachment_t* attachment = frame->commands[j].attachments;
            while (attachment) {
                error = error || attachment->type == TrcAttachType_Error;
                warning = warning || attachment->type == TrcAttachType_Warning;
                info = info || attachment->type == TrcAttachType_Info;
                attachment = attachment->next;
            }
        }
        
        GdkPixbuf* pixbuf = NULL;
        if (error) pixbuf = error_pixbuf;
        else if (warning) pixbuf = warning_pixbuf;
        else if (info) pixbuf = info_pixbuf;
        
        GtkTreeIter frame_row;
        gtk_tree_store_append(store, &frame_row, NULL);
        gtk_tree_store_set(store, &frame_row, 0, pixbuf, 1, frame_str, -1);
        
        for (size_t j = 0; j < frame->command_count; ++j) {
            GtkTreeIter cmd_row;
            gtk_tree_store_append(store, &cmd_row, &frame_row);
            
            trace_command_t* cmd = frame->commands + j;
            char cmd_str[1024];
            memset(cmd_str, 0, 1024);
            format_command(trace, cmd_str, cmd, 1024);
            
            bool error = false;
            bool warning = false;
            bool info = false;
            trc_attachment_t* attachment = cmd->attachments;
            while (attachment) {
                error = error || attachment->type == TrcAttachType_Error;
                warning = warning || attachment->type == TrcAttachType_Warning;
                info = info || attachment->type == TrcAttachType_Info;
                attachment = attachment->next;
            }
            
            pixbuf = NULL;
            if (error) pixbuf = error_pixbuf;
            else if (warning) pixbuf = warning_pixbuf;
            else if (info) pixbuf = info_pixbuf;
            
            gtk_tree_store_set(store, &cmd_row, 0, pixbuf, 1, cmd_str, -1);
        }
    }
}

static uint8_t* get_state(trc_data_t* data) {
    static uint8_t res[8192];
    size_t srcsize = data->size;
    void* ptr = trc_map_data(data, TRC_MAP_READ);
    memcpy(res, ptr, srcsize<8192?srcsize:8192);
    trc_unmap_data(data);
    return res;
}

static bool* get_stateb(trc_data_t* data) {
    return (bool*)get_state(data);
}

static float* get_statef(trc_data_t* data) {
    return (float*)get_state(data);
}

static int* get_statei(trc_data_t* data) {
    return (int*)get_state(data);
}

static uint* get_stateu(trc_data_t* data) {
    return (uint*)get_state(data);
}

static size_t category_stack_size;
static GtkTreeIter category_stack[64];

static void begin_category(GtkTreeStore* store, const char* format, ...) {
    va_list list;
    va_start(list, format);
    char name[1024];
    vsnprintf(name, sizeof(name), format, list);
    va_end(list);
    
    GtkTreeIter* parent = category_stack_size>0 ? &category_stack[category_stack_size-1] : NULL;
    GtkTreeIter* row = &category_stack[category_stack_size++];
    gtk_tree_store_append(store, row, parent);
    gtk_tree_store_set(store, row, 0, name, 1, "", -1);
}

static void end_category() {
    category_stack_size--;
}

static void value(GtkTreeStore* store, const char* name, const char* format, ...) {
    va_list list;
    va_start(list, format);
    char value[1024];
    vsnprintf(value, sizeof(value), format, list);
    va_end(list);
    
    GtkTreeIter* parent = category_stack_size>0 ? &category_stack[category_stack_size-1] : NULL;
    GtkTreeIter row;
    gtk_tree_store_append(store, &row, parent);
    gtk_tree_store_set(store, &row, 0, name, 1, value, -1);
}

static void init_state_tree(GtkTreeView* tree, const trc_gl_context_rev_t* ctx) {
    GtkTreeStore* store = GTK_TREE_STORE(gtk_tree_view_get_model(GTK_TREE_VIEW(tree)));
    gtk_tree_store_clear(store);
    
    if (!ctx) return;
    
    //TODO: The current context
    value(store, "Drawable Size", static_format("%ux%u", ctx->drawable_width, ctx->drawable_height));
    
    begin_category(store, "Buffer Bindings");
    #define BUFFER(type) value(store, #type, "%u", ctx->bound_buffer_##type);
    BUFFER(GL_ARRAY_BUFFER);
    BUFFER(GL_ELEMENT_ARRAY_BUFFER);
    BUFFER(GL_UNIFORM_BUFFER);
    BUFFER(GL_SHADER_STORAGE_BUFFER);
    BUFFER(GL_ATOMIC_COUNTER_BUFFER);
    BUFFER(GL_DRAW_INDIRECT_BUFFER);
    BUFFER(GL_COPY_READ_BUFFER);
    BUFFER(GL_COPY_WRITE_BUFFER);
    BUFFER(GL_DISPATCH_INDIRECT_BUFFER);
    BUFFER(GL_PIXEL_PACK_BUFFER);
    BUFFER(GL_PIXEL_UNPACK_BUFFER);
    BUFFER(GL_QUERY_BUFFER);
    BUFFER(GL_TEXTURE_BUFFER);
    BUFFER(GL_TRANSFORM_FEEDBACK_BUFFER);
    end_category();
    
    #define STATE(prefix, name, getfunc, type, fmt, get) do {\
        type* val = getfunc(ctx->prefix##_##name);\
        size_t count = ctx->prefix##_##name->size/sizeof(uint);\
        if (count == 1) {\
            size_t i = 0;\
            value(store, #name, fmt, get);\
        } else {\
            begin_category(store, #name); \
            for (size_t i = 0; i < count; i++)\
                value(store, static_format("%zu", i), fmt, get);\
            end_category();\
        }\
    } while (0)
    
    #define STATE_FLOAT(prefix, name) STATE(prefix, name, get_statef, float, "%f", val[i])
    #define STATE_UINT(prefix, name) STATE(prefix, name, get_stateu, uint, "%u", val[i])
    #define STATE_INT(prefix, name) STATE(prefix, name, get_statei, int, "%d", val[i])
    #define STATE_ENUM(prefix, group, name) STATE(prefix, name, get_stateu, uint, "%s", get_enum_str(group, val[i]))
    #define STATE_BOOL(prefix, name) STATE(prefix, name, get_stateb, bool, "%s", val[i]?"true":"false")
    
    begin_category(store, "Query Bindings");
    STATE_UINT(bound_queries, GL_SAMPLES_PASSED);
    STATE_UINT(bound_queries, GL_ANY_SAMPLES_PASSED);
    STATE_UINT(bound_queries, GL_ANY_SAMPLES_PASSED_CONSERVATIVE);
    STATE_UINT(bound_queries, GL_PRIMITIVES_GENERATED);
    STATE_UINT(bound_queries, GL_TRANSFORM_FEEDBACK_PRIMITIVES_WRITTEN);
    STATE_UINT(bound_queries, GL_TIME_ELAPSED);
    end_category();
    
    begin_category(store, "Texture Bindings");
    STATE_UINT(bound_textures, GL_TEXTURE_1D);
    STATE_UINT(bound_textures, GL_TEXTURE_2D);
    STATE_UINT(bound_textures, GL_TEXTURE_3D);
    STATE_UINT(bound_textures, GL_TEXTURE_1D_ARRAY);
    STATE_UINT(bound_textures, GL_TEXTURE_2D_ARRAY);
    STATE_UINT(bound_textures, GL_TEXTURE_RECTANGLE);
    STATE_UINT(bound_textures, GL_TEXTURE_CUBE_MAP);
    STATE_UINT(bound_textures, GL_TEXTURE_CUBE_MAP_ARRAY);
    STATE_UINT(bound_textures, GL_TEXTURE_BUFFER);
    STATE_UINT(bound_textures, GL_TEXTURE_2D_MULTISAMPLE);
    STATE_UINT(bound_textures, GL_TEXTURE_2D_MULTISAMPLE_ARRAY);
    end_category();
    
    value(store, "GL_CURRENT_PROGRAM", static_format("%u", ctx->bound_program));
    value(store, "GL_VERTEX_ARRAY_BINDING", static_format("%u", ctx->bound_vao));
    value(store, "GL_RENDERBUFFER_BINDING", static_format("%u", ctx->bound_renderbuffer));
    value(store, "GL_READ_FRAMEBUFFER_BINDING", static_format("%u", ctx->read_framebuffer));
    value(store, "GL_DRAW_FRAMEBUFFER_BINDING", static_format("%u", ctx->draw_framebuffer));
    STATE_ENUM(state_enum, "DrawBufferBuffer", GL_DRAW_BUFFER);
    
    begin_category(store, "Enabled");
    STATE_BOOL(enabled, GL_BLEND);
    {
        bool* val = get_stateb(ctx->enabled_GL_CLIP_DISTANCE0);
        size_t count = ctx->enabled_GL_CLIP_DISTANCE0->size / sizeof(bool);
        for (size_t i = 0; i < count; i++)
            value(store, static_format("GL_CLIP_DISTANCE%zu", i), val[i]?"true":"false");
    }
    STATE_BOOL(enabled, GL_COLOR_LOGIC_OP);
    STATE_BOOL(enabled, GL_CULL_FACE);
    STATE_BOOL(enabled, GL_DEBUG_OUTPUT);
    STATE_BOOL(enabled, GL_DEBUG_OUTPUT_SYNCHRONOUS);
    STATE_BOOL(enabled, GL_DEPTH_CLAMP);
    STATE_BOOL(enabled, GL_DEPTH_TEST);
    STATE_BOOL(enabled, GL_DITHER);
    STATE_BOOL(enabled, GL_FRAMEBUFFER_SRGB);
    STATE_BOOL(enabled, GL_LINE_SMOOTH);
    STATE_BOOL(enabled, GL_MULTISAMPLE);
    STATE_BOOL(enabled, GL_POLYGON_OFFSET_FILL);
    STATE_BOOL(enabled, GL_POLYGON_OFFSET_LINE);
    STATE_BOOL(enabled, GL_POLYGON_OFFSET_POINT);
    STATE_BOOL(enabled, GL_POLYGON_SMOOTH);
    STATE_BOOL(enabled, GL_PRIMITIVE_RESTART);
    STATE_BOOL(enabled, GL_PRIMITIVE_RESTART_FIXED_INDEX);
    STATE_BOOL(enabled, GL_RASTERIZER_DISCARD);
    STATE_BOOL(enabled, GL_SAMPLE_ALPHA_TO_COVERAGE);
    STATE_BOOL(enabled, GL_SAMPLE_ALPHA_TO_ONE);
    STATE_BOOL(enabled, GL_SAMPLE_SHADING);
    STATE_BOOL(enabled, GL_SAMPLE_MASK);
    STATE_BOOL(enabled, GL_SCISSOR_TEST);
    STATE_BOOL(enabled, GL_STENCIL_TEST);
    STATE_BOOL(enabled, GL_TEXTURE_CUBE_MAP_SEAMLESS);
    STATE_BOOL(enabled, GL_PROGRAM_POINT_SIZE);
    end_category();
    
    begin_category(store, "Blending");
    STATE_FLOAT(state_float, GL_BLEND_COLOR);
    STATE_ENUM(state_enum, "BlendingFactorDest", GL_BLEND_DST_RGB);
    STATE_ENUM(state_enum, "BlendingFactorDest", GL_BLEND_DST_ALPHA);
    STATE_ENUM(state_enum, "BlendingFactorSrc", GL_BLEND_SRC_RGB);
    STATE_ENUM(state_enum, "BlendingFactorSrc", GL_BLEND_SRC_ALPHA);
    STATE_ENUM(state_enum, "BlendEquationMode", GL_BLEND_EQUATION_RGB);
    STATE_ENUM(state_enum, "BlendEquationMode", GL_BLEND_EQUATION_ALPHA);
    end_category();
    
    begin_category(store, "Stencil");
    STATE_ENUM(state_enum, "StencilFunction", GL_STENCIL_FUNC);
    STATE_ENUM(state_enum, "StencilOp", GL_STENCIL_FAIL); 
    STATE_ENUM(state_enum, "StencilOp", GL_STENCIL_PASS_DEPTH_FAIL);
    STATE_ENUM(state_enum, "StencilOp", GL_STENCIL_PASS_DEPTH_PASS);
    STATE_ENUM(state_enum, "StencilFunction", GL_STENCIL_BACK_FUNC);
    STATE_ENUM(state_enum, "StencilOp", GL_STENCIL_BACK_FAIL); 
    STATE_ENUM(state_enum, "StencilOp", GL_STENCIL_BACK_PASS_DEPTH_FAIL);
    STATE_ENUM(state_enum, "StencilOp", GL_STENCIL_BACK_PASS_DEPTH_PASS);
    STATE_INT(state_int, GL_STENCIL_CLEAR_VALUE);
    STATE_INT(state_int, GL_STENCIL_VALUE_MASK);
    STATE_INT(state_int, GL_STENCIL_REF);
    STATE_INT(state_int, GL_STENCIL_BACK_VALUE_MASK);
    STATE_INT(state_int, GL_STENCIL_BACK_REF);
    STATE_INT(state_int, GL_STENCIL_WRITEMASK);
    STATE_INT(state_int, GL_STENCIL_BACK_WRITEMASK);
    end_category();
    
    begin_category(store, "GL_CURRENT_VERTEX_ATTRIB");
    double* cur_vertex_attrib = trc_map_data(ctx->state_double_GL_CURRENT_VERTEX_ATTRIB, TRC_MAP_READ);
    size_t count = ctx->state_double_GL_CURRENT_VERTEX_ATTRIB->size/sizeof(double)/4 + 1;
    for (uint i = 1; i < count; i++) {
        double v[4];
        for (uint j = 0; j < 4; j++) v[j] = cur_vertex_attrib[(i-1)*4+j];
        value(store, static_format("%u", i), "%g %g %g %g", v[0], v[1], v[2], v[3]);
    }
    trc_unmap_data(ctx->state_double_GL_CURRENT_VERTEX_ATTRIB);
    end_category();
    
    begin_category(store, "Pack");
    STATE_BOOL(state_bool, GL_PACK_SWAP_BYTES);
    STATE_BOOL(state_bool, GL_PACK_LSB_FIRST);
    STATE_INT(state_int, GL_PACK_ROW_LENGTH);
    STATE_INT(state_int, GL_PACK_IMAGE_HEIGHT);
    STATE_INT(state_int, GL_PACK_SKIP_ROWS);
    STATE_INT(state_int, GL_PACK_SKIP_PIXELS);
    STATE_INT(state_int, GL_PACK_SKIP_IMAGES);
    STATE_INT(state_int, GL_PACK_ALIGNMENT);
    end_category();
    
    begin_category(store, "Unpack");
    STATE_BOOL(state_bool, GL_UNPACK_SWAP_BYTES);
    STATE_BOOL(state_bool, GL_UNPACK_LSB_FIRST);
    STATE_INT(state_int, GL_UNPACK_ROW_LENGTH);
    STATE_INT(state_int, GL_UNPACK_IMAGE_HEIGHT);
    STATE_INT(state_int, GL_UNPACK_SKIP_ROWS);
    STATE_INT(state_int, GL_UNPACK_SKIP_PIXELS);
    STATE_INT(state_int, GL_UNPACK_SKIP_IMAGES);
    STATE_INT(state_int, GL_UNPACK_ALIGNMENT);
    end_category();
    
    STATE_BOOL(state_bool, GL_DEPTH_WRITEMASK);
    STATE_BOOL(state_bool, GL_COLOR_WRITEMASK);
    STATE_BOOL(state_bool, GL_SAMPLE_COVERAGE_INVERT);
    
    STATE_INT(state_int, GL_PRIMITIVE_RESTART_INDEX);
    STATE_INT(state_int, GL_SCISSOR_BOX); //TODO: Improve this
    
    STATE_ENUM(state_enum, "PolygonMode", GL_POLYGON_MODE);
    STATE_ENUM(state_enum, "CullFaceMode", GL_CULL_FACE_MODE);
    STATE_ENUM(state_enum, "FrontFaceDirection", GL_FRONT_FACE);
    STATE_ENUM(state_enum, "DepthFunction", GL_DEPTH_FUNC);
    STATE_ENUM(state_enum, "LogicOp", GL_LOGIC_OP_MODE);
    STATE_ENUM(state_enum, NULL, GL_POINT_SPRITE_COORD_ORIGIN); //TODO: The group
    STATE_ENUM(state_enum, NULL, GL_PROVOKING_VERTEX); //TODO: The group
    
    STATE_FLOAT(state_float, GL_VIEWPORT);
    STATE_FLOAT(state_float, GL_DEPTH_CLEAR_VALUE);
    STATE_FLOAT(state_float, GL_COLOR_CLEAR_VALUE);
    STATE_FLOAT(state_float, GL_DEPTH_RANGE);
    STATE_FLOAT(state_float, GL_POINT_SIZE);
    STATE_FLOAT(state_float, GL_LINE_WIDTH);
    STATE_FLOAT(state_float, GL_POLYGON_OFFSET_UNITS);
    STATE_FLOAT(state_float, GL_POLYGON_OFFSET_FACTOR);
    STATE_FLOAT(state_float, GL_SAMPLE_COVERAGE_VALUE);
    STATE_FLOAT(state_float, GL_POINT_FADE_THRESHOLD_SIZE);
    STATE_FLOAT(state_float, GL_MIN_SAMPLE_SHADING_VALUE);
    
    begin_category(store, "Hints");
    #define HINT(name) value(store, #name, "%s", get_enum_str("HintMode", ctx->hints_##name));
    HINT(GL_FRAGMENT_SHADER_DERIVATIVE_HINT);
    HINT(GL_LINE_SMOOTH_HINT);
    HINT(GL_POLYGON_SMOOTH_HINT);
    HINT(GL_TEXTURE_COMPRESSION_HINT);
    #undef HINT
    end_category();
    
    begin_category(store, "Capabilities");
    STATE_INT(state_int, GL_MAX_CLIP_DISTANCES);
    STATE_INT(state_int, GL_MAX_VIEWPORTS);
    STATE_INT(state_int, GL_MAX_DRAW_BUFFERS);
    end_category();
    
    #undef STATE_BOOL
    #undef STATE_ENUM
    #undef STATE_INT
    #undef STATE_UINT
    #undef STATE_FLOAT
}

void command_select_callback(GObject* obj, gpointer user_data) {
    GtkTreePath* path;
    gtk_tree_view_get_cursor(GTK_TREE_VIEW(obj), &path, NULL);
    if (!path) return;
    
    if (gtk_tree_path_get_depth(path) == 2) {
        gint* indices = gtk_tree_path_get_indices(path);
        
        assert(indices[0] < trace->frame_count);
        trace_frame_t* frame = trace->frames + indices[0];
        assert(indices[1] < frame->command_count);
        trace_command_t* cmd = frame->commands + indices[1];
        revision = cmd->revision;
        
        init_texture_list(GTK_TREE_VIEW(gtk_builder_get_object(builder, "texture_list_treeview")));
        init_buffer_list(GTK_TREE_VIEW(gtk_builder_get_object(builder, "buffers_treeview")));
        init_shader_list(GTK_TREE_VIEW(gtk_builder_get_object(builder, "shader_list_treeview")));
        init_program_list(GTK_TREE_VIEW(gtk_builder_get_object(builder, "program_list_view")));
        init_vao_list(GTK_TREE_VIEW(gtk_builder_get_object(builder, "vao_treeview")));
        init_framebuffers_list(GTK_TREE_VIEW(gtk_builder_get_object(builder, "framebuffers_treeview")));
        init_renderbuffers_list(GTK_TREE_VIEW(gtk_builder_get_object(builder, "renderbuffer_list")));
        init_syncs_list(GTK_TREE_VIEW(gtk_builder_get_object(builder, "sync_list")));
        init_queries_list(GTK_TREE_VIEW(gtk_builder_get_object(builder, "query_list")));
        
        GObject* view = gtk_builder_get_object(builder, "selected_command_attachments");
        GtkTreeStore* store = GTK_TREE_STORE(gtk_tree_view_get_model(GTK_TREE_VIEW(view)));
        gtk_tree_store_clear(store);
        
        trc_attachment_t* attachment = cmd->attachments;
        while (attachment) {
            GtkTreeIter row;
            gtk_tree_store_append(store, &row, NULL);
            gtk_tree_store_set(store, &row, 0, attachment->message, -1);
            
            attachment = attachment->next;
        }
        
        uint64_t fake = trc_lookup_current_fake_gl_context(trace, revision);
        const trc_gl_context_rev_t* state = trc_lookup_gl_context(trace, revision, fake);
        
        init_state_tree(GTK_TREE_VIEW(gtk_builder_get_object(builder, "state_treeview")), state);
    } else {
        GtkTreeView* tree = GTK_TREE_VIEW(gtk_builder_get_object(builder, "state_treeview"));
        GtkTreeStore* store = GTK_TREE_STORE(gtk_tree_view_get_model(tree));
        gtk_tree_store_clear(store);
    }
}

void trace_init() {
    init_treeview(builder, "selected_command_attachments", 1);
    init_treeview(builder, "state_treeview", 2);
    
    //Initialize the command list view
    GtkTreeView* tree = GTK_TREE_VIEW(gtk_builder_get_object(builder, "trace_view"));
    GtkTreeStore* store = gtk_tree_store_new(2, GDK_TYPE_PIXBUF, G_TYPE_STRING);
    
    gtk_tree_view_set_model(tree, GTK_TREE_MODEL(store));
    
    GtkCellRenderer* renderer = gtk_cell_renderer_pixbuf_new();
    GtkTreeViewColumn* column = gtk_tree_view_get_column(tree, 0);
    gtk_tree_view_column_pack_start(column, renderer, FALSE);
    gtk_tree_view_column_set_attributes(column, renderer, "pixbuf", 0, NULL);
    
    renderer = gtk_cell_renderer_text_new();
    column = gtk_tree_view_get_column(tree, 1);
    gtk_tree_view_column_pack_start(column, renderer, FALSE);
    gtk_tree_view_column_set_attributes(column, renderer, "text", 1, NULL);
}

void trace_fill() {
    GObject* trace_view = gtk_builder_get_object(builder, "trace_view");
    init_trace_tree(GTK_TREE_VIEW(trace_view));
}
