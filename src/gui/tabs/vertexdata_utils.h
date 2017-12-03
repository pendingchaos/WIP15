#ifndef TABS_VERTEXDATA_UTILS_H
#define TABS_VERTEXDATA_UTILS_H
#include "libtrace/libtrace.h"
#include "shared/glcorearb.h"

typedef struct type_info_t {
    size_t size_mult;
    size_t size_add;
    bool packed;
    
    bool integral;
    bool signed_;
    size_t bits;
} type_info_t;

typedef struct attrib_list_state_t {
    trc_gl_vao_attrib_t attrib;
    trc_gl_vao_buffer_t buffer;
    size_t attrib_index;
    const trc_gl_context_rev_t* ctx_rev;
    const trc_gl_buffer_rev_t* buf_rev;
    uint8_t* data;
} attrib_list_state_t;

typedef struct index_list_state_t {
    GLenum type;
    const trc_gl_context_rev_t* ctx_rev;
    const trc_gl_buffer_rev_t* buf_rev;
    uint8_t* data_start;
    const uint8_t* data;
    size_t data_size;
} index_list_state_t;

void begin_attrib_list(attrib_list_state_t* s, size_t attrib_index,
                       const trc_gl_context_rev_t* ctx_rev,
                       trc_gl_vao_attrib_t attrib, trc_gl_vao_buffer_t buffer);
const char* get_attrib(attrib_list_state_t* s, size_t instance, size_t index);
void end_attrib_list(attrib_list_state_t* s);

void begin_index_list(index_list_state_t* s, GLenum type, uint64_t offset,
                      const trc_gl_vao_rev_t* vao, const trc_gl_context_rev_t* ctx);
int64_t get_index(index_list_state_t* s, size_t index);
void end_index_list(index_list_state_t* s);
#endif
