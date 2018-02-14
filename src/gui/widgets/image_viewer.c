#include <epoxy/gl.h>
#include "image_viewer.h"
#include "../utils.h"

#if GTKSOURCEVIEW_ENABLED
#include <gtksourceview/gtksource.h>
#endif
#include <stdlib.h>
#include <string.h>
#include <math.h>

static uint compile_shader(GLenum type, size_t source_count, const char*const* sources,
                           GtkTextBuffer* info_log) {
    uint shader = glCreateShader(type);
    glShaderSource(shader, source_count, sources, NULL);
    glCompileShader(shader);
    
    GLint status;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
    if (status == GL_FALSE) {
        char info_log_text[1024];
        glGetShaderInfoLog(shader, 1024, NULL, info_log_text);
        gtk_text_buffer_set_text(info_log, info_log_text, -1);
        glDeleteShader(shader);
        return 0;
    }
    
    return shader;
}

static void create_image_viewer_program(image_viewer_t* viewer) {
    glDeleteProgram(viewer->program);
    viewer->program = 0;
    
    GtkTextBuffer* buf = gtk_text_view_get_buffer(viewer->shader_editor);
    GtkTextIter start, end;
    gtk_text_buffer_get_start_iter(buf, &start);
    gtk_text_buffer_get_end_iter(buf, &end);
    gchar* shader = gtk_text_buffer_get_text(buf, &start, &end, TRUE);
    
    GtkTextBuffer* info_log = gtk_text_view_get_buffer(viewer->shader_info_log);
    gtk_text_buffer_set_text(info_log, "", -1);
    
    const char* vertex_shader_src = "#version 150 core\n"
"in vec2 aPosition;\n"
"void main() {\n"
"    vec2 pos[] = vec2[](vec2(-1.0, -1.0), vec2(1.0, -1.0), vec2(1.0, 1.0),\n"
"                        vec2(-1.0, -1.0), vec2(-1.0, 1.0), vec2(1.0, 1.0));\n"
"    gl_Position = vec4(pos[gl_VertexID], 0.0, 1.0);\n"
"}\n";
    uint vertex_shader = compile_shader(GL_VERTEX_SHADER, 1, &vertex_shader_src, info_log);
    if (!vertex_shader) goto end;
    
    const char* fragment_shader_src = "vec4 shader_main(in vec2 uv);\n"
"uniform ivec4 uParams;\n"
"uniform bvec2 uParams2;\n"
"uniform bool uFlipY;\n"
"out vec4 oColor;\n"
"vec3 to_linear(in vec3 v) {\n"
"    vec3 a = pow((v+0.055)/1.055, vec3(2.4));\n"
"    return mix(a, v/12.92, vec3(lessThanEqual(v, vec3(0.04045))));\n"
"}\n"
"vec3 to_srgb(in vec3 v) {\n"
"    vec3 a = 1.055*pow(v, vec3(1.0/2.4)) - 0.055;\n"
"    return mix(a, v*12.92, vec3(lessThanEqual(v, vec3(0.0031308))));\n"
"}\n"
"vec4 stripes_pattern() {\n"
"    float v = dot(normalize(vec2(0.707106781)), gl_FragCoord.xy);\n"
"    vec4 col = int(vec3(v/20.0))%2==0 ? vec4(1.0, 0.35, 0.35, 1.0) : vec4(1.0);\n"
"    if (!uParams2.x) col.rgb = to_linear(col.rgb);\n"
"    return col;\n"
"}\n"
"void main() {\n"
"    int off = int(gl_FragCoord.y/20.0) % 2;\n"
"    int v = int(gl_FragCoord.x/20.0+off) % 2;\n"
"    oColor.rgb = v==0 ? vec3(0.603827339) : vec3(0.318546778);\n"
"    oColor.a = 1.0;\n"
"    ivec2 image_bl = uParams.xy;\n"
"    ivec2 image_tr = uParams.zw;\n"
"    ivec2 coord = ivec2(gl_FragCoord.xy) - image_bl;\n"
"    ivec2 image_size = image_tr - image_bl;\n"
"    bool within_x = clamp(coord.x, 0, image_size.x-1) == coord.x;\n"
"    bool within_y = clamp(coord.y, 0, image_size.y-1) == coord.y;\n"
"    if (within_x && within_y) {\n"
"        vec2 uv = vec2(coord)/vec2(image_size);\n"
"        vec4 color = shader_main(uFlipY?vec2(uv.x, 1.0-uv.y):uv);\n"
"        if (uParams2.x) color.rgb = to_linear(color.rgb);\n"
"        oColor.rgb = mix(oColor.rgb, color.rgb, clamp(color.a, 0.0, 1.0));\n"
"    }\n"
"    within_x = clamp(coord.x, -1, image_size.x) == coord.x;\n"
"    within_y = clamp(coord.y, -1, image_size.y) == coord.y;\n"
"    bool left = within_y && coord.x==-1;\n"
"    bool right = within_y && coord.x==image_size.x;\n"
"    bool bottom = within_x && coord.y==-1;\n"
"    bool top = within_x && coord.y==image_size.y;"
"    if ((left||right||bottom||top) && uParams2.y) {\n"
"        int dist;\n"
"        if (left) dist = coord.y;\n"
"        else if (right) dist = image_size.y + image_size.x + (image_size.y-coord.y-1) + 2;\n"
"        else if (top) dist = image_size.y + coord.x + 1;\n"
"        else if (bottom) dist = image_size.y*2 + image_size.x + (image_size.x-coord.x-1) + 3;\n"
"        oColor.rgb = dist%8<4 ? vec3(1.0, 1.0, 0.0) : vec3(0.0);\n"
"    }\n"
"    oColor.rgb = to_srgb(oColor.rgb);\n"
"}\n"
"#line 0\n";
    const char* fragment_sources[4] = {
        "#version 150 core\n", NULL, fragment_shader_src, shader};
    switch (viewer->format) {
    case TrcImageFormat_Red_U32:
    case TrcImageFormat_RedGreen_U32:
    case TrcImageFormat_RGB_U32:
    case TrcImageFormat_RGBA_U32:
        fragment_sources[1] = "uniform usampler2D uTexture;\n"
                              "#define UNSIGNED_INT_TEXTURE 1\n";
        break;
    case TrcImageFormat_Red_I32:
    case TrcImageFormat_RedGreen_I32:
    case TrcImageFormat_RGB_I32:
    case TrcImageFormat_RGBA_I32:
        fragment_sources[1] = "uniform isampler2D uTexture;\n"
                              "#define INT_TEXTURE 1\n";
        break;
    case TrcImageFormat_Red_F32:
    case TrcImageFormat_RedGreen_F32:
    case TrcImageFormat_RGB_F32:
    case TrcImageFormat_RGBA_F32:
    case TrcImageFormat_SRGB_U8:
    case TrcImageFormat_SRGBA_U8:
    case TrcImageFormat_RGBA_U8:
        fragment_sources[1] = "uniform sampler2D uTexture;\n"
                              "#define FLOAT_TEXTURE 1\n";
        break;
    case TrcImageFormat_F32_U24_U8:
        fragment_sources[1] = "uniform sampler2D uTexture;\n"
                              "uniform usampler2D uStencilTexture;\n"
                              "#define DEPTH_STENCIL_TEXTURE 1\n";
        break;
    }
    uint fragment_shader = compile_shader(GL_FRAGMENT_SHADER, 4, fragment_sources, info_log);
    if (!fragment_shader) goto end;
    
    viewer->program = glCreateProgram();
    glAttachShader(viewer->program, vertex_shader);
    glAttachShader(viewer->program, fragment_shader);
    glLinkProgram(viewer->program);
    glDetachShader(viewer->program, vertex_shader);
    glDetachShader(viewer->program, fragment_shader);
    
    GLint status;
    glGetProgramiv(viewer->program, GL_LINK_STATUS, &status);
    if (status == GL_FALSE) {
        char info_log_text[1024];
        glGetProgramInfoLog(viewer->program, 1024, NULL, info_log_text);
        gtk_text_buffer_set_text(info_log, info_log_text, -1);
        glDeleteProgram(viewer->program);
        viewer->program = 0;
        goto end;
    }
    
    glValidateProgram(viewer->program);
    glGetProgramiv(viewer->program, GL_VALIDATE_STATUS, &status);
    if (status == GL_FALSE) {
        char info_log_text[1024];
        glGetProgramInfoLog(viewer->program, 1024, NULL, info_log_text);
        gtk_text_buffer_set_text(info_log, info_log_text, -1);
        glDeleteProgram(viewer->program);
        viewer->program = 0;
    }
    
    end:
    g_free(shader);
}

