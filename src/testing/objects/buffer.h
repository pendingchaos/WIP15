static testing_property_t* buffer_properties = NULL;

static int64_t get_int_prop_buffer_gl(uint64_t index, void* ctx, GLuint real, GLenum param) {
    GLint prev;
    F(glGetIntegerv)(GL_ARRAY_BUFFER_BINDING, &prev);
    F(glBindBuffer)(GL_ARRAY_BUFFER, real);
    GLint64 res;
    F(glGetBufferParameteri64v)(GL_ARRAY_BUFFER, param, &res);
    F(glBindBuffer)(GL_ARRAY_BUFFER, prev);
    return res;
}

#if REPLAY
static void* get_data_prop_buffer_data(uint64_t index, const void* rev_, size_t* size) {
    const trc_gl_buffer_rev_t* rev = rev_;
    *size = rev->data.size;
    void* data = malloc(*size);
    trc_read_chunked_data((trc_read_chunked_data_t)
        {.data=rev->data, .start=0, .size=rev->data.size, .dest=data});
    return data;
}
#endif

static void* get_data_prop_buffer_data_gl(uint64_t index, void* ctx, const void* rev, GLuint64 real, size_t* size) {
    GLint prev;
    F(glGetIntegerv)(GL_ARRAY_BUFFER_BINDING, &prev);
    F(glBindBuffer)(GL_ARRAY_BUFFER, real);
    
    GLint sizei;
    F(glGetBufferParameteriv)(GL_ARRAY_BUFFER, GL_BUFFER_SIZE, &sizei);
    
    *size = sizei;
    void* bufdata = malloc(*size);
    F(glGetBufferSubData)(GL_ARRAY_BUFFER, 0, *size, bufdata);
    
    F(glBindBuffer)(GL_ARRAY_BUFFER, prev);
    return bufdata;
}

PROPERTY_INT(buffer, 320, size, GL_BUFFER_SIZE,
    rev->data.size)
PROPERTY_INT(buffer, 320, usage, GL_BUFFER_USAGE,
    rev->data_usage)
PROPERTY_INT(buffer, 320, mapped, SWITCH_REPLAY(0, GL_BUFFER_MAPPED),
    rev->mapped)
PROPERTY_INT(buffer, 320, map_offset, SWITCH_REPLAY(0, GL_BUFFER_MAP_OFFSET),
    rev->map_offset)
PROPERTY_INT(buffer, 320, map_length, SWITCH_REPLAY(0, GL_BUFFER_MAP_LENGTH),
    rev->map_length)
PROPERTY_INT(buffer, 320, map_access, SWITCH_REPLAY(0, GL_BUFFER_ACCESS_FLAGS),
    rev->map_access)
REGISTER_PROPERTY_DATA(buffer, 320, data, SWITCH_REPLAY(get_data_prop_buffer_data, NULL), get_data_prop_buffer_data_gl)
//TODO: GL_BUFFER_IMMUTABLE_STORAGE
//TODO: GL_BUFFER_STORAGE_FLAGS
