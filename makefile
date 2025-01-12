cc = gcc
cflags = -std=c17 -O3

clean:
	touch bin/temp.o
	rm bin/*.o


compile: 
	gcc $(cflags) -c src/*.c

build_mac:
	eval $(cc) src/*.c Lib/cJSON.c -framework IOKit -framework Cocoa -framework OpenGL Lib/libraylib.a -I Include/
build_windows: compile
	$(cc) -o FNAF_World_C src/*.c Lib/cJSON.c -lraylib -I include/ -L lib/