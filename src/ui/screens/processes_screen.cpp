#include "processes_screen.hpp"
#include "../components/ui_card.hpp"
#include "../components/table_component.hpp"
#include <algorithm>
#include <iomanip>
#include <sstream>

void ProcessesScreen::RenderContent(ClayMan* clayMan, const SystemState& systemState)
{
    // Get window dimensions for responsive layout
    int windowWidth = clayMan->getWindowWidth();
    int windowHeight = clayMan->getWindowHeight();
    
    // Calculate responsive spacing
    uint32_t sectionGap = std::max(20u, static_cast<uint32_t>(windowHeight * 0.025f));
    
    Clay_ElementDeclaration processContainer = {};
    // Fill width and allow vertical growth
    processContainer.layout.sizing = clayMan->expandXY();
    processContainer.layout.layoutDirection = CLAY_TOP_TO_BOTTOM;
    processContainer.layout.childGap = sectionGap;
    
    clayMan->element(processContainer, [this, clayMan, &systemState, windowWidth]() {
        RenderProcessSummary(clayMan, systemState, windowWidth);
        RenderProcessTable(clayMan, systemState.processes);
    });
}

void ProcessesScreen::RenderProcessSummary(ClayMan* clayMan, const SystemState& systemState, uint32_t windowWidth)
{
    uint32_t cardHeight = 120;
    
    UICard::RenderSectionCard(clayMan, "Process Summary", [this, clayMan, &systemState, cardHeight, windowWidth]() {
        
        // Summary cards container
        Clay_ElementDeclaration summaryContainer = {};
        summaryContainer.layout.sizing = clayMan->expandXfixedY(cardHeight);
        summaryContainer.layout.layoutDirection = CLAY_LEFT_TO_RIGHT;
        summaryContainer.layout.childGap = std::max(16u, static_cast<uint32_t>(windowWidth * 0.02f));
        
        clayMan->element(summaryContainer, [this, clayMan, &systemState, cardHeight]() {
            
            // Total Processes Card
            UICard::RenderMetricCard(clayMan, "Total Processes",
                std::to_string(systemState.totalProcesses),
                "Running system processes",
                { 0, 255, 150, 255 }, cardHeight);
            
            // Total Threads Card
            UICard::RenderMetricCard(clayMan, "Total Threads", 
                std::to_string(systemState.totalThreads),
                "Active system threads",
                { 100, 150, 255, 255 }, cardHeight);
            
            // Top Process by CPU
            std::string topProcessName = "None";
            float topProcessCpu = 0.0f;
            if (!systemState.processes.empty()) {
                auto topProcess = std::max_element(systemState.processes.begin(), systemState.processes.end(),
                    [](const ProcessInfo& a, const ProcessInfo& b) {
                        return a.cpuUsagePercent < b.cpuUsagePercent;
                    });
                if (topProcess != systemState.processes.end()) {
                    topProcessName = topProcess->name;
                    topProcessCpu = topProcess->cpuUsagePercent;
                }
            }
            
            UICard::RenderMetricCard(clayMan, "Highest CPU",
                FormatPercentage(topProcessCpu),
                topProcessName,
                { 255, 150, 0, 255 }, cardHeight);
            
            // Top Process by Memory
            std::string topMemoryProcessName = "None";
            uint64_t topMemoryUsage = 0;
            if (!systemState.processes.empty()) {
                auto topMemoryProcess = std::max_element(systemState.processes.begin(), systemState.processes.end(),
                    [](const ProcessInfo& a, const ProcessInfo& b) {
                        return a.memoryUsage < b.memoryUsage;
                    });
                if (topMemoryProcess != systemState.processes.end()) {
                    topMemoryProcessName = topMemoryProcess->name;
                    topMemoryUsage = topMemoryProcess->memoryUsage;
                }
            }
            
            UICard::RenderMetricCard(clayMan, "Highest Memory",
                FormatBytes(topMemoryUsage),
                topMemoryProcessName,
                { 255, 100, 255, 255 }, cardHeight);
        });
        
    }, cardHeight + 80);
}

