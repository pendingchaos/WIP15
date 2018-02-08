P = Param

class glXGetProcAddressFunc(Func):
    def gen_wrapper(self):
        res = '__attribute__((visibility("default"))) __GLXextFuncPtr %s(const char* name) {\n' % self.name
        res += '    func_decl_%s();\n' % self.name
        res += '    gl_start_call(%d);\n' % self.func_id
        res += '    gl_write_str(name);\n'
        res += '    func_t result = (func_t)dlsym(RTLD_DEFAULT, name);\n'
        #res += '    gl_write_func_ptr(result);\n' # TODO
        res += '    gl_end_call();\n'
        res += '    return result;\n'
        res += '}'
        
        return res

class wip15DrawableSize(Func):
    def gen_wrapper(self):
        return '''__attribute__((visibility("default"))) void wip15DrawableSize(GLsizei width, GLsizei height) {
    func_decl_wip15DrawableSize();
    gl_start_call(%d);
    gl_write_sleb128(width);
    gl_write_sleb128(height);
    gl_end_call();
    drawable_width = width;
    drawable_height = height;
}''' % self.func_id

class wip15TestFB(Func):
    def gen_wrapper(self):
        return '''__attribute__((visibility("default"))) void wip15TestFB(const GLchar* name, const GLvoid* color, const GLvoid* depth) {
    func_decl_wip15TestFB();
    gl_start_call(%d);
    gl_write_str(name);
    gl_write_data(drawable_width*drawable_height*4, color);
    gl_write_data(drawable_width*drawable_height*4, depth);
    gl_end_call();
}
''' % self.func_id

class wip15BeginTest(Func):
    def gen_wrapper(self):
        return '''__attribute__((visibility("default"))) void wip15BeginTest(const GLchar* name) {
    func_decl_wip15BeginTest();
    gl_start_call(%d);
    gl_write_str(name);
    gl_end_call();
}''' % self.func_id

class wip15EndTest(Func):
    def gen_wrapper(self):
        return '''__attribute__((visibility("default"))) void wip15EndTest() {
    func_decl_wip15EndTest();
    gl_start_call(%d);
    gl_end_call();
}''' % self.func_id

class wip15PrintTestResults(Func):
    def gen_wrapper(self):
        return '''__attribute__((visibility("default"))) void wip15PrintTestResults() {
    func_decl_wip15PrintTestResults();
    gl_start_call(%d);
    gl_end_call();
}''' % self.func_id

class wip15ExpectPropertyi64(Func):
    def gen_wrapper(self):
        return '''__attribute__((visibility("default"))) void wip15ExpectPropertyi64(GLenum objType, GLuint64 objName, const GLchar* name, GLuint64 index, GLint64 val) {
    func_decl_wip15ExpectPropertyi64();
    gl_start_call(%d);
    gl_write_uleb128(objType);
    gl_write_uleb128(objName);
    gl_write_str(name);
    gl_write_uleb128(index);
    gl_write_sleb128(val);
    gl_end_call();
}''' % self.func_id

class wip15ExpectPropertyd(Func):
    def gen_wrapper(self):
        return '''__attribute__((visibility("default"))) void wip15ExpectPropertyd(GLenum objType, GLuint64 objName, const GLchar* name, GLuint64 index, GLdouble val) {
    func_decl_wip15ExpectPropertyd();
    gl_start_call(%d);
    gl_write_uleb128(objType);
    gl_write_uleb128(objName);
    gl_write_str(name);
    gl_write_uleb128(index);
    gl_write_double(val);
    gl_end_call();
}''' % self.func_id

class wip15ExpectPropertybv(Func):
    def gen_wrapper(self):
        return '''__attribute__((visibility("default"))) void wip15ExpectPropertybv(GLenum objType, GLuint64 objName, const GLchar* name, GLuint64 index, GLuint64 size, const GLvoid* data) {
    func_decl_wip15ExpectPropertybv();
    gl_start_call(%d);
    gl_write_uleb128(objType);
    gl_write_uleb128(objName);
    gl_write_str(name);
    gl_write_uleb128(index);
    gl_write_uleb128(size);
    gl_write_data(size, data);
    gl_end_call();
}''' % self.func_id

class wip15ExpectAttachment(Func):
    def gen_wrapper(self):
        return '''__attribute__((visibility("default"))) void wip15ExpectAttachment(const GLchar* attachment) {
    func_decl_wip15ExpectAttachment();
    gl_start_call(%d);
    gl_write_str(attachment);
    gl_end_call();
}''' % self.func_id

class tTexImageData(tData):
    def __init__(self, dim):
        Type.__init__(self)
        self.texel_count_expr = ['width', 'width*height', 'width*height*depth'][dim-1]
        self.row_count_expr = ['1', 'height', 'height'][dim-1]
    
    def gen_write_code(self, var_name):
        res = 'size_t sz_%d = get_texel_size(format, type) * %s / %s;\n' % (id(self), self.texel_count_expr, self.row_count_expr)
        #TODO: Is this incorrect for single-rowed images?
        res += 'GLint alignment_%d;\n' % id(self)
        res += 'F(glGetIntegerv)(GL_UNPACK_ALIGNMENT, &alignment_%d);\n' % id(self)
        res += 'sz_%d = (sz_%d+(alignment_%d-1)) & ~(alignment_%d-1);\n' % (id(self), id(self), id(self), id(self))
        res += 'sz_%d = sz_%d * %s;\n' % (id(self), id(self), self.row_count_expr)
        res += 'if (%s) gl_write_data(sz_%d, %s);\n' % (var_name, id(self), var_name)
        res += 'else {\n'
        res += '    void* d = calloc(sz_%d, 1);\n' % id(self)
        res += '    gl_write_data(sz_%d, d);\n' % id(self)
        res += '    free(d);\n'
        res += '}'
        return res

class tBufData(tData):
    def gen_write_code(self, var_name):
        res = 'if (%s) gl_write_data((%s), %s);\n' % (var_name, self.size_expr, var_name)
        res += 'else {\n'
        res += '    size_t sz_%d = %s;\n' % (id(self), str(self.size_expr))
        res += '    void* d = calloc(sz_%d, 1);\n' % id(self)
        res += '    gl_write_data(sz_%d, d);\n' % id(self)
        res += '    free(d);\n'
        res += '}'
        return res

class tShdrSrc(Type):
    base = 'BASE_STRING'
    
    def gen_type_code(self, var_name='', array_count=None):
        return 'char** %s' % var_name
    
    def gen_write_code(self, var_name):
        return '''if (length) {
    GLchar* src = malloc(length[i]+1);
    memcpy(src, string[i], length[i]);
    src[length[i]] = 0;
    gl_write_str(src);
    free(src);
} else {
    gl_write_str(string[i]);
}'''
    
    def gen_replay_read_code(self, dest, src, array_count=None):
        return 'const char*const* %s = trc_get_str(%s);' % (dest, src)
    
    def gen_replay_finalize_code(self, dest, src, array_count=None):
        return ''

PixelFormat2 = g('GL_RED', 'GL_RG', 'GL_RGB', 'GL_BGR', 'GL_RGBA', 'GL_BGRA', 'GL_RED_INTEGER',
                 'GL_RG_INTEGER', 'GL_RGB_INTEGER', 'GL_BGR_INTEGER', 'GL_RGBA_INTEGER',
                 'GL_BGRA_INTEGER', ('GL_STENCIL_INDEX', 4, 4), 'GL_DEPTH_COMPONENT', 'GL_DEPTH_STENCIL')

BufferTarget = g(('GL_ARRAY_BUFFER', 1, 5), ('GL_ATOMIC_COUNTER_BUFFER', 4, 2), ('GL_COPY_READ_BUFFER', 3, 1),
                 ('GL_COPY_WRITE_BUFFER', 3, 1), ('GL_DISPATCH_INDIRECT_BUFFER', 4, 3), ('GL_DRAW_INDIRECT_BUFFER', 4, 0),
                 ('GL_ELEMENT_ARRAY_BUFFER', 1, 5), ('GL_PIXEL_PACK_BUFFER', 2, 1), ('GL_PIXEL_UNPACK_BUFFER', 2, 1),
                 ('GL_QUERY_BUFFER', 4, 4), ('GL_SHADER_STORAGE_BUFFER', 4, 3), ('GL_TEXTURE_BUFFER', 3, 1),
                 ('GL_TRANSFORM_FEEDBACK_BUFFER', 3, 0), ('GL_UNIFORM_BUFFER', 3, 1))

FramebufferTarget = g('GL_FRAMEBUFFER', 'GL_DRAW_FRAMEBUFFER', 'GL_READ_FRAMEBUFFER')

QueryTarget = g(('GL_SAMPLES_PASSED', 1, 5), ('GL_ANY_SAMPLES_PASSED', 3, 3),
                ('GL_ANY_SAMPLES_PASSED_CONSERVATIVE', 4, 3), ('GL_PRIMITIVES_GENERATED', 3, 0),
                ('GL_TRANSFORM_FEEDBACK_PRIMITIVES_WRITTEN', 3, 0), ('GL_TIME_ELAPSED', 3, 3))

Group('ClipOrigin').add('GL_LOWER_LEFT', 0x8CA1, (4, 5))\
                   .add('GL_UPPER_LEFT', 0x8CA2, (4, 5))

Group('ClipDepth').add('GL_NEGATIVE_ONE_TO_ONE', 0x935E, (4, 5))\
                  .add('GL_ZERO_TO_ONE', 0x935F, (4, 5))

Group('PackedAttribType').add('GL_INT_2_10_10_10_REV', 0x8D9F, (3, 3))\
                        .add('GL_UNSIGNED_INT_2_10_10_10_REV', 0x8368, (3, 3))\

Group('PackedAttrib3Type').add('GL_INT_2_10_10_10_REV', 0x8D9F, (3, 3))\
                          .add('GL_UNSIGNED_INT_2_10_10_10_REV', 0x8368, (3, 3))\
                          .add('GL_UNSIGNED_INT_10F_11F_11F_REV', 0x8C3B, (4, 4))\

Group('ElementType').add('GL_UNSIGNED_BYTE', 0x1401, (1, 1))\
                    .add('GL_UNSIGNED_SHORT', 0x1403, (1, 1))\
                    .add('GL_UNSIGNED_INT', 0x1405, (1, 1))

Group('InternalFormat').add('GL_DEPTH_COMPONENT', 0x1902, (1, 0))\
                       .add('GL_DEPTH_STENCIL', 0x84F9, (1, 0))\
                       .add('GL_RED', 0x1903, (1, 0))\
                       .add('GL_RG', 0x8227, (3, 0))\
                       .add('GL_RGB', 0x1907, (1, 0))\
                       .add('GL_RGBA', 0x1908, (1, 0))\
                       .add('GL_DEPTH_COMPONENT16', 0x81A5, (1, 4))\
                       .add('GL_DEPTH_COMPONENT24', 0x81A7, (1, 4))\
                       .add('GL_DEPTH_COMPONENT32', 0x81A7, (1, 4))\
                       .add('GL_DEPTH_COMPONENT32F', 0x8CAC, (3, 0))\
                       .add('GL_DEPTH24_STENCIL8', 0x88F0, (3, 0))\
                       .add('GL_DEPTH32F_STENCIL8', 0x8CAD, (3, 0))\
                       .add('GL_STENCIL_INDEX1', 0x8D46, (4, 4))\
                       .add('GL_STENCIL_INDEX4', 0x8D47, (4, 4))\
                       .add('GL_STENCIL_INDEX8', 0x8D48, (4, 4))\
                       .add('GL_STENCIL_INDEX16', 0x8D49, (4, 4))\
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

Group('TextureParameterName').add('GL_DEPTH_STENCIL_TEXTURE_MODE', 0x90EA, (4, 3))\
                             .add('GL_TEXTURE_BASE_LEVEL', 0x813C, (1, 2))\
                             .add('GL_TEXTURE_BORDER_COLOR', 0x1004, (1, 0))\
                             .add('GL_TEXTURE_COMPARE_FUNC', 0x884D, (1, 4))\
                             .add('GL_TEXTURE_COMPARE_MODE', 0x884C, (1, 4))\
                             .add('GL_TEXTURE_LOD_BIAS', 0x8501, (1, 4))\
                             .add('GL_TEXTURE_MIN_FILTER', 0x2801, (1, 0))\
                             .add('GL_TEXTURE_MAG_FILTER', 0x2800, (1, 0))\
                             .add('GL_TEXTURE_MIN_LOD', 0x813A, (1, 2))\
                             .add('GL_TEXTURE_MAX_LOD', 0x813B, (1, 2))\
                             .add('GL_TEXTURE_MAX_LEVEL', 0x813D, (1, 2))\
                             .add('GL_TEXTURE_SWIZZLE_R', 0x8E42, (3, 3))\
                             .add('GL_TEXTURE_SWIZZLE_G', 0x8E43, (3, 3))\
                             .add('GL_TEXTURE_SWIZZLE_B', 0x8E44, (3, 3))\
                             .add('GL_TEXTURE_SWIZZLE_A', 0x8E45, (3, 3))\
                             .add('GL_TEXTURE_SWIZZLE_RGBA', 0x8E46, (3, 3))\
                             .add('GL_TEXTURE_WRAP_S', 0x2802, (1, 0))\
                             .add('GL_TEXTURE_WRAP_T', 0x2803, (1, 0))\
                             .add('GL_TEXTURE_WRAP_R', 0x8072, (1, 2))\
                             .add('GL_TEXTURE_MAX_ANISOTROPY', 0x84FE, (4, 6), ['GL_EXT_texture_filter_anisotropic', 'GL_ARB_texture_filter_anisotropic'])

Group('SamplerParameterName').add('GL_TEXTURE_COMPARE_FUNC', 0x884D, (3, 3))\
                             .add('GL_TEXTURE_COMPARE_MODE', 0x884C, (3, 3))\
                             .add('GL_TEXTURE_MIN_FILTER', 0x2801, (3, 3))\
                             .add('GL_TEXTURE_MAG_FILTER', 0x2800, (3, 3))\
                             .add('GL_TEXTURE_MIN_LOD', 0x813A, (3, 3))\
                             .add('GL_TEXTURE_MAX_LOD', 0x813B, (3, 3))\
                             .add('GL_TEXTURE_WRAP_S', 0x2802, (3, 3))\
                             .add('GL_TEXTURE_WRAP_T', 0x2803, (3, 3))\
                             .add('GL_TEXTURE_WRAP_R', 0x8072, (3, 3))\
                             .add('GL_TEXTURE_LOD_BIAS', 0x8501, (3, 3))\
                             .add('GL_TEXTURE_BORDER_COLOR', 0x1004, (3, 3))\
                             .add('GL_TEXTURE_MAX_ANISOTROPY', 0x84FE, (4, 6), ['GL_ARB_texture_filter_anisotropic'])

Group('TexImage1DTarget').add('GL_TEXTURE_1D', 0x0DE0, (1, 0))\
                         .add('GL_PROXY_TEXTURE_1D', 0x8063, (1, 1))

