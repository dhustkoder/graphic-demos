#include <sdl2_opengl/sdl2_opengl.h>


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


int main(void)
{
	if (!sdl2_opengl_init("CUBE", 800, 600, vs_src, fs_src)) {
		return EXIT_FAILURE;
	}


	while (sdl2_opengl_handle_events()) {
		sdl2_opengl_begin_frame();
		
		glClearColor(0, 0, 0, 0xFF);
		glClear(GL_COLOR_BUFFER_BIT);

		sdl2_opengl_end_frame();
	}

	sdl2_opengl_term();
	return 0;
}
