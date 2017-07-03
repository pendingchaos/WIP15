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

BEGIN_TEST(glBindBuffer_0)
    //TODO: assert that the buffer is bound
END_TEST(glBindBuffer_0)

BEGIN_TEST(glBindBuffer_1)
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    //TODO: assert that the buffer is not bound
END_TEST(glBindBuffer_1)

BEGIN_TEST(glBindBuffer_2)
    glBindBuffer(GL_ARRAY_BUFFER, buf+1);
    assert_error("Invalid buffer name");
END_TEST(glBindBuffer_2)

BEGIN_TEST(glBufferData_0)
    glBufferData(GL_ARRAY_BUFFER, 5, NULL, GL_STATIC_DRAW);
    assert_properties(GL_BUFFER, buf, "size", 5, NULL);
END_TEST(glBufferData_0)

BEGIN_TEST(glBufferData_1)
    glBufferData(GL_ARRAY_BUFFER, 5, "Hello", GL_STATIC_DRAW);
    assert_properties(GL_BUFFER, buf, "data", 5, "Hello", NULL);
END_TEST(glBufferData_1)

BEGIN_TEST(glBufferData_2)
    glBufferData(GL_ARRAY_BUFFER, -1, "Hello", GL_STATIC_DRAW);
    assert_error("Invalid size");
END_TEST(glBufferData_2)

BEGIN_TEST(glBufferData_3)
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBufferData(GL_ARRAY_BUFFER, 5, "Hello", GL_STATIC_DRAW);
    assert_error("No buffer bound to target");
END_TEST(glBufferData_3)

BEGIN_TEST(glBufferSubData_0)
    glBufferData(GL_ARRAY_BUFFER, 5, "Hello", GL_STATIC_DRAW);
    glBufferSubData(GL_ARRAY_BUFFER, 1, 3, "Hel");
    assert_properties(GL_BUFFER, buf, "data", 5, "HHelo", NULL);
END_TEST(glBufferSubData_0)

BEGIN_TEST(glBufferSubData_1)
    glBufferData(GL_ARRAY_BUFFER, 5, "Hello", GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBufferSubData(GL_ARRAY_BUFFER, 1, 3, "Hel");
    assert_error("No buffer bound to target");
END_TEST(glBufferSubData_1)

BEGIN_TEST(glBufferSubData_2)
    glBufferData(GL_ARRAY_BUFFER, 5, "Hello", GL_STATIC_DRAW);
    
    glBufferSubData(GL_ARRAY_BUFFER, -1, 3, "Hel");
    assert_error("Invalid offset");
    glBufferSubData(GL_ARRAY_BUFFER, 1, -1, "Hel");
    assert_error("Invalid size");
    glBufferSubData(GL_ARRAY_BUFFER, 1, 6, "Hel");
    assert_error("Invalid range");
    glBufferSubData(GL_ARRAY_BUFFER, 5, 3, "Hel");
    assert_error("Invalid range");
    
    glMapBuffer(GL_ARRAY_BUFFER, GL_MAP_READ_BIT);
    glBufferSubData(GL_ARRAY_BUFFER, 1, 3, "Hel");
    assert_error("Buffer is mapped without persistent access");
    glUnmapBuffer(GL_ARRAY_BUFFER);
    
    //TODO: immutable storage without GL_DYNAMIC_STORAGE_BIT
END_TEST(glBufferSubData_2)

BEGIN_TEST(glMapBuffer_0)
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
END_TEST(glMapBuffer_0)

BEGIN_TEST(glMapBuffer_1)
    glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY);
    assert_properties(GL_BUFFER, buf, "mapped", 1, NULL);
    glUnmapBuffer(GL_ARRAY_BUFFER);
END_TEST(glMapBuffer_1)

BEGIN_TEST(glMapBuffer_2)
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY);
    assert_error("No buffer bound to target");
END_TEST(glMapBuffer_2)

BEGIN_TEST(glMapBuffer_3)
    glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY);
    glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY);
    assert_error("Buffer is already mapped");
END_TEST(glMapBuffer_3)

BEGIN_TEST(glUnmapBuffer_0)
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glUnmapBuffer(GL_ARRAY_BUFFER);
    assert_error("No buffer bound to target");
END_TEST(glUnmapBuffer_0)

BEGIN_TEST(glUnmapBuffer_1)
    glUnmapBuffer(GL_ARRAY_BUFFER);
    assert_error("Buffer is not mapped");
END_TEST(glUnmapBuffer_1)

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