void ProcessesScreen::RenderProcessTable(ClayMan* clayMan, const std::vector<ProcessInfo>& processes)
{
    UICard::RenderSectionCard(clayMan, "Running Processes", [this, clayMan, &processes]() {
        
        Clay_ElementDeclaration tableContainer = {};
        tableContainer.layout.sizing = clayMan->expandXY();
        tableContainer.layout.layoutDirection = CLAY_TOP_TO_BOTTOM;
        tableContainer.layout.childGap = 8;
        
        clayMan->element(tableContainer, [this, clayMan, &processes]() {
            
            // Table header
            RenderProcessTableHeader(clayMan);
            
            // Table content with scrolling
            Clay_ElementDeclaration tableContent = {};
            tableContent.layout.sizing = clayMan->expandXY();
            tableContent.layout.layoutDirection = CLAY_TOP_TO_BOTTOM;
            tableContent.layout.childGap = 2;
            
            clayMan->element(tableContent, [this, clayMan, &processes]() {
                
                if (processes.empty()) {
                    // No processes message
                    Clay_ElementDeclaration noDataContainer = {};
                    noDataContainer.layout.sizing = clayMan->expandXfixedY(100);
                    noDataContainer.layout.childAlignment = clayMan->centerXY();
                    
                    clayMan->element(noDataContainer, [clayMan]() {
                        Clay_TextElementConfig noDataText = {};
                        noDataText.textColor = { 150, 150, 150, 255 };
                        noDataText.fontId = 0;
                        noDataText.fontSize = 14;
                        clayMan->textElement("No process data available", noDataText);
                    });
                } else {
                    // Sort processes by CPU usage (descending)
                    std::vector<ProcessInfo> sortedProcesses = processes;
                    std::sort(sortedProcesses.begin(), sortedProcesses.end(),
                        [](const ProcessInfo& a, const ProcessInfo& b) {
                            return a.cpuUsagePercent > b.cpuUsagePercent;
                        });
                    
                    // Limit to top 50 processes for performance
                    size_t maxProcesses = std::min(static_cast<size_t>(50), sortedProcesses.size());
                    
                    for (size_t i = 0; i < maxProcesses; ++i) {
                        RenderProcessRow(clayMan, sortedProcesses[i], i % 2 == 0);
                    }
                }
            });
        });
        
    }, 0); // Auto height
}

void ProcessesScreen::RenderProcessTableHeader(ClayMan* clayMan)
{
    Clay_ElementDeclaration headerRow = {};
    headerRow.layout.sizing = clayMan->expandXfixedY(40);
    headerRow.layout.layoutDirection = CLAY_LEFT_TO_RIGHT;
    headerRow.layout.childGap = 1;
    headerRow.backgroundColor = { 50, 50, 50, 255 };
    headerRow.cornerRadius = { 8, 8, 0, 0 };
    headerRow.layout.padding = clayMan->padXY(12, 8);
    
    clayMan->element(headerRow, [this, clayMan]() {
        
        // Process Name column
        Clay_ElementDeclaration nameColumn = {};
        nameColumn.layout.sizing = clayMan->expandXfixedY(40);
        nameColumn.layout.childAlignment = { CLAY_ALIGN_X_LEFT, CLAY_ALIGN_Y_CENTER };
        
        clayMan->element(nameColumn, [clayMan]() {
            Clay_TextElementConfig headerText = {};
            headerText.textColor = { 220, 220, 220, 255 };
            headerText.fontId = 0;
            headerText.fontSize = 14;
            clayMan->textElement("Process Name", headerText);
        });
        
        // PID column
        Clay_ElementDeclaration pidColumn = {};
        pidColumn.layout.sizing = clayMan->fixedSize(80, 40);
        pidColumn.layout.childAlignment = clayMan->centerXY();
        
        clayMan->element(pidColumn, [clayMan]() {
            Clay_TextElementConfig headerText = {};
            headerText.textColor = { 220, 220, 220, 255 };
            headerText.fontId = 0;
            headerText.fontSize = 14;
            clayMan->textElement("PID", headerText);
        });
        
        // CPU Usage column
        Clay_ElementDeclaration cpuColumn = {};
        cpuColumn.layout.sizing = clayMan->fixedSize(100, 40);
        cpuColumn.layout.childAlignment = clayMan->centerXY();
        
        clayMan->element(cpuColumn, [clayMan]() {
            Clay_TextElementConfig headerText = {};
            headerText.textColor = { 220, 220, 220, 255 };
            headerText.fontId = 0;
            headerText.fontSize = 14;
            clayMan->textElement("CPU %", headerText);
        });
        
        // Memory Usage column
        Clay_ElementDeclaration memoryColumn = {};
        memoryColumn.layout.sizing = clayMan->fixedSize(120, 40);
        memoryColumn.layout.childAlignment = clayMan->centerXY();
        
        clayMan->element(memoryColumn, [clayMan]() {
            Clay_TextElementConfig headerText = {};
            headerText.textColor = { 220, 220, 220, 255 };
            headerText.fontId = 0;
            headerText.fontSize = 14;
            clayMan->textElement("Memory", headerText);
        });
        
        // Status column
        Clay_ElementDeclaration statusColumn = {};
        statusColumn.layout.sizing = clayMan->fixedSize(100, 40);
        statusColumn.layout.childAlignment = clayMan->centerXY();
        
        clayMan->element(statusColumn, [clayMan]() {
            Clay_TextElementConfig headerText = {};
            headerText.textColor = { 220, 220, 220, 255 };
            headerText.fontId = 0;
            headerText.fontSize = 14;
            clayMan->textElement("Status", headerText);
        });
    });
}

