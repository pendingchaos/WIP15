import glxml

funcs = []
func_dict = {}
next_func_id = 0

groups = []
group_dict = {}
next_group_id = 0

_cur_file = None

gl = glxml.GL(False)

def indent(s, amount):
    return '\n'.join(['    '*amount+l for l in s.split('\n')])

class Group(object):
    def __init__(self, name=None):
        global next_group_id, groups
        self.name = name if name!=None else 'group_%u'%next_group_id
        self.vals = {}
        self.minvers = {}
        if self.name in group_dict:
            self.group_id = group_dict[self.name].group_id
            groups.remove(group_dict[self.name])
        else:
            self.group_id = next_group_id
            next_group_id += 1
        group_dict[self.name] = self
        groups.append(self)
    
    def add(self, name, val, minver):
        self.vals[name] = val
        self.minvers[name] = minver
        return self

def g(*items, **kwargs):
    res = Group(kwargs.get('name', None))
    for item in items:
        name = item
        minver = (1, 0)
        if type(item) == tuple:
            name = item[0]
            minver = item[1], item[2]
        val = gl.enumValues[name]
        res.add(name, val, minver)
    return res

class Type(object):
    def __init__(self):
        pass
    
    def _gen_group_write_code(self, group):
        if group != None: return ';gl_write_uint32(%d)' % group.group_id
        else: return ''
    
    def gen_type_code(self, var_name='', array_count=None):
        return ''
    
    def gen_write_code(self, var_name, array_count=None, group=None):
        return ''
    
    def gen_write_type_code(self, array_count=None, group=False):
        return ''

class Param(object):
    def __init__(self, dtype, name, array_count=None, group=None):
        if type(group)==str and group not in group_dict: group = group.split(' ')
        if type(group) == list: group = g(*tuple(group))
        self.dtype = dtype() if isinstance(dtype, type) else dtype
        self.name = name
        self.array_count = array_count
        self.group = group_dict[group] if type(group)==str else group
    
    def gen_param_code(self):
        return self.dtype.gen_type_code(self.name, self.array_count)
    
    def gen_write_code(self):
        return self.dtype.gen_write_code(self.name, self.array_count, self.group)
    
    def gen_write_type_code(self):
        return self.dtype.gen_write_type_code(self.array_count, self.group)

class Func(object):
    def __init__(self, minver, name, params, rettype=None):
        global next_func_id, funcs
        
        self._file = _cur_file
        
        if name in func_dict:
            if func_dict[name]._file == self._file:
                print "Warning: Redefining %s in %s" % (name, self._file)
            
            # TODO: Remove this
            for p1, p2 in zip(func_dict[name].params, params):
                if p1.group!=None and p2.group==None:
                    print "Warning: Group removed from", name
                    break
            
            self.func_id = func_dict[name].func_id
            funcs.remove(func_dict[name])
        else:
            self.func_id = next_func_id
            next_func_id += 1
        self.minver = minver
        self.name = name
        self.params = params
        self.trace_prologue_code = ''
        self.trace_epilogue_code = ''
        self.trace_extras_code = ''
        if rettype != None: self.rettype = rettype() if isinstance(rettype, type) else rettype
        else: self.rettype = None
        
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
        
        res += '    if (!gl_%s)\n' % self.name
        res += '        gl_%s = (%s_t)gl_glXGetProcAddress("%s");\n' % (self.name, self.name, self.name)
        
        res += indent(self.trace_prologue_code, 1) + '\n'
        
        params = ', '.join([param.name for param in self.params])
        if self.rettype != None:
            res += '    %s result = gl_%s(%s);\n' % (self.rettype.gen_type_code(), self.name, params)
        else:
            res += '    gl_%s(%s);\n' % (self.name, params)
        
        res += '    gl_start_call(%d);\n' % self.func_id
        
        for param in self.params:
            res += indent(param.gen_write_code(), 1) + ';\n'
        
        if self.rettype != None:
            res += indent(self.rettype.gen_write_code('result'), 1) + ';\n'
        
        res += indent(self.trace_extras_code, 1) + '\n'
        
        res += '    gl_end_call();\n'
        
        res += indent(self.trace_epilogue_code, 1) + '\n'
        
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
    
    def gen_write_code(self, var_name, array_count=None, group=None):
        if array_count != None:
            res = 'size_t count_%d = (%s);\n' % (id(self), str(array_count))
            res += 'gl_write_uint32(count_%d);\n' % id(self)
            res += 'for (size_t i = 0; i < count_%d; i++)\n' % id(self)
            res += '    %s(%s[i]);' % (self.__class__.write_func, var_name)
            return res + self._gen_group_write_code(group)
        else:
            return '%s(%s)' % (self.__class__.write_func, var_name) + self._gen_group_write_code(group)
    
    def gen_write_type_code(self, array_count=None, group=False):
        group = 'true' if group else 'false'
        array = 'true' if array_count != None else 'false'
        return 'gl_write_type(%s, %s, %s)' % (self.__class__.base, group, array)

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
    
    def gen_write_code(self, var_name, array_count=None, group=None):
        if array_count != None:
            return 'gl_write_uint32((%s));' % str(array_count) + self._gen_group_write_code(group)
        else:
            return self._gen_group_write_code(group)
    
    def gen_write_type_code(self, array_count=None, group=False):
        group = 'true' if group else 'false'
        array = 'true' if array_count != None else 'false'
        return 'gl_write_type(BASE_FUNC_PTR, %s, %s)' % (group, array)

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

