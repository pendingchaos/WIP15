#!/usr/bin/env python
# -*- coding: utf-8 -*-
# TODO: Nontrivial functions (very important)
# TODO: Return values (very important)
# TODO: Function pointers (very important)
# TODO: Integer arrays
import glxml
from gl_state import *

gl = glxml.GL(False)

output = open("../src/replay_gl.c", "w")

output.write("""#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <stdio.h>
#include "replay.h"
#include "libtrace.h"
#include "libinspect.h"
#include "glapi.h"

#define F(name) (((replay_gl_funcs_t*)ctx->_replay_gl)->real_##name)

typedef void (*_func)();

""")

output.write(gl.typedecls)

output.write("""
static GLuint* gl_param_GLuint_array(trace_value_t* val) {
    return NULL; //TODO
}

static const char* gl_param_string(trace_value_t* val) {
    return *trace_get_str(val);
}

static void* gl_param_data(trace_value_t* val) {
    return *trace_get_data(val);
}

static uint64_t gl_param_pointer(trace_value_t* val) {
    return *trace_get_ptr(val);
}

static GLDEBUGPROC gl_param_GLDEBUGPROC(trace_value_t* val) {
    return NULL; //TODO
}

static GLsizei gl_param_GLsizei(trace_value_t* val) {
    return *trace_get_int(val);
}

static GLint64EXT gl_param_GLint64EXT(trace_value_t* val) {
    return *trace_get_int(val);
}

static GLshort gl_param_GLshort(trace_value_t* val) {
    return *trace_get_int(val);
}

static int64_t gl_param_int64_t(trace_value_t* val) {
    return *trace_get_int(val);
}

static GLubyte gl_param_GLubyte(trace_value_t* val) {
    return *trace_get_uint(val);
}

static GLDEBUGPROCARB gl_param_GLDEBUGPROCARB(trace_value_t* val) {
    return NULL; //TODO
}

static GLboolean gl_param_GLboolean(trace_value_t* val) {
    return *trace_get_bool(val);
}

static Bool gl_param_Bool(trace_value_t* val) {
    return *trace_get_bool(val);
}

static GLbitfield gl_param_GLbitfield(trace_value_t* val) {
    return *trace_get_bitfield(val);
}

static uint64_t gl_param_GLsync(trace_value_t* val) {
    return *trace_get_ptr(val);
}

static GLuint gl_param_GLuint(trace_value_t* val) {
    return *trace_get_uint(val);
}

static GLint64 gl_param_GLint64(trace_value_t* val) {
    return *trace_get_int(val);
}

static int gl_param_int(trace_value_t* val) {
    return *trace_get_int(val);
}

static uint64_t gl_param_GLeglImageOES(trace_value_t* val) {
    return *trace_get_ptr(val);
}

static GLfixed gl_param_GLfixed(trace_value_t* val) {
    return *trace_get_double(val) * 65546.0f;
}

static GLclampf gl_param_GLclampf(trace_value_t* val) {
    return *trace_get_double(val);
}

static float gl_param_float(trace_value_t* val) {
    return *trace_get_double(val);
}

static GLhalfNV gl_param_GLhalfNV(trace_value_t* val) { //TODO
    return *trace_get_uint(val);
}

static uint64_t gl_param_GLintptr(trace_value_t* val) {
    return *trace_get_ptr(val);
}

static GLushort gl_param_GLushort(trace_value_t* val) {
    return *trace_get_uint(val);
}

static GLenum gl_param_GLenum(trace_value_t* val) {
    return *trace_get_uint(val);
}

static GLuint gl_param_unsigned_int(trace_value_t* val) {
    return *trace_get_uint(val);
}

static GLfloat gl_param_GLfloat(trace_value_t* val) {
    return *trace_get_double(val);
}

static GLuint64 gl_param_GLuint64(trace_value_t* val) {
    return *trace_get_uint(val);
}

static GLdouble gl_param_GLdouble(trace_value_t* val) {
    return *trace_get_double(val);
}

static GLhandleARB gl_param_GLhandleARB(trace_value_t* val) {
    return *trace_get_uint(val);
}

static uint64_t gl_param_GLintptrARB(trace_value_t* val) {
    return *trace_get_ptr(val);
}

static uint64_t gl_param_GLsizeiptr(trace_value_t* val)
{
    return *trace_get_ptr(val);
}

static GLint gl_param_GLint(trace_value_t* val)
{
    return *trace_get_int(val);
}

static GLclampx gl_param_GLclampx(trace_value_t* val) {
    return *trace_get_int(val);
}

static GLsizeiptrARB gl_param_GLsizeiptrARB(trace_value_t* val) {
    return *trace_get_ptr(val);
}

static GLuint64EXT gl_param_GLuint64EXT(trace_value_t* val) {
    return *trace_get_uint(val);
}

static uint64_t gl_param_GLvdpauSurfaceNV(trace_value_t* val) {
    return *trace_get_ptr(val);
}

static GLbyte gl_param_GLbyte(trace_value_t* val) {
    return *trace_get_int(val);
}

static GLclampd gl_param_GLclampd(trace_value_t* val) {
    return *trace_get_double(val);
}

static GLDEBUGPROCKHR gl_param_GLDEBUGPROCKHR(trace_value_t* val) {
    return NULL; //TODO
}

static GLDEBUGPROCAMD gl_param_GLDEBUGPROCAMD(trace_value_t* val) {
    return NULL; //TODO
}

static GLXPixmap gl_param_GLXPixmap(trace_value_t* val) {
    return *trace_get_uint(val);
}

static GLXWindow gl_param_GLXWindow(trace_value_t* val) {
    return *trace_get_uint(val);
}

static GLXPbuffer gl_param_GLXPbuffer(trace_value_t* val) {
    return *trace_get_uint(val);
}

static GLXDrawable gl_param_GLXDrawable(trace_value_t* val) {
    return *trace_get_uint(val);
}

static GLXVideoDeviceNV gl_param_GLXVideoDeviceNV(trace_value_t* val) {
    return *trace_get_uint(val);
}

static Pixmap gl_param_Pixmap(trace_value_t* val) {
    return *trace_get_uint(val);
}

static Window gl_param_Window(trace_value_t* val) {
    return *trace_get_uint(val);
}

static Font gl_param_Font(trace_value_t* val) {
    return *trace_get_uint(val);
}

static Colormap gl_param_Colormap(trace_value_t* val) {
    return *trace_get_uint(val);
}

static GLXContextID gl_param_GLXContextID(trace_value_t* val) {
    return *trace_get_uint(val);
}

static uint64_t gl_param_GLXFBConfig(trace_value_t* val) {
    return *trace_get_ptr(val);
}

static GLXVideoCaptureDeviceNV gl_param_GLXVideoCaptureDeviceNV(trace_value_t* val) {
    return *trace_get_uint(val);
}

static uint64_t gl_param_GLXFBConfigSGIX(trace_value_t* val) {
    return *trace_get_ptr(val);
}

static GLXPbufferSGIX gl_param_GLXPbufferSGIX(trace_value_t* val) {
    return *trace_get_uint(val);
}

static GLXVideoSourceSGIX gl_param_GLXVideoSourceSGIX(trace_value_t* val) {
    return *trace_get_uint(val);
}

static GLXContext gl_param_GLXContext(trace_value_t* val) {
    return (GLXContext)*trace_get_ptr(val);
}

static void reset_gl_funcs(replay_context_t* ctx);
static void reload_gl_funcs(replay_context_t* ctx);

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

output.write("""static uint64_t begin_time;

static uint64_t get_time() {
    struct timespec spec;
    clock_gettime(CLOCK_MONOTONIC, &spec);
    return spec.tv_sec*1000000000 + spec.tv_nsec;
}

static void set_state(inspect_gl_state_t* state, const char* name, trace_value_t v) {
    inspect_gl_state_entry_t entry;
    entry.name = name;
    entry.val = v;
    entry.val.group_index = -1;
    append_vec(state->entries, sizeof(inspect_gl_state_entry_t), &entry);
}

static void set_state_bool(inspect_gl_state_t* state, const char* name, size_t count, GLboolean* v) {
    bool arr[count];
    for (size_t i = 0; i < count; ++i) {
        arr[i] = v[i];
    }
    set_state(state, name, trace_create_bool(count, arr));
}

static void set_state_int(inspect_gl_state_t* state, const char* name, size_t count, GLint* v) {
    int64_t arr[count];
    for (size_t i = 0; i < count; ++i) {
        arr[i] = v[i];
    }
    set_state(state, name, trace_create_int(count, arr));
}

static void set_state_float(inspect_gl_state_t* state, const char* name, size_t count, GLfloat* v) {
    double arr[count];
    for (size_t i = 0; i < count; ++i) {
        arr[i] = v[i];
    }
    set_state(state, name, trace_create_double(count, arr));
}

static void set_state_str(inspect_gl_state_t* state, const char* name, const GLubyte* s) {
    set_state(state, name, trace_create_str(1, (const char**)&s));
}

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
        inspect_add_error((inspect_command_t*)user_param, "Error: '%s' from %s", message, source_str);
        break;
    }
    case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR: {
        inspect_add_warning((inspect_command_t*)user_param, "Deprecated behavior warning: '%s' from %s", message, source_str);
        break;
    }
    case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR: {
        inspect_add_warning((inspect_command_t*)user_param, "Undefined behavior warning: '%s' from %s", message, source_str);
        break;
    }
    case GL_DEBUG_TYPE_PORTABILITY: {
        inspect_add_warning((inspect_command_t*)user_param, "Portibility warning: '%s' from %s", message, source_str);
        break;
    }
    case GL_DEBUG_TYPE_PERFORMANCE: {
        inspect_add_warning((inspect_command_t*)user_param, "Performance warning: '%s' from %s", message, source_str);
        break;
    }
    case GL_DEBUG_TYPE_OTHER: {
        inspect_add_warning((inspect_command_t*)user_param, "Other: '%s' from %s", message, source_str);
        break;
    }
    case GL_DEBUG_TYPE_MARKER: {
        inspect_add_info((inspect_command_t*)user_param, "Marker: '%s' from %s", message, source_str);
        break;
    }
    }
}

