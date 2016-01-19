#!/usr/bin/env python
# -*- coding: utf-8 -*-
import glxml

dont_implement = ["glUnmapBuffer", "glLinkProgram"]
fb_commands = ["glDrawArrays", "glDrawElements", "glMultiDrawArrays", #TODO: Update this
               "glMultiDrawElements", "glDrawRangeElements",
               "glClear", "glXSwapBuffers"]

gl = glxml.GL(False)

nameToID = {}
groupToID = {}
nextID = 0

for name in gl.functions:
    nameToID[name] = nextID
    nextID += 1

nextID = 0
for name in gl.groups:
    groupToID[name] = nextID
    nextID += 1

output = open("../src/gl.c", "w")

output.write("""#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <dlfcn.h>
#include <stdio.h>
#include <endian.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <inttypes.h>

#include "shared/uint.h"

typedef void (*func_t)();

#define _STR(...) #__VA_ARGS__
#define STR(...) _STR(__VA_ARGS__)
#define F(name) ((name##_t)get_func((func_t*)&gl_##name, STR(name)))
#define FUNCTION_COUNT %d
#define GROUP_COUNT %d
""" % (len(gl.functions), len(gl.groups)))

for group in gl.groups.keys():
    output.write("#define GROUP_%s %d\n" % (group, groupToID[group]))

for func in gl.functions.keys():
    output.write("#define FUNC_%s %d\n" % (func, nameToID[func]))

for k, v, in gl.enumValues.iteritems():
    output.write("#define %s %s\n" % (k, v))

output.write(gl.typedecls)

for name in gl.functions:
    function = gl.functions[name]
    
    params = []
    
    for param in function.params:
        if param.type_[-1] == "]":
            params.append("%s[%d]" % (param.type_.split("[")[0], eval(param.type_.split("[")[1][:-1])))
        else:
            params.append("%s" % (param.type_))
    
    output.write("typedef %s (*%s_t)(%s);\n" % (function.returnType, name, ", ".join(params)))
    output.write("static %s_t gl_%s;\n" % (name, name))

output.write("""
static func_t get_func(func_t* f, const char* name) {
    if (*f)
        return *f;
    else
        return *f = gl_glXGetProcAddress((const GLubyte*)name);
}

static void reset_gl() {
""")

for name in gl.functions:
    if not name.startswith("glX"):
        output.write("    gl_%s=NULL;\n" % (name))

output.write("}\n\n")

for name in gl.functions:
    output.write("static void func_decl_%s();\n" % (name))

output.write(open("gl.c", "r").read())

nontrivial_str = open("nontrivial_func_trace_impls.txt").read()
nontrivial = {}

current_name = ""
current = ["", None]
for line in nontrivial_str.split("\n"):
    if ":" in line and line.startswith("gl"):
        if len(current_name):
            nontrivial[current_name] = current
        
        current_name = line.split(":")[0]
        current = ["", line.split(":")[1].split(",")]
    else:
        current[0] += line + "\n"

if len(current_name):
    nontrivial[current_name] = current

