gl:
	cd scripts; python generate_gl.py
	gcc src/gl.c -o bin/gl.so -shared -fPIC -ldl -D_GNU_SOURCE -g -std=c99
	rm src/gl.c

libtrace:
	gcc src/libtrace.c -o bin/libtrace.so src/vec.c -shared -fPIC -std=c99 -D_DEFAULT_SOURCE -Wall -g

libinspect:
	gcc -c src/glapi.c -std=c99 -o src/glapi.o -w -rdynamic
	gcc src/libinspect.c src/replay.c src/replay_gl.c src/vec.c src/glapi.o -o bin/libinspect.so -shared -fPIC -std=c99 -D_DEFAULT_SOURCE -D_GNU_SOURCE -lGL -ldl -lX11 -Wall -g
	rm src/glapi.o

trace:
	gcc src/trace.c -o bin/trace -g

inspect-gui:
	cd bin; gcc /home/rugrats/Documents/Python/WIP15/bin/libtrace.so /home/rugrats/Documents/Python/WIP15/bin/libinspect.so ../src/inspect_gui.c -std=c99 -o inspect-gui -Wall -g `pkg-config --cflags --libs gtk+-3.0` -rdynamic -D_DEFAULT_SOURCE

leakcheck:
	cd bin; gcc /home/rugrats/Documents/Python/WIP15/bin/libtrace.so /home/rugrats/Documents/Python/WIP15/bin/libinspect.so ../src/leakcheck.c -std=c99 -o leakcheck -Wall -g -rdynamic -D_DEFAULT_SOURCE

clean:
	rm bin/libtrace.so
	rm bin/libinspect.so
	rm bin/inspect-gui
	rm bin/trace
	rm bin/test
	rm bin/gl.so

test:
	gcc test.c -lSDL2 -lGL -o bin/test -g -std=c99

all:
	make gl
	make libtrace
	make libinspect
	make trace
	make inspect-gui
	make leakcheck
