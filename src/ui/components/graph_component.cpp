#include "graph_component.hpp"
#include <algorithm>
#include <cmath>
#include <iostream>
#include <sstream>
#include <iomanip>

// Static queue implementation
std::vector<GraphRenderInfo>& GraphComponent::GetGraphQueue()
{
    static std::vector<GraphRenderInfo> graphQueue;
    return graphQueue;
}

void GraphComponent::RenderTimeSeriesGraph(ClayMan* clayMan, const RingBuffer<float, 300>& data, Clay_Color lineColor, uint32_t height)
{
    // Calculate responsive sizes based on container height
    uint32_t headerHeight = std::max(30u, static_cast<uint32_t>(height * 0.15f));
    uint32_t bottomLabelHeight = std::max(25u, static_cast<uint32_t>(height * 0.12f));
    uint32_t sideLabelsWidth = std::max(50u, static_cast<uint32_t>(height * 0.25f));
    uint32_t graphAreaHeight = height - headerHeight - bottomLabelHeight;
    uint32_t padding = std::max(8u, static_cast<uint32_t>(height * 0.04f));
    
    // Main graph container with better styling
    Clay_ElementDeclaration graphContainer = {};
    graphContainer.layout.sizing = clayMan->expandXfixedY(height);
    graphContainer.backgroundColor = { 30, 30, 30, 255 }; // Darker background
    graphContainer.cornerRadius = { 12, 12, 12, 12 }; // More rounded corners
    graphContainer.layout.padding = clayMan->padAll(padding);
    
    clayMan->element(graphContainer, [clayMan, &data, lineColor, height, headerHeight, graphAreaHeight, bottomLabelHeight, sideLabelsWidth]() {
        
        // Header with title and current value
        Clay_ElementDeclaration headerArea = {};
        headerArea.layout.sizing = clayMan->expandXfixedY(headerHeight);
        headerArea.layout.layoutDirection = CLAY_LEFT_TO_RIGHT;
        headerArea.layout.childAlignment = { CLAY_ALIGN_X_LEFT, CLAY_ALIGN_Y_CENTER };
        headerArea.layout.padding = clayMan->padXY(8, 4);
        
        clayMan->element(headerArea, [clayMan, &data, lineColor, headerHeight]() {
            // Current value display
            Clay_ElementDeclaration currentValueArea = {};
            currentValueArea.layout.sizing = clayMan->expandXfixedY(headerHeight);
            currentValueArea.layout.childAlignment = { CLAY_ALIGN_X_LEFT, CLAY_ALIGN_Y_CENTER };
            
            clayMan->element(currentValueArea, [clayMan, &data, lineColor, headerHeight]() {
                float currentValue = data.size() > 0 ? data.latest() : 0.0f;
                std::ostringstream oss;
                oss << std::fixed << std::setprecision(1) << currentValue << "%";
                std::string valueStr = oss.str();
                
                Clay_TextElementConfig valueText = {};
                valueText.textColor = lineColor;
                valueText.fontId = 0;
                valueText.fontSize = std::max(16u, static_cast<uint32_t>(headerHeight * 0.5f));
                clayMan->textElement(valueStr, valueText);
            });
            
            // Graph title
            Clay_ElementDeclaration titleArea = {};
            titleArea.layout.sizing = clayMan->expandXfixedY(headerHeight);
            titleArea.layout.childAlignment = { CLAY_ALIGN_X_RIGHT, CLAY_ALIGN_Y_CENTER };
            
            clayMan->element(titleArea, [clayMan, lineColor, headerHeight]() {
                Clay_TextElementConfig titleText = {};
                titleText.textColor = { 180, 180, 180, 255 };
                titleText.fontId = 0;
                titleText.fontSize = std::max(12u, static_cast<uint32_t>(headerHeight * 0.4f));
                clayMan->textElement("Real-time Graph", titleText);
            });
        });
        
        // Main graph area with grid background and improved visualization
        Clay_ElementDeclaration graphArea = {};
        graphArea.layout.sizing = clayMan->expandXfixedY(graphAreaHeight);
        graphArea.layout.layoutDirection = CLAY_LEFT_TO_RIGHT;
        
        clayMan->element(graphArea, [clayMan, &data, lineColor, graphAreaHeight, sideLabelsWidth]() {
            
            // Y-axis labels
            Clay_ElementDeclaration yAxisLabels = {};
            yAxisLabels.layout.sizing = clayMan->fixedSize(sideLabelsWidth, graphAreaHeight);
            yAxisLabels.layout.layoutDirection = CLAY_TOP_TO_BOTTOM;
            yAxisLabels.layout.childAlignment = { CLAY_ALIGN_X_RIGHT, CLAY_ALIGN_Y_CENTER };
            yAxisLabels.backgroundColor = { 25, 25, 25, 255 };
            
            clayMan->element(yAxisLabels, [clayMan, sideLabelsWidth, graphAreaHeight]() {
                // Create 5 Y-axis labels (0%, 25%, 50%, 75%, 100%)
                std::vector<std::string> labels = {"100%", "75%", "50%", "25%", "0%"};
                uint32_t labelHeight = graphAreaHeight / 5;
                
                for (const auto& label : labels) {
                    Clay_ElementDeclaration labelContainer = {};
                    labelContainer.layout.sizing = clayMan->fixedSize(sideLabelsWidth, labelHeight);
                    labelContainer.layout.childAlignment = { CLAY_ALIGN_X_RIGHT, CLAY_ALIGN_Y_CENTER };
                    labelContainer.layout.padding = clayMan->padRight(8);
                    
                    clayMan->element(labelContainer, [clayMan, &label]() {
                        Clay_TextElementConfig labelText = {};
                        labelText.textColor = { 120, 120, 120, 255 };
                        labelText.fontId = 0;
                        labelText.fontSize = 10;
                        clayMan->textElement(label, labelText);
                    });
                }
            });
            
            // Graph content area with grid and data visualization
            Clay_ElementDeclaration graphContentArea = {};
            graphContentArea.layout.sizing = clayMan->expandXfixedY(graphAreaHeight);
            graphContentArea.backgroundColor = { 18, 18, 18, 255 }; // Very dark background
            graphContentArea.cornerRadius = { 6, 6, 6, 6 };
            graphContentArea.layout.padding = clayMan->padAll(4);
            
            // Clip overflowing bars to the graph content area
            graphContentArea.clip.horizontal = true;
            graphContentArea.clip.vertical = false;
            clayMan->element(graphContentArea, [clayMan, &data, lineColor, graphAreaHeight]() {
                RenderGridAndBars(clayMan, data, lineColor, graphAreaHeight - 8);
            });
        });
        
        // Bottom time labels
        Clay_ElementDeclaration bottomLabels = {};
    bottomLabels.layout.sizing = clayMan->expandXfixedY(bottomLabelHeight);
        bottomLabels.layout.layoutDirection = CLAY_LEFT_TO_RIGHT;
        bottomLabels.layout.childAlignment = { CLAY_ALIGN_X_CENTER, CLAY_ALIGN_Y_CENTER };
        bottomLabels.layout.padding = clayMan->padXY(sideLabelsWidth, 4);
        
        clayMan->element(bottomLabels, [clayMan, bottomLabelHeight]() {
            // Time range labels (seconds)
            std::vector<std::string> timeLabels = {"5s", "4s", "3s", "2s", "1s", "Now"};
            
            for (const auto& timeLabel : timeLabels) {
                Clay_ElementDeclaration timeContainer = {};
                timeContainer.layout.sizing = clayMan->expandXfixedY(bottomLabelHeight);
                timeContainer.layout.childAlignment = clayMan->centerXY();
                
                clayMan->element(timeContainer, [clayMan, &timeLabel, bottomLabelHeight]() {
                    Clay_TextElementConfig timeText = {};
                    timeText.textColor = { 100, 100, 100, 255 };
                    timeText.fontId = 0;
                    timeText.fontSize = std::max(9u, static_cast<uint32_t>(bottomLabelHeight * 0.4f));
                    clayMan->textElement(timeLabel, timeText);
                });
            }
        });
    });
}

