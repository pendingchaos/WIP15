#define GL_GLEXT_PROTOTYPES
#include <GL/gl.h>

typedef struct test_t {
    const char* name;
    void (*func)();
    struct test_t* next;
} test_t;

extern test_t* tests;
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
