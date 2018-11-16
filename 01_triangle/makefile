CC=gcc
CFLAGS=-std=c11 -O3 -flto
INCLUDE_DIRS=-I../common
INCLUDE_LIBS=-L../common
LIBS=-lm -lsogl -lSDL2 -lGLEW -lGL

triangle.out: triangle.c
	$(CC) $(CFLAGS) $(INCLUDE_DIRS) $(INCLUDE_LIBS) $^ $(LIBS) -o $@


clean:
	rm -rf *.out
