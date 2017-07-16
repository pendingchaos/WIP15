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

BEGIN_TEST(glUniform1f)
    UNIFORM_SETUP("uniform float u;\n", "vec4(u)");
    glUniform1f(glGetUniformLocation(program, "u"), 1.0f);
    uniform_testd("u", 1, 1.0);
END_TEST(glUniform1f)

BEGIN_TEST(glUniform2f)
    UNIFORM_SETUP("uniform vec2 u;\n", "vec4(u, u)");
    glUniform2f(glGetUniformLocation(program, "u"), 1.0f, 2.0f);
    uniform_testd("u", 2, 1.0, 2.0);
END_TEST(glUniform2f)
