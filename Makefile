gl:
	cd scripts; python generate_gl.py
	gcc src/gl.c -o bin/gl.so -shared -fPIC -ldl -D_GNU_SOURCE -g -std=c99
	rm src/gl.c

libtrace:
	gcc -Isrc src/libtrace/libtrace.c -o bin/libtrace.so src/shared/vec.c -shared -fPIC -std=c99 -D_DEFAULT_SOURCE -Wall -g

libinspect:
	gcc -c src/shared/glapi.c -std=c99 -o src/shared/glapi.o -w -rdynamic
	gcc -Isrc src/libinspect/libinspect.c src/libinspect/actions.c src/libinspect/replay.c src/libinspect/replay_gl.c src/shared/vec.c src/shared/glapi.o -o bin/libinspect.so -shared -fPIC -std=c99 -D_DEFAULT_SOURCE -D_GNU_SOURCE -lGL -ldl -lX11 -Wall -g
	rm src/shared/glapi.o

trace:
	gcc src/trace.c -o bin/trace -g

inspect-gui:
	gcc -c src/shared/glapi.c -std=c99 -o src/shared/glapi.o -w -rdynamic
	cd bin; gcc -I../src /home/rugrats/Documents/Python/WIP15/bin/libtrace.so /home/rugrats/Documents/Python/WIP15/bin/libinspect.so ../src/shared/glapi.o ../src/inspect_gui.c -std=c99 -o inspect-gui -Wall -g `pkg-config --cflags --libs gtk+-3.0` -rdynamic -D_DEFAULT_SOURCE
	rm src/shared/glapi.o

leakcheck:
	cd bin; gcc -I../src /home/rugrats/Documents/Python/WIP15/bin/libtrace.so /home/rugrats/Documents/Python/WIP15/bin/libinspect.so ../src/leakcheck.c -std=c99 -o leakcheck -Wall -g -rdynamic -D_DEFAULT_SOURCE

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
