#!/usr/bin/env python
# -*- coding: utf-8 -*-
from glapi.glapi import *
import glapi.glxml

gl_c = open('../src/libgl.c', 'w')

gl_c.write('''#include <X11/Xlib.h>
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

#include "shared/types.h"

typedef void (*func_t)();

#define GLuintptr size_t

''')

gl = glapi.glxml.GL(False)

gl_c.write(gl.typedecls)

for k, v, in gl.enumValues.iteritems():
    gl_c.write("#define %s %s\n" % (k, v))

for func in funcs:
    params = ', '.join([p.gen_param_code() for p in func.params])
    ret = func.rettype.gen_type_code() if func.rettype else 'void'
    gl_c.write('typedef %s (*%s_t)(%s);\n' % (ret, func.name, params))
    gl_c.write("static %s_t gl_%s;\n" % (func.name, func.name))

gl_c.write('''

void reset_gl_funcs() {''')

for name in [f.name for f in funcs]:
    if not name.startswith('glX'):
        gl_c.write('    gl_%s = NULL;\n' % name)

gl_c.write('''}

#define _STR(...) #__VA_ARGS__
#define STR(...) _STR(__VA_ARGS__)
#define F(name) ((name##_t)get_func((func_t*)&gl_##name, STR(name)))

static func_t get_func(func_t* f, const char* name) {
    if (*f) return *f;
    else return *f = gl_glXGetProcAddress(name);
}
''')

gl_c.write(open("gl_wrapper.c", "r").read())

for func in funcs:
    gl_c.write(func.gen_decl() + '\n\n' + func.gen_wrapper() + '\n\n')

gl_c.write('#define FUNC_COUNT %d\n' % len(funcs))
gl_c.write('#define GROUP_COUNT %d\n' % len(groups))

gl_c.write('''
void __attribute__((constructor)) wip15_gl_init() {
    char *output = getenv("WIP15_OUTPUT");
    trace_file = fopen(output == NULL ? "output.trace" : output, "wb");
    
    if (!trace_file) {
        fprintf(stderr, "Unable to open/create %s.", output==NULL?"output.trace":output);
        exit(1);
    }
        
    char* comp_level = getenv("WIP15_COMPRESSION_LEVEL");
    if (comp_level) {
        char* end;
        int level = strtol(comp_level, &end, 10);
        if (level<0 || level>100 || end==comp_level || *end)
            fprintf(stderr, "Invalid compression level: \\\"%s\\\"\\n", comp_level);
        else
            compression_level = level;
    }
    
    char* comp_meth = getenv("WIP15_COMPESSION_METHOD");
    if (comp_meth && !strcmp(comp_meth, "zlib"))
        compression_method = COMPRESSION_ZLIB;
    else if (comp_meth && !strcmp(comp_meth, "lz4"))
        compression_method = COMPRESSION_LZ4;
    else if (comp_meth && !strcmp(comp_meth, "zstd"))
        compression_method = COMPRESSION_ZSTD;
    
    #ifndef ZSTD_ENABLED
    if (compression_method == COMPRESSION_ZSTD) {
        #ifdef LZ4_ENABLED
        compression_method = COMPRESSION_LZ4;
        printf("Warning: OpenGL wrapper not built with ZStandard support. Using LZ4 reference implementation.\\n");
        #elif defined(ZLIB_ENABLED)
        compression_method = COMPRESSION_ZLIB;
        printf("Warning: OpenGL wrapper not built with ZStandard support. Using Zlib.\\n");
        #else
        compression_method = COMPRESSION_NONE;
        printf("Warning: OpenGL wrapper not built with Zlib, LZ4 or ZStandard support. Disabling compression.\\n");
        #endif
    }
    #endif
    
    #ifndef ZLIB_ENABLED
    if (compression_method == COMPRESSION_ZLIB) {
        #ifdef ZSTD_ENABLED
        compression_method = COMPRESSION_ZSTD;
        printf("Warning: OpenGL wrapper not built with Zlib support. Using Zstandard.\\n");
        #elif defined(LZ4_ENABLED)
        compression_method = COMPRESSION_LZ4;
        printf("Warning: OpenGL wrapper not built with Zlib support. Using LZ4 reference implementation.\\n");
        #else
        compression_method = COMPRESSION_NONE;
        printf("Warning: OpenGL wrapper not built with Zlib, LZ4 or ZStandard support. Disabling compression.\\n");
        #endif
    }
    #endif
    
    #ifndef LZ4_ENABLED
    if (compression_method == COMPRESSION_LZ4) {
        #ifdef ZSTD_ENABLED
        compression_method = COMPRESSION_ZSTD;
        printf("Warning: OpenGL wrapper not built with LZ4 support. Using Zstandard.\\n");
        #elif defined(ZLIB_ENABLED)
        compression_method = COMPRESSION_ZLIB;
        printf("Warning: OpenGL wrapper not built with LZ4 support. Using Zlib.\\n");
        #else
        compression_method = COMPRESSION_NONE;
        printf("Warning: OpenGL wrapper not built with Zlib, LZ4 or Zstandard support. Disabling compression.\\n");
        #endif
    }
    #endif
    
    fwrite("WIP15", 5, 1, trace_file);
    
    if (BYTE_ORDER == LITTLE_ENDIAN)
        fwrite("_", 1, 1, trace_file);
    else
        fwrite("-", 1, 1, trace_file);
    
    fwrite("", 1, 1, trace_file);
    fwrite("", 1, 1, trace_file);
    
    gl_write_uint32(FUNC_COUNT);
    gl_write_uint32(GROUP_COUNT);
    
    lib_gl = actual_dlopen("libGL.so.1", RTLD_NOW|RTLD_LOCAL);
    
    if (!lib_gl) {
        fprintf(stderr, "Unable to open libGL.so");
        fflush(stderr);
    }
    
    handle_limits();
    
    current_limits = NULL;
    
    gl_glXGetProcAddress = dlsym(lib_gl, "glXGetProcAddress");
    
''')

for name, group in group_dict.iteritems():
    gl_c.write('    gl_write_b(OP_DECL_GROUP);\n')
    gl_c.write('    gl_write_b(0);\n') # Currently only enum groups
    gl_c.write('    gl_write_uint32(%d);\n' % group.group_id)
    gl_c.write('    gl_write_str("%s");\n' % name)

for name in [f.name for f in funcs]:
    if name.startswith('glX'):
        gl_c.write("    gl_%s=(%s_t)gl_glXGetProcAddress(\"%s\");\n" % (name, name, name))
    else:
        gl_c.write('    gl_%s = NULL;\n' % name)

gl_c.write("}\n")
