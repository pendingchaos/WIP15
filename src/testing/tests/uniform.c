#include "tests.h"
#include <stdarg.h>

#define TEST_SUITE uniform

static GLuint program;

#define UNIFORM_SETUP(decl, expr) do {\
    program = create_program(NULL,\
        "#version 150\n"\
        "out vec4 color;\n"\
        decl\
        "void main() {\n"\
        "    color = "expr";\n"\
        "}\n");\
    glUseProgram(program);\
} while (0)

void uniform_setup() {
    program = 0;
}

void uniform_cleanup() {
    glDeleteProgram(program);
}

static void uniform_testd(const char* name, size_t count, ...) {
    GLint loc = glGetUniformLocation(program, name);
    va_list list;
    va_start(list, count);
    for (size_t i = 0; i < count; i++)
        wip15ExpectPropertyd(GL_PROGRAM, program, "uniforms", (uint64_t)loc<<32|count<<16|i, va_arg(list, double));
    va_end(list);
}

//============================== Float Uniforms ==============================

BEGIN_TEST(float_0)
    UNIFORM_SETUP("uniform float u;\n", "vec4(u)");
    glUniform1f(glGetUniformLocation(program, "u"), 1.0f);
    uniform_testd("u", 1, 1.0);
END_TEST(float_0)

BEGIN_TEST(float_1)
    UNIFORM_SETUP("uniform float u=1.0;\n", "vec4(u)");
    uniform_testd("u", 1, 1.0);
END_TEST(float_1)

BEGIN_TEST(vec2_0)
    UNIFORM_SETUP("uniform vec2 u;\n", "vec4(u, u)");
    glUniform2f(glGetUniformLocation(program, "u"), 1.0f, 2.0f);
    uniform_testd("u", 2, 1.0, 2.0);
END_TEST(vec2_0)

BEGIN_TEST(vec2_1)
    UNIFORM_SETUP("uniform vec2 u=vec2(1.0, 2.0);\n", "vec4(u, u)");
    uniform_testd("u", 2, 1.0, 2.0);
END_TEST(vec2_1)

BEGIN_TEST(vec3_0)
    UNIFORM_SETUP("uniform vec3 u;\n", "vec4(u, 1.0)");
    glUniform3f(glGetUniformLocation(program, "u"), 1.0f, 2.0f, 3.0f);
    uniform_testd("u", 3, 1.0, 2.0, 3.0);
END_TEST(vec3_0)

BEGIN_TEST(vec3_1)
    UNIFORM_SETUP("uniform vec3 u=vec3(1.0, 2.0, 3.0);\n", "vec4(u, 1.0)");
    uniform_testd("u", 3, 1.0, 2.0, 3.0);
END_TEST(vec3_1)

BEGIN_TEST(vec4_0)
    UNIFORM_SETUP("uniform vec4 u;\n", "u");
    glUniform4f(glGetUniformLocation(program, "u"), 1.0f, 2.0f, 3.0f, 4.0f);
    uniform_testd("u", 4, 1.0, 2.0, 3.0, 4.0);
END_TEST(vec4_0)

BEGIN_TEST(vec4_1)
    UNIFORM_SETUP("uniform vec4 u=vec4(1.0, 2.0, 3.0, 4.0);\n", "u");
    uniform_testd("u", 4, 1.0, 2.0, 3.0, 4.0);
END_TEST(vec4_1)

BEGIN_TEST(float_vec2_0)
    UNIFORM_SETUP("uniform float u0;\nuniform vec2 u1;\n", "vec4(u0, u0, u1)");
    glUniform1f(glGetUniformLocation(program, "u0"), 1.0f);
    glUniform2f(glGetUniformLocation(program, "u1"), 2.0f, 3.0f);
    uniform_testd("u0", 1, 1.0);
    uniform_testd("u1", 2, 2.0, 3.0);
END_TEST(float_vec2_0)

BEGIN_TEST(float_vec2_1)
    UNIFORM_SETUP("uniform float u0=1.0;\nuniform vec2 u1=vec2(2.0, 3.0);\n", "vec4(u0, u0, u1)");
    uniform_testd("u0", 1, 1.0);
    uniform_testd("u1", 2, 2.0, 3.0);
END_TEST(float_vec2_1)

//============================= Integer Uniforms =============================