for name in gl.functions:
    output.write("static bool did_%s_func_decl = false;\nstatic void func_decl_%s() {" % (name, name))
    output.write("if (!did_%s_func_decl) {did_%s_func_decl=true;\n" % (name, name))
    output.write("    gl_start_func_decl(%d, \"%s\");\n" % (nameToID[name], name))
    
    if name in nontrivial:
        output.write("    gl_write_type(BASE_TYPE_%s, false, false);\n" % nontrivial[name][1][0].lstrip().rstrip().upper())
        
        output.write("    gl_start_func_decl_args(%d);\n" % (len(nontrivial[name][1])-1))
        
        for arg in nontrivial[name][1][1:]:
            parts = [s.lstrip().rstrip() for s in arg.split("+")]
            output.write("gl_write_type(BASE_TYPE_%s, %s, %s);\n" % (parts[0].upper(),
                                                                     "true" if "group" in parts else "false",
                                                                     "true" if "array" in parts else "false"))
            if arg == "*":
                output.write("gl_write_type(BASE_TYPE_POINTER, false, false);\n")
                    
    elif name.startswith("glXGetProcAddress"):
        output.write("""gl_write_type(BASE_TYPE_POINTER, false, false);
gl_start_func_decl_args(1);
gl_write_type(BASE_TYPE_STRING, false, false);
""")
    else:
        function = gl.functions[name]
        
        if "*" in function.returnType:
            ret_type = "pointer"
        elif function.returnType.replace(" ", "") == "unsignedint":
            ret_type = "unsigned_int"
        else:
            ret_type = function.returnType
        
        output.write("    gl_write_type(BASE_TYPE_%s, false, false);\n" % ret_type.upper())
        
        output.write("    gl_start_func_decl_args(%d);\n" % len(function.params))
        
        for param in function.params:
            if param.type_[-1] == "]":
                output.write("gl_write_type(BASE_TYPE_%s, false, true);" % param.type_.split("[")[0].upper())
            elif param.type_.replace(" ", "") == "constGLchar*const*":
                output.write("gl_write_type(BASE_TYPE_POINTER, false, false);")
            elif param.type_.replace(" ", "") == "constGLchar**":
                output.write("gl_write_type(BASE_TYPE_POINTER, false, false);")
            elif param.type_.replace(" ", "") == "constGLcharARB**":
                output.write("gl_write_type(BASE_TYPE_POINTER, false, false);")
            elif param.type_.replace(" ", "") == "unsignedint":
                if param.group != None:
                    output.write("gl_write_type(BASE_TYPE_UNSIGNED_INT, true, false);")
                else:
                    output.write("gl_write_type(BASE_TYPE_UNSIGNED_INT, false, false);")
            elif param.type_.replace(" ", "") == "unsignedlong":
                if param.group != None:
                    output.write("gl_write_type(BASE_TYPE_UNSIGNED_INT, true, false);")
                else:
                    output.write("gl_write_type(BASE_TYPE_UNSIGNED_INT, false, false);")
            elif "*" in param.type_:
                if "GLchar" in param.type_:
                    output.write("gl_write_type(BASE_TYPE_STRING, false, false);")
                else:
                    output.write("gl_write_type(BASE_TYPE_POINTER, false, false);")
            else:
                type = param.type_.replace("const", "").lstrip().rstrip().upper()
                if param.group != None:
                    output.write("gl_write_type(BASE_TYPE_%s, true, false);" % (type))
                else:
                    output.write("gl_write_type(BASE_TYPE_%s, false, false);" % (type))
    
    output.write("    gl_end_func_decl_args();\n    gl_end_func_decl();}}\n")

for name in gl.functions:
    if name in ["glXGetProcAddress", "glXGetProcAddressARB"] or name in dont_implement:
        continue
    
    function = gl.functions[name]
    
    params = []
    
    for param in function.params:
        if param.type_[-1] == "]":
            params.append("%s %s[%d]" % (param.type_.split("[")[0], param.name, eval(param.type_.split("[")[1][:-1])))
        else:
            params.append("%s %s" % (param.type_, param.name))
    
    output.write("%s %s(%s)" % (function.returnType, name, ",".join(params)))
    output.write("""{func_decl_%s();limits_t* old_limits = current_limits;""" % name)
    
    output.write("gl_start_call(%d);" % (nameToID[name]))
    
    if name in nontrivial:
        if not name.startswith("glX"):
            output.write("if(!gl_%s) gl_%s=(%s_t)gl_glXGetProcAddress((const GLubyte*)\"%s\");" %\
                         (name, name, name, name))
        output.write(nontrivial[name][0])
    else:
        for param in function.params:
            if param.type_[-1] == "]":
                output.write("gl_param_%s_array(%s, %s);" % (param.type_.split("[")[0], eval(param.type_.split("[")[1][:-1]), param.name))
            elif param.type_.replace(" ", "") == "constGLchar*const*":
                output.write("gl_param_pointer((void *)%s);" % (param.name))
            elif param.type_.replace(" ", "") == "constGLchar**":
                output.write("gl_param_pointer((void *)%s);" % (param.name))
            elif param.type_.replace(" ", "") == "constGLcharARB**":
                output.write("gl_param_pointer((void *)%s);" % (param.name))
            elif param.type_.replace(" ", "") == "unsignedint":
                output.write("gl_param_unsigned_int(%s);" % (param.name))
                if param.group != None:
                    output.write("gl_write_group(%d);") % (groupToID[param.group])
            elif param.type_.replace(" ", "") == "unsignedlong":
                output.write("gl_param_unsigned_int(%s);" % (param.name))
                if param.group != None:
                    output.write("gl_write_group(%d);") % (groupToID[param.group])
            elif "*" in param.type_:
                if "GLchar" in param.type_:
                    output.write("gl_param_string(%s);" % (param.name))
                else:
                    output.write("gl_param_pointer((void*)%s);" % (param.name))
            else:
                output.write("gl_param_%s(%s);" % (param.type_.replace("const", "").lstrip().rstrip(), param.name))
                if param.group != None:
                    output.write("gl_write_group(%d);" % (groupToID[param.group]))
        
        if not name.startswith("glX"):
            output.write("if(!gl_%s) gl_%s=(%s_t)gl_glXGetProcAddress((const GLubyte*)\"%s\");" %\
                         (name, name, name, name))
        
        if function.returnType != "void":
            output.write("%s result=gl_%s(%s);" % (function.returnType, name, ",".join([param.name for param in function.params])))
        else:
            output.write("gl_%s(%s);" % (name, ", ".join([param.name for param in function.params])))
        
        if function.returnType != "void":
            if "*" in function.returnType:
                output.write("gl_result_pointer((void *)result);")
            elif function.returnType.replace(" ", "") == "unsignedint":
                output.write("gl_result_unsigned_int(result);")
            else:
                output.write("gl_result_%s(result);" % (function.returnType))
    
    output.write("gl_end_call();")
    
    output.write("if((old_limits!=current_limits)&&current_limits)glSetContextCapsWIP15();\n")
    
    if name in fb_commands:
        output.write("if (test_mode && current_limits) test_fb(\"%s\");\n" % (name))
    
    output.write("update_drawable_size();")
    
    if function.returnType != "void":
        output.write("return result;")
    
    output.write("}\n")

