#ifndef SDL2_OPENGL_MATH_H_
#define SDL2_OPENGL_MATH_H_
#include <math.h>
#include "sdl2_opengl_types.h"
#ifndef M_PI
#define M_PI (3.14159265358979323846)
#endif



static inline GLfloat sogl_radians(GLfloat deg)
{
	return deg * (M_PI / 180.0f);
}


static inline GLfloat vec3_len(const struct vec3* v)
{
	return sqrtf((v->x * v->x) + (v->y * v->y) + (v->z * v->z));
}

static inline GLfloat vec4_len(const struct vec4* v)
{
	return sqrtf((v->x * v->x) + (v->y * v->y) + (v->z * v->z) + (v->w * v->w));
}

static inline void norm_vec3(struct vec3* v)
{
	const GLfloat len = vec3_len(v);
	v->x /= len;
	v->y /= len;
	v->z /= len;
}

static inline void norm_vec4(struct vec4* v)
{
	const GLfloat len = vec4_len(v);
	v->x /= len;
	v->y /= len;
	v->z /= len;
	v->w /= len;
}


#endif
