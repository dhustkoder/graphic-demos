CC=gcc
CFLAGS=-std=c11 -O3 -flto
INCLUDE_DIRS=-I../../common -I../../external/cglm/include
INCLUDE_LIBS=-L../../external/cglm/.libs/
LIBS= -lcglm -lSDL2 -lGLEW -lGL

piramid: piramid.c ../../common/sdl2_opengl/sdl2_opengl.c
	$(CC) $(CFLAGS) $(INCLUDE_DIRS) $(INCLUDE_LIBS) $^ $(LIBS) -o $@


clean:
	rm -rf piramid
