USE_LZ4 = $(shell pkg-config liblz4 --exists && echo 1 || echo 0)
USE_ZLIB = $(shell pkg-config zlib --exists && echo 1 || echo 0)
USE_ZSTD = 0
USE_GTKSOURCEVIEW = $(shell pkg-config gtksourceview-3.0 --exists && echo 1 || echo 0)

CFLAGS = -fvisibility=hidden -Wall -std=c99 `sdl2-config --cflags` `pkg-config gtk+-3.0 --cflags` -D_DEFAULT_SOURCE -D_GNU_SOURCE -Isrc -fPIC -g -fno-strict-aliasing

COMP_LIBS =
ifeq ($(USE_LZ4), 1)
COMP_LIBS += `pkg-config liblz4 --libs`
CFLAGS += -DLZ4_ENABLED `pkg-config liblz4 --cflags`
endif

ifeq ($(USE_ZLIB), 1)
COMP_LIBS += `pkg-config zlib --libs`
CFLAGS += -DZLIB_ENABLED `pkg-config zlib --cflags`
endif

ifeq ($(USE_ZSTD), 1)
COMP_LIBS += `pkg-config libzstd --libs`
CFLAGS += -DZSTD_ENABLED `pkg-config libzstd --cflags`
endif

GTK_SOURCEVIEW_LIBS =
ifeq ($(USE_GTKSOURCEVIEW), 1)
GTK_SOURCEVIEW_LIBS += `pkg-config gtksourceview-3.0 --libs`
CFLAGS += -DGTKSOURCEVIEW_ENABLED `pkg-config gtksourceview-3.0 --cflags`
endif

gui_src = $(shell find src/gui/ -type f -name '*.c')
libtrace_src = $(wildcard src/libtrace/*.c) $(wildcard src/libtrace/replay/*.c) src/libtrace/replay.c
tests_src = $(wildcard src/testing/tests/*.c)
src = $(shell find src/ -type f -name '*.c') src/libgl.c

base_gui_obj = $(gui_src:.c=.o)
gui_obj = $(join $(dir $(base_gui_obj)), $(addprefix ., $(notdir $(base_gui_obj)))) src/shared/.glapi.o src/shared/.types.o

base_libtrace_obj = $(libtrace_src:.c=.o)
libtrace_obj = $(join $(dir $(base_libtrace_obj)), $(addprefix ., $(notdir $(base_libtrace_obj)))) src/shared/.types.o src/shared/.utils.o src/shared/.glapi.o

base_tests_obj = $(tests_src:.c=.o)
tests_obj = $(join $(dir $(base_tests_obj)), $(addprefix ., $(notdir $(base_tests_obj))))

base_obj = $(src:.c=.o)
obj = $(join $(dir $(base_obj)), $(addprefix ., $(notdir $(base_obj))))

dep = $(obj:.o=.d)

.PHONY: all
all: bin/libtrace.so bin/libgl.so bin/trace bin/gui bin/replaytrace bin/test bin/tests

-include $(dep)

.%.d: %.c src/libtrace/libtrace_glstate.h src/shared/glcorearb.h src/libtrace/replay.h
	@$(CPP) $(CFLAGS) $< -MM -MT $(@:.d=.o) >$@

.%.o: %.c scripts/compile_file.py scripts/preprocess_replay.py src/libtrace/libtrace_glstate.h src/shared/glcorearb.h src/libtrace/replay.h
	python scripts/compile_file.py $(CC) $< $@ $(CFLAGS)

src/libtrace/replay.h: scripts/generate_replay2.py scripts/glapi/* scripts/gl_funcs.py scripts/generated_gl_funcs.py
	cd scripts; python generate_replay2.py

src/libtrace/replay.c: scripts/generate_replay2.py scripts/glapi/* scripts/gl_funcs.py scripts/generated_gl_funcs.py
	cd scripts; python generate_replay2.py

scripts/generated_gl_funcs.py: scripts/generate_gl_funcs.py
	cd scripts; python generate_gl_funcs.py

src/libgl.c: scripts/generate_gl_wrapper.py scripts/gl_funcs.py scripts/generated_gl_funcs.py scripts/gl_wrapper.c scripts/glapi/glapi.py
	cd scripts; python generate_gl_wrapper.py

src/shared/glapi.c: scripts/generate_glapi.py scripts/generated_gl_funcs.py scripts/gl_funcs.py
	cd scripts; python generate_glapi.py

src/libtrace/libtrace_glstate.h: scripts/generate_libtrace_glstate.py
	cd scripts; python generate_libtrace_glstate.py

src/shared/glcorearb.h:
	cd src/shared; wget https://raw.githubusercontent.com/KhronosGroup/OpenGL-Registry/master/api/GL/glcorearb.h

bin/libgl.so: src/.libgl.o src/shared/.types.o src/shared/.utils.o
	$(CC) $^ -o bin/libgl.so -shared -fPIC -ldl -g $(COMP_LIBS) $(CFLAGS)

bin/libtrace.so: $(libtrace_obj)
	$(CC) $^ -o bin/libtrace.so -shared -fPIC -g -lGL -ldl `sdl2-config --libs` -pthread $(COMP_LIBS) $(CFLAGS)

bin/gui: $(gui_obj) bin/libtrace.so
	$(CC) -Lbin -Wl,-rpath=. -ltrace -lm -lepoxy $(gui_obj) -o bin/gui -g `pkg-config gtk+-3.0 --libs` $(GTK_SOURCEVIEW_LIBS) -rdynamic $(CFLAGS)

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
	rm -f src/libtrace/replay.h
	rm -f src/libtrace/replay.c
	rm -f src/shared/glapi.c
	rm -f bin/libtrace.so
	rm -f bin/gui
	rm -f bin/trace
	rm -f bin/test
	rm -f bin/libgl.so
	rm -f bin/tests
	rm -f bin/replaytrace
	rm -f $(obj)
	rm -f $(dep)