Group('TexImage2DTarget').add('GL_TEXTURE_2D', 0x0DE1, (1, 0))\
                         .add('GL_PROXY_TEXTURE_2D', 0x8064, (1, 1))\
                         .add('GL_TEXTURE_1D_ARRAY', 0x8C18, (3, 0))\
                         .add('GL_PROXY_TEXTURE_1D_ARRAY', 0x8C19, (3, 0))\
                         .add('GL_TEXTURE_RECTANGLE', 0x84F5, (3, 1))\
                         .add('GL_PROXY_TEXTURE_RECTANGLE', 0x84F7, (3, 1))\
                         .add('GL_TEXTURE_CUBE_MAP_POSITIVE_X', 0x8515, (1, 3))\
                         .add('GL_TEXTURE_CUBE_MAP_NEGATIVE_X', 0x8516, (1, 3))\
                         .add('GL_TEXTURE_CUBE_MAP_POSITIVE_Y', 0x8517, (1, 3))\
                         .add('GL_TEXTURE_CUBE_MAP_NEGATIVE_Y', 0x8518, (1, 3))\
                         .add('GL_TEXTURE_CUBE_MAP_POSITIVE_Z', 0x8519, (1, 3))\
                         .add('GL_TEXTURE_CUBE_MAP_NEGATIVE_Z', 0x851A, (1, 3))\
                         .add('GL_PROXY_TEXTURE_CUBE_MAP', 0x851B, (1, 3))

Group('TexSubImage1DTarget').add('GL_TEXTURE_1D', 0x0DE0, (1, 0))

Group('TexSubImage2DTarget').add('GL_TEXTURE_2D', 0x0DE1, (1, 0))\
                            .add('GL_TEXTURE_1D_ARRAY', 0x8C18, (3, 0))\
                            .add('GL_TEXTURE_CUBE_MAP_POSITIVE_X', 0x8515, (1, 3))\
                            .add('GL_TEXTURE_CUBE_MAP_NEGATIVE_X', 0x8516, (1, 3))\
                            .add('GL_TEXTURE_CUBE_MAP_POSITIVE_Y', 0x8517, (1, 3))\
                            .add('GL_TEXTURE_CUBE_MAP_NEGATIVE_Y', 0x8518, (1, 3))\
                            .add('GL_TEXTURE_CUBE_MAP_POSITIVE_Z', 0x8519, (1, 3))\
                            .add('GL_TEXTURE_CUBE_MAP_NEGATIVE_Z', 0x851A, (1, 3))

Group('TexImage3DTarget').add('GL_TEXTURE_3D', 0x806F, (1, 2))\
                         .add('GL_PROXY_TEXTURE_3D', 0x8070, (1, 2))\
                         .add('GL_TEXTURE_2D_ARRAY', 0x8C1A, (3, 0))\
                         .add('GL_PROXY_TEXTURE_2D_ARRAY', 0x8C1B, (3, 0))

Group('TexSubImage3DTarget').add('GL_TEXTURE_3D', 0x806F, (1, 2))\
                            .add('GL_TEXTURE_2D_ARRAY', 0x8C1A, (3, 0))

Group('CopyTexImage1DTarget').add('GL_TEXTURE_1D', 0x0DE0, (1, 0))

Group('CopyTexImage2DTarget').add('GL_TEXTURE_2D', 0x0DE1, (1, 0))\
                             .add('GL_TEXTURE_CUBE_MAP_POSITIVE_X', 0x8515, (1, 3))\
                             .add('GL_TEXTURE_CUBE_MAP_NEGATIVE_X', 0x8516, (1, 3))\
                             .add('GL_TEXTURE_CUBE_MAP_POSITIVE_Y', 0x8517, (1, 3))\
                             .add('GL_TEXTURE_CUBE_MAP_NEGATIVE_Y', 0x8518, (1, 3))\
                             .add('GL_TEXTURE_CUBE_MAP_POSITIVE_Z', 0x8519, (1, 3))\
                             .add('GL_TEXTURE_CUBE_MAP_NEGATIVE_Z', 0x851A, (1, 3))

Group('TexBufferTarget').add('GL_TEXTURE_BUFFER', 0x8C2A, (3, 1))

Group('TexImage2DMSTarget').add('GL_TEXTURE_2D_MULTISAMPLE', 0x9100, (3, 2))\
                           .add('GL_PROXY_TEXTURE_2D_MULTISAMPLE', 0x9101, (3, 2))

Group('TexImage3DMSTarget').add('GL_TEXTURE_2D_MULTISAMPLE_ARRAY', 0x9102, (3, 2))\
                           .add('GL_PROXY_TEXTURE_2D_MULTISAMPLE_ARRAY', 0x9103, (3, 2))

Group('ShaderType').add('GL_COMPUTE_SHADER', 0x91B9, (4, 2))\
                   .add('GL_VERTEX_SHADER', 0x8B31, (2, 0))\
                   .add('GL_TESS_CONTROL_SHADER', 0x8E88, (4, 0))\
                   .add('GL_TESS_EVALUATION_SHADER', 0x8E87, (4, 0))\
                   .add('GL_GEOMETRY_SHADER', 0x8DD9, (3, 2))\
                   .add('GL_FRAGMENT_SHADER', 0x8B30, (2, 0))

Group('TexCompareMode').add('GL_COMPARE_REF_TO_TEXTURE', 0x884E, (1, 4))\
                       .add('GL_NONE', 0x0, (1, 4))

Group('ProvokingVertex').add('GL_FIRST_VERTEX_CONVENTION', 0x8E4D, (3, 2))\
                        .add('GL_LAST_VERTEX_CONVENTION', 0x8E4E, (3, 2))

Group('PointSpriteCoordOrigin').add('GL_LOWER_LEFT', 0x8CA1, (2, 0))\
                               .add('GL_UPPER_LEFT', 0x8CA2, (2, 0))

Group('BlendEquation').add('GL_FUNC_ADD', 0x8006, (2, 0))\
                      .add('GL_FUNC_SUBTRACT', 0x800A, (2, 0))\
                      .add('GL_FUNC_REVERSE_SUBTRACT', 0x800B, (2, 0))\
                      .add('GL_MIN', 0x8007, (2, 0))\
                      .add('GL_MAX', 0x8008, (2, 0))

Group('FenceType').add('GL_SYNC_FENCE', 0x9116, (3, 2))

Group('FenceCondition').add('GL_SYNC_GPU_COMMANDS_COMPLETE', 0x9117, (3, 2))

Group('QueryType').add('GL_TIMESTAMP', 0x8E28, (3, 3))\
                  .add('GL_SAMPLES_PASSED', 0x8914, (2, 0))\
                  .add('GL_ANY_SAMPLES_PASSED', 0x8C2F, (3, 3))\
                  .add('GL_ANY_SAMPLES_PASSED_CONSERVATIVE', 0x8D6A, (4, 3))\
                  .add('GL_PRIMITIVES_GENERATED', 0x8C87, (2, 0))\
                  .add('GL_TRANSFORM_FEEDBACK_PRIMITIVES_WRITTEN', 0x8C88, (2, 0))\
                  .add('GL_TIME_ELAPSED', 0x88BF, (3, 3))

Attachment = Group('Attachment').add('GL_DEPTH_ATTACHMENT', 0x8D00, (3, 0))\
                                .add('GL_STENCIL_ATTACHMENT', 0x8D20, (3, 0))\
                                .add('GL_DEPTH_STENCIL_ATTACHMENT', 0x821A, (3, 0))
for i in range(32): Attachment.add('GL_COLOR_ATTACHMENT%d'%i, 0x8CE0+i, (3, 0))

Group('VertexAttribType').add('GL_BYTE', 0x1400, (2, 0))\
                         .add('GL_UNSIGNED_BYTE', 0x1401, (2, 0))\
                         .add('GL_SHORT', 0x1402, (2, 0))\
                         .add('GL_UNSIGNED_SHORT', 0x1403, (2, 0))\
                         .add('GL_INT', 0x1404, (2, 0))\
                         .add('GL_UNSIGNED_INT', 0x1405, (2, 0))\
                         .add('GL_FLOAT', 0x1406, (2, 0))\
                         .add('GL_DOUBLE', 0x140A, (2, 0))\
                         .add('GL_FIXED', 0x140C, (2, 0))\
                         .add('GL_INT_2_10_10_10_REV', 0x8D9F, (3, 3))\
                         .add('GL_UNSIGNED_INT_2_10_10_10_REV', 0x8368, (3, 3))\
                         .add('GL_UNSIGNED_INT_10F_11F_11F_REV', 0x8C3B, (4, 4))

Group('DepthStencilTextureMode').add('GL_DEPTH_COMPONENT', 0x1902, (4, 3))\
                                .add('GL_STENCIL_INDEX', 0x1901, (4, 3))

Group('TextureSwizzle').add('GL_RED', 0x1903, (3, 3))\
                       .add('GL_GREEN', 0x1904, (3, 3))\
                       .add('GL_BLUE', 0x1905, (3, 3))\
                       .add('GL_ALPHA', 0x1906, (3, 3))\
                       .add('GL_ZERO', 0, (3, 3))\
                       .add('GL_ONE', 1, (3, 3))

group_dict['GetPName'].add('GL_MAX_TEXTURE_MAX_ANISOTROPY', 0x84FF, (4, 6), ['GL_EXT_texture_filter_anisotropic', 'GL_ARB_texture_filter_anisotropic'])

Func((1, 0), 'glTexParameterfv', [P(tGLenum, 'target', None, 'TextureTarget'),
                                  P(tGLenum, 'pname', None, 'TextureParameterName'),
                                  P(tGLfloat, 'params', 'tex_param_count(pname)')])

Func((1, 0), 'glTexParameteriv', [P(tGLenum, 'target', None, 'TextureTarget'),
                                  P(tGLenum, 'pname', None, 'TextureParameterName'),
                                  P(tGLint, 'params', 'tex_param_count(pname)')])

Func((1, 0), 'glTexImage1D', [P(tGLenum, 'target', None, 'TexImage1DTarget'), P(tGLint, 'level'),
                              P(tGLint, 'internalformat', None, 'InternalFormat'), P(tGLsizei, 'width'),
                              P(tGLint, 'border'), P(tGLenum, 'format', None, PixelFormat2),
                              P(tGLenum, 'type', None, 'PixelType'), P(tTexImageData(1), 'pixels')])

Func((1, 0), 'glTexImage2D', [P(tGLenum, 'target', None, 'TexImage2DTarget'), P(tGLint, 'level'),
                      P(tGLint, 'internalformat', None, 'InternalFormat'), P(tGLsizei, 'width'),
                      P(tGLsizei, 'height'), P(tGLint, 'border'), P(tGLenum, 'format', None, PixelFormat2),
                      P(tGLenum, 'type', None, 'PixelType'), P(tTexImageData(2), 'pixels')])

#Func((1, 0), 'glReadPixels', [P(tGLint, 'x'), P(tGLint, 'y'), P(tGLsizei, 'width'), P(tGLsizei, 'height'), P(tGLenum, 'format'), P(tGLenum, 'type'), P(tMutablePointer, 'pixels')])
Func((1, 0), 'glGetBooleanv', [P(tGLenum, 'pname', None, 'GetPName'), P(tGLboolean, 'data', 1)])
Func((1, 0), 'glGetDoublev', [P(tGLenum, 'pname', None, 'GetPName'), P(tGLdouble, 'data', 1)])
Func((1, 0), 'glGetFloatv', [P(tGLenum, 'pname', None, 'GetPName'), P(tGLfloat, 'data', 1)])
Func((1, 0), 'glGetIntegerv', [P(tGLenum, 'pname', None, 'GetPName'), P(tGLint, 'data', 1)])
Func((1, 0), 'glGetString', [P(tGLenum, 'name', None, 'StringName')], tString)
#Func((1, 0), 'glGetTexImage', [P(tGLenum, 'target'), P(tGLint, 'level'), P(tGLenum, 'format'), P(tGLenum, 'type'), P(tMutablePointer, 'pixels')])

tex_param = g(('GL_DEPTH_STENCIL_TEXTURE_MODE', 4, 3), 'GL_TEXTURE_MAG_FILTER', 'GL_TEXTURE_MIN_FILTER', 'GL_TEXTURE_MIN_LOD', 'GL_TEXTURE_MAX_LOD',
              'GL_TEXTURE_BASE_LEVEL', 'GL_TEXTURE_MAX_LEVEL', 'GL_TEXTURE_SWIZZLE_R', 'GL_TEXTURE_SWIZZLE_G', 'GL_TEXTURE_SWIZZLE_B',
              'GL_TEXTURE_SWIZZLE_A', 'GL_TEXTURE_SWIZZLE_RGBA', 'GL_TEXTURE_WRAP_S', 'GL_TEXTURE_WRAP_T', 'GL_TEXTURE_WRAP_R',
              'GL_TEXTURE_BORDER_COLOR', 'GL_TEXTURE_COMPARE_MODE', 'GL_TEXTURE_COMPARE_FUNC', ('GL_TEXTURE_VIEW_MIN_LEVEL', 4, 3),
              ('GL_TEXTURE_VIEW_NUM_LEVELS', 4, 3), ('GL_TEXTURE_VIEW_MIN_LAYER', 4, 3), ('GL_TEXTURE_VIEW_NUM_LAYERS', 4, 3), ('GL_TEXTURE_IMMUTABLE_LEVELS', 4, 3),
              ('GL_IMAGE_FORMAT_COMPATIBILITY_TYPE', 4, 2), ('GL_TEXTURE_IMMUTABLE_FORMAT', 4, 2), ('GL_TEXTURE_TARGET', 4, 5))
Func((1, 0), 'glGetTexParameterfv', [P(tGLenum, 'target', None, 'TextureTarget'), P(tGLenum, 'pname', None, tex_param), P(tGLfloat, 'params', '(pname==GL_TEXTURE_SWIZZLE_RGBA||pname==GL_TEXTURE_BORDER_COLOR)?4:1')])
Func((1, 0), 'glGetTexParameteriv', [P(tGLenum, 'target', None, 'TextureTarget'), P(tGLenum, 'pname', None, tex_param), P(tGLint, 'params', '(pname==GL_TEXTURE_SWIZZLE_RGBA||pname==GL_TEXTURE_BORDER_COLOR)?4:1')])
Func((3, 0), 'glGetTexParameterIiv', [P(tGLenum, 'target', None, 'TextureTarget'), P(tGLenum, 'pname', None, tex_param), P(tGLint, 'params', '(pname==GL_TEXTURE_SWIZZLE_RGBA||pname==GL_TEXTURE_BORDER_COLOR)?4:1')])
Func((3, 0), 'glGetTexParameterIuiv', [P(tGLenum, 'target', None, 'TextureTarget'), P(tGLenum, 'pname', None, tex_param), P(tGLuint, 'params', '(pname==GL_TEXTURE_SWIZZLE_RGBA||pname==GL_TEXTURE_BORDER_COLOR)?4:1')])
Func((4, 5), 'glGetTextureParameterfv', [P(tGLTex, 'texture'), P(tGLenum, 'pname', None, tex_param), P(tGLfloat, 'params', '(pname==GL_TEXTURE_SWIZZLE_RGBA||pname==GL_TEXTURE_BORDER_COLOR)?4:1')])
Func((4, 5), 'glGetTextureParameterIiv', [P(tGLTex, 'texture'), P(tGLenum, 'pname', None, tex_param), P(tGLint, 'params', '(pname==GL_TEXTURE_SWIZZLE_RGBA||pname==GL_TEXTURE_BORDER_COLOR)?4:1')])
Func((4, 5), 'glGetTextureParameterIuiv', [P(tGLTex, 'texture'), P(tGLenum, 'pname', None, tex_param), P(tGLuint, 'params', '(pname==GL_TEXTURE_SWIZZLE_RGBA||pname==GL_TEXTURE_BORDER_COLOR)?4:1')])
Func((4, 5), 'glGetTextureParameteriv', [P(tGLTex, 'texture'), P(tGLenum, 'pname', None, tex_param), P(tGLint, 'params', '(pname==GL_TEXTURE_SWIZZLE_RGBA||pname==GL_TEXTURE_BORDER_COLOR)?4:1')])

