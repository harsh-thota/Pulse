#include "performance_screen.hpp"
#include "../components/ui_card.hpp"

void PerformanceScreen::RenderContent(ClayMan* clayMan, const SystemState& systemState)
{
    // Get window dimensions for responsive layout
    int windowWidth = clayMan->getWindowWidth();
    int windowHeight = clayMan->getWindowHeight();
    
    // Calculate responsive spacing and sizing
    uint32_t sectionGap = std::max(24u, static_cast<uint32_t>(windowHeight * 0.03f));
    uint32_t cardHeight = std::max(350u, static_cast<uint32_t>(windowHeight * 0.4f));
    
    // Main performance container - vertical scrollable layout
    Clay_ElementDeclaration performanceContainer = {};
    // Fill available space horizontally and allow vertical growth
    performanceContainer.layout.sizing = clayMan->expandXY();
    performanceContainer.layout.layoutDirection = CLAY_TOP_TO_BOTTOM;
    performanceContainer.layout.childGap = sectionGap;
    
    clayMan->element(performanceContainer, [this, clayMan, &systemState, cardHeight, windowWidth]() {
        
        // System Overview Section
        RenderSystemOverview(clayMan, systemState, windowWidth);
        
        // Performance Metrics Section
        RenderPerformanceMetrics(clayMan, systemState, cardHeight);
        
        // Hardware Details Section  
        RenderHardwareDetails(clayMan, systemState, windowWidth);
    });
}

void PerformanceScreen::RenderSystemOverview(ClayMan* clayMan, const SystemState& systemState, uint32_t windowWidth)
{
    uint32_t cardHeight = 140;
    
    UICard::RenderSectionCard(clayMan, "System Overview", [this, clayMan, &systemState, cardHeight]() {
        
        // Compute responsiveness at render-time
        int curWidth = clayMan->getWindowWidth();
        Clay_ElementDeclaration overviewGrid = {};
        overviewGrid.layout.sizing = clayMan->expandXY();
        if (curWidth < static_cast<int>(cardHeight * 4)) {
            overviewGrid.layout.layoutDirection = CLAY_TOP_TO_BOTTOM;
        } else {
            overviewGrid.layout.layoutDirection = CLAY_LEFT_TO_RIGHT;
        }
    overviewGrid.layout.childGap = std::max(16u, static_cast<uint32_t>(curWidth * 0.02f));
        
        clayMan->element(overviewGrid, [this, clayMan, &systemState, cardHeight]() {
            
            // CPU Overview Card
            UICard::RenderMetricCard(clayMan, "CPU Usage", 
                FormatPercentage(systemState.cpuUsagePercent),
                systemState.cpuName + " (" + std::to_string(systemState.coreCount) + " cores)",
                { 0, 255, 150, 255 }, cardHeight);
            
            // Memory Overview Card  
            UICard::RenderMetricCard(clayMan, "Memory Usage",
                FormatPercentage(systemState.memoryUsagePercent),
                FormatBytes(systemState.usedRAMBytes) + " / " + FormatBytes(systemState.totalRAMBytes),
                { 100, 150, 255, 255 }, cardHeight);
            
            // GPU Overview Card
            UICard::RenderMetricCard(clayMan, "GPU Usage",
                FormatPercentage(systemState.gpuUsagePercent),
                systemState.gpuName,
                { 255, 150, 0, 255 }, cardHeight);
            
            // Disk Overview Card
            UICard::RenderMetricCard(clayMan, "Disk Activity",
                FormatPercentage(systemState.diskUsagePercent),
                "R: " + FormatBytes(systemState.diskReadBytesPerSec) + "/s W: " + FormatBytes(systemState.diskWriteBytesPerSec) + "/s",
                { 255, 100, 255, 255 }, cardHeight);
        });
        
    }, cardHeight + 80);
}

