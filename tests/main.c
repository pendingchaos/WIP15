#define GL_GLEXT_PROTOTYPES
#include <GL/gl.h>
#include <SDL2/SDL.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stddef.h>

#include "client_side_arrays.h"

int main(int argc, char** argv) {
    SDL_Init(SDL_INIT_VIDEO);
    SDL_Window *window = SDL_CreateWindow("",
                                          SDL_WINDOWPOS_UNDEFINED,
                                          SDL_WINDOWPOS_UNDEFINED,
                                          100,
                                          100,
                                          SDL_WINDOW_OPENGL |
                                          SDL_WINDOW_SHOWN);
    
    void (*tests[])() = {&client_side_array_test};
    
    for (size_t i = 0; i < sizeof(tests)/sizeof(tests[0]); i++) {
        SDL_GLContext context = SDL_GL_CreateContext(window);
        tests[i]();
        SDL_GL_DeleteContext(context);
    }
    
    SDL_DestroyWindow(window);
    SDL_Quit();
    
    return EXIT_SUCCESS;
}
