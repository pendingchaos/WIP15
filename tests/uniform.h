void uniform_test() {
    glCurrentTestWIP15("Uniforms");
    
    GLfloat pos[] = {-1.0f, -1.0f, 1.0f, -1.0f, 1.0f, 1.0f,
                     -1.0f, -1.0f, -1.0f, 1.0f, 1.0f, 1.0f};
    buffer(GL_ARRAY_BUFFER, sizeof(pos), pos);
    
    GLuint program = create_program("#version 150\n"
        "in vec2 pos;\n"
        "void main() {\n"
        "    gl_Position = vec4(pos, 0.0, 1.0);\n"
        "}\n",
        "#version 150\n"
        "out vec4 color;\n"
        "uniform float f1;\n"
        "uniform vec2 f2;\n"
        "uniform vec3 f3;\n"
        "uniform vec4 f4;\n"
        "uniform int i1;\n"
        "uniform ivec2 i2;\n"
        "uniform ivec3 i3;\n"
        "uniform ivec4 i4;\n"
        "uniform uint ui1;\n"
        "uniform uvec2 ui2;\n"
        "uniform uvec3 ui3;\n"
        "uniform uvec4 ui4;\n"
        "uniform float f1v[2];\n"
        "uniform vec2 f2v[2];\n"
        "uniform vec3 f3v[2];\n"
        "uniform vec4 f4v[2];\n"
        "uniform int i1v[2];\n"
        "uniform ivec2 i2v[2];\n"
        "uniform ivec3 i3v[2];\n"
        "uniform ivec4 i4v[2];\n"
        "uniform uint ui1v[2];\n"
        "uniform uvec2 ui2v[2];\n"
        "uniform uvec3 ui3v[2];\n"
        "uniform uvec4 ui4v[2];\n"
        "uniform float f1v2[2];\n"
        "layout(std140) uniform Uniforms {\n"
        "    float ubo;"
        "};\n"
        "void main() {\n"
        "    color = vec4(1.0, 0.5, 0.5, 1.0);\n"
        "    if (f1 != 1.0) return;\n"
        "    if (any(notEqual(f2, vec2(2.0, 3.0)))) return;\n"
        "    if (any(notEqual(f3, vec3(4.0, 5.0, 6.0)))) return;\n"
        "    if (any(notEqual(f4, vec4(7.0, 8.0, 9.0, 10.0)))) return;\n"
        "    if (i1 != 11) return;\n"
        "    if (any(notEqual(i2, ivec2(12, 13)))) return;\n"
        "    if (any(notEqual(i3, ivec3(14, 15, 16)))) return;\n"
        "    if (any(notEqual(i4, ivec4(17, 18, 19, 20)))) return;\n"
        "    if (ui1 != 21u) return;\n"
        "    if (any(notEqual(ui2, uvec2(22u, 23u)))) return;\n"
        "    if (any(notEqual(ui3, uvec3(24u, 25u, 26u)))) return;\n"
        "    if (any(notEqual(ui4, uvec4(27u, 28u, 29u, 30u)))) return;\n"
        "    if (f1v[0] != 31.0) return;\n"
        "    if (any(notEqual(f2v[0], vec2(32.0, 33.0)))) return;\n"
        "    if (any(notEqual(f3v[0], vec3(34.0, 35.0, 36.0)))) return;\n"
        "    if (any(notEqual(f4v[0], vec4(37.0, 38.0, 39.0, 40.0)))) return;\n"
        "    if (i1v[0] != 41) return;\n"
        "    if (any(notEqual(i2v[0], ivec2(42, 43)))) return;\n"
        "    if (any(notEqual(i3v[0], ivec3(44, 45, 46)))) return;\n"
        "    if (any(notEqual(i4v[0], ivec4(47, 48, 49, 50)))) return;\n"
        "    if (ui1v[0] != 51u) return;\n"
        "    if (any(notEqual(ui2v[0], uvec2(52u, 53u)))) return;\n"
        "    if (any(notEqual(ui3v[0], uvec3(54u, 55u, 56u)))) return;\n"
        "    if (any(notEqual(ui4v[0], uvec4(57u, 58u, 59u, 60u)))) return;\n"
        "    if (f1v[1] != 61.0) return;\n"
        "    if (any(notEqual(f2v[1], vec2(62.0, 63.0)))) return;\n"
        "    if (any(notEqual(f3v[1], vec3(64.0, 65.0, 66.0)))) return;\n"
        "    if (any(notEqual(f4v[1], vec4(67.0, 68.0, 69.0, 70.0)))) return;\n"
        "    if (i1v[1] != 71) return;\n"
        "    if (any(notEqual(i2v[1], ivec2(72, 73)))) return;\n"
        "    if (any(notEqual(i3v[1], ivec3(74, 75, 76)))) return;\n"
        "    if (any(notEqual(i4v[1], ivec4(77, 78, 79, 80)))) return;\n"
        "    if (ui1v[1] != 81u) return;\n"
        "    if (any(notEqual(ui2v[1], uvec2(82u, 83u)))) return;\n"
        "    if (any(notEqual(ui3v[1], uvec3(84u, 85u, 86u)))) return;\n"
        "    if (any(notEqual(ui4v[1], uvec4(87u, 88u, 89u, 90u)))) return;\n"
        "    if (f1v2[0] != 91.0f) return;\n"
        "    if (f1v2[1] != 92.0f) return;\n"
        "    if (ubo != 93.0f) return;\n"
        "    color = vec4(0.5, 1.0, 0.5, 1.0);\n"
        "}\n");
    glUseProgram(program);
    
    GLuint vao;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);
    GLint loc = glGetAttribLocation(program, "pos");
    glEnableVertexAttribArray(loc);
    glVertexAttribPointer(loc, 2, GL_FLOAT, GL_FALSE, 0, (const GLvoid*)0);
    
    glUniform1f(glGetUniformLocation(program, "f1"), 1.0f);
    glUniform2f(glGetUniformLocation(program, "f2"), 2.0f, 3.0f);
    glUniform3f(glGetUniformLocation(program, "f3"), 4.0f, 5.0f, 6.0f);
    glUniform4f(glGetUniformLocation(program, "f4"), 7.0f, 8.0f, 9.0f, 10.0f);
    glUniform1i(glGetUniformLocation(program, "i1"), 11);
    glUniform2i(glGetUniformLocation(program, "i2"), 12, 13);
    glUniform3i(glGetUniformLocation(program, "i3"), 14, 15, 16);
    glUniform4i(glGetUniformLocation(program, "i4"), 17, 18, 19, 20);
    glUniform1ui(glGetUniformLocation(program, "ui1"), 21);
    glUniform2ui(glGetUniformLocation(program, "ui2"), 22, 23);
    glUniform3ui(glGetUniformLocation(program, "ui3"), 24, 25, 26);
    glUniform4ui(glGetUniformLocation(program, "ui4"), 27, 28, 29, 30);
    float f1v[] = {31.0f, 61.0f};
    glUniform1fv(glGetUniformLocation(program, "f1v"), 2, f1v);
    float f2v[] = {32.0f, 33.0f, 62.0f, 63.0f};
    glUniform2fv(glGetUniformLocation(program, "f2v"), 2, f2v);
    float f3v[] = {34.0f, 35.0f, 36.0f, 64.0f, 65.0f, 66.0f};
    glUniform3fv(glGetUniformLocation(program, "f3v"), 2, f3v);
    float f4v[] = {37.0f, 38.0f, 39.0f, 40.0f, 67.0f, 68.0f, 69.0f, 70.0f};
    glUniform4fv(glGetUniformLocation(program, "f4v"), 2, f4v);
    int32_t i1v[] = {41, 71};
    glUniform1iv(glGetUniformLocation(program, "i1v"), 2, i1v);
    int32_t i2v[] = {42, 43, 72, 73};
    glUniform2iv(glGetUniformLocation(program, "i2v"), 2, i2v);
    int32_t i3v[] = {44, 45, 46, 74, 75, 76};
    glUniform3iv(glGetUniformLocation(program, "i3v"), 2, i3v);
    int32_t i4v[] = {47, 48, 49, 50, 77, 78, 79, 80};
    glUniform4iv(glGetUniformLocation(program, "i4v"), 2, i4v);
    uint32_t ui1v[] = {51, 81};
    glUniform1uiv(glGetUniformLocation(program, "ui1v"), 2, ui1v);
    uint32_t ui2v[] = {52, 53, 82, 83};
    glUniform2uiv(glGetUniformLocation(program, "ui2v"), 2, ui2v);
    uint32_t ui3v[] = {54, 55, 56, 84, 85, 86};
    glUniform3uiv(glGetUniformLocation(program, "ui3v"), 2, ui3v);
    uint32_t ui4v[] = {57, 58, 59, 60, 87, 88, 89, 90};
    glUniform4uiv(glGetUniformLocation(program, "ui4v"), 2, ui4v);
    glUniform1f(glGetUniformLocation(program, "f1v2[0]"), 91.0f);
    glUniform1f(glGetUniformLocation(program, "f1v2[1]"), 92.0f);
    
    float ubo = 93.0f;
    glUniformBlockBinding(program, glGetUniformBlockIndex(program, "Uniforms"), 1);
    glBindBufferBase(GL_UNIFORM_BUFFER, 1, buffer(GL_UNIFORM_BUFFER, sizeof(ubo), &ubo));
    
    glClear(GL_COLOR_BUFFER_BIT);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    
    glDeleteVertexArrays(1, &vao);
    glDeleteProgram(program);
}
