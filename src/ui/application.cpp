#include "clayman.hpp"
#include "application.hpp"
#include "clay_renderer_SDL2.c"
#include "screens/performance_screen.hpp"
#include <iostream>

static void HandleClayErrors(Clay_ErrorData errorData)
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

	window_ = SDL_CreateWindow("Pulse", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 800, 600, SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);
	if (!window_)
	{
		std::cerr << "Window could not be created! SDL_Error: " << SDL_GetError() << "\n";
		TTF_Quit(); SDL_Quit();
		return false;
	}

	renderer_ = SDL_CreateRenderer(window_, -1, SDL_RENDERER_SOFTWARE);
	if (!renderer_)
	{
		std::cerr << "Renderer could not be created! SDL_Error: " << SDL_GetError() << "\n";
		SDL_DestroyWindow(window_);
		TTF_Quit(); SDL_Quit();
		return false;
	}

	bodyFont_ = TTF_OpenFont("assets/fonts/Roboto-Regular.ttf", 12);
	if (!bodyFont_)
	{
		std::cerr << "Failed to load font! TTF_Error: " << TTF_GetError() << "\n";
		return false;
	}

	fonts_[0] = { 0, bodyFont_ };

	int w, h;
	SDL_GetWindowSize(window_, &w, &h);
	clayMan_ = std::make_unique<ClayMan>(w, h, SDL2_MeasureText, fonts_);

	dataCollector_ = std::make_unique<DataCollector>();
	if (!dataCollector_->Initialize())
	{
		std::cerr << "Failed to initialize system monitor\n";
		return false;
	}

	// Initialize modular screens
	screens_[Screen::Performance] = std::make_unique<PerformanceScreen>();
	// TODO: Add other screens when implemented
	// screens_[Screen::Processes] = std::make_unique<ProcessesScreen>();
	// screens_[Screen::Network] = std::make_unique<NetworkScreen>();
	// screens_[Screen::Alerts] = std::make_unique<AlertsScreen>();

	std::cout << "Pulse Initialized - Optimized for low memory usage\n";
	return true;
}

void Application::Run()
{
	bool running = true;
	SDL_Event event;

	while (running)
	{
		int mouseX = 0, mouseY = 0;
		bool mousePressed = false;
		Clay_Vector2 scroll = {};

		SDL_GetMouseState(&mouseX, &mouseY);

		while (SDL_PollEvent(&event))
		{
			if (event.type == SDL_QUIT) {
				running = false;
			}
			else if (event.type == SDL_WINDOWEVENT && event.window.event == SDL_WINDOWEVENT_RESIZED)
			{
				int width, height;
				SDL_GetWindowSize(window_, &width, &height);
			}
			else if (event.type == SDL_MOUSEBUTTONDOWN && event.button.button == SDL_BUTTON_LEFT)
			{
				mousePressed = true;
			}
			else if (event.type == SDL_MOUSEWHEEL)
			{
				scroll.y = static_cast<float>(event.wheel.y) * 20.0f;
			}
		}

		int width, height;
		SDL_GetWindowSize(window_, &width, &height);
		clayMan_->updateClayState(width, height, static_cast<float>(mouseX), static_cast<float>(mouseY),
			scroll.x, scroll.y, 0.016f, mousePressed);

		Update();
		Render();
		SDL_Delay(16);
	}
}

void Application::Update()
{
	if (dataCollector_)
	{
		dataCollector_->Update();
	}
}

void Application::Render()
{
	SDL_SetRenderDrawColor(renderer_, 20, 20, 20, 255);
	SDL_RenderClear(renderer_);

	clayMan_->beginLayout();
	RenderUI();
	Clay_RenderCommandArray clayCommands = clayMan_->endLayout();

	Clay_SDL2_Render(renderer_, clayCommands, fonts_);
	SDL_RenderPresent(renderer_);
}

void Application::RenderUI()
{
	Clay_ElementDeclaration rootConfig = {};
	rootConfig.layout.sizing = clayMan_->expandXY();
	rootConfig.layout.layoutDirection = CLAY_LEFT_TO_RIGHT;

	clayMan_->element(rootConfig, [this]() {
		RenderSidebar();
		RenderMainContent();
		});
}

