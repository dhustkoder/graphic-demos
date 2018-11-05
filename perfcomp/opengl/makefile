CC=gcc
CXX=g++

all: oop dod

oop: oop.cpp
	$(CXX) $< -O3 -Wall -Wextra -ffast-math -o oop -lSDL2

dod: dod.c
	$(CXX) $^ -O3 -Wall -Wextra -ffast-math -I../../externals/pcg-c/include -I../../externals/pcg-c/extras -o dod -lSDL2 -lGLEW -lGL -lm ../../externals/pcg-c/extras/entropy.o ../../externals/pcg-c/src/libpcg_random.a rand.o 

clean:
	rm -rf oop dod

