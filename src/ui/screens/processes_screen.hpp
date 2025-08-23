#pragma once
#include "base_screen.hpp"
#include "../components/ui_card.hpp"
#include "../components/table_component.hpp"

class ProcessesScreen : public BaseScreen
{
protected:
    void RenderContent(ClayMan* clayMan, const SystemState& systemState) override;
    
private:
    void RenderProcessSummary(ClayMan* clayMan, const SystemState& systemState, uint32_t windowWidth);
    void RenderProcessTable(ClayMan* clayMan, const std::vector<ProcessInfo>& processes);
    void RenderProcessTableHeader(ClayMan* clayMan);
    void RenderProcessRow(ClayMan* clayMan, const ProcessInfo& process, bool isEvenRow);
};