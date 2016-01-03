# WIP15
WIP15 is a core OpenGL debugger. It allows you to trace unmodified OpenGL programs and inspect the traces. It is not stable and ready for use.

# Dependencies
It requires GCC with C++ support, SDL2, libdl, a C standard library implementation and an OpenGL implementation support core OpenGL.
The GUI also requires GTK+ 3

# Compiling
```shell
make all
```

You may need to run scripts/generate_replay.py to update src/libinspect/replay_gl.c.

# Usage
## Tracing
Currently it can only trace GLX programs.
```shell
./trace <limits-file> <output-trace> <program> <arguments>
```
For example:
```shell
./trace limits/this.limits output.trace ./test
```
Limits are not fully implemented and ignored so it does not matter which file you specify.

## Inspection
```shell
./inspect-gui <trace-file>
```
