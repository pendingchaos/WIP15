#include <sys/wait.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stddef.h>
#include <unistd.h>
#include <getopt.h>

static char* limits = NULL;
static char* output = NULL;
static char* compress = NULL;

static struct option options[] = {
    {"limits", required_argument, NULL, 'l'},
    {"output", required_argument, NULL, 'o'},
    {"compress", required_argument, NULL, 'c'}
};

static void handle_options(int argc, char** argv) {
    int option_index = 0;
    int c = -1;
    while ((c=getopt_long(argc, argv, "l:o:c", options, &option_index)) != -1) {
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
}

static void run(int cmdc, char** cmd) {
    char* lib_path = realpath("libgl.so", NULL);
    if (lib_path == NULL) {
        fprintf(stderr, "Unable to find libgl.so.\n");
        exit(EXIT_FAILURE);
    }
    
    fclose(fopen(output, "wb"));
    char* output_path = realpath(output, NULL);
    if (output_path == NULL) {
        fprintf(stderr, "Unable to get absolute path for output.\n");
        exit(EXIT_FAILURE);
    }
    
    pid_t pid;
    if (!(pid=fork())) {
        setenv("WIP15_LIMITS", limits, 1);
        setenv("WIP15_OUTPUT", output_path, 1);
        setenv("WIP15_COMPRESSION_LEVEL", compress, 1);
        setenv("SDL_OPEN_LIBRARY", lib_path, 1);
        setenv("LD_PRELOAD", lib_path, 1);
        
        execv(cmd[0], cmd);
    } else {
        int wstatus;
        waitpid(pid, &wstatus, 0);
    }
    
    free(output_path);
    free(lib_path);
}

int main(int argc, char** argv) {
    if (argc < 2) {
        fprintf(stderr, "Usage: trace <arguments> <command>\n");
        fprintf(stderr, "Arguments\n");
        fprintf(stderr, "    --output=<output>  or -o <output> | Defaults to output.trace\n");
        fprintf(stderr, "    --limits=<limits>  or -l <limits> | Defaults to limits/this.limits.txt\n");
        fprintf(stderr, "    --compress=<0-100> or -c <0-100 > | Defaults to 60\n");
        return EXIT_FAILURE;
    }
    
    handle_options(argc, argv);
    run(argc-optind, argv+optind);
    
    free(limits);
    free(compress);
    free(output);
    
    return EXIT_SUCCESS;
}
