#include "libinspect/libinspect.h"
#include "libtrace/libtrace.h"

#include <stdlib.h>
#include <stdio.h>

int main(int argc, char** argv) {
    if (argc != 2) {
        fprintf(stderr, "Expected one argument. Got %d.\n", argc);
        fprintf(stderr, "Usage: inspect-gui <trace file>\n");
        return EXIT_FAILURE;
    }
    
    trace_t* trace = load_trace(argv[1]);
    trace_error_t error = get_trace_error();
    if (error == TraceError_Invalid) {
        fprintf(stderr, "Invalid trace file: %s\n", get_trace_error_desc());
        return EXIT_FAILURE;
    } else if (error == TraceError_UnableToOpen) {
        fprintf(stderr, "Unable to open trace file.\n");
        return EXIT_FAILURE;
    }
    inspection_t* inspection = create_inspection(trace);
    
    inspect(inspection);
    
    free_inspection(inspection);
    free_trace(trace);
    
    return EXIT_SUCCESS;
}