void GraphComponent::RenderGridAndBars(ClayMan* clayMan, const RingBuffer<float, 300>& data, Clay_Color lineColor, uint32_t height)
{
    if (data.size() < 1) {
        // Render "No Data" indicator
        Clay_ElementDeclaration noDataContainer = {};
        noDataContainer.layout.sizing = clayMan->expandXY();
        noDataContainer.layout.childAlignment = clayMan->centerXY();
        
        clayMan->element(noDataContainer, [clayMan, height]() {
            Clay_TextElementConfig noDataText = {};
            noDataText.textColor = { 120, 120, 120, 255 };
            noDataText.fontId = 0;
            noDataText.fontSize = std::max(14u, static_cast<uint32_t>(height * 0.1f));
            clayMan->textElement("No Data Available", noDataText);
        });
        return;
    }
    
    // Create layered container for grid + bars
    Clay_ElementDeclaration layeredContainer = {};
    layeredContainer.layout.sizing = clayMan->expandXY();
    
    clayMan->element(layeredContainer, [clayMan, &data, lineColor, height]() {
        
        // Background grid lines
        RenderGridLines(clayMan, height);
        
        // Data visualization with thick bars
        RenderThickBars(clayMan, data, lineColor, height);
    });
}

void GraphComponent::RenderGridLines(ClayMan* clayMan, uint32_t height)
{
    // Create grid overlay
    Clay_ElementDeclaration gridContainer = {};
    gridContainer.layout.sizing = clayMan->expandXY();
    gridContainer.layout.layoutDirection = CLAY_TOP_TO_BOTTOM;
    
    clayMan->element(gridContainer, [clayMan, height]() {
        // Horizontal grid lines (5 lines for 0%, 25%, 50%, 75%, 100%)
        uint32_t lineHeight = height / 5;
        
        for (int i = 0; i < 5; ++i) {
            Clay_ElementDeclaration gridLine = {};
            gridLine.layout.sizing = clayMan->expandXfixedY(lineHeight);
            gridLine.layout.layoutDirection = CLAY_TOP_TO_BOTTOM;
            gridLine.layout.childAlignment = { CLAY_ALIGN_X_CENTER, CLAY_ALIGN_Y_TOP };
            
            clayMan->element(gridLine, [clayMan]() {
                // Thin horizontal line
                Clay_ElementDeclaration line = {};
                line.layout.sizing = clayMan->expandXfixedY(1);
                line.backgroundColor = { 40, 40, 40, 255 }; // Subtle grid color
                clayMan->element(line, []() {});
            });
        }
    });
}

