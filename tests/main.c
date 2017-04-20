#define GL_GLEXT_PROTOTYPES
#include <GL/gl.h>
#include <SDL2/SDL.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>

static char* static_format(const char* format, ...) {
    static char data[1024];
    memset(data, 0, 1024);
    
    va_list list;
    va_start(list, format);
    vsnprintf(data, 1024, format, list);
    va_end(list);
    
    return data;
}

void (*glCurrentTestWIP15)(const GLchar*);

void null_current_test(const GLchar* name) {}

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

int main(int argc, char** argv) {
    SDL_Init(SDL_INIT_VIDEO);
    SDL_Window *window = SDL_CreateWindow("",
                                          SDL_WINDOWPOS_UNDEFINED,
                                          SDL_WINDOWPOS_UNDEFINED,
                                          100,
                                          100,
                                          SDL_WINDOW_OPENGL |
                                          SDL_WINDOW_SHOWN);
    
    void (*tests[])() = {&draw_test, &buffer_test, &texture_test,
                         &uniform_test};
    
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
    for (size_t i = 0; i < sizeof(tests)/sizeof(tests[0]); i++) {
        SDL_GLContext context = SDL_GL_CreateContext(window);
        
        glCurrentTestWIP15 = SDL_GL_GetProcAddress("glCurrentTestWIP15");
        if (!glCurrentTestWIP15)
            glCurrentTestWIP15 = &null_current_test;
        
        tests[i]();
        SDL_GL_SwapWindow(window);
        
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
