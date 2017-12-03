#!/usr/bin/env python
# -*- coding: utf-8 -*-
import glapi.glxml
from glapi.glapi import *
from glob import glob

gl = glapi.glxml.GL(False)

# Write the header
output = open("../src/libtrace/replay.h", "w")

output.write('''#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <SDL2/SDL.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <stdio.h>
#include <assert.h>
#include "libtrace/libtrace.h"
#include "shared/glapi.h"
#include "shared/utils.h"

#define F(name) (((replay_gl_funcs_t*)((const trc_replay_context_t*)ctx)->_replay_gl)->real_##name)
#define RETURN do {goto end;} while (0)
#define ERROR(...) do {trc_add_error(cmd, __VA_ARGS__); RETURN;} while (0)
#define ERROR2(ret, ...) do {trc_add_error(cmd, __VA_ARGS__); return ret;} while (0)
#define FUNC ""

''')

output.write(gl.typedecls)

output.write('''typedef void (*func_t)();

extern trc_replay_context_t* ctx;
extern trace_command_t* cmd;

static inline uint64_t replay_get_uint(trace_value_t* val, size_t i) {
    return trc_get_uint(val)[i];
}

static inline int64_t replay_get_int(trace_value_t* val, size_t i) {
    return trc_get_int(val)[i];
}

static inline double replay_get_double(trace_value_t* val, size_t i) {
    return trc_get_double(val)[i];
}

static inline uint64_t replay_get_ptr(trace_value_t* val, size_t i) {
    return trc_get_ptr(val)[i];
}

static inline bool replay_get_bool(trace_value_t* val, size_t i) {
    return trc_get_bool(val)[i];
}

void* replay_alloc(size_t amount);

void reset_gl_funcs();
void reload_gl_funcs();

func_t glXGetProcAddress(const GLubyte* procName);

void replay_debug_callback(GLenum source, GLenum type, GLuint id, GLenum severity,
                           GLsizei length, const GLchar* message, const void* user_param);

void replay_begin_cmd(trc_replay_context_t* ctx, const char* name, trace_command_t* cmd);
void replay_end_cmd(trc_replay_context_t* ctx, const char* name, trace_command_t* cmd);

''')

objs = [('buffer', 'TrcBuffer'),
        ('sampler', 'TrcSampler'),
        ('texture', 'TrcTexture'),
        ('query', 'TrcQuery'),
        ('framebuffer', 'TrcFramebuffer'),
        ('renderbuffer', 'TrcRenderbuffer'),
        ('sync', 'TrcSync'),
        ('program', 'TrcProgram'),
        ('program_pipeline', 'TrcProgramPipeline'),
        ('shader', 'TrcShader'),
        ('vao', 'TrcVAO'),
        ('transform_feedback', 'TrcTransformFeedback')]
output.write('#pragma GCC diagnostic ignored "-Wunused-function"')
for n, t in objs:
    container = n in ['framebuffer', 'program_pipeline', 'transform_feedback', 'vao']
    ns = 'priv_ns' if container else 'ns'
    output.write('''
static inline const trc_gl_%s_rev_t* get_%s(uint64_t fake) {
    return trc_get_obj(ctx->%s, %s, fake);
}

static inline void set_%s(const trc_gl_%s_rev_t* rev) {
    trc_obj_set_rev(rev->head.obj, rev);
}

static inline uint64_t get_real_%s(uint64_t fake) {
    const trc_gl_%s_rev_t* rev = get_%s(fake);
    return rev ? rev->real : 0;
}
''' % (n, n, ns, t, n, n, n, n, n))
output.write('#pragma GCC diagnostic pop\n\n')

for name in list(func_dict.keys()):
    function = gl.functions[name]
    params = []
    for param in function.params:
        params.append("%s" % (param.type_))
    
    output.write("typedef %s (*%s_t)(%s);\n" % (function.returnType, name, ", ".join(params)))

output.write("typedef struct {\n")
for name in list(func_dict.keys()):
    output.write("    %s_t real_%s;\n" % (name, name))
output.write("} replay_gl_funcs_t;\n\n")

for k, v in gl.enumValues.items():
    output.write("#define %s %s\n" % (k, v))

# Write the implementation
output = open("../src/libtrace/replay.c", "w")

