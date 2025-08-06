#pragma once
#include "base_screen.hpp"
#include "../components/graph_component.hpp"
#include "../components/ui_card.hpp"
#include "../components/text_components.hpp"
#include "../components/layout_components.hpp"

class PerformanceScreen : public BaseScreen
{
protected:
	void RenderContent(ClayMan* clayMan, const SystemState& systemState);
	
private:
	void RenderHardwareSection(ClayMan* clayMan, const std::string& hardwareName, const std::string hardwareDetails, float currentUsage, const RingBuffer<float, 300>& historyData, Clay_Color accentColor);
};