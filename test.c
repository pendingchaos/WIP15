#define GL_GLEXT_PROTOTYPES
#include <GL/gl.h>
#include <SDL2/SDL.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

static const float positions[] = {-1.0f, -1.0f, 0.0f,
                                   1.0f, -1.0f, 0.0f,
                                   1.0f, 1.0f, 0.0f,
                                  -1.0f, 1.0f, 0.0f};

static const float tex_coords[] = {0.0f, 0.0f,
                                   1.0f, 0.0f,
                                   1.0f, 1.0f,
                                   0.0f, 1.0f};

static const uint32_t elements[] = {0, 1, 2, 0, 3, 2};

void callback(GLenum _1, GLenum _2, GLuint id, GLenum _4, GLsizei _5, const GLchar* message, void* _6) {
    if (id == 131185) //Ignore buffer detailed info with proprietary Nvidia drivers
        return;
    if (id == 131218) //Ignore program/shader state performance warnings with proprietary Nvidia drivers
        return;
    
    printf("%s\n", message);
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
    
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_DEBUG_FLAG);
    
    SDL_GLContext context = SDL_GL_CreateContext(window);
    
    glDebugMessageCallback((GLDEBUGPROC)&callback, NULL);
    
    GLuint texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    
    size_t w = 128;
    size_t h = 128;
    for (size_t i = 0; i < 8; ++i) {
        void* data = malloc(w*h*4);
        
        for (size_t j = 0; j < w*h; ++j)
            ((uint32_t*)data)[j] = i==0 ? (j%2 ? 0xFFFFFFFF : 0xFF000000) : 0xFF7f7f7f;
        
        glTexImage2D(GL_TEXTURE_2D, i, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
        w /= 2;
        h /= 2;
        free(data);
    }
    
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    
    GLuint tex_coord_buffer;
    glGenBuffers(1, &tex_coord_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, tex_coord_buffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(tex_coords), NULL, GL_STATIC_DRAW);
    void* data = glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY);
    memcpy(data, tex_coords, sizeof(tex_coords));
    glUnmapBuffer(GL_ARRAY_BUFFER);
    
    GLuint pos_buffer;
    glGenBuffers(1, &pos_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, pos_buffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(positions), positions, GL_STATIC_DRAW);
    
    GLuint element_buffer;
    glGenBuffers(1, &element_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, element_buffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(elements), NULL, GL_STATIC_DRAW);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(elements), elements);
    
    GLuint vertex = glCreateShader(GL_VERTEX_SHADER);
    static const char* vert_source = "#version 130\n"
                                     "in vec2 texCoord;\n"
                                     "in vec3 position;\n"
                                     "in mat4 mvp;\n"
                                     "out vec2 frag_texCoord;\n"
                                     "void main() {\n"
                                     "    gl_Position = vec4(position, 1.0);"
                                     "    frag_texCoord = texCoord;\n"
                                     "}\n";
    glShaderSource(vertex, 1, &vert_source, NULL);
    glCompileShader(vertex);
    
    GLuint fragment = glCreateShader(GL_FRAGMENT_SHADER);
    static const char* frag_source = "#version 130\n"
                                     "in vec2 frag_texCoord;\n"
                                     "out vec4 out_color;\n"
                                     "uniform vec3 color;\n"
                                     "uniform sampler2D tex;\n"
                                     "void main() {\n"
                                     "    out_color = texture(tex, frag_texCoord) * vec4(color, 1.0);\n"
                                     "}\n";
    glShaderSource(fragment, 1, &frag_source, NULL);
    glCompileShader(fragment);
    
    GLuint program = glCreateProgram();
    glAttachShader(program, vertex);
    glAttachShader(program, fragment);
    glLinkProgram(program);
    glValidateProgram(program);
    
    GLuint vao;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, tex_coord_buffer);
    glEnableVertexAttribArray(glGetAttribLocation(program, "texCoord"));
    glVertexAttribPointer(glGetAttribLocation(program, "texCoord"),
                          2,
                          GL_FLOAT,
                          GL_FALSE,
                          0,
                          (const GLvoid*)0);
    
    glBindBuffer(GL_ARRAY_BUFFER, pos_buffer);
    glEnableVertexAttribArray(glGetAttribLocation(program, "position"));
    glVertexAttribPointer(glGetAttribLocation(program, "position"),
                          3,
                          GL_FLOAT,
                          GL_FALSE,
                          0,
                          (const GLvoid*)0);
    
    bool running = true;
    while (running) {
        SDL_Event event;
        while (SDL_PollEvent(&event))
            if (event.type == SDL_QUIT)
                running = false;
        
        glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
        
        glUseProgram(program);
        
        const GLfloat color[] = {1.0f, 0.0f, 0.0f};
        glUniform3fv(glGetUniformLocation(program, "color"), 1, color);
        
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture);
        glUniform1i(glGetUniformLocation(program, "tex"), 0);
        
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, element_buffer);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, (const GLvoid*)0);
        
        SDL_GL_SwapWindow(window);
    }
    
    glDeleteVertexArrays(1, &vao);
    glDeleteProgram(program);
    glDeleteShader(fragment);
    glDeleteShader(vertex);
    glDeleteBuffers(1, &element_buffer);
    glDeleteBuffers(1, &pos_buffer);
    glDeleteBuffers(1, &tex_coord_buffer);
    glDeleteTextures(1, &texture);
    
    SDL_GL_DeleteContext(context);
    SDL_DestroyWindow(window);
    
    SDL_Quit();
    
    return 0;
}
