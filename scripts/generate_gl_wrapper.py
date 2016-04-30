#!/usr/bin/env python
# -*- coding: utf-8 -*-
funcs = []
func_dict = {}
next_func_id = 0

def indent(s, amount):
    return '\n'.join(['    '*amount+l for l in s.split('\n')])

class Type(object):
    def __init__(self):
        pass
    
    def gen_type_code(self, var_name='', array_count=None):
        return ''
    
    def gen_write_code(self, var_name, array_count=None):
        return ''
    
    def gen_write_type_code(self, array_count=None):
        return ''

class Param(object):
    def __init__(self, dtype, name, array_count=None):
        self.dtype = dtype() if isinstance(dtype, type) else dtype
        self.name = name
        self.array_count = array_count
    
    def gen_param_code(self):
        return self.dtype.gen_type_code(self.name, self.array_count)
    
    def gen_write_code(self):
        return self.dtype.gen_write_code(self.name, self.array_count)
    
    def gen_write_type_code(self):
        return self.dtype.gen_write_type_code(self.array_count)

class Func(object):
    def __init__(self, name, params, rettype=None):
        global next_func_id, funcs
        self.func_id = next_func_id
        next_func_id += 1
        self.name = name
        self.params = params
        self.prologue_code = ''
        self.epilogue_code = ''
        self.extras_code = ''
        if rettype != None: self.rettype = rettype() if isinstance(rettype, type) else rettype
        else: self.rettype = None
        
        if name in func_dict:
            funcs.remove(func_dict[name])
        
        func_dict[name] = self
        funcs.append(self)
    
    def gen_decl(self):
        res = 'static bool did_%s_func_decl = false;\n' % self.name
        res += 'static void func_decl_%s() {\n' % self.name
        res += '    if (!did_%s_func_decl) {\n' % self.name
        res += '        did_%s_func_decl = true;\n' % self.name
        res += '        gl_start_func_decl(%d, "%s");\n' % (self.func_id, self.name)
        
        if self.rettype != None:
            res += indent(self.rettype.gen_write_type_code(), 2) + ';\n'
        else:
            res += '        gl_write_type(BASE_VOID, false, false);\n'
        
        res += '        gl_start_func_decl_args(%d);\n' % len(self.params)
        for param in self.params:
            res += indent(param.gen_write_type_code(), 2) + ';\n'
        res += '        gl_end_func_decl_args();\n'
        
        res += '        gl_end_func_decl();\n'
        res += '    }\n'
        res += '}'
        
        return res
    
    def gen_wrapper(self):
        params = [p.gen_param_code() for p in self.params]
        ret = self.rettype.gen_type_code() if self.rettype else 'void'
        
        res = '%s %s(%s) {\n' % (ret, self.name, ', '.join(params))
        res += '    func_decl_%s();\n' % self.name
        
        res += '    if (!gl_%s)\n' % self.name # TODO: This could change when the context does
        res += '        gl_%s = (%s_t)gl_glXGetProcAddress((const GLubyte*)"%s");\n' % (self.name, self.name, self.name)
        
        res += indent(self.prologue_code, 1) + '\n'
        
        res += '    gl_start_call(%d);\n' % self.func_id
        
        params = ', '.join([param.name for param in self.params])
        if self.rettype != None:
            res += '    %s result = gl_%s(%s);\n' % (self.rettype.gen_type_code(), self.name, params)
        else:
            res += '    gl_%s(%s);\n' % (self.name, params)
        
        for param in self.params:
            res += indent(param.gen_write_code(), 1) + ';\n'
        
        if self.rettype != None:
            res += indent(self.rettype.gen_write_code('result'), 1) + ';\n'
        
        res += indent(self.extras_code, 1) + '\n'
        
        res += '    gl_end_call();\n'
        
        res += indent(self.epilogue_code, 1) + '\n'
        
        if self.rettype != None:
            res += '    return result;\n'
        res += '}'
        
        return res

