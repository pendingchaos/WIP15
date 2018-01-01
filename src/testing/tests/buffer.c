//TODO: Test with large, multi-chunked buffers
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
    assert_attachment("Invalid buffer name");
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
    assert_attachment("Invalid size");
END_TEST(glBufferData_2)

BEGIN_TEST(glBufferData_3)
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBufferData(GL_ARRAY_BUFFER, 5, "Hello", GL_STATIC_DRAW);
    assert_attachment("No buffer bound to target");
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
    assert_attachment("No buffer bound to target");
END_TEST(glBufferSubData_1)

BEGIN_TEST(glBufferSubData_2)
    glBufferData(GL_ARRAY_BUFFER, 5, "Hello", GL_STATIC_DRAW);
    
    glBufferSubData(GL_ARRAY_BUFFER, -1, 3, "Hel");
    assert_attachment("Invalid offset");
    glBufferSubData(GL_ARRAY_BUFFER, 1, -1, "Hel");
    assert_attachment("Invalid size");
    glBufferSubData(GL_ARRAY_BUFFER, 1, 6, "Hel");
    assert_attachment("Invalid range");
    glBufferSubData(GL_ARRAY_BUFFER, 5, 3, "Hel");
    assert_attachment("Invalid range");
    
    glMapBuffer(GL_ARRAY_BUFFER, GL_READ_ONLY);
    glBufferSubData(GL_ARRAY_BUFFER, 1, 3, "Hel");
    assert_attachment("Buffer is mapped without persistent access");
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
    //TODO: The nvidia driver seems to fail at glMapBuffer with
    //GL_INVALID_OPERATION if this is not called
    glBufferData(GL_ARRAY_BUFFER, 5, NULL, GL_STATIC_DRAW);
    
    glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY);
    assert_properties(GL_BUFFER, buf, "mapped", 1, NULL);
    glUnmapBuffer(GL_ARRAY_BUFFER);
END_TEST(glMapBuffer_1)

BEGIN_TEST(glMapBuffer_2)
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY);
    assert_attachment("No buffer bound to target");
END_TEST(glMapBuffer_2)

BEGIN_TEST(glMapBuffer_3)
    glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY);
    glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY);
    assert_attachment("Buffer is already mapped");
END_TEST(glMapBuffer_3)

BEGIN_TEST(glUnmapBuffer_0)
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glUnmapBuffer(GL_ARRAY_BUFFER);
    assert_attachment("No buffer bound to target");
END_TEST(glUnmapBuffer_0)

BEGIN_TEST(glUnmapBuffer_1)
    glUnmapBuffer(GL_ARRAY_BUFFER);
    assert_attachment("Buffer is not mapped");
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

BEGIN_TEST(glMapBufferRange_2)
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glMapBufferRange(GL_ARRAY_BUFFER, 1, 3, GL_MAP_WRITE_BIT|GL_MAP_READ_BIT);
    assert_attachment("No buffer bound to target");
END_TEST(glMapBufferRange_2)

BEGIN_TEST(glMapBufferRange_3)
    glBufferData(GL_ARRAY_BUFFER, 5, "Hello", GL_STATIC_DRAW);
    
    glMapBufferRange(GL_ARRAY_BUFFER, -1, 3, GL_MAP_WRITE_BIT|GL_MAP_READ_BIT);
    assert_attachment("Invalid offset");
    glMapBufferRange(GL_ARRAY_BUFFER, 1, -3, GL_MAP_WRITE_BIT|GL_MAP_READ_BIT);
    assert_attachment("Invalid length");
    glMapBufferRange(GL_ARRAY_BUFFER, 1, 0, GL_MAP_WRITE_BIT|GL_MAP_READ_BIT);
    assert_attachment("Invalid length");
    glMapBufferRange(GL_ARRAY_BUFFER, 5, 3, GL_MAP_WRITE_BIT|GL_MAP_READ_BIT);
    assert_attachment("Invalid range");
    glMapBufferRange(GL_ARRAY_BUFFER, 1, 7, GL_MAP_WRITE_BIT|GL_MAP_READ_BIT);
    assert_attachment("Invalid range");
END_TEST(glMapBufferRange_3)

BEGIN_TEST(glMapBufferRange_4)
    glBufferData(GL_ARRAY_BUFFER, 5, "Hello", GL_STATIC_DRAW);
    
    glMapBuffer(GL_ARRAY_BUFFER, GL_READ_ONLY);
    glMapBufferRange(GL_ARRAY_BUFFER, 1, 3, GL_MAP_WRITE_BIT|GL_MAP_READ_BIT);
    assert_attachment("Buffer is already mapped");
    glUnmapBuffer(GL_ARRAY_BUFFER);
    
    glMapBufferRange(GL_ARRAY_BUFFER, 1, 3, 0);
    assert_attachment("Neither GL_MAP_READ_BIT or GL_MAP_WRITE_BIT is set");
    
    glMapBufferRange(GL_ARRAY_BUFFER, 1, 3, GL_MAP_READ_BIT|GL_MAP_INVALIDATE_BUFFER_BIT);
    assert_attachment("GL_MAP_READ_BIT is set and GL_MAP_INVALIDATE_RANGE_BIT, GL_MAP_INVALIDATE_BUFFER_BIT and/or GL_MAP_UNSYNCHRONIZED_BIT is set");
    
    glMapBufferRange(GL_ARRAY_BUFFER, 1, 3, GL_MAP_FLUSH_EXPLICIT_BIT|GL_MAP_READ_BIT);
    assert_attachment("GL_MAP_FLUSH_EXPLICIT_BIT is set but GL_MAP_WRITE_BIT is not");
    
    //TODO: Test access bits against buffer storage flags
