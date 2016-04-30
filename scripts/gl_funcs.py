P = Param

class glXGetProcAddressFunc(Func):
    def gen_wrapper(self):
        res = '__GLXextFuncPtr %s(const char* name) {\n' % self.name
        res += '    func_decl_%s();\n' % self.name
        res += '    gl_start_call(%d);\n' % self.func_id
        res += '    gl_write_str(name);\n'
        res += '    func_t result = (func_t)dlsym(RTLD_DEFAULT, name);\n'
        #res += '    gl_write_func_ptr(result);\n' # TODO
        res += '    gl_end_call();\n'
        res += '    return result;\n'
        res += '}'
        
        return res

class glDrawableSizeWIP15(Func):
    def gen_wrapper(self):
        return '''void glDrawableSizeWIP15(GLsizei width, GLsizei height) {
    func_decl_glDrawableSizeWIP15();
    gl_start_call(%d);
    gl_write_sleb128(width);
    gl_write_sleb128(height);
    gl_end_call();
}''' % self.func_id

class glTestFBWIP15(Func):
    def gen_wrapper(self):
        return '''void glTestFBWIP15(const GLchar* name, const GLvoid* color, const GLvoid* depth) {
    func_decl_glTestFBWIP15();
    gl_start_call(%d);
    gl_write_str(name);
    gl_write_data(drawable_width*drawable_height*4, color);
    gl_write_data(drawable_width*drawable_height*4, depth);
    gl_end_call();
}
''' % self.func_id

class tTexImageData(tData):
    def __init__(self, dim):
        Type.__init__(self)
        self.texel_count_expr = ['width', 'width*height', 'width*height*depth'][dim-1]
    
    def gen_write_code(self, var_name, array_count=None):
        res = 'size_t sz_%d = get_texel_size(format, type) * %s;\n' % (id(self), self.texel_count_expr)
        if array_count != None:
            res += 'size_t count_%d = (%s);\n' % (id(self), str(array_count))
            res += 'gl_write_uint32(count_%d);\n' % id(self)
            res += 'for (size_t i = 0; i < count_%d; i++)\n' % id(self)
            res += '    if (%s[i]) gl_write_data(sz_%d, %s[i]);' % (var_name, id(self), var_name)
            res += '    else {\n'
            res += '        void* d = calloc(sz_%d, 1);\n' % id(self)
            res += '        gl_write_data(sz_%d, d);' % id(self)
            res += '        free(d);\n'
            res += '    }'
        else:
            res += 'if (%s) gl_write_data(sz_%d, %s);\n' % (var_name, id(self), var_name)
            res += 'else {\n'
            res += '    void* d = calloc(sz_%d, 1);\n' % id(self)
            res += '    gl_write_data(sz_%d, d);\n' % id(self)
            res += '    free(d);\n'
            res += '}'
        return res

class tBufData(tData):
    def gen_write_code(self, var_name, array_count=None):
        if array_count != None:
            res = 'size_t count_%d = (%s);\n' % (id(self), str(array_count))
            res += 'gl_write_uint32(count_%d);\n' % id(self)
            res += 'for (size_t i = 0; i < count_%d; i++)\n' % id(self)
            res += '    if (%s[i]) gl_write_data((%s), %s[i]);\n' % (var_name, str(self.size_expr), var_name)
            res += '    else {\n'
            res += '        size_t sz_%d = %s;\n' % (id(self), str(self.size_expr))
            res += '        void* d = calloc(sz_%d, 1);\n' % id(self)
            res += '        gl_write_data(sz_%d, d);\n' % id(self)
            res += '        free(d);\n'
            res += '    }'
        else:
            res = 'if (%s) gl_write_data((%s), %s);\n' % (var_name, self.size_expr, var_name)
            res += 'else {\n'
            res += '    size_t sz_%d = %s;\n' % (id(self), str(self.size_expr))
            res += '    void* d = calloc(sz_%d, 1);\n' % id(self)
            res += '    gl_write_data(sz_%d, d);\n' % id(self)
            res += '    free(d);\n'
            res += '}'
        return res

class tShdrSrc(Type):
    def gen_type_code(self, var_name='', array_count=None):
        return 'char** %s' % var_name
    
    def gen_write_code(self, var_name, array_count=None):
        return '''if (length) {
    gl_write_uint32(count);
    for (size_t i = 0; i < count; i++) {
        GLchar* src = malloc(length[i]+1);
        memcpy(src, string[i], length[i]);
        src[length[i]] = 0;
        gl_write_str(src);
        free(src);
    }
} else {
    gl_write_uint32(count);
    for (size_t i = 0; i < count; i++)
        gl_write_str(string[i]);
}'''
    
    def gen_write_type_code(self, array_count=None):
        return 'gl_write_type(BASE_STRING, false, true);'

#Func('glTexParameterfv', [P(tGLenum, 'target'), P(tGLenum, 'pname'), P(tPointer, 'params')])
#Func('glTexParameteriv', [P(tGLenum, 'target'), P(tGLenum, 'pname'), P(tPointer, 'params')])

Func('glTexImage1D', [P(tGLenum, 'target'), P(tGLint, 'level'), P(tGLint, 'internalformat'),
                      P(tGLsizei, 'width'), P(tGLint, 'border'), P(tGLenum, 'format'),
                      P(tGLenum, 'type'), P(tTexImageData(1), 'pixels')])

Func('glTexImage2D', [P(tGLenum, 'target'), P(tGLint, 'level'), P(tGLint, 'internalformat'),
                      P(tGLsizei, 'width'), P(tGLsizei, 'height'), P(tGLint, 'border'),
                      P(tGLenum, 'format'), P(tGLenum, 'type'),
                      P(tTexImageData(2), 'pixels')])

#Func('glReadPixels', [P(tGLint, 'x'), P(tGLint, 'y'), P(tGLsizei, 'width'), P(tGLsizei, 'height'), P(tGLenum, 'format'), P(tGLenum, 'type'), P(tMutablePointer, 'pixels')])
#Func('glGetBooleanv', [P(tGLenum, 'pname'), P(tMutablePointer, 'data')])
#Func('glGetDoublev', [P(tGLenum, 'pname'), P(tMutablePointer, 'data')])
#Func('glGetFloatv', [P(tGLenum, 'pname'), P(tMutablePointer, 'data')])
#Func('glGetIntegerv', [P(tGLenum, 'pname'), P(tMutablePointer, 'data')])
#Func('glGetString', [P(tGLenum, 'name')], tPointer)
#Func('glGetTexImage', [P(tGLenum, 'target'), P(tGLint, 'level'), P(tGLenum, 'format'), P(tGLenum, 'type'), P(tMutablePointer, 'pixels')])
#Func('glGetTexParameterfv', [P(tGLenum, 'target'), P(tGLenum, 'pname'), P(tMutablePointer, 'params')])
#Func('glGetTexParameteriv', [P(tGLenum, 'target'), P(tGLenum, 'pname'), P(tMutablePointer, 'params')])
#Func('glGetTexLevelParameterfv', [P(tGLenum, 'target'), P(tGLint, 'level'), P(tGLenum, 'pname'), P(tMutablePointer, 'params')])
#Func('glGetTexLevelParameteriv', [P(tGLenum, 'target'), P(tGLint, 'level'), P(tGLenum, 'pname'), P(tMutablePointer, 'params')])

Func('glTexSubImage1D', [P(tGLenum, 'target'), P(tGLint, 'level'), P(tGLint, 'xoffset'),
                         P(tGLsizei, 'width'), P(tGLenum, 'format'), P(tGLenum, 'type'),
                         P(tTexImageData(1), 'pixels')])

Func('glTexSubImage2D', [P(tGLenum, 'target'), P(tGLint, 'level'), P(tGLint, 'xoffset'),
                         P(tGLint, 'yoffset'), P(tGLsizei, 'width'), P(tGLsizei, 'height'),
                         P(tGLenum, 'format'), P(tGLenum, 'type'),
                         P(tTexImageData(2), 'pixels')])

Func('glDeleteTextures', [P(tGLsizei, 'n'), P(tGLuint, 'textures', 'n')])
Func('glGenTextures', [P(tGLsizei, 'n'), P(tGLuint, 'textures', 'n')])

Func('glDrawRangeElements', [P(tGLenum, 'mode'), P(tGLuint, 'start'), P(tGLuint, 'end'),
                             P(tGLsizei, 'count'), P(tGLenum, 'type'), P(tPointer, 'indices')])\
     .trace_epilogue_code = 'if (test_mode) test_fb("glDrawRangeElements");'

Func('glTexImage3D', [P(tGLenum, 'target'), P(tGLint, 'level'), P(tGLint, 'internalformat'),
                      P(tGLsizei, 'width'), P(tGLsizei, 'height'), P(tGLsizei, 'depth'),
                      P(tGLint, 'border'), P(tGLenum, 'format'), P(tGLenum, 'type'),
                      P(tTexImageData(3), 'pixels')])

Func('glTexSubImage3D', [P(tGLenum, 'target'), P(tGLint, 'level'), P(tGLint, 'xoffset'),
                         P(tGLint, 'yoffset'), P(tGLint, 'zoffset'), P(tGLsizei, 'width'),
                         P(tGLsizei, 'height'), P(tGLsizei, 'depth'), P(tGLenum, 'format'),
                         P(tGLenum, 'type'), P(tTexImageData(3), 'pixels')])

#Func('glCompressedTexImage3D', [P(tGLenum, 'target'), P(tGLint, 'level'), P(tGLenum, 'internalformat'), P(tGLsizei, 'width'), P(tGLsizei, 'height'), P(tGLsizei, 'depth'), P(tGLint, 'border'), P(tGLsizei, 'imageSize'), P(tPointer, 'data')])
#Func('glCompressedTexImage2D', [P(tGLenum, 'target'), P(tGLint, 'level'), P(tGLenum, 'internalformat'), P(tGLsizei, 'width'), P(tGLsizei, 'height'), P(tGLint, 'border'), P(tGLsizei, 'imageSize'), P(tPointer, 'data')])
#Func('glCompressedTexImage1D', [P(tGLenum, 'target'), P(tGLint, 'level'), P(tGLenum, 'internalformat'), P(tGLsizei, 'width'), P(tGLint, 'border'), P(tGLsizei, 'imageSize'), P(tPointer, 'data')])
#Func('glCompressedTexSubImage3D', [P(tGLenum, 'target'), P(tGLint, 'level'), P(tGLint, 'xoffset'), P(tGLint, 'yoffset'), P(tGLint, 'zoffset'), P(tGLsizei, 'width'), P(tGLsizei, 'height'), P(tGLsizei, 'depth'), P(tGLenum, 'format'), P(tGLsizei, 'imageSize'), P(tPointer, 'data')])
#Func('glCompressedTexSubImage2D', [P(tGLenum, 'target'), P(tGLint, 'level'), P(tGLint, 'xoffset'), P(tGLint, 'yoffset'), P(tGLsizei, 'width'), P(tGLsizei, 'height'), P(tGLenum, 'format'), P(tGLsizei, 'imageSize'), P(tPointer, 'data')])
#Func('glCompressedTexSubImage1D', [P(tGLenum, 'target'), P(tGLint, 'level'), P(tGLint, 'xoffset'), P(tGLsizei, 'width'), P(tGLenum, 'format'), P(tGLsizei, 'imageSize'), P(tPointer, 'data')])
#Func('glGetCompressedTexImage', [P(tGLenum, 'target'), P(tGLint, 'level'), P(tMutablePointer, 'img')])

Func('glMultiDrawArrays', [P(tGLenum, 'mode'), P(tGLint, 'first', 'drawcount'),
                           P(tGLsizei, 'count', 'drawcount'), P(tGLsizei, 'drawcount')])\
     .trace_epilogue_code = 'if (test_mode) test_fb("glMultiDrawArrays");'

