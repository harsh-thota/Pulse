#include "graph_component.hpp"
#include <algorithm>
#include <cmath>

void GraphComponent::RenderTimeSeriesGraph(ClayMan* clayMan, const RingBuffer<float, 300>& data, Clay_Color lineColor, uint32_t height)
{
    Clay_ElementDeclaration graphContainer = {};
    graphContainer.layout.sizing = clayMan->expandXfixedY(height);
    graphContainer.backgroundColor = { 20, 20, 20, 255 };
    graphContainer.cornerRadius = { 8, 8, 8, 8 };
    graphContainer.layout.padding = clayMan->padAll(8);
    
    clayMan->element(graphContainer, [clayMan, &data, lineColor, height]()
        {
        uint32_t graphWidth = 400;
        uint32_t graphHeight = height - 16;
        RenderGridBackground(clayMan, graphWidth, graphHeight);
        
            if (data.size() > 1)
            {
            RenderDataLineDirect(clayMan, data, lineColor, graphWidth, graphHeight);
        }
    });
}

void GraphComponent::RenderGridBackground(ClayMan* clayMan, uint32_t width, uint32_t height)
{
    const uint32_t gridSpacing = 25;
    const uint32_t dotSize = 2;
    
    uint32_t dotsX = width / gridSpacing;
    uint32_t dotsY = height / gridSpacing;
    
    for (uint32_t x = 0; x <= dotsX; ++x) 
    {
        for (uint32_t y = 0; y <= dotsY; ++y) 
        {
            Clay_ElementDeclaration dot = {};
            dot.layout.sizing = clayMan->fixedSize(dotSize, dotSize);
            dot.backgroundColor = { 80, 80, 80, 150 };
            dot.cornerRadius = { 1, 1, 1, 1 };
            
            clayMan->element(dot, []() {});
        }
    }
}

void GraphComponent::RenderDataLineDirect(ClayMan* clayMan, const RingBuffer<float, 300>& data, Clay_Color lineColor, uint32_t width, uint32_t height)
{
    float maxValue = 0.0f;
    for (size_t i = 0; i < data.size(); ++i) 
    {
        maxValue = std::max(maxValue, data.get(i));
    }
    
    if (maxValue == 0.0f) maxValue = 100.0f;
    
    for (size_t i = 1; i < data.size(); ++i) 
    {
        float prevValue = data.get(i - 1);
        float currValue = data.get(i);
        
        // Manually clamp values between 0.0f and 1.0f
        float normalizedPrev = prevValue / maxValue;
        if (normalizedPrev < 0.0f) normalizedPrev = 0.0f;
        if (normalizedPrev > 1.0f) normalizedPrev = 1.0f;
        
        float normalizedCurr = currValue / maxValue;
        if (normalizedCurr < 0.0f) normalizedCurr = 0.0f;
        if (normalizedCurr > 1.0f) normalizedCurr = 1.0f;
        
        float x1 = static_cast<float>(i - 1) * width / static_cast<float>(data.size() - 1);
        float y1 = height - (normalizedPrev * height);
        float x2 = static_cast<float>(i) * width / static_cast<float>(data.size() - 1);
        float y2 = height - (normalizedCurr * height);
        
        // Calculate line segment
        float dx = x2 - x1;
        float dy = y2 - y1;
        float length = std::sqrt(dx * dx + dy * dy);
        
        if (length > 0.5f) 
        {
            Clay_ElementDeclaration lineSegment = {};
            lineSegment.layout.sizing = clayMan->fixedSize(std::max(2u, static_cast<uint32_t>(length)), 2);
            lineSegment.backgroundColor = lineColor;
            lineSegment.cornerRadius = { 1, 1, 1, 1 };
            
            clayMan->element(lineSegment, []() {});
        }
    }
}

//void GraphComponent::RenderNetworkGraph(ClayMan* clayMan, const RingBuffer<uint64_t, 300>& uploadData, const RingBuffer<uint64_t, 300>& downloadData, uint32_t height)
//{
//    Clay_ElementDeclaration graphContainer = {};
//    graphContainer.layout.sizing = clayMan->expandXfixedY(height);
//    graphContainer.backgroundColor = { 20, 20, 20, 255 };
//    graphContainer.cornerRadius = { 8, 8, 8, 8 };
//    graphContainer.layout.padding = clayMan->padAll(8);
//
//    clayMan->element(graphContainer, [clayMan, &uploadData, &downloadData, height]()
//        {
//            uint32_t graphWidth = 400;
//            uint32_t graphHeight = height - 16;
//
//            RenderGridBackground(clayMan, graphWidth, graphHeight);
//
//            // Convert and render upload/download data as needed
//            // Placeholder implementation for now
//        });
//}