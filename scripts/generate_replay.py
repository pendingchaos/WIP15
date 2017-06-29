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
#define ERROR2(ret, ...) do {trc_add_error(cmd, __VA_ARGS__); return ret;} while (0)
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
    return *trc_get_str(&cmd->args[index]);
}

static const char*const* gl_param_string_array(trace_command_t* cmd, size_t index) {
    return trc_get_str(&cmd->args[index]);
}

static const void* gl_param_data(trace_command_t* cmd, size_t index) {
    return *trc_get_data(&cmd->args[index]);
}

static size_t gl_param_data_size(trace_command_t* cmd, size_t index) {
    return *trc_get_data_sizes(&cmd->args[index]);
}

static uint64_t gl_param_pointer(trace_command_t* cmd, size_t index) {
    return *trc_get_ptr(&cmd->args[index]);
}

static GLDEBUGPROC gl_param_GLDEBUGPROC(trace_command_t* cmd, size_t index) {
    return NULL; //TODO
}

static GLsizei gl_param_GLsizei(trace_command_t* cmd, size_t index) {
    return *trc_get_int(&cmd->args[index]);
}

static GLint64EXT gl_param_GLint64EXT(trace_command_t* cmd, size_t index) {
    return *trc_get_int(&cmd->args[index]);
}

static GLshort gl_param_GLshort(trace_command_t* cmd, size_t index) {
    return *trc_get_int(&cmd->args[index]);
}

static int64_t gl_param_int64_t(trace_command_t* cmd, size_t index) {
    return *trc_get_int(&cmd->args[index]);
}

static GLubyte gl_param_GLubyte(trace_command_t* cmd, size_t index) {
    return *trc_get_uint(&cmd->args[index]);
}

static GLDEBUGPROCARB gl_param_GLDEBUGPROCARB(trace_command_t* cmd, size_t index) {
    return NULL; //TODO
}

static GLboolean gl_param_GLboolean(trace_command_t* cmd, size_t index) {
    return *trc_get_bool(&cmd->args[index]);
}

static Bool gl_param_Bool(trace_command_t* cmd, size_t index) {
    return *trc_get_bool(&cmd->args[index]);
}

static GLbitfield gl_param_GLbitfield(trace_command_t* cmd, size_t index) {
    return *trc_get_uint(&cmd->args[index]);
}

static GLuint gl_param_GLuint(trace_command_t* cmd, size_t index) {
    return *trc_get_uint(&cmd->args[index]);
}

static GLint64 gl_param_GLint64(trace_command_t* cmd, size_t index) {
    return *trc_get_int(&cmd->args[index]);
}

static int gl_param_int(trace_command_t* cmd, size_t index) {
    return *trc_get_int(&cmd->args[index]);
}

static uint64_t gl_param_GLeglImageOES(trace_command_t* cmd, size_t index) {
    return *trc_get_ptr(&cmd->args[index]);
}

static GLfixed gl_param_GLfixed(trace_command_t* cmd, size_t index) {
    return *trc_get_double(&cmd->args[index]) * 65546.0f;
}

static GLclampf gl_param_GLclampf(trace_command_t* cmd, size_t index) {
    return *trc_get_double(&cmd->args[index]);
}

static float gl_param_float(trace_command_t* cmd, size_t index) {
    return *trc_get_double(&cmd->args[index]);
}

static GLhalfNV gl_param_GLhalfNV(trace_command_t* cmd, size_t index) { //TODO
    return *trc_get_uint(&cmd->args[index]);
}

static int64_t gl_param_GLintptr(trace_command_t* cmd, size_t index) {
    return *trc_get_int(&cmd->args[index]);
}

static GLushort gl_param_GLushort(trace_command_t* cmd, size_t index) {
    return *trc_get_uint(&cmd->args[index]);
}

static GLenum gl_param_GLenum(trace_command_t* cmd, size_t index) {
    return *trc_get_uint(&cmd->args[index]);
}

static GLuint gl_param_unsigned_int(trace_command_t* cmd, size_t index) {
    return *trc_get_uint(&cmd->args[index]);
}

static GLfloat gl_param_GLfloat(trace_command_t* cmd, size_t index) {
    return *trc_get_double(&cmd->args[index]);
}

static GLuint64 gl_param_GLuint64(trace_command_t* cmd, size_t index) {
    return *trc_get_uint(&cmd->args[index]);
}

static GLdouble gl_param_GLdouble(trace_command_t* cmd, size_t index) {
    return *trc_get_double(&cmd->args[index]);
}

static GLhandleARB gl_param_GLhandleARB(trace_command_t* cmd, size_t index) {
    return *trc_get_uint(&cmd->args[index]);
}

static int64_t gl_param_GLintptrARB(trace_command_t* cmd, size_t index) {
    return *trc_get_int(&cmd->args[index]);
}

static int64_t gl_param_GLsizeiptr(trace_command_t* cmd, size_t index) {
    return *trc_get_int(&cmd->args[index]);
}

static GLint gl_param_GLint(trace_command_t* cmd, size_t index) {
    return *trc_get_int(&cmd->args[index]);
}

