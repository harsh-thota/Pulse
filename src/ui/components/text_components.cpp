#include "text_components.hpp"

void TextComponents::RenderTitle(ClayMan* clayMan, const std::string& title, uint32_t fontSize, Clay_Color color)
{
	Clay_TextElementConfig titleText = {};
	titleText.textColor = color;
	titleText.fontId = 0;
	titleText.fontSize = fontSize;
	clayMan->textElement(title, titleText);
}

void TextComponents::RenderSubtitle(ClayMan* clayMan, const std::string& subtitle, uint32_t fontSize, Clay_Color color)
{
	Clay_TextElementConfig subtitleText = {};
	subtitleText.textColor = color;
	subtitleText.fontId = 0;
	subtitleText.fontSize = fontSize;
	clayMan->textElement(subtitle, subtitleText);
}

void TextComponents::RenderLargeValue(ClayMan* clayMan, const std::string& value, Clay_Color color, uint32_t fontSize)
{
	Clay_TextElementConfig valueText = {};
	valueText.textColor = color;
	valueText.fontId = 0;
	valueText.fontSize = fontSize;
	clayMan->textElement(value, valueText);
}

void TextComponents::RenderCenteredTitle(ClayMan* clayMan, const std::string& title, uint32_t containerHeight, uint32_t fontSize)
{
	Clay_ElementDeclaration titleContainer = {};
	titleContainer.layout.sizing = clayMan->expandXfixedY(containerHeight);
	titleContainer.layout.childAlignment = clayMan->centerXY();

	clayMan->element(titleContainer, [clayMan, &title, fontSize]()
		{
			TextComponents::RenderTitle(clayMan, title, fontSize);
		});
}