BEGIN_TEST(int_0)
    UNIFORM_SETUP("uniform int u;\n", "vec4(u)");
    glUniform1i(glGetUniformLocation(program, "u"), 1);
    uniform_testd("u", 1, 1.0);
END_TEST(int_0)

BEGIN_TEST(int_1)
    UNIFORM_SETUP("uniform int u=1;\n", "vec4(u)");
    uniform_testd("u", 1, 1.0);
END_TEST(int_1)

BEGIN_TEST(ivec2_0)
    UNIFORM_SETUP("uniform ivec2 u;\n", "vec4(u, u)");
    glUniform2i(glGetUniformLocation(program, "u"), 1, 2);
    uniform_testd("u", 2, 1.0, 2.0);
END_TEST(ivec2_0)

BEGIN_TEST(ivec2_1)
    UNIFORM_SETUP("uniform ivec2 u=ivec2(1, 2);\n", "vec4(u, u)");
    uniform_testd("u", 2, 1.0, 2.0);
END_TEST(ivec2_1)

BEGIN_TEST(ivec3_0)
    UNIFORM_SETUP("uniform ivec3 u;\n", "vec4(u, 1.0)");
    glUniform3i(glGetUniformLocation(program, "u"), 1, 2, 3);
    uniform_testd("u", 3, 1.0, 2.0, 3.0);
END_TEST(ivec3_0)

BEGIN_TEST(ivec3_1)
    UNIFORM_SETUP("uniform ivec3 u=ivec3(1, 2, 3);\n", "vec4(u, 1.0)");
    uniform_testd("u", 3, 1.0, 2.0, 3.0);
END_TEST(ivec3_1)

BEGIN_TEST(ivec4_0)
    UNIFORM_SETUP("uniform ivec4 u;\n", "u");
    glUniform4i(glGetUniformLocation(program, "u"), 1, 2, 3, 4);
    uniform_testd("u", 4, 1.0, 2.0, 3.0, 4.0);
END_TEST(ivec4_0)

BEGIN_TEST(ivec4_1)
    UNIFORM_SETUP("uniform ivec4 u=ivec4(1, 2, 3, 4);\n", "u");
    uniform_testd("u", 4, 1.0, 2.0, 3.0, 4.0);
END_TEST(ivec4_1)

BEGIN_TEST(int_ivec2_0)
    UNIFORM_SETUP("uniform int u0;\nuniform ivec2 u1;\n", "vec4(u0, u0, u1)");
    glUniform1i(glGetUniformLocation(program, "u0"), 1);
    glUniform2i(glGetUniformLocation(program, "u1"), 2, 3);
    uniform_testd("u0", 1, 1.0);
    uniform_testd("u1", 2, 2.0, 3.0);
END_TEST(int_ivec2_0)

BEGIN_TEST(int_ivec2_1)
    UNIFORM_SETUP("uniform int u0=1;\nuniform ivec2 u1=ivec2(2, 3);\n", "vec4(u0, u0, u1)");
    uniform_testd("u0", 1, 1.0);
    uniform_testd("u1", 2, 2.0, 3.0);
END_TEST(int_ivec2_1)

//========================= Unsigned Integer Uniforms =========================

BEGIN_TEST(uint_0)
    UNIFORM_SETUP("uniform uint u;\n", "vec4(u)");
    glUniform1ui(glGetUniformLocation(program, "u"), 1);
    uniform_testd("u", 1, 1.0);
END_TEST(uint_0)

BEGIN_TEST(uint_1)
    UNIFORM_SETUP("uniform uint u=1u;\n", "vec4(u)");
    uniform_testd("u", 1, 1.0);
END_TEST(uint_1)

BEGIN_TEST(uvec2_0)
    UNIFORM_SETUP("uniform uvec2 u;\n", "vec4(u, u)");
    glUniform2ui(glGetUniformLocation(program, "u"), 1, 2);
    uniform_testd("u", 2, 1.0, 2.0);
END_TEST(uvec2_0)

BEGIN_TEST(uvec2_1)
    UNIFORM_SETUP("uniform uvec2 u=uvec2(1u, 2u);\n", "vec4(u, u)");
    uniform_testd("u", 2, 1.0, 2.0);
END_TEST(uvec2_1)

BEGIN_TEST(uvec3_0)
    UNIFORM_SETUP("uniform uvec3 u;\n", "vec4(u, 1.0)");
    glUniform3ui(glGetUniformLocation(program, "u"), 1, 2, 3);
    uniform_testd("u", 3, 1.0, 2.0, 3.0);
