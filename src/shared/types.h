#ifndef SHARED_TYPES_H
#define SHARED_TYPES_H
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <stddef.h>

typedef uint32_t uint;

double conv_from_signed_norm(uint glver, int64_t val, size_t bits);

double parse_f16(uint16_t val);
double parse_float11(uint v);
double parse_float10(uint v);

void parse_int_2_10_10_10_rev(uint glver, double* dest, uint32_t src, bool norm);
void parse_uint_2_10_10_10_rev(double* dest, uint32_t src, bool norm);
void parse_int_10f_11f_11f_rev(double* dest, uint32_t src);
#endif
