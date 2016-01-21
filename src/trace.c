#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stddef.h>
#include <unistd.h>
#include <getopt.h>

static struct option options[] = {
    {"limits", required_argument, NULL, 'l'},
    {"output", required_argument, NULL, 'o'},
    {"compress", required_argument, NULL, 'c'}
};

int main(int argc, char** argv) {
    if (argc < 2) {
        fprintf(stderr, "Usage: trace <arguments> <command>\n");
        fprintf(stderr, "Arguments\n");
        fprintf(stderr, "    --output=<output>  or -o <output> | Defaults to output.trace\n");
        fprintf(stderr, "    --limits=<limits>  or -l <limits> | Defaults to limits/this.limits.txt\n");
        fprintf(stderr, "    --compress=<0-100> or -c <0-100 > | Defaults to 60\n");
        return EXIT_FAILURE;
    }
    
    char* lib_path = realpath("gl.so", NULL);
    if (lib_path == NULL) {
        fprintf(stderr, "Unable to find gl.so.\n");
        return EXIT_FAILURE;
    }
    
    char* limits = NULL;
    char* output = NULL;
    char* compress = NULL;
    
    int option_index = 0;
    int c = -1;
    while ((c=getopt_long(argc, argv, "l:o:c:", options, &option_index)) != -1) {
        switch (c) {
        case 'l':
            free(limits);
            limits = malloc(strlen(optarg)+1);
            strcpy(limits, optarg);
            break;
        case 'o':
            free(output);
            output = malloc(strlen(optarg)+1);
            strcpy(output, optarg);
            break;
        case 'c':
            free(compress);
            compress = malloc(strlen(optarg)+1);
            strcpy(compress, optarg);
            break;
        }
    }
    
    if (!limits) {
        limits = malloc(strlen("limits/this.limits.txt")+1);
        strcpy(limits, "limits/this.limits.txt");
    }
    
    if (!output) {
        output = malloc(strlen("output.trace")+1);
        strcpy(output, "output.trace");
    }
    
    if (!compress) {
        compress = malloc(strlen("60")+1);
        strcpy(compress, "60");
    }
    
    fclose(fopen(output, "wb"));
    char* output_path = realpath(output, NULL);
    if (output_path == NULL) {
        fprintf(stderr, "Unable to get absolute path for output.\n");
        return EXIT_FAILURE;
    }
    free(output);
    
    char command[16384];
    memset(command, 0, sizeof(command));
    snprintf(command,
             sizeof(command),
             "WIP15_LIMITS=\"%s\" WIP15_OUTPUT=\"%s\" WIP15_COMPRESSION_LEVEL=\"%s\" SDL_OPENGL_LIBRARY=\"%s\" LD_PRELOAD=\"%s\" ",
             limits,
             output_path,
             compress,
             lib_path,
             lib_path);
    free(limits);
    free(compress);
    
    for (int i = optind; i < argc; ++i) {
        strncat(command, " ", sizeof(command));
        strncat(command, argv[i], sizeof(command));
    }
    
    printf("%s\n", command);
    
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
