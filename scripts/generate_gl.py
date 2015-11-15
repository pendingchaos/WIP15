#!/usr/bin/env python
# -*- coding: utf-8 -*-
import glxml

gl = glxml.GL(False)

nameToID = {}
nextID = 0

for name in gl.functions:
    nameToID[name] = nextID
    nextID += 1

output = open("../src/gl.c", "w")

output.write("""#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <dlfcn.h>
#include <stdio.h>
#include <endian.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>

""")

output.write(gl.typedecls)

output.write("""

#if __WORDSIZE == 64
#elif __WORDSIZE == 32
#else
#error "Unsupported word size."
#endif

#define WIP15_END 0
#define WIP15_U32_ARRAY 1
#define WIP15_STR 2
#define WIP15_PTR32 3
#define WIP15_PTR64 4
#define WIP15_FUNC_PTR 5
#define WIP15_U8 6
#define WIP15_S8 7
#define WIP15_U16 8
#define WIP15_S16 9
#define WIP15_U32 10
#define WIP15_S32 11
#define WIP15_U64 12
#define WIP15_S64 13
#define WIP15_BOOLEAN 14
#define WIP15_BITFIELD 15
#define WIP15_FLOAT 16
#define WIP15_DOUBLE 17
#define WIP15_RESULT 18
#define WIP15_STR_ARRAY 19

static FILE *trace_file;
static void *lib_gl;

void __attribute__ ((destructor)) gl_deinit() {
    fclose(trace_file);
    
    dlclose(lib_gl);
}

static void gl_write_b(uint8_t v) {
    fwrite(&v, 1, 1, trace_file);
}

static void gl_write_str(const char *s) {
    uint32_t length = s == NULL ? 0 : strlen(s);
    uint32_t len_le = htole32(length);
    fwrite(&len_le, 4, 1, trace_file);
    fwrite(s, length, 1, trace_file);
}

static void gl_start(uint32_t func_id) {
    uint32_t func_id_le = htole32(func_id);
    fwrite(&func_id_le, 4, 1, trace_file);
}

static void gl_end() {
    gl_write_b(WIP15_END);
}

static void gl_param_GLuint_array(size_t count, GLuint *data) {
    gl_write_b(WIP15_U32_ARRAY);
    
    uint32_t count_le = htole32(count);
    fwrite(&count_le, 4, 1, trace_file);
    
    size_t i;
    for (i = 0; i < count; ++i)
    {
        uint32_t item = htole32(data[i]);
        fwrite(&item, 4, 1, trace_file);
    }
}

static void gl_param_string_array(const GLchar * const *data, size_t count) {
    gl_write_b(WIP15_STR_ARRAY);
    
    uint32_t count_le = htole32(count);
    fwrite(&count_le, 4, 1, trace_file);
    
    size_t i;
    for (i = 0; i < count; ++i)
    {
        gl_write_str(data[i]);
    }
}

static void gl_param_string(const char *value) {
    gl_write_b(WIP15_STR);
    gl_write_str(value);
}

static void gl_param_pointer(void *value) {
#if __WORDSIZE == 64
    gl_write_b(WIP15_PTR64);
    uint64_t v = htole64((uint64_t)value);
    fwrite(&v, 8, 1, trace_file);
#elif __WORDSIZE == 32
    gl_write_b(WIP15_PTR32);
    uint32_t v = htole32((uint32_t)value);
    fwrite(&v, 4, 1, trace_file);
#endif
}

static void gl_param_GLDEBUGPROC(GLDEBUGPROC proc, const char *group) {
    gl_write_b(WIP15_FUNC_PTR);
}

static void gl_param_GLsizei(GLsizei value, const char *group) {
    gl_write_b(WIP15_S32);
    int32_t v = htole32(value);
    fwrite(&v, 4, 1, trace_file);
    gl_write_str(group);
}

static void gl_param_GLint64EXT(GLint64EXT value, const char *group) {
    gl_write_b(WIP15_S64);
    int64_t v = htole64(value);
    fwrite(&v, 8, 1, trace_file);
    gl_write_str(group);
}

static void gl_param_GLshort(GLshort value, const char *group) {
    gl_write_b(WIP15_S16);
    int16_t v = htole16(value);
    fwrite(&v, 2, 1, trace_file);
    gl_write_str(group);
}

static void gl_param_int64_t(GLshort value, const char *group) {
    gl_write_b(WIP15_S64);
    int64_t v = htole64(value);
    fwrite(&v, 8, 1, trace_file);
    gl_write_str(group);
}

static void gl_param_GLubyte(GLubyte value, const char *group) {
    gl_write_b(WIP15_U8);
    gl_write_b(value);
    gl_write_str(group);
}

static void gl_param_GLDEBUGPROCARB(GLDEBUGPROCARB proc, const char *group) {
    gl_write_b(WIP15_FUNC_PTR);
}

static void gl_param_GLboolean(GLboolean value, const char *group) {
    gl_write_b(WIP15_BOOLEAN);
    gl_write_b(value ? 1 : 0);
    gl_write_str(group);
}

static void gl_param_Bool(Bool value, const char *group) {
    gl_write_b(WIP15_BOOLEAN);
    gl_write_b(value ? 1 : 0);
    gl_write_str(group);
}

static void gl_param_GLbitfield(GLbitfield value, const char *group) {
    gl_write_b(WIP15_BITFIELD);
    fwrite(&value, 4, 1, trace_file);
    gl_write_str(group);
}

static void gl_param_GLsync(GLsync value, const char *group) {
#if __WORDSIZE == 64
    gl_write_b(WIP15_U64);
    uint64_t v = htole64(value);
    fwrite(&v, 8, 1, trace_file);
#elif __WORDSIZE == 32
    gl_write_b(WIP15_U32);
    uint32_t v = htole32(value);
    fwrite(&v, 4, 1, trace_file);
#endif
    gl_write_str(group);
}

static void gl_param_GLuint(GLuint value, const char *group) {
    gl_write_b(WIP15_U32);
    uint32_t v = htole32(value);
    fwrite(&v, 4, 1, trace_file);
    gl_write_str(group);
}

static void gl_param_GLint64(GLint64 value, const char *group) {
    gl_write_b(WIP15_S64);
    int64_t v = htole64(value);
    fwrite(&v, 8, 1, trace_file);
    gl_write_str(group);
}

static void gl_param_int(int value, const char *group) {
    gl_write_b(WIP15_S32);
    int v = htole32(value);
    fwrite(&v, 4, 1, trace_file);
    gl_write_str(group);
}

static void gl_param_GLeglImageOES(GLeglImageOES value, const char *group) {
#if __WORDSIZE == 64
    gl_write_b(WIP15_U64);
    uint64_t v = htole64(value);
    fwrite(&v, 8, 1, trace_file);
#elif __WORDSIZE == 32
    gl_write_b(WIP15_U32);
    uint32_t v = htole32(value);
    fwrite(&v, 4, 1, trace_file);
#endif
    gl_write_str(group);
}

static void gl_param_GLfixed(GLfixed value, const char *group) {
    gl_write_b(WIP15_FLOAT);
    float v = value / 65546.0f;
    fwrite(&v, 4, 1, trace_file);
    gl_write_str(group);
}

static void gl_param_GLclampf(GLclampf value, const char *group) {
    gl_write_b(WIP15_FLOAT);
    fwrite(&value, 4, 1, trace_file);
    gl_write_str(group);
}

static void gl_param_float(float value, const char *group) {
    gl_write_b(WIP15_FLOAT);
    fwrite(&value, 4, 1, trace_file);
    gl_write_str(group);
}

static void gl_param_GLhalfNV(GLhalfNV value, const char *group) { //TODO
    gl_write_b(WIP15_U16);
    fwrite(&value, 2, 1, trace_file);
    gl_write_str(group);
}

static void gl_param_GLintptr(GLintptr value, const char *group) {
#if __WORDSIZE == 64
    gl_write_b(WIP15_U64);
    uint64_t v = htole64(value);
    fwrite(&v, 8, 1, trace_file);
#elif __WORDSIZE == 32
    gl_write_b(WIP15_U32);
    uint32_t v = htole32(value);
    fwrite(&v, 4, 1, trace_file);
#endif
    gl_write_str(group);
}

static void gl_param_GLushort(GLushort value, const char *group) {
    gl_write_b(WIP15_U16);
    uint16_t v = htole16(value);
    fwrite(&v, 2, 1, trace_file);
    gl_write_str(group);
}

static void gl_param_GLenum(GLenum value, const char *group) {
    gl_write_b(WIP15_U32);
    uint32_t v = htole32(value);
    fwrite(&v, 4, 1, trace_file);
    gl_write_str(group);
}

static void gl_param_unsigned_int(unsigned int value, const char *group) {
    gl_write_b(WIP15_U32);
    uint32_t v = htole32(value);
    fwrite(&v, 4, 1, trace_file);
    gl_write_str(group);
}

static void gl_param_GLfloat(GLfloat value, const char *group) {
    gl_write_b(WIP15_FLOAT);
    fwrite(&value, 4, 1, trace_file);
    gl_write_str(group);
}

static void gl_param_GLuint64(GLuint64 value, const char *group) {
    gl_write_b(WIP15_U64);
    uint64_t v = htole64(value);
    fwrite(&v, 8, 1, trace_file);
    gl_write_str(group);
}

static void gl_param_GLdouble(GLdouble value, const char *group) {
    gl_write_b(WIP15_DOUBLE);
    fwrite(&value, 8, 1, trace_file);
    gl_write_str(group);
}

static void gl_param_GLhandleARB(GLhandleARB value, const char *group) {
#ifdef __APPLE__
#if __WORDSIZE == 64
    gl_write_b(WIP15_U64);
    uint64_t v = htole64(value);
    fwrite(&v, 8, 1, trace_file);
#elif __WORDSIZE == 32
    gl_write_b(WIP15_U32);
    uint32_t v = htole32(value);
    fwrite(&v, 4, 1, trace_file);
#endif
#else
    gl_write_b(WIP15_U32);
    uint32_t v = htole32(value);
    fwrite(&v, 4, 1, trace_file);
#endif
    gl_write_str(group);
}

static void gl_param_GLintptrARB(GLintptrARB value, const char *group) {
#if __WORDSIZE == 64
    gl_write_b(WIP15_S64);
    uint64_t v = htole64(value);
    fwrite(&v, 8, 1, trace_file);
#elif __WORDSIZE == 32
    gl_write_b(WIP15_S32);
    uint32_t v = htole32(value);
    fwrite(&v, 4, 1, trace_file);
#endif
    gl_write_str(group);
}

static void gl_param_GLsizeiptr(GLsizeiptr value, const char *group)
{
    gl_param_GLintptrARB(value, group);
}

static void gl_param_GLint(GLint value, const char *group)
{
    gl_write_b(WIP15_S32);
    int32_t v = htole32(value);
    fwrite(&v, 4, 1, trace_file);
    gl_write_str(group);
}

static void gl_param_GLclampx(GLclampx value, const char *group) {
    gl_write_b(WIP15_S32);
    int32_t v = htole32(value);
    fwrite(&v, 4, 1, trace_file);
    gl_write_str(group);
}

static void gl_param_GLsizeiptrARB(GLsizeiptrARB value, const char *group) {
    gl_param_GLsizeiptr(value, group);
}

static void gl_param_GLuint64EXT(GLuint64EXT value, const char *group) {
    gl_write_b(WIP15_U64);
    uint64_t v = htole64(value);
    fwrite(&v, 8, 1, trace_file);
    gl_write_str(group);
}

static void gl_param_GLvdpauSurfaceNV(GLvdpauSurfaceNV value, const char *group) {
    gl_param_GLintptrARB(value, group);
}

static void gl_param_GLbyte(GLbyte value, const char *group) {
    gl_write_b(WIP15_S8);
    fwrite(&value, 1, 1, trace_file);
    gl_write_str(group);
}

static void gl_param_GLclampd(GLclampd value, const char *group) {
    gl_write_b(WIP15_DOUBLE);
    fwrite(&value, 8, 1, trace_file);
    gl_write_str(group);
}

static void gl_param_GLDEBUGPROCKHR(GLDEBUGPROCKHR value, const char *group) {
    gl_write_b(WIP15_FUNC_PTR);
    gl_write_str(group);
}

static void gl_param_GLDEBUGPROCAMD(GLDEBUGPROCAMD value, const char *group) {
    gl_write_b(WIP15_FUNC_PTR);
    gl_write_str(group);
}

static void gl_param_GLXPixmap(GLXPixmap value, const char *group) {
    gl_write_b(WIP15_U32);
    uint32_t v = htole32(value);
    fwrite(&v, 4, 1, trace_file);
    gl_write_str(group);
}

static void gl_param_GLXWindow(GLXWindow value, const char *group) {
    gl_write_b(WIP15_U32);
    uint32_t v = htole32(value);
    fwrite(&v, 4, 1, trace_file);
    gl_write_str(group);
}

static void gl_param_GLXPbuffer(GLXPbuffer value, const char *group) {
    gl_write_b(WIP15_U32);
    uint32_t v = htole32(value);
    fwrite(&v, 4, 1, trace_file);
    gl_write_str(group);
}

static void gl_param_GLXDrawable(GLXDrawable value, const char *group) {
    gl_write_b(WIP15_U32);
    uint32_t v = htole32(value);
    fwrite(&v, 4, 1, trace_file);
    gl_write_str(group);
}

static void gl_param_GLXVideoDeviceNV(GLXVideoDeviceNV value, const char *group) {
    gl_write_b(WIP15_U32);
    uint32_t v = htole32(value);
    fwrite(&v, 4, 1, trace_file);
    gl_write_str(group);
}

static void gl_param_Pixmap(Pixmap value, const char *group) {
    gl_write_b(WIP15_U32);
    uint32_t v = htole32(value);
    fwrite(&v, 4, 1, trace_file);
    gl_write_str(group);
}

static void gl_param_Window(Window value, const char *group) {
    gl_write_b(WIP15_U32);
    uint32_t v = htole32(value);
    fwrite(&v, 4, 1, trace_file);
    gl_write_str(group);
}

static void gl_param_Font(Font value, const char *group) {
    gl_write_b(WIP15_U32);
    uint32_t v = htole32(value);
    fwrite(&v, 4, 1, trace_file);
    gl_write_str(group);
}

static void gl_param_Colormap(Colormap value, const char *group) {
    gl_write_b(WIP15_U32);
    uint32_t v = htole32(value);
    fwrite(&v, 4, 1, trace_file);
    gl_write_str(group);
}

static void gl_param_GLXContextID(GLXContextID value, const char *group) {
    gl_write_b(WIP15_U32);
    uint32_t v = htole32(value);
    fwrite(&v, 4, 1, trace_file);
    gl_write_str(group);
}

static void gl_param_GLXFBConfig(GLXFBConfig value, const char *group) {
#if __WORDSIZE == 64
    gl_write_b(WIP15_U64);
    uint64_t v = htole64(value);
    fwrite(&v, 8, 1, trace_file);
#elif __WORDSIZE == 32
    gl_write_b(WIP15_U32);
    uint32_t v = htole32(value);
    fwrite(&v, 4, 1, trace_file);
#endif
    gl_write_str(group);
}

static void gl_param_GLXVideoCaptureDeviceNV(GLXVideoCaptureDeviceNV value, const char *group) {
    gl_write_b(WIP15_U32);
    uint32_t v = htole32(value);
    fwrite(&v, 4, 1, trace_file);
    gl_write_str(group);
}

static void gl_param_GLXFBConfigSGIX(GLXFBConfig value, const char *group) {
#if __WORDSIZE == 64
    gl_write_b(WIP15_U64);
    uint64_t v = htole64(value);
    fwrite(&v, 8, 1, trace_file);
#elif __WORDSIZE == 32
    gl_write_b(WIP15_U32);
    uint32_t v = htole32(value);
    fwrite(&v, 4, 1, trace_file);
#endif
    gl_write_str(group);
}

static void gl_param_GLXPbufferSGIX(GLXPbufferSGIX value, const char *group) {
    gl_write_b(WIP15_U32);
    uint32_t v = htole32(value);
    fwrite(&v, 4, 1, trace_file);
    gl_write_str(group);
}

static void gl_param_GLXVideoSourceSGIX(GLXVideoSourceSGIX value, const char *group) {
    gl_write_b(WIP15_U32);
    uint32_t v = htole32(value);
    fwrite(&v, 4, 1, trace_file);
    gl_write_str(group);
}

static void gl_param_GLXContext(GLXContext value, const char *group) {
#if __WORDSIZE == 64
    gl_write_b(WIP15_U64);
    uint64_t v = htole64(value);
    fwrite(&v, 8, 1, trace_file);
#elif __WORDSIZE == 32
    gl_write_b(WIP15_U32);
    uint32_t v = htole32(value);
    fwrite(&v, 4, 1, trace_file);
#endif
    gl_write_str(group);
}

static void gl_result_GLenum(GLenum value) {
    gl_write_b(WIP15_RESULT);
    gl_write_b(WIP15_U32);
    uint32_t v = htole32(value);
    fwrite(&v, 4, 1, trace_file);
    gl_write_str(NULL);
}

static void gl_result_GLintptr(GLintptr value) {
    gl_write_b(WIP15_RESULT);
#if __WORDSIZE == 64
    gl_write_b(WIP15_S64);
    uint64_t v = htole64(value);
    fwrite(&v, 8, 1, trace_file);
#elif __WORDSIZE == 32
    gl_write_b(WIP15_S32);
    uint32_t v = htole32(value);
    fwrite(&v, 4, 1, trace_file);
#endif
    gl_write_str(NULL);
}

static void gl_result_GLint(GLint value) {
    gl_write_b(WIP15_RESULT);
    gl_write_b(WIP15_S32);
    uint32_t v = htole32(value);
    fwrite(&v, 4, 1, trace_file);
    gl_write_str(NULL);
}

static void gl_result_GLboolean(GLboolean value) {
    gl_write_b(WIP15_RESULT);
    gl_write_b(WIP15_BOOLEAN);
    fwrite(&value, 1, 1, trace_file);
    gl_write_str(NULL);
}

static void gl_result_GLbitfield(GLbitfield value) {
    gl_write_b(WIP15_RESULT);
    gl_write_b(WIP15_BITFIELD);
    fwrite(&value, 4, 1, trace_file);
    gl_write_str(NULL);
}

static void gl_result_GLsync(GLsync value) {
    gl_write_b(WIP15_RESULT);
#if __WORDSIZE == 64
    gl_write_b(WIP15_U64);
    uint64_t v = htole64(value);
    fwrite(&v, 8, 1, trace_file);
#elif __WORDSIZE == 32
    gl_write_b(WIP15_U32);
    uint32_t v = htole32(value);
    fwrite(&v, 4, 1, trace_file);
#endif
    gl_write_str(NULL);
}

static void gl_result_GLuint64(GLuint64 value) {
    gl_write_b(WIP15_RESULT);
    gl_write_b(WIP15_U64);
    uint64_t v = htole64(value);
    fwrite(&v, 8, 1, trace_file);
    gl_write_str(NULL);
}

static void gl_result_GLvdpauSurfaceNV(GLvdpauSurfaceNV value) {
    gl_write_b(WIP15_RESULT);
#if __WORDSIZE == 64
    gl_write_b(WIP15_S64);
    uint64_t v = htole64(value);
    fwrite(&v, 8, 1, trace_file);
#elif __WORDSIZE == 32
    gl_write_b(WIP15_S32);
    uint32_t v = htole32(value);
    fwrite(&v, 4, 1, trace_file);
#endif
    gl_write_str(NULL);
}

static void gl_result_GLfloat(GLfloat value) {
    gl_write_b(WIP15_RESULT);
    gl_write_b(WIP15_FLOAT);
    fwrite(&value, 4, 1, trace_file);
    gl_write_str(NULL);
}

static void gl_result_GLuint(GLuint value) {
    gl_write_b(WIP15_RESULT);
    gl_write_b(WIP15_U32);
    uint32_t v = htole32(value);
    fwrite(&v, 4, 1, trace_file);
    gl_write_str(NULL);
}

static void gl_result_int64_t(GLuint value) {
    gl_write_b(WIP15_RESULT);
    gl_write_b(WIP15_S64);
    int64_t v = htole64(value);
    fwrite(&v, 8, 1, trace_file);
    gl_write_str(NULL);
}

static void gl_result_unsigned_int(unsigned int value) {
    gl_write_b(WIP15_RESULT);
    gl_write_b(WIP15_U32);
    uint32_t v = htole32(value);
    fwrite(&v, 4, 1, trace_file);
    gl_write_str(NULL);
}

static void gl_result_GLhandleARB(GLhandleARB value) {
    gl_write_b(WIP15_RESULT);
#ifdef __APPLE__
#if __WORDSIZE == 64
    gl_write_b(WIP15_U64);
    uint64_t v = htole64(value);
    fwrite(&v, 8, 1, trace_file);
#elif __WORDSIZE == 32
    gl_write_b(WIP15_U32);
    uint32_t v = htole32(value);
    fwrite(&v, 4, 1, trace_file);
#endif
#else
    gl_write_b(WIP15_U32);
    uint32_t v = htole32(value);
    fwrite(&v, 4, 1, trace_file);
#endif
    gl_write_str(NULL);
}

static void gl_result_GLushort(GLushort value) {
    gl_write_b(WIP15_RESULT);
    gl_write_b(WIP15_U16);
    uint16_t v = htole16(value);
    fwrite(&v, 2, 1, trace_file);
    gl_write_str(NULL);
}

static void gl_result_int(int value) {
    gl_write_b(WIP15_RESULT);
    gl_write_b(WIP15_S32);
    int v = htole32(value);
    fwrite(&v, 4, 1, trace_file);
    gl_write_str(NULL);
}

static void gl_result_Bool(int value) {
    gl_write_b(WIP15_RESULT);
    gl_write_b(WIP15_BOOLEAN);
    uint8_t v = value ? 1 : 0;
    fwrite(&v, 1, 1, trace_file);
    gl_write_str(NULL);
}

static void gl_result_pointer(void *value) {
    gl_write_b(WIP15_RESULT);
#if __WORDSIZE == 64
    gl_write_b(WIP15_PTR64);
    uint64_t v = htole64((uint64_t)value);
    fwrite(&v, 8, 1, trace_file);
#elif __WORDSIZE == 32
    gl_write_b(WIP15_PTR32);
    uint32_t v = htole32((uint32_t)value);
    fwrite(&v, 4, 1, trace_file);
#endif
}

static void gl_result_GLXContext(GLXContext value) {
    gl_write_b(WIP15_RESULT);
#if __WORDSIZE == 64
    gl_write_b(WIP15_S64);
    uint64_t v = htole64(value);
    fwrite(&v, 8, 1, trace_file);
#elif __WORDSIZE == 32
    gl_write_b(WIP15_S32);
    uint32_t v = htole32(value);
    fwrite(&v, 4, 1, trace_file);
#endif
    gl_write_str(NULL);
}

static void gl_result_GLXPixmap(GLXPixmap value) {
    gl_write_b(WIP15_RESULT);
    gl_write_b(WIP15_U32);
    uint32_t v = htole32(value);
    fwrite(&v, 4, 1, trace_file);
    gl_write_str(NULL);
}

static void gl_result_GLXWindow(GLXWindow value) {
    gl_write_b(WIP15_RESULT);
    gl_write_b(WIP15_U32);
    uint32_t v = htole32(value);
    fwrite(&v, 4, 1, trace_file);
    gl_write_str(NULL);
}

static void gl_result_GLXPbuffer(GLXPbuffer value) {
    gl_write_b(WIP15_RESULT);
    gl_write_b(WIP15_U32);
    uint32_t v = htole32(value);
    fwrite(&v, 4, 1, trace_file);
    gl_write_str(NULL);
}

static void gl_result_GLXDrawable(GLXDrawable value) {
    gl_write_b(WIP15_RESULT);
    gl_write_b(WIP15_U32);
    uint32_t v = htole32(value);
    fwrite(&v, 4, 1, trace_file);
    gl_write_str(NULL);
}

static void gl_result_GLXContextID(GLXContextID value) {
    gl_write_b(WIP15_RESULT);
    gl_write_b(WIP15_U32);
    uint32_t v = htole32(value);
    fwrite(&v, 4, 1, trace_file);
    gl_write_str(NULL);
}

static void gl_result___GLXextFuncPtr(__GLXextFuncPtr value) {
    gl_write_b(WIP15_RESULT);
    gl_write_b(WIP15_FUNC_PTR);
    gl_write_str(NULL);
}

static void gl_result_Status(Status value) {
    gl_write_b(WIP15_RESULT);
    gl_write_b(WIP15_S32);
    uint32_t v = htole32(value);
    fwrite(&v, 4, 1, trace_file);
    gl_write_str(NULL);
}

static void gl_result_GLXFBConfigSGIX(GLXFBConfigSGIX value) {
    gl_write_b(WIP15_RESULT);
#if __WORDSIZE == 64
    gl_write_b(WIP15_S64);
    uint64_t v = htole64(value);
    fwrite(&v, 8, 1, trace_file);
#elif __WORDSIZE == 32
    gl_write_b(WIP15_S32);
    uint32_t v = htole32(value);
    fwrite(&v, 4, 1, trace_file);
#endif
    gl_write_str(NULL);
}

static void gl_result_GLXVideoCaptureDeviceNV(GLXVideoCaptureDeviceNV value) {
    gl_write_b(WIP15_RESULT);
    gl_write_b(WIP15_U32);
    uint32_t v = htole32(value);
    fwrite(&v, 4, 1, trace_file);
    gl_write_str(NULL);
}

static void gl_result_GLXPbufferSGIX(GLXPbufferSGIX value) {
    gl_write_b(WIP15_RESULT);
    gl_write_b(WIP15_U32);
    uint32_t v = htole32(value);
    fwrite(&v, 4, 1, trace_file);
    gl_write_str(NULL);
}

""")

