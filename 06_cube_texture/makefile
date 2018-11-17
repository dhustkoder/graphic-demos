CC=gcc
CFLAGS=-std=c11 -O3 -flto
INCLUDE_DIRS=-I../common -I../external/cglm/include -I../external/stb
INCLUDE_LIBS=-L../common -L../external/cglm/.libs/
LIBS=  -lm -lsogl -l:libcglm.a -lSDL2 -lGLEW -lGL

cube_texture.out: cube_texture.c
	$(CC) $(CFLAGS) $(INCLUDE_DIRS) $(INCLUDE_LIBS) $^ $(LIBS) -o $@


clean:
	rm -rf *.out
