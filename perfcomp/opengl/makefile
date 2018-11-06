CC=gcc
CXX=g++

all: oop dod

oop: oop.cpp
	$(CXX) $< -O3 -Wall -Wextra -ffast-math -o oop -lSDL2 -lGLEW -lGL -lm

dod: rand.c dod.c
	$(CC) $^ -flto -O3 -Wall -Wextra -ffast-math -fno-exceptions -o dod -lSDL2 -lGLEW -lGL -lm

clean:
	rm -rf oop dod *.o

