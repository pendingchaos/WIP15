void client_side_draw(GLenum pos_type, GLenum col_type, void* pos, void* col) {
    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_COLOR_ARRAY);
    
    GLubyte ubyte_indices[] = {0, 1, 2};
    GLushort ushort_indices[] = {0, 1, 2};
    GLuint uint_indices[] = {0, 1, 2};
    
    glCurrentTestWIP15(static_format("Client side draw with %u positions and %u colors: gl*Pointer", pos_type, col_type));
    glVertexPointer(2, pos_type, 0, pos);
    glColorPointer(3, col_type, 0, col);
    
    glCurrentTestWIP15(static_format("Client side draw with %u positions and %u colors: glDrawArrays", pos_type, col_type));
    glClear(GL_COLOR_BUFFER_BIT);
    glDrawArrays(GL_TRIANGLES, 0, 3);
    
    glCurrentTestWIP15(static_format("Client side draw with %u positions and %u colors: glMultiDrawArrays", pos_type, col_type));
    glClear(GL_COLOR_BUFFER_BIT);
    GLint first = 0;
    GLsizei count = 3;
    glMultiDrawArrays(GL_TRIANGLES, &first, &count, 1);
    
    glCurrentTestWIP15(static_format("Client side draw with %u positions and %u colors: glDrawElements with GL_UNSIGNED_BYTE", pos_type, col_type));
    glClear(GL_COLOR_BUFFER_BIT);
    glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_BYTE, ubyte_indices);
    glCurrentTestWIP15(static_format("Client side draw with %u positions and %u colors: glDrawRangeElements with GL_UNSIGNED_BYTE", pos_type, col_type));
    glClear(GL_COLOR_BUFFER_BIT);
    glDrawRangeElements(GL_TRIANGLES, 0, 2, 3, GL_UNSIGNED_BYTE, ubyte_indices);
    
    glCurrentTestWIP15(static_format("Client side draw with %u positions and %u colors: glDrawElements with GL_UNSIGNED_SHORT", pos_type, col_type));
    glClear(GL_COLOR_BUFFER_BIT);
    glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_SHORT, ushort_indices);
    glCurrentTestWIP15(static_format("Client side draw with %u positions and %u colors: glDrawRangeElements with GL_UNSIGNED_SHORT", pos_type, col_type));
    glClear(GL_COLOR_BUFFER_BIT);
    glDrawRangeElements(GL_TRIANGLES, 0, 2, 3, GL_UNSIGNED_SHORT, ushort_indices);
    
    glCurrentTestWIP15(static_format("Client side draw with %u positions and %u colors: glDrawElements with GL_UNSIGNED_INT", pos_type, col_type));
    glClear(GL_COLOR_BUFFER_BIT);
    glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_INT, uint_indices);
    glCurrentTestWIP15(static_format("Client side draw with %u positions and %u colors: glDrawRangeElements with GL_UNSIGNED_INT", pos_type, col_type));
    glClear(GL_COLOR_BUFFER_BIT);
    glDrawRangeElements(GL_TRIANGLES, 0, 2, 3, GL_UNSIGNED_INT, uint_indices);
}

void buffer(GLenum target, size_t size, void* data) {
    GLuint buf;
    glGenBuffers(1, &buf);
    glBindBuffer(target, buf);
    glBufferData(target, size, data, GL_STATIC_DRAW);
}