static void zoom_value_changed(GtkSpinButton* spin_button, image_viewer_t* viewer) {
    gtk_widget_queue_draw(GTK_WIDGET(viewer->gl_area));
}

static void flip_y_toggled(GtkToggleButton* toggle_button, image_viewer_t* viewer) {
    gtk_widget_queue_draw(GTK_WIDGET(viewer->gl_area));
}

static void srgb_toggled(GtkToggleButton* toggle_button, image_viewer_t* viewer) {
    gtk_widget_queue_draw(GTK_WIDGET(viewer->gl_area));
}

static void border_toggled(GtkToggleButton* toggle_button, image_viewer_t* viewer) {
    gtk_widget_queue_draw(GTK_WIDGET(viewer->gl_area));
}

static void shader_changed(GtkTextBuffer* buf, image_viewer_t* viewer) {
    viewer->program_dirty = true;
    gtk_widget_queue_draw(GTK_WIDGET(viewer->gl_area));
}

static void image_viewer_realize(GtkGLArea* area, gpointer user_data) {
    if (gtk_gl_area_get_error(area)) return;
    
    gtk_gl_area_make_current(area);
    
    image_viewer_t* viewer = user_data;
    
    g_signal_connect(viewer->flip_y, "toggled", G_CALLBACK(flip_y_toggled), viewer);
    g_signal_connect(viewer->srgb, "toggled", G_CALLBACK(srgb_toggled), viewer);
    g_signal_connect(viewer->show_border, "toggled", G_CALLBACK(border_toggled), viewer);
    g_signal_connect(viewer->zoom, "value-changed", G_CALLBACK(zoom_value_changed), viewer);
    GtkTextBuffer* shader_buffer = gtk_text_view_get_buffer(viewer->shader_editor);
    g_signal_connect(shader_buffer, "changed", G_CALLBACK(shader_changed), viewer);
    
    const char* default_shader = "vec4 shader_main(in vec2 uv) {\n"
    "#if INT_TEXTURE || UNSIGNED_INT_TEXTURE\n"
    "    return stripes_pattern();\n"
    "#else\n"
    "    return texture(uTexture, uv);\n"
    "#endif\n"
    "}\n";
    gtk_text_buffer_set_text(shader_buffer, default_shader, -1);
}