static GLclampx gl_param_GLclampx(trace_command_t* cmd, size_t index) {
    return *trc_get_int(&cmd->args[index]);
}

static GLsizeiptrARB gl_param_GLsizeiptrARB(trace_command_t* cmd, size_t index) {
    return *trc_get_ptr(&cmd->args[index]);
}

static GLuint64EXT gl_param_GLuint64EXT(trace_command_t* cmd, size_t index) {
    return *trc_get_uint(&cmd->args[index]);
}

static uint64_t gl_param_GLvdpauSurfaceNV(trace_command_t* cmd, size_t index) {
    return *trc_get_ptr(&cmd->args[index]);
}

static GLbyte gl_param_GLbyte(trace_command_t* cmd, size_t index) {
    return *trc_get_int(&cmd->args[index]);
}

static GLclampd gl_param_GLclampd(trace_command_t* cmd, size_t index) {
    return *trc_get_double(&cmd->args[index]);
}

static GLDEBUGPROCKHR gl_param_GLDEBUGPROCKHR(trace_command_t* cmd, size_t index) {
    return NULL; //TODO
}

static GLDEBUGPROCAMD gl_param_GLDEBUGPROCAMD(trace_command_t* cmd, size_t index) {
    return NULL; //TODO
}

static GLXPixmap gl_param_GLXPixmap(trace_command_t* cmd, size_t index) {
    return *trc_get_uint(&cmd->args[index]);
}

static GLXWindow gl_param_GLXWindow(trace_command_t* cmd, size_t index) {
    return *trc_get_uint(&cmd->args[index]);
}

static GLXPbuffer gl_param_GLXPbuffer(trace_command_t* cmd, size_t index) {
    return *trc_get_uint(&cmd->args[index]);
}

static GLXDrawable gl_param_GLXDrawable(trace_command_t* cmd, size_t index) {
    return *trc_get_uint(&cmd->args[index]);
}

static GLXVideoDeviceNV gl_param_GLXVideoDeviceNV(trace_command_t* cmd, size_t index) {
    return *trc_get_uint(&cmd->args[index]);
}

static Pixmap gl_param_Pixmap(trace_command_t* cmd, size_t index) {
    return *trc_get_uint(&cmd->args[index]);
}

static Window gl_param_Window(trace_command_t* cmd, size_t index) {
    return *trc_get_uint(&cmd->args[index]);
}

static Font gl_param_Font(trace_command_t* cmd, size_t index) {
    return *trc_get_uint(&cmd->args[index]);
}

static Colormap gl_param_Colormap(trace_command_t* cmd, size_t index) {
    return *trc_get_uint(&cmd->args[index]);
}

static GLXContextID gl_param_GLXContextID(trace_command_t* cmd, size_t index) {
    return *trc_get_uint(&cmd->args[index]);
}

static uint64_t gl_param_GLXFBConfig(trace_command_t* cmd, size_t index) {
    return *trc_get_ptr(&cmd->args[index]);
}

static GLXVideoCaptureDeviceNV gl_param_GLXVideoCaptureDeviceNV(trace_command_t* cmd, size_t index) {
    return *trc_get_uint(&cmd->args[index]);
}

static uint64_t gl_param_GLXFBConfigSGIX(trace_command_t* cmd, size_t index) {
    return *trc_get_ptr(&cmd->args[index]);
}

static GLXPbufferSGIX gl_param_GLXPbufferSGIX(trace_command_t* cmd, size_t index) {
    return *trc_get_uint(&cmd->args[index]);
}

static GLXVideoSourceSGIX gl_param_GLXVideoSourceSGIX(trace_command_t* cmd, size_t index) {
    return *trc_get_uint(&cmd->args[index]);
}

static uint64_t gl_param_GLXContext(trace_command_t* cmd, size_t index) {
    return *trc_get_ptr(&cmd->args[index]);
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
    output.write('''
static const trc_gl_%s_rev_t* get_%s(trace_t* trace, uint64_t fake) {
    return trc_get_obj(trace, %s, fake);
}

static void set_%s(trace_t* trace, const trc_gl_%s_rev_t* rev) {
    trc_obj_set_rev(rev->head.obj, rev);
}

static uint64_t trc_get_real_%s(trace_t* trace, uint64_t fake) {
    const trc_gl_%s_rev_t* rev = get_%s(trace, fake);
    return rev ? rev->real : 0;
}
''' % (n, n, t, n, n, n, n, n))
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
        if print_func: print '\n%s: //%s' % (current_name, ', '.join(["%s p_%s"%(p.type_, p.name) for p in f.params]))
        current = ""
    elif line == '' and current_name!='':
        nontrivial[current_name] = current
        current_name = ''
        current= ''
        if print_func: print line
    elif current_name=='':
        output.write(line + '\n')
        if print_func: print line
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
    
    if not name.startswith("glX") and not name.startswith("wip15"):
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
        output.write('    trace_value_t* arg_%s = &cmd->args[%d];\n' % (param.name, i))
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
