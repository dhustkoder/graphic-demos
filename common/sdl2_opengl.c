#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <SDL2/SDL.h>
#include <GL/glew.h>
#include "sdl2_opengl.h"

// graphics
static SDL_Window* window = NULL;
static SDL_GLContext glcontext = NULL;
static GLuint vao = 0, vbo = 0;
static GLuint sp_id = 0, vs_id = 0, fs_id = 0;


// timing
static Uint32 frame_clk;



bool sdl2_opengl_init(const char* const winname,
                      const int width, const int height,
                      const GLchar* const vs_src,
                      const GLchar* const fs_src)
{

	if (SDL_Init(SDL_INIT_EVERYTHING) < 0) {
		fprintf(stderr, "SDL_Init Error: %s\n", SDL_GetError());
		return false;
	}

	window = SDL_CreateWindow(winname,
	                          SDL_WINDOWPOS_CENTERED,
	                          SDL_WINDOWPOS_CENTERED,
	                          width, height,
	                          SDL_WINDOW_OPENGL);
	if (window == NULL) {
		fprintf(stderr, "SDL_CreateWindow Error: %s\n", SDL_GetError());
		sdl2_opengl_term();
		return false;
	}

	if (SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1) < 0) {
		fprintf(stderr, "Couldn't set GL DOUBLE BUFFER\n");
		sdl2_opengl_term();
		return false;
	}

	glcontext = SDL_GL_CreateContext(window);
	if (glcontext == NULL) {
		fprintf(stderr, "Couldn't create GL Context: %s\n", SDL_GetError());
		sdl2_opengl_term();
		return false;
	}

	GLenum err;
	if ((err = glewInit()) != GLEW_OK) {
		fprintf(stderr, "GLEW Error: %s\n", glewGetErrorString(err));
		sdl2_opengl_term();
		return false;
	}

	glGenVertexArrays(1, &vao);
	glGenBuffers(1, &vbo);
	glBindVertexArray(vao);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, MAX_VBO_BYTES,
	             NULL, GL_DYNAMIC_DRAW);

	sp_id = glCreateProgram();
	if (sp_id == 0) {
		fprintf(stderr, "Couldn't create GL Program\n");
		sdl2_opengl_term();
		return false;
	}

	vs_id = glCreateShader(GL_VERTEX_SHADER);
	if (vs_id == 0) {
		fprintf(stderr, "Couldn't create Vertex Shader\n");
		sdl2_opengl_term();
		return false;
	}

	fs_id = glCreateShader(GL_FRAGMENT_SHADER);
	if (fs_id == 0) {
		fprintf(stderr, "Couldn't create Fragment Shader\n");
		sdl2_opengl_term();
		return false;
	}

	// compile vertex shader
	glShaderSource(vs_id, 1, &vs_src, NULL);
	glCompileShader(vs_id);
	
	GLint shader_success;

	glGetShaderiv(vs_id, GL_COMPILE_STATUS, &shader_success);
	if (shader_success == GL_FALSE) {
		fprintf(stderr, "Couldn't compile Vertex Shader\n");
		sdl2_opengl_term();
		return false;
	}


	// compile fragment shader
	glShaderSource(fs_id, 1, &fs_src, NULL);
	glCompileShader(fs_id);
	
	glGetShaderiv(fs_id, GL_COMPILE_STATUS, &shader_success);
	if (shader_success == GL_FALSE) {
		fprintf(stderr, "Couldn't compile Fragment Shader\n");
		sdl2_opengl_term();
		return false;
	}


	glAttachShader(sp_id, vs_id);
	glAttachShader(sp_id, fs_id);
	glLinkProgram(sp_id);
	glUseProgram(sp_id);

	glEnable(GL_DEPTH_TEST);

	printf("SDL2 OPENG INITIALIZED!\n"
	       "W: set wireframe\n"
	       "D: set depth bit\n");

	return true;
}

void sdl2_opengl_term(void)
{
	if (fs_id != 0) {
		glDetachShader(sp_id, fs_id);
		glDeleteShader(fs_id);
	}

	if (vs_id != 0) {
		glDetachShader(sp_id, vs_id);
		glDeleteShader(vs_id);
	}

	if (sp_id != 0)
		glDeleteProgram(sp_id);
	
	if (vbo != 0)
		glDeleteBuffers(1, &vbo);

	if (vao != 0)
		glDeleteVertexArrays(1, &vao);

	if (glcontext != NULL)
		SDL_GL_DeleteContext(glcontext);

	if (window != NULL)
		SDL_DestroyWindow(window);

	SDL_Quit();
}


bool sdl2_opengl_handle_events(void)
{
	static SDL_Event event;
	static bool wireframe = false;
	static bool depth_bit = true;


	while (SDL_PollEvent(&event)) {
		if (event.type == SDL_QUIT)
			return false;

		if (event.type == SDL_KEYDOWN) {
			bool printcfg = false;
			
			switch (event.key.keysym.scancode) {
			case SDL_SCANCODE_W:
				glPolygonMode(GL_FRONT_AND_BACK, wireframe ? GL_FILL : GL_LINE);
				wireframe = !wireframe;
				printcfg = true;
				break;
			case SDL_SCANCODE_D:
				depth_bit = !depth_bit;
				if (depth_bit)
					glEnable(GL_DEPTH_TEST);
				else
					glDisable(GL_DEPTH_TEST);
				printcfg = true;
				break;
			}

			if (printcfg) {
				printf("---------------------\n"
				       "W: set wireframe (%s)\n"
				       "D: set depth bit (%s)\n",
				       wireframe ? "true" : "false",
				       depth_bit ? "true" : "false");
			}
		}
	}
	return true;
}

void sdl2_opengl_begin_frame(void)
{
	frame_clk = SDL_GetTicks();
}

Uint32 sdl2_opengl_end_frame(void)
{
	frame_clk = SDL_GetTicks() - frame_clk;
	SDL_GL_SwapWindow(window);
	return frame_clk;
}


void sdl2_opengl_vattrp(const GLchar* const attrib_name,
                        const GLint size,
                        const GLenum type,
                        const GLboolean normalized,
                        const GLsizei stride,
                        const GLvoid* const pointer)
{
	const GLuint index = glGetAttribLocation(sp_id, attrib_name);
	glEnableVertexAttribArray(index);
	glVertexAttribPointer(index, size, type, normalized, stride, pointer);
}


void sdl2_opengl_set_uniform(const GLchar* const name, const void* const data)
{
	static const GLchar* lastname_addr;
	static GLint index;
	
	if (lastname_addr != name) {
		index = glGetUniformLocation(sp_id, name);
		lastname_addr = name;
	}

	glUniformMatrix4fv(index, 1, GL_FALSE, data);
}
