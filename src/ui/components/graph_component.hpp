#pragma once
#include "clayman.hpp"
#include "../../core/ring_buffer.hpp"
#include <vector>

class GraphComponent {
public:
    static void RenderTimeSeriesGraph(ClayMan* clayMan, const RingBuffer<float, 300>& data, Clay_Color lineColor, uint32_t height = 200);
    //static void RenderNetworkGraph(ClayMan* clayMan, const RingBuffer<uint64_t, 300>& uploadData, const RingBuffer<uint64_t, 300>& downloadData, uint32_t height = 200);

private:
    static void RenderGridBackground(ClayMan* clayMan, uint32_t width, uint32_t height);
    static void RenderDataLineDirect(ClayMan* clayMan, const RingBuffer<float, 300>& data, Clay_Color lineColor, uint32_t width, uint32_t height);
};