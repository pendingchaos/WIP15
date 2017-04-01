#!/usr/bin/env python
# -*- coding: utf-8 -*-
import glapi.glxml
from gl_state import *

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
#define RETURN do {replay_end_cmd(ctx, FUNC, command);return;} while(0)
#define ERROR(...) do {trc_add_error(command, __VA_ARGS__); RETURN;} while (0)
#define FUNC ""

typedef void (*_func)();

""")

output.write(gl.typedecls)

output.write("""
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

static uint64_t gl_param_GLsync(trace_command_t* cmd, size_t index) {
    return *trc_get_ptr(trc_get_arg(cmd, index));
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

static void reset_gl_funcs(trc_replay_context_t* ctx);
static void reload_gl_funcs(trc_replay_context_t* ctx);

extern _func glXGetProcAddress(const GLubyte* procName);

""")

output.write("\n")

for name in gl.functions:
    function = gl.functions[name]
    params = []
    for param in function.params:
        params.append("%s" % (param.type_))
    
    output.write("typedef %s (*%s_t)(%s);\n" % (function.returnType, name, ", ".join(params)))

output.write("typedef struct {\n")
for name in gl.functions:
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
            trc_add_error(command, "Expected 1 value. Got %u.\\n", count);
            return true;
        }
        break;
    case GL_TEXTURE_BORDER_COLOR:
        if (count != 4) {
            trc_add_error(command, "Expected 4 values. Got %u.\\n", count);
            return true;
        }
        break;
    }
    
    switch (param) {
    case GL_TEXTURE_MIN_FILTER:
        if (val[0]!=GL_LINEAR && val[0]!=GL_NEAREST && val[0]!=GL_NEAREST_MIPMAP_NEAREST &&
            val[0]!=GL_LINEAR_MIPMAP_NEAREST && val[0]!=GL_NEAREST_MIPMAP_LINEAR &&
            val[0]!=GL_LINEAR_MIPMAP_LINEAR) {
            trc_add_error(command, "Invalid minification filter.\\n");
            return true;
        }
        break;
    case GL_TEXTURE_MAG_FILTER:
        if (val[0]!=GL_LINEAR && val[0]!=GL_NEAREST) {
            trc_add_error(command, "Invalid magnification filter.\\n");
            return true;
        }
        break;
    case GL_TEXTURE_WRAP_S:
    case GL_TEXTURE_WRAP_T:
    case GL_TEXTURE_WRAP_R:
        if (val[0]!=GL_CLAMP_TO_EDGE && val[0]!=GL_CLAMP_TO_BORDER && val[0]!=GL_MIRRORED_REPEAT &&
            val[0]!=GL_REPEAT && val[0]!=GL_MIRROR_CLAMP_TO_EDGE && val[0]!=GL_CLAMP_TO_EDGE) {
            trc_add_error(command, "Invalid wrap mode.\\n");
            return true;
        }
        break;
    case GL_TEXTURE_COMPARE_MODE:
        if (val[0]!=GL_COMPARE_REF_TO_TEXTURE && val[0]!=GL_NONE) {
            trc_add_error(command, "Invalid compare mode.\\n");
            return true;
        }
        break;
    case GL_TEXTURE_COMPARE_FUNC:
        if (val[0]!=GL_LEQUAL && val[0]!=GL_GEQUAL && val[0]!=GL_LESS && val[0]!=GL_GREATER &&
            val[0]!=GL_EQUAL && val[0]!=GL_NOTEQUAL && val[0]!=GL_ALWAYS && val[0]!=GL_NEVER) {
            trc_add_error(command, "Invalid compare function.\\n");
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

static trc_data_t** get_bound_texture_data(trc_gl_context_rev_t* state, GLenum target) {
    switch (target) {
    case GL_TEXTURE_1D: return &state->tex_1d;
    case GL_TEXTURE_2D: return &state->tex_2d;
    case GL_TEXTURE_3D: return &state->tex_3d;
    case GL_TEXTURE_1D_ARRAY: return &state->tex_1d_array;
    case GL_TEXTURE_2D_ARRAY: return &state->tex_2d_array;
    case GL_TEXTURE_RECTANGLE: return &state->tex_rectangle;
    case GL_TEXTURE_CUBE_MAP: return &state->tex_cube_map;
    case GL_TEXTURE_CUBE_MAP_ARRAY: return &state->tex_cube_map_array;
    case GL_TEXTURE_BUFFER: return &state->tex_buffer;
    case GL_TEXTURE_2D_MULTISAMPLE: return &state->tex_2d_multisample;
    case GL_TEXTURE_2D_MULTISAMPLE_ARRAY: return &state->tex_2d_multisample_array;
    }
    return NULL;
}

static void set_bound_texture(trace_t* trace, GLenum target, int unit, uint tex) {
    trc_gl_context_rev_t state = *trc_get_gl_context(trace, 0);
    
    trc_data_t* data = *get_bound_texture_data(&state, target);
    if (!data) {
        //TODO: Error
        return;
    }
    
    //Copy
    uint* dataptr = trc_lock_data(data, true, false);
    trc_data_t* new_data = trc_create_inspection_data(trace, data->uncompressed_size, dataptr);
    trc_unlock_data(data);
    
    //Modify
    dataptr = trc_lock_data(new_data, false, true);
    dataptr[unit<0?state.active_texture_unit:unit] = tex;
    trc_unlock_data(new_data);
    
    //Replace
    *get_bound_texture_data(&state, target) = new_data;
    trc_set_gl_context(trace, 0,&state);
}

static uint get_bound_texture(trace_t* trace, GLenum target, int unit) {
    const trc_gl_context_rev_t* state = trc_get_gl_context(trace, 0);
    trc_data_t* data = *get_bound_texture_data((trc_gl_context_rev_t*)state, target);
    if (!data) return 0;
    uint* dataptr = trc_lock_data(data, true, false);
    uint res = dataptr[unit<0?state->active_texture_unit:unit];
    trc_unlock_data(data);
    
    return res;
}

static void replay_create_context_buffers(trace_t* trace, trc_gl_context_rev_t* rev) {
    size_t size = rev->drawable_width * rev->drawable_height * 4;
    rev->front_color_buffer = trc_create_inspection_data(trace, size, NULL);
    rev->back_color_buffer = trc_create_inspection_data(trace, size, NULL);
    rev->back_depth_buffer = trc_create_inspection_data(trace, size, NULL);
    rev->back_stencil_buffer = trc_create_inspection_data(trace, size, NULL);
    
    void* data = trc_lock_data(rev->front_color_buffer, false, true);
    memset(data, 0, size);
    trc_unlock_data(rev->front_color_buffer);
    
    data = trc_lock_data(rev->back_color_buffer, false, true);
    memset(data, 0, size);
    trc_unlock_data(rev->back_color_buffer);
    
    data = trc_lock_data(rev->back_depth_buffer, false, true);
    memset(data, 0, size);
    trc_unlock_data(rev->back_depth_buffer);
    
    data = trc_lock_data(rev->back_stencil_buffer, false, true);
    memset(data, 0, size);
    trc_unlock_data(rev->back_stencil_buffer);
}

static trc_gl_context_rev_t create_context_rev(trc_replay_context_t* ctx, void* real) {
    trc_gl_context_rev_t rev;
    rev.real = real;
    int w, h;
    SDL_GL_GetDrawableSize(ctx->window, &w, &h);
    rev.drawable_width = w;
    rev.drawable_height = h;
    rev.array_buffer = 0;
    rev.atomic_counter_buffer = 0;
    rev.copy_read_buffer = 0;
    rev.copy_write_buffer = 0;
    rev.dispatch_indirect_buffer = 0;
    rev.draw_indirect_buffer = 0;
    rev.element_array_buffer = 0;
    rev.pixel_pack_buffer = 0;
    rev.pixel_unpack_buffer = 0;
    rev.query_buffer = 0;
    rev.shader_storage_buffer = 0;
    rev.texture_buffer = 0;
    rev.transform_feedback_buffer = 0;
    rev.uniform_buffer = 0;
    rev.bound_program = 0;
    rev.bound_vao = 0;
    rev.bound_renderbuffer = 0;
    rev.read_framebuffer = 0;
    rev.draw_framebuffer = 0;
    rev.samples_passed_query = 0;
    rev.any_samples_passed_query = 0;
    rev.any_samples_passed_conservative_query = 0;
    rev.primitives_generated_query = 0;
    rev.transform_feedback_primitives_written_query = 0;
    rev.time_elapsed_query = 0;
    rev.timestamp_query = 0;
    rev.active_texture_unit = 0;
    
    replay_create_context_buffers(ctx->trace, &rev);
    
    //TODO
    uint max_combined_tex_units = 48;
    
    trc_data_t* bound_tex_data = trc_create_inspection_data(ctx->trace, max_combined_tex_units*4, NULL);
    uint* bound_tex_data_ptr = trc_lock_data(bound_tex_data, false, true);
    for (uint i = 0; i < max_combined_tex_units; i++) bound_tex_data_ptr[i] = 0;
    trc_unlock_data(bound_tex_data);
    
    F(glGenVertexArrays)(1, &rev.draw_vao);
    F(glBindVertexArray)(rev.draw_vao);
    
    rev.tex_1d = bound_tex_data;
    rev.tex_2d = bound_tex_data;
    rev.tex_3d = bound_tex_data;
    rev.tex_1d_array = bound_tex_data;
    rev.tex_2d_array = bound_tex_data;
    rev.tex_rectangle = bound_tex_data;
    rev.tex_cube_map = bound_tex_data;
    rev.tex_cube_map_array = bound_tex_data;
    rev.tex_buffer = bound_tex_data;
    rev.tex_2d_multisample = bound_tex_data;
    rev.tex_2d_multisample_array = bound_tex_data;
    
    return rev;
}

void replay_set_texture_image(trace_t* trace, trace_command_t* command, uint target, uint level,
                              uint internal_format, uint width, uint height, uint depth, trc_data_t* data) {
    uint fake = get_bound_texture(trace, target, trc_get_gl_context(trace, 0)->active_texture_unit);
    const trc_gl_texture_rev_t* rev = trc_get_gl_texture(trace, fake);
    if (!rev) {
        trc_add_error(command, "No texture bound or invalid target\\n");
        return;
    }
    
    trc_gl_texture_image_t img;
    img.face = 0;
    if (target>=GL_TEXTURE_CUBE_MAP_POSITIVE_X && target<=GL_TEXTURE_CUBE_MAP_NEGATIVE_Z)
        img.face = target - GL_TEXTURE_CUBE_MAP_POSITIVE_X;
    img.level = level;
    img.internal_format = internal_format;
    img.width = width;
    img.height = height;
    img.depth = depth;
    img.data = data;
    
    size_t img_count = rev->images->uncompressed_size / sizeof(trc_gl_texture_image_t);
    trc_gl_texture_image_t* newimages = malloc((img_count+1)*sizeof(trc_gl_texture_image_t));
    
    trc_gl_texture_image_t* images = trc_lock_data(rev->images, true, false);
    bool replaced = false;
    for (size_t i = 0; i < img_count; i++) {
        if (images[i].face==img.face && images[i].level==img.level) {
            newimages[i] = img;
            replaced = true;
        } else {
            newimages[i] = images[i];
        }
    }
    trc_unlock_data(rev->images);
    
    trc_gl_texture_rev_t newrev = *rev;
    if (!replaced) newimages[img_count++] = img;
    
    size_t size = img_count * sizeof(trc_gl_texture_image_t);
    newrev.images = trc_create_inspection_data(trace, size, newimages);
    
    free(newimages);
    
    trc_set_gl_texture(trace, fake, &newrev);
}

void replay_update_tex_image(trc_replay_context_t* ctx, trace_command_t* command, uint target, uint level) {
    uint fake = get_bound_texture(ctx->trace, target, trc_get_gl_context(ctx->trace, 0)->active_texture_unit);
    uint real = trc_get_real_gl_texture(ctx->trace, fake);
    if (!real) {
        trc_add_error(command, "No texture bound or invalid target\\n");
        return;
    }
    
    GLint width, height, depth, internal_format;
    F(glGetTexLevelParameteriv)(target, level, GL_TEXTURE_WIDTH, &width);
    F(glGetTexLevelParameteriv)(target, level, GL_TEXTURE_HEIGHT, &height);
    F(glGetTexLevelParameteriv)(target, level, GL_TEXTURE_DEPTH, &depth);
    F(glGetTexLevelParameteriv)(target, level, GL_TEXTURE_INTERNAL_FORMAT, &internal_format);
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
    trc_data_t* data = trc_create_inspection_data(ctx->trace, data_size, NULL);
    void* dest = trc_lock_data(data, false, true);
    
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
    F(glGetTexImage)(target, level, format, type, dest);
    
    trc_unlock_data(data);
    
    replay_set_texture_image(ctx->trace, command, target, level, internal_format, width, height, depth, data);
}

bool replay_append_fb_attachment(trace_t* trace, uint fb, const trc_gl_framebuffer_attachment_t* attach) {
    const trc_gl_framebuffer_rev_t* rev = trc_get_gl_framebuffer(trace, fb);
    if (!rev) return false;
    
    size_t attach_count = rev->attachments->uncompressed_size / sizeof(trc_gl_framebuffer_attachment_t);
    trc_gl_framebuffer_attachment_t* newattachs = malloc((attach_count+1)*sizeof(trc_gl_framebuffer_attachment_t));
    
    trc_gl_framebuffer_attachment_t* attachs = trc_lock_data(rev->attachments, true, false);
    bool replaced = false;
    for (size_t i = 0; i < attach_count; i++) {
        if (attachs[i].attachment == attach->attachment) {
            newattachs[i] = *attach;
            replaced = true;
        } else {
            newattachs[i] = attachs[i];
        }
    }
    trc_unlock_data(rev->attachments);
    
    trc_gl_framebuffer_rev_t newrev = *rev;
    if (!replaced) newattachs[attach_count++] = *attach;
    
    size_t size = attach_count * sizeof(trc_gl_framebuffer_attachment_t);
    newrev.attachments = trc_create_inspection_data(trace, size, newattachs);
    
    free(newattachs);
    
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
    if ((target>=GL_TEXTURE_CUBE_MAP_POSITIVE_X&&target<=GL_TEXTURE_CUBE_MAP_NEGATIVE_Z) ||
        target==GL_TEXTURE_CUBE_MAP_ARRAY || target==GL_TEXTURE_CUBE_MAP) {
        attach.face = layer % 6;
        attach.layer /= 6;
    }
    if (!replay_append_fb_attachment(trace, fb, &attach))
        //TODO: The framebuffer might not come from a binding
        trc_add_error(cmd, "No framebuffer bound or invalid target\\n");
}

void replay_add_fb_attachment_rb(trace_t* trace, trace_command_t* cmd, uint fb, uint attachment, uint rb) {
    trc_gl_framebuffer_attachment_t attach;
    memset(&attach, 0, sizeof(attach));
    attach.has_renderbuffer = true;
    attach.attachment = attachment;
    attach.fake_renderbuffer = fb;
    if (!replay_append_fb_attachment(trace, fb, &attach))
        //TODO: The framebuffer might not come from a binding
        trc_add_error(cmd, "No framebuffer bound or invalid target\\n");
}

void replay_update_renderbuffer(trc_replay_context_t* ctx, const trc_gl_renderbuffer_rev_t* rev,
                                uint fake, uint width, uint height, uint internal_format, uint samples) {
    GLint bits[6];
    GLint prev;
    F(glGetIntegerv(GL_RENDERBUFFER_BINDING, &prev));
    F(glBindRenderbuffer(GL_RENDERBUFFER, rev->real));
    F(glGetRenderbufferParameteriv(GL_RENDERBUFFER, GL_RENDERBUFFER_RED_SIZE, &bits[0]));
    F(glGetRenderbufferParameteriv(GL_RENDERBUFFER, GL_RENDERBUFFER_GREEN_SIZE, &bits[1]));
    F(glGetRenderbufferParameteriv(GL_RENDERBUFFER, GL_RENDERBUFFER_BLUE_SIZE, &bits[2]));
    F(glGetRenderbufferParameteriv(GL_RENDERBUFFER, GL_RENDERBUFFER_ALPHA_SIZE, &bits[3]));
    F(glGetRenderbufferParameteriv(GL_RENDERBUFFER, GL_RENDERBUFFER_DEPTH_SIZE, &bits[4]));
    F(glGetRenderbufferParameteriv(GL_RENDERBUFFER, GL_RENDERBUFFER_STENCIL_SIZE, &bits[5]));
    F(glBindRenderbuffer(GL_RENDERBUFFER, prev));
    
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
    GLuint texid = get_bound_texture(ctx->trace, target, -1);
    const trc_gl_texture_rev_t* tex_ptr = trc_get_gl_texture(ctx->trace, texid);
    if (!tex_ptr) {
        trc_add_error(command, "No texture bound, invalid texture handle used or invalid target\\n");
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
            trc_add_error(command, "Expected 1 value. Got %u.\\n", count);
            return true;
        }
        break;
    } case GL_TEXTURE_SWIZZLE_RGBA: {
        if (count != 4) {
            trc_add_error(command, "Expected 4 values. Got %u.\\n", count);
            return true;
        }
        break;
    }
    }
    
    switch (param) {
    case GL_DEPTH_STENCIL_TEXTURE_MODE:
        if (val[0]!=GL_DEPTH_COMPONENT && val[0]!=GL_STENCIL_INDEX) {
            trc_add_error(command, "Invalid depth stencil texture mode.\\n");
            return true;
        }
        break;
    case GL_TEXTURE_SWIZZLE_R:
    case GL_TEXTURE_SWIZZLE_G:
    case GL_TEXTURE_SWIZZLE_B:
    case GL_TEXTURE_SWIZZLE_A:
        if (val[0]!=GL_RED && val[0]!=GL_GREEN && val[0]!=GL_BLUE && val[0]!=GL_ALPHA) {
            trc_add_error(command, "Invalid swizzle.\\n");
            return true;
        }
        break;
    case GL_TEXTURE_SWIZZLE_RGBA:
        for (uint i = 0; i < 4; i++) {
            if (val[0]!=GL_RED && val[0]!=GL_GREEN && val[0]!=GL_BLUE && val[0]!=GL_ALPHA) {
                trc_add_error(command, "Invalid swizzle.\\n");
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
    switch (target) {
    case GL_ARRAY_BUFFER:
        return trc_get_gl_context(ctx->trace, 0)->array_buffer;
    case GL_ATOMIC_COUNTER_BUFFER:
        return trc_get_gl_context(ctx->trace, 0)->atomic_counter_buffer;
    case GL_COPY_READ_BUFFER:
        return trc_get_gl_context(ctx->trace, 0)->copy_read_buffer;
    case GL_COPY_WRITE_BUFFER:
        return trc_get_gl_context(ctx->trace, 0)->copy_write_buffer;
    case GL_DISPATCH_INDIRECT_BUFFER:
        return trc_get_gl_context(ctx->trace, 0)->dispatch_indirect_buffer;
    case GL_DRAW_INDIRECT_BUFFER:
        return trc_get_gl_context(ctx->trace, 0)->draw_indirect_buffer;
    case GL_ELEMENT_ARRAY_BUFFER:
        return trc_get_gl_context(ctx->trace, 0)->element_array_buffer;
    case GL_PIXEL_PACK_BUFFER:
        return trc_get_gl_context(ctx->trace, 0)->pixel_pack_buffer;
    case GL_PIXEL_UNPACK_BUFFER:
        return trc_get_gl_context(ctx->trace, 0)->pixel_unpack_buffer;
    case GL_QUERY_BUFFER:
        return trc_get_gl_context(ctx->trace, 0)->query_buffer;
    case GL_SHADER_STORAGE_BUFFER:
        return trc_get_gl_context(ctx->trace, 0)->shader_storage_buffer;
    case GL_TEXTURE_BUFFER:
        return trc_get_gl_context(ctx->trace, 0)->texture_buffer;
    case GL_TRANSFORM_FEEDBACK_BUFFER:
        return trc_get_gl_context(ctx->trace, 0)->transform_feedback_buffer;
    case GL_UNIFORM_BUFFER:
        return trc_get_gl_context(ctx->trace, 0)->uniform_buffer;
    default:
        return 0;
    }
}

static GLint uniform(trc_replay_context_t* ctx, trace_command_t* cmd) {
    const trc_gl_program_rev_t* rev = trc_get_gl_program(ctx->trace, trc_get_gl_context(ctx->trace, 0)->bound_program);
    size_t uniform_count = rev->uniforms->uncompressed_size / (sizeof(uint)*2);
    uint* uniforms = trc_lock_data(rev->uniforms, true, false);
    for (size_t i = 0; i < uniform_count; i++) {
        if (uniforms[i*2+1] == gl_param_GLint(cmd, 0)) {
            uint res = uniforms[i*2];
            trc_unlock_data(rev->uniforms);
            return res;
        }
    }
    trc_unlock_data(rev->uniforms);
    return -1;
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

static uint* get_query_binding_pointer(trc_gl_context_rev_t* state, GLenum target) {
    switch (target) {
    case GL_SAMPLES_PASSED: return &state->samples_passed_query;
    case GL_ANY_SAMPLES_PASSED: return &state->any_samples_passed_query;
    case GL_ANY_SAMPLES_PASSED_CONSERVATIVE: return &state->any_samples_passed_conservative_query;
    case GL_PRIMITIVES_GENERATED: return &state->primitives_generated_query;
    case GL_TRANSFORM_FEEDBACK_PRIMITIVES_WRITTEN: return &state->transform_feedback_primitives_written_query;
    case GL_TIME_ELAPSED: return &state->time_elapsed_query;
    case GL_TIMESTAMP: return &state->timestamp_query;
    }
    static uint dummy = 0;
    return &dummy;
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
    trc_data_t* data = trc_create_inspection_data(ctx->trace, data_size, NULL);
    void* dest = trc_lock_data(data, false, true);
    F(glReadPixels)(0, 0, state->drawable_width, state->drawable_height, format, type, dest);
    trc_unlock_data(data);
    
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

static void begin_draw(trc_replay_context_t* ctx) {
    const trc_gl_context_rev_t* state = trc_get_gl_context(ctx->trace, 0);
    const trc_gl_vao_rev_t* vao = trc_get_gl_vao(ctx->trace, state->bound_vao);
    const trc_gl_program_rev_t* program = trc_get_gl_program(ctx->trace, state->bound_program);
    
    GLint last_buf;
    F(glGetIntegerv)(GL_ARRAY_BUFFER_BINDING, &last_buf);
    
    size_t prog_vertex_attrib_count = program->vertex_attribs->uncompressed_size / (sizeof(uint)*2);
    uint* prog_vertex_attribs = trc_lock_data(program->vertex_attribs, true, false);
    for (size_t i = 0; i < (vao?vao->attrib_count:0); i++) {
        GLint real_loc = -1;
        for (size_t j = 0; j < prog_vertex_attrib_count; j++) {
            if (prog_vertex_attribs[j*2+1] == i) {
                real_loc = prog_vertex_attribs[j*2];
                break;
            }
        }
        if (real_loc < 0) continue;
        
        trc_gl_vao_attrib_t* a = &vao->attribs[i];
        if (!a->enabled) {
            F(glDisableVertexAttribArray)(real_loc);
            continue;
        }
        F(glEnableVertexAttribArray)(real_loc);
        
        if (a->buffer) {
            F(glBindBuffer)(GL_ARRAY_BUFFER, a->buffer);
            if (a->integer)
                F(glVertexAttribIPointer)(real_loc, a->size, a->type, a->stride, (const void*)(uintptr_t)a->offset);
            else
                F(glVertexAttribPointer)(real_loc, a->size, a->type, a->normalized, a->stride, (const void*)(uintptr_t)a->offset);
        } else {
            switch (a->size) {
            case 1:
                F(glVertexAttrib1d)(real_loc, a->value[0]);
                break;
            case 2:
                F(glVertexAttrib2d)(real_loc, a->value[0], a->value[1]);
                break;
            case 3:
                F(glVertexAttrib3d)(real_loc, a->value[0], a->value[1], a->value[2]);
                break;
            case 4:
                F(glVertexAttrib4d)(real_loc, a->value[0], a->value[1], a->value[2], a->value[3]);
                break;
            }
        }
        
        //TODO: Only do this if OpenGL 3.3+ is used
        F(glVertexAttribDivisor)(real_loc, a->divisor);
    }
    
    trc_unlock_data(program->vertex_attribs);
    
    F(glBindBuffer)(GL_ARRAY_BUFFER, last_buf);
}

static void end_draw(trc_replay_context_t* ctx, trace_command_t* cmd) {
    const trc_gl_context_rev_t* state = trc_get_gl_context(ctx->trace, 0);
    if (state->draw_framebuffer) {
        //TODO: Only update buffers that could have been written to
        replay_update_buffers(ctx, true, false, true, true);
    } else {
        //TODO
    }
}

static void replay_begin_cmd(trc_replay_context_t* ctx, const char* name, trace_command_t* cmd) {
    //TODO: This could be done less often
    if (F(glDebugMessageCallback)) {
        F(glEnable)(GL_DEBUG_OUTPUT);
        F(glEnable)(GL_DEBUG_OUTPUT_SYNCHRONOUS);
        F(glDebugMessageCallback)(debug_callback, cmd);
        F(glDebugMessageControl)(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, 0, GL_TRUE);
    } else if (F(glDebugMessageCallbackARB)) {
        F(glEnable)(GL_DEBUG_OUTPUT_SYNCHRONOUS);
        F(glDebugMessageCallbackARB)(debug_callback, cmd);
        //TODO: glDebugMessageControlARB
    }
    
    if (F(glGetError)) F(glGetError)();
}

static void get_uniform(trc_replay_context_t* ctx, trace_command_t* command) {
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
""")

