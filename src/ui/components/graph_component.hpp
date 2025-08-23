#pragma once
#include "clayman.hpp"
#include "../../core/ring_buffer.hpp"
#include <vector>
#include <SDL.h>

struct GraphRenderInfo {
    int x, y, width, height;
    RingBuffer<float, 300> data;
    Clay_Color lineColor;
    std::string title;
    float maxValue;
};

class GraphComponent {
public:
    // Enhanced Clay-based graph rendering with grid, labels, and thick bars
    static void RenderTimeSeriesGraph(ClayMan* clayMan, const RingBuffer<float, 300>& data, Clay_Color lineColor, uint32_t height = 200);
    
    // SDL2 graph rendering methods (for future enhancement)
    static void QueueGraphForRendering(const GraphRenderInfo& info);
    static void RenderAllQueuedGraphs(SDL_Renderer* renderer);
    static void ClearGraphQueue();

private:
    // Enhanced Clay-based rendering methods
    static void RenderGridAndBars(ClayMan* clayMan, const RingBuffer<float, 300>& data, Clay_Color lineColor, uint32_t height);
    static void RenderGridLines(ClayMan* clayMan, uint32_t height);
    static void RenderThickBars(ClayMan* clayMan, const RingBuffer<float, 300>& data, Clay_Color lineColor, uint32_t height);
    
    // Legacy method for compatibility
    static void RenderBarChart(ClayMan* clayMan, const RingBuffer<float, 300>& data, Clay_Color lineColor, uint32_t height);
    
    // Graph queue for future SDL2 rendering
    static std::vector<GraphRenderInfo>& GetGraphQueue();
};