END_TEST(glMapBufferRange_4)

BEGIN_TEST(glCopyBufferSubData_0)
    glBufferData(GL_ARRAY_BUFFER, 5, "World", GL_STATIC_DRAW);
    
    GLuint buf2;
    glGenBuffers(1, &buf2);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buf2);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, 5, "Hello", GL_STATIC_DRAW);
    
    glCopyBufferSubData(GL_ELEMENT_ARRAY_BUFFER, GL_ARRAY_BUFFER, 0, 0, 5);
    assert_properties(GL_BUFFER, buf, "data", 5, "Hello", NULL);
    
    glCopyBufferSubData(GL_ELEMENT_ARRAY_BUFFER, GL_ARRAY_BUFFER, 0, 2, 3);
    assert_properties(GL_BUFFER, buf, "data", 5, "HeHel", NULL);
    
    glCopyBufferSubData(GL_ARRAY_BUFFER, GL_ARRAY_BUFFER, 0, 3, 2);
    assert_properties(GL_BUFFER, buf, "data", 5, "HeHHe", NULL);
END_TEST(glCopyBufferSubData_0)

BEGIN_TEST(glCopyBufferSubData_1)
    glBufferData(GL_ARRAY_BUFFER, 5, "World", GL_STATIC_DRAW);
    
    glCopyBufferSubData(GL_ARRAY_BUFFER, GL_ELEMENT_ARRAY_BUFFER, 0, 0, 5);
    assert_attachment("No buffer bound to write target");
    glCopyBufferSubData(GL_ELEMENT_ARRAY_BUFFER, GL_ARRAY_BUFFER, 0, 0, 5);
    assert_attachment("No buffer bound to read target");
    
    GLuint buf2;
    glGenBuffers(1, &buf2);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buf2);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, 5, "Hello", GL_STATIC_DRAW);
    
    glCopyBufferSubData(GL_ELEMENT_ARRAY_BUFFER, GL_ARRAY_BUFFER, -1, 0, 5);
    assert_attachment("The read offset, write offset or size is negative");
    glCopyBufferSubData(GL_ELEMENT_ARRAY_BUFFER, GL_ARRAY_BUFFER, 0, -1, 5);
    assert_attachment("The read offset, write offset or size is negative");
    glCopyBufferSubData(GL_ELEMENT_ARRAY_BUFFER, GL_ARRAY_BUFFER, 0, 0, -1);
    assert_attachment("The read offset, write offset or size is negative");
    
    glCopyBufferSubData(GL_ELEMENT_ARRAY_BUFFER, GL_ARRAY_BUFFER, 1, 0, 5);
    assert_attachment("Invalid size and read offset");
    glCopyBufferSubData(GL_ELEMENT_ARRAY_BUFFER, GL_ARRAY_BUFFER, 0, 1, 5);
    assert_attachment("Invalid size and write offset");
    
    glMapBuffer(GL_ELEMENT_ARRAY_BUFFER, GL_READ_ONLY);
    glCopyBufferSubData(GL_ELEMENT_ARRAY_BUFFER, GL_ARRAY_BUFFER, 0, 0, 5);
    assert_attachment("Reading from a buffer that is mapped without GL_MAP_PERSISTENT_BIT set");
    glUnmapBuffer(GL_ELEMENT_ARRAY_BUFFER);
    
    glMapBuffer(GL_ARRAY_BUFFER, GL_READ_ONLY);
    glCopyBufferSubData(GL_ELEMENT_ARRAY_BUFFER, GL_ARRAY_BUFFER, 0, 0, 5);
    assert_attachment("Writing to a buffer that is mapped without GL_MAP_PERSISTENT_BIT set");
    glUnmapBuffer(GL_ARRAY_BUFFER);
    
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buf);
    glCopyBufferSubData(GL_ELEMENT_ARRAY_BUFFER, GL_ARRAY_BUFFER, 0, 0, 5);
    assert_attachment("Overlapping read and write ranges");
    glCopyBufferSubData(GL_ELEMENT_ARRAY_BUFFER, GL_ARRAY_BUFFER, 0, 2, 3);
    assert_attachment("Overlapping read and write ranges");
END_TEST(glCopyBufferSubData_1)
