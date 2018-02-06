#include "libtrace/libtrace.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stddef.h>
#include <unistd.h>
#include <getopt.h>

static char* config = NULL;
static char* output = NULL;
static char* cwd = NULL;
static char* compress = NULL;
static bool cwdexec = false;

typedef enum long_only_option_t {
    LongOnlyOption_Config = 256
} long_only_option_t;

static struct option options[] = {
    {"config", required_argument, NULL, LongOnlyOption_Config},
    {"output", required_argument, NULL, 'o'},
    {"cwd", required_argument, NULL, 'd'},
    {"compress", required_argument, NULL, 'c'},
    {"cwdexec", no_argument, NULL, 'e'},
    {"help", no_argument, NULL, 'h'}
};

static void print_usage(char** argv) {
    fprintf(stderr, "Usage: %s <arguments> <command>\n", argv[0]);
    fprintf(stderr, "Arguments\n");
    fprintf(stderr, "    --output=<output>  or -o <output> | Defaults to output.trace\n");
    fprintf(stderr, "    --config=<config>                 | Defaults to configs/this.config.txt\n");
    fprintf(stderr, "    --compress=<0-100> or -c <0-100>  | Defaults to 60\n");
    fprintf(stderr, "    --cwd=<dir>  or -d <dir>          | Defaults to the current working directory\n");
    fprintf(stderr, "    --cwdexec  or -e                  | Sets the current working directory to that of the traced executable\n");
    fprintf(stderr, "    --help  or -h                     | Prints this message and exits\n");
}

//TODO: This also tries to parse options after the command
static void handle_options(char* self_dir, int argc, char** argv) {
    int option_index = 0;
    int c = -1;
    while ((c=getopt_long(argc, argv, "o:c:d:eh", options, &option_index)) != -1) {
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
        case 'd':
            free(cwd);
            cwd = malloc(strlen(optarg)+1);
            strcpy(cwd, optarg);
            break;
        case 'e':
            cwdexec = true;
            break;
        case 'h':
            print_usage(argv);
            exit(EXIT_SUCCESS);
            break;
        case '?':
            print_usage(argv);
            exit(EXIT_FAILURE);
            break;
        }
    }
    
    if (!config) {
        const char* this_cfg = "/configs/this.config.txt";
        config = calloc(strlen(self_dir)+strlen(this_cfg)+1, 1);
        strcpy(config, self_dir);
        strcat(config, this_cfg);
    }
    
    if (!output) {
        output = malloc(strlen("output.trace")+1);
        strcpy(output, "output.trace");
    }
    
    if (!compress) {
        compress = malloc(strlen("60")+1);
        strcpy(compress, "60");
    }
    
    if (!cwd) cwd = get_current_dir_name();
    
    char* end;
    strtol(compress, &end, 10);
    if (end == compress) {
        fprintf(stderr, "Invalid compression level.\n");
        exit(EXIT_FAILURE);
    }
}

static void run(char* self_dir, int cmdc, char** cmd) {
    const char* lib_gl = "/libgl.so";
    char* lib_path = calloc(strlen(self_dir)+strlen(lib_gl)+1, 1);
    strcpy(lib_path, self_dir);
    strcat(lib_path, lib_gl);
    
    fclose(fopen(output, "wb"));
    char* output_path = realpath(output, NULL);
    if (output_path == NULL) {
        fprintf(stderr, "Unable to get absolute path for output.\n");
        exit(EXIT_FAILURE);
    }
    
    int exitcode;
    bool success = trace_program(&exitcode, 6,
        TrcProgramArguments, cmd,
        TrcOutputFilename, output_path,
        TrcConfigFilename, config,
        TrcCompression, atoi(compress),
        TrcLibGL, lib_path,
        TrcCurrentWorkingDirectory, cwdexec?NULL:cwd);
    if (success && exitcode!=EXIT_SUCCESS) {
        fprintf(stderr, "Process returned %d\n", exitcode);
    } else if (!success) {
        fprintf(stderr, "Failed to start process\n");
    }
    
    free(cwd);
    free(output_path);
    free(lib_path);
}

int main(int argc, char** argv) {
    if (argc < 2) {
        print_usage(argv);
        return EXIT_FAILURE;
    }
    
    char* self_dir = realpath(argv[0], NULL);
    for (int i = strlen(self_dir)-1; i>=0; i--) {
        if (self_dir[i] == '/') {
            self_dir[i] = 0;
            break;
        }
    }
    
    handle_options(self_dir, argc, argv);
    run(self_dir, argc-optind, argv+optind);
    
    free(self_dir);
    free(config);
    free(compress);
    free(output);
    
    return EXIT_SUCCESS;
}
