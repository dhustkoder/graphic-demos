CC=gcc
CFLAGS=-std=c11 -O3 -flto
INCLUDE_DIRS=-I../common
INCLUDE_LIBS=
LIBS=-lm -lSDL2 -lGLEW -lGL

rotate: rotate.c ../common/sdl2_opengl.c
	$(CC) $(CFLAGS) $(INCLUDE_DIRS) $(INCLUDE_LIBS) $^ $(LIBS) -o $@


clean:
	rm -rf rotate
