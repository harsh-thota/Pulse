#include "performance_screen.hpp"

void PerformanceScreen::RenderContent(ClayMan* clayMan, const SystemState& systemState)
{
    std::string cpuDetails = systemState.cpuName + " (" + std::to_string(systemState.coreCount) + " cores)";
    RenderHardwareSection(clayMan, "CPU", cpuDetails, systemState.cpuUsagePercent, systemState.cpuHistory, { 0, 255, 150, 255 });
    
    RenderHardwareSection(clayMan, "GPU", "Integrated Graphics", systemState.gpuUsagePercent, systemState.gpuHistory, { 255, 150, 0, 255 });
    std::string memoryDetails = FormatBytes(systemState.usedRAMBytes) + " / " + FormatBytes(systemState.totalRAMBytes);
    RenderHardwareSection(clayMan, "Memory", memoryDetails, systemState.memoryUsagePercent, systemState.memoryHistory, { 100, 150, 255, 255 });
}

void PerformanceScreen::RenderHardwareSection(ClayMan* clayMan, const std::string& hardwareName, const std::string hardwareDetails, float currentUsage, const RingBuffer<float, 300>& historyData, Clay_Color accentColor)
{
    UICard::Render(clayMan, [clayMan, &hardwareName, &hardwareDetails, currentUsage, &historyData, accentColor]() {
        LayoutComponents::RenderVerticalStack(clayMan, [clayMan, &hardwareName, &hardwareDetails, currentUsage, &historyData, accentColor]() {
            TextComponents::RenderCenteredTitle(clayMan, hardwareName, 40, 18);
            GraphComponent::RenderTimeSeriesGraph(clayMan, historyData, accentColor, 200);
            LayoutComponents::RenderTwoColumnLayout(clayMan,
                [clayMan, currentUsage, accentColor]()
                {
                    std::string percentStr = FormatPercentage(currentUsage);
                    TextComponents::RenderLargeValue(clayMan, percentStr, accentColor, 24);
                },
                [clayMan, &hardwareDetails]()
                {
                    TextComponents::RenderSubtitle(clayMan, hardwareDetails, 12);
                },
                150, 60, 20
            );
        }, 16);
    }, 320);
}