class tPointer(_Ptr): ctype = 'const void*'
class tMutablePointer(_Ptr): ctype = 'void*'
class tGLXFBConfig(_Ptr): ctype = 'GLXFBConfig'
class tGLXFBConfigSGIX(_Ptr): ctype = 'GLXFBConfigSGIX'

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
    
    def gen_write_code(self, var_name, array_count=None, group=None):
        if array_count != None:
            res = 'size_t count_%d = (%s);\n' % (id(self), str(array_count))
            res += 'gl_write_uint32(count_%d);\n' % id(self)
            res += 'for (size_t i = 0; i < count_%d; i++)\n' % id(self)
            res += '    gl_write_data((%s), %s[i]);' % (str(self.size_expr), var_name)
            return res + self._gen_group_write_code(group)
        else:
            return 'gl_write_data((%s), %s)' % (str(self.size_expr), var_name) + self._gen_group_write_code(group)
    
    def gen_write_type_code(self, array_count=None, group=False):
        group = 'true' if group else 'false'
        array = 'true' if array_count != None else 'false'
        return 'gl_write_type(BASE_DATA, %s, %s)' % (group, array)

class tGLfixed(Type):
    def gen_type_code(self, var_name='', array_count=None):
        if array_count != None: return 'GLfixed* %s' % var_name
        else: return 'GLfixed %s' % var_name
    
    def gen_write_code(self, var_name, array_count=None, group=None):
        if array_count != None:
            res = 'size_t count_%d = (%s);\n' % (id(self), str(array_count))
            res += 'gl_write_uint32(count_%d);\n' % id(self)
            res += 'for (size_t i = 0; i < count_%d; i++)\n' % id(self)
            res += '    gl_write_float(%s[i] / 65535.0);' % (var_name)
            return res + self._gen_group_write_code(group)
        else:
            return 'gl_write_float(%s/65535.0)' % var_name + self._gen_group_write_code(group)
    
    def gen_write_type_code(self, array_count=None, group=False):
        group = 'true' if group else 'false'
        array = 'true' if array_count != None else 'false'
        return 'gl_write_type(BASE_FLOAT, %s, %s)' % (group, array)

class tGLhalfNV(Type): # TODO
    def gen_type_code(self, var_name='', array_count=None):
        if array_count != None: return 'GLhalfNV* %s' % var_name
        else: return 'GLhalfNV %s' % var_name
    
    def gen_write_code(self, var_name, array_count=None, group=None):
        if array_count != None:
            res = 'size_t count_%d = (%s);\n' % (id(self), str(array_count))
            res += 'gl_write_uint32(count_%d);\n' % id(self)
            res += 'for (size_t i = 0; i < count_%d; i++)\n' % id(self)
            res += '    gl_write_float(0.0);'
            return res + self._gen_group_write_code(group)
        else:
            return 'gl_write_float(0.0)' + self._gen_group_write_code(group)
    
    def gen_write_type_code(self, array_count=None, group=False):
        group = 'true' if group else 'false'
        array = 'true' if array_count != None else 'false'
        return 'gl_write_type(BASE_FLOAT, %s, %s)' % (group, array)

class tString(Type):
    def gen_type_code(self, var_name='', array_count=None):
        if array_count != None: return 'const char** %s' % var_name
        else: return 'const char* %s' % var_name
    
    def gen_write_code(self, var_name, array_count=None, group=None):
        if array_count != None:
            res = 'size_t count_%d = (%s);\n' % (id(self), str(array_count))
            res += 'gl_write_uint32(count_%d);\n' % id(self)
            res += 'for (size_t i = 0; i < count_%d; i++)\n' % id(self)
            res += '    gl_write_str(%s[i]);' % (var_name)
            return res + self._gen_group_write_code(group)
        else:
            return 'gl_write_str(%s)' % var_name + self._gen_group_write_code(group)
    
    def gen_write_type_code(self, array_count=None, group=False):
        group = 'true' if group else 'false'
        array = 'true' if array_count != None else 'false'
        return 'gl_write_type(BASE_STRING, %s, %s)' % (group, array)

class tMutableString(tString):
    def gen_type_code(self, var_name='', array_count=None):
        if array_count != None: return 'char** %s' % var_name
        else: return 'char* %s' % var_name

class tGLBuf(tGLuint): pass
class tGLSampler(tGLuint): pass
class tGLTex(tGLuint): pass
class tGLQuery(tGLuint): pass
class tGLFramebuffer(tGLuint): pass
class tGLRenderbuffer(tGLuint): pass
class tGLsync(_Ptr): ctype = 'GLsync'
class tGLProgram(tGLuint): pass
class tGLProgramPipeline(tGLuint): pass
class tGLShader(tGLuint): pass
class tGLVAO(tGLuint): pass
class tGLTransformFeedback(tGLuint): pass
class tGLeglImageOES(_Ptr): ctype = 'GLeglImageOES'
class tGLXContext(_Ptr): ctype = 'GLXContext'
class tXID(tuint32_t): pass

_cur_file = 'generated_gl_funcs.py'
exec open('generated_gl_funcs.py').read()

_cur_file = 'gl_funcs.py'
exec open('gl_funcs.py').read()
