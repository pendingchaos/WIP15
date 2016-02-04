CFLAGS = -Wall -std=c99 `pkg-config zlib --cflags` `sdl2-config --cflags` `pkg-config gtk+-3.0 --cflags` -D_DEFAULT_SOURCE -D_GNU_SOURCE -Isrc -fPIC

gui_src = $(wildcard src/gui/*.c)
libtrace_src = $(wildcard src/libtrace/*.c)
libinspect_src = $(wildcard src/libinspect/*.c)
src = $(wildcard src/*.c) $(gui_src) $(libinspect_src) $(libtrace_src) $(wildcard src/shared/*.c) src/libgl.c

base_gui_obj = $(gui_src:.c=.o)
gui_obj = $(join $(dir $(base_gui_obj)), $(addprefix ., $(notdir $(base_gui_obj))))

base_libtrace_obj = $(libtrace_src:.c=.o)
libtrace_obj = $(join $(dir $(base_libtrace_obj)), $(addprefix ., $(notdir $(base_libtrace_obj))))

base_libinspect_obj = $(libinspect_src:.c=.o)
libinspect_obj = $(join $(dir $(base_libinspect_obj)), $(addprefix ., $(notdir $(base_libinspect_obj))))

dep = $(obj:.o=.d)

all: bin/libtrace.so bin/libgl.so bin/libinspect.so bin/trace bin/inspect-gui bin/leakcheck bin/testtrace bin/test bin/tests

-include $(dep)

.%.d: %.c
	@$(CPP) $(CFLAGS) $< -MM -MT $(@:.d=.o) >$@

.%.o: %.c
	$(CC) -c $(CFLAGS) $< -o $@

src/libgl.c: scripts/nontrivial_func_trace_impls.txt scripts/gl.c
	cd scripts; python generate_gl.py

bin/libgl.so: src/.libgl.o
	$(CC) $^ -o bin/libgl.so -shared -fPIC -ldl -g `pkg-config zlib --libs` $(CFLAGS)

bin/libtrace.so: $(libtrace_obj) src/shared/.vec.o
	$(CC) $^ -o bin/libtrace.so -shared -fPIC -g `pkg-config zlib --libs` $(CFLAGS)

bin/libinspect.so: $(libinspect_obj) src/shared/.vec.o src/shared/.glapi.o
	$(CC) $^ -o bin/libinspect.so -shared -fPIC -lGL -ldl -g `sdl2-config --libs` $(CFLAGS)

bin/inspect-gui: $(gui_obj) src/shared/.vec.o src/shared/.glapi.o bin/libtrace.so bin/libinspect.so
	$(CC) /home/rugrats/Documents/C/WIP15/bin/libtrace.so /home/rugrats/Documents/C/WIP15/bin/libinspect.so $^ -o bin/inspect-gui -g `pkg-config gtk+-3.0 --libs` -rdynamic $(CFLAGS)

bin/leakcheck: src/.leakcheck.o bin/libtrace.so bin/libinspect.so
	$(CC) /home/rugrats/Documents/C/WIP15/bin/libtrace.so /home/rugrats/Documents/C/WIP15/bin/libinspect.so $^ -o bin/leakcheck -g -rdynamic $(CFLAGS)

bin/testtrace: src/.testtrace.o bin/libtrace.so bin/libinspect.so
	$(CC) /home/rugrats/Documents/C/WIP15/bin/libtrace.so /home/rugrats/Documents/C/WIP15/bin/libinspect.so $^ -o bin/testtrace -g -rdynamic $(CFLAGS)

bin/trace: src/.trace.o
	$(CC) $^ -o bin/trace -g $(CFLAGS)

bin/test: src/.test.o
	$(CC) $^ -o bin/test -g $(CFLAGS) -lSDL2 -lGL

bin/tests: tests/.main.o
	$(CC) $^ `sdl2-config --libs` -lGL -o bin/tests -g $(CFLAGS)

.PHONY: clean
clean:
	rm -f src/.libgl.c
	rm -f bin/libtrace.so
	rm -f bin/libinspect.so
	rm -f bin/inspect-gui
	rm -f bin/trace
	rm -f bin/test
	rm -f bin/gl.so
	rm -f bin/tests
	rm -f bin/testtrace
	rm -f $(obj)
	rm -f $(dep)
