#ifndef SDL2_OPENGL_TYPES_H_
#define SDL2_OPENGL_TYPES_H_
#include <GL/glew.h>


struct vec2f {
	GLfloat x, y;
};


struct vec3f {
	GLfloat x, y, z;
};


struct vec4f {
	GLfloat x, y, z, w;
};


struct mat3f {
	struct vec3f data[3];
};


struct mat4f {
	struct vec4f data[4];
};


#endif
