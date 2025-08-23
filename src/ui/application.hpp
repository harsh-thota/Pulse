#pragma once
#include "sdl2_font.hpp"
#include "clayman.hpp"
#include "../monitoring/data_collector.hpp"
#include "screens/base_screen.hpp"
#include <SDL.h>
#include <SDL_ttf.h>
#include <memory>
#include <string>
#include <unordered_map>

// Forward declaration for Clay integration
extern "C" {
    Clay_Dimensions SDL2_MeasureText(Clay_StringSlice text, Clay_TextElementConfig *config, void *userData);
    void Clay_SDL2_Render(SDL_Renderer *renderer, Clay_RenderCommandArray renderCommands, SDL2_Font *fonts);
}

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
	void RenderUIElements();
	
	// Modern UI methods
	void RenderModernSidebar();
	void RenderScrollableMainContent();
	void RenderModernNavButton(const std::string& icon, const std::string& label, Screen screen, bool isActive);
	
	// Legacy methods for compatibility
	void RenderSidebar();
	void RenderMainContent();
	void RenderNavButton(const std::string& icon, Screen screen, bool isActive);
	
	Screen currentScreen_ = Screen::Performance;
	void SwitchToScreen(Screen screen);

	SDL_Window* window_ = nullptr;
	SDL_Renderer* renderer_ = nullptr;
	TTF_Font* bodyFont_ = nullptr;
	SDL2_Font fonts_[1];
	std::unique_ptr<ClayMan> clayMan_;
	std::unique_ptr<DataCollector> dataCollector_;
	std::unordered_map<Screen, std::unique_ptr<BaseScreen>> screens_;
};