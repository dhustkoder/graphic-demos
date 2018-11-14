#ifndef SDL2_OPENGL_HPP_
#define SDL2_OPENGL_HPP_
#include <SDL2/SDL.h>
#include <GL/glew.h>

#define MAX_VBO_BYTES (1024ll * 1024ll * 8ll) // 8M de VRAM 

class Window;
class Renderer;


struct Color {
	GLfloat r, g, b;
};

struct Vec2f {
	GLfloat x, y;
};


class Window {
	friend class Renderer;
public:
	Window(int width, int height)
	{
		if (SDL_Init(SDL_INIT_VIDEO) < 0)
			throw std::runtime_error(std::string("Couldn't initialize SDL: ") + SDL_GetError());
		
		m_window = SDL_CreateWindow("OOP DEMO",
		                            SDL_WINDOWPOS_CENTERED,
		                            SDL_WINDOWPOS_CENTERED,
		                            width, height,
		                            SDL_WINDOW_SHOWN|SDL_WINDOW_OPENGL);
		if (m_window == nullptr)
			throw std::runtime_error(std::string("Couldn't create window: ") + SDL_GetError());
		
	}

	~Window() 
	{
		if (m_window != nullptr)
			SDL_DestroyWindow(m_window);
		SDL_Quit();
	}

	bool HandleEvents()
	{
		while (SDL_PollEvent(&m_event) != 0) {
			if (m_event.type == SDL_QUIT)
				return false;
		}
		return true;
	}

protected:
	SDL_Window* m_window = nullptr;
	SDL_Event m_event;
};


class ShaderProgram {
public:
	ShaderProgram(const GLchar* vs_src, const GLchar* fs_src)
	{
		m_sp_id = glCreateProgram();
		m_vs_id = glCreateShader(GL_VERTEX_SHADER);
		if (m_vs_id == 0)
			throw std::string("Couldn't create Vertex Shader");
		m_fs_id = glCreateShader(GL_FRAGMENT_SHADER);
		if (m_fs_id == 0) 
			throw std::string("Couldn't create Fragment Shader");

		const auto compile = [](GLuint id, const GLchar* src) {
			// compile shader
			glShaderSource(id, 1, &src, NULL);
			glCompileShader(id);
			
			GLint shader_success;

			glGetShaderiv(id, GL_COMPILE_STATUS, &shader_success);
			if (shader_success == GL_FALSE)
				throw std::string("Couldn't compile Shader\n");
		};

		compile(m_vs_id, vs_src);
		compile(m_fs_id, fs_src);
		glAttachShader(m_sp_id, m_fs_id);
		glAttachShader(m_sp_id, m_vs_id);
		glLinkProgram(m_sp_id);
		glUseProgram(m_sp_id);
	}

	~ShaderProgram() 
	{
		glDetachShader(m_sp_id, m_fs_id);
		glDetachShader(m_sp_id, m_vs_id);
		glDeleteProgram(m_sp_id);
	}

	void VertexAttribPointer(const char* attstr,
	                         GLint size,
	                         GLenum type,
	                         GLsizei stride,
	                         const GLvoid* pointer)
	{
		const GLint index = glGetAttribLocation(m_sp_id, attstr);
		glEnableVertexAttribArray(index);
		glVertexAttribPointer(index, size, type, GL_TRUE, stride, pointer);
	}

private:
	GLuint m_vs_id = 0, m_fs_id = 0;
	GLuint m_sp_id;
};


class Renderer {
public:
	Renderer(const Window& window) :
		m_window(window)
	{
		if (SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1) < 0)
			throw std::string("Couldn't set GL DOUBLE BUFFER");

		m_glcontext = SDL_GL_CreateContext(window.m_window);
		if (m_glcontext == NULL)
			throw std::string("Couldn't create GL Context: ") +  SDL_GetError();

		GLenum err;
		if ((err = glewInit()) != GLEW_OK)
			throw std::string("GLEW Error: ") + reinterpret_cast<const char*>(glewGetErrorString(err));

		glGenVertexArrays(1, &m_vao);
		glGenBuffers(1, &m_vbo);
		glBindVertexArray(m_vao);
		glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
		glBufferData(GL_ARRAY_BUFFER, MAX_VBO_BYTES,
		             NULL, GL_DYNAMIC_DRAW);

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

		m_shader = std::make_unique<ShaderProgram>(vs_src, fs_src);
		m_shader->VertexAttribPointer("pos", 2, GL_FLOAT,
		                             sizeof(GLfloat) * 5, NULL);
		m_shader->VertexAttribPointer("rgb", 3, GL_FLOAT,
		                             sizeof(GLfloat) * 5,
		                             (void*)(sizeof(GLfloat) * 2));
		SDL_GL_SetSwapInterval(0);
	}

	~Renderer()
	{
		if (m_vbo != 0)
			glDeleteBuffers(1, &m_vbo);

		if (m_vao != 0)
			glDeleteVertexArrays(1, &m_vao);

		if (m_glcontext != NULL)
			SDL_GL_DeleteContext(m_glcontext);
	}

	void PushVerts(const void* const verts, const int x)
	{
		if (((x * sizeof(GLfloat) * 5) + (m_verts_pushed * sizeof(GLfloat) * 5)) >= MAX_VBO_BYTES) {
			glDrawArrays(GL_QUADS, 0, m_verts_pushed * 4);
			m_verts_pushed = 0;
		}

		glBufferSubData(GL_ARRAY_BUFFER,
		                m_verts_pushed * sizeof(GLfloat) * 5,
		                x * sizeof(GLfloat) * 5, verts);

		m_verts_pushed += x;
	}

	void Clear(const Color color)
	{
		glClearColor(color.r, color.g, color.b, 0xFF);
		glClear(GL_COLOR_BUFFER_BIT);
	}

	void Present()
	{
		glDrawArrays(GL_QUADS, 0, m_verts_pushed * 4);
		SDL_GL_SwapWindow(m_window.m_window);
		m_verts_pushed = 0;
	}


protected:
	const Window& m_window;
	SDL_GLContext m_glcontext = NULL;
	GLuint m_vao = 0, m_vbo = 0;
	long m_verts_pushed = 0;
	std::unique_ptr<ShaderProgram> m_shader;
};


#endif