static void image_viewer_unrealize(GtkGLArea* area, gpointer user_data) {
    gtk_gl_area_make_current(area);
    image_viewer_t* viewer = user_data;
    glDeleteProgram(viewer->program);
    glDeleteTextures(2, viewer->textures);
}

static void update_textures(image_viewer_t* viewer) {
    viewer->texture_dirty = false;
    viewer->program_dirty = true;
    
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    
    glDeleteTextures(2, viewer->textures);
    glGenTextures(2, viewer->textures);
    uint8_t* data_start = NULL;
    const uint8_t* data = NULL;
    if (viewer->data) {
        data_start = malloc(viewer->data->size);
        
        trc_read_chunked_data_t rinfo =
            {.data=*viewer->data, .start=0, .size=viewer->data->size, .dest=data_start};
        trc_read_chunked_data(rinfo);
        
        data = data_start;
        data += viewer->data_offset;
    }
    for (size_t i = 0; i < 2; i++) {
        glBindTexture(GL_TEXTURE_2D, viewer->textures[0]);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        switch (viewer->format) {
        #define I(i, f, t) glTexImage2D(GL_TEXTURE_2D, 0, i, viewer->image_width, viewer->image_height, 0, f, t, data);
        case TrcImageFormat_Red_U32: I(GL_R32UI, GL_RED, GL_UNSIGNED_INT); break;
        case TrcImageFormat_RedGreen_U32: I(GL_RG32UI, GL_RG, GL_UNSIGNED_INT); break;
        case TrcImageFormat_RGB_U32: I(GL_RGB32UI, GL_RGB, GL_UNSIGNED_INT); break;
        case TrcImageFormat_RGBA_U32: I(GL_RGBA32UI, GL_RGBA, GL_UNSIGNED_INT); break;
        case TrcImageFormat_Red_I32: I(GL_R32I, GL_RED, GL_INT); break;
        case TrcImageFormat_RedGreen_I32: I(GL_RG32I, GL_RG, GL_INT); break;
        case TrcImageFormat_RGB_I32: I(GL_RGB32I, GL_RGB, GL_INT); break;
        case TrcImageFormat_RGBA_I32: I(GL_RGBA32I, GL_RGBA, GL_INT); break;
        case TrcImageFormat_Red_F32: I(GL_R32F, GL_RED, GL_FLOAT); break;
        case TrcImageFormat_RedGreen_F32: I(GL_RG32F, GL_RG, GL_FLOAT); break;
        case TrcImageFormat_RGB_F32: I(GL_RGB32F, GL_RGB, GL_FLOAT); break;
        case TrcImageFormat_RGBA_F32: I(GL_RGBA32F, GL_RGBA, GL_FLOAT); break;
        case TrcImageFormat_SRGB_U8: I(GL_SRGB8, GL_RGB, GL_UNSIGNED_BYTE); break;
        case TrcImageFormat_SRGBA_U8: I(GL_SRGB8_ALPHA8, GL_RGBA, GL_UNSIGNED_BYTE); break;
        case TrcImageFormat_RGBA_U8: I(GL_RGBA8, GL_RGBA, GL_UNSIGNED_BYTE); break;
        case TrcImageFormat_F32_U24_U8: {
            void* data2 = malloc(viewer->image_width*viewer->image_height*4);
            
            for (size_t j = 0; j < viewer->image_width*viewer->image_height; j++) {
                if (i == 0) ((float*)data2)[j] = ((float*)data)[j*2];
                else ((uint32_t*)data2)[j] = ((uint32_t*)data)[j*2+1] & 0xff;
            }
            
            if (i == 0) {
                glTexImage2D(GL_TEXTURE_2D, 0, GL_R32F, viewer->image_width,
                             viewer->image_height, 0, GL_RED, GL_FLOAT, data2);
            } else {
                glTexImage2D(GL_TEXTURE_2D, 0, GL_R32UI, viewer->image_width,
                             viewer->image_height, 0, GL_RED, GL_UNSIGNED_INT, data2);
            }
            
            free(data2);
            goto cont;
        }
        #undef I
        }
        break;
        cont: ;
    }
    viewer->data_data = data_start;
}