tex_level_param = g('GL_TEXTURE_WIDTH', 'GL_TEXTURE_HEIGHT', 'GL_TEXTURE_DEPTH', 'GL_TEXTURE_INTERNAL_FORMAT',
                    'GL_TEXTURE_RED_TYPE', 'GL_TEXTURE_GREEN_TYPE', 'GL_TEXTURE_BLUE_TYPE', 'GL_TEXTURE_ALPHA_TYPE',
                    'GL_TEXTURE_DEPTH_TYPE', 'GL_TEXTURE_RED_SIZE', 'GL_TEXTURE_GREEN_SIZE', 'GL_TEXTURE_BLUE_SIZE',
                    'GL_TEXTURE_ALPHA_SIZE', 'GL_TEXTURE_DEPTH_SIZE', 'GL_TEXTURE_COMPRESSED',
                    'GL_TEXTURE_COMPRESSED_IMAGE_SIZE', ('GL_TEXTURE_BUFFER_OFFSET', 4, 3), ('GL_TEXTURE_BUFFER_SIZE', 4, 3))
Func((1, 0), 'glGetTexLevelParameterfv', [P(tGLenum, 'target', None, 'TextureTarget'), P(tGLint, 'level'), P(tGLenum, 'pname', None, tex_level_param), P(tGLint, 'params', 1)])
Func((1, 0), 'glGetTexLevelParameteriv', [P(tGLenum, 'target', None, 'TextureTarget'), P(tGLint, 'level'), P(tGLenum, 'pname', None, tex_level_param), P(tGLint, 'params', 1)])
Func((4, 5), 'glGetTextureLevelParameterfv', [P(tGLTex, 'texture'), P(tGLint, 'level'), P(tGLenum, 'pname', None, tex_level_param), P(tGLfloat, 'params', 1)])
Func((4, 5), 'glGetTextureLevelParameteriv', [P(tGLTex, 'texture'), P(tGLint, 'level'), P(tGLenum, 'pname', None, tex_level_param), P(tGLint, 'params', 1)])

Func((1, 1), 'glTexSubImage1D', [P(tGLenum, 'target', None, 'TexSubImage1DTarget'), P(tGLint, 'level'),
                                 P(tGLint, 'xoffset'), P(tGLsizei, 'width'),
                                 P(tGLenum, 'format', None, PixelFormat2),
                                 P(tGLenum, 'type', None, 'PixelType'), P(tTexImageData(1), 'pixels')])

Func((1, 1), 'glTexSubImage2D', [P(tGLenum, 'target', None, 'TexSubImage2DTarget'), P(tGLint, 'level'),
                                 P(tGLint, 'xoffset'), P(tGLint, 'yoffset'), P(tGLsizei, 'width'),
                                 P(tGLsizei, 'height'), P(tGLenum, 'format', None, PixelFormat2),
                                 P(tGLenum, 'type', None, 'PixelType'),  P(tTexImageData(2), 'pixels')])

Func((1, 1), 'glDeleteTextures', [P(tGLsizei, 'n'), P(tGLTex, 'textures', 'n')])
Func((1, 1), 'glGenTextures', [P(tGLsizei, 'n'), P(tGLTex, 'textures', 'n')])

Func((1, 2), 'glDrawRangeElements', [P(tGLenum, 'mode', None, 'PrimitiveType'), P(tGLuint, 'start'),
                                     P(tGLuint, 'end'), P(tGLsizei, 'count'), P(tGLenum, 'type', None, 'ElementType'),
                                     P(tPointer, 'indices')])

Func((1, 2), 'glTexImage3D', [P(tGLenum, 'target', None, 'TexImage3DTarget'), P(tGLint, 'level'),
                              P(tGLint, 'internalformat', None, 'InternalFormat'), P(tGLsizei, 'width'),
                              P(tGLsizei, 'height'), P(tGLsizei, 'depth'), P(tGLint, 'border'),
                              P(tGLenum, 'format', None, PixelFormat2), P(tGLenum, 'type', None, 'PixelType'),
                              P(tTexImageData(3), 'pixels')])

Func((1, 2), 'glTexSubImage3D', [P(tGLenum, 'target', None, 'TexSubImage3DTarget'), P(tGLint, 'level'),
                                 P(tGLint, 'xoffset'), P(tGLint, 'yoffset'), P(tGLint, 'zoffset'),
                                 P(tGLsizei, 'width'), P(tGLsizei, 'height'), P(tGLsizei, 'depth'),
                                 P(tGLenum, 'format', None, PixelFormat2), P(tGLenum, 'type', None, 'PixelType'),
                                 P(tTexImageData(3), 'pixels')])

Func((1, 1), 'glCopyTexImage1D', [P(tGLenum, 'target', None, 'CopyTexImage1DTarget'), P(tGLint, 'level'),
                                  P(tGLenum, 'internalformat', None, 'InternalFormat'), P(tGLint, 'x'), P(tGLint, 'y'),
                                  P(tGLsizei, 'width'), P(tGLint, 'border')])

Func((1, 1), 'glCopyTexImage2D', [P(tGLenum, 'target', None, 'CopyTexImage2DTarget'), P(tGLint, 'level'),
                                  P(tGLenum, 'internalformat', None, 'InternalFormat'), P(tGLint, 'x'),
                                  P(tGLint, 'y'), P(tGLsizei, 'width'), P(tGLsizei, 'height'), P(tGLint, 'border')])

#Func((1, 3), 'glGetCompressedTexImage', [P(tGLenum, 'target'), P(tGLint, 'level'), P(tMutablePointer, 'img')])

Func((1, 4), 'glMultiDrawArrays', [P(tGLenum, 'mode', None, 'PrimitiveType'), P(tGLint, 'first', 'drawcount'),
                           P(tGLsizei, 'count', 'drawcount'), P(tGLsizei, 'drawcount')])

Func((1, 4), 'glMultiDrawElements', [P(tGLenum, 'mode', None, 'PrimitiveType'), P(tGLsizei, 'count', 'drawcount'),
                             P(tGLenum, 'type', None, 'ElementType'), P(tPointer, 'indices', 'drawcount'),
                             P(tGLsizei, 'drawcount')])

Func((1, 4), 'glPointParameterfv', [P(tGLenum, 'pname'), P(tGLfloat, 'params', 1)])
Func((1, 4), 'glPointParameteriv', [P(tGLenum, 'pname'), P(tGLint, 'params', 1)])
Func((1, 5), 'glGenQueries', [P(tGLsizei, 'n'), P(tGLQuery, 'ids', 'n')])
Func((1, 5), 'glDeleteQueries', [P(tGLsizei, 'n'), P(tGLQuery, 'ids', 'n')])
Func((1, 5), 'glGetQueryiv', [P(tGLenum, 'target', None, QueryTarget), P(tGLenum, 'pname'), P(tMutablePointer, 'params')])
Func((1, 5), 'glBeginQuery', [P(tGLenum, 'target', None, QueryTarget), P(tGLQuery, 'id')])
Func((1, 5), 'glEndQuery', [P(tGLenum, 'target', None, QueryTarget)])
Func((3, 3), 'glQueryCounter', [P(tGLQuery, 'id'), P(tGLenum, 'target', None, 'GL_TIMESTAMP')])
Func((4, 0), 'glBeginQueryIndexed', [P(tGLenum, 'target', None, QueryTarget), P(tGLuint, 'index'), P(tGLQuery, 'id')])
Func((4, 0), 'glEndQueryIndexed', [P(tGLenum, 'target', None, QueryTarget), P(tGLuint, 'index')])
Func((1, 5), 'glGetQueryObjectiv', [P(tGLQuery, 'id'), P(tGLenum, 'pname'), P(tGLint, 'params', 1)])
Func((1, 5), 'glGetQueryObjectuiv', [P(tGLQuery, 'id'), P(tGLenum, 'pname'), P(tGLuint, 'params', 1)])
Func((3, 3), 'glGetQueryObjecti64v', [P(tGLQuery, 'id'), P(tGLenum, 'pname'), P(tGLint64, 'params', 1)])
Func((3, 3), 'glGetQueryObjectui64v', [P(tGLQuery, 'id'), P(tGLenum, 'pname'), P(tGLuint64, 'params', 1)])
Func((1, 5), 'glDeleteBuffers', [P(tGLsizei, 'n'), P(tGLBuf, 'buffers', 'n')])
Func((1, 5), 'glGenBuffers', [P(tGLsizei, 'n'), P(tGLBuf, 'buffers', 'n')])

Func((1, 5), 'glBufferData', [P(tGLenum, 'target', None, BufferTarget), P(tGLsizeiptr, 'size'), P(tOptional(tData('size')), 'data'), P(tGLenum, 'usage', None, 'BufferUsageARB')])
Func((1, 5), 'glBufferSubData', [P(tGLenum, 'target', None, BufferTarget), P(tGLintptr, 'offset'), P(tGLsizeiptr, 'size'), P(tData('size>=0?size:0'), 'data')])

Func((1, 5), 'glGetBufferSubData', [P(tGLenum, 'target', None, BufferTarget), P(tGLintptr, 'offset'), P(tGLsizeiptr, 'size'), P(tData('size>=0?size:0'), 'data')])
Func((1, 5), 'glGetBufferParameteriv', [P(tGLenum, 'target', None, BufferTarget), P(tGLenum, 'pname'), P(tGLint, 'params', 1)])
Func((3, 2), 'glGetBufferParameteri64v', [P(tGLenum, 'target', None, BufferTarget), P(tGLenum, 'pname'), P(tGLint64, 'params', 1)])
Func((1, 5), 'glGetBufferPointerv', [P(tGLenum, 'target', None, BufferTarget), P(tGLenum, 'pname'), P(tMutablePointer, 'params', 1)])
DrawBuffersBuffer = g('GL_NONE', 'GL_FRONT_LEFT', 'GL_FRONT_RIGHT', 'GL_BACK_LEFT', 'GL_BACK_RIGHT', name='DrawBuffersBuffer')
for i in range(32): DrawBuffersBuffer.add('GL_COLOR_ATTACHMENT%d'%i, 0x8CE0+i, (3, 0))
Func((2, 0), 'glDrawBuffers', [P(tGLsizei, 'n'), P(tGLenum, 'bufs', 'n', DrawBuffersBuffer)])

DrawBufferBuffer = g('GL_NONE', 'GL_FRONT_LEFT', 'GL_FRONT_RIGHT', 'GL_BACK_LEFT', 'GL_BACK_RIGHT',
                     'GL_FRONT', 'GL_BACK', 'GL_LEFT', 'GL_RIGHT', 'GL_FRONT_AND_BACK', name='DrawBufferBuffer')
