cc = gcc
cflags = -std=c17 

clean:
	touch bin/temp.o
	rm bin/*.o

bin/input.o:
	$(cc) -c $(cflags) src/input.c -o bin/input.o

bin/Tilemap_JSON_Conversion.o:
	$(cc) -c $(cflags) src/Tilemap_JSON_Conversion.c -o bin/Tilemap_JSON_Conversion.o

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

bin/World.o:
	$(cc) -c $(cflags) src/World.c -o bin/World.o

bin/Settings.o:
	$(cc) -c $(cflags) src/Settings.c -o bin/Settings.o

bin/Save.o:
	$(cc) -c $(cflags) src/Save.c -o bin/Save.o

bin/World_Chip_Note.o:
	$(cc) -c $(cflags) src/World_Chip_Note.c -o bin/World_Chip_Note.o

bin/main.o:
	$(cc) -c $(cflags) src/main.c -o bin/main.o

compile: clean bin/main.o bin/Animation.o bin/input.o bin/UI.o bin/Title_Screen.o bin/Background.o bin/Game_State.o bin/Particle.o bin/Tilemap_JSON_Conversion.o bin/World.o bin/Settings.o bin/Save.o bin/World_Chip_Note.o

merge: compile
	ld -relocatable bin/*.o -o FNAF_World_C.o
	rm bin/*.o
	cp FNAF_World_C.o bin/FNAF_World_C.o
	rm FNAF_World_C.o

build_windows: merge
	$(cc) -o FNAF_World_C.exe bin/FNAF_World_C.o Lib/cJSON.c -lraylib -lgdi32 -lwinmm -I include/ -L lib/