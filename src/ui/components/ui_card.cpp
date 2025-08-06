#include "ui_card.hpp"

void UICard::Render(ClayMan* clayMan, std::function<void()> content, uint32_t height, uint32_t padding)
{
	RenderWithBackground(clayMan, content, { 35, 35, 35, 255 }, height, padding, 12);
}

void UICard::RenderWithBackground(ClayMan* clayMan, std::function<void()> content, Clay_Color backgroundColor, uint32_t height, uint32_t padding, uint32_t cornerRadius)
{
	Clay_ElementDeclaration cardContainer = {};
	if (height > 0)
	{
		cardContainer.layout.sizing = clayMan->expandXfixedY(height);
	}
	else
	{
		cardContainer.layout.sizing = clayMan->expandXY();
	}

	cardContainer.backgroundColor = backgroundColor;
	cardContainer.cornerRadius = { static_cast<float>(cornerRadius), static_cast<float>(cornerRadius), static_cast<float>(cornerRadius), static_cast<float>(cornerRadius) };
	cardContainer.layout.padding = clayMan->padAll(padding);

	clayMan->element(cardContainer, content);
}