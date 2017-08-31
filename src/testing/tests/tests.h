#define GL_GLEXT_PROTOTYPES
#include "shared/glcorearb.h"

typedef struct test_t {
    const char* name;
    void (*func)();
    struct test_t* next;
} test_t;

extern test_t* tests;
extern void (*wip15BeginTest)(const GLchar* name);
extern void (*wip15EndTest)();
extern void (*wip15PrintTestResults)();
extern void (*wip15TestFB)(const GLchar* name, const GLvoid* color, const GLvoid* depth);
extern void (*wip15DrawableSize)(GLsizei width, GLsizei height);
extern void (*wip15ExpectPropertyi64)(GLenum objType, GLuint64 objName,
                                      const char* name, GLuint64 index, GLint64 val);
extern void (*wip15ExpectPropertyd)(GLenum objType, GLuint64 objName,
                                    const char* name, GLuint64 index, GLdouble val);
extern void (*wip15ExpectPropertybv)(GLenum objType, GLuint64 objName,
                                     const char* name, GLuint64 index,
                                     GLuint64 size, const GLvoid* data);
extern void (*wip15ExpectError)(const GLchar* error);

#define _STR_INTERNAL(v) #v
#define _STR(v) _STR_INTERNAL(v)
#define _CONCAT2_INTERNAL(a, b) a##b
#define _CONCAT2(a, b) _CONCAT2_INTERNAL(a, b)
#define _CONCAT4(a, b, c, d) _CONCAT2(_CONCAT2(a, b), _CONCAT2(c, d))

#define BEGIN_TEST(testname)\
void _CONCAT4(test_, TEST_SUITE, _, testname)() {\
    _CONCAT2(TEST_SUITE, _setup)();

#define END_TEST(testname)\
    _CONCAT2(TEST_SUITE, _cleanup)();\
}\
void __attribute__((constructor)) _CONCAT4(register_test_, TEST_SUITE, _, testname)() {\
    static test_t t;\
    t.name = _STR(TEST_SUITE) ": " #testname;\
    t.func = _CONCAT4(test_, TEST_SUITE, _, testname);\
    t.next = tests;\
    tests = &t;\
}

void assert_properties(GLenum objType, GLuint64 objName, ...);
void assert_error(const char* message);

GLuint create_program(const char* vert, const char* frag);