for i in range(32): DrawBufferBuffer.add('GL_COLOR_ATTACHMENT%d'%i, 0x8CE0+i, (3, 0))
Func((1, 0), 'glDrawBuffer', [P(tGLenum, 'buf', None, DrawBufferBuffer)])
Func((1, 0), 'glReadBuffer', [P(tGLenum, 'src', None, DrawBufferBuffer)], None)
Func((2, 0), 'glAttachShader', [P(tGLProgram, 'program'), P(tGLShader, 'shader')], None)
Func((2, 0), 'glBindAttribLocation', [P(tGLProgram, 'program'), P(tGLuint, 'index'), P(tString, 'name')], None)
Func((2, 0), 'glCompileShader', [P(tGLShader, 'shader')], None)
Func((2, 0), 'glCreateProgram', [], tGLProgram)
Func((2, 0), 'glDeleteProgram', [P(tGLProgram, 'program')], None)
Func((2, 0), 'glDeleteShader', [P(tGLShader, 'shader')], None)
Func((2, 0), 'glDetachShader', [P(tGLProgram, 'program'), P(tGLShader, 'shader')], None)
#TODO: Update glGetActiveAttrib and glGetActiveUniform
Func((2, 0), 'glGetActiveAttrib', [P(tGLProgram, 'program'), P(tGLuint, 'index'), P(tGLsizei, 'bufSize'), P(tMutablePointer, 'length'), P(tMutablePointer, 'size'), P(tMutablePointer, 'type'), P(tMutableString, 'name')])
Func((2, 0), 'glGetActiveUniform', [P(tGLProgram, 'program'), P(tGLuint, 'index'), P(tGLsizei, 'bufSize'), P(tMutablePointer, 'length'), P(tMutablePointer, 'size'), P(tMutablePointer, 'type'), P(tMutableString, 'name')])
Func((2, 0), 'glGetAttachedShaders', [P(tGLProgram, 'program'), P(tGLsizei, 'maxCount'), P(tGLsizei, 'count', 1), P(tGLShader, 'shaders', 'maxCount')])
Func((2, 0), 'glGetAttribLocation', [P(tGLProgram, 'program'), P(tString, 'name')], tGLint)
Func((2, 0), 'glGetProgramiv', [P(tGLProgram, 'program'), P(tGLenum, 'pname'), P(tGLint, 'params', 1)])
#TODO: Handle when bufSize == 0
Func((2, 0), 'glGetProgramInfoLog', [P(tGLProgram, 'program'), P(tGLsizei, 'bufSize'), P(tOptional(tGLsizei), 'length', 1), P(tMutableString, 'infoLog')])
Func((2, 0), 'glGetShaderiv', [P(tGLShader, 'shader'), P(tGLenum, 'pname'), P(tGLint, 'params', 1)])
#TODO: Handle when bufSize == 0 for both of these
Func((2, 0), 'glGetShaderInfoLog', [P(tGLShader, 'shader'), P(tGLsizei, 'bufSize'), P(tOptional(tGLsizei), 'length', 1), P(tMutableString, 'infoLog')])
Func((2, 0), 'glGetShaderSource', [P(tGLShader, 'shader'), P(tGLsizei, 'bufSize'), P(tOptional(tGLsizei), 'length', 1), P(tMutableString, 'source')])
Func((2, 0), 'glGetUniformLocation', [P(tGLProgram, 'program'), P(tString, 'name')], tGLint)
#Func((2, 0), 'glGetUniformfv', [P(tGLProgram, 'program'), P(tGLint, 'location'), P(tMutablePointer, 'params')])
#Func((2, 0), 'glGetUniformiv', [P(tGLProgram, 'program'), P(tGLint, 'location'), P(tMutablePointer, 'params')])
#Func((2, 0), 'glGetVertexAttribdv', [P(tGLuint, 'index'), P(tGLenum, 'pname'), P(tMutablePointer, 'params')])
#Func((2, 0), 'glGetVertexAttribfv', [P(tGLuint, 'index'), P(tGLenum, 'pname'), P(tMutablePointer, 'params')])
#Func((2, 0), 'glGetVertexAttribiv', [P(tGLuint, 'index'), P(tGLenum, 'pname'), P(tMutablePointer, 'params')])
#Func((2, 0), 'glGetVertexAttribPointerv', [P(tGLuint, 'index'), P(tGLenum, 'pname'), P(tMutablePointer, 'pointer')])
Func((2, 0), 'glShaderSource', [P(tGLShader, 'shader'), P(tGLsizei, 'count'), P(tShdrSrc, 'string', 'count'), P(tOptional(tGLint), 'length', 'count')])
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
Func((2, 0), 'glValidateProgram', [P(tGLProgram, 'program')], None)
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
Func((2, 0), 'glVertexAttrib4usv', [P(tGLuint, 'index'), P(tGLushort, 'v', 4)])
Func((2, 0), 'glVertexAttribPointer', [P(tGLuint, 'index'), P(tGLint, 'size'), P(tGLenum, 'type', None, 'VertexAttribType'), P(tGLboolean, 'normalized'), P(tGLsizei, 'stride'), P(tPointer, 'pointer')])

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
Func((3, 1), 'glBindBufferRange', [P(tGLenum, 'target'), P(tGLuint, 'index'), P(tGLBuf, 'buffer'), P(tGLintptr, 'offset'), P(tGLsizeiptr, 'size')], None)
Func((3, 1), 'glBindBufferBase', [P(tGLenum, 'target'), P(tGLuint, 'index'), P(tGLBuf, 'buffer')], None)
Func((3, 0), 'glTransformFeedbackVaryings', [P(tGLProgram, 'program'), P(tGLsizei, 'count'), P(tString, 'varyings', 'count'), P(tGLenum, 'bufferMode')])
#TODO: Handle when bufSize == 0
Func((3, 0), 'glGetTransformFeedbackVarying', [P(tGLProgram, 'program'), P(tGLuint, 'index'), P(tGLsizei, 'bufSize'), P(tOptional(tGLsizei), 'length', 1), P(tGLsizei, 'size', 1), P(tGLenum, 'type', 1), P(tMutableString, 'name')])
Func((3, 0), 'glBeginConditionalRender', [P(tGLQuery, 'id'), P(tGLenum, 'mode')], None)
Func((3, 0), 'glVertexAttribIPointer', [P(tGLuint, 'index'), P(tGLint, 'size'), P(tGLenum, 'type', None, 'VertexAttribType'), P(tGLsizei, 'stride'), P(tPointer, 'pointer')])
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
#Func((3, 0), 'glGetUniformuiv', [P(tGLProgram, 'program'), P(tGLint, 'location'), P(tMutablePointer, 'params')])
Func((3, 0), 'glBindFragDataLocation', [P(tGLProgram, 'program'), P(tGLuint, 'color'), P(tString, 'name')], None)
Func((3, 0), 'glGetFragDataLocation', [P(tGLProgram, 'program'), P(tString, 'name')], tGLint)
Func((3, 0), 'glUniform1uiv', [P(tGLint, 'location'), P(tGLsizei, 'count'), P(tGLuint, 'value', 'count')])
Func((3, 0), 'glUniform2uiv', [P(tGLint, 'location'), P(tGLsizei, 'count'), P(tGLuint, 'value', 'count*2')])
Func((3, 0), 'glUniform3uiv', [P(tGLint, 'location'), P(tGLsizei, 'count'), P(tGLuint, 'value', 'count*3')])
Func((3, 0), 'glUniform4uiv', [P(tGLint, 'location'), P(tGLsizei, 'count'), P(tGLuint, 'value', 'count*4')])
Func((3, 0), 'glTexParameterIiv', [P(tGLenum, 'target', None, 'TextureTarget'), P(tGLenum, 'pname', None, 'TextureParameterName'), P(tGLint, 'params', 'tex_param_count(pname)')])
Func((3, 0), 'glTexParameterIuiv', [P(tGLenum, 'target', None, 'TextureTarget'), P(tGLenum, 'pname', None, 'TextureParameterName'), P(tGLuint, 'params', 'tex_param_count(pname)')])
Func((3, 0), 'glClearBufferiv', [P(tGLenum, 'buffer', None, 'GL_COLOR GL_STENCIL'), P(tGLint, 'drawbuffer'), P(tGLint, 'value', 'buffer==GL_COLOR?4:1')])
Func((3, 0), 'glClearBufferuiv', [P(tGLenum, 'buffer', None, 'GL_COLOR'), P(tGLint, 'drawbuffer'), P(tGLuint, 'value', 'buffer==GL_COLOR?4:1')])
Func((3, 0), 'glClearBufferfv', [P(tGLenum, 'buffer', None, 'GL_COLOR GL_DEPTH'), P(tGLint, 'drawbuffer'), P(tGLfloat, 'value', 'buffer==GL_COLOR?4:1')])
Func((3, 0), 'glClearBufferfi', [P(tGLenum, 'buffer', None, 'GL_DEPTH_STENCIL'), P(tGLint, 'drawbuffer'), P(tGLfloat, 'depth'), P(tGLint, 'stencil')])
#Func((3, 0), 'glGetStringi', [P(tGLenum, 'name'), P(tGLuint, 'index')], tPointer)
Func((3, 0), 'glDeleteRenderbuffers', [P(tGLsizei, 'n'), P(tGLRenderbuffer, 'renderbuffers', 'n')])
Func((3, 0), 'glGenRenderbuffers', [P(tGLsizei, 'n'), P(tGLRenderbuffer, 'renderbuffers', 'n')])
Func((3, 0), 'glGetRenderbufferParameteriv', [P(tGLenum, 'target'), P(tGLenum, 'pname'), P(tGLint, 'params', 1)])
Func((3, 0), 'glDeleteFramebuffers', [P(tGLsizei, 'n'), P(tGLFramebuffer, 'framebuffers', 'n')])
Func((3, 0), 'glGenFramebuffers', [P(tGLsizei, 'n'), P(tGLFramebuffer, 'framebuffers', 'n')])
#Func((3, 0), 'glGetFramebufferAttachmentParameteriv', [P(tGLenum, 'target', None, FramebufferTarget), P(tGLenum, 'attachment'), P(tGLenum, 'pname'), P(tMutablePointer, 'params')])
Func((3, 0), 'glMapBufferRange', [P(tGLenum, 'target', None, BufferTarget), P(tGLintptr, 'offset'), P(tGLsizeiptr, 'length'), P(tGLbitfield, 'access')], tPointer)
Func((3, 0), 'glDeleteVertexArrays', [P(tGLsizei, 'n'), P(tGLVAO, 'arrays', 'n')])
Func((3, 0), 'glGenVertexArrays', [P(tGLsizei, 'n'), P(tGLVAO, 'arrays', 'n')])

Func((3, 1), 'glDrawElementsInstanced', [P(tGLenum, 'mode', None, 'PrimitiveType'),
                                         P(tGLsizei, 'count'), P(tGLenum, 'type', None, 'ElementType'),
                                         P(tPointer, 'indices'), P(tGLsizei, 'instancecount')])

Func((3, 1), 'glGetUniformIndices', [P(tGLProgram, 'program'), P(tGLsizei, 'uniformCount'), P(tMutableString, 'uniformNames', 'uniformCount'), P(tGLuint, 'uniformIndices', 'uniformCount')])
Func((3, 1), 'glGetActiveUniformsiv', [P(tGLProgram, 'program'), P(tGLsizei, 'uniformCount'), P(tGLuint, 'uniformIndices', 'uniformCount'), P(tGLenum, 'pname'), P(tGLint, 'params', 'uniformCount')])
#TODO: Handle when bufSize == 0 for glGetActiveUniformName and glGetActiveUniformBlockName
Func((3, 1), 'glGetActiveUniformName', [P(tGLProgram, 'program'), P(tGLuint, 'uniformIndex'), P(tGLsizei, 'bufSize'), P(tOptional(tGLsizei), 'length', 1), P(tOptional(tMutableString), 'uniformName')])
Func((3, 1), 'glGetUniformBlockIndex', [P(tGLProgram, 'program'), P(tString, 'uniformBlockName')], tGLuint)
Func((3, 1), 'glGetActiveUniformBlockiv', [P(tGLProgram, 'program'), P(tGLuint, 'uniformBlockIndex'), P(tGLenum, 'pname'), P(tMutablePointer, 'params')]) #TODO: Finish this
Func((3, 1), 'glGetActiveUniformBlockName', [P(tGLProgram, 'program'), P(tGLuint, 'uniformBlockIndex'), P(tGLsizei, 'bufSize'), P(tOptional(tGLsizei), 'length', 1), P(tMutableString, 'uniformBlockName')])
#Func((3, 1), 'glGetIntegeri_v', [P(tGLenum, 'target'), P(tGLuint, 'index'), P(tMutablePointer, 'data')])

Func((3, 2), 'glDrawElementsBaseVertex', [P(tGLenum, 'mode', None, 'PrimitiveType'),
                                          P(tGLsizei, 'count'), P(tGLenum, 'type', None, 'ElementType'),
                                          P(tPointer, 'indices'), P(tGLint, 'basevertex')])

Func((3, 2), 'glDrawRangeElementsBaseVertex', [P(tGLenum, 'mode', None, 'PrimitiveType'),
                                               P(tGLuint, 'start'), P(tGLuint, 'end'),
                                               P(tGLsizei, 'count'), P(tGLenum, 'type', None, 'ElementType'),
                                               P(tPointer, 'indices'), P(tGLint, 'basevertex')])

Func((3, 2), 'glDrawElementsInstancedBaseVertex', [P(tGLenum, 'mode', None, 'PrimitiveType'),
                                                   P(tGLsizei, 'count'), P(tGLenum, 'type', None, 'ElementType'),
                                                   P(tPointer, 'indices'), P(tGLsizei, 'instancecount'),
                                                   P(tGLint, 'basevertex')])

Func((3, 2), 'glMultiDrawElementsBaseVertex', [P(tGLenum, 'mode', None, 'PrimitiveType'),
                                               P(tGLsizei, 'count', 'drawcount'), P(tGLenum, 'type', None, 'ElementType'),
                                               P(tPointer, 'indices', 'drawcount'), P(tGLsizei, 'drawcount'),
                                               P(tGLint, 'basevertex', 'drawcount')])

Func((3, 2), 'glProvokingVertex', [P(tGLenum, 'mode', None, 'ProvokingVertex')])

#Func((3, 2), 'glGetInteger64v', [P(tGLenum, 'pname'), P(tMutablePointer, 'data')])
#Func((3, 2), 'glGetSynciv', [P(tGLsync, 'sync'), P(tGLenum, 'pname'), P(tGLsizei, 'bufSize'), P(tMutablePointer, 'length'), P(tMutablePointer, 'values')])
#Func((3, 2), 'glGetInteger64i_v', [P(tGLenum, 'target'), P(tGLuint, 'index'), P(tMutablePointer, 'data')])
#Func((3, 2), 'glGetMultisamplefv', [P(tGLenum, 'pname'), P(tGLuint, 'index'), P(tMutablePointer, 'val')])
Func((3, 3), 'glBindFragDataLocationIndexed', [P(tGLProgram, 'program'), P(tGLuint, 'colorNumber'), P(tGLuint, 'index'), P(tString, 'name')], None)
Func((3, 3), 'glGetFragDataIndex', [P(tGLProgram, 'program'), P(tString, 'name')], tGLint)
Func((3, 3), 'glGenSamplers', [P(tGLsizei, 'count'), P(tGLSampler, 'samplers', 'count')])
Func((3, 3), 'glDeleteSamplers', [P(tGLsizei, 'count'), P(tGLSampler, 'samplers', 'count')])

Func((3, 3), 'glSamplerParameteri', [P(tGLSampler, 'sampler'), P(tGLenum, 'pname', None, 'SamplerParameterName'),
                                     P(tGLint, 'param')])

Func((3, 3), 'glSamplerParameteriv', [P(tGLSampler, 'sampler'), P(tGLenum, 'pname', None, 'SamplerParameterName'),
                                      P(tGLint, 'param', 'tex_param_count(pname)')])

Func((3, 3), 'glSamplerParameterf', [P(tGLSampler, 'sampler'), P(tGLenum, 'pname', None, 'SamplerParameterName'),
                                     P(tGLfloat, 'param')])

Func((3, 3), 'glSamplerParameterfv', [P(tGLSampler, 'sampler'), P(tGLenum, 'pname', None, 'SamplerParameterName'),
                                      P(tGLfloat, 'param', 'tex_param_count(pname)')])

Func((3, 3), 'glSamplerParameterIiv', [P(tGLSampler, 'sampler'), P(tGLenum, 'pname', None, 'SamplerParameterName'),
                                       P(tGLint, 'param', 'tex_param_count(pname)')])

Func((3, 3), 'glSamplerParameterIuiv', [P(tGLSampler, 'sampler'), P(tGLenum, 'pname', None, 'SamplerParameterName'),
                                        P(tGLuint, 'param', 'tex_param_count(pname)')])

Func((3, 3), 'glGetSamplerParameteriv', [P(tGLSampler, 'sampler'), P(tGLenum, 'pname'), P(tGLint, 'params', 'tex_param_count(pname)')])
Func((3, 3), 'glGetSamplerParameterIiv', [P(tGLSampler, 'sampler'), P(tGLenum, 'pname'), P(tGLint, 'params', 'tex_param_count(pname)')])
Func((3, 3), 'glGetSamplerParameterfv', [P(tGLSampler, 'sampler'), P(tGLenum, 'pname'), P(tGLfloat, 'params', 'tex_param_count(pname)')])
Func((3, 3), 'glGetSamplerParameterIuiv', [P(tGLSampler, 'sampler'), P(tGLenum, 'pname'), P(tGLuint, 'params', 'tex_param_count(pname)')])
Func((3, 3), 'glVertexAttribDivisor', [P(tGLuint, 'index'), P(tGLuint, 'divisor')], None)
Func((3, 3), 'glVertexAttribP1ui', [P(tGLuint, 'index'), P(tGLenum, 'type', None, 'PackedAttribType'), P(tGLboolean, 'normalized', None, 'Boolean'), P(tGLuint, 'value')])
Func((3, 3), 'glVertexAttribP2ui', [P(tGLuint, 'index'), P(tGLenum, 'type', None, 'PackedAttribType'), P(tGLboolean, 'normalized', None, 'Boolean'), P(tGLuint, 'value')])
Func((3, 3), 'glVertexAttribP3ui', [P(tGLuint, 'index'), P(tGLenum, 'type', None, 'PackedAttrib3Type'), P(tGLboolean, 'normalized', None, 'Boolean'), P(tGLuint, 'value')])
Func((3, 3), 'glVertexAttribP4ui', [P(tGLuint, 'index'), P(tGLenum, 'type', None, 'PackedAttribType'), P(tGLboolean, 'normalized', None, 'Boolean'), P(tGLuint, 'value')])
Func((3, 3), 'glVertexAttribP1uiv', [P(tGLuint, 'index'), P(tGLenum, 'type', None, 'PackedAttribType'), P(tGLboolean, 'normalized', None, 'Boolean'), P(tGLuint, 'value', 1)])
Func((3, 3), 'glVertexAttribP2uiv', [P(tGLuint, 'index'), P(tGLenum, 'type', None, 'PackedAttribType'), P(tGLboolean, 'normalized', None, 'Boolean'), P(tGLuint, 'value', 1)])
Func((3, 3), 'glVertexAttribP3uiv', [P(tGLuint, 'index'), P(tGLenum, 'type', None, 'PackedAttrib3Type'), P(tGLboolean, 'normalized', None, 'Boolean'), P(tGLuint, 'value', 1)])
Func((3, 3), 'glVertexAttribP4uiv', [P(tGLuint, 'index'), P(tGLenum, 'type', None, 'PackedAttribType'), P(tGLboolean, 'normalized', None, 'Boolean'), P(tGLuint, 'value', 1)])
#Func((4, 0), 'glDrawArraysIndirect', [P(tGLenum, 'mode', None, 'PrimitiveType'), P(tPointer, 'indirect')])
#Func((4, 0), 'glDrawElementsIndirect', [P(tGLenum, 'mode', None, 'PrimitiveType'), P(tGLenum, 'type', None, 'ElementType'), P(tPointer, 'indirect')])
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