for name in gl.functions:
    function = gl.functions[name]
    
    params = []
    
    for param in function.params:
        if param.type_[-1] == "]":
            params.append("%s[%d]" % (param.type_.split("[")[0], eval(param.type_.split("[")[1][:-1])))
        else:
            params.append("%s" % (param.type_))
    
    output.write("typedef %s (*%s_t)(%s);\n" % (function.returnType, name, ", ".join(params)))
    output.write("%s_t gl_f%d;\n" % (name, nameToID[name]))

for name in gl.functions:
    if name in ["glXGetProcAddress", "glXGetProcAddressARB"]:
        continue
    
    function = gl.functions[name]
    
    params = []
    
    for param in function.params:
        if param.type_[-1] == "]":
            params.append("%s %s[%d]" % (param.type_.split("[")[0], param.name, eval(param.type_.split("[")[1][:-1])))
        else:
            params.append("%s %s" % (param.type_, param.name))
    
    output.write("%s %s(%s)" % (function.returnType, name, ",".join(params)))
    output.write("{")
    
    output.write("gl_start(%d);" % (nameToID[name]))
    
    for param in function.params:
        if param.type_[-1] == "]":
            output.write("gl_param_%s_array(%s, %s);" % (param.type_.split("[")[0], eval(param.type_.split("[")[1][:-1]), param.name))
        elif param.type_.replace(" ", "") == "constGLchar*const*":
            output.write("gl_param_pointer((void *)%s);" % (param.name))
        elif param.type_.replace(" ", "") == "constGLchar**":
            output.write("gl_param_pointer((void *)%s);" % (param.name))
        elif param.type_.replace(" ", "") == "constGLcharARB**":
            output.write("gl_param_pointer((void *)%s);" % (param.name))
        elif param.type_.replace(" ", "") == "unsignedint":
            if param.group != None:
                group = "\"%s\"" % (param.group)
            else:
                group = "NULL";
            
            output.write("gl_param_unsigned_int(%s, %s);" % (param.name, group))
        elif param.type_.replace(" ", "") == "unsignedlong":
            if param.group != None:
                group = "\"%s\"" % (param.group)
            else:
                group = "NULL";
            
            output.write("gl_param_unsigned_int(%s, %s);" % (param.name, group))
        elif "*" in param.type_:
            if "GLchar" in param.type_:
                output.write("gl_param_string(%s);" % (param.name))
            else:
                output.write("gl_param_pointer((void*)%s);" % (param.name))
        else:
            if param.group != None:
                group = "\"%s\"" % (param.group)
            else:
                group = "NULL";
            
            output.write("gl_param_%s(%s,%s);" % (param.type_.replace("const", "").lstrip().rstrip(), param.name, group))
    
    if not name.startswith("glX"):
        output.write("if(gl_f%d==NULL){gl_f%d=(%s_t)gl_f%s((const GLubyte*)\"%s\");}" %\
                     (nameToID[name], nameToID[name], name, nameToID["glXGetProcAddress"], name))
    
    if function.returnType != "void":
        output.write("%s result=gl_f%d(%s);" % (function.returnType, nameToID[name], ",".join([param.name for param in function.params])))
    else:
        output.write("gl_f%d(%s);" % (nameToID[name], ", ".join([param.name for param in function.params])))
    
    if function.returnType != "void":
        if "*" in function.returnType:
            output.write("gl_result_pointer((void *)result);")
        elif function.returnType.replace(" ", "") == "unsignedint":
            output.write("gl_result_unsigned_int(result);")
        else:
            output.write("gl_result_%s(result);" % (function.returnType))
    
    output.write("gl_end();")
    
    if function.returnType != "void":
        output.write("return result;")
    
    output.write("}\n")

