#pragma once
#include <string>

class ISystemMonitor
{
public:
	virtual ~ISystemMonitor() = default;

	virtual void UpdateSystemMetrics() = 0;
	virtual float GetCPUUsage() const = 0;
	virtual uint64_t GetTotalMemory() const = 0;
	virtual uint64_t GetUsedMemory() const = 0;
	virtual float GetMemoryUsagePercent() const = 0;
	virtual std::string GetSystemName() const = 0;
	virtual std::string GetCPUName() const = 0;
	virtual uint32_t GetCoreCount() const = 0;
};