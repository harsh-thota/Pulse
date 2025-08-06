#include "clayman.hpp"
#include "application.hpp"
#include "clay_renderer_SDL2.c"
#include <iomanip>
#include <iostream>
#include <cstdlib>
#include <sstream>

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
		std::cerr << "Failed to initialize system monitory\n";
		return false;
	}

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
		Clay_Vector2 scroll = {};
		while (SDL_PollEvent(&event))
		{
			if (event.type == SDL_QUIT) running = false;
			else if (event.type == SDL_WINDOWEVENT && event.window.event == SDL_WINDOWEVENT_RESIZED)
			{
				int width, height;
				SDL_GetWindowSize(window_, &width, &height);
				clayMan_->updateClayState(width, height, 0, 0, 0, 0, 0.16f, false);
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
	// Main container - full screen
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
	// Sidebar - fixed width, full height, dark background
	Clay_ElementDeclaration sidebarConfig = {};
	sidebarConfig.layout.sizing = clayMan_->expandYfixedX(80);
	sidebarConfig.layout.padding = clayMan_->padAll(8);
	sidebarConfig.layout.layoutDirection = CLAY_TOP_TO_BOTTOM;
	sidebarConfig.layout.childGap = 8;
	sidebarConfig.backgroundColor = { 15, 15, 15, 255 }; // Dark sidebar
	
	clayMan_->element(sidebarConfig, [this]() {
		// Pulse Logo/Title
		Clay_ElementDeclaration logoConfig = {};
		logoConfig.layout.sizing = clayMan_->expandXfixedY(50);
		logoConfig.layout.childAlignment = clayMan_->centerXY();
		
		clayMan_->element(logoConfig, [this]() {
			Clay_TextElementConfig logoTextConfig = {};
			logoTextConfig.textColor = { 0, 255, 150, 255 }; // Green accent
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
	Clay_ElementDeclaration buttonConfig = {};
	buttonConfig.layout.sizing = clayMan_->expandXfixedY(40);
	buttonConfig.layout.childAlignment = clayMan_->centerXY();
	buttonConfig.layout.padding = clayMan_->padAll(4);
	
	// Different colors for active/inactive buttons
	if (isActive) {
		buttonConfig.backgroundColor = { 0, 255, 150, 60 }; // Green background for active
	} else {
		buttonConfig.backgroundColor = { 40, 40, 40, 255 }; // Dark gray for inactive
	}
	
	buttonConfig.cornerRadius = { 4, 4, 4, 4 };
	
	clayMan_->element(buttonConfig, [this, label, isActive]() {
		Clay_TextElementConfig textConfig = {};
		if (isActive) {
			textConfig.textColor = { 0, 255, 150, 255 }; // Green text for active  
		} else {
			textConfig.textColor = { 200, 200, 200, 255 }; // Light gray for inactive
		}
		textConfig.fontId = 0;
		textConfig.fontSize = 10;
		clayMan_->textElement(label, textConfig);
	});
	
	// TODO: Add click handling when implementing mouse input
}

void Application::RenderMainContent()
{
	// Main content area - takes remaining space
	Clay_ElementDeclaration contentConfig = {};
	contentConfig.layout.sizing = clayMan_->expandXY();
	contentConfig.layout.padding = clayMan_->padAll(16);
	contentConfig.backgroundColor = { 25, 25, 25, 255 }; // Slightly lighter than sidebar
	
	clayMan_->element(contentConfig, [this]() {
		// Render different screens based on current selection
		switch (currentScreen_) {
			case Screen::Performance:
				RenderPerformanceScreen();
				break;
			case Screen::Processes:
				RenderProcessesScreen();
				break;
			case Screen::Network:
				RenderNetworkScreen();
				break;
			case Screen::Alerts:
				RenderAlertsScreen();
				break;
		}
	});
}

void Application::RenderPerformanceScreen()
{
	const SystemState& systemState = dataCollector_->GetSystemState();

	// Header
	Clay_ElementDeclaration headerConfig = {};
	headerConfig.layout.sizing = clayMan_->expandXfixedY(40);
	headerConfig.layout.childAlignment = clayMan_->centerXY();

	clayMan_->element(headerConfig, [this, &systemState]() {
		Clay_TextElementConfig headerTextConfig = {};
		headerTextConfig.textColor = { 255, 255, 255, 255 };
		headerTextConfig.fontId = 0;
		headerTextConfig.fontSize = 18;

		std::string headerText = "Performance - " + systemState.cpuName +
			" (" + std::to_string(systemState.coreCount) + " cores)";
		clayMan_->textElement(headerText, headerTextConfig);
		});

	// Main metrics area
	Clay_ElementDeclaration metricsConfig = {};
	metricsConfig.layout.sizing = clayMan_->expandXY();
	metricsConfig.layout.layoutDirection = CLAY_TOP_TO_BOTTOM;
	metricsConfig.layout.childGap = 20;
	metricsConfig.layout.padding = clayMan_->padAll(20);

	clayMan_->element(metricsConfig, [this, &systemState]() {
		// CPU Usage
		Clay_ElementDeclaration cpuConfig = {};
		cpuConfig.layout.sizing = clayMan_->expandXfixedY(60);
		cpuConfig.layout.layoutDirection = CLAY_LEFT_TO_RIGHT;
		cpuConfig.layout.childGap = 20;
		cpuConfig.layout.childAlignment = clayMan_->centerXY();
		cpuConfig.backgroundColor = { 40, 40, 40, 255 };
		cpuConfig.cornerRadius = { 8, 8, 8, 8 };
		cpuConfig.layout.padding = clayMan_->padAll(16);

		clayMan_->element(cpuConfig, [this, &systemState]() {
			// CPU Label
			Clay_ElementDeclaration labelConfig = {};
			labelConfig.layout.sizing = clayMan_->expandYfixedX(100);
			labelConfig.layout.childAlignment = clayMan_->centerXY();

			clayMan_->element(labelConfig, [this]() {
				Clay_TextElementConfig textConfig = {};
				textConfig.textColor = { 255, 255, 255, 255 };
				textConfig.fontId = 0;
				textConfig.fontSize = 14;
				clayMan_->textElement("CPU Usage", textConfig);
				});

			// CPU Value
			Clay_ElementDeclaration valueConfig = {};
			valueConfig.layout.sizing = clayMan_->expandXY();
			valueConfig.layout.childAlignment = clayMan_->centerXY();

			clayMan_->element(valueConfig, [this, &systemState]() {
				Clay_TextElementConfig textConfig = {};
				textConfig.textColor = { 0, 255, 150, 255 }; // Green
				textConfig.fontId = 0;
				textConfig.fontSize = 16;

				std::string cpuText = FormatPercentage(systemState.cpuUsagePercent);
				clayMan_->textElement(cpuText, textConfig);
				});
			});

		// Memory Usage
		Clay_ElementDeclaration memConfig = {};
		memConfig.layout.sizing = clayMan_->expandXfixedY(60);
		memConfig.layout.layoutDirection = CLAY_LEFT_TO_RIGHT;
		memConfig.layout.childGap = 20;
		memConfig.layout.childAlignment = clayMan_->centerXY();
		memConfig.backgroundColor = { 40, 40, 40, 255 };
		memConfig.cornerRadius = { 8, 8, 8, 8 };
		memConfig.layout.padding = clayMan_->padAll(16);

		clayMan_->element(memConfig, [this, &systemState]() {
			// Memory Label
			Clay_ElementDeclaration labelConfig = {};
			labelConfig.layout.sizing = clayMan_->expandYfixedX(100);
			labelConfig.layout.childAlignment = clayMan_->centerXY();

			clayMan_->element(labelConfig, [this]() {
				Clay_TextElementConfig textConfig = {};
				textConfig.textColor = { 255, 255, 255, 255 };
				textConfig.fontId = 0;
				textConfig.fontSize = 14;
				clayMan_->textElement("Memory", textConfig);
				});

			// Memory Value
			Clay_ElementDeclaration valueConfig = {};
			valueConfig.layout.sizing = clayMan_->expandXY();
			valueConfig.layout.childAlignment = clayMan_->centerXY();

			clayMan_->element(valueConfig, [this, &systemState]() {
				Clay_TextElementConfig textConfig = {};
				textConfig.textColor = { 100, 150, 255, 255 }; // Blue
				textConfig.fontId = 0;
				textConfig.fontSize = 16;

				std::string memText = FormatBytes(systemState.usedRAMBytes) + " / " +
					FormatBytes(systemState.totalRAMBytes) + " (" +
					FormatPercentage(systemState.memoryUsagePercent) + ")";
				clayMan_->textElement(memText, textConfig);
				});
			});
		});
}

void Application::RenderProcessesScreen()
{
	// Header
	Clay_ElementDeclaration headerConfig = {};
	headerConfig.layout.sizing = clayMan_->expandXfixedY(40);
	headerConfig.layout.childAlignment = clayMan_->centerXY();
	
	clayMan_->element(headerConfig, [this]() {
		Clay_TextElementConfig headerTextConfig = {};
		headerTextConfig.textColor = { 255, 255, 255, 255 };
		headerTextConfig.fontId = 0;
		headerTextConfig.fontSize = 20;
		clayMan_->textElement("Process Manager", headerTextConfig);
	});
	
	// Placeholder for process table
	Clay_ElementDeclaration processAreaConfig = {};
	processAreaConfig.layout.sizing = clayMan_->expandXY();
	processAreaConfig.layout.childAlignment = clayMan_->centerXY();
	
	clayMan_->element(processAreaConfig, [this]() {
		Clay_TextElementConfig placeholderConfig = {};
		placeholderConfig.textColor = { 150, 150, 150, 255 };
		placeholderConfig.fontId = 0;
		placeholderConfig.fontSize = 14;
		clayMan_->textElement("Process table will appear here", placeholderConfig);
	});
}

void Application::RenderNetworkScreen()
{
	// Header
	Clay_ElementDeclaration headerConfig = {};
	headerConfig.layout.sizing = clayMan_->expandXfixedY(40);
	headerConfig.layout.childAlignment = clayMan_->centerXY();
	
	clayMan_->element(headerConfig, [this]() {
		Clay_TextElementConfig headerTextConfig = {};
		headerTextConfig.textColor = { 255, 255, 255, 255 };
		headerTextConfig.fontId = 0;
		headerTextConfig.fontSize = 20;
		clayMan_->textElement("Network Monitor", headerTextConfig);
	});
	
	// Placeholder for network stats
	Clay_ElementDeclaration networkAreaConfig = {};
	networkAreaConfig.layout.sizing = clayMan_->expandXY();
	networkAreaConfig.layout.childAlignment = clayMan_->centerXY();
	
	clayMan_->element(networkAreaConfig, [this]() {
		Clay_TextElementConfig placeholderConfig = {};
		placeholderConfig.textColor = { 150, 150, 150, 255 };
		placeholderConfig.fontId = 0;
		placeholderConfig.fontSize = 14;
		clayMan_->textElement("Network statistics will appear here", placeholderConfig);
	});
}

void Application::RenderAlertsScreen()
{
	// Header
	Clay_ElementDeclaration headerConfig = {};
	headerConfig.layout.sizing = clayMan_->expandXfixedY(40);
	headerConfig.layout.childAlignment = clayMan_->centerXY();
	
	clayMan_->element(headerConfig, [this]() {
		Clay_TextElementConfig headerTextConfig = {};
		headerTextConfig.textColor = { 255, 255, 255, 255 };
		headerTextConfig.fontId = 0;
		headerTextConfig.fontSize = 20;
		clayMan_->textElement("Alert Manager", headerTextConfig);
	});
	
	// Placeholder for alerts
	Clay_ElementDeclaration alertAreaConfig = {};
	alertAreaConfig.layout.sizing = clayMan_->expandXY();
	alertAreaConfig.layout.childAlignment = clayMan_->centerXY();
	
	clayMan_->element(alertAreaConfig, [this]() {
		Clay_TextElementConfig placeholderConfig = {};
		placeholderConfig.textColor = { 150, 150, 150, 255 };
		placeholderConfig.fontId = 0;
		placeholderConfig.fontSize = 14;
		clayMan_->textElement("Alert rules and notifications will appear here", placeholderConfig);
	});
}

void Application::SwitchToScreen(Screen screen)
{
	currentScreen_ = screen;
}

std::string Application::FormatBytes(uint64_t bytes)
{
	std::ostringstream oss;
	if (bytes >= 1024 * 1024 * 1024)
	{
		oss << std::fixed << std::setprecision(1) << (static_cast<double>(bytes) / (1024 * 1024 * 1024)) << "GB";
	}
	else if (bytes >= 1024 * 1024)
	{
		oss << std::fixed << std::setprecision(1) << (static_cast<double>(bytes) / (1024 * 1024)) << "MB";
	}
	else if (bytes >= 1024)
	{
		oss << std::fixed << std::setprecision(1) << (static_cast<double>(bytes) / (1024)) << "KB";
	}
	else
	{
		oss << bytes << "B";
	}
	return oss.str();
}

std::string Application::FormatPercentage(float percentage)
{
	std::ostringstream oss;
	oss << std::fixed << std::setprecision(1) << percentage << "%";
	return oss.str();
}

void Application::Shutdown()
{

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