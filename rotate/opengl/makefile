CC=gcc
CFLAGS=-std=c11 -O3 -flto
INCLUDE_DIRS=-I../../lib
INCLUDE_LIBS=-L../../external/cglm/.libs/
LIBS= -lcglm -lSDL2 -lGLEW -lGL

rotate: rotate.c ../../lib/sdl2_opengl/sdl2_opengl.c
	$(CC) $(CFLAGS) $(INCLUDE_DIRS) $(INCLUDE_LIBS) $^ $(LIBS) -o $@


clean:
	rm -rf rotate
