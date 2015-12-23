void draw_texture1d(GLuint prog) {
    glEnableVertexAttribArray(glGetAttribLocation(prog, "pos"));
    glVertexAttribPointer(glGetAttribLocation(prog, "pos"), 2, GL_FLOAT, GL_FALSE, 0, (const GLvoid*)0);
    glUniform1i(glGetUniformLocation(prog, "tex"), 0);
    glUseProgram(prog);
    glClear(GL_COLOR_BUFFER_BIT);
    glDrawArrays(GL_TRIANGLES, 0, 6);
}

void draw_texture2d(GLuint prog) {
    glEnableVertexAttribArray(glGetAttribLocation(prog, "pos"));
    glVertexAttribPointer(glGetAttribLocation(prog, "pos"), 2, GL_FLOAT, GL_FALSE, 0, (const GLvoid*)0);
    glUniform1i(glGetUniformLocation(prog, "tex"), 0);
    glUseProgram(prog);
    glClear(GL_COLOR_BUFFER_BIT);
    glDrawArrays(GL_TRIANGLES, 0, 6);
}

void draw_texture3d(GLuint prog) {
    glEnableVertexAttribArray(glGetAttribLocation(prog, "pos"));
    glVertexAttribPointer(glGetAttribLocation(prog, "pos"), 2, GL_FLOAT, GL_FALSE, 0, (const GLvoid*)0);
    glUniform1i(glGetUniformLocation(prog, "tex"), 0);
    glUseProgram(prog);
    glClear(GL_COLOR_BUFFER_BIT);
    glDrawArrays(GL_TRIANGLES, 0, 6);
}

//TODO: Sometimes these textures result in a black screen.
void test_tex(GLenum internal, GLenum format, GLenum type, GLuint prog1d, GLuint prog2d, GLuint prog3d) {
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
    draw_texture1d(prog1d);
    draw_texture2d(prog2d);
    draw_texture3d(prog3d);
    
    GEN_DATA
    glTexSubImage1D(GL_TEXTURE_1D, 0, 0, 256, format, type, data);
    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, 16, 16, format, type, data);
    glTexSubImage3D(GL_TEXTURE_3D, 0, 0, 0, 0, 4, 4, 4, format, type, data);
    draw_texture1d(prog1d);
    draw_texture2d(prog2d);
    draw_texture3d(prog3d);
}

void texture_test() {
    glCurrentTestWIP15("Textures");
    
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
    glBindTexture(GL_TEXTURE_3D, tex3d);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    
    GLenum formats[] = {GL_RED, GL_RG, GL_RGB, GL_BGR, GL_RGBA, GL_BGRA,
                        GL_RED_INTEGER, GL_RG_INTEGER, GL_RGB_INTEGER,
                        GL_BGR_INTEGER, GL_RGBA_INTEGER, GL_BGRA_INTEGER,
                        GL_DEPTH_COMPONENT};
    
    GLenum internal_formats[] = {GL_RED, GL_RG, GL_RGB, GL_RGB, GL_RGBA,
                                 GL_RGBA, GL_RED, GL_RG, GL_RGB, GL_RGB,
                                 GL_RGBA, GL_RGBA, GL_DEPTH_COMPONENT};
    
    GLenum types[] = {GL_UNSIGNED_BYTE, GL_BYTE, GL_UNSIGNED_SHORT, GL_SHORT,
                      GL_UNSIGNED_INT, GL_INT, GL_FLOAT/*, GL_UNSIGNED_BYTE_3_3_2, //TODO: One of these is causing an invalid read of size 4
                      GL_UNSIGNED_BYTE_2_3_3_REV, GL_UNSIGNED_SHORT_5_6_5,
                      GL_UNSIGNED_SHORT_5_6_5_REV, GL_UNSIGNED_SHORT_4_4_4_4,
                      GL_UNSIGNED_SHORT_4_4_4_4_REV, GL_UNSIGNED_SHORT_5_5_5_1,
                      GL_UNSIGNED_SHORT_1_5_5_5_REV, GL_UNSIGNED_INT_8_8_8_8,
                      GL_UNSIGNED_INT_8_8_8_8_REV, GL_UNSIGNED_INT_10_10_10_2,
                      GL_UNSIGNED_INT_2_10_10_10_REV*/};
    
    GLuint buf;
    glGenBuffers(1, &buf);
    static const float data[] = {-1.0f, -1.0f,
                                  1.0f, -1.0f,
                                  1.0f,  1.0f,
                                  
                                  1.0f, -1.0f,
                                  1.0f,  1.0f,
                                 -1.0f, -1.0f};
    glBindBuffer(GL_ARRAY_BUFFER, buf);
    glBufferData(GL_ARRAY_BUFFER, sizeof(data), data, GL_STATIC_DRAW);
    
    GLuint prog1d = create_program("#version 130\n"
                                   "in vec2 pos;\n"
                                   "out vec2 tex_coord;\n"
                                   "void main() {\n"
                                   "    gl_Position = vec4(pos, 0.0, 1.0);\n"
                                   "    tex_coord = pos*0.5 + 0.5;\n"
                                   "}\n",
                                   "uniform sampler1D tex;\n"
                                   "in vec2 tex_coord;\n"
                                   "out vec4 color;\n"
                                   "void main() {\n"
                                   "    color = texture(tex, tex_coord.x);\n"
                                   "}\n");
    
    GLuint prog2d = create_program("#version 130\n"
                                   "in vec2 pos;\n"
                                   "out vec2 tex_coord;\n"
                                   "void main() {\n"
                                   "    gl_Position = vec4(pos, 0.0, 1.0);\n"
                                   "    tex_coord = pos*0.5 + 0.5;\n"
                                   "}\n",
                                   "uniform sampler2D tex;\n"
                                   "in vec2 tex_coord;\n"
                                   "out vec4 color;\n"
                                   "void main() {\n"
                                   "    color = texture(tex, tex_coord);\n"
                                   "}\n");
    
    GLuint prog3d = create_program("#version 130\n"
                                   "in vec2 pos;\n"
                                   "out vec2 tex_coord;\n"
                                   "void main() {\n"
                                   "    gl_Position = vec4(pos, 0.0, 1.0);\n"
                                   "    tex_coord = pos*0.5 + 0.5;\n"
                                   "}\n",
                                   "uniform sampler3D tex;\n"
                                   "in vec2 tex_coord;\n"
                                   "out vec4 color;\n"
                                   "void main() {\n"
                                   "    color = texture(tex, tex_coord.xyy);\n"
                                   "}\n");
    
    for (size_t i = 0; i < sizeof(formats)/sizeof(GLenum); i++) {
        GLenum format = formats[i];
        GLenum internal_format = internal_formats[i];
        for (size_t j = 0; j < sizeof(types)/sizeof(GLenum); j++) {
            GLenum type = types[j];
            
            if ((type==GL_UNSIGNED_BYTE_3_3_2 ||
                 type==GL_UNSIGNED_BYTE_2_3_3_REV ||
                 type==GL_UNSIGNED_SHORT_5_6_5 ||
                 type==GL_UNSIGNED_SHORT_5_6_5_REV) &&
                format!=GL_RGB &&
                format!=GL_BGR)
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
            
            test_tex(internal_format, format, type, prog1d, prog2d, prog3d);
        }
    }
    
    glDeleteProgram(prog1d);
    glDeleteBuffers(1, &buf);
    glDeleteTextures(1, &tex3d);
    glDeleteTextures(1, &tex2d);
    glDeleteTextures(1, &tex1d);
}