#Func((4, 0), 'glGetUniformdv', [P(tGLProgram, 'program'), P(tGLint, 'location'), P(tMutablePointer, 'params')])
Func((4, 0), 'glGetSubroutineUniformLocation', [P(tGLProgram, 'program'), P(tGLenum, 'shadertype'), P(tString, 'name')], tGLint)
Func((4, 0), 'glGetSubroutineIndex', [P(tGLProgram, 'program'), P(tGLenum, 'shadertype'), P(tString, 'name')], tGLuint)
#Func((4, 0), 'glGetActiveSubroutineUniformiv', [P(tGLProgram, 'program'), P(tGLenum, 'shadertype'), P(tGLuint, 'index'), P(tGLenum, 'pname'), P(tMutablePointer, 'values')])
Func((4, 0), 'glGetActiveSubroutineUniformName', [P(tGLProgram, 'program'), P(tGLenum, 'shadertype'), P(tGLuint, 'index'), P(tGLsizei, 'bufsize'), P(tGLsizei, 'length', 1), P(tGLchar, 'name', 'bufsize')])
Func((4, 0), 'glGetActiveSubroutineName', [P(tGLProgram, 'program'), P(tGLenum, 'shadertype', None, 'ShaderType'), P(tGLuint, 'index'), P(tGLsizei, 'bufsize'), P(tGLsizei, 'length', 'length?1:0'), P(tData('bufsize'), 'name')])
Func((4, 0), 'glUniformSubroutinesuiv', [P(tGLenum, 'shadertype', None, 'ShaderType'), P(tGLsizei, 'count'), P(tGLuint, 'indices', 'count')])
Func((4, 0), 'glGetUniformSubroutineuiv', [P(tGLenum, 'shadertype'), P(tGLint, 'location'), P(tGLuint, 'params', 1)])
Func((4, 0), 'glGetProgramStageiv', [P(tGLProgram, 'program'), P(tGLenum, 'shadertype'), P(tGLenum, 'pname'), P(tGLint, 'values', 1)])
Func((4, 0), 'glPatchParameteri', [P(tGLenum, 'pname', None, g('GL_PATCH_VERTICES')), P(tGLint, 'value')])
Func((4, 0), 'glPatchParameterfv', [P(tGLenum, 'pname', None, g('GL_PATCH_DEFAULT_OUTER_LEVEL', 'GL_PATCH_DEFAULT_INNER_LEVEL')),
                                    P(tGLfloat, 'values', 'pname==GL_PATCH_DEFAULT_OUTER_LEVEL?4:(pname==GL_PATCH_DEFAULT_INNER_LEVEL?2:0)')])
Func((4, 0), 'glDeleteTransformFeedbacks', [P(tGLsizei, 'n'), P(tGLTransformFeedback, 'ids', 'n')])
Func((4, 0), 'glGenTransformFeedbacks', [P(tGLsizei, 'n'), P(tGLTransformFeedback, 'ids', 'n')])
Func((4, 0), 'glGetQueryIndexediv', [P(tGLenum, 'target', None, QueryTarget), P(tGLuint, 'index'), P(tGLenum, 'pname'), P(tGLint, 'params', 1)])
Func((4, 1), 'glShaderBinary', [P(tGLsizei, 'count'), P(tGLShader, 'shaders', 'count'), P(tGLenum, 'binaryformat'), P(tData('length'), 'binary'), P(tGLsizei, 'length')])
Func((4, 1), 'glGetShaderPrecisionFormat', [P(tGLenum, 'shadertype'),
                                            P(tGLenum, 'precisiontype', None, g('GL_LOW_FLOAT', 'GL_MEDIUM_FLOAT', 'GL_HIGH_FLOAT', 'GL_LOW_INT', 'GL_MEDIUM_INT', 'GL_HIGH_INT')),
                                            P(tGLint, 'range', 2), P(tGLint, 'precision', 1)])
Func((4, 1), 'glGetProgramBinary', [P(tGLProgram, 'program'), P(tGLsizei, 'bufSize'), P(tGLsizei, 'length', 'length?1:0'), P(tGLenum, 'binaryFormat', 1), P(tData('bufSize'), 'binary')])
Func((4, 1), 'glProgramBinary', [P(tGLProgram, 'program'), P(tGLenum, 'binaryFormat'), P(tData('length'), 'binary'), P(tGLsizei, 'length')])
Func((4, 1), 'glBindProgramPipeline', [P(tGLProgramPipeline, 'pipeline')], None)
Func((4, 1), 'glUseProgramStages', [P(tGLProgramPipeline, 'pipeline'), P(tGLbitfield, 'stages'), P(tGLProgram, 'program')], None)
Func((4, 1), 'glActiveShaderProgram', [P(tGLProgramPipeline, 'pipeline'), P(tGLProgram, 'program')], None)
Func((4, 1), 'glProgramParameteri', [P(tGLProgram, 'program'), P(tGLenum, 'pname', None, g('GL_PROGRAM_BINARY_RETRIEVABLE_HINT', 'GL_PROGRAM_SEPARABLE')),
                                     P(tGLint, 'value', None, 'Boolean')])
Func((4, 1), 'glCreateShaderProgramv', [P(tGLenum, 'type', None, 'ShaderType'), P(tGLsizei, 'count'), P(tString, 'strings', 'count')], tGLProgram).trace_extras_code = 'link_program_extras(result);'
Func((2, 0), 'glCreateShader', [P(tGLenum, 'type', None, 'ShaderType')], tGLShader)
Func((4, 1), 'glDeleteProgramPipelines', [P(tGLsizei, 'n'), P(tGLProgramPipeline, 'pipelines', 'n')])
Func((4, 1), 'glGenProgramPipelines', [P(tGLsizei, 'n'), P(tGLProgramPipeline, 'pipelines', 'n')])
Func((4, 1), 'glGetProgramPipelineiv', [P(tGLProgramPipeline, 'pipeline'), P(tGLenum, 'pname'), P(tGLint, 'params', 1)])

Func((4, 1), 'glProgramUniform1iv', [P(tGLProgram, 'program'), P(tGLint, 'location'),
                             P(tGLsizei, 'count'), P(tGLint, 'value', 'count')])

Func((4, 1), 'glProgramUniform1fv', [P(tGLProgram, 'program'), P(tGLint, 'location'),
                             P(tGLsizei, 'count'), P(tGLfloat, 'value', 'count')])

Func((4, 1), 'glProgramUniform1dv', [P(tGLProgram, 'program'), P(tGLint, 'location'),
                             P(tGLsizei, 'count'), P(tGLdouble, 'value', 'count')])

Func((4, 1), 'glProgramUniform1uiv', [P(tGLProgram, 'program'), P(tGLint, 'location'),
                              P(tGLsizei, 'count'), P(tGLuint, 'value', 'count')])

Func((4, 1), 'glProgramUniform2iv', [P(tGLProgram, 'program'), P(tGLint, 'location'),
                             P(tGLsizei, 'count'), P(tGLint, 'value', 'count*2')])

Func((4, 1), 'glProgramUniform2fv', [P(tGLProgram, 'program'), P(tGLint, 'location'),
                             P(tGLsizei, 'count'), P(tGLfloat, 'value', 'count*2')])

Func((4, 1), 'glProgramUniform2dv', [P(tGLProgram, 'program'), P(tGLint, 'location'),
                             P(tGLsizei, 'count'), P(tGLdouble, 'value', 'count*2')])

Func((4, 1), 'glProgramUniform2uiv', [P(tGLProgram, 'program'), P(tGLint, 'location'),
                              P(tGLsizei, 'count'), P(tGLuint, 'value', 'count*2')])

Func((4, 1), 'glProgramUniform3iv', [P(tGLProgram, 'program'), P(tGLint, 'location'),
                             P(tGLsizei, 'count'), P(tGLint, 'value', 'count*3')])

Func((4, 1), 'glProgramUniform3fv', [P(tGLProgram, 'program'), P(tGLint, 'location'),
                             P(tGLsizei, 'count'), P(tGLfloat, 'value', 'count*3')])

Func((4, 1), 'glProgramUniform3dv', [P(tGLProgram, 'program'), P(tGLint, 'location'),
                             P(tGLsizei, 'count'), P(tGLdouble, 'value', 'count*3')])

Func((4, 1), 'glProgramUniform3uiv', [P(tGLProgram, 'program'), P(tGLint, 'location'),
                              P(tGLsizei, 'count'), P(tGLuint, 'value', 'count*3')])

Func((4, 1), 'glProgramUniform4iv', [P(tGLProgram, 'program'), P(tGLint, 'location'),
                             P(tGLsizei, 'count'), P(tGLint, 'value', 'count*4')])

Func((4, 1), 'glProgramUniform4fv', [P(tGLProgram, 'program'), P(tGLint, 'location'),
                             P(tGLsizei, 'count'), P(tGLfloat, 'value', 'count*4')])

Func((4, 1), 'glProgramUniform4dv', [P(tGLProgram, 'program'), P(tGLint, 'location'),
                             P(tGLsizei, 'count'), P(tGLdouble, 'value', 'count*4')])

Func((4, 1), 'glProgramUniform4uiv', [P(tGLProgram, 'program'), P(tGLint, 'location'),
                              P(tGLsizei, 'count'), P(tGLuint, 'value', 'count*4')])

Func((4, 1), 'glProgramUniformMatrix2fv', [P(tGLProgram, 'program'), P(tGLint, 'location'),
                                   P(tGLsizei, 'count'), P(tGLboolean, 'transpose', None, 'Boolean'),
                                   P(tGLfloat, 'value', 'count*4')])

Func((4, 1), 'glProgramUniformMatrix3fv', [P(tGLProgram, 'program'), P(tGLint, 'location'),
                                   P(tGLsizei, 'count'), P(tGLboolean, 'transpose', None, 'Boolean'),
                                   P(tGLfloat, 'value', 'count*9')])

Func((4, 1), 'glProgramUniformMatrix4fv', [P(tGLProgram, 'program'), P(tGLint, 'location'),
                                   P(tGLsizei, 'count'), P(tGLboolean, 'transpose', None, 'Boolean'),
                                   P(tGLfloat, 'value', 'count*16')])

Func((4, 1), 'glProgramUniformMatrix2dv', [P(tGLProgram, 'program'), P(tGLint, 'location'),
                                   P(tGLsizei, 'count'), P(tGLboolean, 'transpose', None, 'Boolean'),
                                   P(tGLdouble, 'value', 'count*4')])

Func((4, 1), 'glProgramUniformMatrix3dv', [P(tGLProgram, 'program'), P(tGLint, 'location'), P(tGLsizei, 'count'), P(tGLboolean, 'transpose', None, 'Boolean'), P(tGLdouble, 'value', 'count*9')])
Func((4, 1), 'glProgramUniformMatrix4dv', [P(tGLProgram, 'program'), P(tGLint, 'location'), P(tGLsizei, 'count'), P(tGLboolean, 'transpose', None, 'Boolean'), P(tGLdouble, 'value', 'count*16')])
Func((4, 1), 'glProgramUniformMatrix2x3fv', [P(tGLProgram, 'program'), P(tGLint, 'location'), P(tGLsizei, 'count'), P(tGLboolean, 'transpose', None, 'Boolean'), P(tGLfloat, 'value', 'count*6')])
Func((4, 1), 'glProgramUniformMatrix3x2fv', [P(tGLProgram, 'program'), P(tGLint, 'location'), P(tGLsizei, 'count'), P(tGLboolean, 'transpose', None, 'Boolean'), P(tGLfloat, 'value', 'count*6')])
Func((4, 1), 'glProgramUniformMatrix2x4fv', [P(tGLProgram, 'program'), P(tGLint, 'location'), P(tGLsizei, 'count'), P(tGLboolean, 'transpose', None, 'Boolean'), P(tGLfloat, 'value', 'count*8')])
Func((4, 1), 'glProgramUniformMatrix4x2fv', [P(tGLProgram, 'program'), P(tGLint, 'location'), P(tGLsizei, 'count'), P(tGLboolean, 'transpose', None, 'Boolean'), P(tGLfloat, 'value', 'count*8')])
Func((4, 1), 'glProgramUniformMatrix3x4fv', [P(tGLProgram, 'program'), P(tGLint, 'location'), P(tGLsizei, 'count'), P(tGLboolean, 'transpose', None, 'Boolean'), P(tGLfloat, 'value', 'count*12')])
Func((4, 1), 'glProgramUniformMatrix4x3fv', [P(tGLProgram, 'program'), P(tGLint, 'location'), P(tGLsizei, 'count'), P(tGLboolean, 'transpose', None, 'Boolean'), P(tGLfloat, 'value', 'count*12')])
Func((4, 1), 'glProgramUniformMatrix2x3dv', [P(tGLProgram, 'program'), P(tGLint, 'location'), P(tGLsizei, 'count'), P(tGLboolean, 'transpose', None, 'Boolean'), P(tGLdouble, 'value', 'count*6')])
Func((4, 1), 'glProgramUniformMatrix3x2dv', [P(tGLProgram, 'program'), P(tGLint, 'location'), P(tGLsizei, 'count'), P(tGLboolean, 'transpose', None, 'Boolean'), P(tGLdouble, 'value', 'count*6')])
Func((4, 1), 'glProgramUniformMatrix2x4dv', [P(tGLProgram, 'program'), P(tGLint, 'location'), P(tGLsizei, 'count'), P(tGLboolean, 'transpose', None, 'Boolean'), P(tGLdouble, 'value', 'count*8')])
Func((4, 1), 'glProgramUniformMatrix4x2dv', [P(tGLProgram, 'program'), P(tGLint, 'location'), P(tGLsizei, 'count'), P(tGLboolean, 'transpose', None, 'Boolean'), P(tGLdouble, 'value', 'count*8')])
Func((4, 1), 'glProgramUniformMatrix3x4dv', [P(tGLProgram, 'program'), P(tGLint, 'location'), P(tGLsizei, 'count'), P(tGLboolean, 'transpose', None, 'Boolean'), P(tGLdouble, 'value', 'count*12')])
Func((4, 1), 'glProgramUniformMatrix4x3dv', [P(tGLProgram, 'program'), P(tGLint, 'location'), P(tGLsizei, 'count'), P(tGLboolean, 'transpose', None, 'Boolean'), P(tGLdouble, 'value', 'count*12')])
Func((4, 1), 'glValidateProgramPipeline', [P(tGLProgramPipeline, 'pipeline')], None)
#TODO: Handle when bufSize == 0
Func((4, 1), 'glGetProgramPipelineInfoLog', [P(tGLProgramPipeline, 'pipeline'), P(tGLsizei, 'bufSize'), P(tOptional(tGLsizei), 'length', 1), P(tMutableString, 'infoLog')])
Func((4, 1), 'glVertexAttribL1dv', [P(tGLuint, 'index'), P(tGLdouble, 'v', 4)])
Func((4, 1), 'glVertexAttribL2dv', [P(tGLuint, 'index'), P(tGLdouble, 'v', 4)])
Func((4, 1), 'glVertexAttribL3dv', [P(tGLuint, 'index'), P(tGLdouble, 'v', 4)])
Func((4, 1), 'glVertexAttribL4dv', [P(tGLuint, 'index'), P(tGLdouble, 'v', 4)])
Func((4, 1), 'glVertexAttribLPointer', [P(tGLuint, 'index'), P(tGLint, 'size'), P(tGLenum, 'type', None, 'VertexAttribType'), P(tGLsizei, 'stride'), P(tPointer, 'pointer')])
#Func((4, 1), 'glGetVertexAttribLdv', [P(tGLuint, 'index'), P(tGLenum, 'pname'), P(tMutablePointer, 'params')])
Func((4, 1), 'glViewportArrayv', [P(tGLuint, 'first'), P(tGLsizei, 'count'), P(tGLfloat, 'v', 'count*4')])
Func((4, 1), 'glViewportIndexedfv', [P(tGLuint, 'index'), P(tGLfloat, 'v', 4)])
Func((4, 1), 'glScissorArrayv', [P(tGLuint, 'first'), P(tGLsizei, 'count'), P(tGLint, 'v', 'count*4')])
Func((4, 1), 'glScissorIndexedv', [P(tGLuint, 'index'), P(tGLint, 'v', 4)])
Func((4, 1), 'glDepthRangeArrayv', [P(tGLuint, 'first'), P(tGLsizei, 'count'), P(tGLdouble, 'v', 'count*2')])
#Func((4, 1), 'glGetFloati_v', [P(tGLenum, 'target'), P(tGLuint, 'index'), P(tMutablePointer, 'data')])
#Func((4, 1), 'glGetDoublei_v', [P(tGLenum, 'target'), P(tGLuint, 'index'), P(tMutablePointer, 'data')])

