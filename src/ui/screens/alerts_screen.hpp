#pragma once
#include "base_screen.hpp"
#include "../components/ui_card.hpp"
#include "../components/text_components.hpp"
#include "../components/layout_components.hpp"

class AlertsScreen : public BaseScreen
{
protected:
	void RenderContent(ClayMan* clayMan, const SystemState& systemState) override;
	
private:
	void RenderAlertsSummary(ClayMan* clayMan, const SystemState& systemState, uint32_t windowWidth);
	void RenderActiveAlerts(ClayMan* clayMan, const std::vector<SystemAlert>& alerts);
	void RenderAlertCard(ClayMan* clayMan, const SystemAlert& alert);
	void RenderAlertRules(ClayMan* clayMan, const std::vector<AlertRule>& rules);
	void RenderRulesTableHeader(ClayMan* clayMan);
	void RenderAlertRuleRow(ClayMan* clayMan, const AlertRule& rule);
	
	Clay_Color GetSeverityColor(uint32_t severity) const;
	std::string GetAlertTypeString(AlertRule::Type type) const;
	std::string GetTimeAgoString(uint64_t timestamp) const;
};