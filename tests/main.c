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

#include "draw.h"
#include "buffer.h"
#include "texture.h"

int main(int argc, char** argv) {
    SDL_Init(SDL_INIT_VIDEO);
    SDL_Window *window = SDL_CreateWindow("",
                                          SDL_WINDOWPOS_UNDEFINED,
                                          SDL_WINDOWPOS_UNDEFINED,
                                          100,
                                          100,
                                          SDL_WINDOW_OPENGL |
                                          SDL_WINDOW_SHOWN);
    
    void (*tests[])() = {&draw_test, &buffer_test, &texture_test};
    
    for (size_t i = 0; i < sizeof(tests)/sizeof(tests[0]); i++) {
        SDL_GLContext context = SDL_GL_CreateContext(window);
        
        glCurrentTestWIP15 = SDL_GL_GetProcAddress("glCurrentTestWIP15");
        if (!glCurrentTestWIP15)
            glCurrentTestWIP15 = &null_current_test;
        
        tests[i]();
        SDL_GL_SwapWindow(window);
        
        /*bool running = true;
        SDL_Event event;
        while (running)
            while (SDL_PollEvent(&event))
                if (event.type == SDL_KEYDOWN)
                    if (event.key.keysym.sym == SDLK_SPACE)
                        running = false;*/
        
        SDL_GL_DeleteContext(context);
    }
    
    SDL_DestroyWindow(window);
    SDL_Quit();
    
    return EXIT_SUCCESS;
}
