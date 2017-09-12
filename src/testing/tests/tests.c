#include "tests.h"
#include <SDL2/SDL.h>
#include <stdarg.h>
#include <stdbool.h>

#define F(f) f
#define SWITCH_REPLAY(a, b) b
#include "testing/objects/objects.h"
#undef F

test_t* tests;

static SDL_Window* window;

void (*wip15BeginTest)(const GLchar* name);
void (*wip15EndTest)();
void (*wip15PrintTestResults)();
void (*wip15TestFB)(const GLchar* name, const GLvoid* color, const GLvoid* depth);
void (*wip15DrawableSize)(GLsizei width, GLsizei height);
void (*wip15ExpectPropertyi64)(GLenum objType, GLuint64 objName,
                               const char* name, GLuint64 index, GLint64 val);
void (*wip15ExpectPropertyd)(GLenum objType, GLuint64 objName,
                             const char* name, GLuint64 index, GLdouble val);
void (*wip15ExpectPropertybv)(GLenum objType, GLuint64 objName,
                              const char* name, GLuint64 index,
                              GLuint64 size, const GLvoid* data);
void (*wip15ExpectAttachment)(const GLchar* attachment);

void assert_properties(GLenum objType, GLuint64 objName, ...) {
    va_list list;
    va_start(list, objName);
    while (true) {
        const char* name = va_arg(list, const char*);
        if (!name) break;
        bool wildcard = strcmp(name, "*") == 0;
        
        for (const testing_property_t* prop = get_object_type_properties(objType);
             prop; prop = prop->next) {
            if (!wildcard && strcmp(prop->name, name)!=0) continue;
            if (prop->get_func_gl_int) {
                int64_t val = wildcard ? prop->get_func_gl_int(0, NULL, NULL, objName) :
                                         va_arg(list, int64_t);
                wip15ExpectPropertyi64(objType, objName, prop->name, 0, val);
            } else if (prop->get_func_gl_double) {
                double val = wildcard ? prop->get_func_gl_double(0, NULL, NULL, objName) :
                                        va_arg(list, double);
                wip15ExpectPropertyd(objType, objName, prop->name, 0, val);
            } else if (prop->get_func_gl_data) {
                size_t size = wildcard ? 0 : va_arg(list, int);
                void* val = wildcard ? prop->get_func_gl_data(0, NULL, NULL, objName, &size) :
                                       va_arg(list, void*);
                wip15ExpectPropertybv(objType, objName, prop->name, 0, size, val);
                if (wildcard) free(val);
            }
            if (!wildcard) break;
        }
    }
    
    va_end(list);
}

void assert_attachment(const char* message) {
    wip15ExpectAttachment(message);
}

static void test_fb(const char* name) {
    int drawable_width, drawable_height;
    SDL_GL_GetDrawableSize(window, &drawable_width, &drawable_height);
    
    wip15DrawableSize(drawable_width, drawable_height);
    
    glFinish();
    
    GLint last_buf;
    glGetIntegerv(GL_READ_BUFFER, &last_buf);
    
    glReadBuffer(GL_BACK);
    void* back = malloc(drawable_width*drawable_height*4);
    glReadPixels(0, 0, drawable_width, drawable_height, GL_RGBA, GL_UNSIGNED_BYTE, back);
    
    void* depth = malloc(drawable_width*drawable_height*4);
    glReadPixels(0, 0, drawable_width, drawable_height, GL_DEPTH_COMPONENT, GL_UNSIGNED_INT, depth);
    
    glReadBuffer(last_buf);
    
    wip15TestFB(name, back, depth);
    
    free(back);
    free(depth);
}

int main(int argc, char** argv) {
    SDL_Init(SDL_INIT_VIDEO);
    int pos = SDL_WINDOWPOS_UNDEFINED;
    Uint32 flags = SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN;
    window = SDL_CreateWindow("", pos, pos, 100, 100, flags);
    
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    for (test_t* test = tests; test; test=test->next) {
        SDL_GLContext context = SDL_GL_CreateContext(window);
        
        wip15BeginTest = SDL_GL_GetProcAddress("wip15BeginTest");
        if (!wip15BeginTest) goto fp_fail;
        wip15EndTest = SDL_GL_GetProcAddress("wip15EndTest");
        if (!wip15EndTest) goto fp_fail;
        wip15TestFB = SDL_GL_GetProcAddress("wip15TestFB");
        if (!wip15TestFB) goto fp_fail;
        wip15DrawableSize = SDL_GL_GetProcAddress("wip15DrawableSize");
        if (!wip15DrawableSize) goto fp_fail;
        wip15ExpectPropertyi64 = SDL_GL_GetProcAddress("wip15ExpectPropertyi64");
        if (!wip15ExpectPropertyi64) goto fp_fail;
        wip15ExpectPropertyd = SDL_GL_GetProcAddress("wip15ExpectPropertyd");
        if (!wip15ExpectPropertyd) goto fp_fail;
        wip15ExpectPropertybv = SDL_GL_GetProcAddress("wip15ExpectPropertybv");
        if (!wip15ExpectPropertybv) goto fp_fail;
        wip15ExpectAttachment = SDL_GL_GetProcAddress("wip15ExpectAttachment");
        if (!wip15ExpectAttachment) goto fp_fail;
        
        wip15BeginTest(test->name);
        
        glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT|GL_STENCIL_BUFFER_BIT);
        
        test->func();
        
        SDL_GL_SwapWindow(window);
        test_fb("glXSwapBuffers");
        
        wip15EndTest();
        
        SDL_GL_DeleteContext(context);
        
        continue;
        fp_fail:
        fprintf(stderr, "%s error: Failed to get WIP15 function pointers\n", argv[0]);
        fprintf(stderr, "Ensure that the program is being traced\n");
    }
    
    wip15PrintTestResults = SDL_GL_GetProcAddress("wip15PrintTestResults");
    if (!wip15PrintTestResults) {
        fprintf(stderr, "%s error: Failed to get WIP15 function pointers\n", argv[0]);
        fprintf(stderr, "Ensure that the program is being traced\n");
    }
    
    wip15PrintTestResults();
    
    SDL_DestroyWindow(window);
    SDL_Quit();
}

GLuint create_program(const char* vert, const char* frag) {
    GLuint vertex=0, fragment=0;
    if (vert) {
        vertex = glCreateShader(GL_VERTEX_SHADER);
        glShaderSource(vertex, 1, &vert, NULL);
        glCompileShader(vertex);
    }
    
    if (frag) {
        fragment = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(fragment, 1, &frag, NULL);
        glCompileShader(fragment);
    }
    
    GLuint program = glCreateProgram();
    if (vertex) glAttachShader(program, vertex);
    if (fragment) glAttachShader(program, fragment);
    glLinkProgram(program);
    glValidateProgram(program);
    if (vertex) glDetachShader(program, vertex);
    if (fragment) glDetachShader(program, fragment);
    if (vertex) glDeleteShader(vertex);
    if (fragment) glDeleteShader(fragment);
    
    return program;
}
