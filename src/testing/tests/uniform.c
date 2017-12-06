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

BEGIN_TEST(float_1)
    UNIFORM_SETUP("uniform float u=1.;\n", "vec4(u)");
    uniform_testd("u", 1, 1.);
END_TEST(float_1)

BEGIN_TEST(vec2_1)
    UNIFORM_SETUP("uniform vec2 u=vec2(1., 2.);\n", "vec4(u, u)");
    uniform_testd("u", 2, 1., 2.);
END_TEST(vec2_1)

BEGIN_TEST(vec3_1)
    UNIFORM_SETUP("uniform vec3 u=vec3(1., 2., 3.);\n", "vec4(u, 1.)");
    uniform_testd("u", 3, 1., 2., 3.);
END_TEST(vec3_1)

BEGIN_TEST(vec4_1)
    UNIFORM_SETUP("uniform vec4 u=vec4(1., 2., 3., 4.);\n", "u");
    uniform_testd("u", 4, 1., 2., 3., 4.);
END_TEST(vec4_1)

BEGIN_TEST(float_vec2_1)
    UNIFORM_SETUP("uniform float u0=1.;\nuniform vec2 u1=vec2(2., 3.);\n", "vec4(u0, u0, u1)");
    uniform_testd("u0", 1, 1.);
    uniform_testd("u1", 2, 2., 3.);
END_TEST(float_vec2_1)

//============================= Integer Uniforms =============================

BEGIN_TEST(int_1)
    UNIFORM_SETUP("uniform int u=1;\n", "vec4(u)");
    uniform_testd("u", 1, 1.);
END_TEST(int_1)

BEGIN_TEST(ivec2_1)
    UNIFORM_SETUP("uniform ivec2 u=ivec2(1, 2);\n", "vec4(u, u)");
    uniform_testd("u", 2, 1., 2.);
END_TEST(ivec2_1)

BEGIN_TEST(ivec3_1)
    UNIFORM_SETUP("uniform ivec3 u=ivec3(1, 2, 3);\n", "vec4(u, 1.)");
    uniform_testd("u", 3, 1., 2., 3.);
END_TEST(ivec3_1)

BEGIN_TEST(ivec4_1)
    UNIFORM_SETUP("uniform ivec4 u=ivec4(1, 2, 3, 4);\n", "u");
    uniform_testd("u", 4, 1., 2., 3., 4.);
END_TEST(ivec4_1)

BEGIN_TEST(int_ivec2_1)
    UNIFORM_SETUP("uniform int u0=1;\nuniform ivec2 u1=ivec2(2, 3);\n", "vec4(u0, u0, u1)");
    uniform_testd("u0", 1, 1.);
    uniform_testd("u1", 2, 2., 3.);
END_TEST(int_ivec2_1)

//========================= Unsigned Integer Uniforms =========================

BEGIN_TEST(uint_1)
    UNIFORM_SETUP("uniform uint u=1u;\n", "vec4(u)");
    uniform_testd("u", 1, 1.);
END_TEST(uint_1)

BEGIN_TEST(uvec2_1)
    UNIFORM_SETUP("uniform uvec2 u=uvec2(1u, 2u);\n", "vec4(u, u)");
    uniform_testd("u", 2, 1., 2.);
END_TEST(uvec2_1)

BEGIN_TEST(uvec3_1)
    UNIFORM_SETUP("uniform uvec3 u=uvec3(1u, 2u, 3u);\n", "vec4(u, 1.)");
    uniform_testd("u", 3, 1., 2., 3.);
END_TEST(uvec3_1)

BEGIN_TEST(uvec4_1)
    UNIFORM_SETUP("uniform uvec4 u=uvec4(1u, 2u, 3u, 4u);\n", "u");
    uniform_testd("u", 4, 1., 2., 3., 4.);
END_TEST(uvec4_1)

BEGIN_TEST(uint_uvec2_1)
    UNIFORM_SETUP("uniform uint u0=1u;\nuniform uvec2 u1=uvec2(2u, 3u);\n", "vec4(u0, u0, u1)");
    uniform_testd("u0", 1, 1.);
    uniform_testd("u1", 2, 2., 3.);
END_TEST(uint_uvec2_1)

//=========================== Float Array Uniforms ===========================

BEGIN_TEST(float_arr_1)
    UNIFORM_SETUP("uniform float u[2]=float[](1., 2.);\n", "vec4(u[0], u[1], 0., 0.)");
    uniform_testd("u[0]", 1, 1.);
    uniform_testd("u[1]", 1, 2.);
