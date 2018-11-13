#include <cglm/cglm.h>
#include <sdl2_opengl/sdl2_opengl.h>


const GLchar* const vs_src =
"#version 130\n"
"in vec3 pos;\n"
"in vec3 rgb;\n"
"out vec4 frag_color;\n"
"uniform mat4 trans;\n"
"void main()\n"
"{\n"
"	gl_Position = trans * vec4(pos, 1.0);\n"
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
	if (!sdl2_opengl_init("ROTATE", 800, 600, vs_src, fs_src)) {
		return EXIT_FAILURE;
	}

	sdl2_opengl_vattrp("pos", 3, GL_FLOAT, GL_TRUE, sizeof(struct vertex_data), NULL);
	sdl2_opengl_vattrp("rgb", 3, GL_FLOAT, GL_TRUE, sizeof(struct vertex_data),
	                   (void*)((struct vertex_data*)NULL)->rgb);


	mat4 trans = GLM_MAT4_IDENTITY_INIT;

	sdl2_opengl_set_uniform("trans", trans);

	struct vertex_data verts[] = {
		{{-0.5, -0.5, 0}, {1, 0, 1}},
		{{ 0.5, -0.5, 0}, {1, 0, 1}},
		{{ 0.5,  0.5, 0}, {1, 0, 1}},
		{{-0.5,  0.5, 0}, {1, 0, 1}}
	};


	while (sdl2_opengl_handle_events()) {
		sdl2_opengl_begin_frame();
		
		glClearColor(0, 0, 0, 0xFF);
		glClear(GL_COLOR_BUFFER_BIT);

		glm_rotate_z(trans, 0.05, trans);
		sdl2_opengl_set_uniform("trans", trans);

		glBufferData(GL_ARRAY_BUFFER, sizeof(verts), verts, GL_STREAM_DRAW);
		glDrawArrays(GL_QUADS, 0, sizeof(verts)/sizeof(verts[0]));

		sdl2_opengl_end_frame();
	}

	sdl2_opengl_term();
	return 0;
}
