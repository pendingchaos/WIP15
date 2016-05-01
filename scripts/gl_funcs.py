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
    
    def gen_write_code(self, var_name, array_count=None, group=None):
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
        return res + self._gen_group_write_code(group)

class tBufData(tData):
    def gen_write_code(self, var_name, array_count=None, group=None):
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
        return res + self._gen_group_write_code(group)

class tShdrSrc(Type):
    def gen_type_code(self, var_name='', array_count=None):
        return 'char** %s' % var_name
    
    def gen_write_code(self, var_name, array_count=None, group=None):
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
}''' + self._gen_group_write_code(group)
    
    def gen_write_type_code(self, array_count=None, group=False):
        return 'gl_write_type(BASE_STRING, %s, true);' % ('true' if group else 'false')

Group('InternalFormat').add('GL_DEPTH_COMPONENT', 0x1902, (1, 0))\
                       .add('GL_DEPTH_STENCIL', 0x84F9, (1, 0))\
                       .add('GL_RED', 0x1903, (1, 0))\
                       .add('GL_RG', 0x8227, (3, 0))\
                       .add('GL_RGB', 0x1907, (1, 0))\
                       .add('GL_RGBA', 0x1908, (1, 0))\
                       .add('GL_R8', 0x8229, (3, 0))\
                       .add('GL_R8_SNORM', 0x8F94, (3, 1))\
                       .add('GL_R16', 0x822A, (3, 0))\
                       .add('GL_R16_SNORM', 0x8F98, (3, 1))\
                       .add('GL_RG8', 0x822B, (3, 0))\
                       .add('GL_RG8_SNORM', 0x8F95, (3, 1))\
                       .add('GL_RG16', 0x822C, (3, 0))\
                       .add('GL_RG16_SNORM', 0x8F99, (3, 1))\
                       .add('GL_R3_G3_B2', 0x2A10, (1, 1))\
                       .add('GL_RGB4', 0x804F, (1, 1))\
                       .add('GL_RGB5', 0x8050, (1, 1))\
                       .add('GL_RGB8', 0x8051, (1, 1))\
                       .add('GL_RGB8_SNORM', 0x8F96, (3, 1))\
                       .add('GL_RGB10', 0x8052, (1, 1))\
                       .add('GL_RGB12', 0x8053, (1, 1))\
                       .add('GL_RGB16', 0x8054, (1, 1))\
                       .add('GL_RGB16_SNORM', 0x8F9A, (3, 1))\
                       .add('GL_RGBA2', 0x8055, (1, 1))\
                       .add('GL_RGBA4', 0x8056, (1, 1))\
                       .add('GL_RGB5_A1', 0x8057, (1, 1))\
                       .add('GL_RGBA8', 0x8058, (1, 1))\
                       .add('GL_RGBA8_SNORM', 0x8F97, (3, 1))\
                       .add('GL_RGB10_A2', 0x8059, (1, 1))\
                       .add('GL_RGB10_A2UI', 0x906F, (3, 3))\
                       .add('GL_RGBA12', 0x805A, (1, 1))\
                       .add('GL_RGBA16', 0x805B, (1, 1))\
                       .add('GL_RGBA16_SNORM', 0x8F9B, (3, 1))\
                       .add('GL_SRGB8', 0x8C41, (2, 1))\
                       .add('GL_SRGB8_ALPHA8', 0x8C43, (2, 1))\
                       .add('GL_R16F', 0x822D, (3, 0))\
                       .add('GL_RG16F', 0x822F, (3, 0))\
                       .add('GL_RGB16F', 0x881B, (3, 0))\
                       .add('GL_RGBA16F', 0x881A, (3, 0))\
                       .add('GL_R32F', 0x822E, (3, 0))\
                       .add('GL_RG32F', 0x8230, (3, 0))\
                       .add('GL_RGB32F', 0x8815, (3, 0))\
                       .add('GL_RGBA32F', 0x8814, (3, 0))\
                       .add('GL_R11F_G11F_B10F', 0x8C3A, (3, 0))\
                       .add('GL_RGB9_E5', 0x8C3D, (3, 0))\
                       .add('GL_R8I', 0x8231, (3, 0))\
                       .add('GL_R8UI', 0x8232, (3, 0))\
                       .add('GL_R16I', 0x8233, (3, 0))\
                       .add('GL_R16UI', 0x8234, (3, 0))\
                       .add('GL_R32I', 0x8235, (3, 0))\
                       .add('GL_R32UI', 0x8236, (3, 0))\
                       .add('GL_RG8I', 0x8237, (3, 0))\
                       .add('GL_RG8UI', 0x8238, (3, 0))\
                       .add('GL_RG16I', 0x8239, (3, 0))\
                       .add('GL_RG16UI', 0x823A, (3, 0))\
                       .add('GL_RG32I', 0x823B, (3, 0))\
                       .add('GL_RG32UI', 0x823C, (3, 0))\
                       .add('GL_RGB8I', 0x8D8F, (3, 0))\
                       .add('GL_RGB8UI', 0x8D7D, (3, 0))\
                       .add('GL_RGB16I', 0x8D89, (3, 0))\
                       .add('GL_RGB16UI', 0x8D77, (3, 0))\
                       .add('GL_RGB32I', 0x8D83, (3, 0))\
                       .add('GL_RGB32UI', 0x8D71, (3, 0))\
                       .add('GL_RGBA8I', 0x8D8E, (3, 0))\
                       .add('GL_RGBA8UI', 0x8D7C, (3, 0))\
                       .add('GL_RGBA16I', 0x8D88, (3, 0))\
                       .add('GL_RGBA16UI', 0x8D76, (3, 0))\
                       .add('GL_RGBA32I', 0x8D82, (3, 0))\
                       .add('GL_RGBA32UI', 0x8D70, (3, 0))\
                       .add('GL_COMPRESSED_RED', 0x8225, (3, 0))\
                       .add('GL_COMPRESSED_RG', 0x8226, (3, 0))\
                       .add('GL_COMPRESSED_RGB', 0x84ED, (1, 3))\
                       .add('GL_COMPRESSED_RGBA', 0x84EE, (1, 3))\
                       .add('GL_COMPRESSED_SRGB', 0x8C48, (2, 1))\
                       .add('GL_COMPRESSED_SRGB_ALPHA', 0x8C49, (2, 1))\
                       .add('GL_COMPRESSED_RED_RGTC1', 0x8DBB, (3, 0))\
                       .add('GL_COMPRESSED_SIGNED_RED_RGTC1', 0x8DBC, (3, 0))\
                       .add('GL_COMPRESSED_RG_RGTC2', 0x8DBD, (3, 0))\
                       .add('GL_COMPRESSED_SIGNED_RG_RGTC2', 0x8DBE, (3, 0))\
                       .add('GL_COMPRESSED_RGBA_BPTC_UNORM', 0x8E8C, (4, 2))\
                       .add('GL_COMPRESSED_SRGB_ALPHA_BPTC_UNORM', 0x8E8D, (4, 2))\
                       .add('GL_COMPRESSED_RGB_BPTC_SIGNED_FLOAT', 0x8E8E, (4, 2))\
                       .add('GL_COMPRESSED_RGB_BPTC_UNSIGNED_FLOAT', 0x8E8F, (4, 2))

#Func((1, 0), 'glTexParameterfv', [P(tGLenum, 'target'), P(tGLenum, 'pname'), P(tPointer, 'params')])
#Func((1, 0), 'glTexParameteriv', [P(tGLenum, 'target'), P(tGLenum, 'pname'), P(tPointer, 'params')])

Func((1, 0), 'glTexImage1D', [P(tGLenum, 'target', None, 'TextureTarget'), P(tGLint, 'level'),
                              P(tGLint, 'internalformat', None, 'InternalFormat'), P(tGLsizei, 'width'),
                              P(tGLint, 'border'), P(tGLenum, 'format', None, 'PixelFormat'),
                              P(tGLenum, 'type', None, 'PixelType'), P(tTexImageData(1), 'pixels')])

Func((1, 0), 'glTexImage2D', [P(tGLenum, 'target', None, 'TextureTarget'), P(tGLint, 'level'),
                      P(tGLint, 'internalformat', None, 'InternalFormat'), P(tGLsizei, 'width'),
                      P(tGLsizei, 'height'), P(tGLint, 'border'), P(tGLenum, 'format', None, 'PixelFormat'),
                      P(tGLenum, 'type', None, 'PixelType'), P(tTexImageData(2), 'pixels')])

#Func((1, 0), 'glReadPixels', [P(tGLint, 'x'), P(tGLint, 'y'), P(tGLsizei, 'width'), P(tGLsizei, 'height'), P(tGLenum, 'format'), P(tGLenum, 'type'), P(tMutablePointer, 'pixels')])
#Func((1, 0), 'glGetBooleanv', [P(tGLenum, 'pname'), P(tMutablePointer, 'data')])
#Func((1, 0), 'glGetDoublev', [P(tGLenum, 'pname'), P(tMutablePointer, 'data')])
#Func((1, 0), 'glGetFloatv', [P(tGLenum, 'pname'), P(tMutablePointer, 'data')])
#Func((1, 0), 'glGetIntegerv', [P(tGLenum, 'pname'), P(tMutablePointer, 'data')])
#Func((1, 0), 'glGetString', [P(tGLenum, 'name')], tPointer)
#Func((1, 0), 'glGetTexImage', [P(tGLenum, 'target'), P(tGLint, 'level'), P(tGLenum, 'format'), P(tGLenum, 'type'), P(tMutablePointer, 'pixels')])
#Func((1, 0), 'glGetTexParameterfv', [P(tGLenum, 'target'), P(tGLenum, 'pname'), P(tMutablePointer, 'params')])
#Func((1, 0), 'glGetTexParameteriv', [P(tGLenum, 'target'), P(tGLenum, 'pname'), P(tMutablePointer, 'params')])
#Func((1, 0), 'glGetTexLevelParameterfv', [P(tGLenum, 'target'), P(tGLint, 'level'), P(tGLenum, 'pname'), P(tMutablePointer, 'params')])
#Func((1, 0), 'glGetTexLevelParameteriv', [P(tGLenum, 'target'), P(tGLint, 'level'), P(tGLenum, 'pname'), P(tMutablePointer, 'params')])

Func((1, 1), 'glTexSubImage1D', [P(tGLenum, 'target', None, 'TextureTarget'), P(tGLint, 'level'),
                                 P(tGLint, 'xoffset'), P(tGLsizei, 'width'),
                                 P(tGLenum, 'format', None, 'PixelFormat'),
                                 P(tGLenum, 'type', None, 'PixelType'), P(tTexImageData(1), 'pixels')])

Func((1, 1), 'glTexSubImage2D', [P(tGLenum, 'target', None, 'TextureTarget'), P(tGLint, 'level'),
                                 P(tGLint, 'xoffset'), P(tGLint, 'yoffset'), P(tGLsizei, 'width'),
                                 P(tGLsizei, 'height'), P(tGLenum, 'format', None, 'PixelFormat'),
                                 P(tGLenum, 'type', None, 'PixelType'),  P(tTexImageData(2), 'pixels')])

Func((1, 1), 'glDeleteTextures', [P(tGLsizei, 'n'), P(tGLuint, 'textures', 'n')])
Func((1, 1), 'glGenTextures', [P(tGLsizei, 'n'), P(tGLuint, 'textures', 'n')])

Func((1, 2), 'glDrawRangeElements', [P(tGLenum, 'mode', None, 'PrimitiveType'), P(tGLuint, 'start'),
                             P(tGLuint, 'end'), P(tGLsizei, 'count'), P(tGLenum, 'type'),
                             P(tPointer, 'indices')])\
     .trace_epilogue_code = 'if (test_mode) test_fb("glDrawRangeElements");'

Func((1, 2), 'glTexImage3D', [P(tGLenum, 'target', None, 'TextureTarget'), P(tGLint, 'level'),
                              P(tGLint, 'internalformat', None, 'InternalFormat'), P(tGLsizei, 'width'),
                              P(tGLsizei, 'height'), P(tGLsizei, 'depth'), P(tGLint, 'border'),
                              P(tGLenum, 'format', None, 'PixelFormat'), P(tGLenum, 'type', None, 'PixelType'),
                              P(tTexImageData(3), 'pixels')])

Func((1, 2), 'glTexSubImage3D', [P(tGLenum, 'target', None, 'TextureTarget'), P(tGLint, 'level'),
                                 P(tGLint, 'xoffset'), P(tGLint, 'yoffset'), P(tGLint, 'zoffset'),
                                 P(tGLsizei, 'width'), P(tGLsizei, 'height'), P(tGLsizei, 'depth'),
                                 P(tGLenum, 'format', None, 'PixelFormat'), P(tGLenum, 'type', None, 'PixelType'),
                                 P(tTexImageData(3), 'pixels')])

Func((1, 1), 'glCopyTexImage1D', [P(tGLenum, 'target', None, 'TextureTarget'), P(tGLint, 'level'),
                                  P(tGLenum, 'internalformat', None, 'InternalFormat'), P(tGLint, 'x'), P(tGLint, 'y'),
                                  P(tGLsizei, 'width'), P(tGLint, 'border')])

Func((1, 1), 'glCopyTexImage2D', [P(tGLenum, 'target', None, 'TextureTarget'), P(tGLint, 'level'),
                                  P(tGLenum, 'internalformat', None, 'InternalFormat'), P(tGLint, 'x'),
                                  P(tGLint, 'y'), P(tGLsizei, 'width'), P(tGLsizei, 'height'), P(tGLint, 'border')])


#Func((1, 3), 'glCompressedTexImage3D', [P(tGLenum, 'target'), P(tGLint, 'level'), P(tGLenum, 'internalformat'), P(tGLsizei, 'width'), P(tGLsizei, 'height'), P(tGLsizei, 'depth'), P(tGLint, 'border'), P(tGLsizei, 'imageSize'), P(tPointer, 'data')])
#Func((1, 3), 'glCompressedTexImage2D', [P(tGLenum, 'target'), P(tGLint, 'level'), P(tGLenum, 'internalformat'), P(tGLsizei, 'width'), P(tGLsizei, 'height'), P(tGLint, 'border'), P(tGLsizei, 'imageSize'), P(tPointer, 'data')])
#Func((1, 3), 'glCompressedTexImage1D', [P(tGLenum, 'target'), P(tGLint, 'level'), P(tGLenum, 'internalformat'), P(tGLsizei, 'width'), P(tGLint, 'border'), P(tGLsizei, 'imageSize'), P(tPointer, 'data')])
#Func((1, 3), 'glCompressedTexSubImage3D', [P(tGLenum, 'target'), P(tGLint, 'level'), P(tGLint, 'xoffset'), P(tGLint, 'yoffset'), P(tGLint, 'zoffset'), P(tGLsizei, 'width'), P(tGLsizei, 'height'), P(tGLsizei, 'depth'), P(tGLenum, 'format'), P(tGLsizei, 'imageSize'), P(tPointer, 'data')])
#Func((1, 3), 'glCompressedTexSubImage2D', [P(tGLenum, 'target'), P(tGLint, 'level'), P(tGLint, 'xoffset'), P(tGLint, 'yoffset'), P(tGLsizei, 'width'), P(tGLsizei, 'height'), P(tGLenum, 'format'), P(tGLsizei, 'imageSize'), P(tPointer, 'data')])
#Func((1, 3), 'glCompressedTexSubImage1D', [P(tGLenum, 'target'), P(tGLint, 'level'), P(tGLint, 'xoffset'), P(tGLsizei, 'width'), P(tGLenum, 'format'), P(tGLsizei, 'imageSize'), P(tPointer, 'data')])
#Func((1, 3), 'glGetCompressedTexImage', [P(tGLenum, 'target'), P(tGLint, 'level'), P(tMutablePointer, 'img')])

Func((1, 4), 'glMultiDrawArrays', [P(tGLenum, 'mode', None, 'PrimitiveType'), P(tGLint, 'first', 'drawcount'),
                           P(tGLsizei, 'count', 'drawcount'), P(tGLsizei, 'drawcount')])\
     .trace_epilogue_code = 'if (test_mode) test_fb("glMultiDrawArrays");'

Func((1, 4), 'glMultiDrawElements', [P(tGLenum, 'mode', None, 'PrimitiveType'), P(tPointer, 'count', 'drawcount'),
                             P(tGLenum, 'type'), P(tPointer, 'indices', 'drawcount'),
                             P(tGLsizei, 'drawcount')])\
     .trace_epilogue_code = 'if (test_mode) test_fb("glMultiDrawElements");'

#Func((1, 4), 'glPointParameterfv', [P(tGLenum, 'pname'), P(tPointer, 'params')])
#Func((1, 4), 'glPointParameteriv', [P(tGLenum, 'pname'), P(tPointer, 'params')])
Func((1, 5), 'glGenQueries', [P(tGLsizei, 'n'), P(tGLuint, 'ids', 'n')])
Func((1, 5), 'glDeleteQueries', [P(tGLsizei, 'n'), P(tGLuint, 'ids', 'n')])
Func((1, 5), 'glGetQueryiv', [P(tGLenum, 'target'), P(tGLenum, 'pname'), P(tMutablePointer, 'params')])
#Func((1, 5), 'glGetQueryObjectiv', [P(tGLuint, 'id'), P(tGLenum, 'pname'), P(tMutablePointer, 'params')])
#Func((1, 5), 'glGetQueryObjectuiv', [P(tGLuint, 'id'), P(tGLenum, 'pname'), P(tMutablePointer, 'params')])
Func((1, 5), 'glDeleteBuffers', [P(tGLsizei, 'n'), P(tGLuint, 'buffers', 'n')])
Func((1, 5), 'glGenBuffers', [P(tGLsizei, 'n'), P(tGLuint, 'buffers', 'n')])

Func((1, 5), 'glBufferData', [P(tGLenum, 'target'), P(tGLsizeiptr, 'size'),
                      P(tBufData('size'), 'data'), P(tGLenum, 'usage')])

Func((1, 5), 'glBufferSubData', [P(tGLenum, 'target'), P(tGLintptr, 'offset'),
                         P(tGLsizeiptr, 'size'), P(tBufData('size'), 'data')])

#Func((1, 5), 'glGetBufferSubData', [P(tGLenum, 'target'), P(tGLintptr, 'offset'), P(tGLsizeiptr, 'size'), P(tMutablePointer, 'data')])
#Func((1, 5), 'glGetBufferParameteriv', [P(tGLenum, 'target'), P(tGLenum, 'pname'), P(tMutablePointer, 'params')])
#Func((1, 5), 'glGetBufferPointerv', [P(tGLenum, 'target'), P(tGLenum, 'pname'), P(tMutablePointer, 'params')])
Func((2, 0), 'glDrawBuffers', [P(tGLsizei, 'n'), P(tGLenum, 'bufs', 'n')])
#Func((2, 0), 'glGetActiveAttrib', [P(tGLuint, 'program'), P(tGLuint, 'index'), P(tGLsizei, 'bufSize'), P(tMutablePointer, 'length'), P(tMutablePointer, 'size'), P(tMutablePointer, 'type'), P(tMutableString, 'name')])
#Func((2, 0), 'glGetActiveUniform', [P(tGLuint, 'program'), P(tGLuint, 'index'), P(tGLsizei, 'bufSize'), P(tMutablePointer, 'length'), P(tMutablePointer, 'size'), P(tMutablePointer, 'type'), P(tMutableString, 'name')])
#Func((2, 0), 'glGetAttachedShaders', [P(tGLuint, 'program'), P(tGLsizei, 'maxCount'), P(tMutablePointer, 'count'), P(tMutablePointer, 'shaders')])
#Func((2, 0), 'glGetProgramiv', [P(tGLuint, 'program'), P(tGLenum, 'pname'), P(tMutablePointer, 'params')])
#Func((2, 0), 'glGetProgramInfoLog', [P(tGLuint, 'program'), P(tGLsizei, 'bufSize'), P(tMutablePointer, 'length'), P(tMutableString, 'infoLog')])
#Func((2, 0), 'glGetShaderiv', [P(tGLuint, 'shader'), P(tGLenum, 'pname'), P(tMutablePointer, 'params')])
#Func((2, 0), 'glGetShaderInfoLog', [P(tGLuint, 'shader'), P(tGLsizei, 'bufSize'), P(tMutablePointer, 'length'), P(tMutableString, 'infoLog')])
#Func((2, 0), 'glGetShaderSource', [P(tGLuint, 'shader'), P(tGLsizei, 'bufSize'), P(tMutablePointer, 'length'), P(tMutableString, 'source')])
#Func((2, 0), 'glGetUniformfv', [P(tGLuint, 'program'), P(tGLint, 'location'), P(tMutablePointer, 'params')])
#Func((2, 0), 'glGetUniformiv', [P(tGLuint, 'program'), P(tGLint, 'location'), P(tMutablePointer, 'params')])
#Func((2, 0), 'glGetVertexAttribdv', [P(tGLuint, 'index'), P(tGLenum, 'pname'), P(tMutablePointer, 'params')])
#Func((2, 0), 'glGetVertexAttribfv', [P(tGLuint, 'index'), P(tGLenum, 'pname'), P(tMutablePointer, 'params')])
#Func((2, 0), 'glGetVertexAttribiv', [P(tGLuint, 'index'), P(tGLenum, 'pname'), P(tMutablePointer, 'params')])
#Func((2, 0), 'glGetVertexAttribPointerv', [P(tGLuint, 'index'), P(tGLenum, 'pname'), P(tMutablePointer, 'pointer')])
Func((2, 0), 'glShaderSource', [P(tGLuint, 'shader'), P(tGLsizei, 'count'), P(tShdrSrc, 'string'), P(tGLint, 'length', 'length?count:0')])
Func((2, 0), 'glUniform1fv', [P(tGLint, 'location'), P(tGLsizei, 'count'), P(tGLfloat, 'value', 'count')])
Func((2, 0), 'glUniform2fv', [P(tGLint, 'location'), P(tGLsizei, 'count'), P(tGLfloat, 'value', 'count*2')])
Func((2, 0), 'glUniform3fv', [P(tGLint, 'location'), P(tGLsizei, 'count'), P(tGLfloat, 'value', 'count*3')])
Func((2, 0), 'glUniform4fv', [P(tGLint, 'location'), P(tGLsizei, 'count'), P(tGLfloat, 'value', 'count*4')])
Func((2, 0), 'glUniform1iv', [P(tGLint, 'location'), P(tGLsizei, 'count'), P(tGLint, 'value', 'count')])
Func((2, 0), 'glUniform2iv', [P(tGLint, 'location'), P(tGLsizei, 'count'), P(tGLint, 'value', 'count*2')])
Func((2, 0), 'glUniform3iv', [P(tGLint, 'location'), P(tGLsizei, 'count'), P(tGLint, 'value', 'count*3')])
Func((2, 0), 'glUniform4iv', [P(tGLint, 'location'), P(tGLsizei, 'count'), P(tGLint, 'value', 'count*4')])
Func((2, 0), 'glUniformMatrix2fv', [P(tGLint, 'location'), P(tGLsizei, 'count'), P(tGLboolean, 'transpose', None, 'Boolean'), P(tGLfloat, 'value', '4*count')])
Func((2, 0), 'glUniformMatrix3fv', [P(tGLint, 'location'), P(tGLsizei, 'count'), P(tGLboolean, 'transpose', None, 'Boolean'), P(tGLfloat, 'value', '9*count')])
Func((2, 0), 'glUniformMatrix4fv', [P(tGLint, 'location'), P(tGLsizei, 'count'), P(tGLboolean, 'transpose', None, 'Boolean'), P(tGLfloat, 'value', '16*count')])
Func((2, 0), 'glVertexAttrib1dv', [P(tGLuint, 'index'), P(tGLdouble, 'v', 1)])
Func((2, 0), 'glVertexAttrib1fv', [P(tGLuint, 'index'), P(tGLfloat, 'v', 1)])
Func((2, 0), 'glVertexAttrib1sv', [P(tGLuint, 'index'), P(tGLshort, 'v', 1)])
Func((2, 0), 'glVertexAttrib2dv', [P(tGLuint, 'index'), P(tGLdouble, 'v', 2)])
Func((2, 0), 'glVertexAttrib2fv', [P(tGLuint, 'index'), P(tGLfloat, 'v', 2)])
Func((2, 0), 'glVertexAttrib2sv', [P(tGLuint, 'index'), P(tGLshort, 'v', 2)])
Func((2, 0), 'glVertexAttrib3dv', [P(tGLuint, 'index'), P(tGLdouble, 'v', 3)])
Func((2, 0), 'glVertexAttrib3fv', [P(tGLuint, 'index'), P(tGLfloat, 'v', 3)])
Func((2, 0), 'glVertexAttrib3sv', [P(tGLuint, 'index'), P(tGLshort, 'v', 3)])
Func((2, 0), 'glVertexAttrib4Nbv', [P(tGLuint, 'index'), P(tGLbyte, 'v', 4)])
Func((2, 0), 'glVertexAttrib4Niv', [P(tGLuint, 'index'), P(tGLint, 'v', 4)])
Func((2, 0), 'glVertexAttrib4Nsv', [P(tGLuint, 'index'), P(tGLshort, 'v', 4)])
Func((2, 0), 'glVertexAttrib4Nubv', [P(tGLuint, 'index'), P(tGLubyte, 'v', 4)])
Func((2, 0), 'glVertexAttrib4Nuiv', [P(tGLuint, 'index'), P(tGLuint, 'v', 4)])
Func((2, 0), 'glVertexAttrib4Nusv', [P(tGLuint, 'index'), P(tGLushort, 'v', 4)])
Func((2, 0), 'glVertexAttrib4bv', [P(tGLuint, 'index'), P(tGLbyte, 'v', 4)])
Func((2, 0), 'glVertexAttrib4dv', [P(tGLuint, 'index'), P(tGLdouble, 'v', 4)])
Func((2, 0), 'glVertexAttrib4fv', [P(tGLuint, 'index'), P(tGLfloat, 'v', 4)])
Func((2, 0), 'glVertexAttrib4iv', [P(tGLuint, 'index'), P(tGLint, 'v', 4)])
Func((2, 0), 'glVertexAttrib4sv', [P(tGLuint, 'index'), P(tGLshort, 'v', 4)])
Func((2, 0), 'glVertexAttrib4ubv', [P(tGLuint, 'index'), P(tGLubyte, 'v', 4)])
Func((2, 0), 'glVertexAttrib4uiv', [P(tGLuint, 'index'), P(tGLuint, 'v', 4)])
Func((2, 0), 'glVertexAttrib4usv', [P(tGLuint, 'index'), P(tGLushort, 'v', )])
#Func((2, 0), 'glVertexAttribPointer', [P(tGLuint, 'index'), P(tGLint, 'size'), P(tGLenum, 'type'), P(tGLboolean, 'normalized'), P(tGLsizei, 'stride'), P(tPointer, 'pointer')])

Func((2, 1), 'glUniformMatrix2x3fv', [P(tGLint, 'location'), P(tGLsizei, 'count'),
                              P(tGLboolean, 'transpose', None, 'Boolean'),
                              P(tGLfloat, 'value', 'count*6')])

Func((2, 1), 'glUniformMatrix3x2fv', [P(tGLint, 'location'), P(tGLsizei, 'count'),
                              P(tGLboolean, 'transpose', None, 'Boolean'),
                              P(tGLfloat, 'value', 'count*6')])

Func((2, 1), 'glUniformMatrix2x4fv', [P(tGLint, 'location'), P(tGLsizei, 'count'),
                              P(tGLboolean, 'transpose', None, 'Boolean'),
                              P(tGLfloat, 'value', 'count*8')])

Func((2, 1), 'glUniformMatrix4x2fv', [P(tGLint, 'location'), P(tGLsizei, 'count'),
                              P(tGLboolean, 'transpose', None, 'Boolean'),
                              P(tGLfloat, 'value', 'count*8')])

Func((2, 1), 'glUniformMatrix3x4fv', [P(tGLint, 'location'), P(tGLsizei, 'count'),
                              P(tGLboolean, 'transpose', None, 'Boolean'),
                              P(tGLfloat, 'value', 'count*12')])

Func((2, 1), 'glUniformMatrix4x3fv', [P(tGLint, 'location'), P(tGLsizei, 'count'),
                              P(tGLboolean, 'transpose', None, 'Boolean'),
                              P(tGLfloat, 'value', 'count*12')])

#Func((3, 0), 'glGetBooleani_v', [P(tGLenum, 'target'), P(tGLuint, 'index'), P(tMutablePointer, 'data')])
#Func((3, 1), 'glGetIntegeri_v', [P(tGLenum, 'target'), P(tGLuint, 'index'), P(tMutablePointer, 'data')])
#Func((3, 0), 'glTransformFeedbackVaryings', [P(tGLuint, 'program'), P(tGLsizei, 'count'), P(tPointer, 'varyings'), P(tGLenum, 'bufferMode')])
#Func((3, 0), 'glGetTransformFeedbackVarying', [P(tGLuint, 'program'), P(tGLuint, 'index'), P(tGLsizei, 'bufSize'), P(tMutablePointer, 'length'), P(tMutablePointer, 'size'), P(tMutablePointer, 'type'), P(tMutableString, 'name')])
#Func((3, 0), 'glVertexAttribIPointer', [P(tGLuint, 'index'), P(tGLint, 'size'), P(tGLenum, 'type'), P(tGLsizei, 'stride'), P(tPointer, 'pointer')])
#Func((3, 0), 'glGetVertexAttribIiv', [P(tGLuint, 'index'), P(tGLenum, 'pname'), P(tMutablePointer, 'params')])
#Func((3, 0), 'glGetVertexAttribIuiv', [P(tGLuint, 'index'), P(tGLenum, 'pname'), P(tMutablePointer, 'params')])
Func((3, 0), 'glVertexAttribI1iv', [P(tGLuint, 'index'), P(tGLint, 'v', 1)])
Func((3, 0), 'glVertexAttribI2iv', [P(tGLuint, 'index'), P(tGLint, 'v', 2)])
Func((3, 0), 'glVertexAttribI3iv', [P(tGLuint, 'index'), P(tGLint, 'v', 3)])
Func((3, 0), 'glVertexAttribI4iv', [P(tGLuint, 'index'), P(tGLint, 'v', 4)])
Func((3, 0), 'glVertexAttribI1uiv', [P(tGLuint, 'index'), P(tGLuint, 'v', 1)])
Func((3, 0), 'glVertexAttribI2uiv', [P(tGLuint, 'index'), P(tGLuint, 'v', 2)])
Func((3, 0), 'glVertexAttribI3uiv', [P(tGLuint, 'index'), P(tGLuint, 'v', 3)])
Func((3, 0), 'glVertexAttribI4uiv', [P(tGLuint, 'index'), P(tGLuint, 'v', 4)])
Func((3, 0), 'glVertexAttribI4bv', [P(tGLuint, 'index'), P(tGLbyte, 'v', 4)])
Func((3, 0), 'glVertexAttribI4sv', [P(tGLuint, 'index'), P(tGLbyte, 'v', 4)])
Func((3, 0), 'glVertexAttribI4ubv', [P(tGLuint, 'index'), P(tGLubyte, 'v', 4)])
Func((3, 0), 'glVertexAttribI4usv', [P(tGLuint, 'index'), P(tGLubyte, 'v', 4)])
#Func((3, 0), 'glGetUniformuiv', [P(tGLuint, 'program'), P(tGLint, 'location'), P(tMutablePointer, 'params')])
Func((3, 0), 'glUniform1uiv', [P(tGLint, 'location'), P(tGLsizei, 'count'), P(tGLuint, 'value', 'count')])
Func((3, 0), 'glUniform2uiv', [P(tGLint, 'location'), P(tGLsizei, 'count'), P(tPointer, 'value', 'count*2')])
Func((3, 0), 'glUniform3uiv', [P(tGLint, 'location'), P(tGLsizei, 'count'), P(tPointer, 'value', 'count*3')])
Func((3, 0), 'glUniform4uiv', [P(tGLint, 'location'), P(tGLsizei, 'count'), P(tPointer, 'value', 'count*4')])
#Func((3, 0), 'glTexParameterIiv', [P(tGLenum, 'target'), P(tGLenum, 'pname'), P(tPointer, 'params')])
#Func((3, 0), 'glTexParameterIuiv', [P(tGLenum, 'target'), P(tGLenum, 'pname'), P(tPointer, 'params')])
#Func((3, 0), 'glGetTexParameterIiv', [P(tGLenum, 'target'), P(tGLenum, 'pname'), P(tMutablePointer, 'params')])
#Func((3, 0), 'glGetTexParameterIuiv', [P(tGLenum, 'target'), P(tGLenum, 'pname'), P(tMutablePointer, 'params')])
Func((3, 0), 'glClearBufferiv', [P(tGLenum, 'buffer'), P(tGLint, 'drawbuffer'), P(tGLint, 'value', 'buffer==GL_COLOR?4:1')])\
     .trace_epilogue_code = 'if (test_mode) test_fb("glClearBufferiv");'
Func((3, 0), 'glClearBufferuiv', [P(tGLenum, 'buffer'), P(tGLint, 'drawbuffer'), P(tGLuint, 'value', 'buffer==GL_COLOR?4:1')])\
     .trace_epilogue_code = 'if (test_mode) test_fb("glClearBufferuiv");'
Func((3, 0), 'glClearBufferfv', [P(tGLenum, 'buffer'), P(tGLint, 'drawbuffer'), P(tGLfloat, 'value', 'buffer==GL_COLOR?4:1')])\
     .trace_epilogue_code = 'if (test_mode) test_fb("glClearBufferfv");'
#Func((3, 0), 'glGetStringi', [P(tGLenum, 'name'), P(tGLuint, 'index')], tPointer)
Func((3, 0), 'glDeleteRenderbuffers', [P(tGLsizei, 'n'), P(tGLuint, 'renderbuffers', 'n')])
Func((3, 0), 'glGenRenderbuffers', [P(tGLsizei, 'n'), P(tGLuint, 'renderbuffers', 'n')])
#Func((3, 0), 'glGetRenderbufferParameteriv', [P(tGLenum, 'target'), P(tGLenum, 'pname'), P(tMutablePointer, 'params')])
Func((3, 0), 'glDeleteFramebuffers', [P(tGLsizei, 'n'), P(tGLuint, 'framebuffers', 'n')])
Func((3, 0), 'glGenFramebuffers', [P(tGLsizei, 'n'), P(tGLuint, 'framebuffers', 'n')])
#Func((3, 0), 'glGetFramebufferAttachmentParameteriv', [P(tGLenum, 'target'), P(tGLenum, 'attachment'), P(tGLenum, 'pname'), P(tMutablePointer, 'params')])
#Func((3, 0), 'glMapBufferRange', [P(tGLenum, 'target'), P(tGLintptr, 'offset'), P(tGLsizeiptr, 'length'), P(tGLbitfield, 'access')], tPointer)
Func((3, 0), 'glDeleteVertexArrays', [P(tGLsizei, 'n'), P(tGLuint, 'arrays', 'n')])
Func((3, 0), 'glGenVertexArrays', [P(tGLsizei, 'n'), P(tGLuint, 'arrays', 'n')])

Func((3, 1), 'glDrawElementsInstanced', [P(tGLenum, 'mode', None, 'PrimitiveType'),
                                 P(tGLsizei, 'count'), P(tGLenum, 'type'),
                                 P(tPointer, 'indices'), P(tGLsizei, 'instancecount')])\
     .trace_epilogue_code = 'if (test_mode) test_fb("glDrawElementsInstanced");'

#Func((3, 1), 'glGetUniformIndices', [P(tGLuint, 'program'), P(tGLsizei, 'uniformCount'), P(tPointer, 'uniformNames'), P(tMutablePointer, 'uniformIndices')])
#Func((3, 1), 'glGetActiveUniformsiv', [P(tGLuint, 'program'), P(tGLsizei, 'uniformCount'), P(tPointer, 'uniformIndices'), P(tGLenum, 'pname'), P(tMutablePointer, 'params')])
#Func((3, 1), 'glGetActiveUniformName', [P(tGLuint, 'program'), P(tGLuint, 'uniformIndex'), P(tGLsizei, 'bufSize'), P(tMutablePointer, 'length'), P(tMutableString, 'uniformName')])
#Func((3, 1), 'glGetActiveUniformBlockiv', [P(tGLuint, 'program'), P(tGLuint, 'uniformBlockIndex'), P(tGLenum, 'pname'), P(tMutablePointer, 'params')])
#Func((3, 1), 'glGetActiveUniformBlockName', [P(tGLuint, 'program'), P(tGLuint, 'uniformBlockIndex'), P(tGLsizei, 'bufSize'), P(tMutablePointer, 'length'), P(tMutableString, 'uniformBlockName')])
#Func((3, 1), 'glGetIntegeri_v', [P(tGLenum, 'target'), P(tGLuint, 'index'), P(tMutablePointer, 'data')])

Func((3, 2), 'glDrawElementsBaseVertex', [P(tGLenum, 'mode', None, 'PrimitiveType'),
                                  P(tGLsizei, 'count'), P(tGLenum, 'type'),
                                  P(tPointer, 'indices'), P(tGLint, 'basevertex')])\
     .trace_epilogue_code = 'if (test_mode) test_fb("glDrawElementsBaseVertex");'

Func((3, 2), 'glDrawRangeElementsBaseVertex', [P(tGLenum, 'mode', None, 'PrimitiveType'),
                                       P(tGLuint, 'start'), P(tGLuint, 'end'),
                                       P(tGLsizei, 'count'), P(tGLenum, 'type'),
                                       P(tPointer, 'indices'), P(tGLint, 'basevertex')])\
     .trace_epilogue_code = 'if (test_mode) test_fb("glDrawRangeElementsBaseVertex");'

Func((3, 2), 'glDrawElementsInstancedBaseVertex', [P(tGLenum, 'mode', None, 'PrimitiveType'),
                                           P(tGLsizei, 'count'), P(tGLenum, 'type'),
                                           P(tPointer, 'indices'), P(tGLsizei, 'instancecount'),
                                           P(tGLint, 'basevertex')])\
     .trace_epilogue_code = 'if (test_mode) test_fb("glDrawElementsInstancedBaseVertex");'

Func((3, 2), 'glMultiDrawElementsBaseVertex', [P(tGLenum, 'mode', None, 'PrimitiveType'),
                                       P(tGLsizei, 'count', 'drawcount'), P(tGLenum, 'type'),
                                       P(tPointer, 'indices', 'drawcount'), P(tGLsizei, 'drawcount'),
                                       P(tPointer, 'basevertex', 'drawcount')])\
     .trace_epilogue_code = 'if (test_mode) test_fb("glMultiDrawElementsBaseVertex");'

#Func((3, 2), 'glGetInteger64v', [P(tGLenum, 'pname'), P(tMutablePointer, 'data')])
#Func((3, 2), 'glGetSynciv', [P(tGLsync, 'sync'), P(tGLenum, 'pname'), P(tGLsizei, 'bufSize'), P(tMutablePointer, 'length'), P(tMutablePointer, 'values')])
#Func((3, 2), 'glGetInteger64i_v', [P(tGLenum, 'target'), P(tGLuint, 'index'), P(tMutablePointer, 'data')])
#Func((3, 2), 'glGetBufferParameteri64v', [P(tGLenum, 'target'), P(tGLenum, 'pname'), P(tMutablePointer, 'params')])
#Func((3, 2), 'glGetMultisamplefv', [P(tGLenum, 'pname'), P(tGLuint, 'index'), P(tMutablePointer, 'val')])
Func((3, 3), 'glGenSamplers', [P(tGLsizei, 'count'), P(tGLuint, 'samplers', 'count')])
Func((3, 3), 'glDeleteSamplers', [P(tGLsizei, 'count'), P(tGLuint, 'samplers', 'count')])
#Func((3, 3), 'glSamplerParameteriv', [P(tGLuint, 'sampler'), P(tGLenum, 'pname'), P(tPointer, 'param')])
#Func((3, 3), 'glSamplerParameterfv', [P(tGLuint, 'sampler'), P(tGLenum, 'pname'), P(tPointer, 'param')])
#Func((3, 3), 'glSamplerParameterIiv', [P(tGLuint, 'sampler'), P(tGLenum, 'pname'), P(tPointer, 'param')])
#Func((3, 3), 'glSamplerParameterIuiv', [P(tGLuint, 'sampler'), P(tGLenum, 'pname'), P(tPointer, 'param')])
#Func((3, 3), 'glGetSamplerParameteriv', [P(tGLuint, 'sampler'), P(tGLenum, 'pname'), P(tMutablePointer, 'params')])
#Func((3, 3), 'glGetSamplerParameterIiv', [P(tGLuint, 'sampler'), P(tGLenum, 'pname'), P(tMutablePointer, 'params')])
#Func((3, 3), 'glGetSamplerParameterfv', [P(tGLuint, 'sampler'), P(tGLenum, 'pname'), P(tMutablePointer, 'params')])
#Func((3, 3), 'glGetSamplerParameterIuiv', [P(tGLuint, 'sampler'), P(tGLenum, 'pname'), P(tMutablePointer, 'params')])
#Func((3, 3), 'glGetQueryObjecti64v', [P(tGLuint, 'id'), P(tGLenum, 'pname'), P(tMutablePointer, 'params')])
#Func((3, 3), 'glGetQueryObjectui64v', [P(tGLuint, 'id'), P(tGLenum, 'pname'), P(tMutablePointer, 'params')])
#Func((3, 3), 'glVertexAttribP1uiv', [P(tGLuint, 'index'), P(tGLenum, 'type'), P(tGLboolean, 'normalized'), P(tPointer, 'value')])
#Func((3, 3), 'glVertexAttribP2uiv', [P(tGLuint, 'index'), P(tGLenum, 'type'), P(tGLboolean, 'normalized'), P(tPointer, 'value')])
#Func((3, 3), 'glVertexAttribP3uiv', [P(tGLuint, 'index'), P(tGLenum, 'type'), P(tGLboolean, 'normalized'), P(tPointer, 'value')])
#Func((3, 3), 'glVertexAttribP4uiv', [P(tGLuint, 'index'), P(tGLenum, 'type'), P(tGLboolean, 'normalized'), P(tPointer, 'value')])
#Func((4, 0), 'glDrawArraysIndirect', [P(tGLenum, 'mode', None, 'PrimitiveType'), P(tPointer, 'indirect')]).trace_epilogue_code = 'if (test_mode) test_fb("glDrawArraysIndirect");'
#Func((4, 0), 'glDrawElementsIndirect', [P(tGLenum, 'mode', None, 'PrimitiveType'), P(tGLenum, 'type'), P(tPointer, 'indirect')]).trace_epilogue_code = 'if (test_mode) test_fb("glDrawElementsIdirect");'
Func((4, 0), 'glUniform1dv', [P(tGLint, 'location'), P(tGLsizei, 'count'), P(tGLdouble, 'value', 'count')])
Func((4, 0), 'glUniform2dv', [P(tGLint, 'location'), P(tGLsizei, 'count'), P(tGLdouble, 'value', 'count*2')])
Func((4, 0), 'glUniform3dv', [P(tGLint, 'location'), P(tGLsizei, 'count'), P(tGLdouble, 'value', 'count*3')])
Func((4, 0), 'glUniform4dv', [P(tGLint, 'location'), P(tGLsizei, 'count'), P(tGLdouble, 'value', 'count*4')])

Func((4, 0), 'glUniformMatrix2dv', [P(tGLint, 'location'), P(tGLsizei, 'count'),
                            P(tGLboolean, 'transpose', None, 'Boolean'),
                            P(tGLdouble, 'value', '4*count')])

Func((4, 0), 'glUniformMatrix3dv', [P(tGLint, 'location'), P(tGLsizei, 'count'),
                            P(tGLboolean, 'transpose', None, 'Boolean'),
                            P(tGLdouble, 'value', '9*count')])

Func((4, 0), 'glUniformMatrix4dv', [P(tGLint, 'location'), P(tGLsizei, 'count'),
                            P(tGLboolean, 'transpose', None, 'Boolean'),
                            P(tGLdouble, 'value', '16*count')])

Func((4, 0), 'glUniformMatrix2x3dv', [P(tGLint, 'location'), P(tGLsizei, 'count'),
                              P(tGLboolean, 'transpose', None, 'Boolean'),
                              P(tGLdouble, 'value', '6*count')])

Func((4, 0), 'glUniformMatrix2x4dv', [P(tGLint, 'location'), P(tGLsizei, 'count'),
                              P(tGLboolean, 'transpose', None, 'Boolean'),
                              P(tGLdouble, 'value', '8*count')])

Func((4, 0), 'glUniformMatrix3x2dv', [P(tGLint, 'location'), P(tGLsizei, 'count'),
                              P(tGLboolean, 'transpose', None, 'Boolean'),
                              P(tGLdouble, 'value', '6*count')])

Func((4, 0), 'glUniformMatrix3x4dv', [P(tGLint, 'location'), P(tGLsizei, 'count'),
                              P(tGLboolean, 'transpose', None, 'Boolean'),
                              P(tGLdouble, 'value', '12*count')])

Func((4, 0), 'glUniformMatrix4x2dv', [P(tGLint, 'location'), P(tGLsizei, 'count'),
                              P(tGLboolean, 'transpose', None, 'Boolean'),
                              P(tGLdouble, 'value', '8*count')])

Func((4, 0), 'glUniformMatrix4x3dv', [P(tGLint, 'location'), P(tGLsizei, 'count'),
                              P(tGLboolean, 'transpose', None, 'Boolean'),
                              P(tGLdouble, 'value', '12*count')])

#Func((4, 0), 'glGetUniformdv', [P(tGLuint, 'program'), P(tGLint, 'location'), P(tMutablePointer, 'params')])
#Func((4, 0), 'glGetActiveSubroutineUniformiv', [P(tGLuint, 'program'), P(tGLenum, 'shadertype'), P(tGLuint, 'index'), P(tGLenum, 'pname'), P(tMutablePointer, 'values')])
#Func((4, 0), 'glGetActiveSubroutineUniformName', [P(tGLuint, 'program'), P(tGLenum, 'shadertype'), P(tGLuint, 'index'), P(tGLsizei, 'bufsize'), P(tMutablePointer, 'length'), P(tMutableString, 'name')])
#Func((4, 0), 'glGetActiveSubroutineName', [P(tGLuint, 'program'), P(tGLenum, 'shadertype'), P(tGLuint, 'index'), P(tGLsizei, 'bufsize'), P(tMutablePointer, 'length'), P(tMutableString, 'name')])
#Func((4, 0), 'glUniformSubroutinesuiv', [P(tGLenum, 'shadertype'), P(tGLsizei, 'count'), P(tPointer, 'indices')])
#Func((4, 0), 'glGetUniformSubroutineuiv', [P(tGLenum, 'shadertype'), P(tGLint, 'location'), P(tMutablePointer, 'params')])
#Func((4, 0), 'glGetProgramStageiv', [P(tGLuint, 'program'), P(tGLenum, 'shadertype'), P(tGLenum, 'pname'), P(tMutablePointer, 'values')])
#Func((4, 0), 'glPatchParameterfv', [P(tGLenum, 'pname'), P(tPointer, 'values')])
Func((4, 0), 'glDeleteTransformFeedbacks', [P(tGLsizei, 'n'), P(tGLuint, 'ids', 'n')])
Func((4, 0), 'glGenTransformFeedbacks', [P(tGLsizei, 'n'), P(tGLuint, 'ids', 'n')])
#Func((4, 0), 'glGetQueryIndexediv', [P(tGLenum, 'target'), P(tGLuint, 'index'), P(tGLenum, 'pname'), P(tMutablePointer, 'params')])
#Func((4, 1), 'glShaderBinary', [P(tGLsizei, 'count'), P(tPointer, 'shaders'), P(tGLenum, 'binaryformat'), P(tPointer, 'binary'), P(tGLsizei, 'length')])
#Func((4, 1), 'glGetShaderPrecisionFormat', [P(tGLenum, 'shadertype'), P(tGLenum, 'precisiontype'), P(tMutablePointer, 'range'), P(tMutablePointer, 'precision')])
#Func((4, 1), 'glGetProgramBinary', [P(tGLuint, 'program'), P(tGLsizei, 'bufSize'), P(tMutablePointer, 'length'), P(tMutablePointer, 'binaryFormat'), P(tMutablePointer, 'binary')])
#Func((4, 1), 'glProgramBinary', [P(tGLuint, 'program'), P(tGLenum, 'binaryFormat'), P(tPointer, 'binary'), P(tGLsizei, 'length')])
#Func((4, 1), 'glCreateShaderProgramv', [P(tGLenum, 'type'), P(tGLsizei, 'count'), P(tPointer, 'strings')], tGLuint)
Func((4, 1), 'glDeleteProgramPipelines', [P(tGLsizei, 'n'), P(tGLuint, 'pipelines', 'n')])
Func((4, 1), 'glGenProgramPipelines', [P(tGLsizei, 'n'), P(tGLuint, 'pipelines', 'n')])
#Func((4, 1), 'glGetProgramPipelineiv', [P(tGLuint, 'pipeline'), P(tGLenum, 'pname'), P(tMutablePointer, 'params')])

Func((4, 1), 'glProgramUniform1iv', [P(tGLuint, 'program'), P(tGLint, 'location'),
                             P(tGLsizei, 'count'), P(tGLint, 'value', 'count')])

Func((4, 1), 'glProgramUniform1fv', [P(tGLuint, 'program'), P(tGLint, 'location'),
                             P(tGLsizei, 'count'), P(tGLfloat, 'value', 'count')])

Func((4, 1), 'glProgramUniform1dv', [P(tGLuint, 'program'), P(tGLint, 'location'),
                             P(tGLsizei, 'count'), P(tGLdouble, 'value', 'count')])

Func((4, 1), 'glProgramUniform1uiv', [P(tGLuint, 'program'), P(tGLint, 'location'),
                              P(tGLsizei, 'count'), P(tGLuint, 'value', 'count')])

Func((4, 1), 'glProgramUniform2iv', [P(tGLuint, 'program'), P(tGLint, 'location'),
                             P(tGLsizei, 'count'), P(tGLint, 'value', 'count*2')])

Func((4, 1), 'glProgramUniform2fv', [P(tGLuint, 'program'), P(tGLint, 'location'),
                             P(tGLsizei, 'count'), P(tGLfloat, 'value', 'count*2')])

Func((4, 1), 'glProgramUniform2dv', [P(tGLuint, 'program'), P(tGLint, 'location'),
                             P(tGLsizei, 'count'), P(tGLdouble, 'value', 'count*2')])

Func((4, 1), 'glProgramUniform2uiv', [P(tGLuint, 'program'), P(tGLint, 'location'),
                              P(tGLsizei, 'count'), P(tGLuint, 'value', 'count*2')])

Func((4, 1), 'glProgramUniform3iv', [P(tGLuint, 'program'), P(tGLint, 'location'),
                             P(tGLsizei, 'count'), P(tGLint, 'value', 'count*3')])

Func((4, 1), 'glProgramUniform3fv', [P(tGLuint, 'program'), P(tGLint, 'location'),
                             P(tGLsizei, 'count'), P(tGLfloat, 'value', 'count*3')])

Func((4, 1), 'glProgramUniform3dv', [P(tGLuint, 'program'), P(tGLint, 'location'),
                             P(tGLsizei, 'count'), P(tGLdouble, 'value', 'count*3')])

Func((4, 1), 'glProgramUniform3uiv', [P(tGLuint, 'program'), P(tGLint, 'location'),
                              P(tGLsizei, 'count'), P(tGLuint, 'value', 'count*3')])

Func((4, 1), 'glProgramUniform4iv', [P(tGLuint, 'program'), P(tGLint, 'location'),
                             P(tGLsizei, 'count'), P(tGLint, 'value', 'count*4')])

Func((4, 1), 'glProgramUniform4fv', [P(tGLuint, 'program'), P(tGLint, 'location'),
                             P(tGLsizei, 'count'), P(tGLfloat, 'value', 'count*4')])

Func((4, 1), 'glProgramUniform4dv', [P(tGLuint, 'program'), P(tGLint, 'location'),
                             P(tGLsizei, 'count'), P(tGLdouble, 'value', 'count*4')])

Func((4, 1), 'glProgramUniform4uiv', [P(tGLuint, 'program'), P(tGLint, 'location'),
                              P(tGLsizei, 'count'), P(tGLuint, 'value', 'count*4')])

Func((4, 1), 'glProgramUniformMatrix2fv', [P(tGLuint, 'program'), P(tGLint, 'location'),
                                   P(tGLsizei, 'count'), P(tGLboolean, 'transpose', None, 'Boolean'),
                                   P(tGLfloat, 'value', 'count*4')])

Func((4, 1), 'glProgramUniformMatrix3fv', [P(tGLuint, 'program'), P(tGLint, 'location'),
                                   P(tGLsizei, 'count'), P(tGLboolean, 'transpose', None, 'Boolean'),
                                   P(tGLfloat, 'value', 'count*9')])

Func((4, 1), 'glProgramUniformMatrix4fv', [P(tGLuint, 'program'), P(tGLint, 'location'),
                                   P(tGLsizei, 'count'), P(tGLboolean, 'transpose', None, 'Boolean'),
                                   P(tGLfloat, 'value', 'count*16')])

Func((4, 1), 'glProgramUniformMatrix2dv', [P(tGLuint, 'program'), P(tGLint, 'location'),
                                   P(tGLsizei, 'count'), P(tGLboolean, 'transpose', None, 'Boolean'),
                                   P(tGLdouble, 'value', 'count*4')])

Func((4, 1), 'glProgramUniformMatrix3dv', [P(tGLuint, 'program'), P(tGLint, 'location'), P(tGLsizei, 'count'), P(tGLboolean, 'transpose', None, 'Boolean'), P(tGLdouble, 'value', 'count*9')])
Func((4, 1), 'glProgramUniformMatrix4dv', [P(tGLuint, 'program'), P(tGLint, 'location'), P(tGLsizei, 'count'), P(tGLboolean, 'transpose', None, 'Boolean'), P(tGLdouble, 'value', 'count*16')])
Func((4, 1), 'glProgramUniformMatrix2x3fv', [P(tGLuint, 'program'), P(tGLint, 'location'), P(tGLsizei, 'count'), P(tGLboolean, 'transpose', None, 'Boolean'), P(tGLfloat, 'value', 'count*6')])
Func((4, 1), 'glProgramUniformMatrix3x2fv', [P(tGLuint, 'program'), P(tGLint, 'location'), P(tGLsizei, 'count'), P(tGLboolean, 'transpose', None, 'Boolean'), P(tGLfloat, 'value', 'count*6')])
Func((4, 1), 'glProgramUniformMatrix2x4fv', [P(tGLuint, 'program'), P(tGLint, 'location'), P(tGLsizei, 'count'), P(tGLboolean, 'transpose', None, 'Boolean'), P(tGLfloat, 'value', 'count*8')])
Func((4, 1), 'glProgramUniformMatrix4x2fv', [P(tGLuint, 'program'), P(tGLint, 'location'), P(tGLsizei, 'count'), P(tGLboolean, 'transpose', None, 'Boolean'), P(tGLfloat, 'value', 'count*8')])
Func((4, 1), 'glProgramUniformMatrix3x4fv', [P(tGLuint, 'program'), P(tGLint, 'location'), P(tGLsizei, 'count'), P(tGLboolean, 'transpose', None, 'Boolean'), P(tGLfloat, 'value', 'count*12')])
Func((4, 1), 'glProgramUniformMatrix4x3fv', [P(tGLuint, 'program'), P(tGLint, 'location'), P(tGLsizei, 'count'), P(tGLboolean, 'transpose', None, 'Boolean'), P(tGLfloat, 'value', 'count*12')])
Func((4, 1), 'glProgramUniformMatrix2x3dv', [P(tGLuint, 'program'), P(tGLint, 'location'), P(tGLsizei, 'count'), P(tGLboolean, 'transpose', None, 'Boolean'), P(tGLdouble, 'value', 'count*6')])
Func((4, 1), 'glProgramUniformMatrix3x2dv', [P(tGLuint, 'program'), P(tGLint, 'location'), P(tGLsizei, 'count'), P(tGLboolean, 'transpose', None, 'Boolean'), P(tGLdouble, 'value', 'count*6')])
Func((4, 1), 'glProgramUniformMatrix2x4dv', [P(tGLuint, 'program'), P(tGLint, 'location'), P(tGLsizei, 'count'), P(tGLboolean, 'transpose', None, 'Boolean'), P(tGLdouble, 'value', 'count*8')])
Func((4, 1), 'glProgramUniformMatrix4x2dv', [P(tGLuint, 'program'), P(tGLint, 'location'), P(tGLsizei, 'count'), P(tGLboolean, 'transpose', None, 'Boolean'), P(tGLdouble, 'value', 'count*8')])
Func((4, 1), 'glProgramUniformMatrix3x4dv', [P(tGLuint, 'program'), P(tGLint, 'location'), P(tGLsizei, 'count'), P(tGLboolean, 'transpose', None, 'Boolean'), P(tGLdouble, 'value', 'count*12')])
Func((4, 1), 'glProgramUniformMatrix4x3dv', [P(tGLuint, 'program'), P(tGLint, 'location'), P(tGLsizei, 'count'), P(tGLboolean, 'transpose', None, 'Boolean'), P(tGLdouble, 'value', 'count*12')])
#Func((4, 1), 'glGetProgramPipelineInfoLog', [P(tGLuint, 'pipeline'), P(tGLsizei, 'bufSize'), P(tMutablePointer, 'length'), P(tMutableString, 'infoLog')])
#Func((4, 1), 'glVertexAttribL1dv', [P(tGLuint, 'index'), P(tPointer, 'v')])
#Func((4, 1), 'glVertexAttribL2dv', [P(tGLuint, 'index'), P(tPointer, 'v')])
#Func((4, 1), 'glVertexAttribL3dv', [P(tGLuint, 'index'), P(tPointer, 'v')])
#Func((4, 1), 'glVertexAttribL4dv', [P(tGLuint, 'index'), P(tPointer, 'v')])
#Func((4, 1), 'glVertexAttribLPointer', [P(tGLuint, 'index'), P(tGLint, 'size'), P(tGLenum, 'type'), P(tGLsizei, 'stride'), P(tPointer, 'pointer')])
#Func((4, 1), 'glGetVertexAttribLdv', [P(tGLuint, 'index'), P(tGLenum, 'pname'), P(tMutablePointer, 'params')])
#Func((4, 1), 'glViewportArrayv', [P(tGLuint, 'first'), P(tGLsizei, 'count'), P(tPointer, 'v')])
#Func((4, 1), 'glViewportIndexedfv', [P(tGLuint, 'index'), P(tPointer, 'v')])
#Func((4, 1), 'glScissorArrayv', [P(tGLuint, 'first'), P(tGLsizei, 'count'), P(tPointer, 'v')])
#Func((4, 1), 'glScissorIndexedv', [P(tGLuint, 'index'), P(tPointer, 'v')])
#Func((4, 1), 'glDepthRangeArrayv', [P(tGLuint, 'first'), P(tGLsizei, 'count'), P(tPointer, 'v')])
#Func((4, 1), 'glGetFloati_v', [P(tGLenum, 'target'), P(tGLuint, 'index'), P(tMutablePointer, 'data')])
#Func((4, 1), 'glGetDoublei_v', [P(tGLenum, 'target'), P(tGLuint, 'index'), P(tMutablePointer, 'data')])
Func((4, 2), 'glDrawElementsInstancedBaseInstance', [P(tGLenum, 'mode', None, 'PrimitiveType'), P(tGLsizei, 'count'), P(tGLenum, 'type'), P(tPointer, 'indices'), P(tGLsizei, 'instancecount'), P(tGLuint, 'baseinstance')]).trace_epilogue_code = 'if (test_mode) test_fb("glDrawElementsInstancedBaseInstance");'
Func((4, 2), 'glDrawElementsInstancedBaseVertexBaseInstance', [P(tGLenum, 'mode', None, 'PrimitiveType'), P(tGLsizei, 'count'), P(tGLenum, 'type'), P(tPointer, 'indices'), P(tGLsizei, 'instancecount'), P(tGLint, 'basevertex'), P(tGLuint, 'baseinstance')]).trace_epilogue_code = 'if (test_mode) test_fb("glDrawElementsInstanceBaseVertexBaseInstance");'
#Func((4, 2), 'glGetInternalformativ', [P(tGLenum, 'target'), P(tGLenum, 'internalformat'), P(tGLenum, 'pname'), P(tGLsizei, 'bufSize'), P(tMutablePointer, 'params')])
#Func((4, 2), 'glGetActiveAtomicCounterBufferiv', [P(tGLuint, 'program'), P(tGLuint, 'bufferIndex'), P(tGLenum, 'pname'), P(tMutablePointer, 'params')])
#Func((4, 3), 'glClearBufferData', [P(tGLenum, 'target'), P(tGLenum, 'internalformat'), P(tGLenum, 'format'), P(tGLenum, 'type'), P(tPointer, 'data')])
#Func((4, 3), 'glClearBufferSubData', [P(tGLenum, 'target'), P(tGLenum, 'internalformat'), P(tGLintptr, 'offset'), P(tGLsizeiptr, 'size'), P(tGLenum, 'format'), P(tGLenum, 'type'), P(tPointer, 'data')])
#Func((4, 3), 'glGetFramebufferParameteriv', [P(tGLenum, 'target'), P(tGLenum, 'pname'), P(tMutablePointer, 'params')])
#Func((4, 3), 'glGetInternalformati64v', [P(tGLenum, 'target'), P(tGLenum, 'internalformat'), P(tGLenum, 'pname'), P(tGLsizei, 'bufSize'), P(tMutablePointer, 'params')])
#Func((4, 3), 'glInvalidateFramebuffer', [P(tGLenum, 'target'), P(tGLsizei, 'numAttachments'), P(tPointer, 'attachments')])
#Func((4, 3), 'glInvalidateSubFramebuffer', [P(tGLenum, 'target'), P(tGLsizei, 'numAttachments'), P(tPointer, 'attachments'), P(tGLint, 'x'), P(tGLint, 'y'), P(tGLsizei, 'width'), P(tGLsizei, 'height')])
#Func((4, 3), 'glMultiDrawArraysIndirect', [P(tGLenum, 'mode', None, 'Boolean'), P(tPointer, 'indirect'), P(tGLsizei, 'drawcount'), P(tGLsizei, 'stride')]).trace_epilogue_code = 'if (test_mode) test_fb("glMultiDrawArraysIndirect");'
#Func((4, 3), 'glMultiDrawElementsIndirect', [P(tGLenum, 'mode', None, 'Boolean'), P(tGLenum, 'type'), P(tPointer, 'indirect'), P(tGLsizei, 'drawcount'), P(tGLsizei, 'stride')]).trace_epilogue_code = 'if (test_mode) test_fb("glMultiDrawElementsIndirect");'
#Func((4, 3), 'glGetProgramInterfaceiv', [P(tGLuint, 'program'), P(tGLenum, 'programInterface'), P(tGLenum, 'pname'), P(tMutablePointer, 'params')])
#Func((4, 3), 'glGetProgramResourceName', [P(tGLuint, 'program'), P(tGLenum, 'programInterface'), P(tGLuint, 'index'), P(tGLsizei, 'bufSize'), P(tMutablePointer, 'length'), P(tMutableString, 'name')])
#Func((4, 3), 'glGetProgramResourceiv', [P(tGLuint, 'program'), P(tGLenum, 'programInterface'), P(tGLuint, 'index'), P(tGLsizei, 'propCount'), P(tPointer, 'props'), P(tGLsizei, 'bufSize'), P(tMutablePointer, 'length'), P(tMutablePointer, 'params')])
#Func((4, 3), 'glDebugMessageControl', [P(tGLenum, 'source'), P(tGLenum, 'type'), P(tGLenum, 'severity'), P(tGLsizei, 'count'), P(tPointer, 'ids'), P(tGLboolean, 'enabled')])
#Func((4, 3), 'glDebugMessageCallback', [P(tGLDEBUGPROC, 'callback'), P(tPointer, 'userP')])
#Func((4, 3), 'glGetDebugMessageLog', [P(tGLuint, 'count'), P(tGLsizei, 'bufSize'), P(tMutablePointer, 'sources'), P(tMutablePointer, 'types'), P(tMutablePointer, 'ids'), P(tMutablePointer, 'severities'), P(tMutablePointer, 'lengths'), P(tMutableString, 'messageLog')], tGLuint)
#Func((4, 3), 'glGetObjectLabel', [P(tGLenum, 'identifier'), P(tGLuint, 'name'), P(tGLsizei, 'bufSize'), P(tMutablePointer, 'length'), P(tMutableString, 'label')])
#Func((4, 3), 'glObjectPtrLabel', [P(tPointer, 'ptr'), P(tGLsizei, 'length'), P(tString, 'label')])
#Func((4, 3), 'glGetObjectPtrLabel', [P(tPointer, 'ptr'), P(tGLsizei, 'bufSize'), P(tMutablePointer, 'length'), P(tMutableString, 'label')])
#Func((1, 1), 'glGetPointerv', [P(tGLenum, 'pname'), P(tMutablePointer, 'params')])
#Func((1, 1), 'glGetPointerv', [P(tGLenum, 'pname'), P(tMutablePointer, 'params')])
#Func((4, 4), 'glBufferStorage', [P(tGLenum, 'target'), P(tGLsizeiptr, 'size'), P(tPointer, 'data'), P(tGLbitfield, 'flags')])
#Func((4, 4), 'glClearTexImage', [P(tGLuint, 'texture'), P(tGLint, 'level'), P(tGLenum, 'format'), P(tGLenum, 'type'), P(tPointer, 'data')])
#Func((4, 4), 'glClearTexSubImage', [P(tGLuint, 'texture'), P(tGLint, 'level'), P(tGLint, 'xoffset'), P(tGLint, 'yoffset'), P(tGLint, 'zoffset'), P(tGLsizei, 'width'), P(tGLsizei, 'height'), P(tGLsizei, 'depth'), P(tGLenum, 'format'), P(tGLenum, 'type'), P(tPointer, 'data')])
#Func((4, 4), 'glBindBuffersBase', [P(tGLenum, 'target'), P(tGLuint, 'first'), P(tGLsizei, 'count'), P(tPointer, 'buffers')])
#Func((4, 4), 'glBindBuffersRange', [P(tGLenum, 'target'), P(tGLuint, 'first'), P(tGLsizei, 'count'), P(tPointer, 'buffers'), P(tPointer, 'offsets'), P(tPointer, 'sizes')])
Func((4, 4), 'glBindTextures', [P(tGLuint, 'first'), P(tGLsizei, 'count'), P(tGLuint, 'textures', 'count')])
Func((4, 4), 'glBindSamplers', [P(tGLuint, 'first'), P(tGLsizei, 'count'), P(tGLuint, 'samplers', 'count')])
Func((4, 4), 'glBindImageTextures', [P(tGLuint, 'first'), P(tGLsizei, 'count'), P(tGLuint, 'textures', 'count')])
Func((4, 4), 'glBindVertexBuffers', [P(tGLuint, 'first'), P(tGLsizei, 'count'), P(tGLuint, 'buffers', 'count'), P(tGLuintptr, 'offsets', 'count'), P(tGLsizei, 'strides', 'count')])
Func((4, 5), 'glCreateTransformFeedbacks', [P(tGLsizei, 'n'), P(tGLuint, 'ids', 'n')])
#Func((4, 5), 'glGetTransformFeedbackiv', [P(tGLuint, 'xfb'), P(tGLenum, 'pname'), P(tMutablePointer, 'param')])
#Func((4, 5), 'glGetTransformFeedbacki_v', [P(tGLuint, 'xfb'), P(tGLenum, 'pname'), P(tGLuint, 'index'), P(tMutablePointer, 'param')])
#Func((4, 5), 'glGetTransformFeedbacki64_v', [P(tGLuint, 'xfb'), P(tGLenum, 'pname'), P(tGLuint, 'index'), P(tMutablePointer, 'param')])
Func((4, 5), 'glCreateBuffers', [P(tGLsizei, 'n'), P(tGLuint, 'buffers', 'n')])
#Func((4, 5), 'glNamedBufferStorage', [P(tGLuint, 'buffer'), P(tGLsizeiptr, 'size'), P(tPointer, 'data'), P(tGLbitfield, 'flags')])
Func((4, 5), 'glNamedBufferData', [P(tGLuint, 'buffer'), P(tGLsizeiptr, 'size'), P(tData('size'), 'data'), P(tGLenum, 'usage')])
Func((4, 5), 'glNamedBufferSubData', [P(tGLuint, 'buffer'), P(tGLintptr, 'offset'), P(tGLsizeiptr, 'size'), P(tData('size'), 'data')])
#Func((4, 5), 'glClearNamedBufferData', [P(tGLuint, 'buffer'), P(tGLenum, 'internalformat'), P(tGLenum, 'format'), P(tGLenum, 'type'), P(tPointer, 'data')])
#Func((4, 5), 'glClearNamedBufferSubData', [P(tGLuint, 'buffer'), P(tGLenum, 'internalformat'), P(tGLintptr, 'offset'), P(tGLsizeiptr, 'size'), P(tGLenum, 'format'), P(tGLenum, 'type'), P(tPointer, 'data')])
#Func((4, 5), 'glMapNamedBuffer', [P(tGLuint, 'buffer'), P(tGLenum, 'access')], tPointer)
#Func((4, 5), 'glMapNamedBufferRange', [P(tGLuint, 'buffer'), P(tGLintptr, 'offset'), P(tGLsizeiptr, 'length'), P(tGLbitfield, 'access')], tPointer)
#Func((4, 5), 'glGetNamedBufferParameteriv', [P(tGLuint, 'buffer'), P(tGLenum, 'pname'), P(tMutablePointer, 'params')])
#Func((4, 5), 'glGetNamedBufferParameteri64v', [P(tGLuint, 'buffer'), P(tGLenum, 'pname'), P(tMutablePointer, 'params')])
#Func((4, 5), 'glGetNamedBufferPointerv', [P(tGLuint, 'buffer'), P(tGLenum, 'pname'), P(tMutablePointer, 'params')])
#Func((4, 5), 'glGetNamedBufferSubData', [P(tGLuint, 'buffer'), P(tGLintptr, 'offset'), P(tGLsizeiptr, 'size'), P(tMutablePointer, 'data')])
#Func((4, 5), 'glCreateFramebuffers', [P(tGLsizei, 'n'), P(tMutablePointer, 'framebuffers')])
Func((4, 5), 'glNamedFramebufferDrawBuffers', [P(tGLuint, 'framebuffer'), P(tGLsizei, 'n'), P(tGLenum, 'bufs', 'n')])
#Func((4, 5), 'glInvalidateNamedFramebufferData', [P(tGLuint, 'framebuffer'), P(tGLsizei, 'numAttachments'), P(tPointer, 'attachments')])
#Func((4, 5), 'glInvalidateNamedFramebufferSubData', [P(tGLuint, 'framebuffer'), P(tGLsizei, 'numAttachments'), P(tPointer, 'attachments'), P(tGLint, 'x'), P(tGLint, 'y'), P(tGLsizei, 'width'), P(tGLsizei, 'height')])
#Func((4, 5), 'glClearNamedFramebufferiv', [P(tGLuint, 'framebuffer'), P(tGLenum, 'buffer'), P(tGLint, 'drawbuffer'), P(tPointer, 'value')])
#Func((4, 5), 'glClearNamedFramebufferuiv', [P(tGLuint, 'framebuffer'), P(tGLenum, 'buffer'), P(tGLint, 'drawbuffer'), P(tPointer, 'value')])
#Func((4, 5), 'glClearNamedFramebufferfv', [P(tGLuint, 'framebuffer'), P(tGLenum, 'buffer'), P(tGLint, 'drawbuffer'), P(tPointer, 'value')])
#Func((4, 5), 'glGetNamedFramebufferParameteriv', [P(tGLuint, 'framebuffer'), P(tGLenum, 'pname'), P(tMutablePointer, 'param')])
#Func((4, 5), 'glGetNamedFramebufferAttachmentParameteriv', [P(tGLuint, 'framebuffer'), P(tGLenum, 'attachment'), P(tGLenum, 'pname'), P(tMutablePointer, 'params')])
Func((4, 5), 'glCreateRenderbuffers', [P(tGLsizei, 'n'), P(tGLuint, 'renderbuffers', 'n')])
#Func((4, 5), 'glGetNamedRenderbufferParameteriv', [P(tGLuint, 'renderbuffer'), P(tGLenum, 'pname'), P(tMutablePointer, 'params')])
Func((4, 5), 'glCreateTextures', [P(tGLenum, 'target'), P(tGLsizei, 'n'), P(tGLuint, 'textures', 'n')])
#Func((4, 5), 'glTextureSubImage1D', [P(tGLuint, 'texture'), P(tGLint, 'level'), P(tGLint, 'xoffset'), P(tGLsizei, 'width'), P(tGLenum, 'format'), P(tGLenum, 'type'), P(tPointer, 'pixels')])
#Func((4, 5), 'glTextureSubImage2D', [P(tGLuint, 'texture'), P(tGLint, 'level'), P(tGLint, 'xoffset'), P(tGLint, 'yoffset'), P(tGLsizei, 'width'), P(tGLsizei, 'height'), P(tGLenum, 'format'), P(tGLenum, 'type'), P(tPointer, 'pixels')])
#Func((4, 5), 'glTextureSubImage3D', [P(tGLuint, 'texture'), P(tGLint, 'level'), P(tGLint, 'xoffset'), P(tGLint, 'yoffset'), P(tGLint, 'zoffset'), P(tGLsizei, 'width'), P(tGLsizei, 'height'), P(tGLsizei, 'depth'), P(tGLenum, 'format'), P(tGLenum, 'type'), P(tPointer, 'pixels')])
#Func((4, 5), 'glCompressedTextureSubImage1D', [P(tGLuint, 'texture'), P(tGLint, 'level'), P(tGLint, 'xoffset'), P(tGLsizei, 'width'), P(tGLenum, 'format'), P(tGLsizei, 'imageSize'), P(tPointer, 'data')])
#Func((4, 5), 'glCompressedTextureSubImage2D', [P(tGLuint, 'texture'), P(tGLint, 'level'), P(tGLint, 'xoffset'), P(tGLint, 'yoffset'), P(tGLsizei, 'width'), P(tGLsizei, 'height'), P(tGLenum, 'format'), P(tGLsizei, 'imageSize'), P(tPointer, 'data')])
#Func((4, 5), 'glCompressedTextureSubImage3D', [P(tGLuint, 'texture'), P(tGLint, 'level'), P(tGLint, 'xoffset'), P(tGLint, 'yoffset'), P(tGLint, 'zoffset'), P(tGLsizei, 'width'), P(tGLsizei, 'height'), P(tGLsizei, 'depth'), P(tGLenum, 'format'), P(tGLsizei, 'imageSize'), P(tPointer, 'data')])
#Func((4, 5), 'glTextureParameterfv', [P(tGLuint, 'texture'), P(tGLenum, 'pname'), P(tPointer, 'param')])
#Func((4, 5), 'glTextureParameterIiv', [P(tGLuint, 'texture'), P(tGLenum, 'pname'), P(tPointer, 'params')])
#Func((4, 5), 'glTextureParameterIuiv', [P(tGLuint, 'texture'), P(tGLenum, 'pname'), P(tPointer, 'params')])
#Func((4, 5), 'glTextureParameteriv', [P(tGLuint, 'texture'), P(tGLenum, 'pname'), P(tPointer, 'param')])
#Func((4, 5), 'glGetTextureImage', [P(tGLuint, 'texture'), P(tGLint, 'level'), P(tGLenum, 'format'), P(tGLenum, 'type'), P(tGLsizei, 'bufSize'), P(tMutablePointer, 'pixels')])
#Func((4, 5), 'glGetCompressedTextureImage', [P(tGLuint, 'texture'), P(tGLint, 'level'), P(tGLsizei, 'bufSize'), P(tMutablePointer, 'pixels')])
#Func((4, 5), 'glGetTextureLevelParameterfv', [P(tGLuint, 'texture'), P(tGLint, 'level'), P(tGLenum, 'pname'), P(tMutablePointer, 'params')])
#Func((4, 5), 'glGetTextureLevelParameteriv', [P(tGLuint, 'texture'), P(tGLint, 'level'), P(tGLenum, 'pname'), P(tMutablePointer, 'params')])
#Func((4, 5), 'glGetTextureParameterfv', [P(tGLuint, 'texture'), P(tGLenum, 'pname'), P(tMutablePointer, 'params')])
#Func((4, 5), 'glGetTextureParameterIiv', [P(tGLuint, 'texture'), P(tGLenum, 'pname'), P(tMutablePointer, 'params')])
#Func((4, 5), 'glGetTextureParameterIuiv', [P(tGLuint, 'texture'), P(tGLenum, 'pname'), P(tMutablePointer, 'params')])
#Func((4, 5), 'glGetTextureParameteriv', [P(tGLuint, 'texture'), P(tGLenum, 'pname'), P(tMutablePointer, 'params')])
Func((4, 5), 'glCreateVertexArrays', [P(tGLsizei, 'n'), P(tGLuint, 'arrays', 'n')])
Func((4, 5), 'glVertexArrayVertexBuffers', [P(tGLuint, 'vaobj'), P(tGLuint, 'first'), P(tGLsizei, 'count'), P(tGLuint, 'buffers', 'count'), P(tGLintptr, 'offsets', 'count'), P(tGLsizei, 'strides', 'count')])
#Func((4, 5), 'glGetVertexArrayiv', [P(tGLuint, 'vaobj'), P(tGLenum, 'pname'), P(tMutablePointer, 'param')])
#Func((4, 5), 'glGetVertexArrayIndexediv', [P(tGLuint, 'vaobj'), P(tGLuint, 'index'), P(tGLenum, 'pname'), P(tMutablePointer, 'param')])
#Func((4, 5), 'glGetVertexArrayIndexed64iv', [P(tGLuint, 'vaobj'), P(tGLuint, 'index'), P(tGLenum, 'pname'), P(tMutablePointer, 'param')])
Func((4, 5), 'glCreateSamplers', [P(tGLsizei, 'n'), P(tGLuint, 'samplers', 'n')])
Func((4, 5), 'glCreateProgramPipelines', [P(tGLsizei, 'n'), P(tGLuint, 'pipelines', 'n')])
Func((4, 5), 'glCreateQueries', [P(tGLenum, 'target'), P(tGLsizei, 'n'), P(tGLuint, 'ids', 'n')])
#Func((4, 5), 'glGetTextureSubImage', [P(tGLuint, 'texture'), P(tGLint, 'level'), P(tGLint, 'xoffset'), P(tGLint, 'yoffset'), P(tGLint, 'zoffset'), P(tGLsizei, 'width'), P(tGLsizei, 'height'), P(tGLsizei, 'depth'), P(tGLenum, 'format'), P(tGLenum, 'type'), P(tGLsizei, 'bufSize'), P(tMutablePointer, 'pixels')])
#Func((4, 5), 'glGetCompressedTextureSubImage', [P(tGLuint, 'texture'), P(tGLint, 'level'), P(tGLint, 'xoffset'), P(tGLint, 'yoffset'), P(tGLint, 'zoffset'), P(tGLsizei, 'width'), P(tGLsizei, 'height'), P(tGLsizei, 'depth'), P(tGLsizei, 'bufSize'), P(tMutablePointer, 'pixels')])
#Func((4, 5), 'glGetnCompressedTexImage', [P(tGLenum, 'target'), P(tGLint, 'lod'), P(tGLsizei, 'bufSize'), P(tMutablePointer, 'pixels')])
#Func((4, 5), 'glGetnTexImage', [P(tGLenum, 'target'), P(tGLint, 'level'), P(tGLenum, 'format'), P(tGLenum, 'type'), P(tGLsizei, 'bufSize'), P(tMutablePointer, 'pixels')])
#Func((4, 5), 'glGetnUniformdv', [P(tGLuint, 'program'), P(tGLint, 'location'), P(tGLsizei, 'bufSize'), P(tMutablePointer, 'params')])
#Func((4, 5), 'glGetnUniformfv', [P(tGLuint, 'program'), P(tGLint, 'location'), P(tGLsizei, 'bufSize'), P(tMutablePointer, 'params')])
#Func((4, 5), 'glGetnUniformiv', [P(tGLuint, 'program'), P(tGLint, 'location'), P(tGLsizei, 'bufSize'), P(tMutablePointer, 'params')])
#Func((4, 5), 'glGetnUniformuiv', [P(tGLuint, 'program'), P(tGLint, 'location'), P(tGLsizei, 'bufSize'), P(tMutablePointer, 'params')])
#Func((4, 5), 'glReadnPixels', [P(tGLint, 'x'), P(tGLint, 'y'), P(tGLsizei, 'width'), P(tGLsizei, 'height'), P(tGLenum, 'format'), P(tGLenum, 'type'), P(tGLsizei, 'bufSize'), P(tMutablePointer, 'data')])

Func((4, 3), 'glTexBufferRange', [P(tGLenum, 'target'),
                                  P(tGLenum, 'internalformat', None, 'InternalFormat'),
                                  P(tGLuint, 'buffer'), P(tGLintptr, 'offset'),
                                  P(tGLsizeiptr, 'size')])

Func((4, 3), 'glTexStorage2DMultisample', [P(tGLenum, 'target'), P(tGLsizei, 'samples'),
                                           P(tGLenum, 'internalformat', None, 'InternalFormat'),
                                           P(tGLsizei, 'width'), P(tGLsizei, 'height'),
                                           P(tGLboolean, 'fixedsamplelocations', None, 'Boolean')])

Func((4, 3), 'glTexStorage3DMultisample', [P(tGLenum, 'target'), P(tGLsizei, 'samples'),
                                           P(tGLenum, 'internalformat', None, 'InternalFormat'),
                                           P(tGLsizei, 'width'), P(tGLsizei, 'height'), P(tGLsizei, 'depth'),
                                           P(tGLboolean, 'fixedsamplelocations', None, 'Boolean')])

Func((3, 2), 'glTexImage2DMultisample', [P(tGLenum, 'target'), P(tGLsizei, 'samples'), P(tGLenum, 'internalformat', None, 'InternalFormat'),
                                         P(tGLsizei, 'width'), P(tGLsizei, 'height'), P(tGLboolean, 'fixedsamplelocations', None, 'Boolean')])

Func((3, 2), 'glTexImage3DMultisample', [P(tGLenum, 'target'), P(tGLsizei, 'samples'), Param(tGLenum, 'internalformat', None, 'InternalFormat'),
                                         P(tGLsizei, 'width'), P(tGLsizei, 'height'), P(tGLsizei, 'depth'), P(tGLboolean, 'fixedsamplelocations', None, 'Boolean')])

Func((4, 3), 'glTextureView', [P(tGLuint, 'texture'), P(tGLenum, 'target'), P(tGLuint, 'origtexture'), P(tGLenum, 'internalformat', None, 'InternalFormat'),
                               P(tGLuint, 'minlevel'), P(tGLuint, 'numlevels'), P(tGLuint, 'minlayer'), P(tGLuint, 'numlayers')])

Func((1, 3), 'glCompressedTexImage3D', [P(tGLenum, 'target'), P(tGLint, 'level'), P(tGLenum, 'internalformat'),
                                        P(tGLsizei, 'width'), P(tGLsizei, 'height'), P(tGLsizei, 'depth'),
                                        P(tGLint, 'border'), P(tGLsizei, 'imageSize'), P(tData('imageSize'), 'data')])

Func((1, 3), 'glCompressedTexImage2D', [P(tGLenum, 'target'), P(tGLint, 'level'), P(tGLenum, 'internalformat'),
                                        P(tGLsizei, 'width'), P(tGLsizei, 'height'), P(tGLint, 'border'),
                                        P(tGLsizei, 'imageSize'), P(tData('imageSize'), 'data')])

Func((1, 3), 'glCompressedTexImage1D', [P(tGLenum, 'target'), P(tGLint, 'level'), P(tGLenum, 'internalformat'), P(tGLsizei, 'width'),
                                        P(tGLint, 'border'), P(tGLsizei, 'imageSize'), P(tData('imageSize'), 'data')])

Func((1, 3), 'glCompressedTexSubImage3D', [P(tGLenum, 'target'), P(tGLint, 'level'), P(tGLint, 'xoffset'), P(tGLint, 'yoffset'),
                                           P(tGLint, 'zoffset'), P(tGLsizei, 'width'), P(tGLsizei, 'height'), P(tGLsizei, 'depth'),
                                           P(tGLenum, 'format'), P(tGLsizei, 'imageSize'), P(tData('imageSize'), 'data')])

Func((1, 3), 'glCompressedTexSubImage2D', [P(tGLenum, 'target'), P(tGLint, 'level'), P(tGLint, 'xoffset'), P(tGLint, 'yoffset'), P(tGLsizei, 'width'),
                                           P(tGLsizei, 'height'), P(tGLenum, 'format'), P(tGLsizei, 'imageSize'), P(tData('imageSize'), 'data')])

Func((1, 3), 'glCompressedTexSubImage1D', [P(tGLenum, 'target'), P(tGLint, 'level'), P(tGLint, 'xoffset'), P(tGLsizei, 'width'),
                                           P(tGLenum, 'format'), P(tGLsizei, 'imageSize'), P(tData('imageSize'), 'data')])

Func((4, 5), 'glCompressedTextureSubImage1D', [P(tGLuint, 'texture'), P(tGLint, 'level'), P(tGLint, 'xoffset'), P(tGLsizei, 'width'),
                                               P(tGLenum, 'format'), P(tGLsizei, 'imageSize'), P(tData('imageSize'), 'data')])

Func((4, 5), 'glCompressedTextureSubImage2D', [P(tGLuint, 'texture'), P(tGLint, 'level'), P(tGLint, 'xoffset'), P(tGLint, 'yoffset'), P(tGLsizei, 'width'),
                                               P(tGLsizei, 'height'), P(tGLenum, 'format'), P(tGLsizei, 'imageSize'), P(tData('imageSize'), 'data')])

Func((4, 5), 'glCompressedTextureSubImage3D', [P(tGLuint, 'texture'), P(tGLint, 'level'), P(tGLint, 'xoffset'), P(tGLint, 'yoffset'),
                                               P(tGLint, 'zoffset'), P(tGLsizei, 'width'), P(tGLsizei, 'height'), P(tGLsizei, 'depth'),
                                               P(tGLenum, 'format'), P(tGLsizei, 'imageSize'), Param(tData('imageSize'), 'data')])

Func((4, 2), 'glTexStorage1D', [P(tGLenum, 'target'), P(tGLsizei, 'levels'), P(tGLenum, 'internalformat', None, 'InternalFormat'), P(tGLsizei, 'width')])

Func((4, 2), 'glTexStorage2D', [P(tGLenum, 'target'), P(tGLsizei, 'levels'),
                                P(tGLenum, 'internalformat', None, 'InternalFormat'), P(tGLsizei, 'width'), P(tGLsizei, 'height')])

Func((4, 2), 'glTexStorage3D', [P(tGLenum, 'target'), P(tGLsizei, 'levels'), P(tGLenum, 'internalformat', None, 'InternalFormat'),
                                P(tGLsizei, 'width'), P(tGLsizei, 'height'), P(tGLsizei, 'depth')])

Func((3, 0), 'glRenderbufferStorage', [P(tGLenum, 'target'), P(tGLenum, 'internalformat', None, 'InternalFormat'),
                                       P(tGLsizei, 'width'), P(tGLsizei, 'height')])

Func((3, 0), 'glRenderbufferStorageMultisample', [P(tGLenum, 'target'), P(tGLsizei, 'samples'),
                                                  P(tGLenum, 'internalformat', None, 'InternalFormat'),
                                                  P(tGLsizei, 'width'), P(tGLsizei, 'height')])

Func((4, 5), 'glNamedRenderbufferStorage', [P(tGLuint, 'renderbuffer'), P(tGLenum, 'internalformat', None, 'InternalFormat'),
                                            P(tGLsizei, 'width'), P(tGLsizei, 'height')])

Func((4, 5), 'glNamedRenderbufferStorageMultisample', [P(tGLuint, 'renderbuffer'), P(tGLsizei, 'samples'), P(tGLenum, 'internalformat', None, 'InternalFormat'),
                                                       P(tGLsizei, 'width'), P(tGLsizei, 'height')])

Func((1, 1), 'glDrawArrays', [P(tGLenum, 'mode', None, 'PrimitiveType'), P(tGLint, 'first'),
                      P(tGLsizei, 'count')])\
     .trace_epilogue_code = 'if (test_mode) test_fb("glDrawArrays");'

Func((1, 1), 'glDrawElements', [P(tGLenum, 'mode', None, 'PrimitiveType'), P(tGLsizei, 'count'),
                        P(tGLenum, 'type'), P(tPointer, 'indices')])\
     .trace_epilogue_code = 'if (test_mode) test_fb("glDrawElements");'

Func((3, 1), 'glDrawArraysInstanced', [P(tGLenum, 'mode', None, 'PrimitiveType'),
                               P(tGLint, 'first'), P(tGLsizei, 'count'),
                               P(tGLsizei, 'instancecount')])\
     .trace_epilogue_code = 'if (test_mode) test_fb("glDrawArraysInstanced");'

Func((4, 2), 'glDrawArraysInstancedBaseInstance', [P(tGLenum, 'mode', None, 'PrimitiveType'),
                                           P(tGLint, 'first'), P(tGLsizei, 'count'),
                                           P(tGLsizei, 'instancecount'),
                                           P(tGLuint, 'baseinstance')])\
     .trace_epilogue_code = 'if (test_mode) test_fb("glDrawArraysInstancedBaseInstance");'

Func((2, 0), 'glLinkProgram', [P(tGLuint, 'program', None)], None).trace_extras_code = 'link_program_extras(program);'
Func((1, 0), 'glViewport', [P(tGLint, 'x'), P(tGLint, 'y'),
                    P(tGLsizei, 'width'), P(tGLsizei, 'height')]).trace_epilogue_code = 'update_drawable_size();'

Func((1, 5), 'glUnmapBuffer', [P(tGLenum, 'target', None)], tGLboolean).trace_extras_code = '''GLint access;
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