END_TEST(float_arr_1)

BEGIN_TEST(vec2_arr_1)
    UNIFORM_SETUP("uniform vec2 u[2]=vec2[](vec2(1., 2.), vec2(3., 4.));\n", "vec4(u[0], u[1])");
    uniform_testd("u[0]", 2, 1., 2.);
    uniform_testd("u[1]", 2, 3., 4.);
END_TEST(vec2_arr_1)

BEGIN_TEST(vec3_arr_1)
    UNIFORM_SETUP("uniform vec3 u[2]=vec3[](vec3(1., 2., 3.), vec3(4., 5., 6.));\n", "vec4(u[0].xy, u[1].xy)");
    uniform_testd("u[0]", 3, 1., 2., 3.);
    uniform_testd("u[1]", 3, 4., 5., 6.);
END_TEST(vec3_arr_1)

BEGIN_TEST(vec4_arr_1)
    UNIFORM_SETUP("uniform vec4 u[2]=vec4[](vec4(1., 2., 3., 4.), vec4(5., 6., 7., 8.));\n", "vec4(u[0].xy, u[1].xy)");
    uniform_testd("u[0]", 4, 1., 2., 3., 4.);
    uniform_testd("u[1]", 4, 5., 6., 7., 8.);
END_TEST(vec4_arr_1)

//========================== Integer Array Uniforms ==========================

BEGIN_TEST(int_arr_1)
    UNIFORM_SETUP("uniform int u[2]=int[](1, 2);\n", "vec4(u[0], u[1], 0, 0)");
    uniform_testd("u[0]", 1, 1.);
    uniform_testd("u[1]", 1, 2.);
END_TEST(int_arr_1)

BEGIN_TEST(ivec2_arr_1)
    UNIFORM_SETUP("uniform ivec2 u[2]=ivec2[](ivec2(1, 2), ivec2(3, 4));\n", "vec4(u[0], u[1])");
    uniform_testd("u[0]", 2, 1., 2.);
    uniform_testd("u[1]", 2, 3., 4.);
END_TEST(ivec2_arr_1)

BEGIN_TEST(ivec3_arr_1)
    UNIFORM_SETUP("uniform ivec3 u[2]=ivec3[](ivec3(1, 2, 3), ivec3(4, 5, 6));\n", "vec4(u[0].xy, u[1].xy)");
    uniform_testd("u[0]", 3, 1., 2., 3.);
    uniform_testd("u[1]", 3, 4., 5., 6.);
END_TEST(ivec3_arr_1)

BEGIN_TEST(ivec4_arr_1)
    UNIFORM_SETUP("uniform ivec4 u[2]=ivec4[](ivec4(1, 2, 3, 4), ivec4(5, 6, 7, 8));\n", "vec4(u[0].xy, u[1].xy)");
    uniform_testd("u[0]", 4, 1., 2., 3., 4.);
    uniform_testd("u[1]", 4, 5., 6., 7., 8.);
END_TEST(ivec4_arr_1)

//====================== Unsigned Integer Array Uniforms ======================

BEGIN_TEST(uint_arr_1)
    UNIFORM_SETUP("uniform uint u[2]=uint[](1, 2);\n", "vec4(u[0], u[1], 0, 0)");
    uniform_testd("u[0]", 1, 1.);
    uniform_testd("u[1]", 1, 2.);
END_TEST(uint_arr_1)

BEGIN_TEST(uvec2_arr_1)
    UNIFORM_SETUP("uniform uvec2 u[2]=uvec2[](uvec2(1, 2), uvec2(3, 4));\n", "vec4(u[0], u[1])");
    uniform_testd("u[0]", 2, 1., 2.);
    uniform_testd("u[1]", 2, 3., 4.);
END_TEST(uvec2_arr_1)

BEGIN_TEST(uvec3_arr_1)
    UNIFORM_SETUP("uniform uvec3 u[2]=uvec3[](uvec3(1, 2, 3), uvec3(4, 5, 6));\n", "vec4(u[0].xy, u[1].xy)");
    uniform_testd("u[0]", 3, 1., 2., 3.);
    uniform_testd("u[1]", 3, 4., 5., 6.);
END_TEST(uvec3_arr_1)

