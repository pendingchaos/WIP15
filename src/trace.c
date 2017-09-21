#include "libtrace/libtrace.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stddef.h>
#include <unistd.h>
#include <getopt.h>

static char* config = NULL;
static char* output = NULL;
static char* compress = NULL;

typedef enum long_only_option_t {
    LongOnlyOption_Config = 256
} long_only_option_t;

static struct option options[] = {
    {"config", optional_argument, NULL, LongOnlyOption_Config},
    {"output", required_argument, NULL, 'o'},
    {"compress", required_argument, NULL, 'c'}
};

//TODO: This also tries to parse options after the command
static void handle_options(int argc, char** argv) {
    int option_index = 0;
    int c = -1;
    while ((c=getopt_long(argc, argv, "o:c", options, &option_index)) != -1) {
        switch (c) {
        case LongOnlyOption_Config:
            free(config);
            config = malloc(strlen(optarg)+1);
            strcpy(config, optarg);
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
    
    if (!config) {
        config = malloc(strlen("configs/this.config.txt")+1);
        strcpy(config, "configs/this.config.txt");
    }
    
    if (!output) {
        output = malloc(strlen("output.trace")+1);
        strcpy(output, "output.trace");
    }
    
    if (!compress) {
        compress = malloc(strlen("60")+1);
        strcpy(compress, "60");
    }
    
    char* end;
    strtol(compress, &end, 10);
    if (end == compress) {
        fprintf(stderr, "Invalid compression level.\n");
        exit(EXIT_FAILURE);
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
    
    int exitcode;
    bool success = trace_program(&exitcode, 5,
        TrcProgramArguments, cmd,
        TrcOutputFilename, output_path,
        TrcConfigFilename, config,
        TrcCompression, atoi(compress),
        TrcLibGL, lib_path);
    if (success && exitcode!=EXIT_SUCCESS) {
        fprintf(stderr, "Process returned %d\n", exitcode);
    } else if (!success) {
        fprintf(stderr, "Failed to start process\n");
    }
    
    free(output_path);
    free(lib_path);
}

int main(int argc, char** argv) {
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <arguments> <command>\n", argv[0]);
        fprintf(stderr, "Arguments\n");
        fprintf(stderr, "    --output=<output>  or -o <output> | Defaults to output.trace\n");
        fprintf(stderr, "    --limits=<limits>  or -l <limits> | Defaults to limits/this.limits.txt\n");
        fprintf(stderr, "    --compress=<0-100> or -c <0-100 > | Defaults to 60\n");
        return EXIT_FAILURE;
    }
    
    handle_options(argc, argv);
    run(argc-optind, argv+optind);
    
    free(config);
    free(compress);
    free(output);
    
    return EXIT_SUCCESS;
}
