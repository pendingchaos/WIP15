typedef struct testing_property_t {
    uint minver;
    
    const char* name;
    
    int64_t (*get_func_int)(uint64_t index, const void* rev);
    int64_t (*get_func_gl_int)(uint64_t index, void* ctx, const void* rev, GLuint64 real);
    
    double (*get_func_double)(uint64_t index, const void* rev);
    double (*get_func_gl_double)(uint64_t index, void* ctx, const void* rev, GLuint64 real);
    
    void* (*get_func_data)(uint64_t index, const void* rev, size_t* size);
    void* (*get_func_gl_data)(uint64_t index, void* ctx, const void* rev, GLuint64 real, size_t* size);
    struct testing_property_t* next;
} testing_property_t;

#define REGISTER_PROPERTY(obj, minver_, propname, initcode)\
void __attribute__((constructor)) register_##obj_##propname##_property() {\
    static testing_property_t p;\
    p.minver = minver_;\
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

#define REGISTER_PROPERTY_INT(obj, minver, propname, get, get_gl)\
REGISTER_PROPERTY(obj, minver, propname,\
    p.get_func_int = get;\
    p.get_func_gl_int = get_gl;\
)

#define REGISTER_PROPERTY_DOUBLE(obj, minver, propname, get, get_gl)\
REGISTER_PROPERTY(obj, minver, propname,\
    p.get_func_double = get;\
    p.get_func_gl_double = get_gl;\
)

#define REGISTER_PROPERTY_DATA(obj, minver, propname, get, get_gl)\
REGISTER_PROPERTY(obj, minver, propname,\
    p.get_func_data = get;\
    p.get_func_gl_data = get_gl;\
)

#if REPLAY
#define PROPERTY_INT(obj, minver, propname, getparam, get_code)\
int64_t get_int_prop_##obj##_##propname(uint64_t index, const void* rev_) {\
    const trc_gl_##obj##_rev_t* rev = rev_;\
    return get_code;\
}\
int64_t get_int_prop_##obj##_##propname##_gl(uint64_t index, void* ctx, const void* rev, GLuint64 real) {\
    return get_int_prop_##obj##_gl(index, ctx, real, getparam);\
}\
REGISTER_PROPERTY_INT(obj, minver, propname, &get_int_prop_##obj##_##propname, (getparam?&get_int_prop_##obj##_##propname##_gl:NULL))

#define PROPERTY_DOUBLE(obj, minver, propname, getparam, get_code)\
double get_double_prop_##obj##_##propname(uint64_t index, const void* rev_) {\
    const trc_gl_##obj##_rev_t* rev = rev_;\
    return get_code;\
}\
double get_double_prop_##obj##_##propname##_gl(uint64_t index, void* ctx, const void* rev, GLuint64 real) {\
    return get_double_prop_##obj##_gl(index, ctx, real, getparam);\
}\
REGISTER_PROPERTY_DOUBLE(obj, minver, propname, &get_double_prop_##obj##_##propname, (getparam?&get_double_prop_##obj##_##propname##_gl:NULL))
#else
#define PROPERTY_INT(obj, minver, propname, getparam, get_code)\
int64_t get_int_prop_##obj##_##propname##_gl(uint64_t index, void* ctx, const void* rev, GLuint64 real) {\
    return get_int_prop_##obj##_gl(index, ctx, real, getparam);\
}\
REGISTER_PROPERTY_INT(obj, minver, propname, NULL, (getparam?&get_int_prop_##obj##_##propname##_gl:NULL))

#define PROPERTY_DOUBLE(obj, minver, propname, getparam, get_code)\
double get_double_prop_##obj##_##propname##_gl(uint64_t index, void* ctx, const void* rev, GLuint64 real) {\
    return get_double_prop_##obj##_gl(index, ctx, real, getparam);\
}\
REGISTER_PROPERTY_DOUBLE(obj, minver, propname, NULL, (getparam?&get_double_prop_##obj##_##propname##_gl:NULL))
#endif

#include "buffer.h"
#include "program.h"
#include "context.h"

static const testing_property_t* get_object_type_properties(GLenum objType) {
    switch (objType) {
    case GL_BUFFER:
        return buffer_properties;
    case GL_PROGRAM:
        return program_properties;
    case 0:
        return context_properties;
    //TODO
    }
    return NULL;
}
