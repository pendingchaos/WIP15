#include "vertexdata_utils.h"
#include "../gui.h"

#include <math.h>

static type_info_t get_type_info(GLenum type) {
    switch (type) {
    case GL_BYTE: return (type_info_t){1, 0, false, true, true, 8};
    case GL_UNSIGNED_BYTE: return (type_info_t){2, 0, false, true, false, 8};
    case GL_HALF_FLOAT: return (type_info_t){2, 0, false, false, false, 0};
    case GL_SHORT: return (type_info_t){2, 0, false, true, true, 16};
    case GL_UNSIGNED_SHORT: return (type_info_t){2, 0, false, true, false, 16};
    case GL_FLOAT: return (type_info_t){4, 0, false, false, false, 0};
    case GL_FIXED: return (type_info_t){4, 0, false, false, false, 0};
    case GL_INT: return (type_info_t){4, 0, false, true, true, 32};
    case GL_UNSIGNED_INT: return (type_info_t){4, 0, false, true, false, 32};
    case GL_DOUBLE: return (type_info_t){8, 0, false, false, false, 0};
    case GL_INT_2_10_10_10_REV:
        return (type_info_t){0, 4, true, false, false, 0};
    case GL_UNSIGNED_INT_2_10_10_10_REV:
        return (type_info_t){0, 4, true, false, false, 0};
    case GL_UNSIGNED_INT_10F_11F_11F_REV:
        return (type_info_t){0, 4, true, false, false, 0};
    }
    return (type_info_t){0, 0, false, false, false, 0};
}

static size_t get_attrib_size(GLenum type, int size) {
    type_info_t info = get_type_info(type);
    return info.size_mult*size + info.size_add;
}

static double parse_component(GLenum type, const void* src) {
    switch (type) {
    case GL_BYTE: return *(const int8_t*)src;
    case GL_UNSIGNED_BYTE: return *(const uint8_t*)src;
    case GL_HALF_FLOAT: return parse_f16(*(const uint16_t*)src);
    case GL_SHORT: return *(const int16_t*)src;
    case GL_UNSIGNED_SHORT: return *(const uint16_t*)src;
    case GL_FLOAT: return *(const float*)src;
    case GL_FIXED: return *(const int32_t*)src / 65536.0;
    case GL_INT: return *(const int32_t*)src;
    case GL_UNSIGNED_INT: return *(const uint32_t*)src;
    case GL_DOUBLE: return *(const double*)src;
    default: return NAN;
    }
}

static double parse_component_normalized(uint glver, GLenum type, const void* src) {
    double val = parse_component(type, src);
    type_info_t info = get_type_info(type);
    if (info.integral && info.signed_)
        return conv_from_signed_norm(glver, val, info.bits);
    else if (info.integral)
        return val / (pow(2.0, info.bits)-1.0);
    else
        return val;
}

static void parse_vertex_attrib(uint glver, double* dest, const uint8_t* src,
                                const trc_gl_vao_attrib_t* attrib) {
    //Handle packed formats
    switch (attrib->type) {
    case GL_INT_2_10_10_10_REV:
        parse_int_2_10_10_10_rev(glver, dest, *(uint32_t*)src, attrib->normalized);
        return;
    case GL_UNSIGNED_INT_2_10_10_10_REV:
        parse_uint_2_10_10_10_rev(dest, *(uint32_t*)src, attrib->normalized);
        return;
    case GL_UNSIGNED_INT_10F_11F_11F_REV:
        parse_int_10f_11f_11f_rev(dest, *(uint32_t*)src);
        return;
    }
    
    for (size_t i = 0; i < attrib->size; i++) {
        const uint8_t* esrc = src + get_attrib_size(attrib->type, 1)*i;
        if (attrib->normalized)
            dest[i] = parse_component_normalized(glver, attrib->type, esrc);
        else
            dest[i] = parse_component(attrib->type, esrc);
    }
}

