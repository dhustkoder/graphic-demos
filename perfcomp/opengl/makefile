CC=gcc
CXX=g++

all: oop dod

oop: oop.cpp
	$(CXX) $< -O3 -Wall -Wextra -ffast-math -o oop -lSDL2

dod: dod.c
	$(CC) $^ -O3 -Wall -Wextra -ffast-math -o dod -lSDL2 -lGLEW -lGL -lm

clean:
	rm -rf oop dod

