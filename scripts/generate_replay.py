#!/usr/bin/env python
# -*- coding: utf-8 -*-
import glapi.glxml
from gl_state import *
from glapi.glapi import *

gl = glapi.glxml.GL(False)

output = open("../src/libtrace/replay_gl.c", "w")

output.write("""#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <SDL2/SDL.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <stdio.h>
#include <assert.h>
#include "libtrace/libtrace.h"
#include "shared/glapi.h"

#define F(name) (((replay_gl_funcs_t*)ctx->_replay_gl)->real_##name)
#define RETURN do {replay_end_cmd(ctx, FUNC, cmd);return;} while(0)
#define ERROR(...) do {trc_add_error(cmd, __VA_ARGS__); RETURN;} while (0)
#define FUNC ""

typedef void (*func_t)();

""")

output.write(gl.typedecls)

output.write("""
//TODO: Identify types that are pointers like GLsync and change them to uint64_t like with GLsync
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-function"
static GLuint* gl_param_GLuint_array(trace_command_t* cmd, size_t index) {
    return NULL; //TODO
}

static const char* gl_param_string(trace_command_t* cmd, size_t index) {
    return *trc_get_str(trc_get_arg(cmd, index));
}

static const char*const* gl_param_string_array(trace_command_t* cmd, size_t index) {
    return trc_get_str(trc_get_arg(cmd, index));
}

static const void* gl_param_data(trace_command_t* cmd, size_t index) {
    return *trc_get_data(trc_get_arg(cmd, index));
}

static size_t gl_param_data_size(trace_command_t* cmd, size_t index) {
    return *trc_get_data_sizes(trc_get_arg(cmd, index));
}

static uint64_t gl_param_pointer(trace_command_t* cmd, size_t index) {
    return *trc_get_ptr(trc_get_arg(cmd, index));
}

static GLDEBUGPROC gl_param_GLDEBUGPROC(trace_command_t* cmd, size_t index) {
    return NULL; //TODO
}

static GLsizei gl_param_GLsizei(trace_command_t* cmd, size_t index) {
    return *trc_get_int(trc_get_arg(cmd, index));
}

static GLint64EXT gl_param_GLint64EXT(trace_command_t* cmd, size_t index) {
    return *trc_get_int(trc_get_arg(cmd, index));
}

static GLshort gl_param_GLshort(trace_command_t* cmd, size_t index) {
    return *trc_get_int(trc_get_arg(cmd, index));
}

static int64_t gl_param_int64_t(trace_command_t* cmd, size_t index) {
    return *trc_get_int(trc_get_arg(cmd, index));
}

static GLubyte gl_param_GLubyte(trace_command_t* cmd, size_t index) {
    return *trc_get_uint(trc_get_arg(cmd, index));
}

static GLDEBUGPROCARB gl_param_GLDEBUGPROCARB(trace_command_t* cmd, size_t index) {
    return NULL; //TODO
}

static GLboolean gl_param_GLboolean(trace_command_t* cmd, size_t index) {
    return *trc_get_bool(trc_get_arg(cmd, index));
}

static Bool gl_param_Bool(trace_command_t* cmd, size_t index) {
    return *trc_get_bool(trc_get_arg(cmd, index));
}

static GLbitfield gl_param_GLbitfield(trace_command_t* cmd, size_t index) {
    return *trc_get_uint(trc_get_arg(cmd, index));
}

static GLuint gl_param_GLuint(trace_command_t* cmd, size_t index) {
    return *trc_get_uint(trc_get_arg(cmd, index));
}

static GLint64 gl_param_GLint64(trace_command_t* cmd, size_t index) {
    return *trc_get_int(trc_get_arg(cmd, index));
}

static int gl_param_int(trace_command_t* cmd, size_t index) {
    return *trc_get_int(trc_get_arg(cmd, index));
}

static uint64_t gl_param_GLeglImageOES(trace_command_t* cmd, size_t index) {
    return *trc_get_ptr(trc_get_arg(cmd, index));
}

static GLfixed gl_param_GLfixed(trace_command_t* cmd, size_t index) {
    return *trc_get_double(trc_get_arg(cmd, index)) * 65546.0f;
}

static GLclampf gl_param_GLclampf(trace_command_t* cmd, size_t index) {
    return *trc_get_double(trc_get_arg(cmd, index));
}

static float gl_param_float(trace_command_t* cmd, size_t index) {
    return *trc_get_double(trc_get_arg(cmd, index));
}

static GLhalfNV gl_param_GLhalfNV(trace_command_t* cmd, size_t index) { //TODO
    return *trc_get_uint(trc_get_arg(cmd, index));
}

static int64_t gl_param_GLintptr(trace_command_t* cmd, size_t index) {
    return *trc_get_int(trc_get_arg(cmd, index));
}

static GLushort gl_param_GLushort(trace_command_t* cmd, size_t index) {
    return *trc_get_uint(trc_get_arg(cmd, index));
}

static GLenum gl_param_GLenum(trace_command_t* cmd, size_t index) {
    return *trc_get_uint(trc_get_arg(cmd, index));
}

static GLuint gl_param_unsigned_int(trace_command_t* cmd, size_t index) {
    return *trc_get_uint(trc_get_arg(cmd, index));
}

static GLfloat gl_param_GLfloat(trace_command_t* cmd, size_t index) {
    return *trc_get_double(trc_get_arg(cmd, index));
}

static GLuint64 gl_param_GLuint64(trace_command_t* cmd, size_t index) {
    return *trc_get_uint(trc_get_arg(cmd, index));
}

static GLdouble gl_param_GLdouble(trace_command_t* cmd, size_t index) {
    return *trc_get_double(trc_get_arg(cmd, index));
}

static GLhandleARB gl_param_GLhandleARB(trace_command_t* cmd, size_t index) {
    return *trc_get_uint(trc_get_arg(cmd, index));
}

static int64_t gl_param_GLintptrARB(trace_command_t* cmd, size_t index) {
    return *trc_get_int(trc_get_arg(cmd, index));
}

static int64_t gl_param_GLsizeiptr(trace_command_t* cmd, size_t index)
{
    return *trc_get_int(trc_get_arg(cmd, index));
}

static GLint gl_param_GLint(trace_command_t* cmd, size_t index)
{
    return *trc_get_int(trc_get_arg(cmd, index));
}

static GLclampx gl_param_GLclampx(trace_command_t* cmd, size_t index) {
    return *trc_get_int(trc_get_arg(cmd, index));
}

static GLsizeiptrARB gl_param_GLsizeiptrARB(trace_command_t* cmd, size_t index) {
    return *trc_get_ptr(trc_get_arg(cmd, index));
}

static GLuint64EXT gl_param_GLuint64EXT(trace_command_t* cmd, size_t index) {
    return *trc_get_uint(trc_get_arg(cmd, index));
}

static uint64_t gl_param_GLvdpauSurfaceNV(trace_command_t* cmd, size_t index) {
    return *trc_get_ptr(trc_get_arg(cmd, index));
}

static GLbyte gl_param_GLbyte(trace_command_t* cmd, size_t index) {
    return *trc_get_int(trc_get_arg(cmd, index));
}

static GLclampd gl_param_GLclampd(trace_command_t* cmd, size_t index) {
    return *trc_get_double(trc_get_arg(cmd, index));
}

static GLDEBUGPROCKHR gl_param_GLDEBUGPROCKHR(trace_command_t* cmd, size_t index) {
    return NULL; //TODO
}

static GLDEBUGPROCAMD gl_param_GLDEBUGPROCAMD(trace_command_t* cmd, size_t index) {
    return NULL; //TODO
}

static GLXPixmap gl_param_GLXPixmap(trace_command_t* cmd, size_t index) {
    return *trc_get_uint(trc_get_arg(cmd, index));
}

static GLXWindow gl_param_GLXWindow(trace_command_t* cmd, size_t index) {
    return *trc_get_uint(trc_get_arg(cmd, index));
}

static GLXPbuffer gl_param_GLXPbuffer(trace_command_t* cmd, size_t index) {
    return *trc_get_uint(trc_get_arg(cmd, index));
}

static GLXDrawable gl_param_GLXDrawable(trace_command_t* cmd, size_t index) {
    return *trc_get_uint(trc_get_arg(cmd, index));
}

static GLXVideoDeviceNV gl_param_GLXVideoDeviceNV(trace_command_t* cmd, size_t index) {
    return *trc_get_uint(trc_get_arg(cmd, index));
}

static Pixmap gl_param_Pixmap(trace_command_t* cmd, size_t index) {
    return *trc_get_uint(trc_get_arg(cmd, index));
}

static Window gl_param_Window(trace_command_t* cmd, size_t index) {
    return *trc_get_uint(trc_get_arg(cmd, index));
}

static Font gl_param_Font(trace_command_t* cmd, size_t index) {
    return *trc_get_uint(trc_get_arg(cmd, index));
}

static Colormap gl_param_Colormap(trace_command_t* cmd, size_t index) {
    return *trc_get_uint(trc_get_arg(cmd, index));
}

static GLXContextID gl_param_GLXContextID(trace_command_t* cmd, size_t index) {
    return *trc_get_uint(trc_get_arg(cmd, index));
}

static uint64_t gl_param_GLXFBConfig(trace_command_t* cmd, size_t index) {
    return *trc_get_ptr(trc_get_arg(cmd, index));
}

static GLXVideoCaptureDeviceNV gl_param_GLXVideoCaptureDeviceNV(trace_command_t* cmd, size_t index) {
    return *trc_get_uint(trc_get_arg(cmd, index));
}

static uint64_t gl_param_GLXFBConfigSGIX(trace_command_t* cmd, size_t index) {
    return *trc_get_ptr(trc_get_arg(cmd, index));
}

static GLXPbufferSGIX gl_param_GLXPbufferSGIX(trace_command_t* cmd, size_t index) {
    return *trc_get_uint(trc_get_arg(cmd, index));
}

static GLXVideoSourceSGIX gl_param_GLXVideoSourceSGIX(trace_command_t* cmd, size_t index) {
    return *trc_get_uint(trc_get_arg(cmd, index));
}

static uint64_t gl_param_GLXContext(trace_command_t* cmd, size_t index) {
    return *trc_get_ptr(trc_get_arg(cmd, index));
}

static uint64_t replay_get_uint(trc_replay_context_t* ctx, trace_value_t* val, size_t i) {
    return trc_get_uint(val)[i];
}

static int64_t replay_get_int(trc_replay_context_t* ctx, trace_value_t* val, size_t i) {
    return trc_get_int(val)[i];
}

static double replay_get_double(trc_replay_context_t* ctx, trace_value_t* val, size_t i) {
    return trc_get_double(val)[i];
}

static uint64_t replay_get_ptr(trc_replay_context_t* ctx, trace_value_t* val, size_t i) {
    return trc_get_ptr(val)[i];
}

static bool replay_get_bool(trc_replay_context_t* ctx, trace_value_t* val, size_t i) {
    return trc_get_bool(val)[i];
}

static size_t replay_alloc_data_size;
static uint8_t replay_alloc_data[65536];
static void* replay_alloc_big_data;

static void* replay_alloc(size_t amount) {
    if (sizeof(replay_alloc_data)-replay_alloc_data_size < amount) {
        void* res = malloc(sizeof(void*)+amount);
        *(void**)res = replay_alloc_big_data;
        replay_alloc_big_data = res;
        return res;
    } else {
        uint8_t* res = &replay_alloc_data[replay_alloc_data_size];
        replay_alloc_data_size += amount;
        return res;
    }
}
#pragma GCC diagnostic pop

static void reset_gl_funcs(trc_replay_context_t* ctx);
static void reload_gl_funcs(trc_replay_context_t* ctx);

extern func_t glXGetProcAddress(const GLubyte* procName);

""")