static gboolean image_viewer_render(GtkGLArea* area, GdkGLContext* ctx, gpointer user_data) {
    image_viewer_t* viewer = user_data;
    
    if (viewer->texture_dirty) update_textures(viewer);
    
    if (viewer->program_dirty) {
        viewer->program_dirty = false;
        create_image_viewer_program(viewer);
    }
    
    uint program = viewer->program;
    
    glClearColor(1.0, 0.4, 0.4, 1.0);
    glClear(GL_COLOR_BUFFER_BIT);
    
    glUseProgram(program);
    GLint vp[4];
    glGetIntegerv(GL_VIEWPORT, vp);
    memcpy(viewer->viewport, vp, sizeof(vp));
    
    uint vao;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);
    
    double zoom = gtk_spin_button_get_value(viewer->zoom) / 100.0;
    bool flip_y = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(viewer->flip_y));
    
    glUniform4i(glGetUniformLocation(program, "uParams"),
                vp[2]/2.0-viewer->image_width/2.0*zoom+viewer->view_offset[0],
                vp[3]/2.0-viewer->image_height/2.0*zoom+viewer->view_offset[1],
                vp[2]/2.0+viewer->image_width/2.0*zoom+viewer->view_offset[0],
                vp[3]/2.0+viewer->image_height/2.0*zoom+viewer->view_offset[1]);
    glUniform2i(glGetUniformLocation(program, "uParams2"),
                gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(viewer->srgb))?1:0,
                gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(viewer->show_border))?1:0);
    glUniform1i(glGetUniformLocation(program, "uFlipY"), flip_y?1:0);
    if (viewer->format == TrcImageFormat_F32_U24_U8) {
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, viewer->textures[0]);
        glUniform1i(glGetUniformLocation(program, "uTexture"), 0);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, viewer->textures[1]);
        glUniform1i(glGetUniformLocation(program, "uStencilTexture"), 1);
    } else {
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, viewer->textures[0]);
        glUniform1i(glGetUniformLocation(program, "uTexture"), 0);
    }
    
    glDrawArrays(GL_TRIANGLES, 0, 6);
    
    glDeleteVertexArrays(1, &vao);
    
    return true;
}