void begin_attrib_list(attrib_list_state_t* s, size_t attrib_index,
                       const trc_gl_context_rev_t* ctx_rev,
                       trc_gl_vao_attrib_t attrib) {
    s->attrib = attrib;
    s->attrib_index = attrib_index;
    s->ctx_rev = ctx_rev;
    if (attrib.buffer.obj) {
        s->buf_rev = trc_obj_get_rev(attrib.buffer.obj, state.revision);
        s->data = malloc(s->buf_rev->data.size);
        trc_read_chunked_data_t rinfo;
        rinfo.data = s->buf_rev->data;
        rinfo.start = 0;
        rinfo.size = s->buf_rev->data.size;
        rinfo.dest = s->data;
        trc_read_chunked_data(rinfo); //TODO: Don't read all of the data
    } else {
        s->buf_rev = NULL;
        s->data = NULL;
    }
}

const char* get_attrib(attrib_list_state_t* s, size_t instance, size_t index) {
    if (s->data) {
        if (s->attrib.divisor)
            index = instance / s->attrib.divisor;
        
        size_t size = get_attrib_size(s->attrib.type, s->attrib.size);
        size_t stride = s->attrib.stride ? s->attrib.stride : size;
        size_t offset = index*stride + s->attrib.offset;
        if (offset+size > s->buf_rev->data.size) return NULL;
        
        double vals[4];
        uint glver = s->ctx_rev->ver;
        parse_vertex_attrib(glver, vals, s->data+offset, &s->attrib);
        
        switch (s->attrib.size) {
        case 1:
            return static_format("%g", vals[0]);
        case 2:
            return static_format("[%g, %g]", vals[0], vals[1]);
        case 3:
            return static_format("[%g, %g, %g]", vals[0], vals[1], vals[2]);
        case 4:
            return static_format("[%g, %g, %g, %g]", vals[0], vals[1], vals[2], vals[3]);
        default:
            return NULL;
        }
    } else {
        trc_data_t* cur_attrib =
            s->ctx_rev->state_double_GL_CURRENT_VERTEX_ATTRIB;
        double* src = trc_map_data(cur_attrib, TRC_MAP_READ);
        src += s->attrib_index * 4;
        const char* res = static_format("[%g, %g, %g, %g]", src[0], src[1], src[2], src[3]);
        trc_unmap_data(src);
        return res;
    }
}

void end_attrib_list(attrib_list_state_t* s) {
    free(s->data);
}

void begin_index_list(index_list_state_t* s, GLenum type, uint64_t offset,
                      const trc_gl_vao_rev_t* vao, const trc_gl_context_rev_t* ctx) {
    s->type = type;
    s->ctx_rev = ctx;
    trc_obj_t* buf = vao->element_buffer.obj;
    if (buf) {
        s->buf_rev = trc_obj_get_rev(buf, state.revision);
        s->data_start = malloc(s->buf_rev->data.size);
        trc_read_chunked_data_t rinfo;
        rinfo.data = s->buf_rev->data;
        rinfo.start = 0;
        rinfo.size = s->buf_rev->data.size;
        rinfo.dest = s->data_start;
        trc_read_chunked_data(rinfo); //TODO: Don't read all of the data
        
        s->data = s->data_start + offset;
        if (offset <= s->buf_rev->data.size)
            s->data_size = s->buf_rev->data.size - offset;
        else
            s->data_size = 0;
    } else {
        s->buf_rev = NULL;
        s->data = NULL;
        s->data_start = NULL;
    }
}

int64_t get_index(index_list_state_t* s, size_t index) {
    size_t size = get_attrib_size(s->type, 1);
    if (index*size+size > s->data_size) return -1;
    switch (s->type) {
    case GL_UNSIGNED_BYTE: return s->data[index];
    case GL_UNSIGNED_SHORT: return ((const uint16_t*)s->data)[index];
    case GL_UNSIGNED_INT: return ((const uint32_t*)s->data)[index];
    default: return -1;
    }
    return -1;
}

void end_index_list(index_list_state_t* s) {
    free(s->data_start);
}
