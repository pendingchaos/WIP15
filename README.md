# WIP15
WIP15 is a core OpenGL debugger. It allows you to trace unmodified OpenGL programs and to replay and inspect the traces. It is not stable and it is not ready for use.

# Dependencies (excluding what is probably already installed)
- [SDL2](https://libsdl.org) headers and libraries
- [GTK+ 3](http://www.gtk.org) headers and libraries
- Optionally [GTKSourceView for Gtk+ 3](https://wiki.gnome.org/Projects/GtkSourceView/) headers and libraries

[Zlib](http://zlib.net) and [LZ4](http://www.lz4.org) development packages are optional but having at least one is recommended.

# Compiling
```shell
make -j$(getconf _NPROCESSORS_ONLN)
```

# Usage
## Tracing
Currently it can only trace GLX programs.
```shell
./trace -o <output-trace> <program> <arguments>
```
For example:
```shell
./trace -o output.trace ./test
```
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
