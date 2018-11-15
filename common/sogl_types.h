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
	union {
		struct {
			GLfloat x0, y0, z0;
			GLfloat x1, y1, z1;
			GLfloat x2, y2, z2;
		};
		struct vec3 vecs[3];
	};
};

struct mat4 {
	union {
		struct {
			GLfloat x0, y0, z0, w0;
			GLfloat x1, y1, z1, w1;
			GLfloat x2, y2, z2, w2;
			GLfloat x3, y3, z3, w3;
		};
		struct vec4 vecs[4];
	};
};


#define VEC2_ONE (struct vec2) { 1, 1 }
#define VEC3_ONE (struct vec3) { 1, 1, 1 }
#define VEC4_ONE (struct vec4) { 1, 1, 1 }

#define SOGL_MAT3_IDENTITY (struct mat3) { \
	1, 0, 0,                               \
	0, 1, 0,                               \
	0, 0, 1                                \
}

#define SOGL_MAT4_IDENTITY (struct mat4) { \
	1, 0, 0, 0,                            \
	0, 1, 0, 0,                            \
	0, 0, 1, 0,                            \
	0, 0, 0, 1                             \
}


#endif