static gboolean image_viewer_press(GtkWidget* widget, GdkEvent* event, image_viewer_t* viewer) {
    guint button;
    if (!gdk_event_get_button(event, &button) || button!=1) return false;
    viewer->dragging = true;
    
    gdk_window_get_device_position_double(
        gtk_widget_get_window(widget),
        gdk_event_get_source_device(event),
        &viewer->drag_start[0], &viewer->drag_start[1], NULL);
    
    GtkAllocation alloc;
    gtk_widget_get_allocation(widget, &alloc);
    viewer->drag_start[0] -= alloc.x;
    viewer->drag_start[1] -= alloc.y;
    
    return false;
}

static gboolean image_viewer_release(GtkWidget* widget, GdkEvent* event, image_viewer_t* viewer) {
    guint button;
    if (!gdk_event_get_button(event, &button) || button!=1) return false;
    viewer->dragging = false;
    return false;
}

static gboolean image_viewer_motion(GtkWidget* widget, GdkEvent* event, image_viewer_t* viewer) {
    gdouble x, y;
    if (!gdk_event_get_coords(event, &x, &y)) return false;
    
    if (viewer->dragging) {
        gdouble dx = x - viewer->drag_start[0];    
        gdouble dy = y - viewer->drag_start[1];
        
        viewer->view_offset[0] += dx;    
        viewer->view_offset[1] -= dy;
        viewer->drag_start[0] = x;
        viewer->drag_start[1] = y;
        
        gtk_widget_queue_draw(GTK_WIDGET(viewer->gl_area));
    }
    
    double zoom = gtk_spin_button_get_value(viewer->zoom) / 100.0;
    bool flip_y = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(viewer->flip_y));
    
    int left = viewer->viewport[2]/2.0 - viewer->image_width/2.0*zoom + viewer->view_offset[0];
    int bottom = viewer->viewport[3]/2.0 - viewer->image_height/2.0*zoom + viewer->view_offset[1];
    
    int px = (x-left) / zoom;
    int py = (flip_y?y:(viewer->viewport[3]-y-1)-bottom) / zoom; //origin = bottom
    if (px>=0 && py>=0 && px<viewer->image_width && py<viewer->image_height) {
        int comp = 0;
        double vals[4];
        const uint8_t* data = viewer->data_data + viewer->data_offset;
        switch (viewer->format) {
        #define READPIXEL(type, comp_) comp = comp_;\
        for (int i = 0; i < comp; i++) {\
            type val;\
            memcpy(&val, data+(py*viewer->image_width+px)*comp*sizeof(type)+i*sizeof(type), sizeof(type));\
            vals[i] = val;\
        }\
        break;
        case TrcImageFormat_Red_U32: {READPIXEL(uint32_t, 1)}
        case TrcImageFormat_RedGreen_U32: {READPIXEL(uint32_t, 2)}
        case TrcImageFormat_RGB_U32: {READPIXEL(uint32_t, 3)}
        case TrcImageFormat_RGBA_U32: {READPIXEL(uint32_t, 4)}
        case TrcImageFormat_Red_I32: {READPIXEL(int32_t, 1)}
        case TrcImageFormat_RedGreen_I32: {READPIXEL(int32_t, 2)}
        case TrcImageFormat_RGB_I32: {READPIXEL(int32_t, 3)}
        case TrcImageFormat_RGBA_I32: {READPIXEL(int32_t, 4)}
        case TrcImageFormat_Red_F32: {READPIXEL(float, 1)}
        case TrcImageFormat_RedGreen_F32: {READPIXEL(float, 2)}
        case TrcImageFormat_RGB_F32: {READPIXEL(float, 3)}
        case TrcImageFormat_RGBA_F32: {READPIXEL(float, 4)}
        #undef READPIXEL
        case TrcImageFormat_SRGB_U8: {
            comp = 3;
            for (int i = 0; i < 3; i++)
                vals[i] = data[(py*viewer->image_width+px)*3+i] / 255.0;
            break;
        }
        case TrcImageFormat_SRGBA_U8:
        case TrcImageFormat_RGBA_U8: {
            comp = 3;
            for (int i = 0; i < 3; i++)
                vals[i] = data[(py*viewer->image_width+px)*3+i] / 255.0;
            break;
        }
        case TrcImageFormat_F32_U24_U8: {
            comp = 2;
            float depth;
            uint32_t stencil;
            memcpy(&depth, data+(py*viewer->image_width+px)*(sizeof(float)+4), sizeof(float));
            memcpy(&stencil, data+(py*viewer->image_width+px)*(sizeof(float)+4)+sizeof(float), sizeof(uint32_t));
            vals[0] = depth;
            vals[1] = stencil;
            break;
        }
        }
        
        char current_pixel[256] = {0};
        switch (comp) {
        case 1: sprintf(current_pixel, "%g", vals[0]); break;
        case 2: sprintf(current_pixel, "[%g %g]", vals[0], vals[1]); break;
        case 3: sprintf(current_pixel, "[%g %g %g]", vals[0], vals[1], vals[2]); break;
        case 4: sprintf(current_pixel, "[%g %g %g %g]", vals[0], vals[1], vals[2], vals[3]); break;
        }
        
        char markup[256];
        sprintf(markup, "<span bgcolor='#ffffffff'>%s</span>", current_pixel);
        
        gtk_label_set_markup(viewer->current_pixel, markup);
    } else {
        gtk_label_set_text(viewer->current_pixel, "");
    }
    
    return false;
}

