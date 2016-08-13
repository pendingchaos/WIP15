#include "libtrace/libtrace.h"

#include <stdlib.h>
#include <stdio.h>

int main(int argc, char** argv) {
    if (argc != 2) {
        fprintf(stderr, "Expected one argument. Got %d.\n", argc);
        fprintf(stderr, "Usage: %s <trace file>\n", argv[0]);
        return EXIT_FAILURE;
    }
    
    trace_t* trace = load_trace(argv[1]);
    trace_error_t error = trc_get_error();
    if (error == TraceError_Invalid) {
        fprintf(stderr, "Invalid trace file: %s\n", trc_get_error_desc());
        return EXIT_FAILURE;
    } else if (error == TraceError_UnableToOpen) {
        fprintf(stderr, "Unable to open trace file.\n");
        return EXIT_FAILURE;
    }
    
    trc_run_inspection(trace);
    
    free_trace(trace);
    
    return EXIT_SUCCESS;
}