void server_side_draw(GLenum pos_type, GLenum col_type, void* pos, void* col, size_t pos_size, size_t col_size) {
    GLubyte ubyte_indices[] = {0, 1, 2};
    GLushort ushort_indices[] = {0, 1, 2};
    GLuint uint_indices[] = {0, 1, 2};
    
    glCurrentTestWIP15(static_format("Server side draw with %u positions and %u colors: gl*Pointer and buffer creation", pos_type, col_type));
    buffer(GL_ARRAY_BUFFER, pos_size, pos);
    glVertexPointer(2, pos_type, 0, (const GLvoid*)0);
    buffer(GL_ARRAY_BUFFER, col_size, col);
    glColorPointer(3, col_type, 0, (const GLvoid*)0);
    
    glCurrentTestWIP15(static_format("Server side draw with %u positions and %u colors: glDrawArrays", pos_type, col_type));
    glClear(GL_COLOR_BUFFER_BIT);
    glDrawArrays(GL_TRIANGLES, 0, 3);
    
    glCurrentTestWIP15(static_format("Server side draw with %u positions and %u colors: glMultiDrawarrays", pos_type, col_type));
    glClear(GL_COLOR_BUFFER_BIT);
    GLint first = 0;
    GLsizei count = 3;
    glMultiDrawArrays(GL_TRIANGLES, &first, &count, 1);
    
    glCurrentTestWIP15(static_format("Server side draw with %u positions and %u colors: glDrawElements with GL_UNSIGNED_BTYE", pos_type, col_type));
    buffer(GL_ELEMENT_ARRAY_BUFFER, sizeof(ubyte_indices), ubyte_indices);
    glClear(GL_COLOR_BUFFER_BIT);
    glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_BYTE, (const GLvoid*)0);
    glCurrentTestWIP15(static_format("Server side draw with %u positions and %u colors: glDrawRangeElements with GL_UNSIGNED_INT", pos_type, col_type));
    glClear(GL_COLOR_BUFFER_BIT);
    glDrawRangeElements(GL_TRIANGLES, 0, 2, 3, GL_UNSIGNED_BYTE, (const GLvoid*)0);
    
    glCurrentTestWIP15(static_format("Server side draw with %u positions and %u colors: glDrawElements with GL_UNSIGNED_SHORT", pos_type, col_type));
    buffer(GL_ELEMENT_ARRAY_BUFFER, sizeof(ushort_indices), ushort_indices);
    glClear(GL_COLOR_BUFFER_BIT);
    glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_SHORT, (const GLvoid*)0);
    glCurrentTestWIP15(static_format("Server side draw with %u positions and %u colors: glDrawRangeElements with GL_UNSIGNED_INT", pos_type, col_type));
    glClear(GL_COLOR_BUFFER_BIT);
    glDrawRangeElements(GL_TRIANGLES, 0, 2, 3, GL_UNSIGNED_SHORT, (const GLvoid*)0);
    
    glCurrentTestWIP15(static_format("Server side draw with %u positions and %u colors: glDrawElements with GL_UNSIGNED_INT", pos_type, col_type));
    buffer(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint_indices), uint_indices);
    glClear(GL_COLOR_BUFFER_BIT);
    glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_INT, (const GLvoid*)0);
    glCurrentTestWIP15(static_format("Server side draw with %u positions and %u colors: glDrawRangeElements with GL_UNSIGNED_INT", pos_type, col_type));
    glClear(GL_COLOR_BUFFER_BIT);
    glDrawRangeElements(GL_TRIANGLES, 0, 2, 3, GL_UNSIGNED_INT, (const GLvoid*)0);
    
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void draw(GLenum pos_type, GLenum col_type, void* pos, void* col, size_t pos_size, size_t col_size) {
    client_side_draw(pos_type, col_type, pos, col);
    server_side_draw(pos_type, col_type, pos, col, pos_size, col_size);
}

void begin_end1() {
    GLbyte c1[] = {127, 0, 0};
    GLshort c2[] = {0, 65534, 0};
    GLint c3[] = {0, 0, 4294967295};
    GLshort v1[] = {-1, -1};
    GLint v2[] = {1, -1};
    GLfloat v3[] = {0.0f, 1.0f};
    glColor3bv(c1);
    glVertex2sv(v1);
    glColor3sv(c2);
    glVertex2iv(v2);
    glColor3iv(c3);
    glVertex2fv(v3);
}