output.write("\n")

for name in func_dict.keys():
    function = gl.functions[name]
    params = []
    for param in function.params:
        params.append("%s" % (param.type_))
    
    output.write("typedef %s (*%s_t)(%s);\n" % (function.returnType, name, ", ".join(params)))

output.write("typedef struct {\n")
for name in func_dict.keys():
    output.write("    %s_t real_%s;\n" % (name, name))
output.write("} replay_gl_funcs_t;\n\n")

for k, v in gl.enumValues.iteritems():
    output.write("#define %s %s\n" % (k, v))

output.write("""
static void debug_callback(GLenum source,
                           GLenum type,
                           GLuint id,
                           GLenum severity,
                           GLsizei length,
                           const GLchar* message,
                           const void* user_param) {
    const char* source_str = "unknown";
    
    switch (source) {
    case GL_DEBUG_SOURCE_API: {
        source_str = "API";
        break;
    }
    case GL_DEBUG_SOURCE_WINDOW_SYSTEM: {
        source_str = "system";
        break;
    }
    case GL_DEBUG_SOURCE_SHADER_COMPILER: {
        source_str = "shader compiler";
        break;
    }
    case GL_DEBUG_SOURCE_THIRD_PARTY: {
        source_str = "third party";
        break;
    }
    case GL_DEBUG_SOURCE_APPLICATION: {
        source_str = "application";
        break;
    }
    case GL_DEBUG_SOURCE_OTHER: {
        source_str = "other";
        break;
    }
    }
    
    switch (type) {
    case GL_DEBUG_TYPE_ERROR: {
        trc_add_error((trace_command_t*)user_param, "Error: '%s' from %s", message, source_str);
        break;
    }
    case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR: {
        trc_add_warning((trace_command_t*)user_param, "Deprecated behavior warning: '%s' from %s", message, source_str);
        break;
    }
    case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR: {
        trc_add_warning((trace_command_t*)user_param, "Undefined behavior warning: '%s' from %s", message, source_str);
        break;
    }
    case GL_DEBUG_TYPE_PORTABILITY: {
        trc_add_warning((trace_command_t*)user_param, "Portibility warning: '%s' from %s", message, source_str);
        break;
    }
    case GL_DEBUG_TYPE_PERFORMANCE: {
        trc_add_warning((trace_command_t*)user_param, "Performance warning: '%s' from %s", message, source_str);
        break;
    }
    case GL_DEBUG_TYPE_OTHER: {
        trc_add_warning((trace_command_t*)user_param, "Other: '%s' from %s", message, source_str);
        break;
    }
    case GL_DEBUG_TYPE_MARKER: {
        trc_add_info((trace_command_t*)user_param, "Marker: '%s' from %s", message, source_str);
        break;
    }
    }
}

static bool sample_param_double(trace_command_t* command, trc_gl_sample_params_t* params,
                                GLenum param, uint32_t count, const double* val) {
    switch (param) {
    case GL_TEXTURE_MIN_FILTER:
    case GL_TEXTURE_MAG_FILTER:
    case GL_TEXTURE_MIN_LOD:
    case GL_TEXTURE_MAX_LOD:
    case GL_TEXTURE_WRAP_S:
    case GL_TEXTURE_WRAP_T:
    case GL_TEXTURE_WRAP_R:
    case GL_TEXTURE_COMPARE_MODE:
    case GL_TEXTURE_COMPARE_FUNC:
        if (count != 1) {
            trc_add_error(command, "Expected 1 value. Got %u.", count);
            return true;
        }
        break;
    case GL_TEXTURE_BORDER_COLOR:
        if (count != 4) {
            trc_add_error(command, "Expected 4 values. Got %u.", count);
            return true;
        }
        break;
    }
    
    switch (param) {
    case GL_TEXTURE_MIN_FILTER:
        if (val[0]!=GL_LINEAR && val[0]!=GL_NEAREST && val[0]!=GL_NEAREST_MIPMAP_NEAREST &&
            val[0]!=GL_LINEAR_MIPMAP_NEAREST && val[0]!=GL_NEAREST_MIPMAP_LINEAR &&
            val[0]!=GL_LINEAR_MIPMAP_LINEAR) {
            trc_add_error(command, "Invalid minification filter.");
            return true;
        }
        break;
    case GL_TEXTURE_MAG_FILTER:
        if (val[0]!=GL_LINEAR && val[0]!=GL_NEAREST) {
            trc_add_error(command, "Invalid magnification filter.");
            return true;
        }
        break;
    case GL_TEXTURE_WRAP_S:
    case GL_TEXTURE_WRAP_T:
    case GL_TEXTURE_WRAP_R:
        if (val[0]!=GL_CLAMP_TO_EDGE && val[0]!=GL_CLAMP_TO_BORDER && val[0]!=GL_MIRRORED_REPEAT &&
            val[0]!=GL_REPEAT && val[0]!=GL_MIRROR_CLAMP_TO_EDGE && val[0]!=GL_CLAMP_TO_EDGE) {
            trc_add_error(command, "Invalid wrap mode.");
            return true;
        }
        break;
    case GL_TEXTURE_COMPARE_MODE:
        if (val[0]!=GL_COMPARE_REF_TO_TEXTURE && val[0]!=GL_NONE) {
            trc_add_error(command, "Invalid compare mode.");
            return true;
        }
        break;
    case GL_TEXTURE_COMPARE_FUNC:
        if (val[0]!=GL_LEQUAL && val[0]!=GL_GEQUAL && val[0]!=GL_LESS && val[0]!=GL_GREATER &&
            val[0]!=GL_EQUAL && val[0]!=GL_NOTEQUAL && val[0]!=GL_ALWAYS && val[0]!=GL_NEVER) {
            trc_add_error(command, "Invalid compare function.");
            return true;
        }
        break;
    }
    
    switch (param) {
    case GL_TEXTURE_MIN_FILTER: params->min_filter = val[0]; break;
    case GL_TEXTURE_MAG_FILTER: params->mag_filter = val[0]; break;
    case GL_TEXTURE_MIN_LOD: params->min_lod = val[0]; break;
    case GL_TEXTURE_MAX_LOD: params->max_lod = val[0]; break;
    case GL_TEXTURE_WRAP_S: params->wrap_s = val[0]; break;
    case GL_TEXTURE_WRAP_T: params->wrap_t = val[0]; break;
    case GL_TEXTURE_WRAP_R: params->wrap_r = val[0]; break;
    case GL_TEXTURE_BORDER_COLOR:
        for (uint i = 0; i < 4; i++) params->border_color[i] = val[i];
        break;
    case GL_TEXTURE_COMPARE_MODE: params->compare_mode = val[0]; break;
    case GL_TEXTURE_COMPARE_FUNC: params->compare_func = val[0]; break;
    }
    
    return false;
}

static void replay_create_context_buffers(trace_t* trace, trc_gl_context_rev_t* rev) {
    size_t size = rev->drawable_width * rev->drawable_height * 4;
    rev->front_color_buffer = trc_create_data(trace, size, NULL, TRC_DATA_NO_ZERO);
    rev->back_color_buffer = trc_create_data(trace, size, NULL, TRC_DATA_NO_ZERO);
    rev->back_depth_buffer = trc_create_data(trace, size, NULL, TRC_DATA_NO_ZERO);
    rev->back_stencil_buffer = trc_create_data(trace, size, NULL, TRC_DATA_NO_ZERO);
    
    void* data = trc_map_data(rev->front_color_buffer, TRC_MAP_REPLACE);
    memset(data, 0, size);
    trc_unmap_freeze_data(trace, rev->front_color_buffer);
    
    data = trc_map_data(rev->back_color_buffer, TRC_MAP_REPLACE);
    memset(data, 0, size);
    trc_unmap_freeze_data(trace, rev->back_color_buffer);
    
    data = trc_map_data(rev->back_depth_buffer, TRC_MAP_REPLACE);
    memset(data, 0, size);
    trc_unmap_freeze_data(trace, rev->back_depth_buffer);
    
    data = trc_map_data(rev->back_stencil_buffer, TRC_MAP_REPLACE);
    memset(data, 0, size);
    trc_unmap_freeze_data(trace, rev->back_stencil_buffer);
}

static void replay_update_buffers(trc_replay_context_t* ctx, bool backcolor, bool frontcolor, bool depth, bool stencil);

static void init_context(trc_replay_context_t* ctx) {
    trace_t* trace = ctx->trace;
    
    trc_gl_state_set_made_current_before(trace, false);
    
    GLint major, minor;
    F(glGetIntegerv)(GL_MAJOR_VERSION, &major);
    F(glGetIntegerv)(GL_MINOR_VERSION, &minor);
    uint ver = major*100 + minor*10;
    
    int w, h;
    SDL_GL_GetDrawableSize(ctx->window, &w, &h);
    trc_gl_state_set_drawable_width(trace, w);
    trc_gl_state_set_drawable_height(trace, h);
    trc_gl_state_set_bound_buffer(trace, GL_ARRAY_BUFFER, 0);
    trc_gl_state_set_bound_buffer(trace, GL_ATOMIC_COUNTER_BUFFER, 0);
    trc_gl_state_set_bound_buffer(trace, GL_COPY_READ_BUFFER, 0);
    trc_gl_state_set_bound_buffer(trace, GL_COPY_WRITE_BUFFER, 0);
    trc_gl_state_set_bound_buffer(trace, GL_DISPATCH_INDIRECT_BUFFER, 0);
    trc_gl_state_set_bound_buffer(trace, GL_DRAW_INDIRECT_BUFFER, 0);
    trc_gl_state_set_bound_buffer(trace, GL_ELEMENT_ARRAY_BUFFER, 0);
    trc_gl_state_set_bound_buffer(trace, GL_PIXEL_PACK_BUFFER, 0);
    trc_gl_state_set_bound_buffer(trace, GL_PIXEL_UNPACK_BUFFER, 0);
    trc_gl_state_set_bound_buffer(trace, GL_QUERY_BUFFER, 0);
    trc_gl_state_set_bound_buffer(trace, GL_SHADER_STORAGE_BUFFER, 0);
    trc_gl_state_set_bound_buffer(trace, GL_TEXTURE_BUFFER, 0);
    trc_gl_state_set_bound_buffer(trace, GL_TRANSFORM_FEEDBACK_BUFFER, 0);
    trc_gl_state_set_bound_buffer(trace, GL_UNIFORM_BUFFER, 0);
    trc_gl_state_set_bound_program(trace, 0);
    trc_gl_state_set_bound_vao(trace, 0);
    trc_gl_state_set_bound_renderbuffer(trace, 0);
    trc_gl_state_set_read_framebuffer(trace, 0);
    trc_gl_state_set_draw_framebuffer(trace, 0);
    trc_gl_state_set_active_texture_unit(trace, 0);
    
    GLint max_query_bindings = 64; //TODO
    trc_gl_state_bound_queries_init(trace, GL_SAMPLES_PASSED, max_query_bindings, NULL);
    trc_gl_state_bound_queries_init(trace, GL_ANY_SAMPLES_PASSED, max_query_bindings, NULL);
    trc_gl_state_bound_queries_init(trace, GL_ANY_SAMPLES_PASSED_CONSERVATIVE, max_query_bindings, NULL);
    trc_gl_state_bound_queries_init(trace, GL_PRIMITIVES_GENERATED, max_query_bindings, NULL);
    trc_gl_state_bound_queries_init(trace, GL_TRANSFORM_FEEDBACK_PRIMITIVES_WRITTEN, max_query_bindings, NULL);
    trc_gl_state_bound_queries_init(trace, GL_TIME_ELAPSED, max_query_bindings, NULL);
    
    GLint max_clip_distances, max_draw_buffers, max_viewports;
    GLint max_vertex_attribs, max_color_attachments, max_tex_units;
    GLint max_uniform_buffer_bindings;
    F(glGetIntegerv)(GL_MAX_CLIP_DISTANCES, &max_clip_distances);
    F(glGetIntegerv)(GL_MAX_DRAW_BUFFERS, &max_draw_buffers);
    if (ver>=410) F(glGetIntegerv)(GL_MAX_VIEWPORTS, &max_viewports);
    else max_viewports = 1;
    F(glGetIntegerv)(GL_MAX_VERTEX_ATTRIBS, &max_vertex_attribs);
    F(glGetIntegerv)(GL_MAX_COLOR_ATTACHMENTS, &max_color_attachments);
    F(glGetIntegerv)(GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS, &max_tex_units);
    F(glGetIntegerv)(GL_MAX_UNIFORM_BUFFER_BINDINGS, &max_uniform_buffer_bindings);
    
    trc_gl_state_state_int_init1(trace, GL_MAX_CLIP_DISTANCES, max_clip_distances);
    trc_gl_state_state_int_init1(trace, GL_MAX_DRAW_BUFFERS, max_draw_buffers);
    trc_gl_state_state_int_init1(trace, GL_MAX_VIEWPORTS, max_viewports);
    trc_gl_state_state_int_init1(trace, GL_MAX_VERTEX_ATTRIBS, max_vertex_attribs);
    trc_gl_state_state_int_init1(trace, GL_MAX_COLOR_ATTACHMENTS, max_color_attachments);
    trc_gl_state_state_int_init1(trace, GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS, max_tex_units);
    trc_gl_state_state_int_init1(trace, GL_MAX_UNIFORM_BUFFER_BINDINGS, max_uniform_buffer_bindings);
    
    trc_gl_state_bound_textures_init(trace, GL_TEXTURE_1D, max_tex_units, NULL);
    trc_gl_state_bound_textures_init(trace, GL_TEXTURE_2D, max_tex_units, NULL);
    trc_gl_state_bound_textures_init(trace, GL_TEXTURE_3D, max_tex_units, NULL);
    trc_gl_state_bound_textures_init(trace, GL_TEXTURE_1D_ARRAY, max_tex_units, NULL);
    trc_gl_state_bound_textures_init(trace, GL_TEXTURE_2D_ARRAY, max_tex_units, NULL);
    trc_gl_state_bound_textures_init(trace, GL_TEXTURE_RECTANGLE, max_tex_units, NULL);
    trc_gl_state_bound_textures_init(trace, GL_TEXTURE_CUBE_MAP, max_tex_units, NULL);
    trc_gl_state_bound_textures_init(trace, GL_TEXTURE_CUBE_MAP_ARRAY, max_tex_units, NULL);
    trc_gl_state_bound_textures_init(trace, GL_TEXTURE_BUFFER, max_tex_units, NULL);
    trc_gl_state_bound_textures_init(trace, GL_TEXTURE_2D_MULTISAMPLE, max_tex_units, NULL);
    trc_gl_state_bound_textures_init(trace, GL_TEXTURE_2D_MULTISAMPLE_ARRAY, max_tex_units, NULL);
    
    trc_gl_state_enabled_init(trace, GL_BLEND, max_draw_buffers, NULL);
    trc_gl_state_enabled_init(trace, GL_CLIP_DISTANCE0, max_clip_distances, NULL);
    trc_gl_state_enabled_init1(trace, GL_COLOR_LOGIC_OP, false);
    trc_gl_state_enabled_init1(trace, GL_CULL_FACE, false);
    trc_gl_state_enabled_init1(trace, GL_DEBUG_OUTPUT, false);
    trc_gl_state_enabled_init1(trace, GL_DEBUG_OUTPUT_SYNCHRONOUS, false);
    trc_gl_state_enabled_init1(trace, GL_DEPTH_CLAMP, false);
    trc_gl_state_enabled_init1(trace, GL_DEPTH_TEST, false);
    trc_gl_state_enabled_init1(trace, GL_DITHER, true);
    trc_gl_state_enabled_init1(trace, GL_FRAMEBUFFER_SRGB, false);
    trc_gl_state_enabled_init1(trace, GL_LINE_SMOOTH, false);
    trc_gl_state_enabled_init1(trace, GL_MULTISAMPLE, false);
    trc_gl_state_enabled_init1(trace, GL_POLYGON_OFFSET_FILL, false);
    trc_gl_state_enabled_init1(trace, GL_POLYGON_OFFSET_LINE, false);
    trc_gl_state_enabled_init1(trace, GL_POLYGON_OFFSET_POINT, false);
    trc_gl_state_enabled_init1(trace, GL_POLYGON_SMOOTH, false);
    trc_gl_state_enabled_init1(trace, GL_PRIMITIVE_RESTART, false);
    trc_gl_state_enabled_init1(trace, GL_PRIMITIVE_RESTART_FIXED_INDEX, false);
    trc_gl_state_enabled_init1(trace, GL_RASTERIZER_DISCARD, false);
    trc_gl_state_enabled_init1(trace, GL_SAMPLE_ALPHA_TO_COVERAGE, false);
    trc_gl_state_enabled_init1(trace, GL_SAMPLE_ALPHA_TO_ONE, false);
    trc_gl_state_enabled_init1(trace, GL_SAMPLE_COVERAGE, false);
    trc_gl_state_enabled_init1(trace, GL_SAMPLE_SHADING, false);
    trc_gl_state_enabled_init1(trace, GL_SAMPLE_MASK, false);
    trc_gl_state_enabled_init(trace, GL_SCISSOR_TEST, max_viewports, NULL);
    trc_gl_state_enabled_init1(trace, GL_STENCIL_TEST, false);
    trc_gl_state_enabled_init1(trace, GL_TEXTURE_CUBE_MAP_SEAMLESS, false);
    trc_gl_state_enabled_init1(trace, GL_PROGRAM_POINT_SIZE, false);
    
    trc_gl_state_state_bool_init1(trace, GL_DEPTH_WRITEMASK, GL_TRUE);
    bool color_mask[max_draw_buffers*4];
    for (size_t i = 0; i < max_draw_buffers*4; i++) color_mask[i] = GL_TRUE;
    trc_gl_state_state_bool_init(trace, GL_COLOR_WRITEMASK, max_draw_buffers*4, color_mask);
    trc_gl_state_state_int_init1(trace, GL_STENCIL_WRITEMASK, 0xffffffff);
    trc_gl_state_state_int_init1(trace, GL_STENCIL_BACK_WRITEMASK, 0xffffffff);
    
    trc_gl_state_state_bool_init1(trace, GL_PACK_SWAP_BYTES, GL_FALSE);
    trc_gl_state_state_bool_init1(trace, GL_PACK_LSB_FIRST, GL_FALSE);
    trc_gl_state_state_bool_init1(trace, GL_UNPACK_SWAP_BYTES, GL_FALSE);
    trc_gl_state_state_bool_init1(trace, GL_UNPACK_LSB_FIRST, GL_FALSE);
    
    trc_gl_state_state_int_init1(trace, GL_PACK_ROW_LENGTH, 0);
    trc_gl_state_state_int_init1(trace, GL_PACK_IMAGE_HEIGHT, 0);
    trc_gl_state_state_int_init1(trace, GL_PACK_SKIP_ROWS, 0);
    trc_gl_state_state_int_init1(trace, GL_PACK_SKIP_PIXELS, 0);
    trc_gl_state_state_int_init1(trace, GL_PACK_SKIP_IMAGES, 0);
    trc_gl_state_state_int_init1(trace, GL_PACK_ALIGNMENT, 4);
    trc_gl_state_state_int_init1(trace, GL_UNPACK_ROW_LENGTH, 0);
    trc_gl_state_state_int_init1(trace, GL_UNPACK_IMAGE_HEIGHT, 0);
    trc_gl_state_state_int_init1(trace, GL_UNPACK_SKIP_ROWS, 0);
    trc_gl_state_state_int_init1(trace, GL_UNPACK_SKIP_PIXELS, 0);
    trc_gl_state_state_int_init1(trace, GL_UNPACK_SKIP_IMAGES, 0);
    trc_gl_state_state_int_init1(trace, GL_UNPACK_ALIGNMENT, 4);
    
    trc_gl_state_state_int_init1(trace, GL_STENCIL_CLEAR_VALUE, 0);
    trc_gl_state_state_float_init1(trace, GL_DEPTH_CLEAR_VALUE, 0);
    float color_clear[4] = {0.0f, 0.0f, 0.0f, 0.0f};
    trc_gl_state_state_float_init(trace, GL_COLOR_CLEAR_VALUE, 4, color_clear);
    
    float depth_range[4] = {0.0f, 1.0f};
    trc_gl_state_state_float_init(trace, GL_DEPTH_RANGE, 2, depth_range);
    
    trc_gl_state_state_float_init1(trace, GL_POINT_SIZE, 1.0f);
    trc_gl_state_state_float_init1(trace, GL_LINE_WIDTH, 1.0f);
    
    trc_gl_state_state_float_init1(trace, GL_POLYGON_OFFSET_UNITS, 0.0f);
    trc_gl_state_state_float_init1(trace, GL_POLYGON_OFFSET_FACTOR, 0.0f);
    
    trc_gl_state_state_float_init1(trace, GL_SAMPLE_COVERAGE_VALUE, 1.0f);
    trc_gl_state_state_bool_init1(trace, GL_SAMPLE_COVERAGE_INVERT, false);
    
    trc_gl_state_state_enum_init1(trace, GL_STENCIL_FUNC, GL_ALWAYS);
    trc_gl_state_state_int_init1(trace, GL_STENCIL_REF, 0);
    trc_gl_state_state_int_init1(trace, GL_STENCIL_VALUE_MASK, 0xffffffff);
    trc_gl_state_state_enum_init1(trace, GL_STENCIL_BACK_FUNC, GL_ALWAYS);
    trc_gl_state_state_int_init1(trace, GL_STENCIL_BACK_REF, 0);
    trc_gl_state_state_int_init1(trace, GL_STENCIL_BACK_VALUE_MASK, 0xffffffff);
    trc_gl_state_state_enum_init1(trace, GL_STENCIL_FAIL, GL_KEEP);
    trc_gl_state_state_enum_init1(trace, GL_STENCIL_PASS_DEPTH_PASS, GL_KEEP);
    trc_gl_state_state_enum_init1(trace, GL_STENCIL_PASS_DEPTH_FAIL, GL_KEEP);
    trc_gl_state_state_enum_init1(trace, GL_STENCIL_BACK_FAIL, GL_KEEP);
    trc_gl_state_state_enum_init1(trace, GL_STENCIL_BACK_PASS_DEPTH_PASS, GL_KEEP);
    trc_gl_state_state_enum_init1(trace, GL_STENCIL_BACK_PASS_DEPTH_FAIL, GL_KEEP);
    
    GLenum blenddata[max_draw_buffers];
    for (size_t i = 0; i < max_draw_buffers; i++) blenddata[i] = GL_ONE;
    trc_gl_state_state_enum_init(trace, GL_BLEND_SRC_RGB, max_draw_buffers, blenddata);
    trc_gl_state_state_enum_init(trace, GL_BLEND_SRC_ALPHA, max_draw_buffers, blenddata);
    
    for (size_t i = 0; i < max_draw_buffers; i++) blenddata[i] = GL_ZERO;
    trc_gl_state_state_enum_init(trace, GL_BLEND_DST_RGB, max_draw_buffers, blenddata);
    trc_gl_state_state_enum_init(trace, GL_BLEND_DST_ALPHA, max_draw_buffers, blenddata);
    
    float blend_color[4] = {0.0f, 0.0f, 0.0f, 0.0f};
    trc_gl_state_state_float_init(trace, GL_BLEND_COLOR, 4, blend_color);
    
    for (size_t i = 0; i < max_draw_buffers; i++) blenddata[i] = GL_FUNC_ADD;
    trc_gl_state_state_enum_init(trace, GL_BLEND_EQUATION_RGB, max_draw_buffers, blenddata);
    trc_gl_state_state_enum_init(trace, GL_BLEND_EQUATION_ALPHA, max_draw_buffers, blenddata);
    
    trc_gl_state_set_hints(trace, GL_FRAGMENT_SHADER_DERIVATIVE_HINT, GL_DONT_CARE);
    trc_gl_state_set_hints(trace, GL_LINE_SMOOTH_HINT, GL_DONT_CARE);
    trc_gl_state_set_hints(trace, GL_POLYGON_SMOOTH_HINT, GL_DONT_CARE);
    trc_gl_state_set_hints(trace, GL_TEXTURE_COMPRESSION_HINT, GL_DONT_CARE);
    
    float zerof[max_viewports*4];
    for (size_t i = 0; i < max_viewports*4; i++) zerof[i] = 0.0f;
    trc_gl_state_state_float_init(trace, GL_VIEWPORT, max_viewports*4, zerof);
    trc_gl_state_state_int_init(trace, GL_SCISSOR_BOX, max_viewports*4, NULL);
    
    trc_gl_state_state_enum_init1(trace, GL_PROVOKING_VERTEX, GL_LAST_VERTEX_CONVENTION);
    trc_gl_state_state_enum_init1(trace, GL_LOGIC_OP_MODE, GL_COPY);
    trc_gl_state_state_int_init1(trace, GL_PRIMITIVE_RESTART_INDEX, 0);
    trc_gl_state_state_enum_init1(trace, GL_POLYGON_MODE, GL_FILL);
    trc_gl_state_state_enum_init1(trace, GL_CULL_FACE_MODE, GL_BACK);
    trc_gl_state_state_enum_init1(trace, GL_FRONT_FACE, GL_CCW);
    trc_gl_state_state_enum_init1(trace, GL_DEPTH_FUNC, GL_LESS);
    trc_gl_state_state_float_init1(trace, GL_POINT_FADE_THRESHOLD_SIZE, GL_UPPER_LEFT);
    trc_gl_state_state_enum_init1(trace, GL_POINT_SPRITE_COORD_ORIGIN, GL_UPPER_LEFT);
    trc_gl_state_state_float_init1(trace, GL_MIN_SAMPLE_SHADING_VALUE, 1.0f); //TODO: What is the initial value?
    
    double* va = malloc((max_vertex_attribs-1)*4*sizeof(double));
    for (size_t i = 0; i < (max_vertex_attribs-1)*4; i++) va[i] = i%4==3 ? 1 : 0;
    trc_gl_state_state_double_init(trace, GL_CURRENT_VERTEX_ATTRIB, (max_vertex_attribs-1)*4, va);
    free(va);
    
    GLenum draw_buffers[1] = {GL_BACK};
    trc_gl_state_state_enum_init(trace, GL_DRAW_BUFFER, 1, draw_buffers);
    
    uint draw_vao;
    F(glGenVertexArrays)(1, &draw_vao);
    F(glBindVertexArray)(draw_vao);
    trc_gl_state_set_draw_vao(trace, draw_vao);
    
    //replay_create_context_buffers(ctx->trace, &rev);
    replay_update_buffers(ctx, true, true, true, true);
}

static void replay_pixel_store(trc_replay_context_t* ctx, trace_command_t* cmd, GLenum pname, GLint param) {
    switch (pname) {
    case GL_PACK_SWAP_BYTES:
    case GL_PACK_LSB_FIRST:
    case GL_UNPACK_SWAP_BYTES:
    case GL_UNPACK_LSB_FIRST: trc_gl_state_set_state_bool(ctx->trace, pname, 0, param!=0); break;
    case GL_PACK_ROW_LENGTH:
    case GL_PACK_IMAGE_HEIGHT:
    case GL_PACK_SKIP_ROWS:
    case GL_PACK_SKIP_PIXELS:
    case GL_PACK_SKIP_IMAGES:
    case GL_UNPACK_ROW_LENGTH:
    case GL_UNPACK_IMAGE_HEIGHT:
    case GL_UNPACK_SKIP_ROWS:
    case GL_UNPACK_SKIP_PIXELS:
    case GL_UNPACK_SKIP_IMAGES:
        if (param < 0) {trc_add_error(cmd,"param is negative"); return;}
        trc_gl_state_set_state_int(ctx->trace, pname, 0, param);
        break;
    case GL_PACK_ALIGNMENT:
    case GL_UNPACK_ALIGNMENT:
        if (param!=1 && param!=2 && param!=4 && param!=8) {
            trc_add_error(cmd,"param is not 1, 2, 4, or 8");
            return;
        }
        trc_gl_state_set_state_int(ctx->trace, pname, 0, param);
        break;
    default:
        trc_add_error(cmd, "Invalid parameter");
        return;
    }
    F(glPixelStorei)(pname, param);
}

static void replay_set_texture_image(trace_t* trace, uint fake, const trc_gl_texture_rev_t* rev, uint level, uint face,
                                     uint internal_format, uint width, uint height, uint depth, trc_data_t* data) {
    trc_gl_texture_image_t img;
    img.face = face;
    img.level = level;
    img.internal_format = internal_format;
    img.width = width;
    img.height = height;
    img.depth = depth;
    img.data = data;
    
    size_t img_count = rev->images->size / sizeof(trc_gl_texture_image_t);
    trc_gl_texture_image_t* newimages = malloc((img_count+1)*sizeof(trc_gl_texture_image_t));
    
    trc_gl_texture_image_t* images = trc_map_data(rev->images, TRC_MAP_READ);
    bool replaced = false;
    for (size_t i = 0; i < img_count; i++) {
        if (images[i].face==img.face && images[i].level==img.level) {
            newimages[i] = img;
            replaced = true;
        } else {
            newimages[i] = images[i];
        }
    }
    trc_unmap_data(rev->images);
    
    trc_gl_texture_rev_t newrev = *rev;
    if (!replaced) newimages[img_count++] = img;
    
    size_t size = img_count * sizeof(trc_gl_texture_image_t);
    newrev.images = trc_create_data_no_copy(trace, size, newimages, TRC_DATA_IMMUTABLE);
    
    trc_set_gl_texture(trace, fake, &newrev);
}

static void replay_update_tex_image(trc_replay_context_t* ctx, const trc_gl_texture_rev_t* tex,
                                    uint fake, uint level, uint face) {
    GLenum prevget;
    switch (tex->type) {
    case GL_TEXTURE_1D: prevget = GL_TEXTURE_BINDING_1D; break;
    case GL_TEXTURE_2D: prevget = GL_TEXTURE_BINDING_2D; break;
    case GL_TEXTURE_3D: prevget = GL_TEXTURE_BINDING_3D; break;
    case GL_TEXTURE_1D_ARRAY: prevget = GL_TEXTURE_BINDING_1D_ARRAY; break;
    case GL_TEXTURE_2D_ARRAY: prevget = GL_TEXTURE_BINDING_2D_ARRAY; break;
    case GL_TEXTURE_RECTANGLE: prevget = GL_TEXTURE_BINDING_RECTANGLE; break;
    case GL_TEXTURE_CUBE_MAP: prevget = GL_TEXTURE_BINDING_CUBE_MAP; break;
    case GL_TEXTURE_CUBE_MAP_ARRAY: prevget = GL_TEXTURE_BINDING_CUBE_MAP; break;
    case GL_TEXTURE_BUFFER: return; //TODO: Error
    case GL_TEXTURE_2D_MULTISAMPLE: return; //TODO: Handle
    case GL_TEXTURE_2D_MULTISAMPLE_ARRAY: return; //TODO: Handle
    }
    GLint prev;
    F(glGetIntegerv)(prevget, &prev);
    F(glBindTexture)(tex->type, tex->real);
    
    GLint width, height, depth, internal_format;
    F(glGetTexLevelParameteriv)(tex->type, level, GL_TEXTURE_WIDTH, &width);
    F(glGetTexLevelParameteriv)(tex->type, level, GL_TEXTURE_HEIGHT, &height);
    F(glGetTexLevelParameteriv)(tex->type, level, GL_TEXTURE_DEPTH, &depth);
    F(glGetTexLevelParameteriv)(tex->type, level, GL_TEXTURE_INTERNAL_FORMAT, &internal_format);
    if (!width) width = 1;
    if (!height) height = 1;
    if (!depth) depth = 1;
    
    uint dtype = 0; //0=uint32_t 1=int32_t 2=float 3=float+unused24+uint8
    uint ftype = 0; //0=normal 1=depth 2=stencil 3=depthstencil
    uint components = 0;
    switch (internal_format) {
    case GL_DEPTH_COMPONENT: dtype = 2; ftype = 1; components = 1; break;
    case GL_DEPTH_COMPONENT16: dtype = 2; ftype = 1; components = 1; break;
    case GL_DEPTH_COMPONENT24: dtype = 2; ftype = 1; components = 1; break;
    case GL_DEPTH_COMPONENT32: dtype = 2; ftype = 1; components = 1; break;
    case GL_DEPTH_COMPONENT32F: dtype = 2; ftype = 1; components = 1; break;
    case GL_DEPTH_STENCIL: dtype = 3; ftype = 3; components = 2; break;
    case GL_DEPTH24_STENCIL8: dtype = 3; ftype = 3; components = 2; break;
    case GL_DEPTH32F_STENCIL8: dtype = 3; ftype = 3; components = 2; break;
    case GL_STENCIL_INDEX: dtype = 0; ftype = 2; components = 1; break;
    case GL_STENCIL_INDEX1: dtype = 0; ftype = 2; components = 1; break;
    case GL_STENCIL_INDEX4: dtype = 0; ftype = 2; components = 1; break;
    case GL_STENCIL_INDEX8: dtype = 0; ftype = 2; components = 1; break;
    case GL_STENCIL_INDEX16: dtype = 0; ftype = 2; components = 1; break;
    case GL_RED: dtype = 2; components = 1; break;
    case GL_RG: dtype = 2; components = 2; break;
    case GL_RGB: dtype = 2; components = 3; break;
    case GL_RGBA: dtype = 2; components = 4; break;
    case GL_R8: dtype = 2; components = 1; break;
    case GL_R8_SNORM: dtype = 2; components = 1; break;
    case GL_R16: dtype = 2; components = 1; break;
    case GL_R16_SNORM: dtype = 2; components = 1; break;
    case GL_RG8: dtype = 2; components = 2; break;
    case GL_RG8_SNORM: dtype = 2; components = 2; break;
    case GL_RG16: dtype = 2; components = 2; break;
    case GL_RG16_SNORM: dtype = 2; components = 2; break;
    case GL_R3_G3_B2: dtype = 2; components = 3; break;
    case GL_RGB4: dtype = 2; components = 3; break;
    case GL_RGB5: dtype = 2; components = 3; break;
    case GL_RGB8: dtype = 2; components = 3; break;
    case GL_RGB8_SNORM: dtype = 2; components = 3; break;
    case GL_RGB10: dtype = 2; components = 3; break;
    case GL_RGB12: dtype = 2; components = 3; break;
    case GL_RGB16_SNORM: dtype = 2; components = 3; break;
    case GL_RGBA2: dtype = 2; components = 4; break;
    case GL_RGBA4: dtype = 2; components = 4; break;
    case GL_RGB5_A1: dtype = 2; components = 4; break;
    case GL_RGBA8: dtype = 2; components = 4; break;
    case GL_RGBA8_SNORM: dtype = 2; components = 4; break;
    case GL_RGB10_A2: dtype = 2; components = 4; break;
    case GL_RGB10_A2UI: dtype = 0; components = 4; break;
    case GL_RGBA12: dtype = 2; components = 4; break;
    case GL_RGBA16: dtype = 2; components = 4; break;
    case GL_SRGB8: dtype = 2; components = 3; break;
    case GL_SRGB8_ALPHA8: dtype = 2; components = 4; break;
    case GL_R16F: dtype = 2; components = 1; break;
    case GL_RG16F: dtype = 2; components = 2; break;
    case GL_RGB16F: dtype = 2; components = 3; break;
    case GL_RGBA16F: dtype = 2; components = 4; break;
    case GL_R32F: dtype = 2; components = 1; break;
    case GL_RG32F: dtype = 2; components = 2; break;
    case GL_RGB32F: dtype = 2; components = 3; break;
    case GL_RGBA32F: dtype = 2; components = 4; break;
    case GL_R11F_G11F_B10F: dtype = 2; components = 3; break;
    case GL_RGB9_E5: dtype = 2; components = 3; break;
    case GL_R8I: dtype = 1; components = 1; break;
    case GL_R8UI: dtype = 0; components = 1; break;
    case GL_R16I: dtype = 1; components = 1; break;
    case GL_R16UI: dtype = 0; components = 1; break;
    case GL_R32I: dtype = 1; components = 1; break;
    case GL_R32UI: dtype = 0; components = 1; break;
    case GL_RG8I: dtype = 1; components = 2; break;
    case GL_RG8UI: dtype = 0; components = 2; break;
    case GL_RG16I: dtype = 1; components = 2; break;
    case GL_RG16UI: dtype = 0; components = 2; break;
    case GL_RG32I: dtype = 1; components = 2; break;
    case GL_RG32UI: dtype = 0; components = 2; break;
    case GL_RGB8I: dtype = 1; components = 3; break;
    case GL_RGB8UI: dtype = 0; components = 3; break;
    case GL_RGB16I: dtype = 1; components = 3; break;
    case GL_RGB16UI: dtype = 0; components = 3; break;
    case GL_RGB32I: dtype = 1; components = 3; break;
    case GL_RGB32UI: dtype = 0; components = 3; break;
    case GL_RGBA8I: dtype = 1; components = 4; break;
    case GL_RGBA8UI: dtype = 0; components = 4; break;
    case GL_RGBA16I: dtype = 1; components = 4; break;
    case GL_RGBA16UI: dtype = 0; components = 4; break;
    case GL_RGBA32I: dtype = 1; components = 4; break;
    case GL_RGBA32UI: dtype = 0; components = 4; break;
    case GL_COMPRESSED_RED: dtype = 2; components = 1; break;
    case GL_COMPRESSED_RG: dtype = 2; components = 2; break;
    case GL_COMPRESSED_RGB: dtype = 2; components = 3; break;
    case GL_COMPRESSED_RGBA: dtype = 2; components = 4; break;
    case GL_COMPRESSED_SRGB: dtype = 2; components = 3; break;
    case GL_COMPRESSED_SRGB_ALPHA: dtype = 2; components = 4; break;
    case GL_COMPRESSED_RED_RGTC1: dtype = 2; components = 1; break;
    case GL_COMPRESSED_SIGNED_RED_RGTC1: dtype = 2; components = 1; break;
    case GL_COMPRESSED_RG_RGTC2: dtype = 2; components = 2; break;
    case GL_COMPRESSED_SIGNED_RG_RGTC2: dtype = 2; components = 2; break;
    case GL_COMPRESSED_RGBA_BPTC_UNORM: dtype = 2; components = 4; break;
    case GL_COMPRESSED_SRGB_ALPHA_BPTC_UNORM: dtype = 2; components = 4; break;
    case GL_COMPRESSED_RGB_BPTC_SIGNED_FLOAT: dtype = 2; components = 3; break;
    case GL_COMPRESSED_RGB_BPTC_UNSIGNED_FLOAT: dtype = 2; components = 3; break;
    default: assert(false);
    }
    
    size_t data_size = width * height * depth * components * (dtype==3?8:4);
    trc_data_t* data = trc_create_data(ctx->trace, data_size, NULL, TRC_DATA_NO_ZERO);
    void* dest = trc_map_data(data, TRC_MAP_REPLACE);
    
    GLenum format;
    switch (ftype) {
    case 0: //normal
        format = (GLenum[]){GL_RED, GL_RG, GL_RGB, GL_RGBA}[components-1];
        break;
    case 1: //depth
        format = GL_DEPTH_COMPONENT;
        break;
    case 2: //stencil
        format = GL_STENCIL_INDEX;
        break;
    case 3: //depth stencil
        format = GL_DEPTH_STENCIL;
        break;
    }
    GLenum type;
    switch (dtype) {
    case 0: //uint32_t
        type = GL_UNSIGNED_INT;
        break;
    case 1: //int32_t
        type = GL_INT;
        break;
    case 2: //float
        type = GL_FLOAT;
        break;
    case 3: //float+unused24+uint8
        type = GL_FLOAT_32_UNSIGNED_INT_24_8_REV;
        break;
    }
    uint target = tex->type;
    if (target==GL_TEXTURE_CUBE_MAP) target = GL_TEXTURE_CUBE_MAP_POSITIVE_X + face;
    F(glGetTexImage)(target, level, format, type, dest);
    F(glBindTexture)(tex->type, prev);
    
    trc_unmap_freeze_data(ctx->trace, data);
    
    replay_set_texture_image(ctx->trace, fake, tex, level, face, internal_format, width, height, depth, data);
}

void replay_update_bound_tex_image(trc_replay_context_t* ctx, trace_command_t* command, uint target, uint level) {
    uint unit = trc_gl_state_get_active_texture_unit(ctx->trace);
    uint fake = trc_gl_state_get_bound_textures(ctx->trace, target, unit);
    const trc_gl_texture_rev_t* rev = trc_get_gl_texture(ctx->trace, fake);
    if (!rev) {
        trc_add_error(command, "No texture bound or invalid target");
        return;
    }
    
    uint face = 0;
    if (target>=GL_TEXTURE_CUBE_MAP_POSITIVE_X && target<=GL_TEXTURE_CUBE_MAP_NEGATIVE_Z)
        face = target - GL_TEXTURE_CUBE_MAP_POSITIVE_X;
    replay_update_tex_image(ctx, rev, fake, level, face);
}

bool replay_append_fb_attachment(trace_t* trace, uint fb, const trc_gl_framebuffer_attachment_t* attach) {
    const trc_gl_framebuffer_rev_t* rev = trc_get_gl_framebuffer(trace, fb);
    if (!rev) return false;
    
    size_t attach_count = rev->attachments->size / sizeof(trc_gl_framebuffer_attachment_t);
    trc_gl_framebuffer_attachment_t* newattachs = malloc((attach_count+1)*sizeof(trc_gl_framebuffer_attachment_t));
    
    trc_gl_framebuffer_attachment_t* attachs = trc_map_data(rev->attachments, TRC_MAP_READ);
    bool replaced = false;
    for (size_t i = 0; i < attach_count; i++) {
        if (attachs[i].attachment == attach->attachment) {
            newattachs[i] = *attach;
            replaced = true;
        } else {
            newattachs[i] = attachs[i];
        }
    }
    trc_unmap_data(rev->attachments);
    
    trc_gl_framebuffer_rev_t newrev = *rev;
    if (!replaced) newattachs[attach_count++] = *attach;
    
    size_t size = attach_count * sizeof(trc_gl_framebuffer_attachment_t);
    newrev.attachments = trc_create_data_no_copy(trace, size, newattachs, TRC_DATA_IMMUTABLE);
    
    trc_set_gl_framebuffer(trace, fb, &newrev);
    
    return true;
}

void replay_add_fb_attachment(trace_t* trace, trace_command_t* cmd, uint fb, uint attachment,
                              uint tex, uint target, uint level, uint layer) {
    trc_gl_framebuffer_attachment_t attach;
    memset(&attach, 0, sizeof(attach));
    attach.has_renderbuffer = false;
    attach.attachment = attachment;
    attach.fake_texture = tex;
    attach.level = level;
    attach.layer = layer;
    attach.face = 0;
    if (target>=GL_TEXTURE_CUBE_MAP_POSITIVE_X && target<=GL_TEXTURE_CUBE_MAP_NEGATIVE_Z) {
        attach.face = target - GL_TEXTURE_CUBE_MAP_POSITIVE_X;
    } else if (target==GL_TEXTURE_CUBE_MAP_ARRAY || target==GL_TEXTURE_CUBE_MAP) {
        attach.face = layer % 6;
        attach.layer /= 6;
    }
    if (!replay_append_fb_attachment(trace, fb, &attach))
        //TODO: The framebuffer might not come from a binding
        trc_add_error(cmd, "No framebuffer bound or invalid target");
}

void replay_add_fb_attachment_rb(trace_t* trace, trace_command_t* cmd, uint fb, uint attachment, uint rb) {
    trc_gl_framebuffer_attachment_t attach;
    memset(&attach, 0, sizeof(attach));
    attach.has_renderbuffer = true;
    attach.attachment = attachment;
    attach.fake_renderbuffer = fb;
    if (!replay_append_fb_attachment(trace, fb, &attach))
        //TODO: The framebuffer might not come from a binding
        trc_add_error(cmd, "No framebuffer bound or invalid target");
}

void replay_update_renderbuffer(trc_replay_context_t* ctx, const trc_gl_renderbuffer_rev_t* rev,
                                uint fake, uint width, uint height, uint internal_format, uint samples) {
    GLint bits[6];
    GLint prev;
    F(glGetIntegerv)(GL_RENDERBUFFER_BINDING, &prev);
    F(glBindRenderbuffer)(GL_RENDERBUFFER, rev->real);
    F(glGetRenderbufferParameteriv)(GL_RENDERBUFFER, GL_RENDERBUFFER_RED_SIZE, &bits[0]);
    F(glGetRenderbufferParameteriv)(GL_RENDERBUFFER, GL_RENDERBUFFER_GREEN_SIZE, &bits[1]);
    F(glGetRenderbufferParameteriv)(GL_RENDERBUFFER, GL_RENDERBUFFER_BLUE_SIZE, &bits[2]);
    F(glGetRenderbufferParameteriv)(GL_RENDERBUFFER, GL_RENDERBUFFER_ALPHA_SIZE, &bits[3]);
    F(glGetRenderbufferParameteriv)(GL_RENDERBUFFER, GL_RENDERBUFFER_DEPTH_SIZE, &bits[4]);
    F(glGetRenderbufferParameteriv)(GL_RENDERBUFFER, GL_RENDERBUFFER_STENCIL_SIZE, &bits[5]);
    F(glBindRenderbuffer)(GL_RENDERBUFFER, prev);
    
    trc_gl_renderbuffer_rev_t newrev = *rev;
    newrev.width = width;
    newrev.height = height;
    newrev.internal_format = internal_format;
    newrev.sample_count = samples;
    for (size_t i = 0; i < 4; i++) newrev.rgba_bits[i] = bits[i];
    newrev.depth_bits = bits[4];
    newrev.stencil_bits = bits[5];
    newrev.has_storage = true;
    
    trc_set_gl_renderbuffer(ctx->trace, fake, &newrev);
}

//TODO or NOTE: Ensure that the border color is handled with integer glTexParameter(s)
//TODO: More validation
static bool texture_param_double(trc_replay_context_t* ctx, trace_command_t* command,
                                 GLenum target, GLenum param, uint32_t count, const double* val) {
    uint unit = trc_gl_state_get_active_texture_unit(ctx->trace);
    GLuint texid = trc_gl_state_get_bound_textures(ctx->trace, target, unit);
    const trc_gl_texture_rev_t* tex_ptr = trc_get_gl_texture(ctx->trace, texid);
    if (!tex_ptr) {
        trc_add_error(command, "No texture bound, invalid texture handle used or invalid target");
        return true;
    }
    trc_gl_texture_rev_t tex = *tex_ptr;
    
    switch (param) {
    case GL_TEXTURE_MIN_FILTER:
    case GL_TEXTURE_MAG_FILTER:
    case GL_TEXTURE_MIN_LOD:
    case GL_TEXTURE_MAX_LOD:
    case GL_TEXTURE_WRAP_S:
    case GL_TEXTURE_WRAP_T:
    case GL_TEXTURE_WRAP_R:
    case GL_TEXTURE_COMPARE_MODE:
    case GL_TEXTURE_COMPARE_FUNC:
    case GL_TEXTURE_BORDER_COLOR: {
        bool res = sample_param_double(command, &tex.sample_params, param, count, val);
        trc_set_gl_texture(ctx->trace, texid, &tex);
        return res;
    } case GL_DEPTH_STENCIL_TEXTURE_MODE:
    case GL_TEXTURE_BASE_LEVEL:
    case GL_TEXTURE_MAX_LEVEL:
    case GL_TEXTURE_LOD_BIAS:
    case GL_TEXTURE_SWIZZLE_R:
    case GL_TEXTURE_SWIZZLE_G:
    case GL_TEXTURE_SWIZZLE_B:
    case GL_TEXTURE_SWIZZLE_A: {
        if (count != 1) {
            trc_add_error(command, "Expected 1 value. Got %u.", count);
            return true;
        }
        break;
    } case GL_TEXTURE_SWIZZLE_RGBA: {
        if (count != 4) {
            trc_add_error(command, "Expected 4 values. Got %u.", count);
            return true;
        }
        break;
    }
    }
    
    switch (param) {
    case GL_DEPTH_STENCIL_TEXTURE_MODE:
        if (val[0]!=GL_DEPTH_COMPONENT && val[0]!=GL_STENCIL_INDEX) {
            trc_add_error(command, "Invalid depth stencil texture mode.");
            return true;
        }
        break;
    case GL_TEXTURE_SWIZZLE_R:
    case GL_TEXTURE_SWIZZLE_G:
    case GL_TEXTURE_SWIZZLE_B:
    case GL_TEXTURE_SWIZZLE_A:
        if (val[0]!=GL_RED && val[0]!=GL_GREEN && val[0]!=GL_BLUE && val[0]!=GL_ALPHA) {
            trc_add_error(command, "Invalid swizzle.");
            return true;
        }
        break;
    case GL_TEXTURE_SWIZZLE_RGBA:
        for (uint i = 0; i < 4; i++) {
            if (val[0]!=GL_RED && val[0]!=GL_GREEN && val[0]!=GL_BLUE && val[0]!=GL_ALPHA) {
                trc_add_error(command, "Invalid swizzle.");
                return true;
            }
        }
        break;
    }
    
    switch (param) {
    case GL_DEPTH_STENCIL_TEXTURE_MODE:
        tex.depth_stencil_mode = val[0];
        break;
    case GL_TEXTURE_BASE_LEVEL: tex.base_level = val[0]; break;
    case GL_TEXTURE_MAX_LEVEL: tex.max_level = val[0]; break;
    case GL_TEXTURE_LOD_BIAS: tex.lod_bias = val[0]; break;
    case GL_TEXTURE_SWIZZLE_R: tex.swizzle[0] = val[0]; break;
    case GL_TEXTURE_SWIZZLE_G: tex.swizzle[1] = val[0]; break;
    case GL_TEXTURE_SWIZZLE_B: tex.swizzle[2] = val[0]; break;
    case GL_TEXTURE_SWIZZLE_A: tex.swizzle[3] = val[0]; break;
    case GL_TEXTURE_SWIZZLE_RGBA:
        for (uint i = 0; i < 4; i++) tex.swizzle[i] = val[i];
        break;
    }
    
    trc_set_gl_texture(ctx->trace, texid, &tex);
    
    return false;
}

static GLuint get_bound_buffer(trc_replay_context_t* ctx, GLenum target) {
    return trc_gl_state_get_bound_buffer(ctx->trace, target); //TODO: Return 0 on invalid targets
}

static int uniform(trc_replay_context_t* ctx, trace_command_t* cmd, bool dsa,
                   bool array, uint dimx, uint dimy, GLenum type, void* data_,
                   uint* realprogram) {
    uint8_t* data = data_;
    
    uint arg_pos = 0;
    uint program;
    if (dsa) program = gl_param_GLuint(cmd, arg_pos++);
    else program = trc_gl_state_get_bound_program(ctx->trace);
    const trc_gl_program_rev_t* rev = trc_get_gl_program(ctx->trace, program);
    if (!rev) {
        trc_add_error(cmd, dsa?"Invalid program":"No current program");
        return -1;
    }
    if (realprogram) *realprogram = rev->real;
    
    int location = gl_param_GLint(cmd, arg_pos++);
    
    trc_gl_program_uniform_t uniform;
    
    size_t uniform_count = rev->uniforms->size /
                           sizeof(trc_gl_program_uniform_t);
    trc_gl_program_uniform_t* uniforms = trc_map_data(rev->uniforms, TRC_MAP_READ);
    uint uniform_index = 0;
    for (; uniform_index < uniform_count; uniform_index++) {
        if (uniforms[uniform_index].fake == location) {
            uniform = uniforms[uniform_index];
            goto success;
        }
    }
    trc_unmap_data(rev->uniforms);
    return -1;
    success: ;
    
    uint count = array ? gl_param_GLsizei(cmd, arg_pos++) : 1;
    bool transpose = dimy==1 ? false : gl_param_GLboolean(cmd, arg_pos++);
    
    uniform.value = trc_create_data(ctx->trace, count*dimx*dimy*sizeof(double), NULL, TRC_DATA_NO_ZERO);
    double* res = trc_map_data(uniform.value, TRC_MAP_REPLACE);
    for (uint i = 0; i < count; i++) {
        for (uint x = 0; x < dimx; x++) {
            for (uint y = 0; y < dimy; y++) {
                double val = 0;
                if (array) {
                    uint si = transpose ? y*dimx+x : x*dimy+y;
                    si += dimx * dimy * i;
                    switch (type) {
                    case GL_FLOAT:
                        val = trc_get_double(trc_get_arg(cmd, arg_pos))[si];
                        break;
                    case GL_DOUBLE:
                        val = trc_get_double(trc_get_arg(cmd, arg_pos))[si];
                        break;
                    case GL_INT:
                        val = trc_get_int(trc_get_arg(cmd, arg_pos))[si];
                        break;
                    case GL_UNSIGNED_INT:
                        val = trc_get_uint(trc_get_arg(cmd, arg_pos))[si];
                        break;
                    }
                } else {
                    switch (type) {
                    case GL_FLOAT:
                        val = gl_param_GLfloat(cmd, arg_pos++); break;
                    case GL_DOUBLE:
                        val = gl_param_GLdouble(cmd, arg_pos++); break;
                    case GL_INT:
                        val = gl_param_GLint(cmd, arg_pos++); break;
                    case GL_UNSIGNED_INT:
                        val = gl_param_GLuint(cmd, arg_pos++); break;
                    }
                }
                *res++ = val;
                if (data) {
                    switch (type) {
                    case GL_FLOAT: *(float*)data = val; data += sizeof(float); break;
                    case GL_DOUBLE: *(double*)data = val; data += sizeof(double); break;
                    case GL_INT: *(int*)data = val; data += sizeof(int); break;
                    case GL_UNSIGNED_INT: *(uint*)data = val; data += sizeof(uint); break;
                    }
                }
            }
        }
    }
    trc_unmap_freeze_data(ctx->trace, uniform.value);
    uniform.dim[0] = dimx;
    uniform.dim[1] = dimy;
    uniform.count = count;
    
    trc_data_t* new_uniforms = trc_create_data(ctx->trace, uniform_count*sizeof(trc_gl_program_uniform_t), uniforms, 0);
    trc_unmap_data(rev->uniforms);
    
    trc_gl_program_rev_t newrev = *rev;
    newrev.uniforms = new_uniforms;
    
    trc_gl_program_uniform_t* dest = trc_map_data(new_uniforms, TRC_MAP_MODIFY);
    dest[uniform_index] = uniform;
    trc_unmap_freeze_data(ctx->trace, new_uniforms);
    
    trc_set_gl_program(ctx->trace, trc_get_gl_context(ctx->trace, 0)->bound_program, &newrev);
    
    return uniform.real;
}

//type in [GL_FLOAT, GL_DOUBLE, GL_UNSIGNED_BYTE, GL_BYTE, GL_SHORT, GL_UNSIGNED_SHORT, GL_UNSIGNED_INT, GL_INT]
//internal in [GL_FLOAT, GL_DOUBLE, GL_UNSIGNED_INT, GL_INT]
static void vertex_attrib(trc_replay_context_t* ctx, trace_command_t* cmd, uint comp,
                          GLenum type, bool array, bool normalized, GLenum internal) {
    uint index = gl_param_GLuint(cmd, 0);
    if (index==0 || index>=trc_gl_state_get_state_int(ctx->trace, GL_MAX_VERTEX_ATTRIBS, 0)) {
        trc_add_error(cmd, "Invalid vertex attribute index");
        return;
    }
    index--;
    uint i = 0;
    for (; i < comp; i++) {
        double val = 0;
        if (array) {
            switch (type) {
            case GL_UNSIGNED_BYTE:
            case GL_UNSIGNED_SHORT:
            case GL_UNSIGNED_INT: val = trc_get_int(trc_get_arg(cmd, 1))[i]; break;
            case GL_BYTE:
            case GL_SHORT:
            case GL_INT: val = trc_get_uint(trc_get_arg(cmd, 1))[i]; break;
            case GL_FLOAT:
            case GL_DOUBLE: val = trc_get_double(trc_get_arg(cmd, 1))[i]; break;
            }
        } else {
            switch (type) {
            case GL_UNSIGNED_BYTE:
            case GL_UNSIGNED_SHORT:
            case GL_UNSIGNED_INT: val = trc_get_int(trc_get_arg(cmd, i+1))[0]; break;
            case GL_BYTE:
            case GL_SHORT:
            case GL_INT: val = trc_get_uint(trc_get_arg(cmd, i+1))[0]; break;
            case GL_FLOAT:
            case GL_DOUBLE: val = trc_get_double(trc_get_arg(cmd, i+1))[0]; break;
            }
        }
        if (internal==GL_FLOAT) val = (float)val;
        if (normalized) {
            switch (type) {
            case GL_UNSIGNED_BYTE: val /= UINT8_MAX; break;
            case GL_UNSIGNED_SHORT: val /= UINT16_MAX; break;
            case GL_UNSIGNED_INT: val /= UINT32_MAX; break;
            case GL_BYTE: val = val<0 ? val/-(double)INT8_MIN : val/INT8_MAX; break;
            case GL_SHORT: val = val<0 ? val/-(double)INT16_MIN : val/INT16_MAX; break;
            case GL_INT: val = val<0 ? val/-(double)INT32_MIN : val/INT32_MAX; break;
            }
        }
        trc_gl_state_set_state_double(ctx->trace, GL_CURRENT_VERTEX_ATTRIB, index*4+i, 0);
    }
    for (; i < 3; i++)
        trc_gl_state_set_state_double(ctx->trace, GL_CURRENT_VERTEX_ATTRIB, index*4+i, 0);
    for (; i < 4; i++)
        trc_gl_state_set_state_double(ctx->trace, GL_CURRENT_VERTEX_ATTRIB, index*4+i, 1);
    
    double vals[4];
    for (i = 0; i < 4; i++)
        vals[i] = trc_gl_state_get_state_double(ctx->trace, GL_CURRENT_VERTEX_ATTRIB, index*4+i);
    
    switch (internal) {
    case GL_FLOAT: F(glVertexAttrib4dv(index+1, vals)); break;
    case GL_DOUBLE: F(glVertexAttribL4dv(index+1, vals)); break;
    case GL_UNSIGNED_INT: F(glVertexAttribI4ui(index, vals[0], vals[1], vals[2], vals[3])); break;
    case GL_INT: F(glVertexAttribI4i(index, vals[0], vals[1], vals[2], vals[3])); break;
    }
}

static GLint get_bound_framebuffer(trc_replay_context_t* ctx, GLenum target) {
    const trc_gl_context_rev_t* state = trc_get_gl_context(ctx->trace, 0);
    switch (target) {
    case GL_DRAW_FRAMEBUFFER: 
        return state->draw_framebuffer;
    case GL_READ_FRAMEBUFFER: 
        return state->read_framebuffer;
    case GL_FRAMEBUFFER: 
        return state->draw_framebuffer;
    }
    return 0;
}

static void update_query(trc_replay_context_t* ctx, trace_command_t* cmd, GLenum target, GLuint fake_id, GLuint id) {
    if (!id) return;
    
    GLint res = 0;
    if (target!=GL_TIME_ELAPSED && target!=GL_TIMESTAMP) {
        F(glFinish)();
        
        while (!res) F(glGetQueryObjectiv)(id, GL_QUERY_RESULT_AVAILABLE, &res);
        
        //TODO: Use glGetQueryObjecti64v when available
        F(glGetQueryObjectiv)(id, GL_QUERY_RESULT, &res);
    }
    
    trc_gl_query_rev_t query = *trc_get_gl_query(ctx->trace, fake_id);
    query.result = res;
    trc_set_gl_query(ctx->trace, fake_id, &query);
}

static void begin_get_fb0_data(trc_replay_context_t* ctx, GLint prev[11]) {
    F(glGetIntegerv)(GL_PACK_SWAP_BYTES, &prev[0]);
    F(glGetIntegerv)(GL_PACK_LSB_FIRST, &prev[1]);
    F(glGetIntegerv)(GL_PACK_ROW_LENGTH, &prev[2]);
    F(glGetIntegerv)(GL_PACK_IMAGE_HEIGHT, &prev[3]);
    F(glGetIntegerv)(GL_PACK_SKIP_PIXELS, &prev[4]);
    F(glGetIntegerv)(GL_PACK_SKIP_ROWS, &prev[5]);
    F(glGetIntegerv)(GL_PACK_SKIP_IMAGES, &prev[6]);
    F(glGetIntegerv)(GL_PACK_ALIGNMENT, &prev[7]);
    F(glGetIntegerv)(GL_READ_BUFFER, &prev[8]);
    F(glGetIntegerv)(GL_READ_FRAMEBUFFER_BINDING, &prev[9]);
    F(glGetIntegerv)(GL_PIXEL_PACK_BUFFER_BINDING, &prev[10]);
    
    F(glPixelStorei)(GL_PACK_SWAP_BYTES, GL_FALSE);
    F(glPixelStorei)(GL_PACK_LSB_FIRST, GL_FALSE);
    F(glPixelStorei)(GL_PACK_ROW_LENGTH, 0);
    F(glPixelStorei)(GL_PACK_IMAGE_HEIGHT, 0);
    F(glPixelStorei)(GL_PACK_SKIP_PIXELS, 0);
    F(glPixelStorei)(GL_PACK_SKIP_ROWS, 0);
    F(glPixelStorei)(GL_PACK_SKIP_IMAGES, 0);
    F(glPixelStorei)(GL_PACK_ALIGNMENT, 1);
    F(glBindFramebuffer)(GL_READ_FRAMEBUFFER, 0);
    F(glBindBuffer)(GL_PIXEL_PACK_BUFFER, 0);
}

static void end_get_fb0_data(trc_replay_context_t* ctx, const GLint prev[11]) {
    F(glBindBuffer)(GL_PIXEL_PACK_BUFFER, prev[10]);
    F(glBindFramebuffer)(GL_READ_FRAMEBUFFER, prev[9]);
    F(glReadBuffer)(prev[8]);
    F(glPixelStorei)(GL_PACK_ALIGNMENT, prev[7]);
    F(glPixelStorei)(GL_PACK_SKIP_IMAGES, prev[6]);
    F(glPixelStorei)(GL_PACK_SKIP_ROWS, prev[5]);
    F(glPixelStorei)(GL_PACK_SKIP_PIXELS, prev[4]);
    F(glPixelStorei)(GL_PACK_IMAGE_HEIGHT, prev[3]);
    F(glPixelStorei)(GL_PACK_ROW_LENGTH, prev[2]);
    F(glPixelStorei)(GL_PACK_LSB_FIRST, prev[1]);
    F(glPixelStorei)(GL_PACK_SWAP_BYTES, prev[0]);
}

static trc_data_t* replay_get_fb0_buffer(trc_replay_context_t* ctx, trc_gl_context_rev_t* state,
                                         GLenum buffer, GLenum format, GLenum type) {
    F(glReadBuffer)(buffer);
    
    size_t data_size = state->drawable_width * state->drawable_height * 4;
    trc_data_t* data = trc_create_data(ctx->trace, data_size, NULL, TRC_DATA_NO_ZERO);
    void* dest = trc_map_data(data, TRC_MAP_REPLACE);
    F(glReadPixels)(0, 0, state->drawable_width, state->drawable_height, format, type, dest);
    trc_unmap_freeze_data(ctx->trace, data);
    
    return data;
}

static void store_and_bind_fb(trc_replay_context_t* ctx, GLint* prev, GLuint fb) {
    F(glGetIntegerv)(GL_DRAW_FRAMEBUFFER_BINDING, prev);
    F(glBindFramebuffer)(GL_DRAW_FRAMEBUFFER, fb);
}

static void replay_update_buffers(trc_replay_context_t* ctx, bool backcolor, bool frontcolor,
                                  bool depth, bool stencil) {
    F(glFinish)();
    
    GLint prevfb;
    store_and_bind_fb(ctx, &prevfb, 0);
    GLint depth_size, stencil_size;
    F(glGetFramebufferAttachmentParameteriv)(GL_DRAW_FRAMEBUFFER, GL_DEPTH,
                                             GL_FRAMEBUFFER_ATTACHMENT_DEPTH_SIZE, &depth_size);
    F(glGetFramebufferAttachmentParameteriv)(GL_DRAW_FRAMEBUFFER, GL_STENCIL,
                                             GL_FRAMEBUFFER_ATTACHMENT_STENCIL_SIZE, &stencil_size);
    if (depth_size == 0) depth = false;
    if (stencil_size == 0) stencil = false;
    F(glBindFramebuffer)(GL_DRAW_FRAMEBUFFER, prevfb);
    
    GLint prev[11];
    begin_get_fb0_data(ctx, prev);
    trc_gl_context_rev_t state = *trc_get_gl_context(ctx->trace, 0);
    if (backcolor) {
        state.back_color_buffer = replay_get_fb0_buffer(ctx, &state, GL_BACK, GL_RGBA, GL_UNSIGNED_BYTE);
    }
    if (frontcolor) {
        state.front_color_buffer = replay_get_fb0_buffer(ctx, &state, GL_FRONT, GL_RGBA, GL_UNSIGNED_BYTE);
    }
    if (depth) {
        state.back_depth_buffer = replay_get_fb0_buffer(ctx, &state, GL_BACK, GL_DEPTH_COMPONENT, GL_UNSIGNED_INT);
    }
    if (stencil) {
        state.back_stencil_buffer = replay_get_fb0_buffer(ctx, &state, GL_BACK, GL_STENCIL_INDEX, GL_UNSIGNED_INT);
    }
    trc_set_gl_context(ctx->trace, 0, &state);
    end_get_fb0_data(ctx, prev);
}

static bool begin_draw(trc_replay_context_t* ctx, trace_command_t* cmd) {
    const trc_gl_context_rev_t* state = trc_get_gl_context(ctx->trace, 0);
    const trc_gl_vao_rev_t* vao = trc_get_gl_vao(ctx->trace, state->bound_vao);
    if (vao == NULL) {trc_add_error(cmd, "No VAO bound"); return false;}
    const trc_gl_program_rev_t* program = trc_get_gl_program(ctx->trace, state->bound_program);
    if (program == NULL) {trc_add_error(cmd, "No program bound"); return false;}
    
    GLint last_buf;
    F(glGetIntegerv)(GL_ARRAY_BUFFER_BINDING, &last_buf);
    
    size_t prog_vertex_attrib_count = program->vertex_attribs->size / (sizeof(uint)*2);
    uint* prog_vertex_attribs = trc_map_data(program->vertex_attribs, TRC_MAP_READ);
    trc_gl_vao_attrib_t* vao_attribs = vao ? trc_map_data(vao->attribs, TRC_MAP_READ) : NULL;
    for (size_t i = 0; i < (vao?vao->attribs->size/sizeof(trc_gl_vao_attrib_t):0); i++) {
        GLint real_loc = -1;
        for (size_t j = 0; j < prog_vertex_attrib_count; j++) {
            if (prog_vertex_attribs[j*2+1] == i) {
                real_loc = prog_vertex_attribs[j*2];
                break;
            }
        }
        if (real_loc < 0) continue;
        
        trc_gl_vao_attrib_t* a = &vao_attribs[i];
        if (!a->enabled) {
            F(glDisableVertexAttribArray)(real_loc);
            continue;
        }
        F(glEnableVertexAttribArray)(real_loc);
        
        F(glBindBuffer)(GL_ARRAY_BUFFER, a->buffer);
        if (a->integer)
            F(glVertexAttribIPointer)(real_loc, a->size, a->type, a->stride, (const void*)(uintptr_t)a->offset);
        else
            F(glVertexAttribPointer)(real_loc, a->size, a->type, a->normalized, a->stride, (const void*)(uintptr_t)a->offset);
        
        //TODO: Only do this if OpenGL 3.3+ is used
        F(glVertexAttribDivisor)(real_loc, a->divisor);
    }
    trc_unmap_data(vao->attribs);
    trc_unmap_data(program->vertex_attribs);
    
    F(glBindBuffer)(GL_ARRAY_BUFFER, last_buf);
    
    return true;
}

static bool not_one_of(int val, ...) {
    va_list list;
    va_start(list, val);
    while (true) {
        int v = va_arg(list, int);
        if (v == -1) break;
        if (v == val) return false;
    }
    va_end(list);
    return true;
}

#define PARTIAL_VALIDATE_CLEAR_BUFFER do {\
int max_draw_buffers = trc_gl_state_get_state_int(ctx->trace, GL_MAX_DRAW_BUFFERS, 0);\
if (p_drawbuffer<0 || (p_drawbuffer==0&&p_buffer!=GL_COLOR) || p_drawbuffer>=max_draw_buffers)\
    ERROR("Invalid buffer");\
} while (0)

static void update_drawbuffer(trc_replay_context_t* ctx, GLenum buffer, GLuint drawbuffer) {
    const trc_gl_context_rev_t* state = trc_get_gl_context(ctx->trace, 0);
    if (state->draw_framebuffer == 0) {
        switch (buffer) {
        case GL_COLOR: replay_update_buffers(ctx, true, false, false, false); break;
        case GL_DEPTH: replay_update_buffers(ctx, false, false, true, false); break;
        case GL_STENCIL: replay_update_buffers(ctx, false, false, false, true); break;
        }
    } else {
        const trc_gl_framebuffer_rev_t* rev = trc_get_gl_framebuffer(ctx->trace, state->draw_framebuffer);
        uint attachment;
        if (drawbuffer>=rev->draw_buffers->size/sizeof(GLenum)) {
            attachment = GL_NONE;
        } else {
            attachment = ((GLenum*)trc_map_data(rev->draw_buffers, TRC_MAP_READ))[drawbuffer];
            trc_unmap_data(rev->draw_buffers);
        }
        
        size_t attach_count = rev->attachments->size / sizeof(trc_gl_framebuffer_attachment_t);
        const trc_gl_framebuffer_attachment_t* attachs = trc_map_data(rev->attachments, TRC_MAP_READ);
        for (size_t i = 0; i < attach_count; i++) {
            const trc_gl_framebuffer_attachment_t* attach = &attachs[i];
            if (attach->attachment+GL_COLOR_ATTACHMENT0 != attachment) continue;
            if (attach->has_renderbuffer) continue;
            const trc_gl_texture_rev_t* tex = trc_get_gl_texture(ctx->trace, attach->fake_texture);
            assert(tex);
            replay_update_tex_image(ctx, tex, attach->fake_texture, attach->level, attach->face);
        }
        trc_unmap_data(rev->attachments);
    }
}

static void end_draw(trc_replay_context_t* ctx, trace_command_t* cmd) {
    const trc_gl_context_rev_t* state = trc_get_gl_context(ctx->trace, 0);
    //TODO: Only update buffers that could have been written to
    if (state->draw_framebuffer == 0) {
        replay_update_buffers(ctx, true, false, true, true);
    } else {
        const trc_gl_framebuffer_rev_t* rev = trc_get_gl_framebuffer(ctx->trace, state->draw_framebuffer);
        
        size_t attach_count = rev->attachments->size / sizeof(trc_gl_framebuffer_attachment_t);
        const trc_gl_framebuffer_attachment_t* attachs = trc_map_data(rev->attachments, TRC_MAP_READ);
        for (size_t i = 0; i < attach_count; i++) {
            const trc_gl_framebuffer_attachment_t* attach = &attachs[i];
            if (attach->has_renderbuffer) continue;
            const trc_gl_texture_rev_t* tex = trc_get_gl_texture(ctx->trace, attach->fake_texture);
            assert(tex);
            replay_update_tex_image(ctx, tex, attach->fake_texture, attach->level, attach->face);
        }
        trc_unmap_data(rev->attachments);
    }
}

static void replay_begin_cmd(trc_replay_context_t* ctx, const char* name, trace_command_t* cmd) {
    replay_alloc_data_size = 0;
    replay_alloc_big_data = NULL;
    
    //TODO: This could be done less often
    if (F(glDebugMessageCallback)) {
        F(glEnable)(GL_DEBUG_OUTPUT);
        F(glEnable)(GL_DEBUG_OUTPUT_SYNCHRONOUS);
        F(glDebugMessageCallback)(debug_callback, cmd);
        F(glDebugMessageControl)(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, 0, GL_TRUE);
    }/* else if (F(glDebugMessageCallbackARB)) {
        F(glEnable)(GL_DEBUG_OUTPUT_SYNCHRONOUS);
        F(glDebugMessageCallbackARB)(debug_callback, cmd);
        //TODO: glDebugMessageControlARB
    }*/
    
    if (F(glGetError)) F(glGetError)();
}

static void validate_get_uniform(trc_replay_context_t* ctx, trace_command_t* command) {
    //TODO: Don't use glGetProgramiv to get the link status
    GLuint fake = gl_param_GLuint(command, 0);
    GLuint real_program = trc_get_real_gl_program(ctx->trace, fake);
    if (!real_program) {
        trc_add_error(command, "No such program.");
        return;
    }
    GLint status;
    F(glGetProgramiv)(real_program, GL_LINK_STATUS, &status);
    if (!status) trc_add_error(command, "Program not successfully linked.");
}

static void replay_end_cmd(trc_replay_context_t* ctx, const char* name, trace_command_t* cmd) {
    while (replay_alloc_big_data) {
        void* next = *(void**)replay_alloc_big_data;
        free(replay_alloc_big_data);
        replay_alloc_big_data = next;
    }
    
    GLenum error = GL_NO_ERROR;
    if (trc_get_current_fake_gl_context(ctx->trace) && F(glGetError)) error = F(glGetError)();
    //TODO: Are all of these needed?
    switch (error) {
    case GL_NO_ERROR:
        break;
    case GL_INVALID_ENUM:
        trc_add_error(cmd, "GL_INVALID_ENUM");
        break;
    case GL_INVALID_VALUE:
        trc_add_error(cmd, "GL_INVALID_VALUE");
        break;
    case GL_INVALID_OPERATION:
        trc_add_error(cmd, "GL_INVALID_OPERATION");
        break;
    case GL_STACK_OVERFLOW:
        trc_add_error(cmd, "GL_STACK_OVERFLOW");
        break;
    case GL_STACK_UNDERFLOW:
        trc_add_error(cmd, "GL_STACK_UNDERFLOW");
        break;
    case GL_OUT_OF_MEMORY:
        trc_add_error(cmd, "GL_OUT_OF_MEMORY");
        break;
    case GL_INVALID_FRAMEBUFFER_OPERATION:
        trc_add_error(cmd, "GL_INVALID_FRAMEBUFFER_OPERATION");
        break;
    case GL_CONTEXT_LOST: //TODO: Handle this
        trc_add_error(cmd, "GL_CONTEXT_LOST");
        break;
    case GL_TABLE_TOO_LARGE: //TODO: Get rid of this?
        trc_add_error(cmd, "GL_TABLE_TOO_LARGE");
        break;
    }
}
""")

