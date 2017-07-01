typedef struct testing_property_t {
    const char* name;
    
    int64_t (*get_func_int)(const void* rev);
    int64_t (*get_func_gl_int)(void* ctx, GLuint64 real);
    
    double (*get_func_double)(const void* rev);
    double (*get_func_gl_double)(void* ctx, GLuint64 real);
    
    void* (*get_func_data)(const void* rev, size_t* size);
    void* (*get_func_gl_data)(void* ctx, GLuint64 real, size_t* size);
    struct testing_property_t* next;
} testing_property_t;

#define REGISTER_PROPERTY(obj, propname, initcode)\
void __attribute__((constructor)) register_##obj_##propname##_property() {\
    static testing_property_t p;\
    p.name = #propname;\
    p.get_func_int = NULL;\
    p.get_func_gl_int = NULL;\
    p.get_func_double = NULL;\
    p.get_func_gl_double = NULL;\
    p.get_func_data = NULL;\
    p.get_func_gl_data = NULL;\
    initcode\
    p.next = obj##_properties;\
    obj##_properties = &p;\
}

#define REGISTER_PROPERTY_INT(obj, propname, get, get_gl)\
REGISTER_PROPERTY(obj, propname,\
    p.get_func_int = get;\
    p.get_func_gl_int = get_gl;\
)

#define REGISTER_PROPERTY_DOUBLE(obj, propname, get, get_gl)\
REGISTER_PROPERTY(obj, propname,\
    p.get_func_double = get;\
    p.get_func_gl_double = get_gl;\
)

#define REGISTER_PROPERTY_DATA(obj, propname, get, get_gl)\
REGISTER_PROPERTY(obj, propname,\
    p.get_func_data = get;\
    p.get_func_gl_data = get_gl;\
)

#if REPLAY
#define PROPERTY_INT(obj, propname, getparam, get_code)\
int64_t get_int_prop_##obj##_##propname(const void* rev_) {\
    const trc_gl_##obj##_rev_t* rev = rev_;\
    return get_code;\
}\
int64_t get_int_prop_##obj##_##propname##_gl(void* ctx, GLuint64 real) {\
    return get_int_prop_##obj##_gl(ctx, real, getparam);\
}\
REGISTER_PROPERTY_INT(obj, propname, &get_int_prop_##obj##_##propname, (getparam?&get_int_prop_##obj##_##propname##_gl:NULL))

#define PROPERTY_DOUBLE(obj, propname, getparam, get_code)\
double test_double_prop_##obj##_##propname(const void* rev_) {\
    const trc_gl_##obj##_rev_t* rev = rev_;\
    return get_code;\
}\
double test_double_prop_##obj##_##propname##_gl(void* ctx, GLuint64 real) {\
    return get_double_prop_##obj##_gl(ctx, real, getparam);\
}\
REGISTER_PROPERTY_DOUBLE(obj, propname, &get_double_prop_##obj##_##propname, (getparam?&get_double_prop_##obj##_##propname##_gl:NULL))
#else
#define PROPERTY_INT(obj, propname, getparam, get_code)\
int64_t get_int_prop_##obj##_##propname##_gl(void* ctx, GLuint64 real) {\
    return get_int_prop_##obj##_gl(ctx, real, getparam);\
}\
REGISTER_PROPERTY_INT(obj, propname, NULL, (getparam?&get_int_prop_##obj##_##propname##_gl:NULL))

#define PROPERTY_DOUBLE(obj, propname, getparam, get_code)\
double test_double_prop_##obj##_##propname##_gl(void* ctx, GLuint64 real) {\
    return get_double_prop_##obj##_gl(ctx, real, getparam);\
}\
REGISTER_PROPERTY_DOUBLE(obj, propname, NULL, (getparam?&get_double_prop_##obj##_##propname##_gl:NULL))
#endif

#include "buffer.h"

static const testing_property_t* get_object_type_properties(GLenum objType) {
    switch (objType) {
    case GL_BUFFER:
        return buffer_properties;
    //TODO
    }
    return NULL;
}
