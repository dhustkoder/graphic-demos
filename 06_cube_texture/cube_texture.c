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
	if (!sdl2_opengl_init("CUBE TEXTURE", 800, 600, vs_src, fs_src))
		return EXIT_FAILURE;

	if (!load_texture())
		goto Lload_texture_failed;

	sdl2_opengl_vattrp("pos", 3, GL_FLOAT, GL_TRUE, sizeof(struct vertex_data), NULL);
	
	sdl2_opengl_vattrp("rgb", 3, GL_FLOAT, GL_TRUE, sizeof(struct vertex_data),
	                   (void*)offsetof(struct vertex_data, rgb));

	sdl2_opengl_vattrp("uv", 2, GL_FLOAT, GL_TRUE, sizeof(struct vertex_data),
	                   (void*)offsetof(struct vertex_data, uv));


	struct vertex_data verts[] = {
		/* FRONT */
		{{ -0.5, -0.5, -0.5 }, {1, 1, 1}, {0, 0}},
		{{  0.5, -0.5, -0.5 }, {1, 1, 1}, {1, 0}},
		{{  0.5,  0.5, -0.5 }, {1, 1, 1}, {1, 1}},
		{{ -0.5,  0.5, -0.5 }, {1, 1, 1}, {0, 1}},

		/* BACK */
		{{ -0.5, -0.5,  0.5 }, {1, 0, 0}, {0, 0}},
		{{  0.5, -0.5,  0.5 }, {1, 0, 0}, {1, 0}},
		{{  0.5,  0.5,  0.5 }, {1, 0, 0}, {1, 1}},
		{{ -0.5,  0.5,  0.5 }, {1, 0, 0}, {0, 1}},

		/* RIGHT */
		{{ -0.5, -0.5,  0.5 }, {0, 1, 0}, {0, 0}},
		{{ -0.5, -0.5, -0.5 }, {0, 1, 0}, {1, 0}},
		{{ -0.5,  0.5, -0.5 }, {0, 1, 0}, {1, 1}},
		{{ -0.5,  0.5,  0.5 }, {0, 1, 0}, {0, 1}},

		/* LEFT */
		{{  0.5, -0.5,  0.5 }, {0, 0, 1}, {0, 0}},
		{{  0.5, -0.5, -0.5 }, {0, 0, 1}, {1, 0}},
		{{  0.5,  0.5, -0.5 }, {0, 0, 1}, {1, 1}},
		{{  0.5,  0.5,  0.5 }, {0, 0, 1}, {0, 1}},

		/* UP */
		{{ -0.5,  0.5,  0.5 }, {1, 0, 1}, {0, 0}},
		{{  0.5,  0.5,  0.5 }, {1, 0, 1}, {1, 0}},
		{{  0.5,  0.5, -0.5 }, {1, 0, 1}, {1, 1}},
		{{ -0.5,  0.5, -0.5 }, {1, 0, 1}, {0, 1}},

		/* DOWN */
		{{ -0.5, -0.5,  0.5 }, {0, 1, 1}, {0, 0}},
		{{  0.5, -0.5,  0.5 }, {0, 1, 1}, {1, 0}},
		{{  0.5, -0.5, -0.5 }, {0, 1, 1}, {1, 1}},
		{{ -0.5, -0.5, -0.5 }, {0, 1, 1}, {0, 1}},
	};

	/* Our rotation matrix is set to rotate 1 degree
	 * */
	mat4 rotation_matrix = GLM_MAT4_IDENTITY_INIT;
	glm_rotate(rotation_matrix, glm_rad(1), (vec3){0.35, 1, 0});


	while (sdl2_opengl_handle_events()) {
		sdl2_opengl_begin_frame();
		
		glClearColor(0, 0, 0, 0xFF);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		/* Rotation is done by applying the 
		 * transformation to all vertices/vectors of the object
		 * this will rotate the triangle 1 degree per frame
		 * */
		for (int i = 0; i < (sizeof(verts)/sizeof(verts[0])); ++i)
			glm_vec_rotate_m4(rotation_matrix, verts[i].pos, verts[i].pos);

		glBufferData(GL_ARRAY_BUFFER, sizeof(verts), verts, GL_STREAM_DRAW);
		glDrawArrays(GL_QUADS, 0, sizeof(verts)/sizeof(verts[0]));

		sdl2_opengl_end_frame();
	}
Lload_texture_failed:
	sdl2_opengl_term();
	return 0;
}