Func('glMultiDrawElements', [P(tGLenum, 'mode'), P(tPointer, 'count', 'drawcount'), P(tGLenum, 'type'),
                             P(tPointer, 'indices', 'drawcount'), P(tGLsizei, 'drawcount')])\
     .trace_epilogue_code = 'if (test_mode) test_fb("glMultiDrawElements");'

#Func('glPointParameterfv', [P(tGLenum, 'pname'), P(tPointer, 'params')])
#Func('glPointParameteriv', [P(tGLenum, 'pname'), P(tPointer, 'params')])
Func('glGenQueries', [P(tGLsizei, 'n'), P(tGLuint, 'ids', 'n')])
Func('glDeleteQueries', [P(tGLsizei, 'n'), P(tGLuint, 'ids', 'n')])
#Func('glGetQueryiv', [P(tGLenum, 'target'), P(tGLenum, 'pname'), P(tMutablePointer, 'params')])
#Func('glGetQueryObjectiv', [P(tGLuint, 'id'), P(tGLenum, 'pname'), P(tMutablePointer, 'params')])
#Func('glGetQueryObjectuiv', [P(tGLuint, 'id'), P(tGLenum, 'pname'), P(tMutablePointer, 'params')])
Func('glDeleteBuffers', [P(tGLsizei, 'n'), P(tGLuint, 'buffers', 'n')])
Func('glGenBuffers', [P(tGLsizei, 'n'), P(tGLuint, 'buffers', 'n')])

Func('glBufferData', [P(tGLenum, 'target'), P(tGLsizeiptr, 'size'),
                      P(tBufData('size'), 'data'), P(tGLenum, 'usage')])

Func('glBufferSubData', [P(tGLenum, 'target'), P(tGLintptr, 'offset'),
                         P(tGLsizeiptr, 'size'), P(tBufData('size'), 'data')])

#Func('glGetBufferSubData', [P(tGLenum, 'target'), P(tGLintptr, 'offset'), P(tGLsizeiptr, 'size'), P(tMutablePointer, 'data')])
#Func('glGetBufferParameteriv', [P(tGLenum, 'target'), P(tGLenum, 'pname'), P(tMutablePointer, 'params')])
#Func('glGetBufferPointerv', [P(tGLenum, 'target'), P(tGLenum, 'pname'), P(tMutablePointer, 'params')])
Func('glDrawBuffers', [P(tGLsizei, 'n'), P(tGLenum, 'bufs', 'n')])
#Func('glGetActiveAttrib', [P(tGLuint, 'program'), P(tGLuint, 'index'), P(tGLsizei, 'bufSize'), P(tMutablePointer, 'length'), P(tMutablePointer, 'size'), P(tMutablePointer, 'type'), P(tMutableString, 'name')])
#Func('glGetActiveUniform', [P(tGLuint, 'program'), P(tGLuint, 'index'), P(tGLsizei, 'bufSize'), P(tMutablePointer, 'length'), P(tMutablePointer, 'size'), P(tMutablePointer, 'type'), P(tMutableString, 'name')])
#Func('glGetAttachedShaders', [P(tGLuint, 'program'), P(tGLsizei, 'maxCount'), P(tMutablePointer, 'count'), P(tMutablePointer, 'shaders')])
#Func('glGetProgramiv', [P(tGLuint, 'program'), P(tGLenum, 'pname'), P(tMutablePointer, 'params')])
#Func('glGetProgramInfoLog', [P(tGLuint, 'program'), P(tGLsizei, 'bufSize'), P(tMutablePointer, 'length'), P(tMutableString, 'infoLog')])
#Func('glGetShaderiv', [P(tGLuint, 'shader'), P(tGLenum, 'pname'), P(tMutablePointer, 'params')])
#Func('glGetShaderInfoLog', [P(tGLuint, 'shader'), P(tGLsizei, 'bufSize'), P(tMutablePointer, 'length'), P(tMutableString, 'infoLog')])
#Func('glGetShaderSource', [P(tGLuint, 'shader'), P(tGLsizei, 'bufSize'), P(tMutablePointer, 'length'), P(tMutableString, 'source')])
#Func('glGetUniformfv', [P(tGLuint, 'program'), P(tGLint, 'location'), P(tMutablePointer, 'params')])
#Func('glGetUniformiv', [P(tGLuint, 'program'), P(tGLint, 'location'), P(tMutablePointer, 'params')])
#Func('glGetVertexAttribdv', [P(tGLuint, 'index'), P(tGLenum, 'pname'), P(tMutablePointer, 'params')])
#Func('glGetVertexAttribfv', [P(tGLuint, 'index'), P(tGLenum, 'pname'), P(tMutablePointer, 'params')])
#Func('glGetVertexAttribiv', [P(tGLuint, 'index'), P(tGLenum, 'pname'), P(tMutablePointer, 'params')])
#Func('glGetVertexAttribPointerv', [P(tGLuint, 'index'), P(tGLenum, 'pname'), P(tMutablePointer, 'pointer')])
Func('glShaderSource', [P(tGLuint, 'shader'), P(tGLsizei, 'count'), P(tShdrSrc, 'string'), P(tGLint, 'length', 'length?count:0')])
Func('glUniform1fv', [P(tGLint, 'location'), P(tGLsizei, 'count'), P(tGLfloat, 'value', 'count')])
Func('glUniform2fv', [P(tGLint, 'location'), P(tGLsizei, 'count'), P(tGLfloat, 'value', 'count*2')])
Func('glUniform3fv', [P(tGLint, 'location'), P(tGLsizei, 'count'), P(tGLfloat, 'value', 'count*3')])
Func('glUniform4fv', [P(tGLint, 'location'), P(tGLsizei, 'count'), P(tGLfloat, 'value', 'count*4')])
Func('glUniform1iv', [P(tGLint, 'location'), P(tGLsizei, 'count'), P(tGLint, 'value', 'count')])
Func('glUniform2iv', [P(tGLint, 'location'), P(tGLsizei, 'count'), P(tGLint, 'value', 'count*2')])
Func('glUniform3iv', [P(tGLint, 'location'), P(tGLsizei, 'count'), P(tGLint, 'value', 'count*3')])
Func('glUniform4iv', [P(tGLint, 'location'), P(tGLsizei, 'count'), P(tGLint, 'value', 'count*4')])
Func('glUniformMatrix2fv', [P(tGLint, 'location'), P(tGLsizei, 'count'), P(tGLboolean, 'transpose'), P(tGLfloat, 'value', '4*count')])
Func('glUniformMatrix3fv', [P(tGLint, 'location'), P(tGLsizei, 'count'), P(tGLboolean, 'transpose'), P(tGLfloat, 'value', '9*count')])
Func('glUniformMatrix4fv', [P(tGLint, 'location'), P(tGLsizei, 'count'), P(tGLboolean, 'transpose'), P(tGLfloat, 'value', '16*count')])
Func('glVertexAttrib1dv', [P(tGLuint, 'index'), P(tGLdouble, 'v', 1)])
Func('glVertexAttrib1fv', [P(tGLuint, 'index'), P(tGLfloat, 'v', 1)])
Func('glVertexAttrib1sv', [P(tGLuint, 'index'), P(tGLshort, 'v', 1)])
Func('glVertexAttrib2dv', [P(tGLuint, 'index'), P(tGLdouble, 'v', 2)])
Func('glVertexAttrib2fv', [P(tGLuint, 'index'), P(tGLfloat, 'v', 2)])
Func('glVertexAttrib2sv', [P(tGLuint, 'index'), P(tGLshort, 'v', 2)])
Func('glVertexAttrib3dv', [P(tGLuint, 'index'), P(tGLdouble, 'v', 3)])
Func('glVertexAttrib3fv', [P(tGLuint, 'index'), P(tGLfloat, 'v', 3)])
Func('glVertexAttrib3sv', [P(tGLuint, 'index'), P(tGLshort, 'v', 3)])
Func('glVertexAttrib4Nbv', [P(tGLuint, 'index'), P(tGLbyte, 'v', 4)])
Func('glVertexAttrib4Niv', [P(tGLuint, 'index'), P(tGLint, 'v', 4)])
Func('glVertexAttrib4Nsv', [P(tGLuint, 'index'), P(tGLshort, 'v', 4)])
Func('glVertexAttrib4Nubv', [P(tGLuint, 'index'), P(tGLubyte, 'v', 4)])
Func('glVertexAttrib4Nuiv', [P(tGLuint, 'index'), P(tGLuint, 'v', 4)])
Func('glVertexAttrib4Nusv', [P(tGLuint, 'index'), P(tGLushort, 'v', 4)])
Func('glVertexAttrib4bv', [P(tGLuint, 'index'), P(tGLbyte, 'v', 4)])
Func('glVertexAttrib4dv', [P(tGLuint, 'index'), P(tGLdouble, 'v', 4)])
Func('glVertexAttrib4fv', [P(tGLuint, 'index'), P(tGLfloat, 'v', 4)])
Func('glVertexAttrib4iv', [P(tGLuint, 'index'), P(tGLint, 'v', 4)])
Func('glVertexAttrib4sv', [P(tGLuint, 'index'), P(tGLshort, 'v', 4)])
Func('glVertexAttrib4ubv', [P(tGLuint, 'index'), P(tGLubyte, 'v', 4)])
Func('glVertexAttrib4uiv', [P(tGLuint, 'index'), P(tGLuint, 'v', 4)])
Func('glVertexAttrib4usv', [P(tGLuint, 'index'), P(tGLushort, 'v', )])
#Func('glVertexAttribPointer', [P(tGLuint, 'index'), P(tGLint, 'size'), P(tGLenum, 'type'), P(tGLboolean, 'normalized'), P(tGLsizei, 'stride'), P(tPointer, 'pointer')])

Func('glUniformMatrix2x3fv', [P(tGLint, 'location'), P(tGLsizei, 'count'),
                              P(tGLboolean, 'transpose'), P(tGLfloat, 'value', 'count*6')])

Func('glUniformMatrix3x2fv', [P(tGLint, 'location'), P(tGLsizei, 'count'),
                              P(tGLboolean, 'transpose'), P(tGLfloat, 'value', 'count*6')])

Func('glUniformMatrix2x4fv', [P(tGLint, 'location'), P(tGLsizei, 'count'),
                              P(tGLboolean, 'transpose'), P(tGLfloat, 'value', 'count*8')])

Func('glUniformMatrix4x2fv', [P(tGLint, 'location'), P(tGLsizei, 'count'),
                              P(tGLboolean, 'transpose'), P(tGLfloat, 'value', 'count*8')])

Func('glUniformMatrix3x4fv', [P(tGLint, 'location'), P(tGLsizei, 'count'),
                              P(tGLboolean, 'transpose'), P(tGLfloat, 'value', 'count*12')])

Func('glUniformMatrix4x3fv', [P(tGLint, 'location'), P(tGLsizei, 'count'),
                              P(tGLboolean, 'transpose'), P(tGLfloat, 'value', 'count*12')])

