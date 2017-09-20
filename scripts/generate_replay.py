#!/usr/bin/env python
# -*- coding: utf-8 -*-
import glapi.glxml
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
#include "shared/utils.h"

#define F(name) (((replay_gl_funcs_t*)((const trc_replay_context_t*)ctx)->_replay_gl)->real_##name)
#define RETURN do {goto end;} while (0)
#define ERROR(...) do {trc_add_error(cmd, __VA_ARGS__); RETURN;} while (0)
#define ERROR2(ret, ...) do {trc_add_error(cmd, __VA_ARGS__); return ret;} while (0)
#define FUNC ""

typedef void (*func_t)();

""")

output.write(gl.typedecls)

output.write("""
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

static void reset_gl_funcs(trc_replay_context_t* ctx);
static void reload_gl_funcs(trc_replay_context_t* ctx);

extern func_t glXGetProcAddress(const GLubyte* procName);

""")

output.write("\n")

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

static void replay_begin_cmd(trc_replay_context_t* ctx, const char* name, trace_command_t* cmd) {
    replay_alloc_data_size = 0;
    replay_alloc_big_data = NULL;
}

static void replay_end_cmd(trc_replay_context_t* ctx, const char* name, trace_command_t* cmd) {
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
""")

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
static const trc_gl_%s_rev_t* get_%s(trc_replay_context_t* ctx, uint64_t fake) {
    return trc_get_obj(ctx->%s, %s, fake);
}

static void set_%s(const trc_gl_%s_rev_t* rev) {
    trc_obj_set_rev(rev->head.obj, rev);
}

static uint64_t get_real_%s(trc_replay_context_t* ctx, uint64_t fake) {
    const trc_gl_%s_rev_t* rev = get_%s(ctx, fake);
    return rev ? rev->real : 0;
}
''' % (n, n, ns, t, n, n, n, n, n))
output.write('#pragma GCC diagnostic pop\n')

nontrivial_str = open("nontrivial_func_impls.c").read()
nontrivial = {}

current_name = ""
current = ""
print_func = False
for line in nontrivial_str.split("\n"):
    if line.split('//')[0].rstrip().endswith(":") and (line.startswith("gl") or line.startswith("wip15")):
        if current_name != '':
            nontrivial[current_name] = current
        current_name = line.split('//')[0].rstrip()[:-1]
        f = gl.functions[current_name]
        print_func =  '//' not in line
        if print_func: print('\n%s: //%s' % (current_name, ', '.join(["%s p_%s"%(p.type_, p.name) for p in f.params])))
        current = ""
    elif line == '' and current_name!='':
        nontrivial[current_name] = current
        current_name = ''
        current= ''
        if print_func: print(line)
    elif current_name=='':
        output.write(line + '\n')
        if print_func: print(line)
    else:
        current += line + "\n"
        if print_func: print(line)
if current_name != "":
    nontrivial[current_name] = current

output.write("""#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-variable"
""")
for name, func in func_dict.items():
    output.write("void replay_%s(trc_replay_context_t* ctx, trace_command_t* cmd) {\n" % (name))
    
    if not name.startswith("glX") and not name.startswith("wip15"):
        output.write("""    if (!trc_get_current_gl_context(ctx->trace, -1)) {
        trc_add_error(cmd, "No current OpenGL context.");
        return;
    } else {
        trc_obj_t* cur_ctx = trc_get_current_gl_context(ctx->trace, -1);
        ctx->ns = ((const trc_gl_context_rev_t*)trc_obj_get_rev(cur_ctx, -1))->namespace;
        ctx->priv_ns = ((const trc_gl_context_rev_t*)trc_obj_get_rev(cur_ctx, -1))->priv_ns;
    }
    """)
    else:
        output.write("""    ctx->ns = NULL;
    """)
    
    output.write("replay_begin_cmd(ctx, \"%s\", cmd);\n" % (name))
    
    if name == "glXGetProcAddress":
        output.write("    %s_t real = &%s;" % (name, name))
    else:
        output.write("    %s_t real = ((replay_gl_funcs_t*)ctx->_replay_gl)->real_%s;\n" % (name, name))
    
    output.write("    do {(void)sizeof((real));} while (0);\n")
    
    function = gl.functions[name]
    
    for i, param in zip(list(range(len(func.params))), func.params):
        output.write('    trace_value_t* arg_%s = &cmd->args[%d];\n' % (param.name, i))
        output.write(param.dtype.gen_replay_read_code('p_'+param.name, 'arg_'+param.name, param.array_count)+'\n')
    
    if name in nontrivial:
        output.write(nontrivial[name])
    else:
        output.write('ERROR("Function not implemented D:\\n");\n')
    
    #The `goto end;` is to remove "label defined but not used" warnings
    output.write('goto end; end: ;\n')
    
    for i, param in zip(list(range(len(func.params))), func.params):
        output.write(param.dtype.gen_replay_finalize_code('p_'+param.name, 'arg_'+param.name, param.array_count)+'\n')
    
    output.write("replay_end_cmd(ctx, \"%s\", cmd);\n" % (name))
    
    if name in nontrivial: output.write("#undef FUNC\n#define FUNC \"%s\"\n" % (name))
    
    output.write("}\n\n")
output.write("""#pragma GCC diagnostic pop
""")

output.write("""static void reset_gl_funcs(trc_replay_context_t* ctx);

void init_replay_gl(trc_replay_context_t* ctx) {
    replay_gl_funcs_t* funcs = malloc(sizeof(replay_gl_funcs_t));
    ctx->_replay_gl = funcs;
    reset_gl_funcs(ctx);
""")

for name in list(func_dict.keys()):
    if not name == "glXGetProcAddress" and name.startswith("glX"):
        output.write("    funcs->real_%s = (%s_t)glXGetProcAddress((const GLubyte*)\"%s\");\n" % (name, name, name))

output.write("""    ctx->_replay_gl = funcs;
}

static void reset_gl_funcs(trc_replay_context_t* ctx) {
    replay_gl_funcs_t* funcs = ctx->_replay_gl;
""")

for name in list(func_dict.keys()):
    if not name.startswith("glX"):
        output.write("    funcs->real_%s = NULL;\n" % (name))

output.write("""}

static void reload_gl_funcs(trc_replay_context_t* ctx) {
    replay_gl_funcs_t* funcs = ctx->_replay_gl;
""")

for name in list(func_dict.keys()):
    if not name.startswith("glX"):
        output.write("    funcs->real_%s = (%s_t)glXGetProcAddress((const GLubyte*)\"%s\");\n" % (name, name, name))

output.write("}\n\n")

output.write("""void deinit_replay_gl(trc_replay_context_t* ctx) {
    free(ctx->_replay_gl);
}
""")
