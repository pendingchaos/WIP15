gl:
	cd scripts; python generate_gl.py
	gcc src/gl.c -o bin/gl.so -shared -fPIC -ldl -D_GNU_SOURCE -O2 -std=c99
	rm src/gl.c

libtrace:
	gcc src/libtrace.c -o bin/libtrace.so src/vec.c -shared -fPIC -std=c99 -D_DEFAULT_SOURCE -Wall -O2

libinspect:
	gcc -c src/glapi.c -std=c99 -o src/glapi.o -w -rdynamic
	gcc src/libinspect.c src/replay.c src/replay_gl.c src/vec.c  src/glapi.o -o bin/libinspect.so -shared -fPIC -std=c99 -D_DEFAULT_SOURCE -D_GNU_SOURCE -lGL -ldl -lX11 -Wall -O2
	rm src/glapi.o

trace:
	gcc src/trace.c -o bin/trace -O2

inspect-gui:
	cd bin; gcc /home/rugrats/Documents/Python/WIP15/bin/libtrace.so /home/rugrats/Documents/Python/WIP15/bin/libinspect.so ../src/inspect_gui.c -std=c99 -o inspect-gui -Wall -O2 `pkg-config --cflags --libs gtk+-3.0` -rdynamic

clean:
	rm bin/libtrace.so
	rm bin/libinspect.so
	rm bin/inspect-gui
	rm bin/trace
	rm bin/test
	rm bin/gl.so

test:
	gcc test.c -lSDL2 -lGL -o bin/test -O2 -std=c99

all:
	make gl
	make libtrace
	make libinspect
	make trace
	make inspect-gui
