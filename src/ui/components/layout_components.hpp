#pragma once
#include "clayman.hpp"
#include <functional>
#include <string>

class LayoutComponents
{
public:
	static void RenderTwoColumnLayout(ClayMan* clayMan, std::function<void()> leftContent, std::function<void()> rightContent, uint32_t leftWidth = 150, uint32_t height = 60, uint32_t gap = 20);
	static void RenderScrollableContainer(ClayMan* clayMan, std::function<void()> content, uint32_t padding = 24, uint32_t childGap = 40);
	static void RenderVerticalStack(ClayMan* clayMan, std::function<void()> content, uint32_t gap = 16);
};