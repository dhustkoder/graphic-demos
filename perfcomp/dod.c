#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <time.h>
#include <SDL2/SDL.h>
#include <GL/glew.h>
#include <sys/random.h>

#define WIN_WIDTH  (800)
#define WIN_HEIGHT (600)
#define CLAMP(val, min, max) (val < min ? min : val > max ? max : val)

static SDL_Window* window = NULL;
static SDL_GLContext glcontext = NULL;
static GLuint vao = 0, vbo = 0;
static GLuint sp_id = 0, vs_id = 0, fs_id = 0;


static GLfloat randnum_normalized()
{
	const long rnd = rand();
	const GLfloat normalized = rnd / ((double)RAND_MAX);
	return rnd > (RAND_MAX / 2) ? normalized : -normalized;
}

static void terminate_system(void)
{
	if (vbo != 0)
		glDeleteBuffers(1, &vbo);

	if (vao != 0)
		glDeleteVertexArrays(1, &vao);

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
	             sizeof(GLfloat) * 5 * 32000,
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


	// compile fragment shader
	glShaderSource(fs_id, 1, &fs_src, NULL);
	glCompileShader(fs_id);
	
	glGetShaderiv(fs_id, GL_COMPILE_STATUS, &shader_success);
	if (shader_success == GL_FALSE) {
		fprintf(stderr, "Couldn't compile Fragment Shader\n");
		terminate_system();
		return false;
	}


	glAttachShader(sp_id, vs_id);
	glAttachShader(sp_id, fs_id);
	glLinkProgram(sp_id);
	glUseProgram(sp_id);

	const GLint pos_attrib = glGetAttribLocation(sp_id, "pos");
	const GLint rgb_attrib = glGetAttribLocation(sp_id, "rgb");

	glEnableVertexAttribArray(pos_attrib);
	glEnableVertexAttribArray(rgb_attrib);
	glVertexAttribPointer(pos_attrib, 2, GL_FLOAT, GL_TRUE,
	                      sizeof(GLfloat) * 5, NULL);
	glVertexAttribPointer(rgb_attrib, 3, GL_FLOAT, GL_FALSE,
	                      sizeof(GLfloat) * 5,
	                      (void*)(sizeof(GLfloat) * 2));

	srand(time(NULL));
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


	SDL_GL_SetSwapInterval(0);


	struct color {
		GLfloat r, g, b;
	};

	struct vec2f {
		GLfloat x, y;
	};

	struct vertex {
		struct vec2f pos;
		struct color color;
	};

	struct vertex* vertexs = malloc(sizeof(struct vertex) * 4 * 32000);
	struct vec2f* vels = malloc(sizeof(struct vec2f) * 32000);
	struct vec2f* poss = malloc(sizeof(struct vec2f) * 32000);

	for (int i = 0; i < 32000 * 4; i += 4) {
		const GLfloat posx = randnum_normalized();
		const GLfloat posy = randnum_normalized();
		const GLfloat velx = randnum_normalized() / 1;
		const GLfloat vely = randnum_normalized() / 1;

		poss[i/4].x = posx;
		poss[i/4].y = posy;
		vels[i/4].x = CLAMP(velx, 0.005, 0.1);
		vels[i/4].y = CLAMP(vely, 0.005, 0.1);

		vertexs[i].pos.x = posx - 0.001;
		vertexs[i].pos.y = posy - 0.001;
		vertexs[i].color.r = 0xFF;
		vertexs[i].color.g = 0xFF;
		vertexs[i].color.b = 0xFF;

		vertexs[i + 1].pos.x = posx + 0.001;
		vertexs[i + 1].pos.y = posy - 0.001;
		vertexs[i + 1].color.r = 0xFF;
		vertexs[i + 1].color.g = 0xFF;
		vertexs[i + 1].color.b = 0xFF;

		vertexs[i + 2].pos.x = posx + 0.001;
		vertexs[i + 2].pos.y = posy + 0.001;
		vertexs[i + 2].color.r = 0xFF;
		vertexs[i + 2].color.g = 0xFF;
		vertexs[i + 2].color.b = 0xFF;

		vertexs[i + 3].pos.x = posx - 0.001;
		vertexs[i + 3].pos.y = posy + 0.001;
		vertexs[i + 3].color.r = 0xFF;
		vertexs[i + 3].color.g = 0xFF;
		vertexs[i + 3].color.b = 0xFF;
	}

	while (handle_events()) {
		const Uint32 start_ticks = SDL_GetTicks();
		
		glClearColor(0x00, 0x00, 0x00, 0xFF);
		glClear(GL_COLOR_BUFFER_BIT);


	for (int i = 0; i < 32000 * 4; i += 4) {

		if (poss[i/4].x < -1.0 || poss[i/4].x > 1.0)
			vels[i/4].x = -vels[i/4].x;
		if (poss[i/4].y < -1.0 || poss[i/4].y > 1.0)
			vels[i/4].y = -vels[i/4].y;

		poss[i/4].x += vels[i/4].x;
		poss[i/4].y += vels[i/4].y;

		const GLfloat posx = poss[i/4].x;
		const GLfloat posy = poss[i/4].y;

		vertexs[i].pos.x = posx - 0.001;
		vertexs[i].pos.y = posy - 0.001;

		vertexs[i + 1].pos.x = posx + 0.001;
		vertexs[i + 1].pos.y = posy - 0.001;

		vertexs[i + 2].pos.x = posx + 0.001;
		vertexs[i + 2].pos.y = posy + 0.001;

		vertexs[i + 3].pos.x = posx - 0.001;
		vertexs[i + 3].pos.y = posy + 0.001;
	}


		glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(struct vertex) * 32000, vertexs);
		glDrawArrays(GL_QUADS, 0, 32000 * 4);
		SDL_GL_SwapWindow(window);

		const Uint32 end_ticks = SDL_GetTicks();

		printf("FRAME TIME: %u ms\n", (end_ticks - start_ticks));

	}

	terminate_system();
	return EXIT_SUCCESS;
}