void begin_end2() {
    GLfloat c1[] = {1.0f, 0.0, 0.0};
    GLdouble c2[] = {0, 1.0, 0};
    GLdouble c3[] = {0, 0, 1.0};
    GLdouble v1[] = {-1.0, -1.0};
    GLdouble v2[] = {1.0, -1.0};
    GLdouble v3[] = {0.0, 1.0};
    glColor3fv(c1);
    glVertex2dv(v1);
    glColor3dv(c2);
    glVertex2dv(v2);
    glColor3dv(c3);
    glVertex2dv(v3);
}

void begin_end3() {
    GLbyte c1[] = {127, 0, 0, 127};
    GLshort c2[] = {0, 65534, 0, 65534};
    GLint c3[] = {0, 0, 4294967295, 4294967295};
    GLshort v1[] = {-1, -1, 0};
    GLint v2[] = {1, -1, 0};
    GLfloat v3[] = {0.0f, 1.0f, 0.0f};
    glColor4bv(c1);
    glVertex3sv(v1);
    glColor4sv(c2);
    glVertex3iv(v2);
    glColor4iv(c3);
    glVertex3fv(v3);
}

void begin_end4() {
    GLfloat c1[] = {1.0f, 0.0, 0.0, 1.0f};
    GLdouble c2[] = {0, 1.0, 0, 1.0};
    GLdouble c3[] = {0, 0, 1.0, 1.0};
    GLdouble v1[] = {-1.0, -1.0, 0.0};
    GLdouble v2[] = {1.0, -1.0, 0.0};
    GLdouble v3[] = {0.0, 1.0, 0.0};
    glColor4fv(c1);
    glVertex3dv(v1);
    glColor4dv(c2);
    glVertex3dv(v2);
    glColor4dv(c3);
    glVertex3dv(v3);
}

void begin_end5() {
    GLbyte c1[] = {127, 0, 0, 127};
    GLshort c2[] = {0, 65534, 0, 65534};
    GLint c3[] = {0, 0, 4294967295, 4294967295};
    GLshort v1[] = {-1, -1, 0, 1};
    GLint v2[] = {1, -1, 0, 1};
    GLfloat v3[] = {0.0f, 1.0f, 0.0f, 1.0f};
    glColor4bv(c1);
    glVertex4sv(v1);
    glColor4sv(c2);
    glVertex4iv(v2);
    glColor4iv(c3);
    glVertex4fv(v3);
}

