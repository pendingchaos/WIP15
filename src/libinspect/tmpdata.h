#ifndef TMPDATA_H
#define TMPDATA_H

#include <stdio.h>

typedef struct tmpdata_t {
    char* filename;
    FILE* file;
    size_t size;
} tmpdata_t;

tmpdata_t tmpdata_new(size_t size, const void* data);
void tmpdata_del(tmpdata_t tmpdata);
void tmpdata_read(tmpdata_t tmpdata, void* data);
#endif
