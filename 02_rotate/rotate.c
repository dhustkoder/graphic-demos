#include <stddef.h>
#include <sogl.h>
#include <sogl_math.h>


const GLchar* const vs_src =
"#version 130\n"
"in vec3 pos;\n"
"in vec3 rgb;\n"
"out vec4 frag_color;\n"
"void main()\n"
"{\n"
"	gl_Position = vec4(pos, 1.0);\n"
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


struct vertex_data {
	struct vec3 pos;
	struct vec3 rgb;
};


int main(void)
{
	if (!sogl_init("ROTATE", 800, 600, vs_src, fs_src)) {
		return EXIT_FAILURE;
	}

	sogl_vattrp("pos", 3, GL_FLOAT, GL_TRUE, sizeof(struct vertex_data), NULL);
	sogl_vattrp("rgb", 3, GL_FLOAT, GL_TRUE, sizeof(struct vertex_data),
	            (void*)offsetof(struct vertex_data, rgb));

	struct vertex_data verts[] = {
		{{-0.5, -0.5, 0}, {1, 0, 0}},
		{{ 0.5, -0.5, 0}, {0, 1, 0}},
		{{ 0.0,  0.5, 0}, {0, 0, 1}},
	};

	/* Our rotation matrix is set to rotate 1 degree around Z axis
	 * */
	struct mat4 rot = SOGL_MAT4_IDENTITY;
	sogl_mat4_rotate(sogl_radians(1), &(struct vec3){0, 0, 1}, &rot, &rot);

	while (sogl_handle_events()) {
		sogl_begin_frame();
		
		glClearColor(0, 0, 0, 0xFF);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		/* Rotation is done by applying the 
		 * transformation to all vertices/vectors of the object
		 * this will rotate the triangle 1 degree per frame
		 * */
		for (int i = 0; i < (sizeof(verts)/sizeof(verts[0])); ++i)
			sogl_mul_mat4_vec3(&rot, &verts[i].pos, &verts[i].pos);

		glBufferData(GL_ARRAY_BUFFER, sizeof(verts), verts, GL_STREAM_DRAW);
		glDrawArrays(GL_TRIANGLES, 0, sizeof(verts)/sizeof(verts[0]));

		sogl_end_frame();
	}

	sogl_term();
	return 0;
}