END_TEST(uvec3_0)

BEGIN_TEST(uvec3_1)
    UNIFORM_SETUP("uniform uvec3 u=uvec3(1u, 2u, 3u);\n", "vec4(u, 1.0)");
    uniform_testd("u", 3, 1.0, 2.0, 3.0);
END_TEST(uvec3_1)

BEGIN_TEST(uvec4_0)
    UNIFORM_SETUP("uniform uvec4 u;\n", "u");
    glUniform4ui(glGetUniformLocation(program, "u"), 1, 2, 3, 4);
    uniform_testd("u", 4, 1.0, 2.0, 3.0, 4.0);
END_TEST(uvec4_0)

BEGIN_TEST(uvec4_1)
    UNIFORM_SETUP("uniform uvec4 u=uvec4(1u, 2u, 3u, 4u);\n", "u");
    uniform_testd("u", 4, 1.0, 2.0, 3.0, 4.0);
END_TEST(uvec4_1)

BEGIN_TEST(uint_uvec2_0)
    UNIFORM_SETUP("uniform uint u0;\nuniform uvec2 u1;\n", "vec4(u0, u0, u1)");
    glUniform1ui(glGetUniformLocation(program, "u0"), 1);
    glUniform2ui(glGetUniformLocation(program, "u1"), 2, 3);
    uniform_testd("u0", 1, 1.0);
    uniform_testd("u1", 2, 2.0, 3.0);
END_TEST(uint_uvec2_0)

BEGIN_TEST(uint_uvec2_1)
    UNIFORM_SETUP("uniform uint u0=1u;\nuniform uvec2 u1=uvec2(2u, 3u);\n", "vec4(u0, u0, u1)");
    uniform_testd("u0", 1, 1.0);
    uniform_testd("u1", 2, 2.0, 3.0);
END_TEST(uint_uvec2_1)

//=========================== Float Array Uniforms ===========================

BEGIN_TEST(float_arr_0)
    UNIFORM_SETUP("uniform float u[2];\n", "vec4(u[0], u[1], 0.0, 0.0)");
    glUniform1fv(glGetUniformLocation(program, "u"), 2, (float[]){1.0f, 2.0f});
    uniform_testd("u[0]", 1, 1.0);
    uniform_testd("u[1]", 1, 2.0);
END_TEST(float_arr_0)

BEGIN_TEST(float_arr_1)
    UNIFORM_SETUP("uniform float u[2]=float[](1.0, 2.0);\n", "vec4(u[0], u[1], 0.0, 0.0)");
    uniform_testd("u[0]", 1, 1.0);
    uniform_testd("u[1]", 1, 2.0);
END_TEST(float_arr_1)

BEGIN_TEST(vec2_arr_0)
    UNIFORM_SETUP("uniform vec2 u[2];\n", "vec4(u[0], u[1])");
    glUniform2fv(glGetUniformLocation(program, "u"), 2, (float[]){1.0f, 2.0f, 3.0f, 4.0f});
    uniform_testd("u[0]", 2, 1.0, 2.0);
    uniform_testd("u[1]", 2, 3.0, 4.0);
END_TEST(vec2_arr_0)

BEGIN_TEST(vec2_arr_1)
    UNIFORM_SETUP("uniform vec2 u[2]=vec2[](vec2(1.0, 2.0), vec2(3.0, 4.0));\n", "vec4(u[0], u[1])");
    uniform_testd("u[0]", 2, 1.0, 2.0);
    uniform_testd("u[1]", 2, 3.0, 4.0);
END_TEST(vec2_arr_1)

BEGIN_TEST(vec3_arr_0)
    UNIFORM_SETUP("uniform vec3 u[2];\n", "vec4(u[0].xy, u[1].xy)");
    glUniform3fv(glGetUniformLocation(program, "u"), 2, (float[]){1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f});
    uniform_testd("u[0]", 3, 1.0, 2.0, 3.0);
    uniform_testd("u[1]", 3, 4.0, 5.0, 6.0);
END_TEST(vec3_arr_0)

BEGIN_TEST(vec3_arr_1)
    UNIFORM_SETUP("uniform vec3 u[2]=vec3[](vec3(1.0, 2.0, 3.0), vec3(4.0, 5.0, 6.0));\n", "vec4(u[0].xy, u[1].xy)");
    uniform_testd("u[0]", 3, 1.0, 2.0, 3.0);
    uniform_testd("u[1]", 3, 4.0, 5.0, 6.0);
