#pragma once
#include "base_screen.hpp"
#include "../components/graph_component.hpp"
#include "../components/ui_card.hpp"
#include "../components/text_components.hpp"
#include "../components/layout_components.hpp"
#include <vector>
#include <utility>

class PerformanceScreen : public BaseScreen
{
protected:
    void RenderContent(ClayMan* clayMan, const SystemState& systemState) override;
    
private:
    // Main section rendering methods
    void RenderSystemOverview(ClayMan* clayMan, const SystemState& systemState, uint32_t windowWidth);
    void RenderPerformanceMetrics(ClayMan* clayMan, const SystemState& systemState, uint32_t cardHeight);
    void RenderHardwareDetails(ClayMan* clayMan, const SystemState& systemState, uint32_t windowWidth);
    
    // Enhanced card rendering methods
    void RenderEnhancedHardwareCard(ClayMan* clayMan, const std::string& title, const std::string& subtitle, float currentUsage, const RingBuffer<float, 300>& historyData, Clay_Color accentColor, uint32_t cardHeight);
    void RenderHardwareInfoCard(ClayMan* clayMan, const std::string& title, const std::vector<std::pair<std::string, std::string>>& info, Clay_Color accentColor, uint32_t cardHeight);
    
    // Legacy method for compatibility
    void RenderHardwareCard(ClayMan* clayMan, const std::string& hardwareName, const std::string& hardwareDetails, float currentUsage, const RingBuffer<float, 300>& historyData, Clay_Color accentColor, uint32_t cardHeight = 400);
};