static void replay_get_back_color(replay_context_t* ctx, inspect_command_t* cmd) {
    if (!ctx->_in_begin_end && F(glReadPixels)) {
        F(glFinish)();
        
        GLint last_buf;
        F(glGetIntegerv)(GL_READ_BUFFER, &last_buf);
        F(glReadBuffer)(GL_BACK);
        
        void* data = malloc(100*100*4);
        F(glReadPixels)(0, 0, 100, 100, GL_RGBA, GL_UNSIGNED_BYTE, data);
        cmd->state.back.width = 100;
        cmd->state.back.height = 100;
        cmd->state.back.data = data;
        
        F(glReadBuffer)(last_buf);
    }
}

static void replay_get_front_color(replay_context_t* ctx, inspect_command_t* cmd) {
    if (!ctx->_in_begin_end && F(glReadPixels)) {
        F(glFinish)();
        
        GLint last_buf;
        F(glGetIntegerv)(GL_READ_BUFFER, &last_buf);
        F(glReadBuffer)(GL_FRONT);
        
        void* data = malloc(100*100*4);
        F(glReadPixels)(0, 0, 100, 100, GL_RGBA, GL_UNSIGNED_BYTE, data);
        cmd->state.front.width = 100;
        cmd->state.front.height = 100;
        cmd->state.front.data = data;
        
        F(glReadBuffer)(last_buf);
    }
}