static gboolean image_viewer_scroll(GtkWidget* widget, GdkEvent* event, image_viewer_t* viewer) {
    gdouble dx, dy;
    if (!gdk_event_get_scroll_deltas(event, &dx, &dy)) return false;
    double zoom = gtk_spin_button_get_value(viewer->zoom);
    zoom = pow(2.0, log2(zoom/100.0)-dy*0.5) * 100.0;
    gtk_spin_button_set_value(viewer->zoom, zoom);
    return false;
}

static void read_shader(GtkButton* button, image_viewer_t* viewer) {
    char* filename = run_file_dialog("Read Shader", "Read", GTK_FILE_CHOOSER_ACTION_OPEN);
    if (!filename) return;
    FILE* file = fopen(filename, "r");    
    g_free(filename);
    if (!file) {display_error_dialog("Failed to read shader"); return;}
    
    fseek(file, -1, SEEK_END);
    long size = ftell(file);
    if (size < 0) {display_error_dialog("Failed to read shader"); fclose(file); return;}
    fseek(file, 0, SEEK_SET);
    
    char* shader = calloc(size+1, 1);
    fread(shader, 1, size, file);
        
    if (fclose(file) == EOF) {
        display_error_dialog("Failed to read shader");
        free(shader);
        return;
    }
    
    GtkTextBuffer* buf = gtk_text_view_get_buffer(viewer->shader_editor);
    gtk_text_buffer_set_text(buf, shader, -1);
    
    free(shader);
}

static void write_shader(GtkButton* button, image_viewer_t* viewer) {
    char* filename = run_file_dialog("Write Shader", "Write", GTK_FILE_CHOOSER_ACTION_SAVE);
    if (!filename) return;
    FILE* file = fopen(filename, "w");
    g_free(filename);
    if (!file) {display_error_dialog("Failed to write shader"); return;}
    
    GtkTextBuffer* buf = gtk_text_view_get_buffer(viewer->shader_editor);
    GtkTextIter start, end;
    gtk_text_buffer_get_start_iter(buf, &start);
    gtk_text_buffer_get_end_iter(buf, &end);
    char* shader = gtk_text_buffer_get_text(buf, &start, &end, TRUE);
    
    if (fputs(shader, file) == EOF) {
        display_error_dialog("Failed to write shader");
        fclose(file);
        g_free(shader);
        return;
    }
    
    if (fclose(file) == EOF) display_error_dialog("Failed to write shader");
    g_free(shader);
}