output.write('''#include "libtrace/replay.h"

trc_replay_context_t* ctx;
trace_command_t* cmd;

static size_t replay_alloc_data_size;
static uint8_t replay_alloc_data[65536];
static void* replay_alloc_big_data;

void* replay_alloc(size_t amount) {
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

void replay_debug_callback(GLenum source, GLenum type, GLuint id, GLenum severity,
                           GLsizei length, const GLchar* message, const void* user_param) {
    const char* source_str = "unknown";
    
    switch (source) {
    case GL_DEBUG_SOURCE_API: source_str = "API"; break;
    case GL_DEBUG_SOURCE_WINDOW_SYSTEM: source_str = "system"; break;
    case GL_DEBUG_SOURCE_SHADER_COMPILER: source_str = "shader compiler"; break;
    case GL_DEBUG_SOURCE_THIRD_PARTY: source_str = "third party"; break;
    case GL_DEBUG_SOURCE_APPLICATION: source_str = "application"; break;
    case GL_DEBUG_SOURCE_OTHER: source_str = "other"; break;
    }
    
    trace_command_t* cmd = ((trc_replay_context_t*)user_param)->current_command;
    if (!cmd) return;
    
    switch (type) {
    case GL_DEBUG_TYPE_ERROR:
        trc_add_error(cmd, "Error: '%s' from %s", message, source_str);
        break;
    case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR:
        trc_add_warning(cmd, "Deprecated behavior warning: '%s' from %s", message, source_str);
        break;
    case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:
        trc_add_warning(cmd, "Undefined behavior warning: '%s' from %s", message, source_str);
        break;
    case GL_DEBUG_TYPE_PORTABILITY:
        trc_add_warning(cmd, "Portibility warning: '%s' from %s", message, source_str);
        break;
    case GL_DEBUG_TYPE_PERFORMANCE:
        trc_add_warning(cmd, "Performance warning: '%s' from %s", message, source_str);
        break;
    case GL_DEBUG_TYPE_OTHER:
        trc_add_warning(cmd, "Other: '%s' from %s", message, source_str);
        break;
    case GL_DEBUG_TYPE_MARKER:
        trc_add_info(cmd, "Marker: '%s' from %s", message, source_str);
        break;
    }
}

void replay_begin_cmd(trc_replay_context_t* ctx, const char* name, trace_command_t* cmd) {
    replay_alloc_data_size = 0;
    replay_alloc_big_data = NULL;
}

void replay_end_cmd(trc_replay_context_t* ctx, const char* name, trace_command_t* cmd) {
    while (replay_alloc_big_data) {
        void* next = *(void**)replay_alloc_big_data;
        free(replay_alloc_big_data);
        replay_alloc_big_data = next;
    }
    
    GLenum error = GL_NO_ERROR;
    if (trc_get_current_gl_context(ctx->trace, -1) && F(glGetError)) error = F(glGetError)();
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

''')

output.write("""void init_replay_gl(trc_replay_context_t* ctx_) {
    ctx = ctx_;
    replay_gl_funcs_t* funcs = malloc(sizeof(replay_gl_funcs_t));
    ctx->_replay_gl = funcs;
    reset_gl_funcs(ctx);
""")

for name in list(func_dict.keys()):
    if not name == "glXGetProcAddress" and name.startswith("glX"):
        output.write("    funcs->real_%s = (%s_t)glXGetProcAddress((const GLubyte*)\"%s\");\n" % (name, name, name))

output.write("""    ctx->_replay_gl = funcs;
}

void deinit_replay_gl() {
    free(ctx->_replay_gl);
}

void reset_gl_funcs() {
    replay_gl_funcs_t* funcs = ctx->_replay_gl;
""")

for name in list(func_dict.keys()):
    if not name.startswith("glX"):
        output.write("    funcs->real_%s = NULL;\n" % (name))

output.write("""}

void reload_gl_funcs() {
    replay_gl_funcs_t* funcs = ctx->_replay_gl;
""")

for name in list(func_dict.keys()):
    if not name.startswith("glX"):
        output.write("    funcs->real_%s = (%s_t)glXGetProcAddress((const GLubyte*)\"%s\");\n" % (name, name, name))

output.write("}\n\n")