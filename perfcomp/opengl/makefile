CC=gcc
CXX=g++

all: oop dod

oop: oop.cpp
	$(CXX) $< -O3 -Wall -Wextra -ffast-math -o oop -lSDL2 -lGLEW -lGL -lm

dod: dod.c ../../lib/sdl2_opengl/sdl2_opengl.c
	$(CC) $^ -flto -O3 -Wall -Wextra -ffast-math -fno-exceptions -I../../lib -o dod -lSDL2 -lGLEW -lGL -lm

clean:
	rm -rf oop dod *.o