void begin_end6() {
    GLfloat c1[] = {1.0f, 0.0, 0.0, 1.0f};
    GLdouble c2[] = {0, 1.0, 0, 1.0};
    GLdouble c3[] = {0, 0, 1.0, 1.0};
    GLdouble v1[] = {-1.0, -1.0, 0.0, 1.0};
    GLdouble v2[] = {1.0, -1.0, 0.0, 1.0};
    GLdouble v3[] = {0.0, 1.0, 0.0, 1.0};
    glColor4fv(c1);
    glVertex4dv(v1);
    glColor4dv(c2);
    glVertex4dv(v2);
    glColor4dv(c3);
    glVertex4dv(v3);
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
    
    //glDraw* with client side arrays and buffers.
    draw(GL_SHORT, GL_FLOAT, short_positions, float_colors, sizeof(short_positions), sizeof(float_colors));
    draw(GL_INT, GL_FLOAT, int_positions, float_colors, sizeof(int_positions), sizeof(float_colors));
    draw(GL_FLOAT, GL_FLOAT, float_positions, float_colors, sizeof(float_positions), sizeof(float_colors));
    draw(GL_DOUBLE, GL_FLOAT, double_positions, float_colors, sizeof(double_positions), sizeof(float_colors));
    
    draw(GL_FLOAT, GL_UNSIGNED_BYTE, float_positions, ubyte_colors, sizeof(float_positions), sizeof(ubyte_colors));
    draw(GL_FLOAT, GL_UNSIGNED_SHORT, float_positions, ushort_colors, sizeof(float_positions), sizeof(ushort_colors));
    draw(GL_FLOAT, GL_UNSIGNED_INT, float_positions, uint_colors, sizeof(float_positions), sizeof(uint_colors));
    draw(GL_FLOAT, GL_FLOAT, float_positions, float_colors, sizeof(float_positions), sizeof(float_colors));
    draw(GL_FLOAT, GL_DOUBLE, float_positions, double_colors, sizeof(float_positions), sizeof(double_colors));
    
    //Begin/end
    glCurrentTestWIP15("Begin end 1");
    glClear(GL_COLOR_BUFFER_BIT);
    glBegin(GL_TRIANGLES);
    begin_end1();
    glEnd();
    
    glCurrentTestWIP15("Begin end 2");
    glClear(GL_COLOR_BUFFER_BIT);
    glBegin(GL_TRIANGLES);
    begin_end2();
    glEnd();
    
    glCurrentTestWIP15("Begin end 3");
    glClear(GL_COLOR_BUFFER_BIT);
    glBegin(GL_TRIANGLES);
    begin_end3();
    glEnd();
    
    glCurrentTestWIP15("Begin end 4");
    glClear(GL_COLOR_BUFFER_BIT);
    glBegin(GL_TRIANGLES);
    begin_end4();
    glEnd();
    
    glCurrentTestWIP15("Begin end 5");
    glClear(GL_COLOR_BUFFER_BIT);
    glBegin(GL_TRIANGLES);
    begin_end5();
    glEnd();
    
    glCurrentTestWIP15("Begin end 6");
    glClear(GL_COLOR_BUFFER_BIT);
    glBegin(GL_TRIANGLES);
    begin_end6();
    glEnd();
    
    //Display list
    glCurrentTestWIP15("Display list creation");
    GLuint list = glGenLists(1);
    glNewList(list, GL_COMPILE);
    glBegin(GL_TRIANGLES);
    begin_end6();
    glEnd();
    glEndList();
    
    glCurrentTestWIP15("glCallList");
    glClear(GL_COLOR_BUFFER_BIT);
    glCallList(list);
    
    glCurrentTestWIP15("glCallLists with GL_UNSIGNED_INT");
    glClear(GL_COLOR_BUFFER_BIT);
    glCallLists(1, GL_UNSIGNED_INT, &list);
    
    glCurrentTestWIP15("glCallLists with GL_BYTE");
    glClear(GL_COLOR_BUFFER_BIT);
    GLbyte byte = list;
    glCallLists(1, GL_BYTE, &byte);
    
    glCurrentTestWIP15("glCallLists with GL_UNSIGNED_BYTE");
    glClear(GL_COLOR_BUFFER_BIT);
    GLubyte ubyte = list;
    glCallLists(1, GL_UNSIGNED_BYTE, &ubyte);
    
    glCurrentTestWIP15("glCallLists with GL_SHORT");
    glClear(GL_COLOR_BUFFER_BIT);
    GLshort short_ = list;
    glCallLists(1, GL_SHORT, &short_);
    
    glCurrentTestWIP15("glCallLists with GL_UNSIGNED_SHORT");
    glClear(GL_COLOR_BUFFER_BIT);
    GLushort ushort_ = list;
    glCallLists(1, GL_UNSIGNED_SHORT, &ushort_);
    
    glCurrentTestWIP15("glCallLists with GL_INT");
    glClear(GL_COLOR_BUFFER_BIT);
    GLint int_ = list;
    glCallLists(1, GL_INT, &int_);
    
    glCurrentTestWIP15("glCallLists with GL_FLOAT");
    glClear(GL_COLOR_BUFFER_BIT);
    GLfloat float_ = list;
    glCallLists(1, GL_FLOAT, &float_);
    
    glDeleteLists(list, 1);
}
