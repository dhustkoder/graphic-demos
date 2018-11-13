#ifndef SDL2_OPENGL_TYPES_H_
#define SDL2_OPENGL_TYPES_H_
#include <GL/glew.h>

struct vec2 {
	GLfloat x, y;
};

struct vec3 {
	GLfloat x, y, z;
};

struct vec4 {
	GLfloat x, y, z, w;
};


struct mat3 {
	struct vec3 data[3];
};

struct mat4 {
	struct vec4 data[4];
};


#endif
