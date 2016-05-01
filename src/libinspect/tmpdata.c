#include "libinspect/tmpdata.h"

#include <stdlib.h>
#include <unistd.h>
#include <inttypes.h>

// http://pubs.opengroup.org/onlinepubs/9699919799/functions/unlink.html
// > If one or more processes have the file open when the last link is
// > removed, the link shall be removed before unlink() returns, but the
// > removal of the file contents shall be postponed until all references
// > to the file are closed.

static size_t counter = 0;

tmpdata_t tmpdata_new(size_t size, const void* data) {
    char fname[256];
    snprintf(fname, sizeof(fname), ".wip15_tmpdata_%"PRId64"_%zu", (int64_t)getpid(), counter++);
    
    tmpdata_t res;
    res.size = size;
    
    res.file = fopen(fname, "wb");
    if (!res.file) {
        fprintf(stderr, "Failed to create/open %s\n", fname);
        exit(EXIT_FAILURE);
    }
    fwrite(data, size, 1, res.file);
    fclose(res.file);
    
    res.filename = realpath(fname, NULL);
    
    res.file = fopen(res.filename, "rb");
    if (!res.file) {
        fprintf(stderr, "Failed to open %s\n", res.filename);
        exit(EXIT_FAILURE);
    }
    unlink(res.filename);
    
    return res;
}

void tmpdata_del(tmpdata_t tmpdata) {
    fclose(tmpdata.file);
    free(tmpdata.filename);
}

void tmpdata_read(tmpdata_t tmpdata, void* data) {
    fread(data, tmpdata.size, 1, tmpdata.file);
}
