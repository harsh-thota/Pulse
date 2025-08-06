#include "table_component.hpp"
#include "ui_card.hpp"
#include "text_components.hpp"
#include "layout_components.hpp"
#include "../screens/base_screen.hpp"
#include <sstream>
#include <iomanip>

static const TableColumn PROCESS_COLUMNS[] = 
{
    {"Name", 250, {200, 200, 200, 255}, 12},
    {"PID", 80, {200, 200, 200, 255}, 12},
    {"CPU", 100, {200, 200, 200, 255}, 12},
    {"Memory", 120, {200, 200, 200, 255}, 12}
};
static constexpr size_t PROCESS_COLUMN_COUNT = sizeof(PROCESS_COLUMNS) / sizeof(PROCESS_COLUMNS[0]);

static std::string FormatBytes(uint64_t bytes)
{
    std::ostringstream oss;
    if (bytes >= 1024 * 1024 * 1024) {
        oss << std::fixed << std::setprecision(1) << (static_cast<double>(bytes) / (1024 * 1024 * 1024)) << "GB";
    } else if (bytes >= 1024 * 1024) {
        oss << std::fixed << std::setprecision(1) << (static_cast<double>(bytes) / (1024 * 1024)) << "MB";
    } else if (bytes >= 1024) {
        oss << std::fixed << std::setprecision(1) << (static_cast<double>(bytes) / 1024) << "KB";
    } else {
        oss << bytes << "B";
    }
    return oss.str();
}

static std::string FormatPercentage(float percentage)
{
    std::ostringstream oss;
    oss << std::fixed << std::setprecision(1) << percentage << "%";
    return oss.str();
}

void TableComponent::RenderProcessTable(ClayMan* clayMan, const std::vector<ProcessInfo>& processes, uint32_t maxRows)
{
    UICard::Render(clayMan, [clayMan, &processes, maxRows]()
        {
            LayoutComponents::RenderVerticalStack(clayMan, [clayMan, &processes, maxRows]()
                {
                    RenderTableHeader(clayMan, PROCESS_COLUMNS, PROCESS_COLUMN_COUNT);

                    Clay_ElementDeclaration rowsContainer = {};
                    rowsContainer.layout.sizing = clayMan->expandXfixedY(350);
                    rowsContainer.layout.layoutDirection = CLAY_TOP_TO_BOTTOM;

                    clayMan->element(rowsContainer, [clayMan, &processes, maxRows]()
                        {
                            size_t displayCount = std::min(processes.size(), static_cast<size_t>(maxRows));
                            if (displayCount == 0)
                            {
                                Clay_ElementDeclaration emptyContainer = {};
                                emptyContainer.layout.sizing = clayMan->expandXfixedY(100);
                                emptyContainer.layout.childAlignment = clayMan->centerXY();

                                clayMan->element(emptyContainer, [clayMan]()
                                    {
                                        TextComponents::RenderSubtitle(clayMan, "Loading processes...", 14, { 150, 150, 150, 255 });
                                    });
                            }
                            else
                            {
                                for (size_t i = 0; i < displayCount; ++i)
                                {
                                    RenderProcessRow(clayMan, processes[i], PROCESS_COLUMNS, i % 2 == 0);
                                }
                            }
                        });
                }, 0);
        }, 400);
}

void TableComponent::RenderFixedTable(ClayMan* clayMan, const TableColumn* columns, size_t columnCount, std::function<void(ClayMan*, size_t)> renderRow, size_t rowCount, uint32_t maxHeight)
{
    UICard::Render(clayMan, [clayMan, columns, columnCount, renderRow, rowCount, maxHeight]()
        {
            LayoutComponents::RenderVerticalStack(clayMan, [clayMan, columns, columnCount, renderRow, rowCount, maxHeight]()
                {
                    RenderTableHeader(clayMan, columns, columnCount);

                    Clay_ElementDeclaration rowsContainer = {};
                    rowsContainer.layout.sizing = clayMan->expandXfixedY(maxHeight - 50);
                    rowsContainer.layout.layoutDirection = CLAY_TOP_TO_BOTTOM;

                    clayMan->element(rowsContainer, [clayMan, renderRow, rowCount]()
                        {
                            for (size_t i = 0; i < rowCount; ++i)
                            {
                                renderRow(clayMan, i);
                            }
                        });
                }, 0);
        }, maxHeight);
}

void TableComponent::RenderTableHeader(ClayMan* clayMan, const TableColumn* columns, size_t columnCount)
{
    Clay_ElementDeclaration headerContainer = {};
    headerContainer.layout.sizing = clayMan->expandXfixedY(40);
    headerContainer.layout.layoutDirection = CLAY_LEFT_TO_RIGHT;
    headerContainer.layout.childGap = 8;
    headerContainer.layout.padding = clayMan->padAll(12);
    headerContainer.backgroundColor = { 45, 45, 45, 255 };
    headerContainer.cornerRadius = { 8, 8, 0, 0 };

    clayMan->element(headerContainer, [clayMan, columns, columnCount]()
        {
            for (size_t i = 0; i < columnCount; ++i)
            {
                const TableColumn& column = columns[i];

                Clay_ElementDeclaration cellContainer = {};
                cellContainer.layout.sizing = clayMan->expandYfixedX(column.width);
                cellContainer.layout.childAlignment = clayMan->centerXY();

                clayMan->element(cellContainer, [clayMan, &column]()
                    {
                        TextComponents::RenderTitle(clayMan, column.title, column.fontSize, column.textColor);
                    });
            }
        });
}

void TableComponent::RenderProcessRow(ClayMan* clayMan, const ProcessInfo& process, const TableColumn* columns, bool isAlternate)
{
    Clay_ElementDeclaration rowContainer = {};
    rowContainer.layout.sizing = clayMan->expandXfixedY(35);
    rowContainer.layout.layoutDirection = CLAY_LEFT_TO_RIGHT;
    rowContainer.layout.childGap = 8;
    rowContainer.layout.padding = clayMan->padAll(12);
    rowContainer.backgroundColor = isAlternate ? Clay_Color{ 35, 35, 35, 255 } : Clay_Color{ 25, 25, 25, 0 };

    clayMan->element(rowContainer, [clayMan, &process, columns]()
        {
            RenderTableCell(clayMan, process.name, columns[0].width, { 255, 255, 255, 255 });
            RenderTableCell(clayMan, std::to_string(process.pid), columns[1].width, { 180, 180, 180, 255 });
            RenderTableCell(clayMan, FormatPercentage(process.cpuUsagePercent), columns[2].width, { 0, 255, 150, 255 });
            RenderTableCell(clayMan, FormatBytes(process.memoryUsage), columns[3].width, { 100, 150, 255, 255 });
        });
}

void TableComponent::RenderTableCell(ClayMan* clayMan, const std::string& text, uint32_t width, Clay_Color color, uint32_t fontSize)
{
    Clay_ElementDeclaration cellContainer = {};
    cellContainer.layout.sizing = clayMan->expandYfixedX(width);
    cellContainer.layout.childAlignment = clayMan->centerXY();

    clayMan->element(cellContainer, [clayMan, &text, color, fontSize]()
        {
            TextComponents::RenderTitle(clayMan, text, fontSize, color);
        });
}