nontrivial_str = open("nontrivial_func_impls.c").read()
nontrivial = {}

current_name = ""
current = ""
print_func = False
for line in nontrivial_str.split("\n"):
    if line.split('//')[0].rstrip().endswith(":") and line.startswith("gl"):
        if len(current_name) != 0:
            nontrivial[current_name] = current
        current_name = line.split('//')[0].rstrip()[:-1]
        f = gl.functions[current_name]
        print_func =  '//' not in line
        if print_func: print '%s: //%s' % (current_name, ', '.join(["%s p_%s"%(p.type_, p.name) for p in f.params]))
        current = ""
    else:
        current += line + "\n"
        if print_func: print line
if current_name != "":
    nontrivial[current_name] = current

output.write("""#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-variable"
""")
for name, func in func_dict.iteritems():
    output.write("void replay_%s(trc_replay_context_t* ctx, trace_command_t* cmd) {\n" % (name))
    
    if not name.startswith("glX"):
        output.write("""    if (!trc_get_current_fake_gl_context(ctx->trace)) {
        trc_add_error(cmd, "No current OpenGL context.");
        return;
    }
    """)
    
    output.write("replay_begin_cmd(ctx, \"%s\", cmd);\n" % (name))
    
    if name == "glXGetProcAddress":
        output.write("    %s_t real = &%s;" % (name, name))
    else:
        output.write("    %s_t real = ((replay_gl_funcs_t*)ctx->_replay_gl)->real_%s;\n" % (name, name))
    
    output.write("    do {(void)sizeof((real));} while (0);\n")
    
    function = gl.functions[name]
    
    """for i, param in zip(range(len(function.params)), function.params):
        arg = "cmd, %d" % (i)
        if param.type_[-1] == "]":
            output.write("%s* p_%s = " % (param.type_.split("[")[0], param.name))
        elif ('*' in param.type_ and 'GLchar' not in param.type_) or 'GLsync' in param.type_:
            if name not in nontrivial:
                #Disabled because of high-volume output
                #print "Warning:", name, "has pointer parameters but is not implemented as non trivial"
                output.write("%s p_%s = (%s)" % (param.type_, param.name, param.type_)) #Get it to compile but not to work
            else:
                if param.type_ == 'GLsync': output.write("uint64_t p_%s = " % param.name)
                else: continue
        elif 'GLsync' in param.type_:
            if name in nontrivial:
                output.write("uint64_t p_%s = " % param.name)
            else:
                #Disabled because of high-volume output
                #print "Warning:", name, "has pointer parameters but is not implemented as non trivial"
                output.write("%s p_%s = (GLsync)" % (param.type_, param.name)) #Get it to compile but not to work
        else:
            output.write("%s p_%s = " % (param.type_, param.name))
        
        if param.type_[-1] == "]":
            output.write("gl_param_%s_array(%s)" % (param.type_.split("[")[0], arg))
        elif param.type_.replace(" ", "") == "constGLchar*const*":
            output.write("(%s)gl_param_pointer(%s)" % (param.type_, arg))
        elif param.type_.replace(" ", "") == "constGLchar**":
            output.write("(%s)gl_param_pointer(%s)" % (param.type_, arg))
        elif param.type_.replace(" ", "") == "constGLcharARB**":
            output.write("(%s)gl_param_pointer(%s)" % (param.type_, arg))
        elif param.type_.replace(" ", "") == "unsignedint":
            output.write("(%s)gl_param_unsigned_int(%s)" % (param.type_, arg))
        elif param.type_.replace(" ", "") == "unsignedlong":
            output.write("(%s)gl_param_unsigned_int(%s)" % (param.type_, arg))
        elif "*" in param.type_ or 'GLsync' in param.type_:
            if "GLchar" in param.type_:
                output.write("(%s)gl_param_string(%s)" % (param.type_, arg))
            else:
                output.write("gl_param_pointer(%s)" % arg)
        else:
            output.write("(%s)gl_param_%s(%s)" % (param.type_, param.type_.replace("const", "").lstrip().rstrip(), arg))
        output.write(";\n")
        output.write("    do {(void)sizeof((p_%s));} while (0);\n" % param.name)"""
    for i, param in zip(range(len(func.params)), func.params):
        output.write('    trace_value_t* arg_%s = trc_get_arg(cmd, %d);\n' % (param.name, i))
        output.write(param.dtype.gen_replay_read_code('p_'+param.name, 'arg_'+param.name, param.array_count)+'\n')
    
    if name in nontrivial:
        output.write(nontrivial[name])
    else:
        pass #output.write("    real(%s);\n" % (", ".join(["p_"+param.name for param in func.params])))
    
    output.write("replay_end_cmd(ctx, \"%s\", cmd);\n" % (name))
    
    if name in nontrivial: output.write("#undef FUNC\n#define FUNC \"%s\"\nRETURN;\n" % (name))
    
    output.write("}\n\n")
