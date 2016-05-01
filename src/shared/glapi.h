#ifndef GLAPI_H
#define GLAPI_H

#include <stddef.h>
#include <stdbool.h>

typedef enum {
    glnone = 0,
    gl1_0 = 1 << 0,
    gl1_1 = 1 << 1,
    gl1_2 = 1 << 2,
    gl1_2_1 = 1 << 3,
    gl1_3 = 1 << 4,
    gl1_4 = 1 << 5,
    gl1_5 = 1 << 6,
    gl2_0 = 1 << 7,
    gl2_1 = 1 << 8,
    gl3_0 = 1 << 9,
    gl3_1 = 1 << 10,
    gl3_2 = 1 << 11,
    gl3_3 = 1 << 12,
    gl4_0 = 1 << 13,
    gl4_1 = 1 << 14,
    gl4_2 = 1 << 15,
    gl4_3 = 1 << 16,
    gl4_4 = 1 << 17,
    gl4_5 = 1 << 18
} glapi_version_mask_t;

typedef struct {
    glapi_version_mask_t version; //Can be 0
    const char* extension; //Can be NULL
    bool compatibility;
    bool core;
} glapi_requirements_t;

typedef struct {
    const glapi_requirements_t* requirements;
    unsigned int value;
    const char* name;
} glapi_group_entry_t;

typedef struct {
    bool bitmask;
    const char *name;
    size_t entry_count;
    const glapi_group_entry_t** entries;
} glapi_group_t;

typedef struct {
    const char* name;
    const glapi_group_t* group; //Can be NULL
} glapi_arg_t;

typedef struct {
    const glapi_requirements_t* requirements;
    const char *name;
    size_t arg_count;
    const glapi_arg_t** args;
} glapi_function_t;

typedef struct {
    size_t group_count;
    glapi_group_t** groups;
    
    size_t function_count;
    glapi_function_t** functions;
} glapi_t;

extern const glapi_t glapi;
#endif
