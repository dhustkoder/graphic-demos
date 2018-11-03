#include <cstdlib>
#include <iostream>
#include <string>
#include <exception>
#include <memory>
#include <vector>
#include <random>
#include <stdexcept>
#include <SDL2/SDL.h>


#define WIN_WIDTH  (1920)
#define WIN_HEIGHT (1080)
#define MAX_RECTS  (50000)

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
			throw std::runtime_error(std::string("Couldn't initialize SDL: ") + SDL_GetError());
		
		m_window = SDL_CreateWindow("OOP DEMO",
		                          SDL_WINDOWPOS_CENTERED,
					  SDL_WINDOWPOS_CENTERED,
					  WIN_WIDTH, WIN_HEIGHT,
					  SDL_WINDOW_SHOWN);
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


class Rectangle;
class Renderer {
	friend class Rectangle;
public:
	Renderer(const Window& window)
	{
		m_renderer = SDL_CreateRenderer(window.m_window, -1, SDL_RENDERER_ACCELERATED);
		if (m_renderer == nullptr)
			throw std::runtime_error(std::string("Couldn't create renderer: ") + SDL_GetError());
	}

	~Renderer()
	{
		if (m_renderer != nullptr)
			SDL_DestroyRenderer(m_renderer);
	}

	void Clear(const Color color)
	{
		if (SDL_SetRenderDrawColor(m_renderer, color.r, color.g, color.b, 0xFF) < 0)
			throw std::runtime_error(std::string("Couldn't set render draw color: ") + SDL_GetError());
		if (SDL_RenderClear(m_renderer) < 0)
			throw std::runtime_error(std::string("Couldn't clear render: ") + SDL_GetError());
	}

	void Present()
	{
		SDL_RenderPresent(m_renderer);
	}


protected:
	SDL_Renderer* m_renderer = nullptr;
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


class RandomRectangleFactory {
public:
	RandomRectangleFactory() :
		m_distPosX(0, WIN_WIDTH),
		m_distPosY(0, WIN_HEIGHT),
		m_distVel(-6, 6),
		m_distColor(0x0F, 0xFF),
		m_distSize(1, 3)
	{

	}

	std::unique_ptr<Rectangle> Make()
	{
		const int size = m_distSize(m_gen);
		
		int velX = 0;
		int velY = 0;

		while (!velX)
			velX = m_distVel(m_gen);
		while (!velY)
			velY = m_distVel(m_gen);

		return std::make_unique<Rectangle>(
				Vec2i{velX, velY},
				Vec2i{m_distPosX(m_gen), m_distPosY(m_gen)},
				Vec2i{size, size},
				Color{m_distColor(m_gen), m_distColor(m_gen), m_distColor(m_gen)}
			);
	}

private:
	std::default_random_engine m_gen;
	std::uniform_int_distribution<int> m_distPosX;
	std::uniform_int_distribution<int> m_distPosY;
	std::uniform_int_distribution<int> m_distVel;
	std::uniform_int_distribution<Uint8> m_distColor;
	std::uniform_int_distribution<int> m_distSize;
};





int main(void)
{
	try {
		std::unique_ptr<Game> game = std::make_unique<Game>();
		std::vector<std::unique_ptr<Rectangle>> rects;

		RandomRectangleFactory rrf;
		for (int i = 0; i < MAX_RECTS; ++i)
			rects.push_back(rrf.Make());

		while (game->HandleEvents()) {
			game->BeginFrame({0x00, 0x00, 0x00});
			
			for (const auto& rect : rects) {
				const Vec2i pos = rect->GetPos();
				Vec2i vel = rect->GetVel();
				if ((pos.x >= WIN_WIDTH && vel.x > 0) || (pos.x <= 0 && vel.x < 0))
					vel.x = -vel.x;
				if ((pos.y >= WIN_HEIGHT && vel.y > 0) || (pos.y <= 0 && vel.y < 0))
					vel.y = -vel.y;

				rect->SetVel(vel);

				rect->Update();

				rect->Draw(game->GetRenderer());
			}
			
			
			game->EndFrame();
			std::cout << "FPS: " << game->GetFps() << '\n';
		}

	} catch(std::exception& except) {
		std::cout << "Fatal Exception: " << except.what() << std::endl;
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}