void Application::RenderSidebar()
{
	Clay_ElementDeclaration sidebarConfig = {};
	sidebarConfig.layout.sizing = clayMan_->expandYfixedX(80);
	sidebarConfig.layout.padding = clayMan_->padAll(8);
	sidebarConfig.layout.layoutDirection = CLAY_TOP_TO_BOTTOM;
	sidebarConfig.layout.childGap = 8;
	sidebarConfig.backgroundColor = { 15, 15, 15, 255 };

	clayMan_->element(sidebarConfig, [this]() {
		// Pulse Logo
		Clay_ElementDeclaration logoConfig = {};
		logoConfig.layout.sizing = clayMan_->expandXfixedY(50);
		logoConfig.layout.childAlignment = clayMan_->centerXY();

		clayMan_->element(logoConfig, [this]() {
			Clay_TextElementConfig logoTextConfig = {};
			logoTextConfig.textColor = { 0, 255, 150, 255 };
			logoTextConfig.fontId = 0;
			logoTextConfig.fontSize = 14;
			clayMan_->textElement("Pulse", logoTextConfig);
			});

		// Navigation Buttons
		RenderNavButton("CPU", Screen::Performance, (currentScreen_ == Screen::Performance));
		RenderNavButton("PROC", Screen::Processes, (currentScreen_ == Screen::Processes));
		RenderNavButton("NET", Screen::Network, (currentScreen_ == Screen::Network));
		RenderNavButton("ALERT", Screen::Alerts, (currentScreen_ == Screen::Alerts));
		});
}

void Application::RenderNavButton(const std::string& label, Screen screen, bool isActive)
{
	std::string buttonId = "nav_" + label;

	Clay_ElementDeclaration buttonConfig = {};
	buttonConfig.layout.sizing = clayMan_->expandXfixedY(40);
	buttonConfig.layout.childAlignment = clayMan_->centerXY();
	buttonConfig.layout.padding = clayMan_->padAll(4);

	// Check for interaction
	bool isHovered = clayMan_->pointerOver(buttonId);
	bool isClicked = isHovered && clayMan_->mousePressed();

	if (isClicked && !isActive) {
		SwitchToScreen(screen);
	}

	// Button styling
	if (isActive) {
		buttonConfig.backgroundColor = { 0, 255, 150, 60 };
	}
	else if (isHovered) {
		buttonConfig.backgroundColor = { 60, 60, 60, 255 };
	}
	else {
		buttonConfig.backgroundColor = { 40, 40, 40, 255 };
	}

	buttonConfig.cornerRadius = { 4, 4, 4, 4 };

	clayMan_->element(buttonConfig, [this, label, isActive]() {
		Clay_TextElementConfig textConfig = {};
		if (isActive) {
			textConfig.textColor = { 0, 255, 150, 255 };
		}
		else {
			textConfig.textColor = { 200, 200, 200, 255 };
		}
		textConfig.fontId = 0;
		textConfig.fontSize = 10;
		clayMan_->textElement(label, textConfig);
		});
}

void Application::RenderMainContent()
{
	Clay_ElementDeclaration contentConfig = {};
	contentConfig.layout.sizing = clayMan_->expandXY();
	contentConfig.backgroundColor = { 25, 25, 25, 255 };

	clayMan_->element(contentConfig, [this]() {
		// Use modular screen system
		auto screenIt = screens_.find(currentScreen_);
		if (screenIt != screens_.end() && screenIt->second) {
			const SystemState& systemState = dataCollector_->GetSystemState();
			screenIt->second->Render(clayMan_.get(), systemState);
		}
		else {
			// Fallback for unimplemented screens
			Clay_ElementDeclaration fallbackConfig = {};
			fallbackConfig.layout.sizing = clayMan_->expandXY();
			fallbackConfig.layout.childAlignment = clayMan_->centerXY();

			clayMan_->element(fallbackConfig, [this]() {
				Clay_TextElementConfig textConfig = {};
				textConfig.textColor = { 150, 150, 150, 255 };
				textConfig.fontId = 0;
				textConfig.fontSize = 14;
				clayMan_->textElement("Screen not implemented yet", textConfig);
				});
		}
		});
}

void Application::SwitchToScreen(Screen screen)
{
	currentScreen_ = screen;
}

void Application::Shutdown()
{
	screens_.clear();
	clayMan_.reset();

	if (bodyFont_)
	{
		TTF_CloseFont(bodyFont_);
		bodyFont_ = nullptr;
	}
	if (renderer_)
	{
		SDL_DestroyRenderer(renderer_);
		renderer_ = nullptr;
	}
	if (window_)
	{
		SDL_DestroyWindow(window_);
		window_ = nullptr;
	}

	TTF_Quit();
	SDL_Quit();
	std::cout << "Pulse Application Shutdown Complete.\n";
}