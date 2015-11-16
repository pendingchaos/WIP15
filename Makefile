gl:
	cd scripts; python generate_gl.py
	gcc src/gl.c -o bin/gl.so -shared -fPIC -ldl -D_GNU_SOURCE
	rm src/gl.c

libtrace:
	gcc src/libtrace.c -o bin/libtrace.so -shared -fPIC -std=c99 -D_DEFAULT_SOURCE

libinspect:
	gcc -c src/glapi.c -std=c99 -o src/glapi.o -w
	gcc src/libinspect.c src/replay.c src/replay_gl.c src/glapi.o -o bin/libinspect.so -shared -fPIC -std=c99 -D_DEFAULT_SOURCE -D_GNU_SOURCE -lGL -ldl -lX11
	rm src/glapi.o

trace:
	gcc src/trace.c -o bin/trace

inspect:
	cd bin; gcc /home/rugrats/Documents/Python/WIP15/bin/libtrace.so /home/rugrats/Documents/Python/WIP15/bin/libinspect.so ../src/inspect.c -std=c99 -o inspect

clean:
	rm bin/libtrace.so
	rm bin/libinspect.so
	rm bin/inspect
	rm bin/trace
	rm bin/test
	rm bin/gl.so

test:
	gcc test.c -lSDL2 -lGL -o bin/test

all:
	make gl
	make libtrace
	make libinspect
	make trace
	make inspect

