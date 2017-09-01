# WIP15
WIP15 is a core OpenGL debugger. It allows you to trace unmodified OpenGL programs and inspect the traces. It is not stable and ready for use.

# Dependencies
It requires [GCC](https://gcc.gnu.org), [SDL2](https://libsdl.org), libdl, [zlib](http://zlib.net), pkg-config, a C standard library implementation and an OpenGL implementation support core OpenGL.
The GUI also requires [GTK+ 3](http://www.gtk.org).

# Compiling
```shell
make -j$(getconf _NPROCESSORS_ONLN)
```

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
Traces can also be created with the GUI.

## Inspection
```shell
./gui <trace-file>
```
or
```shell
./gui
```
and open it in the GUI.
