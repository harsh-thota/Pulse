#pragma once
#include "base_screen.hpp"
#include "../components/ui_card.hpp"
#include "../components/graph_component.hpp"

class NetworkScreen : public BaseScreen
{
protected:
    void RenderContent(ClayMan* clayMan, const SystemState& systemState) override;
    
private:
    void RenderNetworkSummary(ClayMan* clayMan, const SystemState& systemState, uint32_t windowWidth);
    void RenderNetworkGraphs(ClayMan* clayMan, const SystemState& systemState);
    void RenderCombinedNetworkGraph(ClayMan* clayMan, const SystemState& systemState, uint32_t graphHeight);
    void RenderLegendItem(ClayMan* clayMan, const std::string& label, Clay_Color color);
    void RenderNetworkInterfaces(ClayMan* clayMan, const NetworkStats& networkStats);
    void RenderInterfaceCard(ClayMan* clayMan, const NetworkInterface& interface);
    void RenderInterfaceDetails(ClayMan* clayMan, const NetworkInterface& interface);
    void RenderInterfaceStats(ClayMan* clayMan, const NetworkInterface& interface);
};