output.write("""
static void replay_end_cmd(trc_replay_context_t* ctx, const char* name, trace_command_t* cmd) {
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
    
    if (F(glGetIntegerv)) {
""")


ver_to_mask = {(1, 0):"gl1_0",
               (1, 1):"gl1_1",
               (1, 2):"gl1_2",
               (1, 3):"gl1_3",
               (1, 4):"gl1_4",
               (1, 5):"gl1_5",
               (2, 0):"gl2_0",
               (2, 1):"gl2_1",
               (3, 0):"gl3_0",
               (3, 1):"gl3_1",
               (3, 2):"gl3_2",
               (3, 3):"gl3_3",
               (4, 0):"gl4_0",
               (4, 1):"gl4_1",
               (4, 2):"gl4_2",
               (4, 3):"gl4_3",
               (4, 4):"gl4_4",
               (4, 5):"gl4_5"}

for get in gl_gets:
    if get[1] == "P":
        continue
    
    ver_mask = "|".join([ver_to_mask[ver] for ver in get[3]])
    if len(ver_mask) == 0:
        ver_mask = "glnone"
    
    if get[1] == "S":
        output.write("""
        if (((%s) & gl3_0) && F(glGetString))
            ; //TODO //set_state_str(&cmd->state, \"%s\", F(glGetString)(%s));""" % (ver_mask, get[0], get[0]))
    else:
        type = {"B": "GLboolean",
                "I": "GLint",
                "I64": "GLint64",
                "E": "GLint",
                "F": "GLfloat",
                "D": "GLdouble"}[get[1]]
        
        type_str = {"B": "Boolean",
                    "I": "Integer",
                    "I64": "GLint64",
                    "E": "Integer",
                    "F": "Float",
                    "D": "Double"}[get[1]]
        
        type_str2 = {"B": "bool",
                     "I": "int",
                     "I64": "int64",
                     "E": "int",
                     "F": "float",
                     "D": "double"}[get[1]]
        
        output.write("""
        if (((%s) & gl2_1) && F(glGet%sv)) {
            //%s v[%d];
            //F(glGet%sv)(%s, v);
            //TODO
            //set_state_%s(&cmd->state, \"%s\", %d, v);
        }
            """ % (ver_mask, type_str, type, get[2], type_str, get[0], type_str2, get[0], get[2]))

