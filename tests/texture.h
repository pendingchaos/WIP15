static GLuint list1d;
static GLuint list2d;
static GLuint list3d;

void draw_texture1d() {
    glClear(GL_COLOR_BUFFER_BIT);
    glEnable(GL_TEXTURE_1D);
    glDisable(GL_TEXTURE_2D);
    glDisable(GL_TEXTURE_3D);
    glCallList(list1d);
}

void draw_texture2d() {
    glClear(GL_COLOR_BUFFER_BIT);
    glDisable(GL_TEXTURE_1D);
    glEnable(GL_TEXTURE_2D);
    glDisable(GL_TEXTURE_3D);
    glCallList(list2d);
}

void draw_texture3d() {
    glClear(GL_COLOR_BUFFER_BIT);
    glDisable(GL_TEXTURE_1D);
    glDisable(GL_TEXTURE_2D);
    glEnable(GL_TEXTURE_3D);
    glCallList(list3d);
}

//TODO: Sometimes these textures result in a black screen.
void test_tex(GLenum internal, GLenum format, GLenum type) {
    glCurrentTestWIP15(static_format("Texture test with %u internal format, %u format and %u type", internal, format, type));
    
    uint8_t data[4096];
    #define GEN_DATA switch (type) {\
    case GL_UNSIGNED_SHORT:\
    case GL_SHORT:\
        for (size_t i = 0; i < 2048; i++)\
            ((uint16_t*)data)[i] = rand()%2 ? 0xFFFF : 0;\
        break;\
    case GL_UNSIGNED_INT:\
    case GL_INT:\
        for (size_t i = 0; i < 1024; i++)\
            ((uint32_t*)data)[i] = rand()%2 ? 0xFFFFFFFF : 0;\
        break;\
    case GL_FLOAT:\
        for (size_t i = 0; i < 1024; i++)\
            ((float*)data)[i] = rand()%2 ? 1.0f : 0.0f;\
        break;\
    default:\
        for (size_t i = 0; i < 4096; i++)\
            data[i] = rand()%2 ? 0xFF : 0;\
        break;\
    }
    
    GEN_DATA
    glTexImage1D(GL_TEXTURE_1D, 0, internal, 256, 0, format, type, data);
    glTexImage2D(GL_TEXTURE_2D, 0, internal, 16, 16, 0, format, type, data);
    glTexImage3D(GL_TEXTURE_3D, 0, internal, 4, 4, 4, 0, format, type, data);
    draw_texture1d();
    draw_texture2d();
    draw_texture3d();
    
    GEN_DATA
    glTexSubImage1D(GL_TEXTURE_1D, 0, 0, 256, format, type, data);
    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, 16, 16, format, type, data);
    glTexSubImage3D(GL_TEXTURE_3D, 0, 0, 0, 0, 4, 4, 4, format, type, data);
    draw_texture1d();
    draw_texture2d();
    draw_texture3d();
}