END_TEST(vec3_arr_1)

BEGIN_TEST(vec4_arr_0)
    UNIFORM_SETUP("uniform vec4 u[2];\n", "vec4(u[0].xy, u[1].xy)");
    glUniform4fv(glGetUniformLocation(program, "u"), 2, (float[]){1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f, 7.0f, 8.0f});
    uniform_testd("u[0]", 4, 1.0, 2.0, 3.0, 4.0);
    uniform_testd("u[1]", 4, 5.0, 6.0, 7.0, 8.0);
END_TEST(vec4_arr_0)

BEGIN_TEST(vec4_arr_1)
    UNIFORM_SETUP("uniform vec4 u[2]=vec4[](vec4(1.0, 2.0, 3.0, 4.0), vec4(5.0, 6.0, 7.0, 8.0));\n", "vec4(u[0].xy, u[1].xy)");
    uniform_testd("u[0]", 4, 1.0, 2.0, 3.0, 4.0);
    uniform_testd("u[1]", 4, 5.0, 6.0, 7.0, 8.0);
END_TEST(vec4_arr_1)

//========================== Integer Array Uniforms ==========================

BEGIN_TEST(int_arr_0)
    UNIFORM_SETUP("uniform int u[2];\n", "vec4(u[0], u[1], 0, 0)");
    glUniform1iv(glGetUniformLocation(program, "u"), 2, (GLint[]){1, 2});
    uniform_testd("u[0]", 1, 1.0);
    uniform_testd("u[1]", 1, 2.0);
END_TEST(int_arr_0)

BEGIN_TEST(int_arr_1)
    UNIFORM_SETUP("uniform int u[2]=int[](1, 2);\n", "vec4(u[0], u[1], 0, 0)");
    uniform_testd("u[0]", 1, 1.0);
    uniform_testd("u[1]", 1, 2.0);
END_TEST(int_arr_1)

BEGIN_TEST(ivec2_arr_0)
    UNIFORM_SETUP("uniform ivec2 u[2];\n", "vec4(u[0], u[1])");
    glUniform2iv(glGetUniformLocation(program, "u"), 2, (GLint[]){1, 2, 3, 4});
    uniform_testd("u[0]", 2, 1.0, 2.0);
    uniform_testd("u[1]", 2, 3.0, 4.0);
END_TEST(ivec2_arr_0)

BEGIN_TEST(ivec2_arr_1)
    UNIFORM_SETUP("uniform ivec2 u[2]=ivec2[](ivec2(1, 2), ivec2(3, 4));\n", "vec4(u[0], u[1])");
    uniform_testd("u[0]", 2, 1.0, 2.0);
    uniform_testd("u[1]", 2, 3.0, 4.0);
END_TEST(ivec2_arr_1)

BEGIN_TEST(ivec3_arr_0)
    UNIFORM_SETUP("uniform ivec3 u[2];\n", "vec4(u[0].xy, u[1].xy)");
    glUniform3iv(glGetUniformLocation(program, "u"), 2, (GLint[]){1, 2, 3, 4, 5, 6});
    uniform_testd("u[0]", 3, 1.0, 2.0, 3.0);
    uniform_testd("u[1]", 3, 4.0, 5.0, 6.0);
END_TEST(ivec3_arr_0)

BEGIN_TEST(ivec3_arr_1)
    UNIFORM_SETUP("uniform ivec3 u[2]=ivec3[](ivec3(1, 2, 3), ivec3(4, 5, 6));\n", "vec4(u[0].xy, u[1].xy)");
    uniform_testd("u[0]", 3, 1.0, 2.0, 3.0);
    uniform_testd("u[1]", 3, 4.0, 5.0, 6.0);
END_TEST(ivec3_arr_1)

BEGIN_TEST(ivec4_arr_0)
    UNIFORM_SETUP("uniform ivec4 u[2];\n", "vec4(u[0].xy, u[1].xy)");
    glUniform4iv(glGetUniformLocation(program, "u"), 2, (GLint[]){1, 2, 3, 4, 5, 6, 7, 8});
    uniform_testd("u[0]", 4, 1.0, 2.0, 3.0, 4.0);
    uniform_testd("u[1]", 4, 5.0, 6.0, 7.0, 8.0);
