#include "metric_card.hpp"
#include "ui_card.hpp"
#include "text_components.hpp"
#include "layout_components.hpp"
#include "graph_component.hpp"
#include "../screens/base_screen.hpp"
#include <sstream>
#include <iomanip>

static std::string FormatPercentage(float percentage)
{
    std::ostringstream oss;
    oss << std::fixed << std::setprecision(1) << percentage << "%";
    return oss.str();
}

void MetricCard::RenderHardwareMetric(ClayMan* clayMan, const char* title, const char* details, float currentValue, const RingBuffer<float, 300>& history, Clay_Color accentColor)
{
    UICard::Render(clayMan, [clayMan, title, details, currentValue, &history, accentColor]()
        {
            LayoutComponents::RenderVerticalStack(clayMan, [clayMan, title, details, currentValue, &history, accentColor]()
                {
                    TextComponents::RenderCenteredTitle(clayMan, title, 40, 18);
                    GraphComponent::RenderTimeSeriesGraph(clayMan, history, accentColor, 200);
                    LayoutComponents::RenderTwoColumnLayout(clayMan, [clayMan, currentValue, accentColor]()
                        {
                            std::string percentStr = FormatPercentage(currentValue);
                            TextComponents::RenderLargeValue(clayMan, percentStr, accentColor, 24);
                        },
                        [clayMan, details]()
                        {
                            TextComponents::RenderSubtitle(clayMan, details, 12);
                        },
                        150, 60, 20
                    );
                }, 16);
        }, 320);
}

void MetricCard::RenderSimpleMetric(ClayMan* clayMan, const char* title, const char* value, Clay_Color valueColor, const char* subtitle)
{
    UICard::Render(clayMan, [clayMan, title, value, valueColor, subtitle]()
        {
            LayoutComponents::RenderVerticalStack(clayMan, [clayMan, title, value, valueColor, subtitle]()
                {
                    TextComponents::RenderCenteredTitle(clayMan, title, 30, 16);
                    Clay_ElementDeclaration valueContainer = {};
                    valueContainer.layout.sizing = clayMan->expandXfixedY(40);
                    valueContainer.layout.childAlignment = clayMan->centerXY();

                    clayMan->element(valueContainer, [clayMan, value, valueColor]()
                        {
                            TextComponents::RenderLargeValue(clayMan, value, valueColor, 20);
                        });

                    if (subtitle != nullptr)
                    {
                        Clay_ElementDeclaration subtitleContainer = {};
                        subtitleContainer.layout.sizing = clayMan->expandXfixedY(20);
                        subtitleContainer.layout.childAlignment = clayMan->centerXY();

                        clayMan->element(subtitleContainer, [clayMan, subtitle]()
                            {
                                TextComponents::RenderSubtitle(clayMan, subtitle, 10);
                            });
                    }
                }, 8);
        }, 120);
}