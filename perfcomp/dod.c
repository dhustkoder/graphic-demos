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
static GLuint vao = 0, vbo = 0;
static GLuint sp_id = 0, vs_id = 0, fs_id = 0;


static void terminate_system(void)
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

	if (glcontext != NULL)
		SDL_GL_DeleteContext(glcontext);

	if (window != NULL)
		SDL_DestroyWindow(window);

	SDL_Quit();
}


static bool initialize_system(void)
{
	const GLchar* const vs_src =
	"#version 130\n"
	"in vec2 pos;\n"
	"in vec3 rgb;\n"
	"out vec4 frag_color;\n"
	"void main()\n"
	"{\n"
	"	gl_Position = vec4(pos, 0.0, 1.0);\n"
	"	frag_color = vec4(rgb / 255.0, 1.0);\n"
	"}\n";
	const GLchar* const fs_src =
	"#version 130\n"
	"in vec4 frag_color;\n"
	"out vec4 outcolor;\n"
	"void main()\n"
	"{\n"
	"	outcolor = frag_color;\n"
	"}\n";

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


	GLenum err;
	if ((err = glewInit()) != GLEW_OK) {
		fprintf(stderr, "GLEW Error: %s\n", glewGetErrorString(err));
		terminate_system();
		return false;
	}


	glGenVertexArrays(1, &vao);
	glGenBuffers(1, &vbo);
	glBindVertexArray(vao);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER,
	             sizeof(GLfloat) * 1024,
	             NULL, GL_DYNAMIC_DRAW);

	sp_id = glCreateProgram();
	if (sp_id == 0) {
		fprintf(stderr, "Couldn't create GL Program\n");
		terminate_system();
		return false;
	}

	vs_id = glCreateShader(GL_VERTEX_SHADER);
	if (vs_id == 0) {
		fprintf(stderr, "Couldn't create Vertex Shader\n");
		terminate_system();
		return false;
	}

	fs_id = glCreateShader(GL_FRAGMENT_SHADER);
	if (fs_id == 0) {
		fprintf(stderr, "Couldn't create Fragment Shader\n");
		terminate_system();
		return false;
	}

	// compile vertex shader
	glShaderSource(vs_id, 1, &vs_src, NULL);
	glCompileShader(vs_id);
	
	GLint shader_success;

	glGetShaderiv(vs_id, GL_COMPILE_STATUS, &shader_success);
	if (shader_success == GL_FALSE) {
		fprintf(stderr, "Couldn't compile Vertex Shader\n");
		terminate_system();
		return false;
	}

	glAttachShader(sp_id, vs_id);


	// compile fragment shader
	glShaderSource(fs_id, 1, &fs_src, NULL);
	glCompileShader(fs_id);
	
	glGetShaderiv(fs_id, GL_COMPILE_STATUS, &shader_success);
	if (shader_success == GL_FALSE) {
		fprintf(stderr, "Couldn't compile Vertex Shader\n");
		terminate_system();
		return false;
	}

	glAttachShader(sp_id, fs_id);

	glLinkProgram(sp_id);
	glUseProgram(sp_id);

	const GLint pos_attrib = glGetAttribLocation(sp_id, "pos");
	const GLint rgb_attrib = glGetAttribLocation(sp_id, "rgb");

	glEnableVertexAttribArray(pos_attrib);
	glEnableVertexAttribArray(rgb_attrib);
	glVertexAttribPointer(pos_attrib, 2, GL_FLOAT, GL_TRUE,
	                      sizeof(GLfloat) * 5, NULL);
	glVertexAttribPointer(rgb_attrib, 3, GL_FLOAT, GL_TRUE,
	                      sizeof(GLfloat) * 5,
	                      (void*)(sizeof(GLfloat) * 2));



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

		GLfloat* mapbuff = glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY);
		
		mapbuff[0] = -0.5;
		mapbuff[1] = -0.5;
		mapbuff[2] = 0xFF;
		mapbuff[3] = 0;
		mapbuff[4] = 0;
		mapbuff += 5;

		mapbuff[0] = 0.5;
		mapbuff[1] = -0.5;
		mapbuff[2] = 0;
		mapbuff[3] = 0xFF;
		mapbuff[4] = 0;
		mapbuff += 5;

		mapbuff[0] = 0.5;
		mapbuff[1] = 0.5;
		mapbuff[2] = 0;
		mapbuff[3] = 0;
		mapbuff[4] = 0xFF;
		mapbuff += 5;

		mapbuff[0] = -0.5;
		mapbuff[1] = 0.5;
		mapbuff[2] = 0xFF;
		mapbuff[3] = 0xFF;
		mapbuff[4] = 0xFF;

		glUnmapBuffer(GL_ARRAY_BUFFER);
		glDrawArrays(GL_QUADS, 0, 4);

		SDL_GL_SwapWindow(window);	
	}

	terminate_system();
	return EXIT_SUCCESS;
}