void ProcessesScreen::RenderProcessRow(ClayMan* clayMan, const ProcessInfo& process, bool isEvenRow)
{
    Clay_Color rowColor = isEvenRow ? Clay_Color{ 35, 35, 35, 255 } : Clay_Color{ 30, 30, 30, 255 };
    
    Clay_ElementDeclaration processRow = {};
    processRow.layout.sizing = clayMan->expandXfixedY(36);
    processRow.layout.layoutDirection = CLAY_LEFT_TO_RIGHT;
    processRow.layout.childGap = 1;
    processRow.backgroundColor = rowColor;
    processRow.layout.padding = clayMan->padXY(12, 6);
    
    clayMan->element(processRow, [this, clayMan, &process]() {
        
        // Process Name
        Clay_ElementDeclaration nameCell = {};
        nameCell.layout.sizing = clayMan->expandXfixedY(36);
        nameCell.layout.childAlignment = { CLAY_ALIGN_X_LEFT, CLAY_ALIGN_Y_CENTER };
        
        clayMan->element(nameCell, [clayMan, &process]() {
            // Truncate long process names
            std::string displayName = process.name;
            if (displayName.length() > 25) {
                displayName = displayName.substr(0, 22) + "...";
            }
            
            Clay_TextElementConfig nameText = {};
            nameText.textColor = { 200, 200, 200, 255 };
            nameText.fontId = 0;
            nameText.fontSize = 12;
            clayMan->textElement(displayName, nameText);
        });
        
        // PID
        Clay_ElementDeclaration pidCell = {};
        pidCell.layout.sizing = clayMan->fixedSize(80, 36);
        pidCell.layout.childAlignment = clayMan->centerXY();
        
        clayMan->element(pidCell, [clayMan, &process]() {
            Clay_TextElementConfig pidText = {};
            pidText.textColor = { 160, 160, 160, 255 };
            pidText.fontId = 0;
            pidText.fontSize = 12;
            clayMan->textElement(std::to_string(process.pid), pidText);
        });
        
        // CPU Usage with color coding
        Clay_ElementDeclaration cpuCell = {};
        cpuCell.layout.sizing = clayMan->fixedSize(100, 36);
        cpuCell.layout.childAlignment = clayMan->centerXY();
        
        clayMan->element(cpuCell, [clayMan, &process]() {
            Clay_Color cpuColor = { 100, 150, 255, 255 }; // Default blue
            if (process.cpuUsagePercent > 50.0f) {
                cpuColor = { 255, 150, 0, 255 }; // Orange for high usage
            } else if (process.cpuUsagePercent > 80.0f) {
                cpuColor = { 255, 100, 100, 255 }; // Red for very high usage
            }
            
            Clay_TextElementConfig cpuText = {};
            cpuText.textColor = cpuColor;
            cpuText.fontId = 0;
            cpuText.fontSize = 12;
            clayMan->textElement(FormatPercentage(process.cpuUsagePercent), cpuText);
        });
        
        // Memory Usage
        Clay_ElementDeclaration memoryCell = {};
        memoryCell.layout.sizing = clayMan->fixedSize(120, 36);
        memoryCell.layout.childAlignment = clayMan->centerXY();
        
        clayMan->element(memoryCell, [clayMan, &process]() {
            Clay_TextElementConfig memoryText = {};
            memoryText.textColor = { 180, 180, 180, 255 };
            memoryText.fontId = 0;
            memoryText.fontSize = 12;
            clayMan->textElement(FormatBytes(process.memoryUsage), memoryText);
        });
        
        // Status with color coding
        Clay_ElementDeclaration statusCell = {};
        statusCell.layout.sizing = clayMan->fixedSize(100, 36);
        statusCell.layout.childAlignment = clayMan->centerXY();
        
        clayMan->element(statusCell, [clayMan, &process]() {
            Clay_Color statusColor = { 0, 255, 150, 255 }; // Green for running
            if (process.status != "Running") {
                statusColor = { 255, 255, 100, 255 }; // Yellow for other states
            }
            
            Clay_TextElementConfig statusText = {};
            statusText.textColor = statusColor;
            statusText.fontId = 0;
            statusText.fontSize = 12;
            clayMan->textElement(process.status, statusText);
        });
    });
}