END_TEST(ivec4_arr_0)

BEGIN_TEST(ivec4_arr_1)
    UNIFORM_SETUP("uniform ivec4 u[2]=ivec4[](ivec4(1, 2, 3, 4), ivec4(5, 6, 7, 8));\n", "vec4(u[0].xy, u[1].xy)");
    uniform_testd("u[0]", 4, 1.0, 2.0, 3.0, 4.0);
    uniform_testd("u[1]", 4, 5.0, 6.0, 7.0, 8.0);
END_TEST(ivec4_arr_1)

//====================== Unsigned Integer Array Uniforms ======================

BEGIN_TEST(uint_arr_0)
    UNIFORM_SETUP("uniform uint u[2];\n", "vec4(u[0], u[1], 0, 0)");
    glUniform1uiv(glGetUniformLocation(program, "u"), 2, (GLuint[]){1, 2});
    uniform_testd("u[0]", 1, 1.0);
    uniform_testd("u[1]", 1, 2.0);
END_TEST(uint_arr_0)

BEGIN_TEST(uint_arr_1)
    UNIFORM_SETUP("uniform uint u[2]=uint[](1, 2);\n", "vec4(u[0], u[1], 0, 0)");
    uniform_testd("u[0]", 1, 1.0);
    uniform_testd("u[1]", 1, 2.0);
END_TEST(uint_arr_1)

BEGIN_TEST(uvec2_arr_0)
    UNIFORM_SETUP("uniform uvec2 u[2];\n", "vec4(u[0], u[1])");
    glUniform2uiv(glGetUniformLocation(program, "u"), 2, (GLuint[]){1, 2, 3, 4});
    uniform_testd("u[0]", 2, 1.0, 2.0);
    uniform_testd("u[1]", 2, 3.0, 4.0);
END_TEST(uvec2_arr_0)

BEGIN_TEST(uvec2_arr_1)
    UNIFORM_SETUP("uniform uvec2 u[2]=uvec2[](uvec2(1, 2), uvec2(3, 4));\n", "vec4(u[0], u[1])");
    uniform_testd("u[0]", 2, 1.0, 2.0);
    uniform_testd("u[1]", 2, 3.0, 4.0);
END_TEST(uvec2_arr_1)

BEGIN_TEST(uvec3_arr_0)
    UNIFORM_SETUP("uniform uvec3 u[2];\n", "vec4(u[0].xy, u[1].xy)");
    glUniform3uiv(glGetUniformLocation(program, "u"), 2, (GLuint[]){1, 2, 3, 4, 5, 6});
    uniform_testd("u[0]", 3, 1.0, 2.0, 3.0);
    uniform_testd("u[1]", 3, 4.0, 5.0, 6.0);
END_TEST(uvec3_arr_0)

BEGIN_TEST(uvec3_arr_1)
    UNIFORM_SETUP("uniform uvec3 u[2]=uvec3[](uvec3(1, 2, 3), uvec3(4, 5, 6));\n", "vec4(u[0].xy, u[1].xy)");
    uniform_testd("u[0]", 3, 1.0, 2.0, 3.0);
    uniform_testd("u[1]", 3, 4.0, 5.0, 6.0);
END_TEST(uvec3_arr_1)

BEGIN_TEST(uvec4_arr_0)
    UNIFORM_SETUP("uniform uvec4 u[2];\n", "vec4(u[0].xy, u[1].xy)");
    glUniform4uiv(glGetUniformLocation(program, "u"), 2, (GLuint[]){1, 2, 3, 4, 5, 6, 7, 8});
    uniform_testd("u[0]", 4, 1.0, 2.0, 3.0, 4.0);
    uniform_testd("u[1]", 4, 5.0, 6.0, 7.0, 8.0);
END_TEST(uvec4_arr_0)

BEGIN_TEST(uvec4_arr_1)
    UNIFORM_SETUP("uniform uvec4 u[2]=uvec4[](uvec4(1, 2, 3, 4), uvec4(5, 6, 7, 8));\n", "vec4(u[0].xy, u[1].xy)");
    uniform_testd("u[0]", 4, 1.0, 2.0, 3.0, 4.0);
    uniform_testd("u[1]", 4, 5.0, 6.0, 7.0, 8.0);
END_TEST(uvec4_arr_1)