names = ["glXGetProcAddress", "glXGetProcAddressARB"]
for n in names:
    output.write("""
    __GLXextFuncPtr %s(const GLubyte *name) {
        func_decl_%s();
        gl_start_call(%d);
        gl_param_string((const char*)name);
        
        void (*result)() = NULL;
    """ % (n, n, nameToID[n]))
    
    for name in gl.functions:
        if name in names:
            output.write("if(strcmp((const char*)name, \"%s\") == 0) {result=(void (*)())dlsym(lib_gl, \"%s\");};" % (name, name))
        else:
            output.write("if(strcmp((const char*)name, \"%s\") == 0) {result=(void (*)())%s;}\n" % (name, name))
    
    output.write("""
        if (result==NULL)
            result = gl_%s(name);

        gl_result_pointer((void*)result);
        gl_end_call();
        return result;
    }
    
    """ % (n))

output.write("""void __attribute__ ((constructor)) gl_init() {
    char *output = getenv("WIP15_OUTPUT");
    trace_file = fopen(output == NULL ? "output.trace" : output, "wb");
    
    if (!trace_file) {
        fprintf(stderr, "Unable to open/create %s.", output==NULL?"output.trace":output);
        exit(1);
    }
    
    test_mode = getenv("WIP15_TEST") != NULL;
    
    fwrite("WIP15", 5, 1, trace_file);
    
    if (BYTE_ORDER == LITTLE_ENDIAN)
        fwrite("_", 1, 1, trace_file);
    else
        fwrite("-", 1, 1, trace_file);
    
    fwrite("0.0a            ", 16, 1, trace_file);
    
    gl_write_uint32(FUNCTION_COUNT);
    gl_write_uint32(GROUP_COUNT);
    
    lib_gl = actual_dlopen("libGL.so.1", RTLD_NOW|RTLD_LOCAL);
    
    if (!lib_gl) {
        fprintf(stderr, "Unable to open libGL.so");
        fflush(stderr);
    }
    
    handle_limits();
    
    current_limits = NULL;
""")

for name in gl.groups:
    output.write("gl_write_b(OP_DECL_GROUP);gl_write_uint32(%d);gl_write_str(\"%s\");\n" % (groupToID[name], name))

for name in gl.functions:
    if name.startswith("glX"):
        output.write("gl_%s=(%s_t)dlsym(lib_gl, \"%s\");\n" % (name, name, name))
    else:
        output.write("gl_%s=NULL;\n" % (name))

output.write("}")

output.close()