class _BaseType(Type):
    def gen_type_code(self, var_name='', array_count=None):
        if array_count != None:
            return '%s* %s' % (self.__class__.ctype, var_name)
        else:
            return '%s %s' % (self.__class__.ctype, var_name)
    
    def gen_write_code(self, var_name, array_count=None):
        if array_count != None:
            res = 'size_t count_%d = (%s);\n' % (id(self), str(array_count))
            res += 'gl_write_uint32(count_%d);\n' % id(self)
            res += 'for (size_t i = 0; i < count_%d; i++)\n' % id(self)
            res += '    %s(%s[i]);' % (self.__class__.write_func, var_name)
            return res
        else:
            return '%s(%s)' % (self.__class__.write_func, var_name)
    
    def gen_write_type_code(self, array_count=None):
        array = 'true' if array_count != None else 'false'
        return 'gl_write_type(%s, false, %s)' % (self.__class__.base, array)

class _UInt(_BaseType):
    write_func = 'gl_write_uleb128'
    base = 'BASE_UNSIGNED_INT'

class _Int(_BaseType):
    write_func = 'gl_write_sleb128'
    base = 'BASE_INT'

class _Float(_BaseType):
    write_func = 'gl_write_float'
    base = 'BASE_FLOAT'

class _Double(_BaseType):
    write_func = 'gl_write_double'
    base = 'BASE_DOUBLE'

class _Ptr(_BaseType):
    write_func = 'gl_write_ptr'
    base = 'BASE_PTR'

class _Bool(_BaseType):
    write_func = 'gl_write_bool'
    base = 'BASE_BOOL'

class _FuncPtr(Type):
    def gen_type_code(self, var_name='', array_count=None):
        if array_count != None: return 'func_t* %s' % (var_name)
        else: return 'func_t %s' % var_name
    
    def gen_write_code(self, var_name, array_count=None):
        if array_count != None: return 'gl_write_uint32((%s));' % str(array_count)
        else: return ''
    
    def gen_write_type_code(self, array_count=None):
        array = 'true' if array_count != None else 'false'
        return 'gl_write_type(BASE_FUNC_PTR, false, %s)' % array

def _create_uint(name):
    return 'class t%s(_UInt): ctype = \'%s\'' % (name.replace(' ', ''), name)

def _create_int(name):
    return 'class t%s(_Int): ctype = \'%s\'' % (name.replace(' ', ''), name)

exec _create_uint('uint8_t')
exec _create_int('int8_t')
exec _create_uint('uint16_t')
exec _create_int('int16_t')
exec _create_uint('uint32_t')
exec _create_int('int32_t')
exec _create_uint('uint64_t')
exec _create_int('int64_t')
exec _create_int('GLsizei')
exec _create_int('GLint64EXT')
exec _create_int('GLshort')
exec _create_uint('GLubyte')
exec _create_uint('GLbitfield')
exec _create_uint('GLuint')
exec _create_int('GLint64')
exec _create_int('int')
exec _create_int('GLintptr')
exec _create_uint('GLushort')
exec _create_uint('GLenum')
exec _create_uint('unsigned int')
exec _create_uint('unsigned long')
exec _create_int('GLuint64')
exec _create_uint('GLhandleARB')
exec _create_int('GLintptrARB')
exec _create_int('GLsizeiptr')
exec _create_uint('GLuintptr')
exec _create_int('GLint')
exec _create_int('GLclampx')
exec _create_int('GLsizeiptrARB')
exec _create_int('GLuint64EXT')
exec _create_int('GLvdpauSurfaceNV')
exec _create_int('GLbyte')
exec _create_int('GLclampd')
exec _create_uint('GLXPixmap')
exec _create_uint('GLXWindow')
exec _create_uint('GLXPbuffer')
exec _create_uint('GLXDrawable')
exec _create_uint('GLXVideoDeviceNV')
exec _create_uint('Pixmap')
exec _create_uint('Window')
exec _create_uint('Font')
exec _create_uint('Colormap')
exec _create_uint('GLXContextID')
exec _create_uint('GLXFBConfig')
exec _create_uint('GLXVideoCaptureDeviceNV')
exec _create_uint('GLXFBConfigSGIX')
exec _create_uint('GLXPbufferSGIX')
exec _create_uint('GLXVideoSourceSGIX')
exec _create_uint('GLXContext')
exec _create_int('Status')

