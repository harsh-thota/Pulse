#pragma once
#include "sdl2_font.hpp"
#include "clayman.hpp"
#include "../monitoring/data_collector.hpp"
#include <SDL.h>
#include <SDL_ttf.h>
#include <memory>
#include <string>

enum class Screen {
	Performance,
	Processes, 
	Network,
	Alerts
};

class Application
{
public:
	bool Initialize();
	void Run();
	void Shutdown();

	static std::string FormatBytes(uint64_t bytes);
	static std::string FormatPercentage(float percentage);
private:
	void Update();
	void Render();
	void RenderUI();
	void RenderSidebar();
	void RenderMainContent();
	void RenderNavButton(const std::string& label, Screen screen, bool isActive);
	
	void RenderPerformanceScreen();
	void RenderProcessesScreen();
	void RenderNetworkScreen();
	void RenderAlertsScreen();
	
	Screen currentScreen_ = Screen::Performance;
	void SwitchToScreen(Screen screen);

	SDL_Window* window_ = nullptr;
	SDL_Renderer* renderer_ = nullptr;
	TTF_Font* bodyFont_ = nullptr;
	SDL2_Font fonts_[1];
	std::unique_ptr<ClayMan> clayMan_;
	std::unique_ptr<DataCollector> dataCollector_;
};