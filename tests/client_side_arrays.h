void client_side_draw(GLenum pos_type, GLenum col_type, void* pos, void* col) {
    GLubyte ubyte_indices[] = {0, 1, 2};
    GLushort ushort_indices[] = {0, 1, 2};
    GLuint uint_indices[] = {0, 1, 2};
    
    glVertexPointer(2, pos_type, 0, pos);
    glColorPointer(3, col_type, 0, col);
    
    glClear(GL_COLOR_BUFFER_BIT);
    glDrawArrays(GL_TRIANGLES, 0, 3);
    
    glClear(GL_COLOR_BUFFER_BIT);
    GLint first = 0;
    GLsizei count = 3;
    glMultiDrawArrays(GL_TRIANGLES, &first, &count, 1);
    
    glClear(GL_COLOR_BUFFER_BIT);
    glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_BYTE, ubyte_indices);
    
    glClear(GL_COLOR_BUFFER_BIT);
    glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_SHORT, ushort_indices);
    
    glClear(GL_COLOR_BUFFER_BIT);
    glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_INT, uint_indices);
    
    glClear(GL_COLOR_BUFFER_BIT);
    glDrawRangeElements(GL_TRIANGLES, 0, 2, 3, GL_UNSIGNED_BYTE, ubyte_indices);
    
    glClear(GL_COLOR_BUFFER_BIT);
    glDrawRangeElements(GL_TRIANGLES, 0, 2, 3, GL_UNSIGNED_SHORT, ushort_indices);
    
    glClear(GL_COLOR_BUFFER_BIT);
    glDrawRangeElements(GL_TRIANGLES, 0, 2, 3, GL_UNSIGNED_INT, uint_indices);
}

void client_side_array_test() {
    GLshort short_positions[] = {-1, -1,
                                  1, -1,
                                  0,  1};
    
    GLint int_positions[] = {-1, -1,
                              1, -1,
                              0,  1};
    
    GLfloat float_positions[] = {-1.0f, -1.0f,
                                  1.0f, -1.0f,
                                  0.0f,  1.0f};
    
    GLdouble double_positions[] = {-1.0, -1.0,
                                    1.0, -1.0,
                                    0.0,  1.0};
    
    GLubyte ubyte_colors[] = {255, 0, 0,
                              0, 255, 0,
                              0, 0, 255};
    
    GLushort ushort_colors[] = {65534, 0, 0,
                                0, 65534, 0,
                                0, 0, 65534};
    
    GLuint uint_colors[] = {4294967295, 0, 0,
                            0, 4294967295, 0,
                            0, 0, 4294967295};
    
    GLfloat float_colors[] = {1.0f, 0.0f, 0.0f,
                              0.0f, 1.0f, 0.0f,
                              0.0f, 0.0f, 1.0f};
    
    GLdouble double_colors[] = {1.0, 0.0, 0.0,
                                0.0, 1.0, 0.0,
                                0.0, 0.0, 1.0};
    
    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_COLOR_ARRAY);
    
    client_side_draw(GL_SHORT, GL_FLOAT, short_positions, float_colors);
    client_side_draw(GL_INT, GL_FLOAT, int_positions, float_colors);
    client_side_draw(GL_FLOAT, GL_FLOAT, float_positions, float_colors);
    client_side_draw(GL_DOUBLE, GL_FLOAT, double_positions, float_colors);
    
    client_side_draw(GL_FLOAT, GL_UNSIGNED_BYTE, float_positions, ubyte_colors);
    client_side_draw(GL_FLOAT, GL_UNSIGNED_SHORT, float_positions, ushort_colors);
    client_side_draw(GL_FLOAT, GL_UNSIGNED_INT, float_positions, uint_colors);
    client_side_draw(GL_FLOAT, GL_FLOAT, float_positions, float_colors);
    client_side_draw(GL_FLOAT, GL_DOUBLE, float_positions, double_colors);
}