#Func('glGetBooleani_v', [P(tGLenum, 'target'), P(tGLuint, 'index'), P(tMutablePointer, 'data')])
#Func('glGetIntegeri_v', [P(tGLenum, 'target'), P(tGLuint, 'index'), P(tMutablePointer, 'data')])
#Func('glTransformFeedbackVaryings', [P(tGLuint, 'program'), P(tGLsizei, 'count'), P(tPointer, 'varyings'), P(tGLenum, 'bufferMode')])
#Func('glGetTransformFeedbackVarying', [P(tGLuint, 'program'), P(tGLuint, 'index'), P(tGLsizei, 'bufSize'), P(tMutablePointer, 'length'), P(tMutablePointer, 'size'), P(tMutablePointer, 'type'), P(tMutableString, 'name')])
#Func('glVertexAttribIPointer', [P(tGLuint, 'index'), P(tGLint, 'size'), P(tGLenum, 'type'), P(tGLsizei, 'stride'), P(tPointer, 'pointer')])
#Func('glGetVertexAttribIiv', [P(tGLuint, 'index'), P(tGLenum, 'pname'), P(tMutablePointer, 'params')])
#Func('glGetVertexAttribIuiv', [P(tGLuint, 'index'), P(tGLenum, 'pname'), P(tMutablePointer, 'params')])
Func('glVertexAttribI1iv', [P(tGLuint, 'index'), P(tGLint, 'v', 1)])
Func('glVertexAttribI2iv', [P(tGLuint, 'index'), P(tGLint, 'v', 2)])
Func('glVertexAttribI3iv', [P(tGLuint, 'index'), P(tGLint, 'v', 3)])
Func('glVertexAttribI4iv', [P(tGLuint, 'index'), P(tGLint, 'v', 4)])
Func('glVertexAttribI1uiv', [P(tGLuint, 'index'), P(tGLuint, 'v', 1)])
Func('glVertexAttribI2uiv', [P(tGLuint, 'index'), P(tGLuint, 'v', 2)])
Func('glVertexAttribI3uiv', [P(tGLuint, 'index'), P(tGLuint, 'v', 3)])
Func('glVertexAttribI4uiv', [P(tGLuint, 'index'), P(tGLuint, 'v', 4)])
Func('glVertexAttribI4bv', [P(tGLuint, 'index'), P(tGLbyte, 'v', 4)])
Func('glVertexAttribI4sv', [P(tGLuint, 'index'), P(tGLbyte, 'v', 4)])
Func('glVertexAttribI4ubv', [P(tGLuint, 'index'), P(tGLubyte, 'v', 4)])
Func('glVertexAttribI4usv', [P(tGLuint, 'index'), P(tGLubyte, 'v', 4)])
#Func('glGetUniformuiv', [P(tGLuint, 'program'), P(tGLint, 'location'), P(tMutablePointer, 'params')])
Func('glUniform1uiv', [P(tGLint, 'location'), P(tGLsizei, 'count'), P(tGLuint, 'value', 'count')])
Func('glUniform2uiv', [P(tGLint, 'location'), P(tGLsizei, 'count'), P(tPointer, 'value', 'count*2')])
Func('glUniform3uiv', [P(tGLint, 'location'), P(tGLsizei, 'count'), P(tPointer, 'value', 'count*3')])
Func('glUniform4uiv', [P(tGLint, 'location'), P(tGLsizei, 'count'), P(tPointer, 'value', 'count*4')])
#Func('glTexParameterIiv', [P(tGLenum, 'target'), P(tGLenum, 'pname'), P(tPointer, 'params')])
#Func('glTexParameterIuiv', [P(tGLenum, 'target'), P(tGLenum, 'pname'), P(tPointer, 'params')])
#Func('glGetTexParameterIiv', [P(tGLenum, 'target'), P(tGLenum, 'pname'), P(tMutablePointer, 'params')])
#Func('glGetTexParameterIuiv', [P(tGLenum, 'target'), P(tGLenum, 'pname'), P(tMutablePointer, 'params')])
Func('glClearBufferiv', [P(tGLenum, 'buffer'), P(tGLint, 'drawbuffer'), P(tGLint, 'value', 'buffer==GL_COLOR?4:1')])\
     .trace_epilogue_code = 'if (test_mode) test_fb("glClearBufferiv");'
Func('glClearBufferuiv', [P(tGLenum, 'buffer'), P(tGLint, 'drawbuffer'), P(tGLuint, 'value', 'buffer==GL_COLOR?4:1')])\
     .trace_epilogue_code = 'if (test_mode) test_fb("glClearBufferuiv");'
Func('glClearBufferfv', [P(tGLenum, 'buffer'), P(tGLint, 'drawbuffer'), P(tGLfloat, 'value', 'buffer==GL_COLOR?4:1')])\
     .trace_epilogue_code = 'if (test_mode) test_fb("glClearBufferfv");'
#Func('glGetStringi', [P(tGLenum, 'name'), P(tGLuint, 'index')], tPointer)
Func('glDeleteRenderbuffers', [P(tGLsizei, 'n'), P(tGLuint, 'renderbuffers', 'n')])
Func('glGenRenderbuffers', [P(tGLsizei, 'n'), P(tGLuint, 'renderbuffers', 'n')])
#Func('glGetRenderbufferParameteriv', [P(tGLenum, 'target'), P(tGLenum, 'pname'), P(tMutablePointer, 'params')])
Func('glDeleteFramebuffers', [P(tGLsizei, 'n'), P(tGLuint, 'framebuffers', 'n')])
Func('glGenFramebuffers', [P(tGLsizei, 'n'), P(tGLuint, 'framebuffers', 'n')])
#Func('glGetFramebufferAttachmentParameteriv', [P(tGLenum, 'target'), P(tGLenum, 'attachment'), P(tGLenum, 'pname'), P(tMutablePointer, 'params')])
#Func('glMapBufferRange', [P(tGLenum, 'target'), P(tGLintptr, 'offset'), P(tGLsizeiptr, 'length'), P(tGLbitfield, 'access')], tPointer)
Func('glDeleteVertexArrays', [P(tGLsizei, 'n'), P(tGLuint, 'arrays', 'n')])
Func('glGenVertexArrays', [P(tGLsizei, 'n'), P(tGLuint, 'arrays', 'n')])

Func('glDrawElementsInstanced', [P(tGLenum, 'mode'), P(tGLsizei, 'count'),
                                 P(tGLenum, 'type'), P(tPointer, 'indices'),
                                 P(tGLsizei, 'instancecount')])\
     .trace_epilogue_code = 'if (test_mode) test_fb("glDrawElementsInstanced");'

#Func('glGetUniformIndices', [P(tGLuint, 'program'), P(tGLsizei, 'uniformCount'), P(tPointer, 'uniformNames'), P(tMutablePointer, 'uniformIndices')])
#Func('glGetActiveUniformsiv', [P(tGLuint, 'program'), P(tGLsizei, 'uniformCount'), P(tPointer, 'uniformIndices'), P(tGLenum, 'pname'), P(tMutablePointer, 'params')])
#Func('glGetActiveUniformName', [P(tGLuint, 'program'), P(tGLuint, 'uniformIndex'), P(tGLsizei, 'bufSize'), P(tMutablePointer, 'length'), P(tMutableString, 'uniformName')])
#Func('glGetActiveUniformBlockiv', [P(tGLuint, 'program'), P(tGLuint, 'uniformBlockIndex'), P(tGLenum, 'pname'), P(tMutablePointer, 'params')])
#Func('glGetActiveUniformBlockName', [P(tGLuint, 'program'), P(tGLuint, 'uniformBlockIndex'), P(tGLsizei, 'bufSize'), P(tMutablePointer, 'length'), P(tMutableString, 'uniformBlockName')])
#Func('glGetIntegeri_v', [P(tGLenum, 'target'), P(tGLuint, 'index'), P(tMutablePointer, 'data')])

Func('glDrawElementsBaseVertex', [P(tGLenum, 'mode'), P(tGLsizei, 'count'),
                                  P(tGLenum, 'type'), P(tPointer, 'indices'),
                                  P(tGLint, 'basevertex')])\
     .trace_epilogue_code = 'if (test_mode) test_fb("glDrawElementsBaseVertex");'

Func('glDrawRangeElementsBaseVertex', [P(tGLenum, 'mode'), P(tGLuint, 'start'),
                                       P(tGLuint, 'end'), P(tGLsizei, 'count'),
                                       P(tGLenum, 'type'), P(tPointer, 'indices'),
                                       P(tGLint, 'basevertex')])\
     .trace_epilogue_code = 'if (test_mode) test_fb("glDrawRangeElementsBaseVertex");'

Func('glDrawElementsInstancedBaseVertex', [P(tGLenum, 'mode'), P(tGLsizei, 'count'),
                                           P(tGLenum, 'type'), P(tPointer, 'indices'),
                                           P(tGLsizei, 'instancecount'), P(tGLint, 'basevertex')])\
     .trace_epilogue_code = 'if (test_mode) test_fb("glDrawElementsInstancedBaseVertex");'

Func('glMultiDrawElementsBaseVertex', [P(tGLenum, 'mode'), P(tGLsizei, 'count', 'drawcount'),
                                       P(tGLenum, 'type'), P(tPointer, 'indices', 'drawcount'),
                                       P(tGLsizei, 'drawcount'), P(tPointer, 'basevertex', 'drawcount')])\
     .trace_epilogue_code = 'if (test_mode) test_fb("glMultiDrawElementsBaseVertex");'

#Func('glGetInteger64v', [P(tGLenum, 'pname'), P(tMutablePointer, 'data')])
#Func('glGetSynciv', [P(tGLsync, 'sync'), P(tGLenum, 'pname'), P(tGLsizei, 'bufSize'), P(tMutablePointer, 'length'), P(tMutablePointer, 'values')])
#Func('glGetInteger64i_v', [P(tGLenum, 'target'), P(tGLuint, 'index'), P(tMutablePointer, 'data')])
#Func('glGetBufferParameteri64v', [P(tGLenum, 'target'), P(tGLenum, 'pname'), P(tMutablePointer, 'params')])
#Func('glGetMultisamplefv', [P(tGLenum, 'pname'), P(tGLuint, 'index'), P(tMutablePointer, 'val')])
Func('glGenSamplers', [P(tGLsizei, 'count'), P(tGLuint, 'samplers', 'count')])
Func('glDeleteSamplers', [P(tGLsizei, 'count'), P(tGLuint, 'samplers', 'count')])
#Func('glSamplerParameteriv', [P(tGLuint, 'sampler'), P(tGLenum, 'pname'), P(tPointer, 'param')])
#Func('glSamplerParameterfv', [P(tGLuint, 'sampler'), P(tGLenum, 'pname'), P(tPointer, 'param')])
#Func('glSamplerParameterIiv', [P(tGLuint, 'sampler'), P(tGLenum, 'pname'), P(tPointer, 'param')])
#Func('glSamplerParameterIuiv', [P(tGLuint, 'sampler'), P(tGLenum, 'pname'), P(tPointer, 'param')])
#Func('glGetSamplerParameteriv', [P(tGLuint, 'sampler'), P(tGLenum, 'pname'), P(tMutablePointer, 'params')])
#Func('glGetSamplerParameterIiv', [P(tGLuint, 'sampler'), P(tGLenum, 'pname'), P(tMutablePointer, 'params')])
#Func('glGetSamplerParameterfv', [P(tGLuint, 'sampler'), P(tGLenum, 'pname'), P(tMutablePointer, 'params')])
#Func('glGetSamplerParameterIuiv', [P(tGLuint, 'sampler'), P(tGLenum, 'pname'), P(tMutablePointer, 'params')])
#Func('glGetQueryObjecti64v', [P(tGLuint, 'id'), P(tGLenum, 'pname'), P(tMutablePointer, 'params')])
#Func('glGetQueryObjectui64v', [P(tGLuint, 'id'), P(tGLenum, 'pname'), P(tMutablePointer, 'params')])
#Func('glVertexAttribP1uiv', [P(tGLuint, 'index'), P(tGLenum, 'type'), P(tGLboolean, 'normalized'), P(tPointer, 'value')])
#Func('glVertexAttribP2uiv', [P(tGLuint, 'index'), P(tGLenum, 'type'), P(tGLboolean, 'normalized'), P(tPointer, 'value')])
#Func('glVertexAttribP3uiv', [P(tGLuint, 'index'), P(tGLenum, 'type'), P(tGLboolean, 'normalized'), P(tPointer, 'value')])
#Func('glVertexAttribP4uiv', [P(tGLuint, 'index'), P(tGLenum, 'type'), P(tGLboolean, 'normalized'), P(tPointer, 'value')])
#Func('glDrawArraysIndirect', [P(tGLenum, 'mode'), P(tPointer, 'indirect')]).trace_epilogue_code = 'if (test_mode) test_fb("glDrawArraysIndirect");'
#Func('glDrawElementsIndirect', [P(tGLenum, 'mode'), P(tGLenum, 'type'), P(tPointer, 'indirect')]).trace_epilogue_code = 'if (test_mode) test_fb("glDrawElementsIdirect");'
Func('glUniform1dv', [P(tGLint, 'location'), P(tGLsizei, 'count'), P(tGLdouble, 'value', 'count')])
Func('glUniform2dv', [P(tGLint, 'location'), P(tGLsizei, 'count'), P(tGLdouble, 'value', 'count*2')])
Func('glUniform3dv', [P(tGLint, 'location'), P(tGLsizei, 'count'), P(tGLdouble, 'value', 'count*3')])
Func('glUniform4dv', [P(tGLint, 'location'), P(tGLsizei, 'count'), P(tGLdouble, 'value', 'count*4')])