output.write("__GLXextFuncPtr glXGetProcAddress(const GLubyte *name);\n")
output.write("__GLXextFuncPtr glXGetProcAddressARB(const GLubyte *name);\n")

names = ["glXGetProcAddress", "glXGetProcAddressARB"]
for n in names:
    output.write("""
    __GLXextFuncPtr %s(const GLubyte *name) {
        gl_start(%d);
        gl_param_string(name);
        
        void (*result)() = NULL;
    """ % (n, nameToID[n]))
    
    for name in gl.functions:
        output.write("if(strcmp(name, \"%s\") == 0) {result=(void (*)())%s;}\n" % (name, name))
    
    output.write("""
        if (result==NULL)
            result = gl_f%d(name);

        gl_result_pointer((void*)result);
        gl_end();
        return result;
    }
    
    """ % (nameToID[n]))

output.write("""static void dummy_func() {}

static void* actual_dlopen(const char* filename, int flags) {
    return ((void* (*)(const char*, int))dlsym(RTLD_NEXT, "dlopen"))(filename, flags);
}

void* dlopen(const char* filename, int flags) {
    if (filename == NULL)
        return actual_dlopen(filename, flags);
    
    if (strcmp(filename, "libGL.so") == 0 ||
        strcmp(filename, "libGL.so.1") == 0)
        return actual_dlopen(NULL, flags);
    
    return actual_dlopen(filename, flags);
}""")

output.write("""void __attribute__ ((constructor)) gl_init() {
    char *output = getenv("WIP15_OUTPUT");
    trace_file = fopen(output == NULL ? "output.trace" : output, "wb");
    
    lib_gl = actual_dlopen("libGL.so.1", RTLD_NOW|RTLD_LOCAL);
    
    if (lib_gl == NULL) {
        fprintf(stderr, "Unable to open libGL.so");
        fflush(stderr);
    }
""")

for name in gl.functions:
    if name.startswith("glX"):
        output.write("gl_f%d=(%s_t)dlsym(lib_gl, \"%s\");\n" % (nameToID[name], name, name))
    else:
        output.write("gl_f%d=NULL;\n" % (nameToID[name]))

output.write("uint32_t count = htole32(%d);\n" % (len(gl.functions.keys())))
output.write("fwrite(&count, 4, 1, trace_file);\n")

for name in gl.functions:
    output.write("gl_write_str(\"%s\");\n" % (name))

output.write("}")

output.close()
