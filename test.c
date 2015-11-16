#include <GL/gl.h>
#include <SDL2/SDL.h>

void (*getProcAddress(const char *name, void *user_data))() {
    return (void (*)())SDL_GL_GetProcAddress(name);
}

int main(int argc, char **argv)
{
    SDL_Init(SDL_INIT_VIDEO);
    
    SDL_Window *window = SDL_CreateWindow("",
                                          SDL_WINDOWPOS_UNDEFINED,
                                          SDL_WINDOWPOS_UNDEFINED,
                                          640,
                                          640,
                                          SDL_WINDOW_OPENGL |
                                          SDL_WINDOW_SHOWN);
    
    SDL_GLContext context = SDL_GL_CreateContext(window);
    
    SDL_GL_LoadLibrary(NULL);
    
    while (1) {
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT)
                goto end;
        }
        
        glClear(GL_COLOR_BUFFER_BIT);
        glBegin(GL_TRIANGLES);
        glColor3f(1.0f, 0.0f, 0.0f);
        glVertex2f(-1.0f, -1.0f);
        glColor3f(0.0f, 1.0f, 0.0f);
        glVertex2f(1.0f, -1.0f);
        glColor3f(0.0f, 0.0f, 1.0f);
        glVertex2f(0.0f, 1.0f);
        glEnd();
        SDL_GL_SwapWindow(window);
    }
    end:;
    
    SDL_GL_UnloadLibrary();
    SDL_GL_DeleteContext(context);
    SDL_DestroyWindow(window);
    
    SDL_Quit();
    
    return 0;
}