Func('glUniformMatrix2dv', [P(tGLint, 'location'), P(tGLsizei, 'count'),
                            P(tGLboolean, 'transpose'), P(tGLdouble, 'value', '4*count')])

Func('glUniformMatrix3dv', [P(tGLint, 'location'), P(tGLsizei, 'count'),
                            P(tGLboolean, 'transpose'), P(tGLdouble, 'value', '9*count')])

Func('glUniformMatrix4dv', [P(tGLint, 'location'), P(tGLsizei, 'count'),
                            P(tGLboolean, 'transpose'), P(tGLdouble, 'value', '16*count')])

Func('glUniformMatrix2x3dv', [P(tGLint, 'location'), P(tGLsizei, 'count'),
                              P(tGLboolean, 'transpose'), P(tGLdouble, 'value', '6*count')])

Func('glUniformMatrix2x4dv', [P(tGLint, 'location'), P(tGLsizei, 'count'),
                              P(tGLboolean, 'transpose'), P(tGLdouble, 'value', '8*count')])

Func('glUniformMatrix3x2dv', [P(tGLint, 'location'), P(tGLsizei, 'count'),
                              P(tGLboolean, 'transpose'), P(tGLdouble, 'value', '6*count')])

Func('glUniformMatrix3x4dv', [P(tGLint, 'location'), P(tGLsizei, 'count'),
                              P(tGLboolean, 'transpose'), P(tGLdouble, 'value', '12*count')])

Func('glUniformMatrix4x2dv', [P(tGLint, 'location'), P(tGLsizei, 'count'),
                              P(tGLboolean, 'transpose'), P(tGLdouble, 'value', '8*count')])

Func('glUniformMatrix4x3dv', [P(tGLint, 'location'), P(tGLsizei, 'count'),
                              P(tGLboolean, 'transpose'), P(tGLdouble, 'value', '12*count')])

#Func('glGetUniformdv', [P(tGLuint, 'program'), P(tGLint, 'location'), P(tMutablePointer, 'params')])
#Func('glGetActiveSubroutineUniformiv', [P(tGLuint, 'program'), P(tGLenum, 'shadertype'), P(tGLuint, 'index'), P(tGLenum, 'pname'), P(tMutablePointer, 'values')])
#Func('glGetActiveSubroutineUniformName', [P(tGLuint, 'program'), P(tGLenum, 'shadertype'), P(tGLuint, 'index'), P(tGLsizei, 'bufsize'), P(tMutablePointer, 'length'), P(tMutableString, 'name')])
#Func('glGetActiveSubroutineName', [P(tGLuint, 'program'), P(tGLenum, 'shadertype'), P(tGLuint, 'index'), P(tGLsizei, 'bufsize'), P(tMutablePointer, 'length'), P(tMutableString, 'name')])
#Func('glUniformSubroutinesuiv', [P(tGLenum, 'shadertype'), P(tGLsizei, 'count'), P(tPointer, 'indices')])
#Func('glGetUniformSubroutineuiv', [P(tGLenum, 'shadertype'), P(tGLint, 'location'), P(tMutablePointer, 'params')])
#Func('glGetProgramStageiv', [P(tGLuint, 'program'), P(tGLenum, 'shadertype'), P(tGLenum, 'pname'), P(tMutablePointer, 'values')])
#Func('glPatchParameterfv', [P(tGLenum, 'pname'), P(tPointer, 'values')])
Func('glDeleteTransformFeedbacks', [P(tGLsizei, 'n'), P(tGLuint, 'ids', 'n')])
Func('glGenTransformFeedbacks', [P(tGLsizei, 'n'), P(tGLuint, 'ids', 'n')])
#Func('glGetQueryIndexediv', [P(tGLenum, 'target'), P(tGLuint, 'index'), P(tGLenum, 'pname'), P(tMutablePointer, 'params')])
#Func('glShaderBinary', [P(tGLsizei, 'count'), P(tPointer, 'shaders'), P(tGLenum, 'binaryformat'), P(tPointer, 'binary'), P(tGLsizei, 'length')])
#Func('glGetShaderPrecisionFormat', [P(tGLenum, 'shadertype'), P(tGLenum, 'precisiontype'), P(tMutablePointer, 'range'), P(tMutablePointer, 'precision')])
#Func('glGetProgramBinary', [P(tGLuint, 'program'), P(tGLsizei, 'bufSize'), P(tMutablePointer, 'length'), P(tMutablePointer, 'binaryFormat'), P(tMutablePointer, 'binary')])
#Func('glProgramBinary', [P(tGLuint, 'program'), P(tGLenum, 'binaryFormat'), P(tPointer, 'binary'), P(tGLsizei, 'length')])
#Func('glCreateShaderProgramv', [P(tGLenum, 'type'), P(tGLsizei, 'count'), P(tPointer, 'strings')], tGLuint)
Func('glDeleteProgramPipelines', [P(tGLsizei, 'n'), P(tGLuint, 'pipelines', 'n')])
Func('glGenProgramPipelines', [P(tGLsizei, 'n'), P(tGLuint, 'pipelines', 'n')])
#Func('glGetProgramPipelineiv', [P(tGLuint, 'pipeline'), P(tGLenum, 'pname'), P(tMutablePointer, 'params')])

Func('glProgramUniform1iv', [P(tGLuint, 'program'), P(tGLint, 'location'),
                             P(tGLsizei, 'count'), P(tGLint, 'value', 'count')])

Func('glProgramUniform1fv', [P(tGLuint, 'program'), P(tGLint, 'location'),
                             P(tGLsizei, 'count'), P(tGLfloat, 'value', 'count')])

Func('glProgramUniform1dv', [P(tGLuint, 'program'), P(tGLint, 'location'),
                             P(tGLsizei, 'count'), P(tGLdouble, 'value', 'count')])

Func('glProgramUniform1uiv', [P(tGLuint, 'program'), P(tGLint, 'location'),
                              P(tGLsizei, 'count'), P(tGLuint, 'value', 'count')])

Func('glProgramUniform2iv', [P(tGLuint, 'program'), P(tGLint, 'location'),
                             P(tGLsizei, 'count'), P(tGLint, 'value', 'count*2')])

Func('glProgramUniform2fv', [P(tGLuint, 'program'), P(tGLint, 'location'),
                             P(tGLsizei, 'count'), P(tGLfloat, 'value', 'count*2')])

Func('glProgramUniform2dv', [P(tGLuint, 'program'), P(tGLint, 'location'),
                             P(tGLsizei, 'count'), P(tGLdouble, 'value', 'count*2')])

Func('glProgramUniform2uiv', [P(tGLuint, 'program'), P(tGLint, 'location'),
                              P(tGLsizei, 'count'), P(tGLuint, 'value', 'count*2')])

Func('glProgramUniform3iv', [P(tGLuint, 'program'), P(tGLint, 'location'),
                             P(tGLsizei, 'count'), P(tGLint, 'value', 'count*3')])

Func('glProgramUniform3fv', [P(tGLuint, 'program'), P(tGLint, 'location'),
                             P(tGLsizei, 'count'), P(tGLfloat, 'value', 'count*3')])

Func('glProgramUniform3dv', [P(tGLuint, 'program'), P(tGLint, 'location'),
                             P(tGLsizei, 'count'), P(tGLdouble, 'value', 'count*3')])

Func('glProgramUniform3uiv', [P(tGLuint, 'program'), P(tGLint, 'location'),
                              P(tGLsizei, 'count'), P(tGLuint, 'value', 'count*3')])

Func('glProgramUniform4iv', [P(tGLuint, 'program'), P(tGLint, 'location'),
                             P(tGLsizei, 'count'), P(tGLint, 'value', 'count*4')])

Func('glProgramUniform4fv', [P(tGLuint, 'program'), P(tGLint, 'location'),
                             P(tGLsizei, 'count'), P(tGLfloat, 'value', 'count*4')])

Func('glProgramUniform4dv', [P(tGLuint, 'program'), P(tGLint, 'location'),
                             P(tGLsizei, 'count'), P(tGLdouble, 'value', 'count*4')])

Func('glProgramUniform4uiv', [P(tGLuint, 'program'), P(tGLint, 'location'),
                              P(tGLsizei, 'count'), P(tGLuint, 'value', 'count*4')])

Func('glProgramUniformMatrix2fv', [P(tGLuint, 'program'), P(tGLint, 'location'),
                                   P(tGLsizei, 'count'), P(tGLboolean, 'transpose'),
                                   P(tGLfloat, 'value', 'count*4')])

Func('glProgramUniformMatrix3fv', [P(tGLuint, 'program'), P(tGLint, 'location'),
                                   P(tGLsizei, 'count'), P(tGLboolean, 'transpose'),
                                   P(tGLfloat, 'value', 'count*9')])

Func('glProgramUniformMatrix4fv', [P(tGLuint, 'program'), P(tGLint, 'location'),
                                   P(tGLsizei, 'count'), P(tGLboolean, 'transpose'),
                                   P(tGLfloat, 'value', 'count*16')])

Func('glProgramUniformMatrix2dv', [P(tGLuint, 'program'), P(tGLint, 'location'),
                                   P(tGLsizei, 'count'), P(tGLboolean, 'transpose'),
                                   P(tGLdouble, 'value', 'count*4')])

