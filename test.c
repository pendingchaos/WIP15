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
    
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_TEXTURE_2D);
    
    GLuint texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    static const uint8_t data[] = {255, 255, 255, 255,  0, 0, 0, 255,
                                   0, 0, 0, 255,        255, 255, 255, 255};
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 2, 2, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    
    while (1) {
        SDL_Event event;
        while (SDL_PollEvent(&event))
            if (event.type == SDL_QUIT)
                goto end;
        
        glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
        
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        glFrustum(-1.0f, 1.0f, -1.0f, 1.0f, 1.0f, 10.0f);
        
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();
        glTranslatef(0.0f, 0.0f, -3.0f);
        glRotatef(45.0f, 1.0f, 0.0f, 0.0f);
        glRotatef(45.0f, 0.0f, 0.0f, 1.0f);
        
        glBegin(GL_QUADS);
        glTexCoord2f(0.0f, 0.0f);
        glVertex3f(-1.0f, -1.0f, 0.0f);
        glTexCoord2f(1.0f, 0.0f);
        glVertex3f(1.0f, -1.0f, 0.0f);
        glTexCoord2f(1.0f, 1.0f);
        glVertex3f(1.0f, 1.0f, 0.0f);
        glTexCoord2f(0.0f, 1.0f);
        glVertex3f(-1.0f, 1.0f, 0.0f);
        glEnd();
        
        SDL_GL_SwapWindow(window);
    }
    end:;
    
    glDeleteTextures(1, &texture);
    
    SDL_GL_UnloadLibrary();
    SDL_GL_DeleteContext(context);
    SDL_DestroyWindow(window);
    
    SDL_Quit();
    
    return 0;
}
