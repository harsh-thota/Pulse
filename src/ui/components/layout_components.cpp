#include "layout_components.hpp"

void LayoutComponents::RenderTwoColumnLayout(ClayMan *clayMan, std::function<void()> leftContent, std::function<void()> rightContent, uint32_t leftWidth, uint32_t height, uint32_t gap)
{
	Clay_ElementDeclaration rowContainer = {};
	rowContainer.layout.sizing = clayMan->expandXfixedY(height);
	rowContainer.layout.layoutDirection = CLAY_LEFT_TO_RIGHT;
	rowContainer.layout.childGap = gap;

	clayMan->element(rowContainer, [clayMan, leftContent, rightContent, leftWidth]()
		{
			Clay_ElementDeclaration leftContainer = {};
			leftContainer.layout.sizing = clayMan->expandYfixedX(leftWidth);
			leftContainer.layout.childAlignment = clayMan->centerXY();
			clayMan->element(leftContainer, leftContent);

			Clay_ElementDeclaration rightContainer = {};
			rightContainer.layout.sizing = clayMan->expandXY();
			rightContainer.layout.childAlignment = clayMan->centerXY();
			clayMan->element(rightContainer, rightContent);
		});
}

void LayoutComponents::RenderScrollableContainer(ClayMan* clayMan, std::function<void()> content, uint32_t padding, uint32_t childGap)
{
	// Create scrollable container with proper scroll configuration
	Clay_ElementDeclaration scrollContainer = {};
	scrollContainer.layout.sizing = clayMan->expandXY();
	scrollContainer.layout.layoutDirection = CLAY_TOP_TO_BOTTOM;
	
	// Note: Clay scrolling is handled by parent containers in application.cpp
	// This container just provides the layout structure
	
	if (padding > 0) {
		scrollContainer.layout.padding = clayMan->padAll(padding);
	}

	clayMan->element(scrollContainer, [clayMan, content, childGap]()
		{
			Clay_ElementDeclaration contentContainer = {};
			contentContainer.layout.sizing = clayMan->expandX(); // Allow vertical growth for scrolling
			contentContainer.layout.layoutDirection = CLAY_TOP_TO_BOTTOM;
			
			if (childGap > 0) {
				contentContainer.layout.childGap = childGap;
			}
			
			clayMan->element(contentContainer, content);
		});
}

void LayoutComponents::RenderVerticalStack(ClayMan* clayMan, std::function<void()> content, uint32_t gap)
{
	Clay_ElementDeclaration stackContainer = {};
	stackContainer.layout.sizing = clayMan->expandXY();
	stackContainer.layout.layoutDirection = CLAY_TOP_TO_BOTTOM;
	stackContainer.layout.childGap = gap;
	clayMan->element(stackContainer, content);
}