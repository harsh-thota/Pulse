#pragma once
#include "clayman.hpp"
#include <functional>
#include <string>

class UICard
{
public:
	static void Render(ClayMan* clayMan, std::function<void()> content, uint32_t height = 0, uint32_t padding = 16);
	static void RenderWithBackground(ClayMan* clayMan, std::function<void()> content, Clay_Color backgroundColor = {40, 40, 40, 255}, uint32_t height = 0, uint32_t padding = 16, uint32_t cornerRadius = 16);
	
	// New enhanced card types
	static void RenderMetricCard(ClayMan* clayMan, const std::string& title, const std::string& value, const std::string& subtitle, Clay_Color accentColor, uint32_t height = 120);
	static void RenderSectionCard(ClayMan* clayMan, const std::string& sectionTitle, std::function<void()> content, uint32_t minHeight = 200);
};