#pragma once
#include "../../core/interfaces/i_system_monitor.hpp"
#include <Windows.h>
#include <string>
#include <cstdint>

class WindowsSystemMonitor : public ISystemMonitor
{
public:
	WindowsSystemMonitor();
	~WindowsSystemMonitor() override = default;

	void UpdateSystemMetrics() override;
	float GetCPUUsage() const override;
	uint64_t GetTotalMemory() const override;
	uint64_t GetUsedMemory() const override;
	float GetMemoryUsagePercent() const override;
	std::string GetSystemName() const override;
	std::string GetCPUName() const override;
	uint32_t GetCoreCount() const override;

private:
	float cpuUsage_ = 0.0f;
	uint64_t totalMemory_ = 0;
	uint64_t usedMemory_ = 0;
	std::string systemName_ = "Windows";
	std::string cpuName_ = "Unknown CPU";
	uint32_t coreCount_ = 0;

	ULARGE_INTEGER lastCPU_ = {};
	ULARGE_INTEGER lastSysCPU_ = {};
	ULARGE_INTEGER lastUserCPU_ = {};

	void InitializeSystemInfo();
	void UpdateCPUUsage();
	void UpdateMemoryInfo();
};