class tfloat(_Float): ctype = 'float'
class tGLclampf(_Float): ctype = 'GLclampf'
class tGLfloat(_Float): ctype = 'GLfloat'
class tdouble(_Double): ctype = 'double'
class tGLclampd(_Double): ctype = 'GLclampd'
class tGLdouble(_Double): ctype = 'GLdouble'

class tPointer(_Ptr): ctype = 'void*'
class tMutablePointer(_Ptr): ctype = 'const void*'
class tGLsync(_Ptr): ctype = 'GLsync'
class tGLeglImageOES(_Ptr): ctype = 'GLeglImageOES'
class tGLXFBConfig(_Ptr): ctype = 'GLXFBConfig'
class tGLXFBConfigSGIX(_Ptr): ctype = 'GLXFBConfigSGIX'
class tGLXContext(_Ptr): ctype = 'GLXContext'

class tbool(_Bool): ctype = 'bool'
class tGLboolean(_Bool): ctype = 'GLboolean'
class tBool(_Bool): ctype = 'Bool'

class tGLDEBUGPROC(_FuncPtr): pass
class tGLDEBUGPROCARB(_FuncPtr): pass
class tGLDEBUGPROCKHR(_FuncPtr): pass
class tGLDEBUGPROCAMD(_FuncPtr): pass
class t__GLXextFuncPtr(_FuncPtr): pass

class tData(Type):
    def __init__(self, size_expr):
        Type.__init__(self)
        self.size_expr = size_expr
    
    def gen_type_code(self, var_name='', array_count=None):
        if array_count != None:
            return 'void** %s' % var_name
        else:
            return 'void* %s' % var_name
    
    def gen_write_code(self, var_name, array_count=None):
        if array_count != None:
            res = 'size_t count_%d = (%s);\n' % (id(self), str(array_count))
            res += 'gl_write_uint32(count_%d);\n' % id(self)
            res += 'for (size_t i = 0; i < count_%d; i++)\n' % id(self)
            res += '    gl_write_data((%s), %s[i]);' % (self.size_expr, var_name)
            return res
        else:
            return 'gl_write_data((%s), %s)' % (self.size_expr, var_name)
    
    def gen_write_type_code(self, array_count=None):
        array = 'true' if array_count != None else 'false'
        return 'gl_write_type(BASE_DATA, false, %s)' % (array)

class tGLfixed(Type):
    def gen_type_code(self, var_name='', array_count=None):
        if array_count != None: return 'GLfixed* %s' % var_name
        else: return 'GLfixed %s' % var_name
    
    def gen_write_code(self, var_name, array_count):
        if array_count != None:
            res = 'size_t count_%d = (%s);\n' % (id(self), str(array_count))
            res += 'gl_write_uint32(count_%d);\n' % id(self)
            res += 'for (size_t i = 0; i < count_%d; i++)\n' % id(self)
            res += '    gl_write_float(%s[i] / 65535.0);' % (var_name)
            return res
        else:
            return 'gl_write_float(%s/65535.0)' % var_name
    
    def gen_write_type_code(self, array_count):
        array = 'true' if array_count != None else 'false'
        return 'gl_write_type(BASE_FLOAT, false, %s)' % (array)