void texture_test() {
    glCurrentTestWIP15("Textures");
    
    GLuint list1d = glGenLists(1);
    glNewList(list1d, GL_COMPILE);
    glBegin(GL_QUADS);
    glTexCoord1f(0.0f);
    glVertex2f(-1.0f, -1.0f);
    glTexCoord1f(1.0f);
    glVertex2f(1.0f, -1.0f);
    glTexCoord1f(1.0f);
    glVertex2f(1.0f, 1.0f);
    glTexCoord1f(0.0f);
    glVertex2f(-1.0f, 1.0f);
    glEnd();
    glEndList();
    
    GLuint list2d = glGenLists(1);
    glNewList(list2d, GL_COMPILE);
    glBegin(GL_QUADS);
    glTexCoord2f(0.0f, 0.0f);
    glVertex2f(-1.0f, -1.0f);
    glTexCoord2f(1.0f, 0.0f);
    glVertex2f(1.0f, -1.0f);
    glTexCoord2f(1.0f, 1.0f);
    glVertex2f(1.0f, 1.0f);
    glTexCoord2f(0.0f, 1.0f);
    glVertex2f(-1.0f, 1.0f);
    glEnd();
    glEndList();
    
    GLuint list3d = glGenLists(1);
    glNewList(list3d, GL_COMPILE);
    glBegin(GL_QUADS);
    glTexCoord3f(0.0f, 0.0f, 1.0f);
    glVertex2f(-1.0f, -1.0f);
    glTexCoord3f(1.0f, 0.0f, 1.0f);
    glVertex2f(1.0f, -1.0f);
    glTexCoord3f(1.0f, 1.0f, 0.0f);
    glVertex2f(1.0f, 1.0f);
    glTexCoord3f(0.0f, 1.0f, 0.0f);
    glVertex2f(-1.0f, 1.0f);
    glEnd();
    glEndList();
    
    GLuint tex1d;
    glGenTextures(1, &tex1d);
    glBindTexture(GL_TEXTURE_1D, tex1d);
    glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    
    GLuint tex2d;
    glGenTextures(1, &tex2d);
    glBindTexture(GL_TEXTURE_2D, tex2d);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    
    GLuint tex3d;
    glGenTextures(1, &tex3d);
    glBindTexture(GL_TEXTURE_3D, tex2d);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    
    GLenum formats[] = {GL_COLOR_INDEX, GL_RED, GL_GREEN, GL_BLUE, GL_ALPHA,
                        GL_RGB, GL_BGR, GL_RGBA, GL_BGRA, GL_LUMINANCE, GL_LUMINANCE_ALPHA,
                        GL_DEPTH_COMPONENT};
    
    GLenum internal_formats[] = {3, 1, 1, 1, 1, 3, 3, 4, 4, 1, 2, GL_DEPTH_COMPONENT};
    
    GLenum types[] = {GL_UNSIGNED_BYTE, GL_BYTE, GL_UNSIGNED_SHORT, GL_SHORT,
                      GL_UNSIGNED_INT, GL_INT, GL_FLOAT, GL_UNSIGNED_BYTE_3_3_2,
                      GL_UNSIGNED_BYTE_2_3_3_REV, GL_UNSIGNED_SHORT_5_6_5,
                      GL_UNSIGNED_SHORT_5_6_5_REV, GL_UNSIGNED_SHORT_4_4_4_4,
                      GL_UNSIGNED_SHORT_4_4_4_4_REV, GL_UNSIGNED_SHORT_5_5_5_1,
                      GL_UNSIGNED_SHORT_1_5_5_5_REV, GL_UNSIGNED_INT_8_8_8_8,
                      GL_UNSIGNED_INT_8_8_8_8_REV, GL_UNSIGNED_INT_10_10_10_2,
                      GL_UNSIGNED_INT_2_10_10_10_REV};
    
    for (size_t i = 0; i < sizeof(formats)/sizeof(GLenum); i++) {
        GLenum format = formats[i];
        GLenum internal_format = internal_formats[i];
        for (size_t j = 0; j < sizeof(types)/sizeof(GLenum); j++) {
            GLenum type = types[j];
            
            if ((type==GL_UNSIGNED_BYTE_3_3_2 ||
                 type==GL_UNSIGNED_BYTE_2_3_3_REV ||
                 type==GL_UNSIGNED_SHORT_5_6_5 ||
                 type==GL_UNSIGNED_SHORT_5_6_5_REV) &&
                format!=GL_RGB)
                continue;
            
            if ((type==GL_UNSIGNED_SHORT_4_4_4_4 ||
                 type==GL_UNSIGNED_SHORT_4_4_4_4_REV ||
                 type==GL_UNSIGNED_SHORT_5_5_5_1 ||
                 type==GL_UNSIGNED_SHORT_1_5_5_5_REV ||
                 type==GL_UNSIGNED_INT_8_8_8_8 ||
                 type==GL_UNSIGNED_INT_8_8_8_8_REV ||
                 type==GL_UNSIGNED_INT_10_10_10_2||
                 type==GL_UNSIGNED_INT_2_10_10_10_REV) &&
                format!=GL_RGBA &&
                format!=GL_BGRA)
                continue;
            
            if (format==GL_DEPTH_COMPONENT &&
                (internal_format!=GL_DEPTH_COMPONENT &&
                 internal_format!=GL_DEPTH_COMPONENT16 &&
                 internal_format!=GL_DEPTH_COMPONENT24 &&
                 internal_format!=GL_DEPTH_COMPONENT32))
                continue;
            
            if ((internal_format==GL_DEPTH_COMPONENT ||
                 internal_format==GL_DEPTH_COMPONENT16 ||
                 internal_format==GL_DEPTH_COMPONENT24 ||
                 internal_format==GL_DEPTH_COMPONENT32) &&
                format!=GL_DEPTH_COMPONENT)
                continue;
            
            test_tex(internal_format, format, type);
        }
    }
    
    glDeleteLists(list1d, 1);
    glDeleteLists(list2d, 1);
    glDeleteLists(list3d, 1);
    glDeleteTextures(1, &tex3d);
    glDeleteTextures(1, &tex2d);
    glDeleteTextures(1, &tex1d);
}
