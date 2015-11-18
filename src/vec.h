#ifndef VEC_H
#define VEC_H

#include <stddef.h>

typedef void* vec_t;

vec_t alloc_vec(size_t amount);
void free_vec(vec_t vec);
void append_vec(vec_t vec, size_t amount, void* data);
void resize_vec(vec_t vec, size_t new_size);
size_t get_vec_size(vec_t vec);
void* get_vec_data(vec_t vec);
#endif
