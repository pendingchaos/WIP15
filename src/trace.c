#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stddef.h>
#include <unistd.h>

int main(int argc, char** argv) {
    if (argc < 3) {
        fprintf(stderr, "Usage: trace <output> <command> <arguments>\n");
        return EXIT_FAILURE;
    }
    
    char* lib_path = realpath("gl.so", NULL);
    if (lib_path == NULL) {
        fprintf(stderr, "Unable to find gl.so.\n");
        return EXIT_FAILURE;
    }
    
    fclose(fopen(argv[1], "wb"));
    char* output_path = realpath(argv[1], NULL);
    if (output_path == NULL) {
        fprintf(stderr, "Unable to get absolute path for output.\n");
        return EXIT_FAILURE;
    }
    
    char command[1024];
    memset(command, 0, 1024);
    snprintf(command,
             1024,
             "WIP15_OUTPUT=\"%s\" SDL_OPENGL_LIBRARY=\"%s\" LD_PRELOAD=\"%s\" ",
             output_path,
             lib_path,
             lib_path);
    size_t command_length = strlen(command);
    
    memcpy(command+command_length, argv[2], strlen(argv[2]));
    command_length += strlen(argv[2]);
    
    int i;
    for (i = 3; i < argc; ++i) {
        command[command_length++] = ' ';
        memcpy(command+command_length, argv[i], strlen(argv[i]));
        command_length += strlen(argv[i]);
    }
    
    if (system(command) != 0) {
        fprintf(stderr, "Unable to execute command.\n");
        free(output_path);
        free(lib_path);
        return EXIT_FAILURE;
    }
    
    free(output_path);
    free(lib_path);
    
    return EXIT_SUCCESS;
}
