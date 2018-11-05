#include <cstdlib>
#include <iostream>
#include <string>
#include <exception>
#include <memory>
#include <vector>
#include <random>
#include <stdexcept>
#include <SDL2/SDL.h>
#include <GL/glew.h>


#define WIN_WIDTH  (1280)
#define WIN_HEIGHT (720)
#define MAX_VBO_BYTES (1024 * 1024) // 1G de VRAM 

struct Color {
	GLfloat r, g, b;
};

struct Vec2f {
	GLfloat x, y;
};



class Renderer;
class Window {
	friend class Renderer;
public:
	Window()
	{
		if (SDL_Init(SDL_INIT_VIDEO) < 0)
			throw std::runtime_error(std::string("Couldn't initialize SDL: ") + SDL_GetError());
		
		m_window = SDL_CreateWindow("OOP DEMO",
		                          SDL_WINDOWPOS_CENTERED,
					  SDL_WINDOWPOS_CENTERED,
					  WIN_WIDTH, WIN_HEIGHT,
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


class Shader {
public:
	enum class Type
	{
		Vertex,
		Fragment
	};

	Shader(Type type, const GLchar* source) :
		m_type(type)
	{
		if (s_sp_id == 0) {
			s_sp_id = glCreateProgram();
			if (s_sp_id == 0)
				throw std::string("Couldn't create GL Program");
		}

		if (m_type == Type::Vertex) {
			m_id = glCreateShader(GL_VERTEX_SHADER);
			if (m_id == 0)
				throw std::string("Couldn't create Vertex Shader");
		} else if (m_type == Type::Fragment) {
			m_id = glCreateShader(GL_FRAGMENT_SHADER);
			if (m_id == 0) 
				throw std::string("Couldn't create Fragment Shader");
		}

		// compile shader
		glShaderSource(m_id, 1, &source, NULL);
		glCompileShader(m_id);
		
		GLint shader_success;

		glGetShaderiv(m_id, GL_COMPILE_STATUS, &shader_success);
		if (shader_success == GL_FALSE)
			throw std::string("Couldn't compile Shader\n");
	}

	~Shader() 
	{
		if (m_attached) {
			this->Detach();
			glDeleteShader(m_id);
		}

		if (s_shaders_attached == 0 && s_sp_id != 0) {
			glDeleteProgram(s_sp_id);
			s_sp_id = 0;
		}
	}

	void Attach()
	{
		glAttachShader(s_sp_id, m_id);
		m_attached = true;
		++s_shaders_attached;
	}

	void Detach()
	{
		glDetachShader(s_sp_id, m_id);
		m_attached = false;
		--s_shaders_attached;
	}

	static void VertexAttribPointer(const char* attstr,
	                         GLint size,
	                         GLenum type,
	                         GLsizei stride,
	                         const GLvoid* pointer)
	{
		const GLint index = glGetAttribLocation(s_sp_id, attstr);
		glEnableVertexAttribArray(index);
		glVertexAttribPointer(index, size, type, GL_TRUE, stride, pointer);
	}

	static void LinkAndUse()
	{
		glLinkProgram(s_sp_id);
		glUseProgram(s_sp_id);
	}


private:
	Type m_type;
	GLuint m_id = 0;
	bool m_attached = false;
	static GLuint s_sp_id;
	static long s_shaders_attached;
};

GLuint Shader::s_sp_id = 0;
long Shader::s_shaders_attached = 0;



class Rectangle;
class Renderer {
	friend class Rectangle;
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

		m_vertex_shader = std::make_unique<Shader>(Shader::Type::Vertex, vs_src);
		m_frag_shader = std::make_unique<Shader>(Shader::Type::Fragment, fs_src);
		m_vertex_shader->Attach();
		m_frag_shader->Attach();
		Shader::LinkAndUse();

		Shader::VertexAttribPointer("pos", 2, GL_FLOAT,
		                             sizeof(GLfloat) * 5, NULL);
		Shader::VertexAttribPointer("rgb", 3, GL_FLOAT,
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
	std::unique_ptr<Shader> m_vertex_shader;
	std::unique_ptr<Shader> m_frag_shader;

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
};


class Game final : public Window, public Renderer {
public:
	Game() : Window(), Renderer(static_cast<Window&>(*this))
	{
		m_secondsClock = SDL_GetTicks();
	}

	void BeginFrame(const Color clearColor)
	{
		Renderer::Clear(clearColor);
		m_frameBegin = SDL_GetTicks();
	}

	void EndFrame()
	{
		const Uint32 frameEnd = SDL_GetTicks();
		const Uint32 frameTimeElapsed = (frameEnd - m_frameBegin);

		if (m_vsync && frameTimeElapsed < 16)
			SDL_Delay(16 - frameTimeElapsed);

		++m_frameCnt;
		if ((frameEnd - m_secondsClock) > 1000) {
			m_fps = m_frameCnt;
			m_frameCnt = 0;
			m_secondsClock = SDL_GetTicks();
		}
		
		Renderer::Present();
	}

	Uint32 GetFps() 
	{
		return m_fps;
	}

	Renderer& GetRenderer()
	{
		return static_cast<Renderer&>(*this);
	}

	void setVSync(bool active)
	{
		m_vsync = active;
	}

private:
	SDL_Event m_event;
	Uint32 m_frameBegin;
	Uint32 m_secondsClock;
	Uint32 m_frameCnt;
	Uint32 m_fps = 0;
	bool m_vsync = false;
};


class Rectangle {
public:
	Rectangle(const Vec2f vel, const Vec2f pos, const Vec2f size, const Color color) :
		m_vel(vel), m_pos(pos), m_size(size), m_color(color)
	{
	
	}

	void Draw(Renderer& render)
	{
		struct Vertex {
			Vec2f pos;
			Color color;
		} verts[4] = {
			{
				{m_pos.x - m_size.x, m_pos.y - m_size.y},
				m_color
			},
			{
				{m_pos.x + m_size.x, m_pos.y - m_size.y},
				m_color.r, m_color.g, m_color.b 
			},
			{
				{m_pos.x + m_size.x, m_pos.y + m_size.y},
				m_color
			},
			{
				{m_pos.x - m_size.x, m_pos.y + m_size.y},
				m_color
			},
		};
		render.PushVerts(static_cast<const void*>(verts), 4);
	}

	void Update()
	{
		m_pos.x += m_vel.x;
		m_pos.y += m_vel.y;
	}

	Vec2f GetPos()
	{
		return m_pos;
	}

	void SetPos(const Vec2f newPos)
	{
		m_pos = newPos;
	}

	Vec2f GetVel()
	{
		return m_vel;
	}

	void SetVel(const Vec2f newVel)
	{
		m_vel = newVel;
	}

private:
	Vec2f m_vel;
	Vec2f m_pos;
	Vec2f m_size;
	Color m_color;
};


class RandomRectangleFactory {
public:
	RandomRectangleFactory() :
		m_gen(m_rd()),
		m_distPosX(-0.00005, 0.00005),
		m_distPosY(-0.00005, 0.00005),
		m_distVel(-0.0015, 0.0015),
		m_distColor(0.1, 1.0),
		m_distSize(0.0009, 0.0022)
	{

	}

	Rectangle Make()
	{
		const GLfloat size = m_distSize(m_gen);
		return Rectangle(
				Vec2f{m_distVel(m_gen), m_distVel(m_gen)},
				Vec2f{m_distPosX(m_gen), m_distPosY(m_gen)},
				Vec2f{size, size},
				Color{m_distColor(m_gen),
					  m_distColor(m_gen),
					  m_distColor(m_gen)}
		);
	}

private:
	std::random_device m_rd;
	std::mt19937 m_gen;
	std::uniform_real_distribution<GLfloat> m_distPosX;
	std::uniform_real_distribution<GLfloat> m_distPosY;
	std::uniform_real_distribution<GLfloat> m_distVel;
	std::uniform_real_distribution<GLfloat> m_distColor;
	std::uniform_real_distribution<GLfloat> m_distSize;
};





int main(int, char**)
{
	try {
		std::unique_ptr<Game> game = std::make_unique<Game>();
		std::vector<Rectangle> rects;
		RandomRectangleFactory rrf;

		while (game->HandleEvents()) {
			game->BeginFrame({0x00, 0x00, 0x00});
			
			for (auto& rect : rects) {
				const Vec2f pos = rect.GetPos();
				Vec2f vel = rect.GetVel();
				if ((pos.x < -1 && vel.x < 0) || (pos.x > 1 && vel.x > 0))
					vel.x = -vel.x;
				if ((pos.y < -1 && vel.y < 0) || (pos.y > 1 && vel.y > 0))
					vel.y = -vel.y;

				rect.SetVel(vel);

				rect.Update();

				rect.Draw(game->GetRenderer());
			}
			
			
			game->EndFrame();

			if (game->GetFps() > 60) {
				for (int i = 0; i < 50; ++i)
					rects.push_back(rrf.Make());
			} else if (game->GetFps() < 59 && rects.size() > 0) {
				rects.pop_back();
			}

			std::cout << "FPS: " << game->GetFps() << '\n';
			std::cout << "RECTS: " << rects.size() << '\n';
		}

	} catch(std::exception& except) {
		std::cout << "Fatal Exception: " << except.what() << std::endl;
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}


