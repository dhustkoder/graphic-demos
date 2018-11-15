#ifndef SOGL_MATH_H_
#define SOGL_MATH_H_
#include <math.h>
#include "sogl_types.h"
#ifndef M_PI
#define M_PI (3.14159265358979323846)
#endif



static inline GLfloat sogl_radians(GLfloat deg)
{
	return deg * (M_PI / 180.0f);
}


static inline GLfloat sogl_vec3_len(const struct vec3* v)
{
	return sqrtf((v->x * v->x) + (v->y * v->y) + (v->z * v->z));
}

static inline GLfloat sogl_vec4_len(const struct vec4* v)
{
	return sqrtf((v->x * v->x) + (v->y * v->y) + (v->z * v->z) + (v->w * v->w));
}

static inline void sogl_norm_vec3(struct vec3* v)
{
	const GLfloat len = sogl_vec3_len(v);
	v->x /= len;
	v->y /= len;
	v->z /= len;
}

static inline void sogl_norm_vec4(struct vec4* v)
{
	const GLfloat len = sogl_vec4_len(v);
	v->x /= len;
	v->y /= len;
	v->z /= len;
	v->w /= len;
}

static inline void sogl_mat4_mul_rot(const struct mat4* const ma,
                                     const struct mat4* const mb,
                                     struct mat4* const mout)
{
	struct mat4 mr;
	mr.x0 = ma->x0 * mb->x0 + ma->x1 * mb->y0 + ma->x2 * mb->z0;
	mr.y0 = ma->y0 * mb->x0 + ma->y1 * mb->y0 + ma->y2 * mb->z0;
	mr.z0 = ma->z0 * mb->x0 + ma->z1 * mb->y0 + ma->z2 * mb->z0;
	mr.w0 = ma->w0 * mb->x0 + ma->w1 * mb->y0 + ma->w2 * mb->z0;

	mr.x1 = ma->x0 * mb->x1 + ma->x1 * mb->y1 + ma->x2 * mb->z1;
	mr.y1 = ma->y0 * mb->x1 + ma->y1 * mb->y1 + ma->y2 * mb->z1;
	mr.z1 = ma->z0 * mb->x1 + ma->z1 * mb->y1 + ma->z2 * mb->z1;
	mr.w1 = ma->w0 * mb->x1 + ma->w1 * mb->y1 + ma->w2 * mb->z1;

	mr.x2 = ma->x0 * mb->x2 + ma->x1 * mb->y2 + ma->x2 * mb->z2;
	mr.y2 = ma->y0 * mb->x2 + ma->y1 * mb->y2 + ma->y2 * mb->z2;
	mr.z2 = ma->z0 * mb->x2 + ma->z1 * mb->y2 + ma->z2 * mb->z2;
	mr.w2 = ma->w0 * mb->x2 + ma->w1 * mb->y2 + ma->w2 * mb->z2;

	mr.x3 = ma->x3;
	mr.y3 = ma->y3;
	mr.z3 = ma->z3;
	mr.w3 = ma->w3;

	memcpy(mout, &mr, sizeof(struct mat4));
}

static inline void sogl_mat4_rotate(const GLfloat radians,
                                    const struct vec3* const axis,
                                    const struct mat4* const mat_in,
                                    struct mat4* const mat_out)
{
	/*
	const GLfloat c = cosf(radians);
	const GLfloat s = sinf(radians);
	const GLfloat rx = axis->x;
	const GLfloat ry = axis->y;
	const GLfloat rz = axis->z;


	struct mat4 im = {
		(c + (rx * rx) * (1 - c)),      (rx * ry * (1 - c) - rz * s),   (rx * rz * (1 - c) + ry * s), 0,

		(ry * rx * (1 - c) + rz * s),   (c * (ry * ry) * (1 - c)),      (ry * rz * (1 - c) - rx * s), 0,

		(rz * rx * (1 - c) - ry * s),   (rz * ry * (1 - c) + rx * s),   (c * (rz * rz) * (1 - c)),    0,

		           0,                              0,                               0,                1
	};
	sogl_mat4_mul_rot(&im, mat_in, mat_out);
	*/

	const GLfloat cx = cosf(radians * axis->x);
	const GLfloat cy = cosf(radians * axis->y);
	const GLfloat cz = cosf(radians * axis->z);

	const GLfloat sx = sinf(radians * axis->x);
	const GLfloat sy = sinf(radians * axis->y);
	const GLfloat sz = sinf(radians * axis->z);

	const struct mat4 mx = {
		1,  0,  0,  0,
		0, cx, -sx, 0,
		0, sx, cx,  0,
		0,  0,  0,  1
	};
	const struct mat4 my = {
		 cy,  0, sy,  0,
		 0,   1,  0,  0,
		-sy,  0, cy,  0,
	         0,   0,  0,  1	
	};
	const struct mat4 mz = {
		cz, -sz,  0,  0,
		sz,  cz,  0,  0,
		0,    0,  1,  0,
		0,    0,  0,  1
	};

	struct mat4 r = SOGL_MAT4_IDENTITY;

	sogl_mat4_mul_rot(&r, &mx, &r);
	sogl_mat4_mul_rot(&r, &my, &r);
	sogl_mat4_mul_rot(&r, &mz, &r);

	memcpy(mat_out, &r, sizeof(struct mat4));

}

static inline void sogl_mul_mat4_vec3(const struct mat4* const rot,
                                      const struct vec3* const vin,
                                      struct vec3* const vout)
{
	const struct vec3 vr = {
		vin->x * rot->x0 + vin->y * rot->y0 + vin->z * rot->z0,
		vin->x * rot->x1 + vin->y * rot->y1 + vin->z * rot->z1,
		vin->x * rot->x2 + vin->y * rot->y2 + vin->z * rot->z2
	};
	memcpy(vout, &vr, sizeof(struct vec3));
}



#endif
