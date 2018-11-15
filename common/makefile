CC=gcc
AR=ar
CFLAGS=-std=c11 -O3 -flto -c
INCLUDE_DIRS=
INCLUDE_LIBS=
LIBS= -lm -lSDL2 -lGLEW -lGL

libsogl.a: libsogl.o
	$(AR) rcs $@ $^

libsogl.o: sogl.c
	$(CC) $(CFLAGS) $(INCLUDE_DIRS) $(INCLUDE_LIBS) $^ $(LIBS) -o $@


clean:
	rm -rf *.a *.o