static void replay_get_depth(replay_context_t* ctx, inspect_command_t* cmd) {
    if (!ctx->_in_begin_end && F(glReadPixels)) {
        F(glFinish)();
        
        void* data = malloc(100*100*4);
        F(glReadPixels)(0, 0, 100, 100, GL_DEPTH_COMPONENT, GL_UNSIGNED_INT, data);
        cmd->state.depth.width = 100;
        cmd->state.depth.height = 100;
        cmd->state.depth.data = data;
    }
}

static void replay_get_tex_params(replay_context_t* ctx,
                                  inspect_command_t* cmd,
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
    
    inspect_gl_tex_params_t params;
    params.fake_texture = replay_get_fake_object(ctx, ReplayObjType_GLTexture, tex);
    params.type = target;
    F(glGetTexParameteriv)(target, GL_TEXTURE_MIN_FILTER, (GLint*)&params.min_filter);
    F(glGetTexParameteriv)(target, GL_TEXTURE_MAG_FILTER, (GLint*)&params.mag_filter);
    F(glGetTexParameterfv)(target, GL_TEXTURE_MIN_LOD, &params.min_lod);
    F(glGetTexParameterfv)(target, GL_TEXTURE_MAX_LOD, &params.max_lod);
    F(glGetTexParameteriv)(target, GL_TEXTURE_BASE_LEVEL, &params.base_level);
    F(glGetTexParameteriv)(target, GL_TEXTURE_MAX_LEVEL, (GLint*)&params.max_level);
    F(glGetTexParameteriv)(target, GL_TEXTURE_WRAP_S, (GLint*)&params.wrap_s);
    F(glGetTexParameteriv)(target, GL_TEXTURE_WRAP_S, (GLint*)&params.wrap_t);
    F(glGetTexParameteriv)(target, GL_TEXTURE_WRAP_R, (GLint*)&params.wrap_r);
    F(glGetTexParameterfv)(target, GL_TEXTURE_PRIORITY, &params.priority);
    F(glGetTexParameteriv)(target, GL_TEXTURE_COMPARE_MODE, (GLint*)&params.compare_mode);
    F(glGetTexParameteriv)(target, GL_TEXTURE_COMPARE_FUNC, (GLint*)&params.compare_func);
    F(glGetTexParameteriv)(target, GL_DEPTH_TEXTURE_MODE, (GLint*)&params.depth_texture_mode);
    
    GLint generate_mipmap;
    F(glGetTexParameteriv)(target, GL_GENERATE_MIPMAP, &generate_mipmap);
    params.generate_mipmap = generate_mipmap;
    
    F(glGetTexParameteriv)(target, GL_DEPTH_STENCIL_TEXTURE_MODE,(GLint*) &params.depth_stencil_mode);
    F(glGetTexParameterfv)(target, GL_TEXTURE_LOD_BIAS, &params.lod_bias);
    F(glGetTexParameteriv)(target, GL_TEXTURE_SWIZZLE_RGBA, (GLint*)params.swizzle);
    F(glGetTexParameterfv)(target, GL_TEXTURE_BORDER_COLOR, params.border_color);
    F(glGetTexLevelParameteriv)(target, 0, GL_TEXTURE_WIDTH, (GLint*)&params.width);
    F(glGetTexLevelParameteriv)(target, 0, GL_TEXTURE_HEIGHT, (GLint*)&params.height);
    F(glGetTexLevelParameteriv)(target, 0, GL_TEXTURE_DEPTH, (GLint*)&params.depth);
    F(glGetTexLevelParameteriv)(target, 0, GL_TEXTURE_INTERNAL_FORMAT, (GLint*)&params.internal_format);
    
    append_vec(cmd->state.texture_params, sizeof(inspect_gl_tex_params_t), &params);
}