Func('glProgramUniformMatrix3dv', [P(tGLuint, 'program'), P(tGLint, 'location'), P(tGLsizei, 'count'), P(tGLboolean, 'transpose'), P(tGLdouble, 'value', 'count*9')])
Func('glProgramUniformMatrix4dv', [P(tGLuint, 'program'), P(tGLint, 'location'), P(tGLsizei, 'count'), P(tGLboolean, 'transpose'), P(tGLdouble, 'value', 'count*16')])
Func('glProgramUniformMatrix2x3fv', [P(tGLuint, 'program'), P(tGLint, 'location'), P(tGLsizei, 'count'), P(tGLboolean, 'transpose'), P(tGLfloat, 'value', 'count*6')])
Func('glProgramUniformMatrix3x2fv', [P(tGLuint, 'program'), P(tGLint, 'location'), P(tGLsizei, 'count'), P(tGLboolean, 'transpose'), P(tGLfloat, 'value', 'count*6')])
Func('glProgramUniformMatrix2x4fv', [P(tGLuint, 'program'), P(tGLint, 'location'), P(tGLsizei, 'count'), P(tGLboolean, 'transpose'), P(tGLfloat, 'value', 'count*8')])
Func('glProgramUniformMatrix4x2fv', [P(tGLuint, 'program'), P(tGLint, 'location'), P(tGLsizei, 'count'), P(tGLboolean, 'transpose'), P(tGLfloat, 'value', 'count*8')])
Func('glProgramUniformMatrix3x4fv', [P(tGLuint, 'program'), P(tGLint, 'location'), P(tGLsizei, 'count'), P(tGLboolean, 'transpose'), P(tGLfloat, 'value', 'count*12')])
Func('glProgramUniformMatrix4x3fv', [P(tGLuint, 'program'), P(tGLint, 'location'), P(tGLsizei, 'count'), P(tGLboolean, 'transpose'), P(tGLfloat, 'value', 'count*12')])
Func('glProgramUniformMatrix2x3dv', [P(tGLuint, 'program'), P(tGLint, 'location'), P(tGLsizei, 'count'), P(tGLboolean, 'transpose'), P(tGLdouble, 'value', 'count*6')])
Func('glProgramUniformMatrix3x2dv', [P(tGLuint, 'program'), P(tGLint, 'location'), P(tGLsizei, 'count'), P(tGLboolean, 'transpose'), P(tGLdouble, 'value', 'count*6')])
Func('glProgramUniformMatrix2x4dv', [P(tGLuint, 'program'), P(tGLint, 'location'), P(tGLsizei, 'count'), P(tGLboolean, 'transpose'), P(tGLdouble, 'value', 'count*8')])
Func('glProgramUniformMatrix4x2dv', [P(tGLuint, 'program'), P(tGLint, 'location'), P(tGLsizei, 'count'), P(tGLboolean, 'transpose'), P(tGLdouble, 'value', 'count*8')])
Func('glProgramUniformMatrix3x4dv', [P(tGLuint, 'program'), P(tGLint, 'location'), P(tGLsizei, 'count'), P(tGLboolean, 'transpose'), P(tGLdouble, 'value', 'count*12')])
Func('glProgramUniformMatrix4x3dv', [P(tGLuint, 'program'), P(tGLint, 'location'), P(tGLsizei, 'count'), P(tGLboolean, 'transpose'), P(tGLdouble, 'value', 'count*12')])
#Func('glGetProgramPipelineInfoLog', [P(tGLuint, 'pipeline'), P(tGLsizei, 'bufSize'), P(tMutablePointer, 'length'), P(tMutableString, 'infoLog')])
#Func('glVertexAttribL1dv', [P(tGLuint, 'index'), P(tPointer, 'v')])
#Func('glVertexAttribL2dv', [P(tGLuint, 'index'), P(tPointer, 'v')])
#Func('glVertexAttribL3dv', [P(tGLuint, 'index'), P(tPointer, 'v')])
#Func('glVertexAttribL4dv', [P(tGLuint, 'index'), P(tPointer, 'v')])
#Func('glVertexAttribLPointer', [P(tGLuint, 'index'), P(tGLint, 'size'), P(tGLenum, 'type'), P(tGLsizei, 'stride'), P(tPointer, 'pointer')])
#Func('glGetVertexAttribLdv', [P(tGLuint, 'index'), P(tGLenum, 'pname'), P(tMutablePointer, 'params')])
#Func('glViewportArrayv', [P(tGLuint, 'first'), P(tGLsizei, 'count'), P(tPointer, 'v')])
#Func('glViewportIndexedfv', [P(tGLuint, 'index'), P(tPointer, 'v')])
#Func('glScissorArrayv', [P(tGLuint, 'first'), P(tGLsizei, 'count'), P(tPointer, 'v')])
#Func('glScissorIndexedv', [P(tGLuint, 'index'), P(tPointer, 'v')])
#Func('glDepthRangeArrayv', [P(tGLuint, 'first'), P(tGLsizei, 'count'), P(tPointer, 'v')])
#Func('glGetFloati_v', [P(tGLenum, 'target'), P(tGLuint, 'index'), P(tMutablePointer, 'data')])
#Func('glGetDoublei_v', [P(tGLenum, 'target'), P(tGLuint, 'index'), P(tMutablePointer, 'data')])
Func('glDrawElementsInstancedBaseInstance', [P(tGLenum, 'mode'), P(tGLsizei, 'count'), P(tGLenum, 'type'), P(tPointer, 'indices'), P(tGLsizei, 'instancecount'), P(tGLuint, 'baseinstance')]).trace_epilogue_code = 'if (test_mode) test_fb("glDrawElementsInstancedBaseInstance");'
Func('glDrawElementsInstancedBaseVertexBaseInstance', [P(tGLenum, 'mode'), P(tGLsizei, 'count'), P(tGLenum, 'type'), P(tPointer, 'indices'), P(tGLsizei, 'instancecount'), P(tGLint, 'basevertex'), P(tGLuint, 'baseinstance')]).trace_epilogue_code = 'if (test_mode) test_fb("glDrawElementsInstanceBaseVertexBaseInstance");'
#Func('glGetInternalformativ', [P(tGLenum, 'target'), P(tGLenum, 'internalformat'), P(tGLenum, 'pname'), P(tGLsizei, 'bufSize'), P(tMutablePointer, 'params')])
#Func('glGetActiveAtomicCounterBufferiv', [P(tGLuint, 'program'), P(tGLuint, 'bufferIndex'), P(tGLenum, 'pname'), P(tMutablePointer, 'params')])
#Func('glClearBufferData', [P(tGLenum, 'target'), P(tGLenum, 'internalformat'), P(tGLenum, 'format'), P(tGLenum, 'type'), P(tPointer, 'data')])
#Func('glClearBufferSubData', [P(tGLenum, 'target'), P(tGLenum, 'internalformat'), P(tGLintptr, 'offset'), P(tGLsizeiptr, 'size'), P(tGLenum, 'format'), P(tGLenum, 'type'), P(tPointer, 'data')])
#Func('glGetFramebufferParameteriv', [P(tGLenum, 'target'), P(tGLenum, 'pname'), P(tMutablePointer, 'params')])
#Func('glGetInternalformati64v', [P(tGLenum, 'target'), P(tGLenum, 'internalformat'), P(tGLenum, 'pname'), P(tGLsizei, 'bufSize'), P(tMutablePointer, 'params')])
#Func('glInvalidateFramebuffer', [P(tGLenum, 'target'), P(tGLsizei, 'numAttachments'), P(tPointer, 'attachments')])
#Func('glInvalidateSubFramebuffer', [P(tGLenum, 'target'), P(tGLsizei, 'numAttachments'), P(tPointer, 'attachments'), P(tGLint, 'x'), P(tGLint, 'y'), P(tGLsizei, 'width'), P(tGLsizei, 'height')])
#Func('glMultiDrawArraysIndirect', [P(tGLenum, 'mode'), P(tPointer, 'indirect'), P(tGLsizei, 'drawcount'), P(tGLsizei, 'stride')]).trace_epilogue_code = 'if (test_mode) test_fb("glMultiDrawArraysIndirect");'
#Func('glMultiDrawElementsIndirect', [P(tGLenum, 'mode'), P(tGLenum, 'type'), P(tPointer, 'indirect'), P(tGLsizei, 'drawcount'), P(tGLsizei, 'stride')]).trace_epilogue_code = 'if (test_mode) test_fb("glMultiDrawElementsIndirect");'
#Func('glGetProgramInterfaceiv', [P(tGLuint, 'program'), P(tGLenum, 'programInterface'), P(tGLenum, 'pname'), P(tMutablePointer, 'params')])
#Func('glGetProgramResourceName', [P(tGLuint, 'program'), P(tGLenum, 'programInterface'), P(tGLuint, 'index'), P(tGLsizei, 'bufSize'), P(tMutablePointer, 'length'), P(tMutableString, 'name')])
#Func('glGetProgramResourceiv', [P(tGLuint, 'program'), P(tGLenum, 'programInterface'), P(tGLuint, 'index'), P(tGLsizei, 'propCount'), P(tPointer, 'props'), P(tGLsizei, 'bufSize'), P(tMutablePointer, 'length'), P(tMutablePointer, 'params')])
#Func('glDebugMessageControl', [P(tGLenum, 'source'), P(tGLenum, 'type'), P(tGLenum, 'severity'), P(tGLsizei, 'count'), P(tPointer, 'ids'), P(tGLboolean, 'enabled')])
#Func('glDebugMessageCallback', [P(tGLDEBUGPROC, 'callback'), P(tPointer, 'userP')])
#Func('glGetDebugMessageLog', [P(tGLuint, 'count'), P(tGLsizei, 'bufSize'), P(tMutablePointer, 'sources'), P(tMutablePointer, 'types'), P(tMutablePointer, 'ids'), P(tMutablePointer, 'severities'), P(tMutablePointer, 'lengths'), P(tMutableString, 'messageLog')], tGLuint)
#Func('glGetObjectLabel', [P(tGLenum, 'identifier'), P(tGLuint, 'name'), P(tGLsizei, 'bufSize'), P(tMutablePointer, 'length'), P(tMutableString, 'label')])
#Func('glObjectPtrLabel', [P(tPointer, 'ptr'), P(tGLsizei, 'length'), P(tString, 'label')])
#Func('glGetObjectPtrLabel', [P(tPointer, 'ptr'), P(tGLsizei, 'bufSize'), P(tMutablePointer, 'length'), P(tMutableString, 'label')])
#Func('glGetPointerv', [P(tGLenum, 'pname'), P(tMutablePointer, 'params')])
#Func('glGetPointerv', [P(tGLenum, 'pname'), P(tMutablePointer, 'params')])
#Func('glBufferStorage', [P(tGLenum, 'target'), P(tGLsizeiptr, 'size'), P(tPointer, 'data'), P(tGLbitfield, 'flags')])
#Func('glClearTexImage', [P(tGLuint, 'texture'), P(tGLint, 'level'), P(tGLenum, 'format'), P(tGLenum, 'type'), P(tPointer, 'data')])
#Func('glClearTexSubImage', [P(tGLuint, 'texture'), P(tGLint, 'level'), P(tGLint, 'xoffset'), P(tGLint, 'yoffset'), P(tGLint, 'zoffset'), P(tGLsizei, 'width'), P(tGLsizei, 'height'), P(tGLsizei, 'depth'), P(tGLenum, 'format'), P(tGLenum, 'type'), P(tPointer, 'data')])
#Func('glBindBuffersBase', [P(tGLenum, 'target'), P(tGLuint, 'first'), P(tGLsizei, 'count'), P(tPointer, 'buffers')])
#Func('glBindBuffersRange', [P(tGLenum, 'target'), P(tGLuint, 'first'), P(tGLsizei, 'count'), P(tPointer, 'buffers'), P(tPointer, 'offsets'), P(tPointer, 'sizes')])
Func('glBindTextures', [P(tGLuint, 'first'), P(tGLsizei, 'count'), P(tGLuint, 'textures', 'count')])
Func('glBindSamplers', [P(tGLuint, 'first'), P(tGLsizei, 'count'), P(tGLuint, 'samplers', 'count')])
Func('glBindImageTextures', [P(tGLuint, 'first'), P(tGLsizei, 'count'), P(tGLuint, 'textures', 'count')])
Func('glBindVertexBuffers', [P(tGLuint, 'first'), P(tGLsizei, 'count'), P(tGLuint, 'buffers', 'count'), P(tGLuintptr, 'offsets', 'count'), P(tGLsizei, 'strides', 'count')])
Func('glCreateTransformFeedbacks', [P(tGLsizei, 'n'), P(tGLuint, 'ids', 'n')])
#Func('glGetTransformFeedbackiv', [P(tGLuint, 'xfb'), P(tGLenum, 'pname'), P(tMutablePointer, 'param')])
#Func('glGetTransformFeedbacki_v', [P(tGLuint, 'xfb'), P(tGLenum, 'pname'), P(tGLuint, 'index'), P(tMutablePointer, 'param')])
#Func('glGetTransformFeedbacki64_v', [P(tGLuint, 'xfb'), P(tGLenum, 'pname'), P(tGLuint, 'index'), P(tMutablePointer, 'param')])
Func('glCreateBuffers', [P(tGLsizei, 'n'), P(tGLuint, 'buffers', 'n')])
#Func('glNamedBufferStorage', [P(tGLuint, 'buffer'), P(tGLsizeiptr, 'size'), P(tPointer, 'data'), P(tGLbitfield, 'flags')])
Func('glNamedBufferData', [P(tGLuint, 'buffer'), P(tGLsizeiptr, 'size'), P(tData('size'), 'data'), P(tGLenum, 'usage')])
Func('glNamedBufferSubData', [P(tGLuint, 'buffer'), P(tGLintptr, 'offset'), P(tGLsizeiptr, 'size'), P(tData('size'), 'data')])
#Func('glClearNamedBufferData', [P(tGLuint, 'buffer'), P(tGLenum, 'internalformat'), P(tGLenum, 'format'), P(tGLenum, 'type'), P(tPointer, 'data')])
#Func('glClearNamedBufferSubData', [P(tGLuint, 'buffer'), P(tGLenum, 'internalformat'), P(tGLintptr, 'offset'), P(tGLsizeiptr, 'size'), P(tGLenum, 'format'), P(tGLenum, 'type'), P(tPointer, 'data')])
#Func('glMapNamedBuffer', [P(tGLuint, 'buffer'), P(tGLenum, 'access')], tPointer)
#Func('glMapNamedBufferRange', [P(tGLuint, 'buffer'), P(tGLintptr, 'offset'), P(tGLsizeiptr, 'length'), P(tGLbitfield, 'access')], tPointer)
#Func('glGetNamedBufferParameteriv', [P(tGLuint, 'buffer'), P(tGLenum, 'pname'), P(tMutablePointer, 'params')])
#Func('glGetNamedBufferParameteri64v', [P(tGLuint, 'buffer'), P(tGLenum, 'pname'), P(tMutablePointer, 'params')])
#Func('glGetNamedBufferPointerv', [P(tGLuint, 'buffer'), P(tGLenum, 'pname'), P(tMutablePointer, 'params')])
#Func('glGetNamedBufferSubData', [P(tGLuint, 'buffer'), P(tGLintptr, 'offset'), P(tGLsizeiptr, 'size'), P(tMutablePointer, 'data')])
#Func('glCreateFramebuffers', [P(tGLsizei, 'n'), P(tMutablePointer, 'framebuffers')])
Func('glNamedFramebufferDrawBuffers', [P(tGLuint, 'framebuffer'), P(tGLsizei, 'n'), P(tGLenum, 'bufs', 'n')])
#Func('glInvalidateNamedFramebufferData', [P(tGLuint, 'framebuffer'), P(tGLsizei, 'numAttachments'), P(tPointer, 'attachments')])
#Func('glInvalidateNamedFramebufferSubData', [P(tGLuint, 'framebuffer'), P(tGLsizei, 'numAttachments'), P(tPointer, 'attachments'), P(tGLint, 'x'), P(tGLint, 'y'), P(tGLsizei, 'width'), P(tGLsizei, 'height')])
#Func('glClearNamedFramebufferiv', [P(tGLuint, 'framebuffer'), P(tGLenum, 'buffer'), P(tGLint, 'drawbuffer'), P(tPointer, 'value')])
#Func('glClearNamedFramebufferuiv', [P(tGLuint, 'framebuffer'), P(tGLenum, 'buffer'), P(tGLint, 'drawbuffer'), P(tPointer, 'value')])
#Func('glClearNamedFramebufferfv', [P(tGLuint, 'framebuffer'), P(tGLenum, 'buffer'), P(tGLint, 'drawbuffer'), P(tPointer, 'value')])
#Func('glGetNamedFramebufferParameteriv', [P(tGLuint, 'framebuffer'), P(tGLenum, 'pname'), P(tMutablePointer, 'param')])
#Func('glGetNamedFramebufferAttachmentParameteriv', [P(tGLuint, 'framebuffer'), P(tGLenum, 'attachment'), P(tGLenum, 'pname'), P(tMutablePointer, 'params')])
Func('glCreateRenderbuffers', [P(tGLsizei, 'n'), P(tGLuint, 'renderbuffers', 'n')])
#Func('glGetNamedRenderbufferParameteriv', [P(tGLuint, 'renderbuffer'), P(tGLenum, 'pname'), P(tMutablePointer, 'params')])
Func('glCreateTextures', [P(tGLenum, 'target'), P(tGLsizei, 'n'), P(tGLuint, 'textures', 'n')])
#Func('glTextureSubImage1D', [P(tGLuint, 'texture'), P(tGLint, 'level'), P(tGLint, 'xoffset'), P(tGLsizei, 'width'), P(tGLenum, 'format'), P(tGLenum, 'type'), P(tPointer, 'pixels')])
#Func('glTextureSubImage2D', [P(tGLuint, 'texture'), P(tGLint, 'level'), P(tGLint, 'xoffset'), P(tGLint, 'yoffset'), P(tGLsizei, 'width'), P(tGLsizei, 'height'), P(tGLenum, 'format'), P(tGLenum, 'type'), P(tPointer, 'pixels')])
#Func('glTextureSubImage3D', [P(tGLuint, 'texture'), P(tGLint, 'level'), P(tGLint, 'xoffset'), P(tGLint, 'yoffset'), P(tGLint, 'zoffset'), P(tGLsizei, 'width'), P(tGLsizei, 'height'), P(tGLsizei, 'depth'), P(tGLenum, 'format'), P(tGLenum, 'type'), P(tPointer, 'pixels')])
#Func('glCompressedTextureSubImage1D', [P(tGLuint, 'texture'), P(tGLint, 'level'), P(tGLint, 'xoffset'), P(tGLsizei, 'width'), P(tGLenum, 'format'), P(tGLsizei, 'imageSize'), P(tPointer, 'data')])
#Func('glCompressedTextureSubImage2D', [P(tGLuint, 'texture'), P(tGLint, 'level'), P(tGLint, 'xoffset'), P(tGLint, 'yoffset'), P(tGLsizei, 'width'), P(tGLsizei, 'height'), P(tGLenum, 'format'), P(tGLsizei, 'imageSize'), P(tPointer, 'data')])
#Func('glCompressedTextureSubImage3D', [P(tGLuint, 'texture'), P(tGLint, 'level'), P(tGLint, 'xoffset'), P(tGLint, 'yoffset'), P(tGLint, 'zoffset'), P(tGLsizei, 'width'), P(tGLsizei, 'height'), P(tGLsizei, 'depth'), P(tGLenum, 'format'), P(tGLsizei, 'imageSize'), P(tPointer, 'data')])
#Func('glTextureParameterfv', [P(tGLuint, 'texture'), P(tGLenum, 'pname'), P(tPointer, 'param')])
#Func('glTextureParameterIiv', [P(tGLuint, 'texture'), P(tGLenum, 'pname'), P(tPointer, 'params')])
#Func('glTextureParameterIuiv', [P(tGLuint, 'texture'), P(tGLenum, 'pname'), P(tPointer, 'params')])
#Func('glTextureParameteriv', [P(tGLuint, 'texture'), P(tGLenum, 'pname'), P(tPointer, 'param')])
#Func('glGetTextureImage', [P(tGLuint, 'texture'), P(tGLint, 'level'), P(tGLenum, 'format'), P(tGLenum, 'type'), P(tGLsizei, 'bufSize'), P(tMutablePointer, 'pixels')])
#Func('glGetCompressedTextureImage', [P(tGLuint, 'texture'), P(tGLint, 'level'), P(tGLsizei, 'bufSize'), P(tMutablePointer, 'pixels')])
#Func('glGetTextureLevelParameterfv', [P(tGLuint, 'texture'), P(tGLint, 'level'), P(tGLenum, 'pname'), P(tMutablePointer, 'params')])
#Func('glGetTextureLevelParameteriv', [P(tGLuint, 'texture'), P(tGLint, 'level'), P(tGLenum, 'pname'), P(tMutablePointer, 'params')])
#Func('glGetTextureParameterfv', [P(tGLuint, 'texture'), P(tGLenum, 'pname'), P(tMutablePointer, 'params')])
#Func('glGetTextureParameterIiv', [P(tGLuint, 'texture'), P(tGLenum, 'pname'), P(tMutablePointer, 'params')])
#Func('glGetTextureParameterIuiv', [P(tGLuint, 'texture'), P(tGLenum, 'pname'), P(tMutablePointer, 'params')])
#Func('glGetTextureParameteriv', [P(tGLuint, 'texture'), P(tGLenum, 'pname'), P(tMutablePointer, 'params')])
Func('glCreateVertexArrays', [P(tGLsizei, 'n'), P(tGLuint, 'arrays', 'n')])
Func('glVertexArrayVertexBuffers', [P(tGLuint, 'vaobj'), P(tGLuint, 'first'), P(tGLsizei, 'count'), P(tGLuint, 'buffers', 'count'), P(tGLintptr, 'offsets', 'count'), P(tGLsizei, 'strides', 'count')])
#Func('glGetVertexArrayiv', [P(tGLuint, 'vaobj'), P(tGLenum, 'pname'), P(tMutablePointer, 'param')])
#Func('glGetVertexArrayIndexediv', [P(tGLuint, 'vaobj'), P(tGLuint, 'index'), P(tGLenum, 'pname'), P(tMutablePointer, 'param')])
#Func('glGetVertexArrayIndexed64iv', [P(tGLuint, 'vaobj'), P(tGLuint, 'index'), P(tGLenum, 'pname'), P(tMutablePointer, 'param')])
Func('glCreateSamplers', [P(tGLsizei, 'n'), P(tGLuint, 'samplers', 'n')])
Func('glCreateProgramPipelines', [P(tGLsizei, 'n'), P(tGLuint, 'pipelines', 'n')])
Func('glCreateQueries', [P(tGLenum, 'target'), P(tGLsizei, 'n'), P(tGLuint, 'ids', 'n')])
#Func('glGetTextureSubImage', [P(tGLuint, 'texture'), P(tGLint, 'level'), P(tGLint, 'xoffset'), P(tGLint, 'yoffset'), P(tGLint, 'zoffset'), P(tGLsizei, 'width'), P(tGLsizei, 'height'), P(tGLsizei, 'depth'), P(tGLenum, 'format'), P(tGLenum, 'type'), P(tGLsizei, 'bufSize'), P(tMutablePointer, 'pixels')])
#Func('glGetCompressedTextureSubImage', [P(tGLuint, 'texture'), P(tGLint, 'level'), P(tGLint, 'xoffset'), P(tGLint, 'yoffset'), P(tGLint, 'zoffset'), P(tGLsizei, 'width'), P(tGLsizei, 'height'), P(tGLsizei, 'depth'), P(tGLsizei, 'bufSize'), P(tMutablePointer, 'pixels')])
#Func('glGetnCompressedTexImage', [P(tGLenum, 'target'), P(tGLint, 'lod'), P(tGLsizei, 'bufSize'), P(tMutablePointer, 'pixels')])
#Func('glGetnTexImage', [P(tGLenum, 'target'), P(tGLint, 'level'), P(tGLenum, 'format'), P(tGLenum, 'type'), P(tGLsizei, 'bufSize'), P(tMutablePointer, 'pixels')])
#Func('glGetnUniformdv', [P(tGLuint, 'program'), P(tGLint, 'location'), P(tGLsizei, 'bufSize'), P(tMutablePointer, 'params')])
#Func('glGetnUniformfv', [P(tGLuint, 'program'), P(tGLint, 'location'), P(tGLsizei, 'bufSize'), P(tMutablePointer, 'params')])
#Func('glGetnUniformiv', [P(tGLuint, 'program'), P(tGLint, 'location'), P(tGLsizei, 'bufSize'), P(tMutablePointer, 'params')])
#Func('glGetnUniformuiv', [P(tGLuint, 'program'), P(tGLint, 'location'), P(tGLsizei, 'bufSize'), P(tMutablePointer, 'params')])
#Func('glReadnPixels', [P(tGLint, 'x'), P(tGLint, 'y'), P(tGLsizei, 'width'), P(tGLsizei, 'height'), P(tGLenum, 'format'), P(tGLenum, 'type'), P(tGLsizei, 'bufSize'), P(tMutablePointer, 'data')])
Func('glDrawArrays', [P(tGLenum, 'mode'), P(tGLint, 'first'), P(tGLsizei, 'count')]).trace_epilogue_code = 'if (test_mode) test_fb("glDrawArrays");'
Func('glDrawElements', [P(tGLenum, 'mode'), P(tGLsizei, 'count'), P(tGLenum, 'type'), P(tPointer, 'indices')]).trace_epilogue_code = 'if (test_mode) test_fb("glDrawElements");'
Func('glDrawArraysInstanced', [P(tGLenum, 'mode'), P(tGLint, 'first'), P(tGLsizei, 'count'), P(tGLsizei, 'instancecount')]).trace_epilogue_code = 'if (test_mode) test_fb("glDrawArraysInstanced");'
Func('glDrawArraysInstancedBaseInstance', [P(tGLenum, 'mode'), P(tGLint, 'first'), P(tGLsizei, 'count'), P(tGLsizei, 'instancecount'), P(tGLuint, 'baseinstance')]).trace_epilogue_code = 'if (test_mode) test_fb("glDrawArraysInstancedBaseInstance");'
Func('glLinkProgram', [Param(tGLuint, 'program', None)], None).trace_extras_code = 'link_program_extras(program);'
Func('glViewport', [P(tGLint, 'x'), P(tGLint, 'y'),
                    P(tGLsizei, 'width'), P(tGLsizei, 'height')]).trace_epilogue_code = 'update_drawable_size();'

