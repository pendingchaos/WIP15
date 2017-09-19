#ifndef UTILS_H
#define UTILS_H
#include "shared/types.h"

typedef struct str_t {
    bool tmp;
    size_t length;
    char* data;
} str_t;

typedef struct data_reader_t {
    const uint8_t* data;
    size_t bytes_left;
} data_reader_t;

typedef struct data_writer_t {
    bool resizable;
    uint8_t* data;
    size_t size;
    size_t capacity;
} data_writer_t;

void* alloc(void* parent, size_t size);
void* copy(void* parent, size_t size, const void* src);
void* copy2(void* parent, size_t size, size_t src_size, const void* src);
void* resize(void* ptr, size_t size);
void dealloc(void* ptr);

str_t str_new(void* parent, ptrdiff_t length, const void* data);
str_t str_null();
str_t str_copy(void* parent, str_t str);
str_t str_fmt(void* parent, const char* format, ...);
str_t str_cat(void* parent, str_t a, str_t b);
void str_del(str_t str);
str_t str_tmp(str_t str);

data_reader_t dr_new(size_t data_size, const uint8_t* data);
bool dr_read_str(data_reader_t* reader, void* parent, size_t length, str_t* str);
bool dr_read(data_reader_t* reader, size_t amount, void* dest);
bool dr_read_le(data_reader_t* reader, ...);

data_writer_t dw_new(size_t data_size, uint8_t* data, bool resizable);
bool dw_write(data_writer_t* writer, size_t amount, const void* src);
bool dw_write_le(data_writer_t* writer, ...);
#endif
