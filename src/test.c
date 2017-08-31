#define GL_GLEXT_PROTOTYPES
#include "shared/glcorearb.h"
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

int main(int argc, char **argv) {
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
    
    //Texture
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
    
    //Framebuffer texture
    GLuint fb_texture;
    glGenTextures(1, &fb_texture);
    glBindTexture(GL_TEXTURE_2D, fb_texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 640, 640, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    
    //Sampler
    GLuint sampler;
    glGenSamplers(1, &sampler);
    glSamplerParameteri(sampler, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glSamplerParameteri(sampler, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    
    //Depth stencil renderbuffer
    GLuint rb;
    glGenRenderbuffers(1, &rb);
    glBindRenderbuffer(GL_RENDERBUFFER, rb);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_STENCIL, 640, 640);
    
    //Tex coord buffer
    GLuint tex_coord_buffer;
    glGenBuffers(1, &tex_coord_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, tex_coord_buffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(tex_coords), NULL, GL_STATIC_DRAW);
    void* data = glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY);
    memcpy(data, tex_coords, sizeof(tex_coords)/2);
    glUnmapBuffer(GL_ARRAY_BUFFER);
    data = glMapBufferRange(GL_ARRAY_BUFFER, 16, 16, GL_MAP_WRITE_BIT);
    memcpy(data, tex_coords+4, sizeof(tex_coords)/2);
    glUnmapBuffer(GL_ARRAY_BUFFER);
    
    //Position buffer
    GLuint pos_buffer;
    glGenBuffers(1, &pos_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, pos_buffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(positions), positions, GL_STATIC_DRAW);
    
    //Element buffer
    GLuint element_buffer;
    glGenBuffers(1, &element_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, element_buffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(elements), NULL, GL_STATIC_DRAW);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(elements), elements);
    
    //Vertex
    GLuint vertex = glCreateShader(GL_VERTEX_SHADER);
    static const char* vert_source = "#version 130\n"
                                     "in vec2 texCoord;\n"
                                     "in vec3 position;\n"
                                     "in mat4 mvp;\n"
                                     "out vec2 frag_texCoord;\n"
                                     "void main() {\n"
                                     "    gl_Position = vec4(position, 1.0);\n"
                                     "    frag_texCoord = texCoord;\n"
                                     "}\n";
    glShaderSource(vertex, 1, &vert_source, NULL);
    glCompileShader(vertex);
    
    //Fragment
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
    
    //Program
    GLuint program = glCreateProgram();
    glAttachShader(program, vertex);
    glAttachShader(program, fragment);
    glLinkProgram(program);
    glValidateProgram(program);
    glDetachShader(program, vertex);
    glDetachShader(program, fragment);
    
    //Display vertex
    GLuint display_vert = glCreateShader(GL_VERTEX_SHADER);
    static const char* dpy_vert_source = "#version 130\n"
                                         "out vec2 frag_texCoord;\n"
                                         "void main() {\n"
                                         "    gl_Position = vec4(vec2[](vec2(-1.0, -1.0), vec2(1.0, -1.0), vec2(1.0, 1.0), vec2(-1.0, -1.0), vec2(-1.0, 1.0), vec2(1.0, 1.0))[gl_VertexID], 0.0, 1.0);\n"
                                         "    frag_texCoord = gl_Position.xy*0.5 + 0.5;\n"
                                         "}\n";
    glShaderSource(display_vert, 1, &dpy_vert_source, NULL);
    glCompileShader(display_vert);
    
    //Display fragment
    GLuint display_frag = glCreateShader(GL_FRAGMENT_SHADER);
    static const char* dpy_frag_source = "#version 130\n"
                                         "out vec4 out_color;\n"
                                         "in vec2 frag_texCoord;\n"
                                         "uniform sampler2D tex;\n"
                                         "void main() {\n"
                                         "    out_color = texture(tex, frag_texCoord).grba;\n"
                                         "}\n";
    glShaderSource(display_frag, 1, &dpy_frag_source, NULL);
    glCompileShader(display_frag);
    
    //Display program
    GLuint dpy_prog = glCreateProgram();
    glAttachShader(dpy_prog, display_vert);
    glAttachShader(dpy_prog, display_frag);
    glLinkProgram(dpy_prog);
    glValidateProgram(dpy_prog);
    glDetachShader(dpy_prog, display_vert);
    glDetachShader(dpy_prog, display_frag);
    
    //VAO
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
    
    //Framebuffer
    GLuint fb;
    glGenFramebuffers(1, &fb);
    glBindFramebuffer(GL_FRAMEBUFFER, fb);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, fb_texture, 0);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rb);
    
    //Sync
    GLsync sync = glFenceSync(GL_SYNC_GPU_COMMANDS_COMPLETE, 0);
    glWaitSync(sync, 0, GL_TIMEOUT_IGNORED);
    glDeleteSync(sync);
    
    //Query
    GLuint query;
    glGenQueries(1, &query);
    
    bool running = true;
    while (running) {
        SDL_Event event;
        while (SDL_PollEvent(&event))
            if (event.type == SDL_QUIT)
                running = false;
        
        //Render to framebuffer
        glBeginQuery(GL_SAMPLES_PASSED, query);
        glBindFramebuffer(GL_FRAMEBUFFER, fb);
        GLenum bufs[1] = {GL_COLOR_ATTACHMENT0};
        glDrawBuffers(1, bufs);
        static const GLfloat ccolor[] = {0.0f, 0.0f, 0.0f, 1.0f};
        glClearBufferfv(GL_COLOR, 0, ccolor);
        
        glUseProgram(program);
        
        const GLfloat color[] = {1.0f, 0.0f, 0.0f};
        glUniform3fv(glGetUniformLocation(program, "color"), 1, color);
        
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture);
        glUniform1i(glGetUniformLocation(program, "tex"), 0);
        
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, element_buffer);
        glBindVertexArray(vao);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, (const GLvoid*)0);
        
        //Render to window
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glClear(GL_COLOR_BUFFER_BIT);
        
        glUseProgram(dpy_prog);
        
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, fb_texture);
        glBindSampler(0, sampler);
        glUniform1i(glGetUniformLocation(dpy_prog, "tex"), 0);
        
        glDrawArrays(GL_TRIANGLES, 0, 6);
        glBindSampler(0, 0);
        glEndQuery(GL_SAMPLES_PASSED);
        
        SDL_GL_SwapWindow(window);
    }
    
    glDeleteQueries(1, &query);
    glDeleteRenderbuffers(1, &rb);
    glDeleteFramebuffers(1, &fb);
    glDeleteVertexArrays(1, &vao);
    glDeleteProgram(dpy_prog);
    glDeleteShader(display_frag);
    glDeleteShader(display_vert);
    glDeleteProgram(program);
    glDeleteShader(fragment);
    glDeleteShader(vertex);
    glDeleteBuffers(1, &element_buffer);
    glDeleteBuffers(1, &pos_buffer);
    glDeleteBuffers(1, &tex_coord_buffer);
    glDeleteSamplers(1, &sampler);
    glDeleteTextures(1, &fb_texture);
    glDeleteTextures(1, &texture);
    
    SDL_GL_DeleteContext(context);
    SDL_DestroyWindow(window);
    
    SDL_Quit();
    
    return 0;
}
