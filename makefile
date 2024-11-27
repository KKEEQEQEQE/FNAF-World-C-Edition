cc = gcc
cflags = -std=c17 

clean:
	touch bin/temp.o
	rm -r bin/*.o
bin/input.o:
	$(cc) -c $(cflags) src/input.c -o bin/input.o

bin/Background.o:
	$(cc) -c $(cflags) src/Background.c -o bin/Background.o

bin/Animation.o:
	$(cc) -c $(cflags) src/Animation.c -o bin/Animation.o

bin/UI.o:
	$(cc) -c $(cflags) src/UI.c -o bin/UI.o

bin/Title_Screen.o:
	$(cc) -c $(cflags) src/Title_Screen.c -o bin/Title_Screen.o

bin/Game_State.o:
	$(cc) -c $(cflags) src/Game_State.c -o bin/Game_State.o

bin/Particle.o:
	$(cc) -c $(cflags) src/Particle.c -o bin/Particle.o


build_win: clean bin/Animation.o bin/input.o bin/UI.o bin/Title_Screen.o bin/Background.o bin/Game_State.o bin/Particle.o
	$(cc) -o FNAF_World_C.exe src/main.c bin/input.o bin/Animation.o bin/UI.o bin/Background.o bin/Game_State.o bin/Title_Screen.o bin/Particle.o -lraylib -lgdi32 -lwinmm -I include/ -L lib/