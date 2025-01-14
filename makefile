cc = gcc
cflags = -std=c17

clean:
	touch bin/temp.o
	rm bin/*.o


compile: 
	gcc $(cflags) -c src/*.c
	ld -relocatable *.o -o FNAF_World_C.o
	cp FNAF_World_C.o bin/FNAF_World_C.o
	rm *.o

build_windows: compile
	$(cc) -o FNAF_World_C.exe bin/FNAF_World_C.o Lib/cJSON.c -lraylib -lgdi32 -lwinmm -I include/ -L lib/