void PerformanceScreen::RenderPerformanceMetrics(ClayMan* clayMan, const SystemState& systemState, uint32_t cardHeight)
{
    UICard::RenderSectionCard(clayMan, "Performance Graphs", [this, clayMan, &systemState, cardHeight]() {
        
        Clay_ElementDeclaration metricsGrid = {};
        metricsGrid.layout.sizing = clayMan->expandXY();
        metricsGrid.layout.layoutDirection = CLAY_TOP_TO_BOTTOM;
        metricsGrid.layout.childGap = 20;
        
        clayMan->element(metricsGrid, [this, clayMan, &systemState, cardHeight]() {
            
            // First row: CPU and GPU
            Clay_ElementDeclaration topRow = {};
            topRow.layout.sizing = clayMan->expandXfixedY(cardHeight);
            topRow.layout.layoutDirection = CLAY_LEFT_TO_RIGHT;
            topRow.layout.childGap = 20;
            
            clayMan->element(topRow, [this, clayMan, &systemState, cardHeight]() {
                // CPU Performance Card
                RenderEnhancedHardwareCard(clayMan, "CPU Performance", 
                    systemState.cpuName, systemState.cpuUsagePercent, 
                    systemState.cpuHistory, { 0, 255, 150, 255 }, cardHeight);
                
                // GPU Performance Card  
                RenderEnhancedHardwareCard(clayMan, "GPU Performance",
                    systemState.gpuName, systemState.gpuUsagePercent,
                    systemState.gpuHistory, { 255, 150, 0, 255 }, cardHeight);
            });
            
            // Second row: Memory and Disk
            Clay_ElementDeclaration bottomRow = {};
            bottomRow.layout.sizing = clayMan->expandXfixedY(cardHeight);
            bottomRow.layout.layoutDirection = CLAY_LEFT_TO_RIGHT;
            bottomRow.layout.childGap = 20;
            
            clayMan->element(bottomRow, [this, clayMan, &systemState, cardHeight]() {
                // Memory Performance Card
                RenderEnhancedHardwareCard(clayMan, "Memory Usage",
                    FormatBytes(systemState.usedRAMBytes) + " / " + FormatBytes(systemState.totalRAMBytes),
                    systemState.memoryUsagePercent, systemState.memoryHistory,
                    { 100, 150, 255, 255 }, cardHeight);
                
                // Disk Performance Card
                RenderEnhancedHardwareCard(clayMan, "Disk I/O Activity",
                    systemState.primaryDiskName, systemState.diskUsagePercent,
                    systemState.diskHistory, { 255, 100, 255, 255 }, cardHeight);
            });
        });
        
    }, (cardHeight * 2) + 120);
}

void PerformanceScreen::RenderHardwareDetails(ClayMan* clayMan, const SystemState& systemState, uint32_t windowWidth)
{
    uint32_t cardHeight = 200;
    
    UICard::RenderSectionCard(clayMan, "Hardware Information", [this, clayMan, &systemState, cardHeight, windowWidth]() {
        
        Clay_ElementDeclaration detailsGrid = {};
        detailsGrid.layout.sizing = clayMan->expandXfixedY(cardHeight);
        detailsGrid.layout.layoutDirection = CLAY_LEFT_TO_RIGHT;
        detailsGrid.layout.childGap = std::max(16u, static_cast<uint32_t>(windowWidth * 0.02f));
        
        clayMan->element(detailsGrid, [this, clayMan, &systemState, cardHeight]() {
            
            // CPU Details
            RenderHardwareInfoCard(clayMan, "CPU Information", {
                {"Name", systemState.cpuName},
                {"Cores", std::to_string(systemState.coreCount)},
                {"Usage", FormatPercentage(systemState.cpuUsagePercent)},
                {"System", systemState.systemName}
            }, { 0, 255, 150, 255 }, cardHeight);
            
            // Memory Details
            RenderHardwareInfoCard(clayMan, "Memory Information", {
                {"Total RAM", FormatBytes(systemState.totalRAMBytes)},
                {"Used RAM", FormatBytes(systemState.usedRAMBytes)},
                {"Usage", FormatPercentage(systemState.memoryUsagePercent)},
                {"Available", FormatBytes(systemState.totalRAMBytes - systemState.usedRAMBytes)}
            }, { 100, 150, 255, 255 }, cardHeight);
            
            // Network Details
            RenderHardwareInfoCard(clayMan, "Network Information", {
                {"Interface", systemState.primaryNetworkInterface},
                {"Download", FormatBytes(systemState.downloadBytesPerSec) + "/s"},
                {"Upload", FormatBytes(systemState.uploadBytesPerSec) + "/s"},
                {"Usage", FormatPercentage(systemState.networkUsagePercent)}
            }, { 255, 255, 100, 255 }, cardHeight);
        });
        
    }, cardHeight + 80);
}