Func((4, 2), 'glDrawElementsInstancedBaseInstance', [P(tGLenum, 'mode', None, 'PrimitiveType'), P(tGLsizei, 'count'),
                                                     P(tGLenum, 'type', None, 'ElementType'), P(tPointer, 'indices'),
                                                     P(tGLsizei, 'instancecount'), P(tGLuint, 'baseinstance')])

Func((4, 2), 'glDrawElementsInstancedBaseVertexBaseInstance', [P(tGLenum, 'mode', None, 'PrimitiveType'), P(tGLsizei, 'count'),
                                                               P(tGLenum, 'type', None, 'ElementType'), P(tPointer, 'indices'),
                                                               P(tGLsizei, 'instancecount'), P(tGLint, 'basevertex'),
                                                               P(tGLuint, 'baseinstance')])

#Func((4, 2), 'glGetInternalformativ', [P(tGLenum, 'target'), P(tGLenum, 'internalformat'), P(tGLenum, 'pname'), P(tGLsizei, 'bufSize'), P(tMutablePointer, 'params')])
#Func((4, 2), 'glGetActiveAtomicCounterBufferiv', [P(tGLProgram, 'program'), P(tGLuint, 'bufferIndex'), P(tGLenum, 'pname'), P(tMutablePointer, 'params')])
Func((4, 3), 'glClearBufferData', [P(tGLenum, 'target', None, BufferTarget), P(tGLenum, 'internalformat'), P(tGLenum, 'format'), P(tGLenum, 'type'), P(tData('get_texel_size(format, type)'), 'data')])
Func((4, 3), 'glClearBufferSubData', [P(tGLenum, 'target', None, BufferTarget), P(tGLenum, 'internalformat'), P(tGLintptr, 'offset'), P(tGLsizeiptr, 'size'), P(tGLenum, 'format'), P(tGLenum, 'type'), P(tData('get_texel_size(format, type)'), 'data')])
#Func((4, 3), 'glGetFramebufferParameteriv', [P(tGLenum, 'target', None, FramebufferTarget), P(tGLenum, 'pname'), P(tMutablePointer, 'params')])
#Func((4, 3), 'glGetInternalformati64v', [P(tGLenum, 'target'), P(tGLenum, 'internalformat'), P(tGLenum, 'pname'), P(tGLsizei, 'bufSize'), P(tMutablePointer, 'params')])
#Func((4, 3), 'glInvalidateFramebuffer', [P(tGLenum, 'target', None, FramebufferTarget), P(tGLsizei, 'numAttachments'), P(tPointer, 'attachments')])
#Func((4, 3), 'glInvalidateSubFramebuffer', [P(tGLenum, 'target', None, FramebufferTarget), P(tGLsizei, 'numAttachments'), P(tPointer, 'attachments'), P(tGLint, 'x'), P(tGLint, 'y'), P(tGLsizei, 'width'), P(tGLsizei, 'height')])
#Func((4, 3), 'glMultiDrawArraysIndirect', [P(tGLenum, 'mode', None, 'Boolean'), P(tPointer, 'indirect'), P(tGLsizei, 'drawcount'), P(tGLsizei, 'stride')])
#Func((4, 3), 'glMultiDrawElementsIndirect', [P(tGLenum, 'mode', None, 'Boolean'), P(tGLenum, 'type', None, 'ElementType'), P(tPointer, 'indirect'), P(tGLsizei, 'drawcount'), P(tGLsizei, 'stride')])
Func((4, 3), 'glGetProgramInterfaceiv', [P(tGLProgram, 'program'), P(tGLenum, 'programInterface'), P(tGLenum, 'pname'), P(tGLint, 'params', 1)])
#Func((4, 3), 'glGetProgramResourceName', [P(tGLProgram, 'program'), P(tGLenum, 'programInterface'), P(tGLuint, 'index'), P(tGLsizei, 'bufSize'), P(tMutablePointer, 'length'), P(tMutableString, 'name')])
#Func((4, 3), 'glGetProgramResourceiv', [P(tGLProgram, 'program'), P(tGLenum, 'programInterface'), P(tGLuint, 'index'), P(tGLsizei, 'propCount'), P(tPointer, 'props'), P(tGLsizei, 'bufSize'), P(tMutablePointer, 'length'), P(tMutablePointer, 'params')])
Func((4, 3), 'glBindVertexBuffer', [P(tGLuint, 'bindingindex'), P(tGLBuf, 'buffer'), P(tGLintptr, 'offset'), P(tGLsizei, 'stride')], None)
#Func((4, 3), 'glDebugMessageControl', [P(tGLenum, 'source'), P(tGLenum, 'type'), P(tGLenum, 'severity'), P(tGLsizei, 'count'), P(tPointer, 'ids'), P(tGLboolean, 'enabled')])
#Func((4, 3), 'glDebugMessageCallback', [P(tGLDEBUGPROC, 'callback'), P(tPointer, 'userP')])
#Func((4, 3), 'glGetDebugMessageLog', [P(tGLuint, 'count'), P(tGLsizei, 'bufSize'), P(tMutablePointer, 'sources'), P(tMutablePointer, 'types'), P(tMutablePointer, 'ids'), P(tMutablePointer, 'severities'), P(tMutablePointer, 'lengths'), P(tMutableString, 'messageLog')], tGLuint)
#Func((4, 3), 'glGetObjectLabel', [P(tGLenum, 'identifier'), P(tGLuint, 'name'), P(tGLsizei, 'bufSize'), P(tMutablePointer, 'length'), P(tMutableString, 'label')])
#Func((4, 3), 'glObjectPtrLabel', [P(tPointer, 'ptr'), P(tGLsizei, 'length'), P(tString, 'label')])
#Func((4, 3), 'glGetObjectPtrLabel', [P(tPointer, 'ptr'), P(tGLsizei, 'bufSize'), P(tMutablePointer, 'length'), P(tMutableString, 'label')])
Func((4, 3), 'glGetPointerv', [P(tGLenum, 'pname'), P(tMutablePointer, 'params', 1)])
#Func((4, 4), 'glBufferStorage', [P(tGLenum, 'target', None, BufferTarget), P(tGLsizeiptr, 'size'), P(tPointer, 'data'), P(tGLbitfield, 'flags')])
#Func((4, 4), 'glClearTexImage', [P(tGLTex, 'texture'), P(tGLint, 'level'), P(tGLenum, 'format'), P(tGLenum, 'type'), P(tPointer, 'data')])
#Func((4, 4), 'glClearTexSubImage', [P(tGLTex, 'texture'), P(tGLint, 'level'), P(tGLint, 'xoffset'), P(tGLint, 'yoffset'), P(tGLint, 'zoffset'), P(tGLsizei, 'width'), P(tGLsizei, 'height'), P(tGLsizei, 'depth'), P(tGLenum, 'format'), P(tGLenum, 'type'), P(tPointer, 'data')])
Func((1, 5), 'glBindBuffer', [P(tGLenum, 'target', None, BufferTarget), P(tGLBuf, 'buffer')])
#TODO: Handle when buffers is NULL
Func((4, 4), 'glBindBuffersBase', [P(tGLenum, 'target', None, BufferTarget), P(tGLuint, 'first'), P(tGLsizei, 'count'), P(tGLBuf, 'buffers', 'count')])
#TODO: Handle when buffers is NULL
Func((4, 4), 'glBindBuffersRange', [P(tGLenum, 'target', None, BufferTarget), P(tGLuint, 'first'), P(tGLsizei, 'count'), P(tGLBuf, 'buffers', 'count'), P(tGLintptr, 'offsets', 'count'), P(tGLintptr, 'sizes', 'count')])
Func((4, 4), 'glBindTextures', [P(tGLuint, 'first'), P(tGLsizei, 'count'), P(tGLTex, 'textures', 'count')])
Func((4, 4), 'glBindSamplers', [P(tGLuint, 'first'), P(tGLsizei, 'count'), P(tGLSampler, 'samplers', 'count')])
Func((4, 4), 'glBindImageTextures', [P(tGLuint, 'first'), P(tGLsizei, 'count'), P(tGLTex, 'textures', 'count')])
Func((4, 4), 'glBindVertexBuffers', [P(tGLuint, 'first'), P(tGLsizei, 'count'), P(tGLBuf, 'buffers', 'count'), P(tGLintptr, 'offsets', 'count'), P(tGLsizei, 'strides', 'count')])
Func((4, 5), 'glClipControl', [P(tGLenum, 'origin', None, 'ClipOrigin'), P(tGLenum, 'depth', None, 'ClipDepth')])
Func((4, 5), 'glCreateTransformFeedbacks', [P(tGLsizei, 'n'), P(tGLTransformFeedback, 'ids', 'n')])
Func((4, 5), 'glGetTransformFeedbackiv', [P(tGLTransformFeedback, 'xfb'), P(tGLenum, 'pname'), P(tGLint, 'param', 1)])
Func((4, 5), 'glGetTransformFeedbacki_v', [P(tGLTransformFeedback, 'xfb'), P(tGLenum, 'pname'), P(tGLuint, 'index'), P(tGLint, 'param', 1)])
Func((4, 5), 'glGetTransformFeedbacki64_v', [P(tGLTransformFeedback, 'xfb'), P(tGLenum, 'pname'), P(tGLuint, 'index'), P(tGLint64, 'param', 1)])
Func((4, 5), 'glCreateBuffers', [P(tGLsizei, 'n'), P(tGLBuf, 'buffers', 'n')])
#Func((4, 5), 'glNamedBufferStorage', [P(tGLBuf, 'buffer'), P(tGLsizeiptr, 'size'), P(tPointer, 'data'), P(tGLbitfield, 'flags')])
Func((4, 5), 'glNamedBufferData', [P(tGLBuf, 'buffer'), P(tGLsizeiptr, 'size'), P(tOptional(tData('size')), 'data'), P(tGLenum, 'usage')])
Func((4, 5), 'glNamedBufferSubData', [P(tGLBuf, 'buffer'), P(tGLintptr, 'offset'), P(tGLsizeiptr, 'size'), P(tData('size'), 'data')])
Func((4, 5), 'glCopyNamedBufferSubData', [P(tGLBuf, 'readBuffer'), P(tGLBuf, 'writeBuffer'), P(tGLintptr, 'readOffset'), P(tGLintptr, 'writeOffset'), P(tGLsizeiptr, 'size')])
Func((4, 5), 'glClearNamedBufferData', [P(tGLBuf, 'buffer'), P(tGLenum, 'internalformat'), P(tGLenum, 'format'), P(tGLenum, 'type'), P(tData('get_texel_size(format, type)'), 'data')])
Func((4, 5), 'glClearNamedBufferSubData', [P(tGLBuf, 'buffer'), P(tGLenum, 'internalformat'), P(tGLintptr, 'offset'), P(tGLsizeiptr, 'size'), P(tGLenum, 'format'), P(tGLenum, 'type'), P(tData('get_texel_size(format, type)'), 'data')])
Func((4, 5), 'glMapNamedBuffer', [P(tGLBuf, 'buffer'), P(tGLenum, 'access')], tPointer)
Func((4, 5), 'glMapNamedBufferRange', [P(tGLBuf, 'buffer'), P(tGLintptr, 'offset'), P(tGLsizeiptr, 'length'), P(tGLbitfield, 'access')], tPointer)
Func((4, 5), 'glFlushMappedNamedBufferRange', [P(tGLBuf, 'buffer'), P(tGLintptr, 'offset'), P(tGLsizeiptr, 'length')], None)
Func((4, 5), 'glGetNamedBufferParameteriv', [P(tGLBuf, 'buffer'), P(tGLenum, 'pname'), P(tGLint, 'params', 1)])
Func((4, 5), 'glGetNamedBufferParameteri64v', [P(tGLBuf, 'buffer'), P(tGLenum, 'pname'), P(tGLint64, 'params', 1)])
Func((4, 5), 'glGetNamedBufferPointerv', [P(tGLBuf, 'buffer'), P(tGLenum, 'pname'), P(tMutablePointer, 'params', 1)])
Func((4, 5), 'glGetNamedBufferSubData', [P(tGLBuf, 'buffer'), P(tGLintptr, 'offset'), P(tGLsizeiptr, 'size'), P(tData('size>=0?size:0'), 'data')])
Func((4, 5), 'glCreateFramebuffers', [P(tGLsizei, 'n'), P(tGLFramebuffer, 'framebuffers', 'n')])
Func((4, 5), 'glNamedFramebufferRenderbuffer', [P(tGLFramebuffer, 'framebuffer'), P(tGLenum, 'attachment'), P(tGLenum, 'renderbuffertarget'), P(tGLRenderbuffer, 'renderbuffer')])
Func((4, 5), 'glNamedFramebufferTexture', [P(tGLFramebuffer, 'framebuffer'), P(tGLenum, 'attachment'), P(tGLTex, 'texture'), P(tGLint, 'level')])
Func((4, 5), 'glNamedFramebufferTextureLayer', [P(tGLFramebuffer, 'framebuffer'), P(tGLenum, 'attachment'), P(tGLTex, 'texture'), P(tGLint, 'level'), P(tGLint, 'layer')])
Func((4, 5), 'glNamedFramebufferDrawBuffers', [P(tGLFramebuffer, 'framebuffer'), P(tGLsizei, 'n'), P(tGLenum, 'bufs', 'n')])
Func((4, 5), 'glInvalidateNamedFramebufferData', [P(tGLFramebuffer, 'framebuffer'), P(tGLsizei, 'numAttachments'), P(tGLenum, 'attachments', 'numAttachments')])
Func((4, 5), 'glInvalidateNamedFramebufferSubData', [P(tGLFramebuffer, 'framebuffer'), P(tGLsizei, 'numAttachments'), P(tGLenum, 'attachments', 'numAttachments'), P(tGLint, 'x'), P(tGLint, 'y'), P(tGLsizei, 'width'), P(tGLsizei, 'height')])
Func((4, 5), 'glClearNamedFramebufferiv', [P(tGLFramebuffer, 'framebuffer'), P(tGLenum, 'buffer', None, 'GL_COLOR GL_STENCIL'), P(tGLint, 'drawbuffer'), P(tGLint, 'value', 'buffer==GL_COLOR?4:1')])
Func((4, 5), 'glClearNamedFramebufferuiv', [P(tGLFramebuffer, 'framebuffer'), P(tGLenum, 'buffer', None, 'GL_COLOR'), P(tGLint, 'drawbuffer'), P(tGLuint, 'value', 'buffer==GL_COLOR?4:1')])
Func((4, 5), 'glClearNamedFramebufferfv', [P(tGLFramebuffer, 'framebuffer'), P(tGLenum, 'buffer', None, 'GL_COLOR GL_DEPTH'), P(tGLint, 'drawbuffer'), P(tGLfloat, 'value', 'buffer==GL_COLOR?4:1')])
Func((4, 5), 'glBlitNamedFramebuffer', [P(tGLFramebuffer, 'readFramebuffer'), P(tGLFramebuffer, 'drawFramebuffer'), P(tGLint, 'srcX0'),
                                        P(tGLint, 'srcY0'), P(tGLint, 'srcX1'), P(tGLint, 'srcY1'), P(tGLint, 'dstX0'), P(tGLint, 'dstY0'),
                                        P(tGLint, 'dstX1'), P(tGLint, 'dstY1'), P(tGLbitfield, 'mask'), P(tGLenum, 'filter')])