output.write("""#pragma GCC diagnostic pop
""")

output.write("""static void reset_gl_funcs(trc_replay_context_t* ctx);

void init_replay_gl(trc_replay_context_t* ctx) {
    replay_gl_funcs_t* funcs = malloc(sizeof(replay_gl_funcs_t));
    ctx->_replay_gl = funcs;
    reset_gl_funcs(ctx);
""")

for name in func_dict.keys():
    if not name == "glXGetProcAddress" and name.startswith("glX"):
        output.write("    funcs->real_%s = (%s_t)glXGetProcAddress((const GLubyte*)\"%s\");\n" % (name, name, name))

output.write("""    ctx->_replay_gl = funcs;
}

static void reset_gl_funcs(trc_replay_context_t* ctx) {
    replay_gl_funcs_t* funcs = ctx->_replay_gl;
""")

for name in func_dict.keys():
    if not name.startswith("glX"):
        output.write("    funcs->real_%s = NULL;\n" % (name))

output.write("""}

static void reload_gl_funcs(trc_replay_context_t* ctx) {
    replay_gl_funcs_t* funcs = ctx->_replay_gl;
""")

for name in func_dict.keys():
    if not name.startswith("glX"):
        output.write("    funcs->real_%s = (%s_t)glXGetProcAddress((const GLubyte*)\"%s\");\n" % (name, name, name))

output.write("}\n\n")

output.write("""void deinit_replay_gl(trc_replay_context_t* ctx) {
    free(ctx->_replay_gl);
}
""")