class tGLhalfNV(Type): # TODO
    def gen_type_code(self, var_name='', array_count=None):
        if array_count != None: return 'GLhalfNV* %s' % var_name
        else: return 'GLhalfNV %s' % var_name
    
    def gen_write_code(self, var_name, array_count):
        if array_count != None:
            res = 'size_t count_%d = (%s);\n' % (id(self), str(array_count))
            res += 'gl_write_uint32(count_%d);\n' % id(self)
            res += 'for (size_t i = 0; i < count_%d; i++)\n' % id(self)
            res += '    gl_write_float(0.0);'
            return res
        else:
            return 'gl_write_float(0.0)'
    
    def gen_write_type_code(self, array_count):
        array = 'true' if array_count != None else 'false'
        return 'gl_write_type(BASE_FLOAT, false, %s)' % (array)

class tString(Type):
    def gen_type_code(self, var_name='', array_count=None):
        if array_count != None: return 'const char** %s' % var_name
        else: return 'const char* %s' % var_name
    
    def gen_write_code(self, var_name, array_count=None):
        if array_count != None:
            res = 'size_t count_%d = (%s);\n' % (id(self), str(array_count))
            res += 'gl_write_uint32(count_%d);\n' % id(self)
            res += 'for (size_t i = 0; i < count_%d; i++)\n' % id(self)
            res += '    gl_write_str(%s[i]);' % (var_name)
            return res
        else:
            return 'gl_write_str(%s)' % var_name

class tMutableString(tString):
    def gen_type_code(self, var_name='', array_count=None):
        if array_count != None: return 'char** %s' % var_name
        else: return 'char* %s' % var_name

exec open('generated_gl_funcs.py').read()
exec open('gl_funcs.py').read()

gl_c = open('output_gl.c', 'w')

gl_c.write('''#define _GNU_SOURCE
#include <X11/Xlib.h>
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

''')

import glxml
gl_c.write(glxml.GL(False).typedecls)

gl_c.write(open("new_gl.c", "r").read())

for func in funcs:
    params = ', '.join([p.gen_param_code() for p in func.params])
    ret = func.rettype.gen_type_code() if func.rettype else 'void'
    gl_c.write('typedef %s (*%s_t)(%s);\n' % (ret, func.name, params))
    gl_c.write("static %s_t gl_%s;\n" % (func.name, func.name))

for func in funcs:
    gl_c.write(func.gen_decl() + '\n\n' + func.gen_wrapper() + '\n\n')

gl_c.write('#define FUNC_COUNT %d\n' % len(funcs))

gl_c.write('''
void __attribute__ ((constructor)) wip15_gl_init() {
    char *output = getenv("WIP15_OUTPUT");
    trace_file = fopen(output == NULL ? "output.trace" : output, "wb");
    
    if (!trace_file) {
        fprintf(stderr, "Unable to open/create %s.", output==NULL?"output.trace":output);
        exit(1);
    }
    
    test_mode = getenv("WIP15_TEST") != NULL;
    
    char* comp_level = getenv("WIP15_COMPRESSION_LEVEL");
    if (comp_level) {
        char* end;
        int level = strtol(comp_level, &end, 10);
        if (level<0 || level>100 || end==comp_level || *end)
            fprintf(stderr, "Invalid compression level: \\\"%s\\\"\\n", comp_level);
        else
            compression_level = level;
    }
    
    fwrite("WIP15", 5, 1, trace_file);
    
    if (BYTE_ORDER == LITTLE_ENDIAN)
        fwrite("_", 1, 1, trace_file);
    else
        fwrite("-", 1, 1, trace_file);
    
    fwrite("0.0a            ", 16, 1, trace_file);
    
    gl_write_uint32(FUNC_COUNT);
    
    lib_gl = actual_dlopen("libGL.so.1", RTLD_NOW|RTLD_LOCAL);
    
    if (!lib_gl) {
        fprintf(stderr, "Unable to open libGL.so");
        fflush(stderr);
    }
    
    handle_limits();
    
    current_limits = NULL;
    
''')

for name in [f.name for f in funcs]:
    if name.startswith('glX'):
        gl_c.write('    gl_%s = (%s_t)dlsym(lib_gl, "%s");\n' % (name, name, name))
    else:
        gl_c.write('    gl_%s = NULL;\n' % name)

gl_c.write("}\n")
