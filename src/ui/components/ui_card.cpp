#include "ui_card.hpp"

void UICard::Render(ClayMan* clayMan, std::function<void()> content, uint32_t height, uint32_t padding)
{
	RenderWithBackground(clayMan, content, { 40, 40, 40, 255 }, height, padding, 16);
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
	cardContainer.cornerRadius = {
		static_cast<float>(cornerRadius),
		static_cast<float>(cornerRadius),
		static_cast<float>(cornerRadius),
		static_cast<float>(cornerRadius)
	};
	cardContainer.layout.padding = clayMan->padAll(padding);

	clayMan->element(cardContainer, content);
}

void UICard::RenderMetricCard(ClayMan* clayMan, const std::string& title, const std::string& value, const std::string& subtitle, Clay_Color accentColor, uint32_t height)
{
	RenderWithBackground(clayMan, [clayMan, &title, &value, &subtitle, accentColor, height]() {
		Clay_ElementDeclaration cardLayout = {};
		cardLayout.layout.sizing = clayMan->expandXY();
		cardLayout.layout.layoutDirection = CLAY_TOP_TO_BOTTOM;
		cardLayout.layout.childAlignment = clayMan->centerXY();
		cardLayout.layout.childGap = std::max(8u, static_cast<uint32_t>(height * 0.08f));

		clayMan->element(cardLayout, [clayMan, &title, &value, &subtitle, accentColor, height]() {
			// Title
			Clay_ElementDeclaration titleContainer = {};
			titleContainer.layout.sizing = clayMan->expandXfixedY(std::max(20u, static_cast<uint32_t>(height * 0.25f)));
			titleContainer.layout.childAlignment = clayMan->centerXY();

			clayMan->element(titleContainer, [clayMan, &title, height]() {
				Clay_TextElementConfig titleText = {};
				titleText.textColor = { 180, 180, 180, 255 };
				titleText.fontId = 0;
				titleText.fontSize = std::max(12u, static_cast<uint32_t>(height * 0.15f));
				clayMan->textElement(title, titleText);
			});

			// Main value
			Clay_ElementDeclaration valueContainer = {};
			valueContainer.layout.sizing = clayMan->expandXfixedY(std::max(30u, static_cast<uint32_t>(height * 0.4f)));
			valueContainer.layout.childAlignment = clayMan->centerXY();

			clayMan->element(valueContainer, [clayMan, &value, accentColor, height]() {
				Clay_TextElementConfig valueText = {};
				valueText.textColor = accentColor;
				valueText.fontId = 0;
				valueText.fontSize = std::max(18u, static_cast<uint32_t>(height * 0.25f));
				clayMan->textElement(value, valueText);
			});

			// Subtitle
			if (!subtitle.empty()) {
				Clay_ElementDeclaration subtitleContainer = {};
				subtitleContainer.layout.sizing = clayMan->expandXfixedY(std::max(15u, static_cast<uint32_t>(height * 0.2f)));
				subtitleContainer.layout.childAlignment = clayMan->centerXY();

				clayMan->element(subtitleContainer, [clayMan, &subtitle, height]() {
					Clay_TextElementConfig subtitleText = {};
					subtitleText.textColor = { 140, 140, 140, 255 };
					subtitleText.fontId = 0;
					subtitleText.fontSize = std::max(10u, static_cast<uint32_t>(height * 0.12f));
					clayMan->textElement(subtitle, subtitleText);
				});
			}
		});
	}, { 45, 45, 45, 255 }, height, 16, 12);
}

void UICard::RenderSectionCard(ClayMan* clayMan, const std::string& sectionTitle, std::function<void()> content, uint32_t minHeight)
{
	RenderWithBackground(clayMan, [clayMan, &sectionTitle, content, minHeight]() {
		Clay_ElementDeclaration sectionLayout = {};
		sectionLayout.layout.sizing = clayMan->expandXY();
		sectionLayout.layout.layoutDirection = CLAY_TOP_TO_BOTTOM;
		sectionLayout.layout.childGap = 16;

		clayMan->element(sectionLayout, [clayMan, &sectionTitle, content, minHeight]() {
			// Section header
			Clay_ElementDeclaration headerContainer = {};
			headerContainer.layout.sizing = clayMan->expandXfixedY(std::max(40u, static_cast<uint32_t>(minHeight * 0.15f)));
			headerContainer.layout.childAlignment = { CLAY_ALIGN_X_LEFT, CLAY_ALIGN_Y_CENTER };
			headerContainer.layout.padding = clayMan->padXY(4, 0);

			clayMan->element(headerContainer, [clayMan, &sectionTitle, minHeight]() {
				Clay_TextElementConfig headerText = {};
				headerText.textColor = { 220, 220, 220, 255 };
				headerText.fontId = 0;
				headerText.fontSize = std::max(16u, static_cast<uint32_t>(minHeight * 0.08f));
				clayMan->textElement(sectionTitle, headerText);
			});

			// Section content
			Clay_ElementDeclaration contentContainer = {};
			contentContainer.layout.sizing = clayMan->expandXY();

			clayMan->element(contentContainer, content);
		});
	}, { 35, 35, 35, 255 }, 0, 20, 16);
}