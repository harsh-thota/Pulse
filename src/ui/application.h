#pragma once
#include <SDL.h>
#include <SDL_ttf.h>

class Application
{
public:
	bool Initialize();
	void Run();
	void Shutdown();
private:
	SDL_Window* _window = nullptr;
	SDL_Renderer* _renderer = nullptr;
	TTF_Font* bodyFont = nullptr;
};