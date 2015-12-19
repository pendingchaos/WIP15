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
    
    switch (type) {
    case GL_UNSIGNED_SHORT:
    case GL_SHORT: {
        short data[2048];
        for (size_t i = 0; i < 2048; i++) {
            data[i] = rand()/(double)RAND_MAX*32768;
        }
        glTexImage1D(GL_TEXTURE_1D, 0, internal, 256, 0, format, type, data);
        glTexImage2D(GL_TEXTURE_2D, 0, internal, 16, 16, 0, format, type, data);
        glTexImage3D(GL_TEXTURE_3D, 0, internal, 4, 4, 4, 0, format, type, data);
        break;
    }
    case GL_UNSIGNED_INT:
    case GL_INT: {
        int data[1024];
        for (size_t i = 0; i < 1024; i++) {
            data[i] = rand()/(double)RAND_MAX*2147483648;
        }
        glTexImage1D(GL_TEXTURE_1D, 0, internal, 256, 0, format, type, data);
        glTexImage2D(GL_TEXTURE_2D, 0, internal, 16, 16, 0, format, type, data);
        glTexImage3D(GL_TEXTURE_3D, 0, internal, 4, 4, 4, 0, format, type, data);
        break;
    }
    case GL_FLOAT: {
        float data[1024];
        for (size_t i = 0; i < 1024; i++) {
            data[i] = rand()/(double)RAND_MAX;
        }
        glTexImage1D(GL_TEXTURE_1D, 0, internal, 256, 0, format, type, data);
        glTexImage2D(GL_TEXTURE_2D, 0, internal, 16, 16, 0, format, type, data);
        glTexImage3D(GL_TEXTURE_3D, 0, internal, 4, 4, 4, 0, format, type, data);
        break;
    }
    default: {
        uint8_t data[4096];
        for (size_t i = 0; i < 4096; i++) {
            data[i] = rand()%255;
        }
        glTexImage1D(GL_TEXTURE_1D, 0, internal, 256, 0, format, type, data);
        glTexImage2D(GL_TEXTURE_2D, 0, internal, 16, 16, 0, format, type, data);
        glTexImage3D(GL_TEXTURE_3D, 0, internal, 4, 4, 4, 0, format, type, data);
        break;
    }
    }
    
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
    
    GLenum internal_formats[] = {GL_ALPHA, GL_ALPHA4, GL_ALPHA8, GL_ALPHA12,
                                 GL_ALPHA16, GL_COMPRESSED_ALPHA, GL_COMPRESSED_LUMINANCE,
                                 GL_COMPRESSED_LUMINANCE_ALPHA, GL_COMPRESSED_INTENSITY,
                                 GL_COMPRESSED_RGB, GL_COMPRESSED_RGBA, GL_DEPTH_COMPONENT,
                                 GL_DEPTH_COMPONENT16, GL_DEPTH_COMPONENT24,
                                 GL_DEPTH_COMPONENT32, GL_LUMINANCE, GL_LUMINANCE4,
                                 GL_LUMINANCE8, GL_LUMINANCE12, GL_LUMINANCE16,
                                 GL_LUMINANCE_ALPHA, GL_LUMINANCE4_ALPHA4,
                                 GL_LUMINANCE6_ALPHA2, GL_LUMINANCE8_ALPHA8,
                                 GL_LUMINANCE12_ALPHA4, GL_LUMINANCE12_ALPHA12,
                                 GL_LUMINANCE16_ALPHA16, GL_INTENSITY, GL_INTENSITY4,
                                 GL_INTENSITY8, GL_INTENSITY12, GL_INTENSITY16, GL_R3_G3_B2,
                                 GL_RGB, GL_RGB4, GL_RGB5, GL_RGB8, GL_RGB10, GL_RGB12,
                                 GL_RGB16, GL_RGBA, GL_RGBA2, GL_RGBA4, GL_RGB5_A1, GL_RGBA8,
                                 GL_RGB10_A2, GL_RGBA12, GL_RGBA16, GL_SLUMINANCE, GL_SLUMINANCE8,
                                 GL_SLUMINANCE_ALPHA, GL_SLUMINANCE8_ALPHA8, GL_SRGB, GL_SRGB8,
                                 GL_SRGB_ALPHA, GL_SRGB8_ALPHA8, 1, 2, 3, 4};
    
    GLenum formats[] = {GL_COLOR_INDEX, GL_RED, GL_GREEN, GL_BLUE, GL_ALPHA,
                        GL_RGB, GL_BGR, GL_RGBA, GL_BGRA, GL_LUMINANCE, GL_LUMINANCE_ALPHA};
    
    GLenum types[] = {GL_UNSIGNED_BYTE, GL_BYTE, GL_UNSIGNED_SHORT, GL_SHORT,
                      GL_UNSIGNED_INT, GL_INT, GL_FLOAT, GL_UNSIGNED_BYTE_3_3_2,
                      GL_UNSIGNED_BYTE_2_3_3_REV, GL_UNSIGNED_SHORT_5_6_5,
                      GL_UNSIGNED_SHORT_5_6_5_REV, GL_UNSIGNED_SHORT_4_4_4_4,
                      GL_UNSIGNED_SHORT_4_4_4_4_REV, GL_UNSIGNED_SHORT_5_5_5_1,
                      GL_UNSIGNED_SHORT_1_5_5_5_REV, GL_UNSIGNED_INT_8_8_8_8,
                      GL_UNSIGNED_INT_8_8_8_8_REV, GL_UNSIGNED_INT_10_10_10_2,
                      GL_UNSIGNED_INT_2_10_10_10_REV};
    
    for (size_t i = 0; i < sizeof(internal_formats)/sizeof(GLenum); i++) {
        GLenum internal_format = internal_formats[i];
        for (size_t j = 0; j < sizeof(formats)/sizeof(GLenum); j++) {
            GLenum format = formats[j];
            for (size_t k = 0; k < sizeof(types)/sizeof(GLenum); k++) {
                GLenum type = types[k];
                
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
    }
    
    glDeleteLists(list1d, 1);
    glDeleteLists(list2d, 1);
    glDeleteLists(list3d, 1);
    glDeleteTextures(1, &tex3d);
    glDeleteTextures(1, &tex2d);
    glDeleteTextures(1, &tex1d);
}
