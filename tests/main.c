#define GL_GLEXT_PROTOTYPES
#include <GL/gl.h>
#include <SDL2/SDL.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>

void (*wip15CurrentTest)(const GLchar* name);
void (*wip15TestFB)(const GLchar* name, const GLvoid* color, const GLvoid* depth);
void (*wip15DrawableSize)(GLsizei width, GLsizei height);

static SDL_Window* window;

static char* static_format(const char* format, ...) {
    static char data[1024];
    memset(data, 0, 1024);
    
    va_list list;
    va_start(list, format);
    vsnprintf(data, 1024, format, list);
    va_end(list);
    
    return data;
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

GLuint create_program(const char* vert, const char* frag) {
    GLuint vertex = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex, 1, &vert, NULL);
    glCompileShader(vertex);
    
    GLuint fragment = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment, 1, &frag, NULL);
    glCompileShader(fragment);
    
    GLuint program = glCreateProgram();
    glAttachShader(program, vertex);
    glAttachShader(program, fragment);
    glLinkProgram(program);
    glValidateProgram(program);
    
    return program;
}

GLuint buffer(GLenum target, size_t size, void* data) {
    GLuint buf;
    glGenBuffers(1, &buf);
    glBindBuffer(target, buf);
    glBufferData(target, size, data, GL_STATIC_DRAW);
    return buf;
}

#include "draw.h"
#include "buffer.h"
#include "texture.h"
#include "uniform.h"

void null_current_test(const GLchar* name) {}
void null_test_fb(const GLchar* name, const GLvoid* color, const GLvoid* depth) {}
void null_drawable_size(GLsizei width, GLsizei height) {}

//TODO: Test if the program is being traced and print a warning if it is not
int main(int argc, char** argv) {
    SDL_Init(SDL_INIT_VIDEO);
    int pos = SDL_WINDOWPOS_UNDEFINED;
    Uint32 flags = SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN;
    window = SDL_CreateWindow("", pos, pos, 100, 100, flags);
    
    void (*tests[])() = {&draw_test, &buffer_test, &texture_test,
                         &uniform_test};
    
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
    for (size_t i = 0; i < sizeof(tests)/sizeof(tests[0]); i++) {
        SDL_GLContext context = SDL_GL_CreateContext(window);
        
        bool problem = false;
        wip15CurrentTest = SDL_GL_GetProcAddress("wip15CurrentTest");
        if ((problem|=!wip15CurrentTest)) wip15CurrentTest = &null_current_test;
        wip15TestFB = SDL_GL_GetProcAddress("wip15TestFB");
        if ((problem|=!wip15TestFB)) wip15TestFB = &null_test_fb;
        wip15DrawableSize = SDL_GL_GetProcAddress("wip15DrawableSize");
        if ((problem|=!wip15DrawableSize)) wip15DrawableSize = &null_drawable_size;
        
        if (problem) {
            fprintf(stderr, "Warning: Failed to get function pointers for "
                            "wip15CurrentTest, wip15TestFB and/or wip15DrawableSize\n");
        }
        
        glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT|GL_STENCIL_BUFFER_BIT);
        
        tests[i]();
        SDL_GL_SwapWindow(window);
        test_fb("glXSwapBuffers");
        
        /*bool running = true;
        SDL_Event event;
        while (running) {
            while (SDL_PollEvent(&event)) {
                if (event.type == SDL_KEYUP) {
                    if (event.key.keysym.sym == SDLK_SPACE)
                        running = false;
                }
            }
        }*/
        
        SDL_GL_DeleteContext(context);
    }
    
    SDL_DestroyWindow(window);
    SDL_Quit();
    
    return EXIT_SUCCESS;
}
