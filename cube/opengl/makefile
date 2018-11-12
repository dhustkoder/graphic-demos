CC=gcc
CFLAGS=-std=c11 -O3 -flto
INCLUDE_DIRS=-I../../lib
LIBS=../../lib/sdl2_opengl/*.c -lSDL2 -lGLEW -lGL

cube: cube.c
	$(CC) $(CFLAGS) $(INCLUDE_DIRS) $^ $(LIBS) -o cube


clean:
	rm -rf cube
