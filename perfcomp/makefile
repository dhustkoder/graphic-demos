CC=gcc
CXX=g++

all: oop dod

oop: oop.cpp
	$(CXX) $< -O3 -Wall -Wextra -ffast-math -o oop -lSDL2

dod: ../externals/true-random/true_random.c dod.c
	$(CC) -I../externals/true-random $^ -O3 -Wall -Wextra -ffast-math -o dod -lSDL2 -lGLEW -lGL

clean:
	rm -rf oop dod

