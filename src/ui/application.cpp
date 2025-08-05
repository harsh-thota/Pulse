#include <iostream>
#include <cstdlib>
#include "application.hpp"
#include "clay_renderer_SDL2.c"

void HandleClayErrors(Clay_ErrorData errorData)
{
	std::cerr << "[Clay Error]: " << errorData.errorText.chars << "\n";
}

bool Application::Initialize()
{
	if (SDL_Init(SDL_INIT_VIDEO) < 0)
	{
		std::cerr << "SDL could not initialize! SDL_Error: " << SDL_GetError() << "\n";
		return false;
	}

	if (TTF_Init() == -1)
	{
		std::cerr << "SDL_ttf could not initialize! TTF_Error: " << TTF_GetError() << "\n";
		SDL_Quit();
		return false;
	}

	_window = SDL_CreateWindow("Pulse", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 1200, 800, SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);
	if (!_window)
	{
		std::cerr << "Window could not be created! SDL_Error: " << SDL_GetError() << "\n";
		TTF_Quit(); SDL_Quit();
		return false;
	}

	_renderer = SDL_CreateRenderer(_window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
	if (!_renderer)
	{
		std::cerr << "Renderer could not be created! SDL_Error: " << SDL_GetError() << "\n";
		SDL_DestroyWindow(_window);
		TTF_Quit(); SDL_Quit();
		return false;
	}

	_bodyFont = TTF_OpenFont("assets/fonts/Roboto-Regular.ttf", 16);
	if (!_bodyFont)
	{
		std::cerr << "Failed to load font! TTF_Error: " << TTF_GetError() << "\n";
		return false;
	}

	_fonts[0] = { 0, _bodyFont };

	int w, h;
	SDL_GetWindowSize(_window, &w, &h);
	_clayMan = std::make_unique<ClayMan>(w, h, SDL2_MeasureText, _fonts);

	return true;
}

void Application::Run()
{
	bool running = true;
	SDL_Event event;

	while (running)
	{
		int mouseX = 0, mouseY = 0;
		Clay_Vector2 scroll = {};
		while (SDL_PollEvent(&event))
		{
			if (event.type == SDL_QUIT) running = false;
			else if (event.type == SDL_WINDOWEVENT && event.window.event == SDL_WINDOWEVENT_RESIZED)
			{
				int width, height;
				SDL_GetWindowSize(_window, &width, &height);
				_clayMan->updateClayState(width, height, 0, 0, 0, 0, 0.16f, false);
			}
		}

		Update();
		Render();

		SDL_Delay(16);

	}
}

void Application::Update()
{
	// Update logic goes here
}

void Application::Render()
{
	SDL_SetRenderDrawColor(_renderer, 20, 20, 20, 255);
	SDL_RenderClear(_renderer);

	_clayMan->beginLayout();
	RenderUI();
	Clay_RenderCommandArray clayCommands = _clayMan->endLayout();

	Clay_SDL2_Render(_renderer, clayCommands, _fonts);

	SDL_RenderPresent(_renderer);
}

void Application::RenderUI()
{
	Clay_ElementDeclaration rootConfig = {};
	rootConfig.layout.sizing = _clayMan->expandXY();
	rootConfig.layout.padding = _clayMan->padAll(20);
	rootConfig.layout.childAlignment = _clayMan->centerXY();
	rootConfig.layout.layoutDirection = CLAY_TOP_TO_BOTTOM;
	rootConfig.layout.childGap = 20;
	
	_clayMan->element(rootConfig, [this]() {
		Clay_ElementDeclaration headerConfig = {};
		headerConfig.layout.sizing = _clayMan->expandXfixedY(60);
		headerConfig.layout.childAlignment = _clayMan->centerXY();
		
		_clayMan->element(headerConfig, [this]() {
			Clay_TextElementConfig headerTextConfig = {};
			headerTextConfig.textColor = { 255, 255, 255, 255 };
			headerTextConfig.fontId = 0;
			headerTextConfig.fontSize = 24;

			_clayMan->textElement("Pulse - System Monitor", headerTextConfig);
		});

		Clay_ElementDeclaration contentConfig = {};
		contentConfig.layout.sizing = _clayMan->expandXY();
		contentConfig.layout.layoutDirection = CLAY_LEFT_TO_RIGHT;
		contentConfig.layout.childGap = 40;
		
		_clayMan->element(contentConfig, [this]() {
			Clay_ElementDeclaration overviewConfig = {};
			overviewConfig.layout.sizing = _clayMan->expandXY();
			overviewConfig.layout.childAlignment = _clayMan->centerXY();
			
			_clayMan->element(overviewConfig, [this]() {
				Clay_TextElementConfig textConfig = {};
				textConfig.textColor = { 255, 255, 255, 255 };
				textConfig.fontId = 0;
				textConfig.fontSize = 18;

				_clayMan->textElement("System Overview", textConfig);
			});

			Clay_ElementDeclaration metricsConfig = {};
			metricsConfig.layout.sizing = _clayMan->expandXY();
			metricsConfig.layout.childAlignment = _clayMan->centerXY();
			
			_clayMan->element(metricsConfig, [this]() {
				Clay_TextElementConfig textConfig = {};
				textConfig.textColor = { 255, 255, 255, 255 };
				textConfig.fontId = 0;
				textConfig.fontSize = 18;

				_clayMan->textElement("Detailed Metrics", textConfig);
			});
		});
	});
}

void Application::Shutdown()
{

	_clayMan.reset();
	if (_bodyFont)
	{
		TTF_CloseFont(_bodyFont);
		_bodyFont = nullptr;
	}
	if (_renderer)
	{
		SDL_DestroyRenderer(_renderer);
		_renderer = nullptr;
	}
	if (_window)
	{
		SDL_DestroyWindow(_window);
		_window = nullptr;
	}

	TTF_Quit();
	SDL_Quit();

	std::cout << "Pulse Application Shutdown Complete.\n";
}