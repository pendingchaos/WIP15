#include "shared/types.h"

#include <math.h>

double conv_from_signed_norm(uint glver, int64_t val, size_t bits) {
    if (glver >= 420) return fmax(val/pow(2.0, bits-1), -1.0);
    else return (val*2+1) / pow(2.0, bits-1);
}

double parse_f16(uint16_t val) {
    uint16_t e = (val&0x7fff) >> 10;
    uint16_t m = val & 0x3ff;
    double s = val&0x8000 ? -1.0 : 1.0;
    if (e == 0)
        return s * 6.103515625e-05 * (m/1024.0);
    else if (e>0 && e<31)
        return s * pow(2.0, e-15) * (1.0+m/1024.0);
    else if (e==31 && m==0)
        return s * INFINITY;
    else if (e==31 && m!=0)
        return NAN;
    else
        return NAN; //Should never happen
}

double parse_float11(uint v) {
    uint e = v >> 6;
    uint m = v & 63;
    if (!e && m) return 6.103515625e-05 * (m/64.0);
    else if (e>0 && e<31) return pow(2, e-15) * (1.0+m/64.0);
    else if (e==31 && !m) return INFINITY;
    else if (e==31 && m) return NAN;
    else return NAN; //should never happen
}

double parse_float10(uint v) {
    uint e = v >> 5;
    uint m = v & 31;
    if (!e && m) return 6.103515625e-05 * (m/32.0);
    else if (e>0 && e<31) return pow(2, e-15) * (1.0+m/32.0);
    else if (e==31 && !m) return INFINITY;
    else if (e==31 && m) return NAN;
    else return NAN; //should never happen
}

void parse_int_2_10_10_10_rev(uint glver, double* dest, uint32_t src, bool norm) {
    int64_t vals[4] = {src&1023, src>>10&1023, src>>20&1023, src>>30&3};
    for (size_t i = 0; i < 3; i++)
        if (vals[i] & 512) vals[i] = -512 + (vals[i]&511);
    if (vals[3] & 2) vals[3] = -2 + (vals[3]&1);
    
    for (size_t i = 0; i < 3; i++)
        dest[i] = norm ? conv_from_signed_norm(glver, vals[i], 10) : vals[i];
    dest[3] = norm ? conv_from_signed_norm(glver, vals[3], 2) : vals[3];
}

void parse_uint_2_10_10_10_rev(double* dest, uint32_t src, bool norm) {
    float div10 = norm ? 1023.0 : 1.0;
    float div2 = norm ? 3.0 : 1.0;
    dest[0] = (src&1023) / div10;
    dest[1] = (src>>10&1023) / div10;
    dest[2] = (src>>20&1023) / div10;
    dest[3] = (src>>30&3) / div2;
}

void parse_int_10f_11f_11f_rev(double* dest, uint32_t src) {
    dest[0] = parse_float11(src&2047);
    dest[1] = parse_float11(src>>11&2047);
    dest[2] = parse_float10(src>>22&1023);
}
