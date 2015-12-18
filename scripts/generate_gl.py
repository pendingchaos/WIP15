#!/usr/bin/env python
# -*- coding: utf-8 -*-
import glxml

dont_implement = ["glUnmapBuffer", "glLinkProgram", "glDrawArrays",
                  "glMultiDrawArrays", "glDrawElements", "glDrawRangeElements"]
fb_commands = ["glDrawArrays", "glDrawElements", "glMultiDrawArrays",
               "glMultiDrawElements", "glDrawRangeElements", "glEnd",
               "glClear", "glRectd", "glRectd", "glRecti", "glRects",
               "glRectdv", "glRectfv", "glRectiv", "glRectsv",
               "glCallList", "glCallLists", "glXSwapBuffers", "glEndList"]

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

#define _STR(...) #__VA_ARGS__
#define STR(...) _STR(__VA_ARGS__)
#define F(name) ((!gl_##name)?(gl_##name=(name##_t)gl_glXGetProcAddress((const GLubyte*)STR(name))):gl_##name)
""")

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
    output.write("%s_t gl_%s;\n" % (name, name))

output.write(open("gl.c", "r").read())

nontrivial_str = open("nontrivial_func_trace_impls.txt").read()
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
    output.write("""{limits_t* old_limits = current_limits;""")
    
    output.write("gl_start(%d);" % (nameToID[name]))
    
    if name in nontrivial:
        if not name.startswith("glX"):
            output.write("if(!gl_%s) gl_%s=(%s_t)gl_glXGetProcAddress((const GLubyte*)\"%s\");" %\
                         (name, name, name, name))
        output.write(nontrivial[name])
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
                if param.group != None:
                    group = "%d" % (groupToID[param.group])
                else:
                    group = "-1";
                
                output.write("gl_param_unsigned_int(%s, %s);" % (param.name, group))
            elif param.type_.replace(" ", "") == "unsignedlong":
                if param.group != None:
                    group = "%d" % (groupToID[param.group])
                else:
                    group = "-1";
                
                output.write("gl_param_unsigned_int(%s, %s);" % (param.name, group))
            elif "*" in param.type_:
                if "GLchar" in param.type_:
                    output.write("gl_param_string(%s);" % (param.name))
                else:
                    output.write("gl_param_pointer((void*)%s);" % (param.name))
            else:
                if param.group != None:
                    group = "%d" % (groupToID[param.group])
                else:
                    group = "-1";
                
                output.write("gl_param_%s(%s,%s);" % (param.type_.replace("const", "").lstrip().rstrip(), param.name, group))
        
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
    
    output.write("gl_end();")
    
    output.write("if((old_limits!=current_limits)&&current_limits)glSetContextCapsWIP15();\n")
    
    if name in fb_commands:
        output.write("if (test_mode && current_limits) test_fb(\"%s\");\n" % (name))
    
    if function.returnType != "void":
        output.write("return result;")
    
    output.write("}\n")

names = ["glXGetProcAddress", "glXGetProcAddressARB"]
for n in names:
    output.write("""
    __GLXextFuncPtr %s(const GLubyte *name) {
        gl_start(%d);
        gl_param_string(name);
        
        void (*result)() = NULL;
    """ % (n, nameToID[n]))
    
    for name in gl.functions:
        if name in names:
            output.write("if(strcmp(name, \"%s\") == 0) {result=(void (*)())dlsym(lib_gl, \"%s\");};" % (name, name))
        else:
            output.write("if(strcmp(name, \"%s\") == 0) {result=(void (*)())%s;}\n" % (name, name))
    
    output.write("""
        if (result==NULL)
            result = gl_%s(name);

        gl_result_pointer((void*)result);
        gl_end();
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
    
    uint8_t v = BYTE_ORDER == LITTLE_ENDIAN;
    fwrite(&v, 1, 1, trace_file);
    
    lib_gl = actual_dlopen("libGL.so.1", RTLD_NOW|RTLD_LOCAL);
    
    if (!lib_gl) {
        fprintf(stderr, "Unable to open libGL.so");
        fflush(stderr);
    }
    
    handle_limits();
    
    current_limits = NULL;
""")

for name in gl.functions:
    if name.startswith("glX"):
        output.write("gl_%s=(%s_t)dlsym(lib_gl, \"%s\");\n" % (name, name, name))
    else:
        output.write("gl_%s=NULL;\n" % (name))

output.write("uint32_t count = htole32(%d);\n" % (len(gl.functions.keys())))
output.write("fwrite(&count, 4, 1, trace_file);\n")

for name in gl.functions:
    output.write("gl_write_str(\"%s\");\n" % (name))

output.write("count = htole32(%d);\n" % (len(gl.groups.keys())))
output.write("fwrite(&count, 4, 1, trace_file);\n")

for name in gl.groups:
    output.write("gl_write_str(\"%s\");\n" % (name))

output.write("}")

output.close()