void PerformanceScreen::RenderEnhancedHardwareCard(ClayMan* clayMan, const std::string& title, const std::string& subtitle, float currentUsage, const RingBuffer<float, 300>& historyData, Clay_Color accentColor, uint32_t cardHeight)
{
    UICard::RenderWithBackground(clayMan, [clayMan, &title, &subtitle, currentUsage, &historyData, accentColor, cardHeight]() {
        
        Clay_ElementDeclaration cardLayout = {};
        cardLayout.layout.sizing = clayMan->expandXY();
        cardLayout.layout.layoutDirection = CLAY_TOP_TO_BOTTOM;
        cardLayout.layout.padding = clayMan->padAll(16);
        cardLayout.layout.childGap = 12;
        
        clayMan->element(cardLayout, [clayMan, &title, &subtitle, currentUsage, &historyData, accentColor, cardHeight]() {
            
            // Header section with title and current value
            Clay_ElementDeclaration headerSection = {};
            headerSection.layout.sizing = clayMan->expandXfixedY(50);
            headerSection.layout.layoutDirection = CLAY_LEFT_TO_RIGHT;
            headerSection.layout.childAlignment = { CLAY_ALIGN_X_LEFT, CLAY_ALIGN_Y_CENTER };
            
            clayMan->element(headerSection, [clayMan, &title, currentUsage, accentColor]() {
                // Title
                Clay_ElementDeclaration titleContainer = {};
                titleContainer.layout.sizing = clayMan->expandXY();
                titleContainer.layout.childAlignment = { CLAY_ALIGN_X_LEFT, CLAY_ALIGN_Y_CENTER };
                
                clayMan->element(titleContainer, [clayMan, &title]() {
                    Clay_TextElementConfig titleText = {};
                    titleText.textColor = { 220, 220, 220, 255 };
                    titleText.fontId = 0;
                    titleText.fontSize = 16;
                    clayMan->textElement(title, titleText);
                });
                
                // Current value badge
                Clay_ElementDeclaration valueBadge = {};
                valueBadge.layout.sizing = clayMan->fixedSize(80, 32);
                valueBadge.backgroundColor = accentColor;
                valueBadge.cornerRadius = { 16, 16, 16, 16 };
                valueBadge.layout.childAlignment = clayMan->centerXY();
                
                clayMan->element(valueBadge, [clayMan, currentUsage]() {
                    Clay_TextElementConfig valueText = {};
                    valueText.textColor = { 25, 25, 25, 255 };
                    valueText.fontId = 0;
                    valueText.fontSize = 14;
                    clayMan->textElement(FormatPercentage(currentUsage), valueText);
                });
            });
            
            // Graph section
            Clay_ElementDeclaration graphSection = {};
            graphSection.layout.sizing = clayMan->expandXfixedY(cardHeight - 120);
            
            clayMan->element(graphSection, [clayMan, &historyData, accentColor, cardHeight]() {
                GraphComponent::RenderTimeSeriesGraph(clayMan, historyData, accentColor, cardHeight - 120);
            });
            
            // Footer with subtitle
            Clay_ElementDeclaration footerSection = {};
            footerSection.layout.sizing = clayMan->expandXfixedY(30);
            footerSection.layout.childAlignment = { CLAY_ALIGN_X_LEFT, CLAY_ALIGN_Y_CENTER };
            footerSection.backgroundColor = { 25, 25, 25, 255 };
            footerSection.cornerRadius = { 8, 8, 8, 8 };
            footerSection.layout.padding = clayMan->padXY(12, 8);
            
            clayMan->element(footerSection, [clayMan, &subtitle]() {
                Clay_TextElementConfig subtitleText = {};
                subtitleText.textColor = { 160, 160, 160, 255 };
                subtitleText.fontId = 0;
                subtitleText.fontSize = 12;
                clayMan->textElement(subtitle, subtitleText);
            });
        });
        
    }, { 45, 45, 45, 255 }, cardHeight, 0, 16);
}

