ifndef $(BUILD_TYPE)
BUILD_TYPE = debug
endif

BUILD_DIR = build/$(BUILD_TYPE)/

USE_LZ4 = $(shell pkg-config liblz4 --exists && echo 1 || echo 0)
USE_ZLIB = $(shell pkg-config zlib --exists && echo 1 || echo 0)
USE_ZSTD = 0
USE_GTKSOURCEVIEW = $(shell pkg-config gtksourceview-3.0 --exists && echo 1 || echo 0)

CFLAGS = -fvisibility=hidden -Wall -std=c99 `sdl2-config --cflags` `pkg-config gtk+-3.0 --cflags` -D_DEFAULT_SOURCE -D_GNU_SOURCE -Isrc -fPIC -fno-strict-aliasing -DBUILD_TYPE=\"$(BUILD_TYPE)\"
ifeq ($(BUILD_TYPE), debug)
CFLAGS += -g
endif
ifeq ($(BUILD_TYPE), release)
CFLAGS += -O2 -DNDEBUG
endif
ifeq ($(BUILD_TYPE), coverage)
CFLAGS += -g -fprofile-arcs -ftest-coverage
endif

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

base_gui_obj = $(gui_src:.c=.o) src/shared/glapi.o src/shared/types.o
gui_obj = $(addprefix $(BUILD_DIR), $(base_gui_obj))

base_libtrace_obj = $(libtrace_src:.c=.o) src/shared/types.o src/shared/utils.o src/shared/glapi.o
libtrace_obj = $(addprefix $(BUILD_DIR), $(base_libtrace_obj))

base_tests_obj = $(tests_src:.c=.o)
tests_obj = $(addprefix $(BUILD_DIR), $(base_tests_obj))

base_obj = $(src:.c=.o)
obj = $(addprefix $(BUILD_DIR), $(base_obj))

dep = $(obj:.o=.d)

.PHONY: bin
bin: all clean-bin
	@cp $(BUILD_DIR)libgl.so bin/libgl.so
	@cp $(BUILD_DIR)libtrace.so bin/libtrace.so
	@cp $(BUILD_DIR)gui bin/gui
	@cp $(BUILD_DIR)replaytrace bin/replaytrace
	@cp $(BUILD_DIR)trace bin/trace
	@cp $(BUILD_DIR)test bin/test
	@cp $(BUILD_DIR)tests bin/tests

.PHONY: all
all: $(BUILD_DIR)libtrace.so $(BUILD_DIR)libgl.so $(BUILD_DIR)trace $(BUILD_DIR)gui\
	 $(BUILD_DIR)replaytrace $(BUILD_DIR)test $(BUILD_DIR)tests

.PHONY: clean-bin
clean-bin:
	@rm -f bin/libgl.so bin/libtrace.so bin/gui bin/replaytrace bin/trace bin/test bin/tests

.PHONY: clean
clean: clean-bin
	@rm -f scripts/generated_gl_funcs.py
	@rm -f src/libgl.c
	@rm -f src/shared/glcorearb.h
	@rm -f src/libtrace/libtrace_glstate.h
	@rm -f src/libtrace/replay.h
	@rm -f src/libtrace/replay.c
	@rm -f src/shared/glapi.c
	@rm -rf build/
	@rm -rf coverage

.PHONY: test-coverage
test-coverage:
	rm -rf coverage
	
	make -j$(getconf _NPROCESSORS_ONLN) BUILD_TYPE=debug
	make -j$(getconf _NPROCESSORS_ONLN) BUILD_TYPE=coverage
	
	make bin BUILD_TYPE=debug
	cd bin; ./trace -o tests.trace ./tests
	
	make bin BUILD_TYPE=coverage
	cd bin; ./replaytrace tests.trace;
	rm bin/tests.trace
	
	lcov --capture --directory build/coverage/src/libtrace/replay --output-file coverage.info
	genhtml coverage.info --output-directory coverage
	rm coverage.info

-include $(dep)

$(BUILD_DIR)%.d: %.c src/libtrace/libtrace_glstate.h src/shared/glcorearb.h src/libtrace/replay.h
	@mkdir -p $(dir $@)
	@$(CPP) $(CFLAGS) $< -MM -MT $(@:.d=.o) >$@

$(BUILD_DIR)%.o: %.c scripts/compile_file.py scripts/preprocess_replay.py src/libtrace/libtrace_glstate.h src/shared/glcorearb.h src/libtrace/replay.h
	@mkdir -p $(dir $@)
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

$(BUILD_DIR)libgl.so: $(BUILD_DIR)src/libgl.o $(BUILD_DIR)src/shared/types.o $(BUILD_DIR)src/shared/utils.o
	$(CC) $^ -o $@ -shared -fPIC -ldl -pthread -g $(COMP_LIBS) $(CFLAGS)

$(BUILD_DIR)libtrace.so: $(libtrace_obj)
	$(CC) $^ -o $@ -shared -fPIC -g -lepoxy -lGL -ldl `sdl2-config --libs` -pthread $(COMP_LIBS) $(CFLAGS)

$(BUILD_DIR)gui: $(gui_obj) $(BUILD_DIR)libtrace.so $(BUILD_DIR)libgl.so
	$(CC) -L$(BUILD_DIR) -Wl,-rpath,'$$ORIGIN' -ltrace -lm -lepoxy $(gui_obj) -o $@ -g `pkg-config gtk+-3.0 --libs` $(GTK_SOURCEVIEW_LIBS) -rdynamic $(CFLAGS)

$(BUILD_DIR)replaytrace: $(BUILD_DIR)src/replaytrace.o $(BUILD_DIR)libtrace.so
	$(CC) -L$(BUILD_DIR) -Wl,-rpath,'$$ORIGIN' -ltrace $(BUILD_DIR)src/replaytrace.o -o $@ -g -rdynamic $(CFLAGS)

$(BUILD_DIR)trace: $(BUILD_DIR)src/trace.o $(BUILD_DIR)libtrace.so $(BUILD_DIR)libgl.so
	$(CC) -L$(BUILD_DIR) -Wl,-rpath,'$$ORIGIN' -ltrace $(BUILD_DIR)src/trace.o -o $@ -g $(CFLAGS)

$(BUILD_DIR)test: $(BUILD_DIR)src/test.o
	$(CC) $^ -o $@ -g $(CFLAGS) -lSDL2 -lGL

$(BUILD_DIR)tests: $(tests_obj)
	$(CC) $^ `sdl2-config --libs` -lGL -o $@ -g $(CFLAGS)
