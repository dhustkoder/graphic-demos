#include <cstdlib>
#include <iostream>
#include <string>
#include <exception>
#include <memory>
#include <vector>
#include <random>
#include <SDL2/SDL.h>


struct Color {
	Uint8 r, g, b;
};

struct Vec2f {
	float x, y;
};

struct Vec2i {
	Sint32 x, y;
};


class Renderer;
class Window {
	friend class Renderer;
public:
	Window()
	{
		if (SDL_Init(SDL_INIT_VIDEO) < 0)
			throw std::string("Couldn't initialize SDL: ") + SDL_GetError();
		
		m_window = SDL_CreateWindow("OOP DEMO",
		                          SDL_WINDOWPOS_CENTERED,
					  SDL_WINDOWPOS_CENTERED,
					  800, 600,
					  SDL_WINDOW_SHOWN);
		if (m_window == nullptr)
			throw std::string("Couldn't create window: ") + SDL_GetError();
		
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


class Rectangle;
class Renderer {
	friend class Rectangle;
public:
	Renderer(const Window& window)
	{
		m_renderer = SDL_CreateRenderer(window.m_window, -1, SDL_RENDERER_ACCELERATED);
		if (m_renderer == nullptr)
			throw std::string("Couldn't create renderer: ") + SDL_GetError();
		SDL_SetRenderDrawBlendMode(m_renderer, SDL_BLENDMODE_NONE); 
	}

	~Renderer()
	{
		if (m_renderer != nullptr)
			SDL_DestroyRenderer(m_renderer);
	}

	void Clear(const Color color)
	{
		SDL_SetRenderDrawColor(m_renderer, color.r, color.g, color.b, 0xFF);
		SDL_RenderClear(m_renderer);
	}

	void Present()
	{
		SDL_RenderPresent(m_renderer);
	}


protected:
	SDL_Renderer* m_renderer = nullptr;
};


class Rectangle {
public:
	Rectangle(const Vec2i vel, const Vec2i pos, const Vec2i size, const Color color) :
		m_vel(vel), m_pos(pos), m_size(size), m_color(color)
	{
	
	}

	void Draw(const Renderer& render)
	{
		SDL_Rect rect;
		rect.x = m_pos.x;
		rect.y = m_pos.y;
		rect.w = m_size.x;
		rect.h = m_size.y;
		SDL_SetRenderDrawColor(render.m_renderer, m_color.r, m_color.g, m_color.b, 0xFF);
		SDL_RenderFillRect(render.m_renderer, &rect);
	}

	void Update()
	{
		m_pos.x += m_vel.x;
		m_pos.y += m_vel.y;
	}

	Vec2i GetPos()
	{
		return m_pos;
	}

	void SetPos(const Vec2i newPos)
	{
		m_pos = newPos;
	}

	Vec2i GetVel()
	{
		return m_vel;
	}

	void SetVel(const Vec2i newVel)
	{
		m_vel = newVel;
	}

private:
	Vec2i m_vel;
	Vec2i m_pos;
	Vec2i m_size;
	Color m_color;
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



int main(void)
{
	try {
		std::unique_ptr<Game> game = std::make_unique<Game>();
		std::vector<Rectangle> rects;

		std::default_random_engine gen;
		std::uniform_int_distribution<int> distPosX(0, 800);
		std::uniform_int_distribution<int> distPosY(0, 600);
		std::uniform_int_distribution<int> distVelX(-3, 3);
		std::uniform_int_distribution<int> distVelY(-3, 3);

		for (int i = 0; i < 55500; ++i) {
			Rectangle rect({distVelX(gen), distVelY(gen)},
			          {distPosX(gen), distPosY(gen)},
			          {1, 1},
			          {0xFF, 0x00, 0x00});
			rects.push_back(rect);
		}

		while (game->HandleEvents()) {
			game->BeginFrame({0xDE, 0xDE, 0xDE});
			
			for (Rectangle& rect : rects) {
				const Vec2i pos = rect.GetPos();
				Vec2i vel = rect.GetVel();
				if ((pos.x >= 800 && vel.x > 0) || (pos.x <= 0 && vel.x < 0))
					vel.x = -vel.x;
				if ((pos.y >= 600 && vel.y > 0) || (pos.y <= 0 && vel.y < 0))
					vel.y = -vel.y;

				rect.SetVel(vel);

				rect.Update();

				rect.Draw(game->GetRenderer());
			}
			
			
			game->EndFrame();
			std::cout << "FPS: " << game->GetFps() << '\n';
		}

	} catch(std::exception& except) {
		std::cout << "Fatal Exception: " << except.what();
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}


