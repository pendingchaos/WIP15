USE_LZ4 = 1
USE_ZLIB = 1

CFLAGS = -Wall -std=c99 `sdl2-config --cflags` `pkg-config gtk+-3.0 gtksourceview-3.0 --cflags` -D_DEFAULT_SOURCE -D_GNU_SOURCE -Isrc -fPIC -g -fno-strict-aliasing

COMP_LIBS =
ifeq ($(USE_LZ4), 1)
COMP_LIBS += -llz4
CFLAGS += -DLZ4_ENABLED
endif

ifeq ($(USE_ZLIB), 1)
COMP_LIBS += `pkg-config zlib --libs`
CFLAGS += -DZLIB_ENABLED `pkg-config zlib --cflags`
endif

gui_src = $(wildcard src/gui/*.c) $(wildcard src/gui/objects/*.c) $(wildcard src/gui/widgets/*.c) $(wildcard src/gui/tabs/*.c)
libtrace_src = $(wildcard src/libtrace/*.c)
tests_src = $(wildcard src/testing/tests/*.c)
src = $(wildcard src/*.c) $(gui_src) $(libtrace_src) $(tests_src) $(wildcard src/shared/*.c) src/libgl.c

base_gui_obj = $(gui_src:.c=.o)
gui_obj = $(join $(dir $(base_gui_obj)), $(addprefix ., $(notdir $(base_gui_obj))))

base_libtrace_obj = $(libtrace_src:.c=.o)
libtrace_obj = $(join $(dir $(base_libtrace_obj)), $(addprefix ., $(notdir $(base_libtrace_obj))))

base_tests_obj = $(tests_src:.c=.o)
tests_obj = $(join $(dir $(base_tests_obj)), $(addprefix ., $(notdir $(base_tests_obj))))

base_obj = $(src:.c=.o)
obj = $(join $(dir $(base_obj)), $(addprefix ., $(notdir $(base_obj))))

dep = $(obj:.o=.d)

.PHONY: all
all: bin/libtrace.so bin/libgl.so bin/trace bin/gui bin/replaytrace bin/test bin/tests

-include $(dep)

.%.d: %.c src/libtrace/libtrace_glstate.h src/shared/glcorearb.h
	@$(CPP) $(CFLAGS) $< -MM -MT $(@:.d=.o) >$@

.%.o: %.c src/libtrace/libtrace_glstate.h src/shared/glcorearb.h
	$(CC) -c $< $(CFLAGS) -o $@

src/libtrace/.replay_gl.o: src/libtrace/replay_gl.c
	$(CC) -c $< $(CFLAGS) -o $@

scripts/generated_gl_funcs.py: scripts/generate_gl_funcs.py
	cd scripts; python generate_gl_funcs.py

src/libgl.c: scripts/generate_gl_wrapper.py scripts/gl_funcs.py scripts/generated_gl_funcs.py scripts/gl_wrapper.c
	cd scripts; python generate_gl_wrapper.py

src/shared/glapi.c: scripts/generate_glapi.py scripts/generated_gl_funcs.py scripts/gl_funcs.py
	cd scripts; python generate_glapi.py

src/libtrace/replay_gl.c: scripts/nontrivial_func_impls.c scripts/generate_replay.py scripts/glapi/*
	cd scripts; python generate_replay.py

src/libtrace/libtrace_glstate.h: scripts/generate_libtrace_glstate.py
	cd scripts; python generate_libtrace_glstate.py

src/shared/glcorearb.h:
	cd src/shared; wget https://raw.githubusercontent.com/KhronosGroup/OpenGL-Registry/master/api/GL/glcorearb.h

bin/libgl.so: src/.libgl.o
	$(CC) $^ -o bin/libgl.so -shared -fPIC -ldl -g $(COMP_LIBS) $(CFLAGS)

bin/libtrace.so: $(libtrace_obj) src/libtrace/.replay_gl.o
	$(CC) $^ -o bin/libtrace.so -shared -fPIC -g -lGL -ldl `sdl2-config --libs` -pthread $(COMP_LIBS) $(CFLAGS)

bin/gui: $(gui_obj) src/shared/.glapi.o bin/libtrace.so
	$(CC) -Lbin -Wl,-rpath=. -ltrace -lm -lepoxy $(gui_obj) src/shared/.glapi.o -o bin/gui -g `pkg-config gtk+-3.0 gtksourceview-3.0 --libs` -rdynamic $(CFLAGS)

bin/replaytrace: src/.replaytrace.o bin/libtrace.so
	$(CC) -Lbin -Wl,-rpath=. -ltrace src/.replaytrace.o -o bin/replaytrace -g -rdynamic $(CFLAGS)

bin/trace: src/.trace.o bin/libtrace.so
	$(CC) -Lbin -Wl,-rpath=. -ltrace src/.trace.o -o bin/trace -g $(CFLAGS)

bin/test: src/.test.o
	$(CC) $^ -o bin/test -g $(CFLAGS) -lSDL2 -lGL

bin/tests: $(tests_obj)
	$(CC) $^ `sdl2-config --libs` -lGL -o bin/tests -g $(CFLAGS)

.PHONY: clean
clean:
	rm -f scripts/generated_gl_funcs.py
	rm -f src/libgl.c
	rm -f src/shared/glcorearb.h
	rm -f src/libtrace/libtrace_glstate.h
	rm -f src/libtrace/replay_gl.c
	rm -f src/shared/glapi.c
	rm -f bin/libtrace.so
	rm -f bin/gui
	rm -f bin/trace
	rm -f bin/test
	rm -f bin/gl.so
	rm -f bin/tests
	rm -f bin/replaytrace
	rm -f $(obj)
	rm -f $(dep)
