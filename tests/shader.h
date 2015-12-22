void shader_test() {
    glCurrentTestWIP15("Shader test");
    
    GLuint vertex = glCreateShader(GL_VERTEX_SHADER);
    static const char* vert_source = "#version 120\n"
                                     "void main() {\n"
                                     "    gl_Position = ftransform();"
                                     "}\n";
    glShaderSource(vertex, 1, &vert_source, NULL);
    glCompileShader(vertex);
    
    GLuint fragment = glCreateShader(GL_FRAGMENT_SHADER);
    static const char* frag_source = "#version 120\n"
                                     "void main() {"
                                     "    gl_FragColor = vec4(1.0, 0.0, 0.0, 1.0);\n"
                                     "}\n";
    glShaderSource(fragment, 1, &frag_source, NULL);
    glCompileShader(fragment);
    
    GLuint program = glCreateProgram();
    glAttachShader(program, vertex);
    glAttachShader(program, fragment);
    glLinkProgram(program);
    glValidateProgram(program);
    
    glUseProgram(program);
    glBegin(GL_TRIANGLES);
    glVertex2f(-1.0f, -1.0f);
    glVertex2f(1.0f, -1.0f);
    glVertex2f(0.0f, 1.0f);
    glEnd();
    
    glDeleteProgram(program);
    glDeleteShader(fragment);
    glDeleteShader(vertex);
}
