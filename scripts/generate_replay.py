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
#include "libtrace/libtrace.h"
#include "shared/glapi.h"

#define F(name) (((replay_gl_funcs_t*)ctx->_replay_gl)->real_##name)
#define RETURN do {replay_end_cmd(ctx, FUNC, command);return;} while(0)
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

static char** gl_param_string_array(trace_command_t* cmd, size_t index) {
    return trc_get_str(trc_get_arg(cmd, index));
}

static void* gl_param_data(trace_command_t* cmd, size_t index) {
    return *trc_get_data(trc_get_arg(cmd, index));
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

static void replay_get_back_color(trc_replay_context_t* ctx, trace_command_t* cmd) {
    if (F(glReadPixels)) {
        F(glFinish)();
        
        GLint last_buf;
        F(glGetIntegerv)(GL_READ_BUFFER, &last_buf);
        F(glReadBuffer)(GL_BACK);
        
        int w, h;
        SDL_GL_GetDrawableSize(ctx->window, &w, &h);
        
        void* data = malloc(w*h*4);
        F(glReadPixels)(0, 0, w, h, GL_RGBA, GL_UNSIGNED_BYTE, data);
        //TODO
        //inspect_replace_image(&cmd->state.back, w, h, data);
        free(data);
        
        F(glReadBuffer)(last_buf);
    }
}

static void replay_get_front_color(trc_replay_context_t* ctx, trace_command_t* cmd) {
    if (F(glReadPixels)) {
        F(glFinish)();
        
        GLint last_buf;
        F(glGetIntegerv)(GL_READ_BUFFER, &last_buf);
        F(glReadBuffer)(GL_FRONT);
        
        int w, h;
        SDL_GL_GetDrawableSize(ctx->window, &w, &h);
        
        void* data = malloc(w*h*4);
        F(glReadPixels)(0, 0, w, h, GL_RGBA, GL_UNSIGNED_BYTE, data);
        //TODO
        //inspect_replace_image(&cmd->state.front, w, h, data);
        free(data);
        
        F(glReadBuffer)(last_buf);
    }
}

static void replay_get_depth(trc_replay_context_t* ctx, trace_command_t* cmd) {
    if (F(glReadPixels)) {
        F(glFinish)();
        
        GLint last_buf;
        F(glGetIntegerv)(GL_READ_BUFFER, &last_buf);
        F(glReadBuffer)(GL_BACK);
        
        int w, h;
        SDL_GL_GetDrawableSize(ctx->window, &w, &h);
        
        void* data = malloc(w*h*4);
        F(glReadPixels)(0, 0, w, h, GL_DEPTH_COMPONENT, GL_UNSIGNED_INT, data);
        //TODO
        //inspect_replace_image(&cmd->state.depth, w, h, data);
        free(data);
        
        F(glReadBuffer)(last_buf);
    }
}

static void replay_get_tex_params(trc_replay_context_t* ctx,
                                  trace_command_t* cmd,
                                  GLenum target) {
    GLint tex;
    switch (target) {
    case GL_TEXTURE_1D: {
        F(glGetIntegerv)(GL_TEXTURE_BINDING_1D, &tex);
        break;
    }
    case GL_TEXTURE_2D: {
        F(glGetIntegerv)(GL_TEXTURE_BINDING_2D, &tex);
        break;
    }
    case GL_TEXTURE_3D: {
        F(glGetIntegerv)(GL_TEXTURE_BINDING_3D, &tex);
        break;
    }
    case GL_TEXTURE_CUBE_MAP: {
        F(glGetIntegerv)(GL_TEXTURE_BINDING_CUBE_MAP, &tex);
        break;
    }
    default: {
        return;
    }
    }
    
    //TODO: Only do these if it is supported
    //TODO
    /*inspect_gl_tex_params_t params;
    params.texture = replay_get_fake_object(ctx, ReplayObjType_GLTexture, tex);
    params.type = target;
    F(glGetTexParameteriv)(target, GL_DEPTH_STENCIL_TEXTURE_MODE, (GLint*)&params.depth_stencil_mode);
    F(glGetTexParameteriv)(target, GL_TEXTURE_MAG_FILTER, (GLint*)&params.mag_filter);
    F(glGetTexParameteriv)(target, GL_TEXTURE_MIN_FILTER, (GLint*)&params.min_filter);
    F(glGetTexParameterfv)(target, GL_TEXTURE_MIN_LOD, &params.min_lod);
    F(glGetTexParameterfv)(target, GL_TEXTURE_MAX_LOD, &params.max_lod);
    F(glGetTexParameterfv)(target, GL_TEXTURE_LOD_BIAS, &params.lod_bias);
    F(glGetTexParameteriv)(target, GL_TEXTURE_BASE_LEVEL, &params.base_level);
    F(glGetTexParameteriv)(target, GL_TEXTURE_MAX_LEVEL, &params.max_level);
    F(glGetTexParameteriv)(target, GL_TEXTURE_SWIZZLE_RGBA, (GLint*)params.swizzle);
    F(glGetTexParameteriv)(target, GL_TEXTURE_WRAP_S, params.wrap);
    F(glGetTexParameteriv)(target, GL_TEXTURE_WRAP_T, params.wrap+1);
    F(glGetTexParameteriv)(target, GL_TEXTURE_WRAP_R, params.wrap+2);
    F(glGetTexParameterfv)(target, GL_TEXTURE_BORDER_COLOR, params.border_color);
    F(glGetTexParameteriv)(target, GL_TEXTURE_COMPARE_MODE, (GLint*)&params.compare_mode);
    F(glGetTexParameteriv)(target, GL_TEXTURE_COMPARE_FUNC, (GLint*)&params.compare_func);
    F(glGetTexParameteriv)(target, GL_TEXTURE_VIEW_MIN_LEVEL, &params.view_min_level);
    F(glGetTexParameteriv)(target, GL_TEXTURE_VIEW_NUM_LEVELS, (GLint*)&params.view_num_levels);
    F(glGetTexParameteriv)(target, GL_TEXTURE_VIEW_MIN_LAYER, &params.view_min_layer);
    F(glGetTexParameteriv)(target, GL_TEXTURE_VIEW_NUM_LAYERS, (GLint*)&params.view_num_layers);
    F(glGetTexParameteriv)(target, GL_TEXTURE_IMMUTABLE_LEVELS, (GLint*)&params.immutable_levels);
    F(glGetTexParameteriv)(target, GL_IMAGE_FORMAT_COMPATIBILITY_TYPE, (GLint*)&params.image_format_compatibility_type);
    F(glGetTexParameteriv)(target, GL_TEXTURE_IMMUTABLE_FORMAT, (GLint*)&params.immutable_format);
    inspect_act_tex_params(&cmd->state, params.texture, &params);*/
}

static GLenum get_tex_binding(GLenum type) {
    switch (type) {
    case GL_TEXTURE_1D:
        return GL_TEXTURE_BINDING_1D;
    case GL_TEXTURE_1D_ARRAY:
        return GL_TEXTURE_BINDING_1D_ARRAY;
    case GL_TEXTURE_2D:
        return GL_TEXTURE_BINDING_2D;
    case GL_TEXTURE_2D_ARRAY:
        return GL_TEXTURE_BINDING_2D_ARRAY;
    case GL_TEXTURE_2D_MULTISAMPLE:
        return GL_TEXTURE_BINDING_2D_MULTISAMPLE;
    case GL_TEXTURE_2D_MULTISAMPLE_ARRAY:
        return GL_TEXTURE_BINDING_2D_MULTISAMPLE_ARRAY;
    case GL_TEXTURE_3D:
        return GL_TEXTURE_BINDING_3D;
    case GL_TEXTURE_CUBE_MAP:
        return GL_TEXTURE_BINDING_CUBE_MAP;
    case GL_TEXTURE_RECTANGLE:
        return GL_TEXTURE_BINDING_RECTANGLE;
    case GL_TEXTURE_CUBE_MAP_ARRAY:
        return GL_TEXTURE_BINDING_CUBE_MAP_ARRAY;
    default:
        return GL_NONE;
    }
}

static size_t min_size_t(size_t a, size_t b) {
    return a < b ? a : b;
}

static void replay_alloc_tex(trc_replay_context_t* ctx, trace_command_t* cmd, GLenum target,
                             size_t level, size_t width, size_t height, size_t depth, size_t layers,
                             size_t faces) {
    for (uint i = 0; i < level; i++) {
        width *= 2;
        height *= 2;
        depth *= 2;
    }
    
    size_t mipmaps = 1;
    if (!height) mipmaps = log2(width) + 1;
    else if (!depth) mipmaps = log2(min_size_t(width, height)) + 1;
    else mipmaps = log2(min_size_t(min_size_t(width, height), depth)) + 1;
    
    width = width ? width : 1;
    height = height ? height : 1;
    depth = depth ? depth : 1;
    
    GLint tex;
    F(glGetIntegerv)(get_tex_binding(target), &tex);
    
    //TODO
    //inspect_act_alloc_tex(&cmd->state, replay_get_fake_object(ctx, ReplayObjType_GLTexture, tex), mipmaps, layers, width, height, depth);
}

static void replay_get_tex_data(trc_replay_context_t* ctx,
                                trace_command_t* cmd,
                                GLenum target,
                                GLint level) {
    GLint tex;
    F(glGetIntegerv)(get_tex_binding(target), &tex);
    
    GLint width, height, depth;
    F(glGetTexLevelParameteriv)(target, level, GL_TEXTURE_WIDTH, &width);
    F(glGetTexLevelParameteriv)(target, level, GL_TEXTURE_HEIGHT, &height);
    F(glGetTexLevelParameteriv)(target, level, GL_TEXTURE_DEPTH, &depth);
    
    GLint alignment;
    F(glGetIntegerv)(GL_PACK_ALIGNMENT, &alignment);
    F(glPixelStorei)(GL_PACK_ALIGNMENT, 4);
    
    //TODO
    /*GLuint fake = replay_get_fake_object(ctx, ReplayObjType_GLTexture, tex);
    
    void* data = NULL;
    if (target == GL_TEXTURE_1D) {
        data = malloc(width*4);
        F(glGetTexImage)(target, level, GL_RGBA, GL_UNSIGNED_BYTE, data);
    } else if (target == GL_TEXTURE_2D) {
        data = malloc(width*height*4);
        F(glGetTexImage)(target, level, GL_RGBA, GL_UNSIGNED_BYTE, data);
    } else if (target == GL_TEXTURE_3D) {
        data = malloc(width*height*depth*4);
        F(glGetTexImage)(target, level, GL_RGBA, GL_UNSIGNED_BYTE, data);
    } else if (target == GL_TEXTURE_CUBE_MAP) {
        //TODO
    }
    
    if (target == GL_TEXTURE_1D)
        inspect_act_tex_data(&cmd->state, fake, level, 0, 0, width, 1, data);
    else if (target == GL_TEXTURE_2D)
        inspect_act_tex_data(&cmd->state, fake, level, 0, 0, width, height, data);
    else if (target == GL_TEXTURE_3D)
        for (GLint i = 0; i < depth; i++)
            inspect_act_tex_data(&cmd->state, fake, level, i, 0, width, height, ((uint8_t*)data)+width*height*4*i);
    else if (target == GL_TEXTURE_CUBE_MAP)
        ; //TODO
    
    free(data);*/
    
    F(glPixelStorei)(GL_PACK_ALIGNMENT, alignment);
}

static GLuint get_bound_buffer(trc_replay_context_t* ctx, GLenum target) {
    switch (target) {
    case GL_ARRAY_BUFFER:
        return trc_get_gl_state(ctx->trace)->array_buffer;
    case GL_ATOMIC_COUNTER_BUFFER:
        return trc_get_gl_state(ctx->trace)->atomic_counter_buffer;
    case GL_COPY_READ_BUFFER:
        return trc_get_gl_state(ctx->trace)->copy_read_buffer;
    case GL_COPY_WRITE_BUFFER:
        return trc_get_gl_state(ctx->trace)->copy_write_buffer;
    case GL_DISPATCH_INDIRECT_BUFFER:
        return trc_get_gl_state(ctx->trace)->dispatch_indirect_buffer;
    case GL_DRAW_INDIRECT_BUFFER:
        return trc_get_gl_state(ctx->trace)->draw_indirect_buffer;
    case GL_ELEMENT_ARRAY_BUFFER:
        return trc_get_gl_state(ctx->trace)->element_array_buffer;
    case GL_PIXEL_PACK_BUFFER:
        return trc_get_gl_state(ctx->trace)->pixel_pack_buffer;
    case GL_PIXEL_UNPACK_BUFFER:
        return trc_get_gl_state(ctx->trace)->pixel_unpack_buffer;
    case GL_QUERY_BUFFER:
        return trc_get_gl_state(ctx->trace)->query_buffer;
    case GL_SHADER_STORAGE_BUFFER:
        return trc_get_gl_state(ctx->trace)->shader_storage_buffer;
    case GL_TEXTURE_BUFFER:
        return trc_get_gl_state(ctx->trace)->texture_buffer;
    case GL_TRANSFORM_FEEDBACK_BUFFER:
        return trc_get_gl_state(ctx->trace)->transform_feedback_buffer;
    case GL_UNIFORM_BUFFER:
        return trc_get_gl_state(ctx->trace)->uniform_buffer;
    default:
        return 0;
    }
}

static GLint uniform(trc_replay_context_t* ctx, trace_command_t* cmd) {
    const trc_gl_program_rev_t* rev = trc_get_gl_program(ctx->trace, trc_get_gl_state(ctx->trace)->bound_program);
    for (size_t i = 0; i < rev->uniform_count; i++)
        if (rev->uniforms[i*2+1] == gl_param_GLint(cmd, 0)) return rev->uniforms[i*2];
    return -1;
}

static GLint get_bound_framebuffer(trc_replay_context_t* ctx, GLenum target) {
    GLint fb;
    switch (target) {
    case GL_DRAW_FRAMEBUFFER:
        F(glGetIntegerv)(GL_DRAW_FRAMEBUFFER_BINDING, &fb);
        break;
    case GL_READ_FRAMEBUFFER:
        F(glGetIntegerv)(GL_READ_FRAMEBUFFER_BINDING, &fb);
        break;
    case GL_FRAMEBUFFER:
        F(glGetIntegerv)(GL_DRAW_FRAMEBUFFER_BINDING, &fb);
        break;
    }
    
    //TODO
    //return replay_get_real_object(ctx, ReplayObjType_GLFramebuffer, fb);
}

static void framebuffer_attachment(trace_command_t* cmd, trc_replay_context_t* ctx, GLuint fb, GLenum attachment, GLuint tex, GLuint level) {
    //TODO
    /*switch (attachment) {
    case GL_DEPTH_ATTACHMENT:
        replay_set_depth_tex(ctx, fb, tex, level);
        inspect_act_fb_depth(&cmd->state, fb, tex, level);
        break;
    case GL_STENCIL_ATTACHMENT:
        replay_set_stencil_tex(ctx, fb, tex, level);
        inspect_act_fb_stencil(&cmd->state, fb, tex, level);
        break;
    case GL_DEPTH_STENCIL_ATTACHMENT:
        replay_set_depth_stencil_tex(ctx, fb, tex, level);
        inspect_act_fb_depth_stencil(&cmd->state, fb, tex, level);
        break;
    default:
        replay_set_color_tex(ctx, fb, attachment-GL_COLOR_ATTACHMENT0, tex, level);
        inspect_act_fb_color(&cmd->state, fb, attachment-GL_COLOR_ATTACHMENT0, tex, level);
        break;
    }*/
}

static void update_renderbuffer(trc_replay_context_t* ctx, trace_command_t* cmd) {
    GLint buf;
    F(glGetIntegerv)(GL_RENDERBUFFER_BINDING, &buf);
    //TODO
    /*buf = replay_get_fake_object(ctx, ReplayObjType_GLRenderbuffer, buf);
    
    if (buf) {
        GLint width, height, internal_format, sample_count, red_size;
        GLint green_size, blue_size, alpha_size, depth_size, stencil_size;
        F(glGetRenderbufferParameteriv)(GL_RENDERBUFFER, GL_RENDERBUFFER_WIDTH, &width);
        F(glGetRenderbufferParameteriv)(GL_RENDERBUFFER, GL_RENDERBUFFER_HEIGHT, &height);
        F(glGetRenderbufferParameteriv)(GL_RENDERBUFFER, GL_RENDERBUFFER_INTERNAL_FORMAT, &internal_format);
        F(glGetRenderbufferParameteriv)(GL_RENDERBUFFER, GL_RENDERBUFFER_SAMPLES, &sample_count);
        F(glGetRenderbufferParameteriv)(GL_RENDERBUFFER, GL_RENDERBUFFER_RED_SIZE, &red_size);
        F(glGetRenderbufferParameteriv)(GL_RENDERBUFFER, GL_RENDERBUFFER_GREEN_SIZE, &green_size);
        F(glGetRenderbufferParameteriv)(GL_RENDERBUFFER, GL_RENDERBUFFER_BLUE_SIZE, &blue_size);
        F(glGetRenderbufferParameteriv)(GL_RENDERBUFFER, GL_RENDERBUFFER_ALPHA_SIZE, &alpha_size);
        F(glGetRenderbufferParameteriv)(GL_RENDERBUFFER, GL_RENDERBUFFER_DEPTH_SIZE, &depth_size);
        F(glGetRenderbufferParameteriv)(GL_RENDERBUFFER, GL_RENDERBUFFER_STENCIL_SIZE, &stencil_size);
        inspect_rb_t rb;
        rb.fake = buf;
        rb.width = width;
        rb.height = height;
        rb.internal_format = internal_format;
        rb.sample_count = sample_count;
        rb.red_size = red_size;
        rb.green_size = green_size;
        rb.blue_size = blue_size;
        rb.alpha_size = alpha_size;
        rb.depth_size = depth_size;
        rb.stencil_size = stencil_size;
        inspect_act_set_rb(&cmd->state, &rb);
    }*/
}

static void update_query_type(trc_replay_context_t* ctx, trace_command_t* cmd, GLenum target) {
    GLint id;
    F(glGetQueryiv)(target, GL_CURRENT_QUERY, &id);
    if (!id)
        return;
    
    //TODO
    /*inspect_query_t query;
    query.fake = replay_get_fake_object(ctx, ReplayObjType_GLQuery, id);
    query.type = target;
    query.result = 0;
    inspect_act_set_query(&cmd->state, &query);*/
}

static void update_query(trc_replay_context_t* ctx, trace_command_t* cmd, GLenum target, GLuint id) {
    if (!id)
        return;
    
    GLint res = 0;
    if (target!=GL_TIME_ELAPSED && target!=GL_TIMESTAMP) {
        F(glFinish)();
        
        while (!res) F(glGetQueryObjectiv)(id, GL_QUERY_RESULT_AVAILABLE, &res);
        
        //TODO: Use glGetQueryObjecti64v when available
        F(glGetQueryObjectiv)(id, GL_QUERY_RESULT, &res);
    }
    
    //TODO
    /*inspect_query_t query;
    query.fake = replay_get_fake_object(ctx, ReplayObjType_GLQuery, id);
    query.type = target;
    query.result = res;
    inspect_act_set_query(&cmd->state, &query);*/
}

static void update_drawbuffer(trc_replay_context_t* ctx, trace_command_t* cmd, GLenum buffer, GLint drawbuffer) {
    GLint fb;
    F(glGetIntegerv)(GL_DRAW_FRAMEBUFFER_BINDING, &fb);
    
    //TODO: Support depth stencil attachments
    if (fb) {
        GLint attach;
        
        switch (buffer) {
        case GL_COLOR: {
            GLint maxattach;
            F(glGetIntegerv)(GL_MAX_COLOR_ATTACHMENTS, &maxattach);
            
            F(glGetIntegerv)(GL_DRAW_BUFFER0+drawbuffer, &attach);
            
            if (attach < GL_COLOR_ATTACHMENT0 ||
                attach>=GL_COLOR_ATTACHMENT0+maxattach)
                return;
            break;
        }
        case GL_DEPTH: {
            attach = GL_DEPTH_ATTACHMENT;
            break;
        }
        case GL_STENCIL: {
            attach = GL_STENCIL_ATTACHMENT;
            break;
        }
        }
        
        GLint type;
        F(glGetFramebufferAttachmentParameteriv)(GL_DRAW_FRAMEBUFFER,
                                                 attach,
                                                 GL_FRAMEBUFFER_ATTACHMENT_OBJECT_TYPE,
                                                 &type);
        
        if (type == GL_TEXTURE) {
            GLint tex;
            F(glGetFramebufferAttachmentParameteriv)(GL_DRAW_FRAMEBUFFER,
                                                     attach,
                                                     GL_FRAMEBUFFER_ATTACHMENT_OBJECT_NAME,
                                                     &tex);
            
            GLint level;
            F(glGetFramebufferAttachmentParameteriv)(GL_DRAW_FRAMEBUFFER,
                                                     attach,
                                                     GL_FRAMEBUFFER_ATTACHMENT_TEXTURE_LEVEL,
                                                     &level);
            
            return; //TODO
            GLenum type = 0; //replay_get_tex_type(ctx, replay_get_fake_object(ctx, ReplayObjType_GLTexture, tex));
            if (!type) return;
            
            GLint last;
            F(glGetIntegerv)(get_tex_binding(type), &last);
            F(glBindTexture)(type, tex);
            
            //TODO
            //replay_get_tex_data(ctx, cmd, GL_TEXTURE_2D, level);
            
            F(glBindTexture)(type, last);
        }
    } else if (drawbuffer == 0) {
        switch (buffer) {
        case GL_COLOR:
            replay_get_back_color(ctx, cmd);
            break;
        case GL_DEPTH:
            replay_get_depth(ctx, cmd);
            break;
        case GL_STENCIL:
            break;
        }
    }
}

static void begin_draw(trc_replay_context_t* ctx) {
    const trc_gl_vao_rev_t* vao = trc_get_gl_vao(ctx->trace, trc_get_gl_state(ctx->trace)->bound_vao);
    const trc_gl_program_rev_t* program = trc_get_gl_program(ctx->trace, trc_get_gl_state(ctx->trace)->bound_program);
    
    for (size_t i = 0; i < vao->attrib_count; i++) {
        GLint real_loc = -1;
        for (size_t j = 0; j < program->vertex_attrib_count; j++) {
            if (program->vertex_attribs[j*2+1] == j) {
                real_loc = program->vertex_attribs[j*2];
                break;
            }
        }
        if (real_loc < 0) continue;
        
        trc_gl_vao_attrib_t* a = &vao->attribs[i];
        if (a->enabled) F(glEnableVertexAttribArray)(real_loc);
        else F(glDisableVertexAttribArray)(real_loc);
        
        if (a->buffer) {
            GLint last_buf;
            F(glGetIntegerv)(GL_ARRAY_BUFFER_BINDING, &last_buf);
            F(glBindBuffer)(GL_ARRAY_BUFFER, a->buffer);
            if (a->integer)
                F(glVertexAttribIPointer)(real_loc, a->size, a->type, a->stride, (const void*)(uintptr_t)a->offset);
            else
                F(glVertexAttribPointer)(real_loc, a->size, a->type, a->normalized, a->stride, (const void*)(uintptr_t)a->offset);
            F(glBindBuffer)(GL_ARRAY_BUFFER, last_buf);
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
        
        F(glVertexAttribDivisor)(real_loc, a->divisor);
    }
}

static void end_draw(trc_replay_context_t* ctx, trace_command_t* cmd) {
    GLint maxbuf;
    F(glGetIntegerv)(GL_MAX_DRAW_BUFFERS, &maxbuf);
    
    for (GLint i = 0; i < maxbuf; i++)
        update_drawbuffer(ctx, cmd, GL_COLOR, i);
    
    update_drawbuffer(ctx, cmd, GL_DEPTH, 0);
    update_drawbuffer(ctx, cmd, GL_STENCIL, 0);
}

static void replay_begin_cmd(trc_replay_context_t* ctx, const char* name, trace_command_t* cmd) {
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
    if (ctx->trace->inspection.cur_fake_context && F(glGetError))
        error = F(glGetError)();
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
            %s v[%d];
            F(glGet%sv)(%s, v);
            //TODO
            //set_state_%s(&cmd->state, \"%s\", %d, v);
        }
            """ % (ver_mask, type_str, type, get[2], type_str, get[0], type_str2, get[0], get[2]))

for v in enable_entries:
    output.write("            {\n                //GLboolean v = F(glIsEnabled)(%s);\n" % v)
    output.write("                //TODO //set_state_bool(&cmd->state, \"%s enabled\", 1, &v);\n" % v)
    output.write("            }\n")

output.write("    }\n}\n\n")

nontrivial_str = open("nontrivial_func_impls.txt").read()
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
        output.write("""    if (!ctx->trace->inspection.cur_fake_context) {
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