BEGIN_TEST(uvec4_arr_1)
    UNIFORM_SETUP("uniform uvec4 u[2]=uvec4[](uvec4(1, 2, 3, 4), uvec4(5, 6, 7, 8));\n", "vec4(u[0].xy, u[1].xy)");
    uniform_testd("u[0]", 4, 1., 2., 3., 4.);
    uniform_testd("u[1]", 4, 5., 6., 7., 8.);
END_TEST(uvec4_arr_1)

//============================== Matrix Uniforms ==============================

BEGIN_TEST(mat2_2)
    UNIFORM_SETUP("uniform mat2 u=mat2(1., 2., 3., 4.);\n", "vec4(u[0], u[1])");
    uniform_testd("u", 4, 1., 2., 3., 4.);
END_TEST(mat2_2)

BEGIN_TEST(mat3_2)
    UNIFORM_SETUP("uniform mat3 u=mat3(1., 2., 3., 4., 5., 6., 7., 8., 9.);\n", "vec4(u[0], u[1].x)");
    uniform_testd("u", 9, 1., 2., 3., 4., 5., 6., 7., 8., 9.);
END_TEST(mat3_2)

BEGIN_TEST(mat4_2)
    UNIFORM_SETUP("uniform mat4 u=mat4(1., 2., 3., 4., 5., 6., 7., 8., 9., 10., 11., 12., 13., 14., 15., 16.);\n", "u[0]");
    uniform_testd("u", 16, 1., 2., 3., 4., 5., 6., 7., 8., 9., 10., 11., 12., 13., 14., 15., 16.);
END_TEST(mat4_2)

BEGIN_TEST(mat2x3_2)
    UNIFORM_SETUP("uniform mat2x3 u=mat2x3(1., 2., 3., 4., 5., 6.);\n", "vec4(u[0], u[1].x)");
    uniform_testd("u", 6, 1., 2., 3., 4., 5., 6.);
END_TEST(mat2x3_2)

BEGIN_TEST(mat3x2_2)
    UNIFORM_SETUP("uniform mat3x2 u=mat3x2(1., 2., 3., 4., 5., 6.);\n", "vec4(u[0], u[1])");
    uniform_testd("u", 6, 1., 2., 3., 4., 5., 6.);
END_TEST(mat3x2_2)

BEGIN_TEST(mat2x4_2)
    UNIFORM_SETUP("uniform mat2x4 u=mat2x4(1., 2., 3., 4., 5., 6., 7., 8.);\n", "u[0]");
    uniform_testd("u", 8, 1., 2., 3., 4., 5., 6., 7., 8.);
END_TEST(mat2x4_2)

BEGIN_TEST(mat4x2_2)
    UNIFORM_SETUP("uniform mat4x2 u=mat4x2(1., 2., 3., 4., 5., 6., 7., 8.);\n", "vec4(u[0], u[1])");
    uniform_testd("u", 8, 1., 2., 3., 4., 5., 6., 7., 8.);
END_TEST(mat4x2_2)

BEGIN_TEST(mat3x4_2)
    UNIFORM_SETUP("uniform mat3x4 u=mat3x4(1., 2., 3., 4., 5., 6., 7., 8., 9., 10., 11., 12.);\n", "u[0]");
    uniform_testd("u", 12, 1., 2., 3., 4., 5., 6., 7., 8., 9., 10., 11., 12.);
END_TEST(mat3x4_2)

BEGIN_TEST(mat4x3_2)
    UNIFORM_SETUP("uniform mat4x3 u=mat4x3(1., 2., 3., 4., 5., 6., 7., 8., 9., 10., 11., 12.);\n", "vec4(u[0], u[1].x)");
    uniform_testd("u", 12, 1., 2., 3., 4., 5., 6., 7., 8., 9., 10., 11., 12.);
END_TEST(mat4x3_2)

#define BEGIN_UNIFORM_TEST BEGIN_TEST
#define END_UNIFORM_TEST END_TEST
#define U(type, name, ...) glUniform##type(glGetUniformLocation(program, name), __VA_ARGS__)
#include "uniform_tests.h"

#undef BEGIN_UNIFORM_TEST
#undef END_UNIFORM_TEST
#undef U

#define BEGIN_UNIFORM_TEST(name) BEGIN_TEST(name##_dsa)
#define END_UNIFORM_TEST(name) END_TEST(name##_dsa)
#define U(type, name, ...) do {\
    glUseProgram(0);\
    glProgramUniform##type(program, glGetUniformLocation(program, name), __VA_ARGS__);\
    glUseProgram(program);} while (0)
#include "uniform_tests.h"
