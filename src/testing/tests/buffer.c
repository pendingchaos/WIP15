#include "tests.h"
#include <string.h>

#define TEST_SUITE buffer

static GLuint buf;

void buffer_setup() {
    glGenBuffers(1, &buf);
    glBindBuffer(GL_ARRAY_BUFFER, buf);
}

void buffer_cleanup() {
    glDeleteBuffers(1, &buf);
}

BEGIN_TEST(default_properties)
    assert_properties(GL_BUFFER, buf, "*", NULL);
END_TEST(default_properties)

BEGIN_TEST(glBufferData_0)
    glBufferData(GL_ARRAY_BUFFER, 5, NULL, GL_STATIC_DRAW);
    assert_properties(GL_BUFFER, buf, "size", 5, NULL);
END_TEST(glBufferData_0)

BEGIN_TEST(glBufferData_1)
    glBufferData(GL_ARRAY_BUFFER, 5, "Hello", GL_STATIC_DRAW);
    assert_properties(GL_BUFFER, buf, "data", 5, "Hello", NULL);
END_TEST(glBufferData_1)

BEGIN_TEST(glBufferSubData)
    glBufferData(GL_ARRAY_BUFFER, 5, "Hello", GL_STATIC_DRAW);
    glBufferSubData(GL_ARRAY_BUFFER, 1, 3, "Hel");
    assert_properties(GL_BUFFER, buf, "data", 5, "HHelo", NULL);
END_TEST(glBufferSubData)

BEGIN_TEST(glMapBuffer)
    glBufferData(GL_ARRAY_BUFFER, 5, NULL, GL_STATIC_DRAW);
    void* data = glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY);
    assert_properties(GL_BUFFER, buf,
        "mapped", 1,
        "map_offset", 0,
        "map_length", 5,
        "map_access", GL_MAP_WRITE_BIT, NULL);
    memcpy(data, "World", 5);
    glUnmapBuffer(GL_ARRAY_BUFFER);
    assert_properties(GL_BUFFER, buf,
        "mapped", 0,
        "map_offset", 0,
        "map_length", 0,
        "map_access", 0,
        "data", 5, "World",
        NULL);
END_TEST(glMapBuffer)

BEGIN_TEST(glMapBufferRange_0)
    glBufferData(GL_ARRAY_BUFFER, 5, NULL, GL_STATIC_DRAW);
    void* data = glMapBufferRange(GL_ARRAY_BUFFER, 0, 5, GL_MAP_WRITE_BIT);
    assert_properties(GL_BUFFER, buf,
        "mapped", 1,
        "map_offset", 0,
        "map_length", 5,
        "map_access", GL_MAP_WRITE_BIT, NULL);
    memcpy(data, "World", 5);
    glUnmapBuffer(GL_ARRAY_BUFFER);
    assert_properties(GL_BUFFER, buf, "data", 5, "World", NULL);
END_TEST(glMapBufferRange_0)

BEGIN_TEST(glMapBufferRange_1)
    glBufferData(GL_ARRAY_BUFFER, 5, "Hello", GL_STATIC_DRAW);
    void* data = glMapBufferRange(GL_ARRAY_BUFFER, 1, 3, GL_MAP_WRITE_BIT|GL_MAP_READ_BIT);
    assert_properties(GL_BUFFER, buf,
        "mapped", 1,
        "map_offset", 1,
        "map_length", 3,
        "map_access", GL_MAP_WRITE_BIT|GL_MAP_READ_BIT, NULL);
    memcpy(data, "Hel", 3);
    glUnmapBuffer(GL_ARRAY_BUFFER);
    assert_properties(GL_BUFFER, buf, "data", 5, "HHelo", NULL);
END_TEST(glMapBufferRange_1)

/*void buffer_test() {
    currentTest("Setup");
    GLuint buf;
    glGenBuffers(1, &buf);
    glBindBuffer(GL_ARRAY_buf);
    assert_properties(GL_BUFFER, buf, "*", NULL);
    
    currentTest("glMapBuffer");
    void* data = glMapBuffer(GL_ARRAY_GL_WRITE_ONLY);
    assert_properties(GL_BUFFER, buf,
        "mapped", 1,
        "map_offset", 0,
        "map_length", 5,
        "map_access", GL_MAP_WRITE_BIT, NULL);
    memcpy(data, "World", 5);
    glUnmapBuffer(GL_ARRAY_BUFFER);
    assert_properties(GL_BUFFER, buf,
        "mapped", 0,
        "map_offset", 0,
        "map_length", 0,
        "map_access", 0,
        "data": "World"
        NULL);
    
    currentTest("glBufferSubData");
    glBufferSubData(GL_ARRAY_0, 5, "Hello");
    assert_properties(GL_BUFFER, buf, "data", "Hello", NULL);
    
    currentTest("glMapBufferRange");
    data = glMapBufferRange(GL_ARRAY_0, 5, GL_MAP_WRITE_BIT);
    assert_properties(GL_BUFFER, buf,
        "mapped", 1,
        "map_offset", 0,
        "map_length", 5,
        "map_access", GL_MAP_WRITE_BIT, NULL);
    memcpy(data, "World", 5);
    glUnmapBuffer(GL_ARRAY_BUFFER);
    assert_properties(GL_BUFFER, buf, "data", "World", NULL);
    
    currentTest("Cleanup");
    glDeleteBuffers(1, &buf);
}

REGISTER_TEST(Buffers, buffer_test,
    glBufferData, glMapglUnmapBuffer,
    glBufferSubData, glMapBufferRange)*/
