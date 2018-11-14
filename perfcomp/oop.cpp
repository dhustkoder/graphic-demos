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
#include <sdl2_opengl.hpp>


#define WIN_WIDTH  (1280)
#define WIN_HEIGHT (720)


class Game final : public Window, public Renderer {
public:
	Game() : Window(WIN_WIDTH, WIN_HEIGHT), Renderer(static_cast<Window&>(*this))
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


