#include "vec.h"

#include <stdlib.h>
#include <string.h>
#include <stdint.h>

vec_t alloc_vec(size_t amount) {
    vec_data_t* result = malloc(sizeof(vec_data_t));
    result->size = amount;
    result->data = malloc(amount);
    return result;
}

void free_vec(vec_t vec) {
    free(vec->data);
    free(vec);
}

void append_vec(vec_t vec, size_t amount, void* data) {
    size_t old_size = vec->size;
    resize_vec(vec, vec->size+amount);
    memcpy((uint8_t*)vec->data+old_size, data, amount);
}

void resize_vec(vec_t vec, size_t new_size) {
    vec->data = realloc(vec->data, new_size);
    vec->size = new_size;
}

void remove_vec(vec_t vec, size_t offset, size_t amount) {
    memmove((uint8_t*)vec->data+offset,
            (uint8_t*)vec->data+offset+amount,
            vec->size-amount-offset);
    
    resize_vec(vec, vec->size-amount);
}
