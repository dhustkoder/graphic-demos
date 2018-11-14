CC=gcc
CFLAGS=-std=c11 -O3 -flto
INCLUDE_DIRS=-I../common -I../external/cglm/include -I../external/stb
INCLUDE_LIBS=-L../external/cglm/.libs/
LIBS= -lm -lcglm -lSDL2 -lGLEW -lGL

cube_texture: cube_texture.c ../common/sdl2_opengl.c
	$(CC) $(CFLAGS) $(INCLUDE_DIRS) $(INCLUDE_LIBS) $^ $(LIBS) -o $@


clean:
	rm -rf cube_texture
