#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <time.h>
#include <SDL2/SDL.h>
#include <GL/glew.h>


#define WIN_WIDTH  (1280)
#define WIN_HEIGHT (720)
#define MAX_RECTS  (500000)


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


static struct vertex vertexs[MAX_RECTS * 4];
static struct vec2f vels[MAX_RECTS];
static struct vec2f poss[MAX_RECTS];
static GLfloat sizes[MAX_RECTS];
static long nrects = 0;

static SDL_Window* window = NULL;
static SDL_GLContext glcontext = NULL;
static GLuint vao = 0, vbo = 0;
static GLuint sp_id = 0, vs_id = 0, fs_id = 0;


static GLfloat randnum_normalized(double mu, double sigma)
{
	double U1, U2, W, mult;
	static double X1, X2;
	static bool call = 0;

	if (call) {
		call = !call;
		return (mu + sigma * (double) X2);
	}

	do {
		U1 = -1 + ((double) rand () / RAND_MAX) * 2;
		U2 = -1 + ((double) rand () / RAND_MAX) * 2;
		W = pow (U1, 2) + pow (U2, 2);
	} while (W >= 1 || W == 0);

	mult = sqrt ((-2 * log (W)) / W);
	X1 = U1 * mult;
	X2 = U2 * mult;

	call = !call;
	return (mu + sigma * (double) X1);
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
	"	frag_color = vec4(rgb, 1.0);\n"
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


static void push_rect(void)
{
	if (nrects >= MAX_RECTS)
		return;
	
	const GLfloat posx = randnum_normalized(-0.01, 0.01);
	const GLfloat posy = randnum_normalized(-0.01, 0.01);
	const GLfloat velx = randnum_normalized(-0.0001, 0.001);
	const GLfloat vely = randnum_normalized(-0.0001, 0.001);
	const GLfloat r = randnum_normalized(0.2, 0.8);
	const GLfloat g = randnum_normalized(0.2, 0.8);
	const GLfloat b = randnum_normalized(0.2, 0.8);
	const GLfloat size = randnum_normalized(0.0001, 0.0009);
	//printf("POSX: %f\nPOSY: %f\nVELX: %f\nVELY: %f\n\n", posx, posy, velx, vely);

	poss[nrects].x = posx;
	poss[nrects].y = posy;
	vels[nrects].x = velx;
	vels[nrects].y = vely;
	sizes[nrects] = size;

	vertexs[nrects * 4].pos.x = posx - size;
	vertexs[nrects * 4].pos.y = posy - size;
	vertexs[nrects * 4].color.r = r;
	vertexs[nrects * 4].color.g = g;
	vertexs[nrects * 4].color.b = b;

	vertexs[nrects * 4 + 1].pos.x = posx + size;
	vertexs[nrects * 4 + 1].pos.y = posy - size;
	vertexs[nrects * 4 + 1].color.r = r;
	vertexs[nrects * 4 + 1].color.g = g;
	vertexs[nrects * 4 + 1].color.b = b;

	vertexs[nrects * 4 + 2].pos.x = posx + size;
	vertexs[nrects * 4 + 2].pos.y = posy + size;
	vertexs[nrects * 4 + 2].color.r = r;
	vertexs[nrects * 4 + 2].color.g = g;
	vertexs[nrects * 4 + 2].color.b = b;

	vertexs[nrects * 4 + 3].pos.x = posx - size;
	vertexs[nrects * 4 + 3].pos.y = posy + size;
	vertexs[nrects * 4 + 3].color.r = r;
	vertexs[nrects * 4 + 3].color.g = g;
	vertexs[nrects * 4 + 3].color.b = b;

	++nrects;
}



int main(int argc, char** argv)
{
	if (!initialize_system())
		return EXIT_FAILURE;

	SDL_GL_SetSwapInterval(0);

	while (handle_events()) {
		const Uint32 start_ticks = SDL_GetTicks();
		
		glClearColor(0x00, 0x00, 0x00, 0xFF);
		glClear(GL_COLOR_BUFFER_BIT);


		for (long i = 0; i < nrects; ++i) {

			if (poss[i].x < -1.0 || poss[i].x > 1.0)
				vels[i].x = -vels[i].x;
			if (poss[i].y < -1.0 || poss[i].y > 1.0)
				vels[i].y = -vels[i].y;

			poss[i].x += vels[i].x;
			poss[i].y += vels[i].y;

			const GLfloat posx = poss[i].x;
			const GLfloat posy = poss[i].y;

			vertexs[i * 4].pos.x = posx - sizes[i];
			vertexs[i * 4].pos.y = posy - sizes[i];

			vertexs[i * 4 + 1].pos.x = posx + sizes[i];
			vertexs[i * 4 + 1].pos.y = posy - sizes[i];

			vertexs[i * 4 + 2].pos.x = posx + sizes[i];
			vertexs[i * 4 + 2].pos.y = posy + sizes[i];

			vertexs[i * 4 + 3].pos.x = posx - sizes[i];
			vertexs[i * 4 + 3].pos.y = posy + sizes[i];
		}

		if (nrects < 6000) {
			glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(struct vertex) * 4 * nrects, vertexs);
			glDrawArrays(GL_QUADS, 0, nrects * 4);
		} else {
			const long packs = nrects / 6000;
			for (long i = 0; i < packs; ++i) {
				glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(struct vertex) * 4 * 6000, &vertexs[i * 4 * 6000]);
				glDrawArrays(GL_QUADS, 0, 6000 * 4);
			}
			const long remaining = nrects - packs * 6000;
			glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(struct vertex) * 4 * remaining, &vertexs[packs * 6000 * 4]);
			glDrawArrays(GL_QUADS, 0, remaining * 4);
			
		}
		SDL_GL_SwapWindow(window);

		const Uint32 end_ticks = SDL_GetTicks();
		if ((end_ticks - start_ticks) < 16) {
			for (int i = 0; i < 50; ++i)
				push_rect();
		}

		printf("RECTS: %ld\n", nrects);
		printf("FRAME TIME: %u ms\n", (end_ticks - start_ticks));
	}

	terminate_system();
	return EXIT_SUCCESS;
}
