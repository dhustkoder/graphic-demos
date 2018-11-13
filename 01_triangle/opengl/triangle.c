#include <cglm/cglm.h>
#include <sdl2_opengl/sdl2_opengl.h>


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
	vec3 pos;
	vec3 rgb;
};


int main(void)
{
	if (!sdl2_opengl_init("TRIANGLE", 800, 600, vs_src, fs_src)) {
		return EXIT_FAILURE;
	}

	sdl2_opengl_vattrp("pos", 3, GL_FLOAT, GL_TRUE, sizeof(struct vertex_data), NULL);
	sdl2_opengl_vattrp("rgb", 3, GL_FLOAT, GL_TRUE, sizeof(struct vertex_data),
	                   (void*)((struct vertex_data*)NULL)->rgb);

	/* define the vertices of our triangle (3 points)
	 * */
	struct vertex_data verts[] = {
		{{-0.5, -0.5, 0}, {1, 0, 0}}, // down left  point
		{{ 0.5, -0.5, 0}, {0, 1, 0}}, // down right point
		{{ 0.0,  0.5, 0}, {0, 0, 1}}, // up         point
	};

	/* send to GPU RAM
	 * */
	glBufferData(GL_ARRAY_BUFFER, sizeof(verts), verts, GL_STATIC_DRAW);

	while (sdl2_opengl_handle_events()) {
		sdl2_opengl_begin_frame();
		
		glClearColor(0, 0, 0, 0xFF);
		glClear(GL_COLOR_BUFFER_BIT);

		/* Draw the triangle from GPU RAM to Screen
		 * */
		glDrawArrays(GL_TRIANGLES, 0, sizeof(verts)/sizeof(verts[0]));

		sdl2_opengl_end_frame();
	}

	sdl2_opengl_term();
	return 0;
}
