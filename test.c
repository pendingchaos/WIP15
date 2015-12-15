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
    
    GLuint tex_coord_buffer;
    glGenBuffers(1, &tex_coord_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, tex_coord_buffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(tex_coords), NULL, GL_STATIC_DRAW);
    void* data = glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY);
    memcpy(data, tex_coords, sizeof(tex_coords));
    glUnmapBuffer(GL_ARRAY_BUFFER);
    
    GLuint vertex = glCreateShader(GL_VERTEX_SHADER);
    static const char* vert_source = "#version 120\n"
                                     "attribute vec2 texCoord;\n"
                                     "varying vec2 frag_texCoord;\n"
                                     "void main() {\n"
                                     "    gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;"
                                     "    frag_texCoord = texCoord;\n"
                                     "}\n";
    glShaderSource(vertex, 1, &vert_source, NULL);
    glCompileShader(vertex);
    
    GLuint fragment = glCreateShader(GL_FRAGMENT_SHADER);
    static const char* frag_source = "#version 120\n"
                                     "varying vec2 frag_texCoord;\n"
                                     "uniform vec3 color;\n"
                                     "uniform sampler2D tex;\n"
                                     "void main() {"
                                     "    gl_FragColor = texture2D(tex, frag_texCoord) * vec4(color, 1.0);\n"
                                     "}\n";
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
        
        const GLfloat color[] = {1.0f, 0.0f, 0.0f};
        glUniform3fv(glGetUniformLocation(program, "color"), 1, color);
        
        //glBindBuffer(GL_ARRAY_BUFFER, tex_coord_buffer);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glEnableVertexAttribArray(glGetAttribLocation(program, "texCoord"));
        glVertexAttribPointer(glGetAttribLocation(program, "texCoord"),
                              2,
                              GL_FLOAT,
                              GL_FALSE,
                              0,
                              tex_coords/*(const GLvoid*)0*/);
        
        glEnableClientState(GL_VERTEX_ARRAY);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glVertexPointer(3, GL_FLOAT, 0, positions);
        
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture);
        glUniform1i(glGetUniformLocation(program, "tex"), 0);
        
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
    glDeleteTextures(1, &texture);
    
    SDL_GL_DeleteContext(context);
    SDL_DestroyWindow(window);
    
    SDL_Quit();
    
    return 0;
}