Func('glUnmapBuffer', [Param(tGLenum, 'target', None)], tGLboolean).trace_extras_code = '''GLint access;
F(glGetBufferParameteriv)(target, GL_BUFFER_ACCESS, &access);
if (access != GL_READ_ONLY) {
    GLint size;
    F(glGetBufferParameteriv)(target, GL_BUFFER_SIZE, &size);
    
    void* data = malloc(size);
    F(glGetBufferSubData)(target, 0, size, data);
    gl_add_extra("replay/glUnmapBuffer/data", size, data);
    free(data);
}
'''

glDrawableSizeWIP15('glDrawableSizeWIP15', [P(tGLsizei, 'width'), P(tGLsizei, 'height')], None)
glTestFBWIP15('glTestFBWIP15', [P(tString, 'name'), P(tData('drawable_width*drawable_height*4'), 'color'),
                                P(tData('drawable_width*drawable_height*4'), 'depth')], None)

#Func('glXGetFBConfigs', [P(tMutablePointer, 'dpy'), P(tint, 'screen'), P(tMutablePointer, 'nelements')], tPointer)
#Func('glXGetGPUIDsAMD', [P(tunsignedint, 'maxCount'), P(tMutablePointer, 'ids')], tunsignedint)
#Func('glXQueryExtension', [P(tMutablePointer, 'dpy'), P(tMutablePointer, 'errorb'), P(tMutablePointer, 'event')], tBool)
#Func('glXQueryServerString', [P(tMutablePointer, 'dpy'), P(tint, 'screen'), P(tint, 'name')], tPointer)
#Func('glXChannelRectSGIX', [P(tMutablePointer, 'display'), P(tint, 'screen'), P(tint, 'channel'), P(tint, 'x'), P(tint, 'y'), P(tint, 'w'), P(tint, 'h')], tint)
#Func('glXCopyContext', [P(tMutablePointer, 'dpy'), P(tGLXContext, 'src'), P(tGLXContext, 'dst'), P(tunsignedlong, 'mask')])
#Func('glXGetTransparentIndexSUN', [P(tMutablePointer, 'dpy'), P(tWindow, 'overlay'), P(tWindow, 'underlay'), P(tMutablePointer, 'pTransparentIndex')], tStatus)
Func('glXChooseVisual', [P(tMutablePointer, 'dpy'), P(tint, 'screen'), P(tint, 'attribList', 'glx_attrib_int_count(attribList)')], tPointer)
#Func('glXReleaseVideoImageNV', [P(tMutablePointer, 'dpy'), P(tGLXPbuffer, 'pbuf')], tint)
#Func('glXDestroyGLXPbufferSGIX', [P(tMutablePointer, 'dpy'), P(tGLXPbufferSGIX, 'pbuf')])
#Func('glXGetCurrentAssociatedContextAMD', [], tGLXContext)
#Func('glXQueryContextInfoEXT', [P(tMutablePointer, 'dpy'), P(tGLXContext, 'context'), P(tint, 'attribute'), P(tMutablePointer, 'value')], tint)
#Func('glXQueryCurrentRendererIntegerMESA', [P(tint, 'attribute'), P(tMutablePointer, 'value')], tBool)
#Func('glXDestroyPbuffer', [P(tMutablePointer, 'dpy'), P(tGLXPbuffer, 'pbuf')])
#Func('glXGetFBConfigAttribSGIX', [P(tMutablePointer, 'dpy'), P(tGLXFBConfigSGIX, 'config'), P(tint, 'attribute'), P(tMutablePointer, 'value')], tint)
#Func('glXNamedCopyBufferSubDataNV', [P(tMutablePointer, 'dpy'), P(tGLXContext, 'readCtx'), P(tGLXContext, 'writeCtx'), P(tGLuint, 'readBuffer'), P(tGLuint, 'writeBuffer'), P(tGLintptr, 'readOffset'), P(tGLintptr, 'writeOffset'), P(tGLsizeiptr, 'size')])
#Func('glXHyperpipeAttribSGIX', [P(tMutablePointer, 'dpy'), P(tint, 'timeSlice'), P(tint, 'attrib'), P(tint, 'size'), P(tMutablePointer, 'attribList')], tint)
#Func('glXDestroyGLXVideoSourceSGIX', [P(tMutablePointer, 'dpy'), P(tGLXVideoSourceSGIX, 'glxvideosource')])
#Func('glXQueryExtensionsString', [P(tMutablePointer, 'dpy'), P(tint, 'screen')], tPointer)
#Func('glXSendPbufferToVideoNV', [P(tMutablePointer, 'dpy'), P(tGLXPbuffer, 'pbuf'), P(tint, 'iBufferType'), P(tMutablePointer, 'pulCounterPbuffer'), P(tGLboolean, 'bBlock')], tint)
#Func('glXDestroyWindow', [P(tMutablePointer, 'dpy'), P(tGLXWindow, 'win')])
#Func('glXQueryHyperpipeAttribSGIX', [P(tMutablePointer, 'dpy'), P(tint, 'timeSlice'), P(tint, 'attrib'), P(tint, 'size'), P(tMutablePointer, 'returnAttribList')], tint)
#Func('glXHyperpipeConfigSGIX', [P(tMutablePointer, 'dpy'), P(tint, 'networkId'), P(tint, 'npipes'), P(tMutablePointer, 'cfg'), P(tMutablePointer, 'hpId')], tint)
#Func('glXDestroyContext', [P(tMutablePointer, 'dpy'), P(tGLXContext, 'ctx')])
#Func('glXGetCurrentDisplayEXT', [], tPointer)
#Func('glXCreateContextWithConfigSGIX', [P(tMutablePointer, 'dpy'), P(tGLXFBConfigSGIX, 'config'), P(tint, 'render_type'), P(tGLXContext, 'share_list'), P(tBool, 'direct')], tGLXContext)
#Func('glXMakeAssociatedContextCurrentAMD', [P(tGLXContext, 'ctx')], tBool)
#Func('glXMakeCurrentReadSGI', [P(tMutablePointer, 'dpy'), P(tGLXDrawable, 'draw'), P(tGLXDrawable, 'read'), P(tGLXContext, 'ctx')], tBool)
#Func('glXBindSwapBarrierSGIX', [P(tMutablePointer, 'dpy'), P(tGLXDrawable, 'drawable'), P(tint, 'barrier')])
#Func('glXFreeContextEXT', [P(tMutablePointer, 'dpy'), P(tGLXContext, 'context')])
#Func('glXDestroyHyperpipeConfigSGIX', [P(tMutablePointer, 'dpy'), P(tint, 'hpId')], tint)
#Func('glXWaitForSbcOML', [P(tMutablePointer, 'dpy'), P(tGLXDrawable, 'drawable'), P(tint64_t, 'target_sbc'), P(tMutablePointer, 'ust'), P(tMutablePointer, 'msc'), P(tMutablePointer, 'sbc')], tBool)
#Func('glXGetSelectedEvent', [P(tMutablePointer, 'dpy'), P(tGLXDrawable, 'draw'), P(tMutablePointer, 'event_mask')])
#Func('glXEnumerateVideoCaptureDevicesNV', [P(tMutablePointer, 'dpy'), P(tint, 'screen'), P(tMutablePointer, 'nelements')], tPointer)
#Func('glXCreateGLXPixmapMESA', [P(tMutablePointer, 'dpy'), P(tMutablePointer, 'visual'), P(tPixmap, 'pixmap'), P(tColormap, 'cmap')], tGLXPixmap)
#Func('glXCreateWindow', [P(tMutablePointer, 'dpy'), P(tGLXFBConfig, 'config'), P(tWindow, 'win'), P(tPointer, 'attrib_list')], tGLXWindow)
#Func('glXGetCurrentDrawable', [], tGLXDrawable)
#Func('glXQueryHyperpipeBestAttribSGIX', [P(tMutablePointer, 'dpy'), P(tint, 'timeSlice'), P(tint, 'attrib'), P(tint, 'size'), P(tMutablePointer, 'attribList'), P(tMutablePointer, 'returnAttribList')], tint)
glXGetProcAddressFunc('glXGetProcAddress', [P(tString, 'procName')], t__GLXextFuncPtr)
#Func('glXCushionSGI', [P(tMutablePointer, 'dpy'), P(tWindow, 'window'), P(tfloat, 'cushion')])
#Func('glXMakeContextCurrent', [P(tMutablePointer, 'dpy'), P(tGLXDrawable, 'draw'), P(tGLXDrawable, 'read'), P(tGLXContext, 'ctx')], tBool)
#Func('glXSwapIntervalSGI', [P(tint, 'interval')], tint)
#Func('glXBindTexImageEXT', [P(tMutablePointer, 'dpy'), P(tGLXDrawable, 'drawable'), P(tint, 'buffer'), P(tPointer, 'attrib_list')])
#Func('glXReleaseVideoCaptureDeviceNV', [P(tMutablePointer, 'dpy'), P(tGLXVideoCaptureDeviceNV, 'device')])
#Func('glXGetCurrentContext', [], tGLXContext)
#Func('glXQueryMaxSwapGroupsNV', [P(tMutablePointer, 'dpy'), P(tint, 'screen'), P(tMutablePointer, 'maxGroups'), P(tMutablePointer, 'maxBarriers')], tBool)
#Func('glXQueryHyperpipeConfigSGIX', [P(tMutablePointer, 'dpy'), P(tint, 'hpId'), P(tMutablePointer, 'npipes')], tPointer)
#Func('glXWaitVideoSyncSGI', [P(tint, 'divisor'), P(tint, 'remainder'), P(tMutablePointer, 'count')], tint)
#Func('glXGetConfig', [P(tMutablePointer, 'dpy'), P(tMutablePointer, 'visual'), P(tint, 'attrib'), P(tMutablePointer, 'value')], tint)
#Func('glXCreateGLXPbufferSGIX', [P(tMutablePointer, 'dpy'), P(tGLXFBConfigSGIX, 'config'), P(tunsignedint, 'width'), P(tunsignedint, 'height'), P(tMutablePointer, 'attrib_list')], tGLXPbufferSGIX)
#Func('glXQueryVideoCaptureDeviceNV', [P(tMutablePointer, 'dpy'), P(tGLXVideoCaptureDeviceNV, 'device'), P(tint, 'attribute'), P(tMutablePointer, 'value')], tint)
#Func('glXGetVideoSyncSGI', [P(tMutablePointer, 'count')], tint)
#Func('glXQueryDrawable', [P(tMutablePointer, 'dpy'), P(tGLXDrawable, 'draw'), P(tint, 'attribute'), P(tMutablePointer, 'value')])
#Func('glXLockVideoCaptureDeviceNV', [P(tMutablePointer, 'dpy'), P(tGLXVideoCaptureDeviceNV, 'device')])
#Func('glXGetCurrentDisplay', [], tPointer)
#Func('glXBlitContextFramebufferAMD', [P(tGLXContext, 'dstCtx'), P(tGLint, 'srcX0'), P(tGLint, 'srcY0'), P(tGLint, 'srcX1'), P(tGLint, 'srcY1'), P(tGLint, 'dstX0'), P(tGLint, 'dstY0'), P(tGLint, 'dstX1'), P(tGLint, 'dstY1'), P(tGLbitfield, 'mask'), P(tGLenum, 'filter')])
#Func('glXGetClientString', [P(tMutablePointer, 'dpy'), P(tint, 'name')], tPointer)
#Func('glXGetMscRateOML', [P(tMutablePointer, 'dpy'), P(tGLXDrawable, 'drawable'), P(tMutablePointer, 'numerator'), P(tMutablePointer, 'denominator')], tBool)
#Func('glXGetCurrentReadDrawableSGI', [], tGLXDrawable)
#Func('glXReleaseBuffersMESA', [P(tMutablePointer, 'dpy'), P(tGLXDrawable, 'drawable')], tBool)
#Func('glXGetFBConfigAttrib', [P(tMutablePointer, 'dpy'), P(tGLXFBConfig, 'config'), P(tint, 'attribute'), P(tMutablePointer, 'value')], tint)
#Func('glXDestroyGLXPixmap', [P(tMutablePointer, 'dpy'), P(tGLXPixmap, 'pixmap')])
#Func('glXQueryVersion', [P(tMutablePointer, 'dpy'), P(tMutablePointer, 'maj'), P(tMutablePointer, 'min')], tBool)
#Func('glXGetAGPOffsetMESA', [P(tPointer, 'pointer')], tunsignedint)
#Func('glXWaitForMscOML', [P(tMutablePointer, 'dpy'), P(tGLXDrawable, 'drawable'), P(tint64_t, 'target_msc'), P(tint64_t, 'divisor'), P(tint64_t, 'remainder'), P(tMutablePointer, 'ust'), P(tMutablePointer, 'msc'), P(tMutablePointer, 'sbc')], tBool)
#Func('glXQueryMaxSwapBarriersSGIX', [P(tMutablePointer, 'dpy'), P(tint, 'screen'), P(tMutablePointer, 'max')], tBool)
#Func('glXCreateGLXPixmap', [P(tMutablePointer, 'dpy'), P(tMutablePointer, 'visual'), P(tPixmap, 'pixmap')], tGLXPixmap)
#Func('glXBindChannelToWindowSGIX', [P(tMutablePointer, 'display'), P(tint, 'screen'), P(tint, 'channel'), P(tWindow, 'window')], tint)
#Func('glXChooseFBConfigSGIX', [P(tMutablePointer, 'dpy'), P(tint, 'screen'), P(tMutablePointer, 'attrib_list'), P(tMutablePointer, 'nelements')], tPointer)
#Func('glXSelectEventSGIX', [P(tMutablePointer, 'dpy'), P(tGLXDrawable, 'drawable'), P(tunsignedlong, 'mask')])
#Func('glXGetContextGPUIDAMD', [P(tGLXContext, 'ctx')], tunsignedint)
#Func('glXDestroyPixmap', [P(tMutablePointer, 'dpy'), P(tGLXPixmap, 'pixmap')])
#Func('glXCreateNewContext', [P(tMutablePointer, 'dpy'), P(tGLXFBConfig, 'config'), P(tint, 'render_type'), P(tGLXContext, 'share_list'), P(tBool, 'direct')], tGLXContext)
#Func('glXQueryGLXPbufferSGIX', [P(tMutablePointer, 'dpy'), P(tGLXPbufferSGIX, 'pbuf'), P(tint, 'attribute'), P(tMutablePointer, 'value')], tint)
#Func('glXCopySubBufferMESA', [P(tMutablePointer, 'dpy'), P(tGLXDrawable, 'drawable'), P(tint, 'x'), P(tint, 'y'), P(tint, 'width'), P(tint, 'height')])
#Func('glXQueryFrameCountNV', [P(tMutablePointer, 'dpy'), P(tint, 'screen'), P(tMutablePointer, 'count')], tBool)
#Func('glXGetCurrentReadDrawable', [], tGLXDrawable)
#Func('glXResetFrameCountNV', [P(tMutablePointer, 'dpy'), P(tint, 'screen')], tBool)
#Func('glXCreateAssociatedContextAttribsAMD', [P(tunsignedint, 'id'), P(tGLXContext, 'share_context'), P(tPointer, 'attribList')], tGLXContext)
Func('glXCreateContextAttribsARB', [P(tMutablePointer, 'dpy'), P(tGLXFBConfig, 'config'), P(tGLXContext, 'share_context'), P(tBool, 'direct'), P(tint, 'attrib_list', 'glx_attrib_int_count(attrib_list)')], tGLXContext)
#Func('glXDelayBeforeSwapNV', [P(tMutablePointer, 'dpy'), P(tGLXDrawable, 'drawable'), P(tGLfloat, 'seconds')], tBool)
#Func('glXImportContextEXT', [P(tMutablePointer, 'dpy'), P(tGLXContextID, 'contextID')], tGLXContext)
#Func('glXSelectEvent', [P(tMutablePointer, 'dpy'), P(tGLXDrawable, 'draw'), P(tunsignedlong, 'event_mask')])
#Func('glXGetVideoInfoNV', [P(tMutablePointer, 'dpy'), P(tint, 'screen'), P(tGLXVideoDeviceNV, 'VideoDevice'), P(tMutablePointer, 'pulCounterOutputPbuffer'), P(tMutablePointer, 'pulCounterOutputVideo')], tint)
#Func('glXGetSyncValuesOML', [P(tMutablePointer, 'dpy'), P(tGLXDrawable, 'drawable'), P(tMutablePointer, 'ust'), P(tMutablePointer, 'msc'), P(tMutablePointer, 'sbc')], tBool)
#Func('glXDeleteAssociatedContextAMD', [P(tGLXContext, 'ctx')], tBool)
glXGetProcAddressFunc('glXGetProcAddressARB', [P(tString, 'procName')], t__GLXextFuncPtr)
#Func('glXEnumerateVideoDevicesNV', [P(tMutablePointer, 'dpy'), P(tint, 'screen'), P(tMutablePointer, 'nelements')], tPointer)
#Func('glXCreateContext', [P(tMutablePointer, 'dpy'), P(tMutablePointer, 'vis'), P(tGLXContext, 'shareList'), P(tBool, 'direct')], tGLXContext)
#Func('glXReleaseTexImageEXT', [P(tMutablePointer, 'dpy'), P(tGLXDrawable, 'drawable'), P(tint, 'buffer')])
#Func('glXJoinSwapGroupNV', [P(tMutablePointer, 'dpy'), P(tGLXDrawable, 'drawable'), P(tGLuint, 'group')], tBool)
#Func('glXCreateAssociatedContextAMD', [P(tunsignedint, 'id'), P(tGLXContext, 'share_list')], tGLXContext)
#Func('glXBindHyperpipeSGIX', [P(tMutablePointer, 'dpy'), P(tint, 'hpId')], tint)
#Func('glXGetSelectedEventSGIX', [P(tMutablePointer, 'dpy'), P(tGLXDrawable, 'drawable'), P(tMutablePointer, 'mask')])
#Func('glXChooseFBConfig', [P(tMutablePointer, 'dpy'), P(tint, 'screen'), P(tPointer, 'attrib_list'), P(tMutablePointer, 'nelements')], tPointer)
#Func('glXIsDirect', [P(tMutablePointer, 'dpy'), P(tGLXContext, 'ctx')], tBool)
#Func('glXReleaseVideoDeviceNV', [P(tMutablePointer, 'dpy'), P(tint, 'screen'), P(tGLXVideoDeviceNV, 'VideoDevice')], tint)
#Func('glXGetGPUInfoAMD', [P(tunsignedint, 'id'), P(tint, 'property'), P(tGLenum, 'dataType'), P(tunsignedint, 'size'), P(tMutablePointer, 'data')], tint)
#Func('glXSwapBuffersMscOML', [P(tMutablePointer, 'dpy'), P(tGLXDrawable, 'drawable'), P(tint64_t, 'target_msc'), P(tint64_t, 'divisor'), P(tint64_t, 'remainder')], tint64_t)
#Func('glXGetContextIDEXT', [P(tGLXContext, 'context')], tGLXContextID)
#Func('glXJoinSwapGroupSGIX', [P(tMutablePointer, 'dpy'), P(tGLXDrawable, 'drawable'), P(tGLXDrawable, 'member')])
#Func('glXQueryContext', [P(tMutablePointer, 'dpy'), P(tGLXContext, 'ctx'), P(tint, 'attribute'), P(tMutablePointer, 'value')], tint)
#Func('glXWaitGL', [])
#Func('glXQueryCurrentRendererStringMESA', [P(tint, 'attribute')], tPointer)
#Func('glXCopyBufferSubDataNV', [P(tMutablePointer, 'dpy'), P(tGLXContext, 'readCtx'), P(tGLXContext, 'writeCtx'), P(tGLenum, 'readTarget'), P(tGLenum, 'writeTarget'), P(tGLintptr, 'readOffset'), P(tGLintptr, 'writeOffset'), P(tGLsizeiptr, 'size')])
Func('glXSwapBuffers', [P(tMutablePointer, 'dpy'), P(tGLXDrawable, 'drawable')]).trace_epilogue_code = 'update_drawable_size();\nif (test_mode) test_fb("glXSwapBuffers");'
#Func('glXWaitX', [])
#Func('glXQueryHyperpipeNetworkSGIX', [P(tMutablePointer, 'dpy'), P(tMutablePointer, 'npipes')], tPointer)
#Func('glXGetVideoDeviceNV', [P(tMutablePointer, 'dpy'), P(tint, 'screen'), P(tint, 'numVideoDevices'), P(tMutablePointer, 'pVideoDevice')], tint)
#Func('glXQueryChannelRectSGIX', [P(tMutablePointer, 'display'), P(tint, 'screen'), P(tint, 'channel'), P(tMutablePointer, 'dx'), P(tMutablePointer, 'dy'), P(tMutablePointer, 'dw'), P(tMutablePointer, 'dh')], tint)
#Func('glXQueryChannelDeltasSGIX', [P(tMutablePointer, 'display'), P(tint, 'screen'), P(tint, 'channel'), P(tMutablePointer, 'x'), P(tMutablePointer, 'y'), P(tMutablePointer, 'w'), P(tMutablePointer, 'h')], tint)
#Func('glXBindVideoDeviceNV', [P(tMutablePointer, 'dpy'), P(tunsignedint, 'video_slot'), P(tunsignedint, 'video_device'), P(tPointer, 'attrib_list')], tint)
#Func('glXQueryRendererIntegerMESA', [P(tMutablePointer, 'dpy'), P(tint, 'screen'), P(tint, 'renderer'), P(tint, 'attribute'), P(tMutablePointer, 'value')], tBool)
#Func('glXQuerySwapGroupNV', [P(tMutablePointer, 'dpy'), P(tGLXDrawable, 'drawable'), P(tMutablePointer, 'group'), P(tMutablePointer, 'barrier')], tBool)
#Func('glXCreatePbuffer', [P(tMutablePointer, 'dpy'), P(tGLXFBConfig, 'config'), P(tPointer, 'attrib_list')], tGLXPbuffer)
#Func('glXUseXFont', [P(tFont, 'font'), P(tint, 'first'), P(tint, 'count'), P(tint, 'list')])
#Func('glXGetVisualFromFBConfig', [P(tMutablePointer, 'dpy'), P(tGLXFBConfig, 'config')], tPointer)

#TODO
Func('glXMakeCurrent', [Param(tMutablePointer, 'dpy', None), Param(tGLXDrawable, 'drawable', None), Param(tGLXContext, 'ctx', None)], tBool).trace_epilogue_code = 'current_limits=ctx?&gl30_limits : NULL; reset_gl_funcs(); update_drawable_size();'
