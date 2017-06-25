void draw(GLuint program, GLenum pos_type, GLenum col_type, void* pos, void* col, size_t pos_size, size_t col_size) {
    GLubyte ubyte_indices[] = {0, 1, 2};
    GLushort ushort_indices[] = {0, 1, 2};
    GLuint uint_indices[] = {0, 1, 2};
    
    glCurrentTestWIP15(static_format("Draw with %u positions and %u colors: gl*Pointer and buffer creation", pos_type, col_type));
    buffer(GL_ARRAY_BUFFER, pos_size, pos);
    glVertexAttribPointer(glGetAttribLocation(program, "pos"), 2, pos_type, GL_TRUE, 0, (const GLvoid*)0);
    buffer(GL_ARRAY_BUFFER, col_size, col);
    glVertexAttribPointer(glGetAttribLocation(program, "col"), 3, col_type, GL_TRUE, 0, (const GLvoid*)0);
    
    glCurrentTestWIP15(static_format("Draw with %u positions and %u colors: glDrawArrays", pos_type, col_type));
    glClear(GL_COLOR_BUFFER_BIT);
    glDrawArrays(GL_TRIANGLES, 0, 3);
    test_fb("glDrawArrays");
    
    glCurrentTestWIP15(static_format("Draw with %u positions and %u colors: glMultiDrawarrays", pos_type, col_type));
    glClear(GL_COLOR_BUFFER_BIT);
    GLint first = 0;
    GLsizei count = 3;
    glMultiDrawArrays(GL_TRIANGLES, &first, &count, 1);
    test_fb("glMultiDrawArrays");
    
    glCurrentTestWIP15(static_format("Draw with %u positions and %u colors: glDrawElements with GL_UNSIGNED_BTYE", pos_type, col_type));
    buffer(GL_ELEMENT_ARRAY_BUFFER, sizeof(ubyte_indices), ubyte_indices);
    glClear(GL_COLOR_BUFFER_BIT);
    glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_BYTE, (const GLvoid*)0);
    test_fb("glDrawElements");
    glCurrentTestWIP15(static_format("Draw with %u positions and %u colors: glDrawRangeElements with GL_UNSIGNED_INT", pos_type, col_type));
    glClear(GL_COLOR_BUFFER_BIT);
    glDrawRangeElements(GL_TRIANGLES, 0, 2, 3, GL_UNSIGNED_BYTE, (const GLvoid*)0);
    test_fb("glDrawRangeElements");
    
    glCurrentTestWIP15(static_format("Draw with %u positions and %u colors: glDrawElements with GL_UNSIGNED_SHORT", pos_type, col_type));
    buffer(GL_ELEMENT_ARRAY_BUFFER, sizeof(ushort_indices), ushort_indices);
    glClear(GL_COLOR_BUFFER_BIT);
    glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_SHORT, (const GLvoid*)0);
    test_fb("glDrawElements");
    glCurrentTestWIP15(static_format("Draw with %u positions and %u colors: glDrawRangeElements with GL_UNSIGNED_INT", pos_type, col_type));
    glClear(GL_COLOR_BUFFER_BIT);
    glDrawRangeElements(GL_TRIANGLES, 0, 2, 3, GL_UNSIGNED_SHORT, (const GLvoid*)0);
    test_fb("glDrawRangeElements");
    
    glCurrentTestWIP15(static_format("Draw with %u positions and %u colors: glDrawElements with GL_UNSIGNED_INT", pos_type, col_type));
    buffer(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint_indices), uint_indices);
    glClear(GL_COLOR_BUFFER_BIT);
    glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_INT, (const GLvoid*)0);
    test_fb("glDrawElements");
    glCurrentTestWIP15(static_format("Draw with %u positions and %u colors: glDrawRangeElements with GL_UNSIGNED_INT", pos_type, col_type));
    glClear(GL_COLOR_BUFFER_BIT);
    glDrawRangeElements(GL_TRIANGLES, 0, 2, 3, GL_UNSIGNED_INT, (const GLvoid*)0);
    test_fb("glDrawRangeElements");
    
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void draw_test() {
    glCurrentTestWIP15("Drawing");
    
    GLshort short_positions[] = {-1, -1, 1, -1, 0,  1};
    GLint int_positions[] = {-1, -1, 1, -1, 0,  1};
    GLfloat float_positions[] = {-1.0f, -1.0f, 1.0f, -1.0f, 0.0f,  1.0f};
    GLdouble double_positions[] = {-1.0, -1.0, 1.0, -1.0, 0.0,  1.0};
    
    GLubyte ubyte_colors[] = {255, 0, 0, 0, 255, 0, 0, 0, 255};
    GLushort ushort_colors[] = {65534, 0, 0, 0, 65534, 0, 0, 0, 65534};
    GLuint uint_colors[] = {4294967295, 0, 0, 0, 4294967295, 0, 0, 0, 4294967295};
    GLfloat float_colors[] = {1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f};
    GLdouble double_colors[] = {1.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 1.0};
    
    GLuint program = create_program("#version 150\n"
                                    "in vec2 pos;\n"
                                    "in vec3 col;\n"
                                    "out vec4 frag_color;\n"
                                    "void main() {\n"
                                    "    gl_Position = vec4(pos, 0.0, 1.0);\n"
                                    "    frag_color = vec4(col, 1.0);\n"
                                    "}\n",
                                    "#version 150\n"
                                    "in vec4 frag_color;\n"
                                    "out vec4 color;\n"
                                    "void main() {\n"
                                    "    color = frag_color;\n"
                                    "}\n");
    
    glUseProgram(program);
    
    GLuint vao;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);
    glEnableVertexAttribArray(glGetAttribLocation(program, "pos"));
    glEnableVertexAttribArray(glGetAttribLocation(program, "col"));
    draw(program, GL_SHORT, GL_FLOAT, short_positions, float_colors, sizeof(short_positions), sizeof(float_colors));
    draw(program, GL_INT, GL_FLOAT, int_positions, float_colors, sizeof(int_positions), sizeof(float_colors));
    draw(program, GL_FLOAT, GL_FLOAT, float_positions, float_colors, sizeof(float_positions), sizeof(float_colors));
    draw(program, GL_DOUBLE, GL_FLOAT, double_positions, float_colors, sizeof(double_positions), sizeof(float_colors));
    draw(program, GL_FLOAT, GL_UNSIGNED_BYTE, float_positions, ubyte_colors, sizeof(float_positions), sizeof(ubyte_colors));
    draw(program, GL_FLOAT, GL_UNSIGNED_SHORT, float_positions, ushort_colors, sizeof(float_positions), sizeof(ushort_colors));
    draw(program, GL_FLOAT, GL_UNSIGNED_INT, float_positions, uint_colors, sizeof(float_positions), sizeof(uint_colors));
    draw(program, GL_FLOAT, GL_FLOAT, float_positions, float_colors, sizeof(float_positions), sizeof(float_colors));
    draw(program, GL_FLOAT, GL_DOUBLE, float_positions, double_colors, sizeof(float_positions), sizeof(double_colors));
    
    glDeleteVertexArrays(1, &vao);
    glDeleteProgram(program);
}