Func((4, 5), 'glCheckNamedFramebufferStatus', [P(tGLFramebuffer, 'framebuffer'), P(tGLenum, 'target')], tGLenum)
Func((4, 5), 'glGetNamedFramebufferParameteriv', [P(tGLFramebuffer, 'framebuffer'), P(tGLenum, 'pname'), P(tGLint, 'param', 1)])
Func((4, 5), 'glGetNamedFramebufferAttachmentParameteriv', [P(tGLFramebuffer, 'framebuffer'), P(tGLenum, 'attachment'), P(tGLenum, 'pname'), P(tGLint, 'params', 1)])
Func((4, 5), 'glCreateRenderbuffers', [P(tGLsizei, 'n'), P(tGLRenderbuffer, 'renderbuffers', 'n')])
Func((4, 5), 'glGetNamedRenderbufferParameteriv', [P(tGLRenderbuffer, 'renderbuffer'), P(tGLenum, 'pname'), P(tGLint, 'params', 1)])
Func((4, 5), 'glCreateTextures', [P(tGLenum, 'target'), P(tGLsizei, 'n'), P(tGLTex, 'textures', 'n')])

Func((4, 5), 'glTextureSubImage1D', [P(tGLTex, 'texture'), P(tGLint, 'level'), P(tGLint, 'xoffset'),
                                     P(tGLsizei, 'width'), P(tGLenum, 'format', None, PixelFormat2),
                                     P(tGLenum, 'type', None, 'PixelType'), P(tTexImageData(1), 'pixels')])

Func((4, 5), 'glTextureSubImage2D', [P(tGLTex, 'texture'), P(tGLint, 'level'), P(tGLint, 'xoffset'),
                                     P(tGLint, 'yoffset'), P(tGLsizei, 'width'), P(tGLsizei, 'height'),
                                     P(tGLenum, 'format', None, PixelFormat2), P(tGLenum, 'type', None, 'PixelType'),
                                     P(tTexImageData(2), 'pixels')])

Func((4, 5), 'glTextureSubImage3D', [P(tGLTex, 'texture'), P(tGLint, 'level'), P(tGLint, 'xoffset'), P(tGLint, 'yoffset'),
                                     P(tGLint, 'zoffset'), P(tGLsizei, 'width'), P(tGLsizei, 'height'), P(tGLsizei, 'depth'),
                                     P(tGLenum, 'format', None, PixelFormat2), P(tGLenum, 'type', None, 'PixelType'),
                                     P(tTexImageData(3), 'pixels')])

Func((4, 5), 'glTextureParameterf', [P(tGLTex, 'texture'), P(tGLenum, 'pname', None, 'TextureParameterName'), Param(tGLfloat, 'param')])
Func((4, 5), 'glTextureParameteri', [P(tGLTex, 'texture'), P(tGLenum, 'pname', None, 'TextureParameterName'), Param(tGLint, 'param')])
Func((4, 5), 'glTextureParameterfv', [P(tGLTex, 'texture'), P(tGLenum, 'pname'), P(tGLfloat, 'param', 'tex_param_count(pname)')])
Func((4, 5), 'glTextureParameterIiv', [P(tGLTex, 'texture'), P(tGLenum, 'pname'), P(tGLint, 'params', 'tex_param_count(pname)')])
Func((4, 5), 'glTextureParameterIuiv', [P(tGLTex, 'texture'), P(tGLenum, 'pname'), P(tGLuint, 'params', 'tex_param_count(pname)')])
Func((4, 5), 'glTextureParameteriv', [P(tGLTex, 'texture'), P(tGLenum, 'pname'), P(tGLint, 'param', 'tex_param_count(pname)')])
Func((4, 5), 'glGenerateTextureMipmap', [P(tGLTex, 'texture')], None)
Func((4, 5), 'glBindTextureUnit', [P(tGLuint, 'unit'), P(tGLTex, 'texture')])
#Func((4, 5), 'glGetTextureImage', [P(tGLTex, 'texture'), P(tGLint, 'level'), P(tGLenum, 'format'), P(tGLenum, 'type'), P(tGLsizei, 'bufSize'), P(tMutablePointer, 'pixels')])
#Func((4, 5), 'glGetCompressedTextureImage', [P(tGLTex, 'texture'), P(tGLint, 'level'), P(tGLsizei, 'bufSize'), P(tMutablePointer, 'pixels')])
Func((4, 5), 'glCreateVertexArrays', [P(tGLsizei, 'n'), P(tGLVAO, 'arrays', 'n')])
Func((4, 5), 'glDisableVertexArrayAttrib', [P(tGLVAO, 'vaobj'), P(tGLuint, 'index')], None)
Func((4, 5), 'glEnableVertexArrayAttrib', [P(tGLVAO, 'vaobj'), P(tGLuint, 'index')], None)
Func((4, 5), 'glVertexArrayElementBuffer', [P(tGLVAO, 'vaobj'), P(tGLBuf, 'buffer')], None)
Func((4, 5), 'glVertexArrayVertexBuffer', [P(tGLVAO, 'vaobj'), P(tGLuint, 'bindingindex'), P(tGLBuf, 'buffer'), P(tGLintptr, 'offset'), P(tGLsizei, 'stride')], None)
Func((4, 5), 'glVertexArrayVertexBuffers', [P(tGLVAO, 'vaobj'), P(tGLuint, 'first'), P(tGLsizei, 'count'), P(tGLBuf, 'buffers', 'count'), P(tGLintptr, 'offsets', 'count'), P(tGLsizei, 'strides', 'count')])
Func((4, 5), 'glVertexArrayAttribBinding', [P(tGLVAO, 'vaobj'), P(tGLuint, 'attribindex'), P(tGLuint, 'bindingindex')], None)
Func((4, 5), 'glVertexArrayAttribFormat', [P(tGLVAO, 'vaobj'), P(tGLuint, 'attribindex'), P(tGLint, 'size'), P(tGLenum, 'type'), P(tGLboolean, 'normalized'), P(tGLuint, 'relativeoffset')], None)
Func((4, 5), 'glVertexArrayAttribIFormat', [P(tGLVAO, 'vaobj'), P(tGLuint, 'attribindex'), P(tGLint, 'size'), P(tGLenum, 'type'), P(tGLuint, 'relativeoffset')], None)
Func((4, 5), 'glVertexArrayAttribLFormat', [P(tGLVAO, 'vaobj'), P(tGLuint, 'attribindex'), P(tGLint, 'size'), P(tGLenum, 'type'), P(tGLuint, 'relativeoffset')], None)
Func((4, 5), 'glVertexArrayBindingDivisor', [P(tGLVAO, 'vaobj'), P(tGLuint, 'bindingindex'), P(tGLuint, 'divisor')], None)
Func((4, 5), 'glGetVertexArrayiv', [P(tGLVAO, 'vaobj'), P(tGLenum, 'pname', 'GL_ELEMENT_ARRAY_BUFFER_BINDING'), P(tGLint, 'param', 1)])
Func((4, 5), 'glGetVertexArrayIndexediv', [P(tGLVAO, 'vaobj'), P(tGLuint, 'index'), P(tGLenum, 'pname'), P(tGLint64, 'param', 1)])
Func((4, 5), 'glGetVertexArrayIndexed64iv', [P(tGLVAO, 'vaobj'), P(tGLuint, 'index'), P(tGLenum, 'pname'), P(tGLint, 'param', 1)])
Func((4, 5), 'glCreateSamplers', [P(tGLsizei, 'n'), P(tGLSampler, 'samplers', 'n')])
Func((4, 5), 'glCreateProgramPipelines', [P(tGLsizei, 'n'), P(tGLProgramPipeline, 'pipelines', 'n')])
Func((4, 5), 'glCreateQueries', [P(tGLenum, 'target'), P(tGLsizei, 'n'), P(tGLQuery, 'ids', 'n')])
Func((4, 5), 'glGetTextureSubImage', [P(tGLTex, 'texture'), P(tGLint, 'level'), P(tGLint, 'xoffset'), P(tGLint, 'yoffset'), P(tGLint, 'zoffset'), P(tGLsizei, 'width'), P(tGLsizei, 'height'), P(tGLsizei, 'depth'), P(tGLenum, 'format'), P(tGLenum, 'type'), P(tGLsizei, 'bufSize'), P(tData('bufSize'), 'pixels')])
Func((4, 5), 'glGetCompressedTextureSubImage', [P(tGLTex, 'texture'), P(tGLint, 'level'), P(tGLint, 'xoffset'), P(tGLint, 'yoffset'), P(tGLint, 'zoffset'), P(tGLsizei, 'width'), P(tGLsizei, 'height'), P(tGLsizei, 'depth'), P(tGLsizei, 'bufSize'), P(tData('bufSize'), 'pixels')])
Func((4, 5), 'glGetnCompressedTexImage', [P(tGLenum, 'target'), P(tGLint, 'lod'), P(tGLsizei, 'bufSize'), P(tData('bufSize'), 'pixels')])
Func((4, 5), 'glGetnTexImage', [P(tGLenum, 'target'), P(tGLint, 'level'), P(tGLenum, 'format'), P(tGLenum, 'type'), P(tGLsizei, 'bufSize'), P(tData('bufSize'), 'pixels')])
Func((4, 5), 'glGetnUniformdv', [P(tGLProgram, 'program'), P(tGLint, 'location'), P(tGLsizei, 'bufSize'), P(tGLdouble, 'params', 'bufSize/sizeof(GLdouble)')])
Func((4, 5), 'glGetnUniformfv', [P(tGLProgram, 'program'), P(tGLint, 'location'), P(tGLsizei, 'bufSize'), P(tGLfloat, 'params', 'bufSize/sizeof(GLfloat)')])
Func((4, 5), 'glGetnUniformiv', [P(tGLProgram, 'program'), P(tGLint, 'location'), P(tGLsizei, 'bufSize'), P(tGLint, 'params', 'bufSize/sizeof(GLint)')])
Func((4, 5), 'glGetnUniformuiv', [P(tGLProgram, 'program'), P(tGLint, 'location'), P(tGLsizei, 'bufSize'), P(tGLuint, 'params', 'bufSize/sizeof(GLuint)')])
#Func((4, 5), 'glReadnPixels', [P(tGLint, 'x'), P(tGLint, 'y'), P(tGLsizei, 'width'), P(tGLsizei, 'height'), P(tGLenum, 'format'), P(tGLenum, 'type'), P(tGLsizei, 'bufSize'), P(tMutablePointer, 'data')])

Func((4, 3), 'glTexBufferRange', [P(tGLenum, 'target', None, 'TexBufferTarget'),
                                  P(tGLenum, 'internalformat', None, 'InternalFormat'),
                                  P(tGLBuf, 'buffer'), P(tGLintptr, 'offset'),
                                  P(tGLsizeiptr, 'size')])
Func((3, 1), 'glTexBuffer', [P(tGLenum, 'target', None, 'TexBufferTarget'),
                             P(tGLenum, 'internalformat', None, 'InternalFormat'), P(tGLBuf, 'buffer')])
Func((4, 5), 'glTextureBuffer', [P(tGLTex, 'texture'), P(tGLenum, 'internalformat'), P(tGLBuf, 'buffer')])
Func((4, 5), 'glTextureBufferRange', [P(tGLTex, 'texture'), P(tGLenum, 'internalformat'), P(tGLBuf, 'buffer'), P(tGLintptr, 'offset'), P(tGLsizeiptr, 'size')])

Func((4, 3), 'glTexStorage2DMultisample', [P(tGLenum, 'target', None, 'TexImage2DMSTarget'), P(tGLsizei, 'samples'),
                                           P(tGLenum, 'internalformat', None, 'InternalFormat'),
                                           P(tGLsizei, 'width'), P(tGLsizei, 'height'),
                                           P(tGLboolean, 'fixedsamplelocations', None, 'Boolean')])

Func((4, 3), 'glTexStorage3DMultisample', [P(tGLenum, 'target', None, 'TexImage3DMSTarget'), P(tGLsizei, 'samples'),
                                           P(tGLenum, 'internalformat', None, 'InternalFormat'),
                                           P(tGLsizei, 'width'), P(tGLsizei, 'height'), P(tGLsizei, 'depth'),
                                           P(tGLboolean, 'fixedsamplelocations', None, 'Boolean')])

Func((3, 2), 'glTexImage2DMultisample', [P(tGLenum, 'target', None, 'TexImage2DMSTarget'), P(tGLsizei, 'samples'), P(tGLenum, 'internalformat', None, 'InternalFormat'),
                                         P(tGLsizei, 'width'), P(tGLsizei, 'height'), P(tGLboolean, 'fixedsamplelocations', None, 'Boolean')])

Func((3, 2), 'glTexImage3DMultisample', [P(tGLenum, 'target', None, 'TexImage3DMSTarget'), P(tGLsizei, 'samples'), Param(tGLenum, 'internalformat', None, 'InternalFormat'),
                                         P(tGLsizei, 'width'), P(tGLsizei, 'height'), P(tGLsizei, 'depth'), P(tGLboolean, 'fixedsamplelocations', None, 'Boolean')])

Func((4, 3), 'glTextureView', [P(tGLTex, 'texture'), P(tGLenum, 'target'), P(tGLTex, 'origtexture'), P(tGLenum, 'internalformat', None, 'InternalFormat'),
                               P(tGLuint, 'minlevel'), P(tGLuint, 'numlevels'), P(tGLuint, 'minlayer'), P(tGLuint, 'numlayers')])

Func((1, 3), 'glCompressedTexImage3D', [P(tGLenum, 'target', None, 'TexImage3DTarget'), P(tGLint, 'level'),
                                        P(tGLenum, 'internalformat'), P(tGLsizei, 'width'), P(tGLsizei, 'height'), P(tGLsizei, 'depth'),
                                        P(tGLint, 'border'), P(tGLsizei, 'imageSize'), P(tData('imageSize'), 'data')])

Func((1, 3), 'glCompressedTexImage2D', [P(tGLenum, 'target', None, 'TexImage2DTarget'), P(tGLint, 'level'),
                                        P(tGLenum, 'internalformat'), P(tGLsizei, 'width'), P(tGLsizei, 'height'), P(tGLint, 'border'),
                                        P(tGLsizei, 'imageSize'), P(tData('imageSize'), 'data')])

Func((1, 3), 'glCompressedTexImage1D', [P(tGLenum, 'target', None, 'TexImage1DTarget'), P(tGLint, 'level'), P(tGLenum, 'internalformat'), P(tGLsizei, 'width'),
                                        P(tGLint, 'border'), P(tGLsizei, 'imageSize'), P(tData('imageSize'), 'data')])

Func((1, 3), 'glCompressedTexSubImage3D', [P(tGLenum, 'target', None, 'TexSubImage3DTarget'), P(tGLint, 'level'), P(tGLint, 'xoffset'),
                                           P(tGLint, 'yoffset'), P(tGLint, 'zoffset'), P(tGLsizei, 'width'), P(tGLsizei, 'height'), P(tGLsizei, 'depth'),
                                           P(tGLenum, 'format'), P(tGLsizei, 'imageSize'), P(tData('imageSize'), 'data')])