for v in enable_entries:
    output.write("            {\n                //GLboolean v = F(glIsEnabled)(%s);\n" % v)
    output.write("                //TODO //set_state_bool(&cmd->state, \"%s enabled\", 1, &v);\n" % v)
    output.write("            }\n")

output.write("    }\n}\n\n")

nontrivial_str = open("nontrivial_func_impls.c").read()
nontrivial = {}

current_name = ""
current = ""
for line in nontrivial_str.split("\n"):
    if line.endswith(":") and line.startswith("gl"):
        if len(current_name) != 0:
            nontrivial[current_name] = current
        
        current_name = line[:-1]
        current = ""
    else:
        current += line + "\n"
if len(current_name) != 0:
    nontrivial[current_name] = current

for name in gl.functions:
    output.write("void replay_%s(trc_replay_context_t* ctx, trace_command_t* command) {\n" % (name))
    
    if not name.startswith("glX"):
        output.write("""    if (!trc_get_current_fake_gl_context(ctx->trace)) {
        trc_add_error(command, "No current OpenGL context.");
        return;
    }
    """)
    
    output.write("replay_begin_cmd(ctx, \"%s\", command);\n" % (name))
    
    if name == "glXGetProcAddress":
        output.write("    %s_t real = &%s;" % (name, name))
    else:
        output.write("    %s_t real = ((replay_gl_funcs_t*)ctx->_replay_gl)->real_%s;\n" % (name, name))
    
    output.write("    do {(void)sizeof((real));} while (0);\n")
    
    if name in nontrivial:
        output.write(nontrivial[name])
        output.write("replay_end_cmd(ctx, \"%s\", command);\n" % (name))
        output.write("#undef FUNC\n#define FUNC \"%s\"\nRETURN;\n" % (name))
        output.write("}\n\n")
        continue
    
    function = gl.functions[name]
    
    params = []
    
    i = 0
    for param in function.params:
        arg = "command, %d" % (i)
        
        if param.type_[-1] == "]":
            params.append("gl_param_%s_array(%s)" % (param.type_.split("[")[0], arg))
        elif param.type_.replace(" ", "") == "constGLchar*const*":
            params.append("(%s)gl_param_pointer(%s)" % (param.type_, arg))
        elif param.type_.replace(" ", "") == "constGLchar**":
            params.append("(%s)gl_param_pointer(%s)" % (param.type_, arg))
        elif param.type_.replace(" ", "") == "constGLcharARB**":
            params.append("(%s)gl_param_pointer(%s)" % (param.type_, arg))
        elif param.type_.replace(" ", "") == "unsignedint":
            params.append("(%s)gl_param_unsigned_int(%s)" % (param.type_, arg))
        elif param.type_.replace(" ", "") == "unsignedlong":
            params.append("(%s)gl_param_unsigned_int(%s)" % (param.type_, arg))
        elif "*" in param.type_:
            if "GLchar" in param.type_:
                params.append("(%s)gl_param_string(%s)" % (param.type_, arg))
            else:
                params.append("(%s)gl_param_pointer(%s)" % (param.type_, arg))
        else:
            params.append("(%s)gl_param_%s(%s)" % (param.type_, param.type_.replace("const", "").lstrip().rstrip(), arg))
        
        i += 1
    
    output.write("    real(%s);\n" % (", ".join(params)))
    
    output.write("replay_end_cmd(ctx, \"%s\", command);\n" % (name))
    
    output.write("}\n\n")

