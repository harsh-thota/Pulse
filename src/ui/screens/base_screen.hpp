#pragma once
#include "clayman.hpp"
#include "../../monitoring/data_collector.hpp"
#include "../components/layout_components.hpp"
#include "../components/ui_card.hpp"
#include "../components/text_components.hpp"
#include "../../monitoring/data_collector.hpp"

class BaseScreen
{
public:
	virtual ~BaseScreen() = default;
	virtual void Render(ClayMan* clayMan, const SystemState& systemState);

protected:
	virtual void RenderContent(ClayMan* clayMan, const SystemState& systemState) = 0;
	static std::string FormatBytes(uint64_t bytes);
	static std::string FormatPercentage(float percentage);

private:
	uint32_t GetScrollPadding() const { return 24; }
	uint32_t GetSectionGap() const { return 40; }
};