glDrawableSizeWIP15((1, 0), 'glDrawableSizeWIP15', [P(tGLsizei, 'width'), P(tGLsizei, 'height')], None)
glTestFBWIP15((1, 0), 'glTestFBWIP15', [P(tString, 'name'), P(tData('drawable_width*drawable_height*4'), 'color'),
                                        P(tData('drawable_width*drawable_height*4'), 'depth')], None)

#Func(None, 'glXGetFBConfigs', [P(tMutablePointer, 'dpy'), P(tint, 'screen'), P(tMutablePointer, 'nelements')], tPointer)
#Func(None, 'glXGetGPUIDsAMD', [P(tunsignedint, 'maxCount'), P(tMutablePointer, 'ids')], tunsignedint)
#Func(None, 'glXQueryExtension', [P(tMutablePointer, 'dpy'), P(tMutablePointer, 'errorb'), P(tMutablePointer, 'event')], tBool)
#Func(None, 'glXQueryServerString', [P(tMutablePointer, 'dpy'), P(tint, 'screen'), P(tint, 'name')], tPointer)
#Func(None, 'glXChannelRectSGIX', [P(tMutablePointer, 'display'), P(tint, 'screen'), P(tint, 'channel'), P(tint, 'x'), P(tint, 'y'), P(tint, 'w'), P(tint, 'h')], tint)
#Func(None, 'glXCopyContext', [P(tMutablePointer, 'dpy'), P(tGLXContext, 'src'), P(tGLXContext, 'dst'), P(tunsignedlong, 'mask')])
#Func(None, 'glXGetTransparentIndexSUN', [P(tMutablePointer, 'dpy'), P(tWindow, 'overlay'), P(tWindow, 'underlay'), P(tMutablePointer, 'pTransparentIndex')], tStatus)
Func(None, 'glXChooseVisual', [P(tMutablePointer, 'dpy'), P(tint, 'screen'), P(tint, 'attribList', 'glx_attrib_int_count(attribList)')], tPointer)
#Func(None, 'glXReleaseVideoImageNV', [P(tMutablePointer, 'dpy'), P(tGLXPbuffer, 'pbuf')], tint)
#Func(None, 'glXDestroyGLXPbufferSGIX', [P(tMutablePointer, 'dpy'), P(tGLXPbufferSGIX, 'pbuf')])
#Func(None, 'glXGetCurrentAssociatedContextAMD', [], tGLXContext)
#Func(None, 'glXQueryContextInfoEXT', [P(tMutablePointer, 'dpy'), P(tGLXContext, 'context'), P(tint, 'attribute'), P(tMutablePointer, 'value')], tint)
#Func(None, 'glXQueryCurrentRendererIntegerMESA', [P(tint, 'attribute'), P(tMutablePointer, 'value')], tBool)
#Func(None, 'glXDestroyPbuffer', [P(tMutablePointer, 'dpy'), P(tGLXPbuffer, 'pbuf')])
#Func(None, 'glXGetFBConfigAttribSGIX', [P(tMutablePointer, 'dpy'), P(tGLXFBConfigSGIX, 'config'), P(tint, 'attribute'), P(tMutablePointer, 'value')], tint)
#Func(None, 'glXNamedCopyBufferSubDataNV', [P(tMutablePointer, 'dpy'), P(tGLXContext, 'readCtx'), P(tGLXContext, 'writeCtx'), P(tGLuint, 'readBuffer'), P(tGLuint, 'writeBuffer'), P(tGLintptr, 'readOffset'), P(tGLintptr, 'writeOffset'), P(tGLsizeiptr, 'size')])
#Func(None, 'glXHyperpipeAttribSGIX', [P(tMutablePointer, 'dpy'), P(tint, 'timeSlice'), P(tint, 'attrib'), P(tint, 'size'), P(tMutablePointer, 'attribList')], tint)
#Func(None, 'glXDestroyGLXVideoSourceSGIX', [P(tMutablePointer, 'dpy'), P(tGLXVideoSourceSGIX, 'glxvideosource')])
#Func(None, 'glXQueryExtensionsString', [P(tMutablePointer, 'dpy'), P(tint, 'screen')], tPointer)
#Func(None, 'glXSendPbufferToVideoNV', [P(tMutablePointer, 'dpy'), P(tGLXPbuffer, 'pbuf'), P(tint, 'iBufferType'), P(tMutablePointer, 'pulCounterPbuffer'), P(tGLboolean, 'bBlock')], tint)
#Func(None, 'glXDestroyWindow', [P(tMutablePointer, 'dpy'), P(tGLXWindow, 'win')])
#Func(None, 'glXQueryHyperpipeAttribSGIX', [P(tMutablePointer, 'dpy'), P(tint, 'timeSlice'), P(tint, 'attrib'), P(tint, 'size'), P(tMutablePointer, 'returnAttribList')], tint)
#Func(None, 'glXHyperpipeConfigSGIX', [P(tMutablePointer, 'dpy'), P(tint, 'networkId'), P(tint, 'npipes'), P(tMutablePointer, 'cfg'), P(tMutablePointer, 'hpId')], tint)
#Func(None, 'glXDestroyContext', [P(tMutablePointer, 'dpy'), P(tGLXContext, 'ctx')])
#Func(None, 'glXGetCurrentDisplayEXT', [], tPointer)
#Func(None, 'glXCreateContextWithConfigSGIX', [P(tMutablePointer, 'dpy'), P(tGLXFBConfigSGIX, 'config'), P(tint, 'render_type'), P(tGLXContext, 'share_list'), P(tBool, 'direct')], tGLXContext)
#Func(None, 'glXMakeAssociatedContextCurrentAMD', [P(tGLXContext, 'ctx')], tBool)
#Func(None, 'glXMakeCurrentReadSGI', [P(tMutablePointer, 'dpy'), P(tGLXDrawable, 'draw'), P(tGLXDrawable, 'read'), P(tGLXContext, 'ctx')], tBool)
#Func(None, 'glXBindSwapBarrierSGIX', [P(tMutablePointer, 'dpy'), P(tGLXDrawable, 'drawable'), P(tint, 'barrier')])
#Func(None, 'glXFreeContextEXT', [P(tMutablePointer, 'dpy'), P(tGLXContext, 'context')])
#Func(None, 'glXDestroyHyperpipeConfigSGIX', [P(tMutablePointer, 'dpy'), P(tint, 'hpId')], tint)
#Func(None, 'glXWaitForSbcOML', [P(tMutablePointer, 'dpy'), P(tGLXDrawable, 'drawable'), P(tint64_t, 'target_sbc'), P(tMutablePointer, 'ust'), P(tMutablePointer, 'msc'), P(tMutablePointer, 'sbc')], tBool)
#Func(None, 'glXGetSelectedEvent', [P(tMutablePointer, 'dpy'), P(tGLXDrawable, 'draw'), P(tMutablePointer, 'event_mask')])
#Func(None, 'glXEnumerateVideoCaptureDevicesNV', [P(tMutablePointer, 'dpy'), P(tint, 'screen'), P(tMutablePointer, 'nelements')], tPointer)
#Func(None, 'glXCreateGLXPixmapMESA', [P(tMutablePointer, 'dpy'), P(tMutablePointer, 'visual'), P(tPixmap, 'pixmap'), P(tColormap, 'cmap')], tGLXPixmap)
#Func(None, 'glXCreateWindow', [P(tMutablePointer, 'dpy'), P(tGLXFBConfig, 'config'), P(tWindow, 'win'), P(tPointer, 'attrib_list')], tGLXWindow)
#Func(None, 'glXGetCurrentDrawable', [], tGLXDrawable)
#Func(None, 'glXQueryHyperpipeBestAttribSGIX', [P(tMutablePointer, 'dpy'), P(tint, 'timeSlice'), P(tint, 'attrib'), P(tint, 'size'), P(tMutablePointer, 'attribList'), P(tMutablePointer, 'returnAttribList')], tint)
glXGetProcAddressFunc(None, 'glXGetProcAddress', [P(tString, 'procName')], t__GLXextFuncPtr)
#Func(None, 'glXCushionSGI', [P(tMutablePointer, 'dpy'), P(tWindow, 'window'), P(tfloat, 'cushion')])
#Func(None, 'glXMakeContextCurrent', [P(tMutablePointer, 'dpy'), P(tGLXDrawable, 'draw'), P(tGLXDrawable, 'read'), P(tGLXContext, 'ctx')], tBool)
#Func(None, 'glXSwapIntervalSGI', [P(tint, 'interval')], tint)
#Func(None, 'glXBindTexImageEXT', [P(tMutablePointer, 'dpy'), P(tGLXDrawable, 'drawable'), P(tint, 'buffer'), P(tPointer, 'attrib_list')])
#Func(None, 'glXReleaseVideoCaptureDeviceNV', [P(tMutablePointer, 'dpy'), P(tGLXVideoCaptureDeviceNV, 'device')])
#Func(None, 'glXGetCurrentContext', [], tGLXContext)
#Func(None, 'glXQueryMaxSwapGroupsNV', [P(tMutablePointer, 'dpy'), P(tint, 'screen'), P(tMutablePointer, 'maxGroups'), P(tMutablePointer, 'maxBarriers')], tBool)
#Func(None, 'glXQueryHyperpipeConfigSGIX', [P(tMutablePointer, 'dpy'), P(tint, 'hpId'), P(tMutablePointer, 'npipes')], tPointer)
#Func(None, 'glXWaitVideoSyncSGI', [P(tint, 'divisor'), P(tint, 'remainder'), P(tMutablePointer, 'count')], tint)
#Func(None, 'glXGetConfig', [P(tMutablePointer, 'dpy'), P(tMutablePointer, 'visual'), P(tint, 'attrib'), P(tMutablePointer, 'value')], tint)
#Func(None, 'glXCreateGLXPbufferSGIX', [P(tMutablePointer, 'dpy'), P(tGLXFBConfigSGIX, 'config'), P(tunsignedint, 'width'), P(tunsignedint, 'height'), P(tMutablePointer, 'attrib_list')], tGLXPbufferSGIX)
#Func(None, 'glXQueryVideoCaptureDeviceNV', [P(tMutablePointer, 'dpy'), P(tGLXVideoCaptureDeviceNV, 'device'), P(tint, 'attribute'), P(tMutablePointer, 'value')], tint)
#Func(None, 'glXGetVideoSyncSGI', [P(tMutablePointer, 'count')], tint)
#Func(None, 'glXQueryDrawable', [P(tMutablePointer, 'dpy'), P(tGLXDrawable, 'draw'), P(tint, 'attribute'), P(tMutablePointer, 'value')])
#Func(None, 'glXLockVideoCaptureDeviceNV', [P(tMutablePointer, 'dpy'), P(tGLXVideoCaptureDeviceNV, 'device')])
#Func(None, 'glXGetCurrentDisplay', [], tPointer)
#Func(None, 'glXBlitContextFramebufferAMD', [P(tGLXContext, 'dstCtx'), P(tGLint, 'srcX0'), P(tGLint, 'srcY0'), P(tGLint, 'srcX1'), P(tGLint, 'srcY1'), P(tGLint, 'dstX0'), P(tGLint, 'dstY0'), P(tGLint, 'dstX1'), P(tGLint, 'dstY1'), P(tGLbitfield, 'mask'), P(tGLenum, 'filter')])
#Func(None, 'glXGetClientString', [P(tMutablePointer, 'dpy'), P(tint, 'name')], tPointer)
#Func(None, 'glXGetMscRateOML', [P(tMutablePointer, 'dpy'), P(tGLXDrawable, 'drawable'), P(tMutablePointer, 'numerator'), P(tMutablePointer, 'denominator')], tBool)
#Func(None, 'glXGetCurrentReadDrawableSGI', [], tGLXDrawable)
#Func(None, 'glXReleaseBuffersMESA', [P(tMutablePointer, 'dpy'), P(tGLXDrawable, 'drawable')], tBool)
#Func(None, 'glXGetFBConfigAttrib', [P(tMutablePointer, 'dpy'), P(tGLXFBConfig, 'config'), P(tint, 'attribute'), P(tMutablePointer, 'value')], tint)
#Func(None, 'glXDestroyGLXPixmap', [P(tMutablePointer, 'dpy'), P(tGLXPixmap, 'pixmap')])
#Func(None, 'glXQueryVersion', [P(tMutablePointer, 'dpy'), P(tMutablePointer, 'maj'), P(tMutablePointer, 'min')], tBool)
#Func(None, 'glXGetAGPOffsetMESA', [P(tPointer, 'pointer')], tunsignedint)
#Func(None, 'glXWaitForMscOML', [P(tMutablePointer, 'dpy'), P(tGLXDrawable, 'drawable'), P(tint64_t, 'target_msc'), P(tint64_t, 'divisor'), P(tint64_t, 'remainder'), P(tMutablePointer, 'ust'), P(tMutablePointer, 'msc'), P(tMutablePointer, 'sbc')], tBool)
#Func(None, 'glXQueryMaxSwapBarriersSGIX', [P(tMutablePointer, 'dpy'), P(tint, 'screen'), P(tMutablePointer, 'max')], tBool)
#Func(None, 'glXCreateGLXPixmap', [P(tMutablePointer, 'dpy'), P(tMutablePointer, 'visual'), P(tPixmap, 'pixmap')], tGLXPixmap)
#Func(None, 'glXBindChannelToWindowSGIX', [P(tMutablePointer, 'display'), P(tint, 'screen'), P(tint, 'channel'), P(tWindow, 'window')], tint)
#Func(None, 'glXChooseFBConfigSGIX', [P(tMutablePointer, 'dpy'), P(tint, 'screen'), P(tMutablePointer, 'attrib_list'), P(tMutablePointer, 'nelements')], tPointer)
#Func(None, 'glXSelectEventSGIX', [P(tMutablePointer, 'dpy'), P(tGLXDrawable, 'drawable'), P(tunsignedlong, 'mask')])
#Func(None, 'glXGetContextGPUIDAMD', [P(tGLXContext, 'ctx')], tunsignedint)
#Func(None, 'glXDestroyPixmap', [P(tMutablePointer, 'dpy'), P(tGLXPixmap, 'pixmap')])
#Func(None, 'glXCreateNewContext', [P(tMutablePointer, 'dpy'), P(tGLXFBConfig, 'config'), P(tint, 'render_type'), P(tGLXContext, 'share_list'), P(tBool, 'direct')], tGLXContext)
#Func(None, 'glXQueryGLXPbufferSGIX', [P(tMutablePointer, 'dpy'), P(tGLXPbufferSGIX, 'pbuf'), P(tint, 'attribute'), P(tMutablePointer, 'value')], tint)
#Func(None, 'glXCopySubBufferMESA', [P(tMutablePointer, 'dpy'), P(tGLXDrawable, 'drawable'), P(tint, 'x'), P(tint, 'y'), P(tint, 'width'), P(tint, 'height')])
#Func(None, 'glXQueryFrameCountNV', [P(tMutablePointer, 'dpy'), P(tint, 'screen'), P(tMutablePointer, 'count')], tBool)
#Func(None, 'glXGetCurrentReadDrawable', [], tGLXDrawable)
#Func(None, 'glXResetFrameCountNV', [P(tMutablePointer, 'dpy'), P(tint, 'screen')], tBool)
#Func(None, 'glXCreateAssociatedContextAttribsAMD', [P(tunsignedint, 'id'), P(tGLXContext, 'share_context'), P(tPointer, 'attribList')], tGLXContext)
Func(None, 'glXCreateContextAttribsARB', [P(tMutablePointer, 'dpy'), P(tGLXFBConfig, 'config'), P(tGLXContext, 'share_context'), P(tBool, 'direct'), P(tint, 'attrib_list', 'glx_attrib_int_count(attrib_list)')], tGLXContext)
#Func(None, 'glXDelayBeforeSwapNV', [P(tMutablePointer, 'dpy'), P(tGLXDrawable, 'drawable'), P(tGLfloat, 'seconds')], tBool)
#Func(None, 'glXImportContextEXT', [P(tMutablePointer, 'dpy'), P(tGLXContextID, 'contextID')], tGLXContext)
#Func(None, 'glXSelectEvent', [P(tMutablePointer, 'dpy'), P(tGLXDrawable, 'draw'), P(tunsignedlong, 'event_mask')])
#Func(None, 'glXGetVideoInfoNV', [P(tMutablePointer, 'dpy'), P(tint, 'screen'), P(tGLXVideoDeviceNV, 'VideoDevice'), P(tMutablePointer, 'pulCounterOutputPbuffer'), P(tMutablePointer, 'pulCounterOutputVideo')], tint)
#Func(None, 'glXGetSyncValuesOML', [P(tMutablePointer, 'dpy'), P(tGLXDrawable, 'drawable'), P(tMutablePointer, 'ust'), P(tMutablePointer, 'msc'), P(tMutablePointer, 'sbc')], tBool)
#Func(None, 'glXDeleteAssociatedContextAMD', [P(tGLXContext, 'ctx')], tBool)
glXGetProcAddressFunc(None, 'glXGetProcAddressARB', [P(tString, 'procName')], t__GLXextFuncPtr)
#Func(None, 'glXEnumerateVideoDevicesNV', [P(tMutablePointer, 'dpy'), P(tint, 'screen'), P(tMutablePointer, 'nelements')], tPointer)
#Func(None, 'glXCreateContext', [P(tMutablePointer, 'dpy'), P(tMutablePointer, 'vis'), P(tGLXContext, 'shareList'), P(tBool, 'direct')], tGLXContext)
#Func(None, 'glXReleaseTexImageEXT', [P(tMutablePointer, 'dpy'), P(tGLXDrawable, 'drawable'), P(tint, 'buffer')])
#Func(None, 'glXJoinSwapGroupNV', [P(tMutablePointer, 'dpy'), P(tGLXDrawable, 'drawable'), P(tGLuint, 'group')], tBool)
#Func(None, 'glXCreateAssociatedContextAMD', [P(tunsignedint, 'id'), P(tGLXContext, 'share_list')], tGLXContext)
#Func(None, 'glXBindHyperpipeSGIX', [P(tMutablePointer, 'dpy'), P(tint, 'hpId')], tint)
#Func(None, 'glXGetSelectedEventSGIX', [P(tMutablePointer, 'dpy'), P(tGLXDrawable, 'drawable'), P(tMutablePointer, 'mask')])
#Func(None, 'glXChooseFBConfig', [P(tMutablePointer, 'dpy'), P(tint, 'screen'), P(tPointer, 'attrib_list'), P(tMutablePointer, 'nelements')], tPointer)
#Func(None, 'glXIsDirect', [P(tMutablePointer, 'dpy'), P(tGLXContext, 'ctx')], tBool)
#Func(None, 'glXReleaseVideoDeviceNV', [P(tMutablePointer, 'dpy'), P(tint, 'screen'), P(tGLXVideoDeviceNV, 'VideoDevice')], tint)
#Func(None, 'glXGetGPUInfoAMD', [P(tunsignedint, 'id'), P(tint, 'property'), P(tGLenum, 'dataType'), P(tunsignedint, 'size'), P(tMutablePointer, 'data')], tint)
#Func(None, 'glXSwapBuffersMscOML', [P(tMutablePointer, 'dpy'), P(tGLXDrawable, 'drawable'), P(tint64_t, 'target_msc'), P(tint64_t, 'divisor'), P(tint64_t, 'remainder')], tint64_t)
#Func(None, 'glXGetContextIDEXT', [P(tGLXContext, 'context')], tGLXContextID)
#Func(None, 'glXJoinSwapGroupSGIX', [P(tMutablePointer, 'dpy'), P(tGLXDrawable, 'drawable'), P(tGLXDrawable, 'member')])
#Func(None, 'glXQueryContext', [P(tMutablePointer, 'dpy'), P(tGLXContext, 'ctx'), P(tint, 'attribute'), P(tMutablePointer, 'value')], tint)
#Func(None, 'glXWaitGL', [])
#Func(None, 'glXQueryCurrentRendererStringMESA', [P(tint, 'attribute')], tPointer)
#Func(None, 'glXCopyBufferSubDataNV', [P(tMutablePointer, 'dpy'), P(tGLXContext, 'readCtx'), P(tGLXContext, 'writeCtx'), P(tGLenum, 'readTarget'), P(tGLenum, 'writeTarget'), P(tGLintptr, 'readOffset'), P(tGLintptr, 'writeOffset'), P(tGLsizeiptr, 'size')])
Func(None, 'glXSwapBuffers', [P(tMutablePointer, 'dpy'), P(tGLXDrawable, 'drawable')]).trace_epilogue_code = 'update_drawable_size();\nif (test_mode) test_fb("glXSwapBuffers");'
#Func(None, 'glXWaitX', [])
#Func(None, 'glXQueryHyperpipeNetworkSGIX', [P(tMutablePointer, 'dpy'), P(tMutablePointer, 'npipes')], tPointer)
#Func(None, 'glXGetVideoDeviceNV', [P(tMutablePointer, 'dpy'), P(tint, 'screen'), P(tint, 'numVideoDevices'), P(tMutablePointer, 'pVideoDevice')], tint)
#Func(None, 'glXQueryChannelRectSGIX', [P(tMutablePointer, 'display'), P(tint, 'screen'), P(tint, 'channel'), P(tMutablePointer, 'dx'), P(tMutablePointer, 'dy'), P(tMutablePointer, 'dw'), P(tMutablePointer, 'dh')], tint)
#Func(None, 'glXQueryChannelDeltasSGIX', [P(tMutablePointer, 'display'), P(tint, 'screen'), P(tint, 'channel'), P(tMutablePointer, 'x'), P(tMutablePointer, 'y'), P(tMutablePointer, 'w'), P(tMutablePointer, 'h')], tint)
#Func(None, 'glXBindVideoDeviceNV', [P(tMutablePointer, 'dpy'), P(tunsignedint, 'video_slot'), P(tunsignedint, 'video_device'), P(tPointer, 'attrib_list')], tint)
#Func(None, 'glXQueryRendererIntegerMESA', [P(tMutablePointer, 'dpy'), P(tint, 'screen'), P(tint, 'renderer'), P(tint, 'attribute'), P(tMutablePointer, 'value')], tBool)
#Func(None, 'glXQuerySwapGroupNV', [P(tMutablePointer, 'dpy'), P(tGLXDrawable, 'drawable'), P(tMutablePointer, 'group'), P(tMutablePointer, 'barrier')], tBool)
#Func(None, 'glXCreatePbuffer', [P(tMutablePointer, 'dpy'), P(tGLXFBConfig, 'config'), P(tPointer, 'attrib_list')], tGLXPbuffer)
#Func(None, 'glXUseXFont', [P(tFont, 'font'), P(tint, 'first'), P(tint, 'count'), P(tint, 'list')])
#Func(None, 'glXGetVisualFromFBConfig', [P(tMutablePointer, 'dpy'), P(tGLXFBConfig, 'config')], tPointer)

#TODO: Choose the correct limits
Func(None, 'glXMakeCurrent', [P(tMutablePointer, 'dpy', None), P(tGLXDrawable, 'drawable', None), P(tGLXContext, 'ctx', None)], tBool).trace_epilogue_code = 'current_limits=ctx?&gl30_limits : NULL; reset_gl_funcs(); update_drawable_size();'