Func((1, 3), 'glCompressedTexSubImage2D', [P(tGLenum, 'target', None, 'TexSubImage2DTarget'), P(tGLint, 'level'), P(tGLint, 'xoffset'), P(tGLint, 'yoffset'),
                                           P(tGLsizei, 'width'), P(tGLsizei, 'height'), P(tGLenum, 'format'), P(tGLsizei, 'imageSize'), P(tData('imageSize'), 'data')])

Func((1, 3), 'glCompressedTexSubImage1D', [P(tGLenum, 'target', None, 'TexSubImage1DTarget'), P(tGLint, 'level'), P(tGLint, 'xoffset'),
                                           P(tGLsizei, 'width'), P(tGLenum, 'format'), P(tGLsizei, 'imageSize'), P(tData('imageSize'), 'data')])

Func((4, 5), 'glCompressedTextureSubImage1D', [P(tGLTex, 'texture'), P(tGLint, 'level'), P(tGLint, 'xoffset'), P(tGLsizei, 'width'),
                                               P(tGLenum, 'format'), P(tGLsizei, 'imageSize'), P(tData('imageSize'), 'data')])

Func((4, 5), 'glCompressedTextureSubImage2D', [P(tGLTex, 'texture'), P(tGLint, 'level'), P(tGLint, 'xoffset'), P(tGLint, 'yoffset'), P(tGLsizei, 'width'),
                                               P(tGLsizei, 'height'), P(tGLenum, 'format'), P(tGLsizei, 'imageSize'), P(tData('imageSize'), 'data')])

Func((4, 5), 'glCompressedTextureSubImage3D', [P(tGLTex, 'texture'), P(tGLint, 'level'), P(tGLint, 'xoffset'), P(tGLint, 'yoffset'),
                                               P(tGLint, 'zoffset'), P(tGLsizei, 'width'), P(tGLsizei, 'height'), P(tGLsizei, 'depth'),
                                               P(tGLenum, 'format'), P(tGLsizei, 'imageSize'), Param(tData('imageSize'), 'data')])

Func((4, 2), 'glTexStorage1D', [P(tGLenum, 'target', None, 'TexImage1DTarget'), P(tGLsizei, 'levels'),
                                P(tGLenum, 'internalformat', None, 'InternalFormat'), P(tGLsizei, 'width')])

Func((4, 2), 'glTexStorage2D', [P(tGLenum, 'target', None, 'TexImage2DTarget'), P(tGLsizei, 'levels'),
                                P(tGLenum, 'internalformat', None, 'InternalFormat'), P(tGLsizei, 'width'), P(tGLsizei, 'height')])

Func((4, 2), 'glTexStorage3D', [P(tGLenum, 'target', None, 'TexImage3DTarget'), P(tGLsizei, 'levels'),
                                P(tGLenum, 'internalformat', None, 'InternalFormat'),
                                P(tGLsizei, 'width'), P(tGLsizei, 'height'), P(tGLsizei, 'depth')])

Func((1, 1), 'glCopyTexSubImage1D', [P(tGLenum, 'target', None, 'CopyTexImage1DTarget'), P(tGLint, 'level'),
                                     P(tGLint, 'xoffset'), P(tGLint, 'x'), P(tGLint, 'y'), P(tGLsizei, 'width')])

Func((1, 1), 'glCopyTexSubImage2D', [P(tGLenum, 'target', None, 'CopyTexImage2DTarget'), P(tGLint, 'level'), P(tGLint, 'xoffset'),
                                     P(tGLint, 'yoffset'), P(tGLint, 'x'), P(tGLint, 'y'), P(tGLsizei, 'width'), P(tGLsizei, 'height')])

group = Group()
for i in range(1024): group = group.add('GL_TEXTURE%d'%i, 0x84c0+i, (1, 3))
Func((1, 3), 'glActiveTexture', [P(tGLenum, 'texture', None, group)])

Func((3, 0), 'glRenderbufferStorage', [P(tGLenum, 'target'), P(tGLenum, 'internalformat', None, 'InternalFormat'),
                                       P(tGLsizei, 'width'), P(tGLsizei, 'height')])

Func((3, 0), 'glRenderbufferStorageMultisample', [P(tGLenum, 'target'), P(tGLsizei, 'samples'),
                                                  P(tGLenum, 'internalformat', None, 'InternalFormat'),
                                                  P(tGLsizei, 'width'), P(tGLsizei, 'height')])

Func((4, 5), 'glNamedRenderbufferStorage', [P(tGLRenderbuffer, 'renderbuffer'), P(tGLenum, 'internalformat', None, 'InternalFormat'),
                                            P(tGLsizei, 'width'), P(tGLsizei, 'height')])
Func((4, 5), 'glNamedRenderbufferStorageMultisample', [P(tGLRenderbuffer, 'renderbuffer'), P(tGLsizei, 'samples'), P(tGLenum, 'internalformat', None, 'InternalFormat'),
                                                       P(tGLsizei, 'width'), P(tGLsizei, 'height')])

Func((1, 1), 'glDrawArrays', [P(tGLenum, 'mode', None, 'PrimitiveType'), P(tGLint, 'first'),
                      P(tGLsizei, 'count')])

Func((1, 1), 'glDrawElements', [P(tGLenum, 'mode', None, 'PrimitiveType'), P(tGLsizei, 'count'),
                                P(tGLenum, 'type', None, 'ElementType'), P(tPointer, 'indices')])

Func((3, 1), 'glDrawArraysInstanced', [P(tGLenum, 'mode', None, 'PrimitiveType'),
                               P(tGLint, 'first'), P(tGLsizei, 'count'),
                               P(tGLsizei, 'instancecount')])

Func((4, 2), 'glDrawArraysInstancedBaseInstance', [P(tGLenum, 'mode', None, 'PrimitiveType'),
                                           P(tGLint, 'first'), P(tGLsizei, 'count'),
                                           P(tGLsizei, 'instancecount'),
                                           P(tGLuint, 'baseinstance')])

Func((2, 0), 'glLinkProgram', [P(tGLProgram, 'program', None)], None).trace_extras_code = 'link_program_extras(program);'
Func((1, 0), 'glViewport', [P(tGLint, 'x'), P(tGLint, 'y'),
                    P(tGLsizei, 'width'), P(tGLsizei, 'height')]).trace_epilogue_code = 'update_drawable_size();'

f = Func((1, 5), 'glUnmapBuffer', [P(tGLenum, 'target', None, BufferTarget)], tGLboolean)
f.trace_prologue_code = '''
GLint mapped=0, access=0;
GLint64 offset=0, length=0;
F(glGetBufferParameteriv)(target, GL_BUFFER_MAPPED, &mapped);
if (mapped) {
    F(glGetBufferParameteriv)(target, GL_BUFFER_ACCESS, &access);
    F(glGetBufferParameteri64v)(target, GL_BUFFER_MAP_OFFSET, &offset);
    F(glGetBufferParameteri64v)(target, GL_BUFFER_MAP_LENGTH, &length);
}
'''
f.trace_extras_code = '''if (mapped && access!=GL_READ_ONLY) {
    uint64_t offset_le = htole64(offset);
    uint8_t* data = malloc(length+8);
    memcpy(data, &offset_le, 8);
    F(glGetBufferSubData)(target, offset, length, data+8);
    gl_add_extra("replay/glUnmapBuffer/data_ranged", length+8, data);
    free(data);
}
'''

f = Func((4, 5), 'glUnmapNamedBuffer', [P(tGLBuf, 'buffer')], tGLboolean)
f.trace_prologue_code = '''
GLint mapped=0, access=0;
GLint64 offset=0, length=0;
F(glGetNamedBufferParameteriv)(buffer, GL_BUFFER_MAPPED, &mapped);
if (mapped) {
    F(glGetNamedBufferParameteriv)(buffer, GL_BUFFER_ACCESS, &access);
    F(glGetNamedBufferParameteri64v)(buffer, GL_BUFFER_MAP_OFFSET, &offset);
    F(glGetNamedBufferParameteri64v)(buffer, GL_BUFFER_MAP_LENGTH, &length);
}
'''
f.trace_extras_code = '''if (mapped && access!=GL_READ_ONLY) {
    uint64_t offset_le = htole64(offset);
    uint8_t* data = malloc(length+8);
    memcpy(data, &offset_le, 8);
    F(glGetNamedBufferSubData)(buffer, offset, length, data+8);
    gl_add_extra("replay/glUnmapBuffer/data_ranged", length+8, data);
    free(data);
}
'''

Func((3, 0), 'glBindFramebuffer', [P(tGLenum, 'target', None, FramebufferTarget), P(tGLFramebuffer, 'framebuffer')])
Func((3, 0), 'glCheckFramebufferStatus', [P(tGLenum, 'target', None, FramebufferTarget)], tGLenum)
Func((3, 0), 'glFramebufferTexture1D', [P(tGLenum, 'target', None, FramebufferTarget), P(tGLenum, 'attachment', None, 'Attachment'), P(tGLenum, 'textarget'), P(tGLTex, 'texture'), P(tGLint, 'level')])
Func((3, 0), 'glFramebufferTexture2D', [P(tGLenum, 'target', None, FramebufferTarget), P(tGLenum, 'attachment', None, 'Attachment'), P(tGLenum, 'textarget'), P(tGLTex, 'texture'), P(tGLint, 'level')])
Func((3, 0), 'glFramebufferTexture3D', [P(tGLenum, 'target', None, FramebufferTarget), P(tGLenum, 'attachment', None, 'Attachment'), P(tGLenum, 'textarget'), P(tGLTex, 'texture'), P(tGLint, 'level'), P(tGLint, 'zoffset')])
Func((3, 0), 'glFramebufferRenderbuffer', [P(tGLenum, 'target', None, FramebufferTarget), P(tGLenum, 'attachment', None, 'Attachment'), P(tGLenum, 'renderbuffertarget'), P(tGLRenderbuffer, 'renderbuffer')])
#TODO: Update glGetFramebufferAttachmentParameteriv
Func((3, 0), 'glGetFramebufferAttachmentParameteriv', [P(tGLenum, 'target', None, FramebufferTarget), P(tGLenum, 'attachment', None, 'Attachment'), P(tGLenum, 'pname'), P(tMutablePointer, 'params')])
Func((3, 0), 'glFramebufferTextureLayer', [P(tGLenum, 'target', None, FramebufferTarget), P(tGLenum, 'attachment', None, 'Attachment'), P(tGLTex, 'texture'), P(tGLint, 'level'), P(tGLint, 'layer')])
Func((3, 2), 'glFramebufferTexture', [P(tGLenum, 'target', None, FramebufferTarget), P(tGLenum, 'attachment', None, 'Attachment'), P(tGLTex, 'texture'), P(tGLint, 'level')])
Func((4, 3), 'glFramebufferParameteri', [P(tGLenum, 'target', None, FramebufferTarget), P(tGLenum, 'pname'), P(tGLint, 'param')])
Func((4, 3), 'glGetFramebufferParameteriv', [P(tGLenum, 'target', None, FramebufferTarget), P(tGLenum, 'pname'), P(tGLint, 'params', 1)])

wip15DrawableSize(None, 'wip15DrawableSize', [P(tGLsizei, 'width'), P(tGLsizei, 'height')])
wip15BeginTest(None, 'wip15BeginTest', [P(tString, 'name')])
wip15EndTest(None, 'wip15EndTest', [])
wip15PrintTestResults(None, 'wip15PrintTestResults', [])
wip15TestFB(None, 'wip15TestFB', [P(tString, 'name'), P(tData('drawable_width*drawable_height*4'), 'color'),
                                  P(tData('drawable_width*drawable_height*4'), 'depth')])
wip15ExpectPropertyi64(None, 'wip15ExpectPropertyi64', [P(tGLenum, 'objType'), P(tGLuint64, 'objName'), P(tString, 'name'), P(tGLuint64, 'index'), P(tGLint64, 'val')])
wip15ExpectPropertyd(None, 'wip15ExpectPropertyd', [P(tGLenum, 'objType'), P(tGLuint64, 'objName'), P(tString, 'name'), P(tGLuint64, 'index'), P(tGLdouble, 'val')])
wip15ExpectPropertybv(None, 'wip15ExpectPropertybv', [P(tGLenum, 'objType'), P(tGLuint64, 'objName'), P(tString, 'name'), P(tGLuint64, 'index'), P(tGLuint64, 'size'), P(tData('size'), 'data')])
wip15ExpectAttachment(None, 'wip15ExpectAttachment', [P(tString, 'attachment')])

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
Func(None, 'glXGetCurrentDisplay', [], tMutablePointer)
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
f = Func(None, 'glXCreateContextAttribsARB', [P(tMutablePointer, 'dpy'), P(tGLXFBConfig, 'config'), P(tGLXContext, 'share_context'), P(tBool, 'direct'), P(tint, 'attrib_list', 'glx_attrib_int_count(attrib_list)')], tGLXContext)
f.trace_epilogue_code = 'update_drawable_size();'
#Func(None, 'glXDelayBeforeSwapNV', [P(tMutablePointer, 'dpy'), P(tGLXDrawable, 'drawable'), P(tGLfloat, 'seconds')], tBool)
#Func(None, 'glXImportContextEXT', [P(tMutablePointer, 'dpy'), P(tGLXContextID, 'contextID')], tGLXContext)
#Func(None, 'glXSelectEvent', [P(tMutablePointer, 'dpy'), P(tGLXDrawable, 'draw'), P(tunsignedlong, 'event_mask')])
#Func(None, 'glXGetVideoInfoNV', [P(tMutablePointer, 'dpy'), P(tint, 'screen'), P(tGLXVideoDeviceNV, 'VideoDevice'), P(tMutablePointer, 'pulCounterOutputPbuffer'), P(tMutablePointer, 'pulCounterOutputVideo')], tint)
#Func(None, 'glXGetSyncValuesOML', [P(tMutablePointer, 'dpy'), P(tGLXDrawable, 'drawable'), P(tMutablePointer, 'ust'), P(tMutablePointer, 'msc'), P(tMutablePointer, 'sbc')], tBool)
#Func(None, 'glXDeleteAssociatedContextAMD', [P(tGLXContext, 'ctx')], tBool)
glXGetProcAddressFunc(None, 'glXGetProcAddressARB', [P(tString, 'procName')], t__GLXextFuncPtr)
#Func(None, 'glXEnumerateVideoDevicesNV', [P(tMutablePointer, 'dpy'), P(tint, 'screen'), P(tMutablePointer, 'nelements')], tPointer)
f = Func(None, 'glXCreateContext', [P(tMutablePointer, 'dpy'), P(tMutablePointer, 'vis'), P(tGLXContext, 'shareList'), P(tBool, 'direct')], tGLXContext)
f.trace_epilogue_code = 'update_drawable_size();'
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
f = Func(None, 'glXSwapBuffers', [P(tMutablePointer, 'dpy'), P(tGLXDrawable, 'drawable')])
f.trace_epilogue_code = 'update_drawable_size();'
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
f = Func(None, 'glXMakeCurrent', [P(tMutablePointer, 'dpy', None), P(tGLXDrawable, 'drawable', None), P(tGLXContext, 'ctx', None)], tBool)
f.trace_epilogue_code = 'glx_make_current_epilogue();'
f.trace_extras_code = '''
int32_t size[2] = {-1, -1};
if (dpy && drawable!=None) {
    F(glXQueryDrawable)(dpy, drawable, GLX_WIDTH, &size[0]);
    F(glXQueryDrawable)(dpy, drawable, GLX_HEIGHT, &size[1]);
}
gl_add_extra("replay/glXMakeCurrent/drawable_size", 8, size);
create_make_current_config_extra(ctx);
'''
