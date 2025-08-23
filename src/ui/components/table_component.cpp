#include "table_component.hpp"
#include "../screens/base_screen.hpp"
#include "../application.hpp"
#include <algorithm>

void TableComponent::RenderProcessTable(ClayMan* clayMan, const std::vector<ProcessInfo>& processes, uint32_t maxRows)
{
    // Define table columns
    static const std::array<TableColumn, 5> columns = {{
        {"Process Name", 0, {220, 220, 220, 255}, 12},  // expandable width
        {"PID", 80, {160, 160, 160, 255}, 12},
        {"CPU %", 100, {255, 150, 0, 255}, 12},
        {"Memory", 120, {100, 150, 255, 255}, 12},
        {"Status", 100, {0, 255, 150, 255}, 12}
    }};

    // Sort processes by CPU usage (descending)
    std::vector<ProcessInfo> sortedProcesses = processes;
    std::sort(sortedProcesses.begin(), sortedProcesses.end(),
        [](const ProcessInfo& a, const ProcessInfo& b) {
            return a.cpuUsagePercent > b.cpuUsagePercent;
        });

    // Limit the number of processes to display
    size_t displayCount = std::min(static_cast<size_t>(maxRows), sortedProcesses.size());

    RenderFixedTable(clayMan, columns.data(), columns.size(),
        [&sortedProcesses](ClayMan* clayMan, size_t rowIndex) {
            if (rowIndex < sortedProcesses.size()) {
                RenderProcessRow(clayMan, sortedProcesses[rowIndex], nullptr, rowIndex % 2 == 0);
            }
        }, displayCount, 600);
}

void TableComponent::RenderFixedTable(ClayMan* clayMan, const TableColumn* columns, size_t columnCount, 
                                     std::function<void(ClayMan*, size_t)> renderRow, size_t rowCount, uint32_t maxHeight)
{
    Clay_ElementDeclaration tableContainer = {};
    tableContainer.layout.sizing = clayMan->expandXY();
    tableContainer.layout.layoutDirection = CLAY_TOP_TO_BOTTOM;
    tableContainer.backgroundColor = { 25, 25, 25, 255 };
    tableContainer.cornerRadius = { 8, 8, 8, 8 };

    clayMan->element(tableContainer, [clayMan, columns, columnCount, renderRow, rowCount, maxHeight]() {
        // Render table header
        RenderTableHeader(clayMan, columns, columnCount);

        // Render table body
        Clay_ElementDeclaration tableBody = {};
        tableBody.layout.sizing = clayMan->expandXY();
        tableBody.layout.layoutDirection = CLAY_TOP_TO_BOTTOM;
        tableBody.layout.childGap = 1;

        clayMan->element(tableBody, [clayMan, renderRow, rowCount]() {
            for (size_t i = 0; i < rowCount; ++i) {
                renderRow(clayMan, i);
            }
        });
    });
}

void TableComponent::RenderTableHeader(ClayMan* clayMan, const TableColumn* columns, size_t columnCount)
{
    Clay_ElementDeclaration headerRow = {};
    headerRow.layout.sizing = clayMan->expandXfixedY(40);
    headerRow.layout.layoutDirection = CLAY_LEFT_TO_RIGHT;
    headerRow.layout.childGap = 1;
    headerRow.backgroundColor = { 50, 50, 50, 255 };
    headerRow.cornerRadius = { 8, 8, 0, 0 };
    headerRow.layout.padding = clayMan->padXY(12, 8);

    clayMan->element(headerRow, [clayMan, columns, columnCount]() {
        for (size_t i = 0; i < columnCount; ++i) {
            Clay_ElementDeclaration columnHeader = {};
            if (columns[i].width == 0) {
                columnHeader.layout.sizing = clayMan->expandXfixedY(40);
            } else {
                columnHeader.layout.sizing = clayMan->fixedSize(columns[i].width, 40);
            }
            columnHeader.layout.childAlignment = { CLAY_ALIGN_X_LEFT, CLAY_ALIGN_Y_CENTER };

            clayMan->element(columnHeader, [clayMan, &columns, i]() {
                Clay_TextElementConfig headerText = {};
                headerText.textColor = columns[i].textColor;
                headerText.fontId = 0;
                headerText.fontSize = columns[i].fontSize;
                clayMan->textElement(columns[i].title, headerText);
            });
        }
    });
}

void TableComponent::RenderProcessRow(ClayMan* clayMan, const ProcessInfo& process, const TableColumn* columns, bool isAlternate)
{
    Clay_Color rowColor = isAlternate ? Clay_Color{ 35, 35, 35, 255 } : Clay_Color{ 30, 30, 30, 255 };

    Clay_ElementDeclaration processRow = {};
    processRow.layout.sizing = clayMan->expandXfixedY(36);
    processRow.layout.layoutDirection = CLAY_LEFT_TO_RIGHT;
    processRow.layout.childGap = 1;
    processRow.backgroundColor = rowColor;
    processRow.layout.padding = clayMan->padXY(12, 6);

    clayMan->element(processRow, [clayMan, &process]() {
        // Process Name (expandable column)
        Clay_ElementDeclaration nameCell = {};
        nameCell.layout.sizing = clayMan->expandXfixedY(36);
        nameCell.layout.childAlignment = { CLAY_ALIGN_X_LEFT, CLAY_ALIGN_Y_CENTER };

        clayMan->element(nameCell, [clayMan, &process]() {
            std::string displayName = process.name;
            if (displayName.length() > 25) {
                displayName = displayName.substr(0, 22) + "...";
            }
            RenderTableCell(clayMan, displayName, 0, { 200, 200, 200, 255 });
        });

        // PID (fixed width)
        RenderTableCell(clayMan, std::to_string(process.pid), 80, { 160, 160, 160, 255 });

        // CPU Usage (with color coding)
        Clay_Color cpuColor = { 100, 150, 255, 255 };
        if (process.cpuUsagePercent > 50.0f) {
            cpuColor = { 255, 150, 0, 255 };
        } else if (process.cpuUsagePercent > 80.0f) {
            cpuColor = { 255, 100, 100, 255 };
        }
        RenderTableCell(clayMan, Application::FormatPercentage(process.cpuUsagePercent), 100, cpuColor);

        // Memory Usage
        RenderTableCell(clayMan, Application::FormatBytes(process.memoryUsage), 120, { 180, 180, 180, 255 });

        // Status (with color coding)
        Clay_Color statusColor = process.status == "Running" ? 
            Clay_Color{ 0, 255, 150, 255 } : Clay_Color{ 255, 255, 100, 255 };
        RenderTableCell(clayMan, process.status, 100, statusColor);
    });
}

void TableComponent::RenderTableCell(ClayMan* clayMan, const std::string& text, uint32_t width, Clay_Color color, uint32_t fontSize)
{
    Clay_ElementDeclaration cell = {};
    if (width == 0) {
        cell.layout.sizing = clayMan->expandXfixedY(36);
        cell.layout.childAlignment = { CLAY_ALIGN_X_LEFT, CLAY_ALIGN_Y_CENTER };
    } else {
        cell.layout.sizing = clayMan->fixedSize(width, 36);
        cell.layout.childAlignment = clayMan->centerXY();
    }

    clayMan->element(cell, [clayMan, &text, color, fontSize]() {
        Clay_TextElementConfig cellText = {};
        cellText.textColor = color;
        cellText.fontId = 0;
        cellText.fontSize = fontSize;
        clayMan->textElement(text, cellText);
    });
}