void GraphComponent::RenderThickBars(ClayMan* clayMan, const RingBuffer<float, 300>& data, Clay_Color lineColor, uint32_t height)
{
    // Find max value for dynamic scaling
    float maxValue = 1.0f;
    for (size_t i = 0; i < data.size(); ++i) {
        maxValue = std::max(maxValue, data.get(i));
    }
    maxValue = std::max(maxValue, 100.0f); // Ensure minimum scale
    
    // Create container for thick bars
    Clay_ElementDeclaration barsContainer = {};
    barsContainer.layout.sizing = clayMan->expandXY();
    barsContainer.layout.layoutDirection = CLAY_LEFT_TO_RIGHT;
    barsContainer.layout.childGap = 1; // Minimal gap between bars
    barsContainer.layout.childAlignment = { CLAY_ALIGN_X_LEFT, CLAY_ALIGN_Y_BOTTOM };
    
    clayMan->element(barsContainer, [clayMan, &data, lineColor, height, maxValue]() {
        
        // Calculate responsive bar dimensions
        uint32_t minBarWidth = 3; // Minimum bar width for visibility
        uint32_t maxBarWidth = 8; // Maximum bar width
        uint32_t barWidth = std::clamp(
            static_cast<uint32_t>(height * 0.02f), 
            minBarWidth, 
            maxBarWidth
        );
        
    // Limit number of bars to last 5 samples (~5 seconds)
    size_t maxBars = 5;
        size_t available = data.size();
        size_t sampleCount = std::min(available, maxBars);
        size_t startIdx = available > sampleCount ? available - sampleCount : 0;
        
        for (size_t idx = startIdx; idx < available; ++idx) {
            float value = data.get(idx);
            float normalizedValue = std::clamp(value / maxValue, 0.0f, 1.0f);
            
            // Calculate bar height with minimum visibility
            uint32_t barHeight = std::max(3u, static_cast<uint32_t>(normalizedValue * height * 0.9f));
            
            // Create thick, visible bar with gradient effect
            Clay_ElementDeclaration bar = {};
            bar.layout.sizing = clayMan->fixedSize(barWidth, barHeight);
            
            // Create gradient effect by varying opacity based on value
            Clay_Color barColor = lineColor;
            if (normalizedValue < 0.3f) {
                barColor.a = static_cast<uint8_t>(80 + normalizedValue * 175); // Fade low values
            }
            
            bar.backgroundColor = barColor;
            bar.cornerRadius = { 1, 1, 0, 0 }; // Rounded top corners
            
            clayMan->element(bar, []() {});
        }
    });
}

void GraphComponent::RenderBarChart(ClayMan* clayMan, const RingBuffer<float, 300>& data, Clay_Color lineColor, uint32_t height)
{
    // Use the new improved rendering method
    RenderGridAndBars(clayMan, data, lineColor, height);
}

// Queue methods for future SDL2 integration
void GraphComponent::QueueGraphForRendering(const GraphRenderInfo& info)
{
    GetGraphQueue().push_back(info);
}

void GraphComponent::RenderAllQueuedGraphs(SDL_Renderer* renderer)
{
    // Future: implement precise SDL2 line rendering
    // For now, Clay handles all visualization
}

void GraphComponent::ClearGraphQueue()
{
    GetGraphQueue().clear();
}