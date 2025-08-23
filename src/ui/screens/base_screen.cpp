#include "base_screen.hpp"
#include <sstream>
#include <iomanip>

void BaseScreen::Render(ClayMan* clayMan, const SystemState& systemState)
{
	// Simple container - scrolling is handled by the main content area in Application
	Clay_ElementDeclaration contentContainer = {};
		contentContainer.layout.sizing = clayMan->expandXY(); // Fill width and allow vertical growth
	contentContainer.layout.layoutDirection = CLAY_TOP_TO_BOTTOM;
	contentContainer.layout.childGap = GetSectionGap();
	
	clayMan->element(contentContainer, [this, clayMan, &systemState]() {
		RenderContent(clayMan, systemState);
	});
}

std::string BaseScreen::FormatBytes(uint64_t bytes)
{
	std::ostringstream oss;
	if (bytes >= 1024 * 1024 * 1024) {
		oss << std::fixed << std::setprecision(1) << (static_cast<double>(bytes) / (1024 * 1024 * 1024)) << "GB";
	} else if (bytes >= 1024 * 1024) {
		oss << std::fixed << std::setprecision(1) << (static_cast<double>(bytes) / (1024 * 1024)) << "MB";
	} else if (bytes >= 1024) {
		oss << std::fixed << std::setprecision(1) << (static_cast<double>(bytes) / 1024) << "KB";
	} else {
		oss << bytes << "B";
	}
	return oss.str();
}

std::string BaseScreen::FormatPercentage(float percentage)
{
	std::ostringstream oss;
	oss << std::fixed << std::setprecision(1) << percentage << "%";
	return oss.str();
}