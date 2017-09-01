#include "gui.h"

#include <stdlib.h>

int main(int argc, char** argv) {
    if (argc > 2) {
        fprintf(stderr, "Expected one or zero arguments. Got %d.\n", argc-1);
        fprintf(stderr, "Usage: inspect-gui <trace file (optional)>\n");
        return EXIT_FAILURE;
    }
    
    if (argc == 2)
        return run_gui(argv[1], argc, argv);
    else
        return run_gui(NULL, argc, argv);
}