output.write("""static void reset_gl_funcs(trc_replay_context_t* ctx);

void init_replay_gl(trc_replay_context_t* ctx) {
    replay_gl_funcs_t* funcs = malloc(sizeof(replay_gl_funcs_t));
    ctx->_replay_gl = funcs;
    reset_gl_funcs(ctx);
""")

for name in gl.functions:
    if not name == "glXGetProcAddress" and name.startswith("glX"):
        output.write("    funcs->real_%s = (%s_t)glXGetProcAddress((const GLubyte*)\"%s\");\n" % (name, name, name))

output.write("""    ctx->_replay_gl = funcs;
}

static void reset_gl_funcs(trc_replay_context_t* ctx) {
    replay_gl_funcs_t* funcs = ctx->_replay_gl;
""")

for name in gl.functions:
    if not name.startswith("glX"):
        output.write("    funcs->real_%s = NULL;\n" % (name))

output.write("""}

static void reload_gl_funcs(trc_replay_context_t* ctx) {
    replay_gl_funcs_t* funcs = ctx->_replay_gl;
""")

for name in gl.functions:
    if not name.startswith("glX"):
        output.write("    funcs->real_%s = (%s_t)glXGetProcAddress((const GLubyte*)\"%s\");\n" % (name, name, name))

output.write("}\n\n")

output.write("""void deinit_replay_gl(trc_replay_context_t* ctx) {
    free(ctx->_replay_gl);
}
""")
