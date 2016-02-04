#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stddef.h>
#include <unistd.h>
#include <getopt.h>

static char* limits = NULL;
static char* output = NULL;
static char* compress = NULL;
static int print_cmd = 0;

static struct option options[] = {
    {"limits", required_argument, NULL, 'l'},
    {"output", required_argument, NULL, 'o'},
    {"compress", required_argument, NULL, 'c'},
    {"printcmd", no_argument, &print_cmd, 1}
};

static void handle_options(int argc, char** argv) {
    int option_index = 0;
    int c = -1;
    while ((c=getopt_long(argc, argv, "l:o:c:p", options, &option_index)) != -1) {
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
        case 'p':
            print_cmd = 1;
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
    
    for (int i = 0; i < cmdc; i++) {
        strncat(command, " ", sizeof(command));
        strncat(command, cmd[i], sizeof(command));
    }
    
    if (print_cmd)
        printf("%s\n", command);
    
    if (system(command) != 0) {
        fprintf(stderr, "Unable to execute command.\n");
        free(output_path);
        free(lib_path);
        exit(EXIT_FAILURE);
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
        fprintf(stderr, "    --printcmd         or -p          | Print the command\n");
        return EXIT_FAILURE;
    }
    
    handle_options(argc, argv);
    run(argc-optind, argv+optind);
    
    free(limits);
    free(compress);
    free(output);
    
    return EXIT_SUCCESS;
}
