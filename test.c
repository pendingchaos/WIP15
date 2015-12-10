#define GL_GLEXT_PROTOTYPES
#include <GL/gl.h>
#include <SDL2/SDL.h>

static const float positions[] = {-1.0f, -1.0f, 0.0f,
                                   1.0f, -1.0f, 0.0f,
                                   1.0f, 1.0f, 0.0f,
                                  -1.0f, 1.0f, 0.0f};

static const float tex_coords[] = {0.0f, 0.0f,
                                   1.0f, 0.0f,
                                   1.0f, 1.0f,
                                   0.0f, 1.0f};

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
    
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_TEXTURE_2D);
    
    GLuint texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    
    size_t w = 128;
    size_t h = 128;
    for (size_t i = 0; i < 8; ++i) {
        void* data = malloc(w*h*4);
        
        for (size_t j = 0; j < w*h; ++j) {
            ((uint32_t*)data)[j] = i==0 ? (j%2 ? 0xFFFFFFFF : 0xFF000000) : 0xFF7f7f7f;
        }
        
        glTexImage2D(GL_TEXTURE_2D, i, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
        w /= 2;
        h /= 2;
        free(data);
    }
    
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    
    GLuint pos_buffer;
    glGenBuffers(1, &pos_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, pos_buffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(positions), NULL, GL_STATIC_DRAW);
    void* pos_data = glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY);
    memcpy(pos_data, positions, sizeof(positions));
    glUnmapBuffer(GL_ARRAY_BUFFER);
    
    GLuint tex_coord_buffer;
    glGenBuffers(1, &tex_coord_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, tex_coord_buffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(tex_coords), tex_coords, GL_STATIC_DRAW);
    
    GLuint vertex = glCreateShader(GL_VERTEX_SHADER);
    static const char* vert_source = "void main() {gl_Position = ftransform();}";
    glShaderSource(vertex, 1, &vert_source, NULL);
    glCompileShader(vertex);
    
    GLuint fragment = glCreateShader(GL_FRAGMENT_SHADER);
    static const char* frag_source = "void main() {gl_FragColor = vec4(1.0, 0.0, 0.0, 1.0);}";
    glShaderSource(fragment, 1, &frag_source, NULL);
    glCompileShader(fragment);
    
    GLuint program = glCreateProgram();
    glAttachShader(program, vertex);
    glAttachShader(program, fragment);
    glLinkProgram(program);
    glValidateProgram(program);
    
    float priority = 0.5f;
    glPrioritizeTextures(1, &texture, &priority);
    GLboolean resident;
    glAreTexturesResident(1, &texture, &resident);
    
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
        
        glUseProgram(program);
        
        glEnableClientState(GL_VERTEX_ARRAY);
        glBindBuffer(GL_ARRAY_BUFFER, pos_buffer);
        glVertexPointer(3, GL_FLOAT, 0, NULL);
        
        glEnableClientState(GL_TEXTURE_COORD_ARRAY);
        glBindBuffer(GL_ARRAY_BUFFER, tex_coord_buffer);
        glTexCoordPointer(2, GL_FLOAT, 0, NULL);
        
        GLint first = 0;
        GLsizei count = 4;
        glMultiDrawArrays(GL_QUADS, &first, &count, 1);
        
        SDL_GL_SwapWindow(window);
    }
    end:;
    
    glDeleteProgram(program);
    glDeleteShader(fragment);
    glDeleteShader(vertex);
    
    glDeleteBuffers(1, &tex_coord_buffer);
    glDeleteBuffers(1, &pos_buffer);
    glDeleteTextures(1, &texture);
    
    SDL_GL_DeleteContext(context);
    SDL_DestroyWindow(window);
    
    SDL_Quit();
    
    return 0;
}
