#pragma once
#include "clayman.hpp"
#include <functional>

class UICard
{
public:
	static void Render(ClayMan* clayMan, std::function<void()> content, uint32_t height = 0, uint32_t padding = 16);
	static void RenderWithBackground(ClayMan* clayMan, std::function<void()> content, Clay_Color backgroundColor = {35, 35, 35, 255}, uint32_t height = 0, uint32_t padding = 16, uint32_t cornerRadius = 12);
};