void PerformanceScreen::RenderHardwareInfoCard(ClayMan* clayMan, const std::string& title, const std::vector<std::pair<std::string, std::string>>& info, Clay_Color accentColor, uint32_t cardHeight)
{
    UICard::RenderWithBackground(clayMan, [clayMan, &title, &info, accentColor, cardHeight]() {
        
        Clay_ElementDeclaration cardLayout = {};
        cardLayout.layout.sizing = clayMan->expandXY();
        cardLayout.layout.layoutDirection = CLAY_TOP_TO_BOTTOM;
        cardLayout.layout.padding = clayMan->padAll(16);
        cardLayout.layout.childGap = 12;
        
        clayMan->element(cardLayout, [clayMan, &title, &info, accentColor, cardHeight]() {
            
            // Header with colored accent
            Clay_ElementDeclaration headerSection = {};
            headerSection.layout.sizing = clayMan->expandXfixedY(40);
            headerSection.layout.layoutDirection = CLAY_LEFT_TO_RIGHT;
            headerSection.layout.childAlignment = { CLAY_ALIGN_X_LEFT, CLAY_ALIGN_Y_CENTER };
            headerSection.layout.childGap = 12;
            
            clayMan->element(headerSection, [clayMan, &title, accentColor]() {
                // Colored indicator
                Clay_ElementDeclaration indicator = {};
                indicator.layout.sizing = clayMan->fixedSize(4, 30);
                indicator.backgroundColor = accentColor;
                indicator.cornerRadius = { 2, 2, 2, 2 };
                clayMan->element(indicator, []() {});
                
                // Title
                Clay_ElementDeclaration titleContainer = {};
                titleContainer.layout.sizing = clayMan->expandXY();
                titleContainer.layout.childAlignment = { CLAY_ALIGN_X_LEFT, CLAY_ALIGN_Y_CENTER };
                
                clayMan->element(titleContainer, [clayMan, &title]() {
                    Clay_TextElementConfig titleText = {};
                    titleText.textColor = { 220, 220, 220, 255 };
                    titleText.fontId = 0;
                    titleText.fontSize = 16;
                    clayMan->textElement(title, titleText);
                });
            });
            
            // Information list
            Clay_ElementDeclaration infoList = {};
            infoList.layout.sizing = clayMan->expandXY();
            infoList.layout.layoutDirection = CLAY_TOP_TO_BOTTOM;
            infoList.layout.childGap = 8;
            
            clayMan->element(infoList, [clayMan, &info, accentColor]() {
                for (const auto& item : info) {
                    Clay_ElementDeclaration infoRow = {};
                    infoRow.layout.sizing = clayMan->expandXfixedY(24);
                    infoRow.layout.layoutDirection = CLAY_LEFT_TO_RIGHT;
                    infoRow.layout.childAlignment = { CLAY_ALIGN_X_LEFT, CLAY_ALIGN_Y_CENTER };
                    infoRow.backgroundColor = { 35, 35, 35, 255 };
                    infoRow.cornerRadius = { 6, 6, 6, 6 };
                    infoRow.layout.padding = clayMan->padXY(12, 6);
                    
                    clayMan->element(infoRow, [clayMan, &item, accentColor]() {
                        // Key
                        Clay_ElementDeclaration keyContainer = {};
                        keyContainer.layout.sizing = clayMan->fixedSize(100, 24);
                        keyContainer.layout.childAlignment = { CLAY_ALIGN_X_LEFT, CLAY_ALIGN_Y_CENTER };
                        
                        clayMan->element(keyContainer, [clayMan, &item]() {
                            Clay_TextElementConfig keyText = {};
                            keyText.textColor = { 160, 160, 160, 255 };
                            keyText.fontId = 0;
                            keyText.fontSize = 12;
                            clayMan->textElement(item.first, keyText);
                        });
                        
                        // Value
                        Clay_ElementDeclaration valueContainer = {};
                        valueContainer.layout.sizing = clayMan->expandXY();
                        valueContainer.layout.childAlignment = { CLAY_ALIGN_X_LEFT, CLAY_ALIGN_Y_CENTER };
                        
                        clayMan->element(valueContainer, [clayMan, &item, accentColor]() {
                            Clay_TextElementConfig valueText = {};
                            valueText.textColor = accentColor;
                            valueText.fontId = 0;
                            valueText.fontSize = 12;
                            clayMan->textElement(item.second, valueText);
                        });
                    });
                }
            });
        });
        
    }, { 40, 40, 40, 255 }, cardHeight, 0, 16);
}

void PerformanceScreen::RenderHardwareCard(ClayMan* clayMan, const std::string& hardwareName, const std::string& hardwareDetails, float currentUsage, const RingBuffer<float, 300>& historyData, Clay_Color accentColor, uint32_t cardHeight)
{
    // Use the enhanced version
    RenderEnhancedHardwareCard(clayMan, hardwareName, hardwareDetails, currentUsage, historyData, accentColor, cardHeight);
}