static void image_viewer_destroyed(GtkWidget* _, image_viewer_t* viewer) {
    free(viewer->data);
    free(viewer->data_data);
    free(viewer);
}

image_viewer_t* create_image_viewer() {
    image_viewer_t* viewer = malloc(sizeof(image_viewer_t));
    viewer->data_offset = 0;
    viewer->data = NULL;
    viewer->data_data = NULL;
    viewer->format = TrcImageFormat_RGBA_F32;
    viewer->image_width = 0;
    viewer->image_height = 0;
    viewer->view_offset[0] = 0.0;
    viewer->view_offset[1] = 0.0;
    viewer->program = 0;
    viewer->textures[0] = 0;
    viewer->textures[1] = 0;
    viewer->program_dirty = true;
    viewer->texture_dirty = true;
    viewer->dragging = false;
    memset(viewer->viewport, 0, sizeof(viewer->viewport));
    
    viewer->flip_y = GTK_CHECK_BUTTON(gtk_check_button_new_with_label("Flip Y"));
    viewer->srgb = GTK_CHECK_BUTTON(gtk_check_button_new_with_label("sRGB"));
    gtk_widget_set_tooltip_text(GTK_WIDGET(viewer->srgb), "whether shader_main()'s output is sRGB");
    viewer->show_border = GTK_CHECK_BUTTON(gtk_check_button_new_with_label("Border"));
    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(viewer->show_border), true);
    viewer->zoom = GTK_SPIN_BUTTON(gtk_spin_button_new_with_range(0.0, G_MAXDOUBLE, 10));
    gtk_spin_button_set_value(viewer->zoom, 100.0);
    viewer->current_pixel = GTK_LABEL(gtk_label_new(""));
    gtk_widget_set_halign(GTK_WIDGET(viewer->current_pixel), GTK_ALIGN_START);
    gtk_widget_set_valign(GTK_WIDGET(viewer->current_pixel), GTK_ALIGN_START);
    
    GtkWidget* read_button = create_button("Read Shader", &read_shader, viewer);
    GtkWidget* write_button = create_button("Write Shader", &write_shader, viewer);
    
    GtkWidget* box = create_box(false, 6,
        write_button, read_button, GTK_WIDGET(viewer->flip_y),
        GTK_WIDGET(viewer->srgb), GTK_WIDGET(viewer->show_border),
        GTK_WIDGET(viewer->zoom));
    
    viewer->gl_area = GTK_GL_AREA(gtk_gl_area_new());
    gtk_gl_area_set_required_version(viewer->gl_area, 3, 2);
    g_signal_connect(viewer->gl_area, "realize", G_CALLBACK(image_viewer_realize), viewer);
    g_signal_connect(viewer->gl_area, "unrealize", G_CALLBACK(image_viewer_unrealize), viewer);
    g_signal_connect(viewer->gl_area, "render", G_CALLBACK(image_viewer_render), viewer);
    gtk_widget_add_events(GTK_WIDGET(viewer->gl_area),
                          GDK_POINTER_MOTION_MASK|GDK_BUTTON_PRESS_MASK|
                          GDK_BUTTON_RELEASE_MASK|GDK_SMOOTH_SCROLL_MASK);
    g_signal_connect(viewer->gl_area, "button-press-event",
                     G_CALLBACK(image_viewer_press), viewer);
    g_signal_connect(viewer->gl_area, "button-release-event",
                     G_CALLBACK(image_viewer_release), viewer);
    g_signal_connect(viewer->gl_area, "motion-notify-event",
                     G_CALLBACK(image_viewer_motion), viewer);
    g_signal_connect(viewer->gl_area, "scroll-event",
                     G_CALLBACK(image_viewer_scroll), viewer);
    
    #if GTKSOURCEVIEW_ENABLED
    viewer->shader_editor = GTK_TEXT_VIEW(gtk_source_view_new());
    #else
    viewer->shader_editor = GTK_TEXT_VIEW(gtk_text_view_new());
    #endif
    viewer->shader_info_log = GTK_TEXT_VIEW(gtk_text_view_new());
    gtk_text_view_set_editable(viewer->shader_info_log, false);
    gtk_text_view_set_monospace(viewer->shader_info_log, true);
    gtk_text_view_set_monospace(viewer->shader_editor, true);
    
    #if GTKSOURCEVIEW_ENABLED
    GtkSourceView* source_view = GTK_SOURCE_VIEW(viewer->shader_editor);
    gtk_source_view_set_auto_indent(source_view, true);
    gtk_source_view_set_highlight_current_line(source_view, true);
    gtk_source_view_set_indent_width(source_view, 4);
    gtk_source_view_set_insert_spaces_instead_of_tabs(source_view, true);
    gtk_source_view_set_right_margin_position(source_view, 80);
    gtk_source_view_set_show_line_numbers(source_view, true);
    gtk_source_view_set_tab_width(source_view, 4);
    
    GtkSourceBuffer* source_buf = GTK_SOURCE_BUFFER(
        gtk_text_view_get_buffer(viewer->shader_editor));
    gtk_source_buffer_set_implicit_trailing_newline(source_buf, false);
    #endif
    
    GtkOverlay* overlay = GTK_OVERLAY(gtk_overlay_new());
    gtk_container_add(GTK_CONTAINER(overlay), GTK_WIDGET(viewer->gl_area));
    gtk_overlay_add_overlay(overlay, GTK_WIDGET(viewer->current_pixel));
    
    GtkBox* upper_box = GTK_BOX(gtk_box_new(GTK_ORIENTATION_VERTICAL, 5));
    gtk_box_pack_start(upper_box, box, false, false, 0);
    gtk_box_pack_start(upper_box, GTK_WIDGET(overlay), true, true, 0);
    
    GtkWidget* shader_editor_window = create_scrolled_window(GTK_WIDGET(viewer->shader_editor));
    GtkWidget* info_log_window = create_scrolled_window(GTK_WIDGET(viewer->shader_info_log));
    
    GtkPaned* lower_paned = GTK_PANED(gtk_paned_new(GTK_ORIENTATION_VERTICAL));
    gtk_paned_pack1(lower_paned, shader_editor_window, true, true);
    gtk_paned_pack2(lower_paned, info_log_window, false, true);
    
    viewer->widget = gtk_paned_new(GTK_ORIENTATION_VERTICAL);
    gtk_paned_pack1(GTK_PANED(viewer->widget), GTK_WIDGET(upper_box), true, true);
    gtk_paned_pack2(GTK_PANED(viewer->widget), GTK_WIDGET(lower_paned), false, true);
    
    //gtk_widget_set_size_request(GTK_WIDGET(upper_box), -1, 128);
    gtk_widget_set_size_request(GTK_WIDGET(lower_paned), -1, 96);
    gtk_widget_set_size_request(viewer->widget, -1, 256);
    g_signal_connect(viewer->widget, "destroy", G_CALLBACK(image_viewer_destroyed), viewer);
    
    return viewer;
}

void clear_image_viewer(image_viewer_t* viewer) {
    viewer->data_offset = 0;
    free(viewer->data);
    free(viewer->data_data);
    viewer->data = NULL;
    viewer->data_data = NULL;
    viewer->format = TrcImageFormat_Red_U32;
    viewer->image_width = 0;
    viewer->image_height = 0;
    viewer->texture_dirty = true;
    
    gtk_widget_queue_draw(GTK_WIDGET(viewer->gl_area));
}

void update_image_viewer(image_viewer_t* viewer, size_t offset,
                         trc_chunked_data_t data,
                         int dim[2], trc_image_format_t format) {
    viewer->data_offset = offset;
    free(viewer->data);
    free(viewer->data_data);
    viewer->data = malloc(sizeof(trc_chunked_data_t));
    *viewer->data = data;
    viewer->format = format;
    viewer->image_width = dim[0];
    viewer->image_height = dim[1];
    viewer->texture_dirty = true;
    
    gtk_widget_queue_draw(GTK_WIDGET(viewer->gl_area));
}
