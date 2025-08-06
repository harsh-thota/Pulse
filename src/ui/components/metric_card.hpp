#pragma once
#include "clayman.hpp"
#include "../../core/ring_buffer.hpp"
#include <string>

class MetricCard {
public:
    static void RenderHardwareMetric(ClayMan* clayMan, const char* title, const char* details, float currentValue, const RingBuffer<float, 300>& history, Clay_Color accentColor);
    static void RenderSimpleMetric(ClayMan* clayMan, const char* title, const char* value, Clay_Color valueColor, const char* subtitle = nullptr);

//private:
//    static const char* FormatPercentageStatic(float percentage);
//    static const char* FormatBytesStatic(uint64_t bytes);
};