static void replay_get_tex_data(replay_context_t* ctx,
                                inspect_command_t* cmd,
                                GLenum target,
                                GLint level) {
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
    
    if (target == GL_TEXTURE_2D) {
        GLint width;
        GLint height;
        F(glGetTexLevelParameteriv)(target, level, GL_TEXTURE_WIDTH, &width);
        F(glGetTexLevelParameteriv)(target, level, GL_TEXTURE_HEIGHT, &height);
        
        GLint alignment;
        F(glGetIntegerv)(GL_PACK_ALIGNMENT, &alignment);
        F(glPixelStorei)(GL_PACK_ALIGNMENT, 4);
        
        void* data = malloc(width*height*4);
        F(glGetTexImage)(target, level, GL_RGBA, GL_UNSIGNED_BYTE, data);
        
        F(glPixelStorei)(GL_PACK_ALIGNMENT, alignment);
        
        inspect_gl_tex_data_t tex_data;
        tex_data.fake_texture = tex;
        tex_data.mipmap = level;
        tex_data.data = data;
        tex_data.data_size = width*height*4;
        
        append_vec(cmd->state.texture_data, sizeof(inspect_gl_tex_data_t), &tex_data);
    } else {
        //TODO
    }
}

static void replay_begin_cmd(replay_context_t* ctx, const char* name, inspect_command_t* cmd) {
    if (!ctx->_in_begin_end) {
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
        
        if (F(glGetError))
            F(glGetError)();
    }
    begin_time = get_time();
}
""")

output.write("""
static void replay_end_cmd(replay_context_t* ctx, const char* name, inspect_command_t* cmd) {
    GLenum error = GL_NO_ERROR;
    
    if (ctx->_current_context) {
        if (F(glGetError) && !ctx->_in_begin_end) {
            error = F(glGetError)();
            F(glFlush)();
        }
    }
    
    uint64_t end_time = get_time();
    
    cmd->cpu_duration = end_time - begin_time;
    
    begin_time = get_time();
    if (ctx->_current_context) if (F(glFinish) && !ctx->_in_begin_end) F(glFinish)();
    end_time = get_time();
    
    cmd->gpu_duration = end_time - begin_time;
    
    switch (error) {
    case GL_NO_ERROR: {
        break;
    }
    case GL_INVALID_ENUM: {
        inspect_add_error(cmd, "Invalid enum");
        break;
    }
    case GL_INVALID_VALUE: {
        inspect_add_error(cmd, "Invalid value");
        break;
    }
    case GL_INVALID_OPERATION: {
        inspect_add_error(cmd, "Invalid operation");
        break;
    }
    case GL_STACK_OVERFLOW: {
        inspect_add_error(cmd, "Stack overflow");
        break;
    }
    case GL_STACK_UNDERFLOW: {
        inspect_add_error(cmd, "Stack underflow");
        break;
    }
    case GL_OUT_OF_MEMORY: {
        inspect_add_error(cmd, "Out of memory");
        break;
    }
    case GL_INVALID_FRAMEBUFFER_OPERATION: {
        inspect_add_error(cmd, "Invalid framebuffer operation");
        break;
    }
    case GL_CONTEXT_LOST: {
        inspect_add_error(cmd, "Context lost");
        break;
    }
    case GL_TABLE_TOO_LARGE: {
        inspect_add_error(cmd, "Table too large");
        break;
    }
    }
    
    if (!ctx->_in_begin_end && F(glGetIntegerv)) {
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
        if (((%s) & gl2_1) && F(glGetString))
            set_state_str(&cmd->state, \"%s\", F(glGetString)(%s));""" % (ver_mask, get[0], get[0]))
    elif get[1] not in ["I", "E"]:
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
            set_state_%s(&cmd->state, \"%s\", %d, v);
        }
            """ % (ver_mask, type_str, type, get[2], type_str, get[0], type_str2, get[0], get[2]))

for v in enable_entries:
    output.write("            {\n                GLboolean v = F(glIsEnabled)(%s);\n" % v)
    output.write("                set_state_bool(&cmd->state, \"%s enabled\", 1, &v);\n" % v)
    output.write("            }\n")

output.write("    }\n}\n\n")

nontrivial_str = open("nontrivial_func_impls.txt").read()
nontrivial = {}

current_name = ""
current = ""
for line in nontrivial_str.split("\n"):
    if line.endswith(":"):
        if len(current_name) != 0:
            nontrivial[current_name] = current
        
        current_name = line[:-1]
        current = ""
    else:
        current += line + "\n"
if len(current_name) != 0:
    nontrivial[current_name] = current

for name in gl.functions:
    output.write("void replay_%s(replay_context_t* ctx, trace_command_t* command, inspect_command_t* inspect_command) {\n" % (name))
    
    if not name.startswith("glX"):
        output.write("""    if (!ctx->_current_context) {
        inspect_add_error(inspect_command, "No current OpenGL context.");
        return;
    }
    """)
    
    output.write("replay_begin_cmd(ctx, \"%s\", inspect_command);\n" % (name))
    
    if name == "glXGetProcAddress":
        output.write("    %s_t real = &%s;" % (name, name))
    else:
        output.write("    %s_t real = ((replay_gl_funcs_t*)ctx->_replay_gl)->real_%s;\n" % (name, name))
    
    output.write("    do {(void)sizeof((real));} while (0);\n")
    
    if name in nontrivial:
        output.write(nontrivial[name])
        output.write("replay_end_cmd(ctx, \"%s\", inspect_command);\n" % (name))
        output.write("}\n\n")
        continue
    
    function = gl.functions[name]
    
    params = []
    
    i = 0
    for param in function.params:
        arg = "trace_get_arg(command, %d)->val" % (i)
        
        if param.type_[-1] == "]":
            params.append("gl_param_%s_array(&%s)" % (param.type_.split("[")[0], arg))
        elif param.type_.replace(" ", "") == "constGLchar*const*":
            params.append("(%s)gl_param_pointer(&%s)" % (param.type_, arg))
        elif param.type_.replace(" ", "") == "constGLchar**":
            params.append("(%s)gl_param_pointer(&%s)" % (param.type_, arg))
        elif param.type_.replace(" ", "") == "constGLcharARB**":
            params.append("(%s)gl_param_pointer(&%s)" % (param.type_, arg))
        elif param.type_.replace(" ", "") == "unsignedint":
            params.append("(%s)gl_param_unsigned_int(&%s)" % (param.type_, arg))
        elif param.type_.replace(" ", "") == "unsignedlong":
            params.append("(%s)gl_param_unsigned_int(&%s)" % (param.type_, arg))
        elif "*" in param.type_:
            if "GLchar" in param.type_:
                params.append("(%s)gl_param_string(&%s)" % (param.type_, arg))
            else:
                params.append("(%s)gl_param_pointer(&%s)" % (param.type_, arg))
        else:
            params.append("(%s)gl_param_%s(&%s)" % (param.type_, param.type_.replace("const", "").lstrip().rstrip(), arg))
        
        i += 1
    
    output.write("    real(%s);\n" % (", ".join(params)))
    
    output.write("replay_end_cmd(ctx, \"%s\", inspect_command);\n" % (name))
    
    output.write("}\n\n")

output.write("""static void reset_gl_funcs(replay_context_t* ctx);

void init_replay_gl(replay_context_t* ctx) {
    replay_gl_funcs_t* funcs = malloc(sizeof(replay_gl_funcs_t));
    ctx->_replay_gl = funcs;
    reset_gl_funcs(ctx);
""")

for name in gl.functions:
    if not name == "glXGetProcAddress" and name.startswith("glX"):
        output.write("    funcs->real_%s = (%s_t)glXGetProcAddress((const GLubyte*)\"%s\");\n" % (name, name, name))

output.write("""    ctx->_replay_gl = funcs;
}

static void reset_gl_funcs(replay_context_t* ctx) {
    replay_gl_funcs_t* funcs = ctx->_replay_gl;
""")

for name in gl.functions:
    if not name.startswith("glX"):
        output.write("    funcs->real_%s = NULL;\n" % (name))

output.write("""}

static void reload_gl_funcs(replay_context_t* ctx) {
    replay_gl_funcs_t* funcs = ctx->_replay_gl;
""")

for name in gl.functions:
    if not name.startswith("glX"):
        output.write("    funcs->real_%s = (%s_t)glXGetProcAddress((const GLubyte*)\"%s\");\n" % (name, name, name))

output.write("}\n\n")

output.write("""void deinit_replay_gl(replay_context_t* ctx) {
    free(ctx->_replay_gl);
}
""")
