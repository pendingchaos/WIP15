# WIP15
WIP15 is a core OpenGL debugger. It allows you to trace unmodified OpenGL programs and inspect the traces. It is not stable and ready for use.

# Dependencies (excluding what is probably already installed)
- [SDL2](https://libsdl.org) headers and libraries
- [GTK+ 3](http://www.gtk.org) headers and libraries
- [GTKSourceView for Gtk+ 3](https://wiki.gnome.org/Projects/GtkSourceView/) headers and libraries

[Zlib](http://zlib.net) and [LZ4](http://www.lz4.org) development packages are optional but having at least one is recommended.

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
