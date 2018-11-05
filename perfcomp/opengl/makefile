CC=gcc
CXX=g++

all: rand.o oop dod

oop: oop.cpp
	$(CXX) $< -O3 -Wall -Wextra -ffast-math -o oop -lSDL2

dod: rand.o dod.c
	$(CXX) $^ -O3 -Wall -Wextra -ffast-math -fno-exceptions -o dod -lSDL2 -lGLEW -lGL -lm

rand.o: rand.cpp
	$(CXX) -c $^ -O3 -Wall -Wextra -ffast-math

clean:
	rm -rf oop dod rand.o

