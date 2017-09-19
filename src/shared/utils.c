#include "shared/utils.h"

#include <endian.h>
#include <string.h>
#include <stdarg.h>
#include <setjmp.h>
#include <stdio.h>

typedef struct header_t {
    void* first_sibling;
    void** prev_nextp;
    void* next;
} header_t;

static header_t* get_header(void* ptr) {
    return ((header_t*)ptr) - 1;
}

void* alloc(void* parent, size_t size) {
    return copy(parent, size, NULL);
}

void* copy2(void* parent, size_t size, size_t src_size, const void* src) {
    header_t* header = malloc(sizeof(header_t)+size);
    void* res = header + 1;
    if (src) {
        memcpy(res, src, src_size);
        memset(res+src_size, 0, size-src_size);
    } else {
        memset(res, 0, size);
    }
    
    if (parent) {
        header_t* parent_header = get_header(parent);
        if (parent_header->first_sibling) {
            header_t* sibling_header = get_header(parent_header->first_sibling);
            sibling_header->prev_nextp = &header->next;
        }
        header->next = parent_header->first_sibling;
        parent_header->first_sibling = res;
        header->prev_nextp = &parent_header->first_sibling;
    } else {
        header->next = NULL;
        header->prev_nextp = NULL;
    }
    
    header->first_sibling = NULL;
    
    return res;
}

void* copy(void* parent, size_t size, const void* src) {
    return copy2(parent, size, size, src);
}

void* resize(void* ptr, size_t size) {
    header_t* header = realloc(get_header(ptr), size+sizeof(header_t));
    if (!header) return NULL;
    void* newptr = header + 1;
    
    if (header->first_sibling) {
        header_t* sibling_header = get_header(header->first_sibling);
        sibling_header->prev_nextp = &header->first_sibling;
    }
    
    if (header->next)
        get_header(header->next)->prev_nextp = &header->next;
    
    return newptr;
}

void dealloc(void* ptr) {
    header_t* header = get_header(ptr);
    
    for (void* sibling = header->first_sibling; sibling;) {
        void* next = get_header(sibling)->next;
        dealloc(sibling);
        sibling = next;
    }
    
    if (header->prev_nextp) *header->prev_nextp = header->next;
    if (header->next)
        get_header(header->next)->prev_nextp = header->prev_nextp;
    
    free(header);
}

static void use_str(str_t str) {
    if (str.tmp) str_del(str);
}

str_t str_new(void* parent, ptrdiff_t length, const void* data) {
    str_t res;
    res.tmp = false;
    res.length = length<0 ? strlen(data) : length;
    res.data = copy2(parent, res.length+1, res.length, data);
    return res;
}

str_t str_null() {
    str_t res;
    res.tmp = false;
    res.length = 0;
    res.data = NULL;
    return res;
}

str_t str_copy(void* parent, str_t str) {
    str_t res = str_new(parent, str.length, str.data);
    use_str(str);
    return res;
}

str_t str_fmt(void* parent, const char* format, ...) {
    va_list list, list2;
    va_start(list, format);
    va_copy(list2, list);
    
    size_t len = vsnprintf(NULL, 0, format, list2);
    
    str_t res = str_new(parent, len, NULL);
    vsnprintf(res.data, res.length, format, list);
    
    va_end(list);
    
    return res;
}

str_t str_cat(void* parent, str_t a, str_t b) {
    if (a.tmp) {
        a.data = resize(a.data, a.length+b.length+1);
        memcpy(a.data+a.length, b.data, b.length);
        use_str(b);
        a.length += b.length;
        a.data[a.length] = 0;
        return a;
    } else {
        str_t res = str_new(parent, a.length+b.length, NULL);
        memcpy(res.data, a.data, a.length);
        memcpy(res.data+a.length, b.data, b.length);
        use_str(a);
        use_str(b);
        return res;
    }
}

void str_del(str_t str) {
    dealloc(str.data);
}

str_t str_tmp(str_t str) {
    str.tmp = true;
    return str;
}

data_reader_t dr_new(size_t data_size, const uint8_t* data) {
    data_reader_t res;
    res.data = data;
    res.bytes_left = data_size;
    return res;
}

bool dr_read_str(data_reader_t* reader, void* parent, size_t length, str_t* str) {
    if (reader->bytes_left < length) return false;
    for (size_t i = 0; i < length; i++)
        if (reader->data[i] == 0) return false;
    *str = str_new(parent, length, NULL);
    dr_read(reader, length, str->data);
    return true;
}

bool dr_read(data_reader_t* reader, size_t amount, void* dest) {
    if (reader->bytes_left < amount) return false;
    memcpy(dest, reader->data, amount);
    reader->data += amount;
    reader->bytes_left -= amount;
    return true;
}

bool dr_read_le(data_reader_t* reader, ...) {
    va_list list;
    va_start(list, reader);
    bool success = true;
    while (true) {
        int size = va_arg(list, int);
        if (size < 0) break;
        uint8_t* dest = va_arg(list, uint8_t*);
        
        if (reader->bytes_left < size) {
            success = false;
            break;
        }
        
        bool swap = htole16(1) != 1;
        for (int i = 0; i < size; i++)
            dest[i] = swap ? reader->data[size-i-1] : reader->data[i];
        
        reader->data += size;
        reader->bytes_left -= size;
    }
    va_end(list);
    return success;
}

data_writer_t dw_new(size_t data_size, uint8_t* data, bool resizable) {
    data_writer_t res;
    res.resizable = resizable;
    res.data = data;
    res.size = 0;
    res.capacity = data_size;
    return res;
}

static bool write(data_writer_t* writer, size_t amount, bool swap, const uint8_t* src) {
    size_t left = writer->capacity - writer->size;
    if (left<amount && !writer->resizable) {
        return false;
    } else if (left<amount) {
        while (writer->capacity-writer->size < amount)
            writer->capacity *= 2;
        writer->data = resize(writer->data, writer->capacity);
    }
    
    for (size_t i = 0; i < amount; i++)
        writer->data[writer->size++] = swap ? src[amount-i-1] : src[i];
    
    return true;
}

bool dw_write(data_writer_t* writer, size_t amount, const void* src) {
    return write(writer, amount, false, src);
}

bool dw_write_le(data_writer_t* writer, ...) {
    va_list list;
    va_start(list, writer);
    bool success = true;
    while (true) {
        int size = va_arg(list, int);
        if (size < 0) break;
        const uint8_t* src = va_arg(list, const uint8_t*);
        
        bool swap = htole16(1) != 1;
        success = success && write(writer, size, swap, src);
        if (!success) break;
    }
    va_end(list);
    return success;
}
