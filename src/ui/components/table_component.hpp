#pragma once
#include "clayman.hpp"
#include "../../core/system_state.hpp"
#include <string>
#include <array>
#include <functional>

struct TableColumn {
    const char* title;
    uint32_t width;
    Clay_Color textColor = { 200, 200, 200, 255 };
    uint32_t fontSize = 12;
};

class TableComponent {
public:
    static void RenderProcessTable(ClayMan* clayMan, const std::vector<ProcessInfo>& processes, uint32_t maxRows = 20);
    static void RenderFixedTable(ClayMan* clayMan, const TableColumn* columns, size_t columnCount, std::function<void(ClayMan*, size_t)> renderRow, size_t rowCount, uint32_t maxHeight = 400);

private:
    static void RenderTableHeader(ClayMan* clayMan, const TableColumn* columns, size_t columnCount);
    static void RenderProcessRow(ClayMan* clayMan, const ProcessInfo& process, const TableColumn* columns, bool isAlternate);
    static void RenderTableCell(ClayMan* clayMan, const std::string& text, uint32_t width, Clay_Color color = { 255, 255, 255, 255 }, uint32_t fontSize = 11);
};