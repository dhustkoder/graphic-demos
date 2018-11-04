#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <SDL2/SDL.h>
#include <GL/glew.h>

#define WIN_WIDTH  (800)
#define WIN_HEIGHT (600)


static SDL_Window* window = NULL;
static SDL_GLContext glcontext = NULL;


static void terminate_system(void)
{
	if (glcontext != NULL)
		SDL_GL_DeleteContext(glcontext);

	if (window != NULL)
		SDL_DestroyWindow(window);

	SDL_Quit();
}


static bool initialize_system(void)
{
	if (SDL_Init(SDL_INIT_VIDEO) < 0) {
		fprintf(stderr, "SDL_Init Error: %s\n", SDL_GetError());
		return false;
	}

	window = SDL_CreateWindow("DOD DEMO",
	                          SDL_WINDOWPOS_CENTERED,
	                          SDL_WINDOWPOS_CENTERED,
	                          WIN_WIDTH, WIN_HEIGHT,
	                          SDL_WINDOW_OPENGL);
	if (window == NULL) {
		fprintf(stderr, "SDL_CreateWindow Error: %s\n", SDL_GetError());
		terminate_system();
		return false;
	}

	if (SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1) < 0) {
		fprintf(stderr, "Couldn't set GL DOUBLE BUFFER\n");
		terminate_system();
		return false;
	}

	glcontext = SDL_GL_CreateContext(window);
	if (glcontext == NULL) {
		fprintf(stderr, "Couldn't create GL Context: %s\n", SDL_GetError());
		terminate_system();
		return false;
	}


	glewInit();

	return true;
}

static bool handle_events(void)
{
	static SDL_Event event;
	while (SDL_PollEvent(&event)) {
		if (event.type == SDL_QUIT)
			return false;
	}
	return true;
}


int main(int argc, char** argv)
{
	if (!initialize_system())
		return EXIT_FAILURE;


	while (handle_events()) {
		glClearColor(0x00, 0x00, 0x00, 0xFF);
		glClear(GL_COLOR_BUFFER_BIT);

		glRectf(-0.5, -0.5, 0.5, 0.5);	

		SDL_GL_SwapWindow(window);	
	}

	terminate_system();
	return EXIT_SUCCESS;
}

