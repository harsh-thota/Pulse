#pragma once
#include <SDL.h>
#include <SDL_ttf.h>
#include <memory>
#include "sdl2_font.hpp"
#include "clayman.hpp"

class Application
{
public:
	bool Initialize();
	void Run();
	void Shutdown();
private:
	void Update();
	void Render();
	void RenderUI();

	SDL_Window* _window = nullptr;
	SDL_Renderer* _renderer = nullptr;
	TTF_Font* _bodyFont = nullptr;
	SDL2_Font _fonts[1];
	std::unique_ptr<ClayMan> _clayMan;
};