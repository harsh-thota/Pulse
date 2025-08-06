#include "data_collector.hpp"
#include "../platform/windows/windows_system_monitor.hpp"
#include <iostream>

DataCollector::DataCollector() : lastUpdate_(std::chrono::steady_clock::now()) {}
DataCollector::~DataCollector() {}

bool DataCollector::Initialize()
{
	systemMonitor_ = std::make_unique<WindowsSystemMonitor>();
	
	if (!systemMonitor_)
	{
		std::cerr << "Failed to create sys monitor\n";
		return false;
	}

	systemState_.systemName = systemMonitor_->GetSystemName();
	systemState_.cpuName = systemMonitor_->GetCPUName();
	systemState_.coreCount = systemMonitor_->GetCoreCount();
	systemState_.totalRAMBytes = systemMonitor_->GetTotalMemory();

	std::cout << "DataCollector Initialized: " << systemState_.cpuName << " (" << systemState_.coreCount << " cores)\n";
	return true;
}

void DataCollector::Update()
{
	auto now = std::chrono::steady_clock::now();
	auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(now - lastUpdate_).count();

	if (elapsed >= 1000)
	{
		UpdateSystemMetrics();
		lastUpdate_ = now;
	}
}

void DataCollector::UpdateSystemMetrics()
{
	if (!systemMonitor_) return;

	systemMonitor_->UpdateSystemMetrics();
	
	systemState_.cpuUsagePercent = systemMonitor_->GetCPUUsage();
	systemState_.usedRAMBytes = systemMonitor_->GetUsedMemory();
	systemState_.memoryUsagePercent = systemMonitor_->GetMemoryUsagePercent();
	systemState_.cpuHistory.push(systemState_.cpuUsagePercent);
	systemState_.memoryHistory.push(systemState_.memoryUsagePercent);
	systemState_.lastUpdateTime = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
}