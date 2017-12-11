#ifndef REPLAY_UTILS_H
#define REPLAY_UTILS_H
#include "shared/types.h"
#include <stdarg.h>

static inline bool not_one_of(int val, ...) {
    va_list list;
    va_start(list, val);
    while (true) {
        int v = va_arg(list, int);
        if (v == -1) break;
        if (v == val) {
            va_end(list);
            return false;
        }
    }
    va_end(list);
    return true;
}

static inline uint floor_log2(uint val) {
    if (val == 0) return 0; //invalid input - just return zero
    uint lower = 1;
    for (uint guess = 0; guess < sizeof(uint)*8; guess++) {
        uint upper = lower*2u - 1u; //this should be fine due to how unsigned arithmatic wraps around
        if (val>=lower && val<=upper) return guess;
        lower = upper + 1u;
    }
    return 0; //should never happen - just return zero
}

static inline uint ceil_log2(uint val) {
    uint res = 0;
    while (1<<res < val) res++;
    return res;
}

static inline int ceil_div(int a, int b) {
    return a/b + !!(a%b);
    //if (a%b != 0) return a/b + 1;
    //else return a / b;
}

static inline void conv_from_signed_norm_array_i64(size_t count, double* dest, const int64_t* src, size_t bits) {
    uint ver = gls_get_ver();
    for (size_t i = 0; i < count; i++)
        dest[i] = conv_from_signed_norm(ver, src[i], bits);
}

static inline void conv_from_signed_norm_array_i32(size_t count, double* dest, const int32_t* src, size_t bits) {
    uint ver = gls_get_ver();
    for (size_t i = 0; i < count; i++)
        dest[i] = conv_from_signed_norm(ver, src[i], bits);
}

static inline void delete_obj(uint64_t fake, trc_obj_type_t type) {
    if (!fake) return;
    trc_namespace_t* ns;
    switch (type) {
    case TrcFramebuffer:
    case TrcProgramPipeline:
    case TrcVAO:
    case TrcTransformFeedback: ns = ctx->priv_ns; break;
    case TrcContext: ns = &ctx->trace->inspection.global_namespace; break;
    default: ns = ctx->ns; break;
    }
    trc_obj_t* obj = trc_lookup_name(ns, type, fake, -1);
    if (!obj) return;
    trc_drop_obj(obj);
    trc_free_name(ns, type, fake);
}

static inline const trc_gl_transform_feedback_rev_t* get_current_tf() {
    return trc_obj_get_rev(gls_get_current_tf(), -1);
}

//Implemented in programs.replay.c
bool program_has_stage(trc_obj_t* program, GLenum stage);
trc_obj_t* get_active_program_for_stage(GLenum stage);
trc_obj_t* get_active_program();

//Implemented in buffers.replay.c
trc_obj_t* get_bound_buffer(GLenum target);
void update_buffer_from_gl(trc_obj_t* obj, size_t offset, ptrdiff_t size);

//Implemented in framebuffers.replay.c
trc_obj_t* get_bound_framebuffer(GLenum target);

//Implemented in contexts.replay.c
void update_fb0_buffers(bool backcolor, bool frontcolor, bool depth, bool stencil);

//Implemented in misc_objects.replay.c
trc_obj_t** get_tf_buffer_list(size_t* count); 
#endif
