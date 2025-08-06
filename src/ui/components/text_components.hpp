#pragma once
#include "clayman.hpp"
#include <string>

class TextComponents
{
public:
	static void RenderTitle(ClayMan* clayMan, const std::string& title, uint32_t fontSize = 20, Clay_Color color = { 255, 255, 255, 2555 });
	static void RenderSubtitle(ClayMan* clayMan, const std::string& subtitle, uint32_t fontSize = 16, Clay_Color color = { 180, 180, 180, 255 });
	static void RenderLargeValue(ClayMan* clayMan, const std::string& value, Clay_Color color, uint32_t fontSize = 24);
	static void RenderCenteredTitle(ClayMan* clayMan, const std::string& title, uint32_t containerHeight = 40, uint32_t fontSize = 20);
};