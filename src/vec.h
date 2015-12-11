#ifndef VEC_H
#define VEC_H

#include <stddef.h>

typedef struct {
    size_t size;
    void* data;
} vec_data_t;
typedef vec_data_t* vec_t;

vec_t alloc_vec(size_t amount);
void free_vec(vec_t vec);
void append_vec(vec_t vec, size_t amount, void* data);
void resize_vec(vec_t vec, size_t new_size);
void remove_vec(vec_t vec, size_t offset, size_t amount);

#define TYPED_VEC(type, name) typedef vec_t name##_vec_t; \
static inline vec_t alloc_##name##_vec(size_t count) {return alloc_vec(sizeof(type)*count);} \
static inline void free_##name##_vec(name##_vec_t vec) {free_vec(vec);} \
static inline void append_##name##_vec(name##_vec_t vec, type* val) {append_vec(vec, sizeof(type), val);} \
static inline void resize_##name##_vec(name##_vec_t vec, size_t count) {resize_vec(vec, sizeof(type)*count);} \
static inline void remove_##name##_vec(name##_vec_t vec, size_t index, size_t count) {remove_vec(vec, index*sizeof(type), count*sizeof(type));} \
static inline size_t get_##name##_vec_count(name##_vec_t vec) {return vec->size / sizeof(type);} \
static inline type* get_##name##_vec_data(name##_vec_t vec) {return (type*)vec->data;} \
static inline type* get_##name##_vec(name##_vec_t vec, size_t index) {\
    if (index > get_##name##_vec_count(vec))\
        return NULL;\
    return (type*)vec->data + index;\
}
#endif
