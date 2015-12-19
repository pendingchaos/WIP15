void buffer_test() {
    glCurrentTestWIP15("Buffers");
    GLuint buf;
    glGenBuffers(1, &buf);
    glBindBuffer(GL_ARRAY_BUFFER, buf);
    
    glBufferData(GL_ARRAY_BUFFER, 5, NULL, GL_STATIC_DRAW);
    glBufferData(GL_ARRAY_BUFFER, 5, "Hello", GL_STATIC_DRAW);
    
    void* data = glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY);
    memcpy(data, "World", 5);
    glUnmapBuffer(GL_ARRAY_BUFFER);
    
    glBufferSubData(GL_ARRAY_BUFFER, 0, 5, "Hello");
}
