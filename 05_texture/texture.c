#include <stddef.h>
#include <cglm/cglm.h>
#include <sdl2_opengl.h>
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>


const GLchar* const vs_src =
"#version 130\n"
"in vec3 pos;\n"
"in vec3 rgb;\n"
"in vec2 uv;\n"
"out vec4 frag_color;\n"
"out vec2 frag_uv;"
"void main()\n"
"{\n"
"	gl_Position = vec4(pos, 1.0);\n"
"	frag_color = vec4(rgb, 1.0);\n"
"	frag_uv = uv;\n"
"}\n";

const GLchar* const fs_src =
"#version 130\n"
"in vec4 frag_color;\n"
"in vec2 frag_uv;\n"
"out vec4 outcolor;\n"
"uniform sampler2D texture_data;\n"
"void main()\n"
"{\n"
"	outcolor = frag_color * texture(texture_data, frag_uv);\n"
"}\n";


struct vertex_data {
	vec3 pos;
	vec3 rgb;
	vec2 uv;
};


static bool load_texture(void)
{
	int tex_width, tex_height, tex_channels;
	stbi_set_flip_vertically_on_load(true);
	unsigned char* const tex_data = stbi_load("tex.png", &tex_width, &tex_height, &tex_channels, 0);

	if (tex_data == NULL) {
		fprintf(stderr, "Couldn't load texture image\n");
		return false;
	}

	GLuint gl_tex_id;

	glGenTextures(1, &gl_tex_id);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, gl_tex_id);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, tex_width, tex_height, 0, GL_RGB, GL_UNSIGNED_BYTE, tex_data);
	glGenerateMipmap(GL_TEXTURE_2D);

	stbi_image_free(tex_data);

	return true;
}


int main(void)
{
	if (!sogl_init("TEXTURE", 800, 600, vs_src, fs_src))
		return EXIT_FAILURE;

	if (!load_texture())
		goto Lload_texture_failed;

	sogl_vattrp("pos", 3, GL_FLOAT, GL_TRUE, sizeof(struct vertex_data), NULL);
	
	sogl_vattrp("rgb", 3, GL_FLOAT, GL_TRUE, sizeof(struct vertex_data),
	                   (void*)offsetof(struct vertex_data, rgb));

	sogl_vattrp("uv", 2, GL_FLOAT, GL_TRUE, sizeof(struct vertex_data),
	                   (void*)offsetof(struct vertex_data, uv));


	struct vertex_data verts[] = {
		{{-0.5, -0.5, 0}, {1, 0, 0}, {  0,  0 }},
		{{ 0.5, -0.5, 0}, {0, 1, 0}, {  1,  0 }},
		{{ 0.5,  0.5, 0}, {0, 0, 1}, {  1,  1 }},
		{{-0.5,  0.5, 0}, {0, 0, 1}, {  0,  1 }}
	};


	glBufferData(GL_ARRAY_BUFFER, sizeof(verts), verts, GL_STATIC_DRAW);

	while (sogl_handle_events()) {
		sogl_begin_frame();
		
		glClearColor(0, 0, 0, 0xFF);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glDrawArrays(GL_QUADS, 0, sizeof(verts)/sizeof(verts[0]));

		sogl_end_frame();
	}

Lload_texture_failed:
	sogl_term();
	return 0;
}
