#!/usr/bin/env python
# -*- coding: utf-8 -*-
# TODO: Nontrivial functions (very important)
# TODO: Return values (very important)
# TODO: Function pointers (very important)
# TODO: Integer arrays
import glxml

gl = glxml.GL(False)

output = open("../src/replay_gl.c", "w")

output.write("""#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <stdlib.h>
#include <time.h>
#include "replay.h"
#include "libtrace.h"
#include "libinspect.h"

#define F(name) (((replay_gl_funcs_t*)ctx->_replay_gl)->real_##name)

typedef void (*_func)();

""")

output.write(gl.typedecls)

output.write("""
static GLuint* gl_param_GLuint_array(trace_value_t* val) {
    return NULL; //TODO
}

static const char** gl_param_string_array(trace_value_t* val) {
    return (const char**)val->str;
}

static const char* gl_param_string(trace_value_t* val) {
    return *val->str;
}

static uint64_t gl_param_pointer(trace_value_t* val) {
    return *val->ptr;
}

static GLDEBUGPROC gl_param_GLDEBUGPROC(trace_value_t* val) {
    return NULL; //TODO
}

static GLsizei gl_param_GLsizei(trace_value_t* val) {
    return *val->i64;
}

static GLint64EXT gl_param_GLint64EXT(trace_value_t* val) {
    return *val->i64;
}

static GLshort gl_param_GLshort(trace_value_t* val) {
    return *val->i64;
}

static int64_t gl_param_int64_t(trace_value_t* val) {
    return *val->i64;
}

static GLubyte gl_param_GLubyte(trace_value_t* val) {
    return *val->u64;
}

static GLDEBUGPROCARB gl_param_GLDEBUGPROCARB(trace_value_t* val) {
    return NULL; //TODO
}

static GLboolean gl_param_GLboolean(trace_value_t* val) {
    return *val->bl;
}

static Bool gl_param_Bool(trace_value_t* val) {
    return *val->bl;
}

static GLbitfield gl_param_GLbitfield(trace_value_t* val) {
    return *val->bitfield;
}

static uint64_t gl_param_GLsync(trace_value_t* val) {
    return *val->ptr;
}

static GLuint gl_param_GLuint(trace_value_t* val) {
    return *val->u64;
}

static GLint64 gl_param_GLint64(trace_value_t* val) {
    return *val->i64;
}

static int gl_param_int(trace_value_t* val) {
    return *val->i64;
}

static uint64_t gl_param_GLeglImageOES(trace_value_t* val) {
    return *val->ptr;
}

static GLfixed gl_param_GLfixed(trace_value_t* val) {
    return (*val->dbl) * 65546.0f;
}

static GLclampf gl_param_GLclampf(trace_value_t* val) {
    return *val->dbl;
}

static float gl_param_float(trace_value_t* val) {
    return *val->dbl;
}

static GLhalfNV gl_param_GLhalfNV(trace_value_t* val) { //TODO
    return *val->u64;
}

static uint64_t gl_param_GLintptr(trace_value_t* val) {
    return *val->ptr;
}

static GLushort gl_param_GLushort(trace_value_t* val) {
    return *val->u64;
}

static GLenum gl_param_GLenum(trace_value_t* val) {
    return *val->u64;
}

static GLuint gl_param_unsigned_int(trace_value_t* val) {
    return *val->u64;
}

static GLfloat gl_param_GLfloat(trace_value_t* val) {
    return *val->dbl;
}

static GLuint64 gl_param_GLuint64(trace_value_t* val) {
    return *val->u64;
}

static GLdouble gl_param_GLdouble(trace_value_t* val) {
    return *val->dbl;
}

static GLhandleARB gl_param_GLhandleARB(trace_value_t* val) {
    return *val->u64;
}

static uint64_t gl_param_GLintptrARB(trace_value_t* val) {
    return *val->ptr;
}

static uint64_t gl_param_GLsizeiptr(trace_value_t* val)
{
    return *val->ptr;
}

static GLint gl_param_GLint(trace_value_t* val)
{
    return *val->i64;
}

static GLclampx gl_param_GLclampx(trace_value_t* val) {
    return *val->i64;
}

static GLsizeiptrARB gl_param_GLsizeiptrARB(trace_value_t* val) {
    return *val->ptr;
}

static GLuint64EXT gl_param_GLuint64EXT(trace_value_t* val) {
    return *val->u64;
}

static uint64_t gl_param_GLvdpauSurfaceNV(trace_value_t* val) {
    return *val->ptr;
}

static GLbyte gl_param_GLbyte(trace_value_t* val) {
    return *val->i64;
}

static GLclampd gl_param_GLclampd(trace_value_t* val) {
    return *val->dbl;
}

static GLDEBUGPROCKHR gl_param_GLDEBUGPROCKHR(trace_value_t* val) {
    return NULL; //TODO
}

static GLDEBUGPROCAMD gl_param_GLDEBUGPROCAMD(trace_value_t* val) {
    return NULL; //TODO
}

static GLXPixmap gl_param_GLXPixmap(trace_value_t* val) {
    return *val->u64;
}

static GLXWindow gl_param_GLXWindow(trace_value_t* val) {
    return *val->u64;
}

static GLXPbuffer gl_param_GLXPbuffer(trace_value_t* val) {
    return *val->u64;
}

static GLXDrawable gl_param_GLXDrawable(trace_value_t* val) {
    return *val->u64;
}

static GLXVideoDeviceNV gl_param_GLXVideoDeviceNV(trace_value_t* val) {
    return *val->u64;
}

static Pixmap gl_param_Pixmap(trace_value_t* val) {
    return *val->u64;
}

static Window gl_param_Window(trace_value_t* val) {
    return *val->u64;
}

static Font gl_param_Font(trace_value_t* val) {
    return *val->u64;
}

static Colormap gl_param_Colormap(trace_value_t* val) {
    return *val->u64;
}

static GLXContextID gl_param_GLXContextID(trace_value_t* val) {
    return *val->u64;
}

static uint64_t gl_param_GLXFBConfig(trace_value_t* val) {
    return *val->ptr;
}

static GLXVideoCaptureDeviceNV gl_param_GLXVideoCaptureDeviceNV(trace_value_t* val) {
    return *val->u64;
}

static uint64_t gl_param_GLXFBConfigSGIX(trace_value_t* val) {
    return *val->ptr;
}

static GLXPbufferSGIX gl_param_GLXPbufferSGIX(trace_value_t* val) {
    return *val->u64;
}

static GLXVideoSourceSGIX gl_param_GLXVideoSourceSGIX(trace_value_t* val) {
    return *val->u64;
}

static GLXContext gl_param_GLXContext(trace_value_t* val) {
    return (GLXContext)*val->ptr;
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

static void debug_callback(GLenum source,
                           GLenum type,
                           GLuint id,
                           GLenum severity,
                           GLsizei length,
                           const GLchar* message,
                           const void* user_param) {
    const char* source_str = "unknown";
    const char* type_str = "unknown";
    
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

static void replay_begin_cmd(replay_context_t* ctx, const char* name, inspect_command_t* cmd) {
    if (F(glDebugMessageCallback)) {
        F(glEnable)(GL_DEBUG_OUTPUT);
        F(glEnable)(GL_DEBUG_OUTPUT_SYNCHRONOUS);
        F(glDebugMessageCallback)(debug_callback, cmd);
    } else if (F(glDebugMessageCallbackARB)) {
        F(glEnable)(GL_DEBUG_OUTPUT_SYNCHRONOUS);
        F(glDebugMessageCallbackARB)(debug_callback, cmd);
    }
    
    begin_time = get_time();
    if (ctx->_current_context) F(glGetError)();
}

static void replay_end_cmd(replay_context_t* ctx, const char* name, inspect_command_t* cmd) {
    GLenum error = GL_NO_ERROR;
    
    if (ctx->_current_context) {
        error = F(glGetError)();
        F(glFlush)();
    }
    
    uint64_t end_time = get_time();
    
    cmd->cpu_duration = end_time - begin_time;
    
    begin_time = get_time();
    if (ctx->_current_context) F(glFinish)();
    end_time = get_time();
    
    cmd->gpu_duration = end_time - begin_time;
    
    const char *gl_error = "Unknown";
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
}

""")

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
    
    if name in nontrivial:
        output.write(nontrivial[name])
        output.write("replay_end_cmd(ctx, \"%s\", inspect_command);\n" % (name))
        output.write("}\n\n")
        continue
    
    function = gl.functions[name]
    
    params = []
    
    i = 0
    for param in function.params:
        arg = "command->args%s->val" % ("->next"*i)
        
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

output.write("""void init_replay_gl(replay_context_t* ctx) {
    replay_gl_funcs_t* funcs = malloc(sizeof(replay_gl_funcs_t));
    ctx->_replay_gl = funcs;
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
