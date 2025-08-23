#include "data_collector.hpp"
#include "../platform/windows/windows_system_monitor.hpp"
#include <iostream>

DataCollector::DataCollector() : lastUpdate_(std::chrono::steady_clock::now()) {}
DataCollector::~DataCollector() {}

bool DataCollector::Initialize()
{
	systemMonitor_ = std::unique_ptr<ISystemMonitor>(new WindowsSystemMonitor());
	
	if (!systemMonitor_)
	{
		std::cerr << "Failed to create sys monitor\n";
		return false;
	}

	systemState_.systemName = systemMonitor_->GetSystemName();
	systemState_.cpuName = systemMonitor_->GetCPUName();
	systemState_.coreCount = systemMonitor_->GetCoreCount();
	systemState_.totalRAMBytes = systemMonitor_->GetTotalMemory();
	systemState_.gpuName = systemMonitor_->GetGPUName();
	systemState_.primaryDiskName = systemMonitor_->GetPrimaryDiskName();
	systemState_.primaryNetworkInterface = systemMonitor_->GetPrimaryNetworkInterface();

	std::cout << "DataCollector Initialized: " << systemState_.cpuName << " (" << systemState_.coreCount << " cores)\n";
	std::cout << "GPU: " << systemState_.gpuName << "\n";
	std::cout << "Primary Disk: " << systemState_.primaryDiskName << "\n";
	std::cout << "Network Interface: " << systemState_.primaryNetworkInterface << "\n";
	
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
	
	// Update CPU metrics
	systemState_.cpuUsagePercent = systemMonitor_->GetCPUUsage();
	systemState_.cpuHistory.push(systemState_.cpuUsagePercent);
	
	// Update memory metrics
	systemState_.usedRAMBytes = systemMonitor_->GetUsedMemory();
	systemState_.memoryUsagePercent = systemMonitor_->GetMemoryUsagePercent();
	systemState_.memoryHistory.push(systemState_.memoryUsagePercent);
	
	// Update GPU metrics
	systemState_.gpuUsagePercent = systemMonitor_->GetGPUUsage();
	systemState_.gpuHistory.push(systemState_.gpuUsagePercent);
	systemState_.gpuMemoryUsed = systemMonitor_->GetGPUMemoryUsed();
	systemState_.gpuMemoryTotal = systemMonitor_->GetGPUMemoryTotal();
	
	// Update disk metrics
	systemState_.diskUsagePercent = systemMonitor_->GetDiskUsage();
	systemState_.diskReadBytesPerSec = systemMonitor_->GetDiskReadBytesPerSec();
	systemState_.diskWriteBytesPerSec = systemMonitor_->GetDiskWriteBytesPerSec();
	systemState_.diskHistory.push(systemState_.diskUsagePercent);
	
	// Update network metrics
	systemState_.networkUsagePercent = systemMonitor_->GetNetworkUsage();
	systemState_.uploadBytesPerSec = systemMonitor_->GetNetworkUploadBytesPerSec();
	systemState_.downloadBytesPerSec = systemMonitor_->GetNetworkDownloadBytesPerSec();
	systemState_.networkHistory.push(systemState_.networkUsagePercent);
	systemState_.uploadHistory.push(systemState_.uploadBytesPerSec);
	systemState_.downloadHistory.push(systemState_.downloadBytesPerSec);
	
	// Update process data
	systemState_.processes = systemMonitor_->GetProcesses();
	systemState_.processes.shrink_to_fit();  // release excess capacity
	systemState_.totalProcesses = systemMonitor_->GetTotalProcesses();
	systemState_.totalThreads = systemMonitor_->GetTotalThreads();
	
	// Update network stats
	systemState_.networkStats = systemMonitor_->GetNetworkStats();
	systemState_.networkStats.interfaces.shrink_to_fit();
	
	// Update alert system
	UpdateAlerts();
	
	systemState_.lastUpdateTime = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
}

void DataCollector::UpdateAlerts()
{
	// Initialize default alert rules if empty
	if (systemState_.alertRules.empty()) {
		InitializeDefaultAlertRules();
	}
	
	auto currentTime = std::chrono::duration_cast<std::chrono::milliseconds>(
		std::chrono::system_clock::now().time_since_epoch()).count();
	
	// Check each alert rule
	for (auto& rule : systemState_.alertRules) {
		if (!rule.isEnabled) continue;
		
		float currentValue = 0.0f;
		bool shouldTrigger = false;
		
		// Get current value based on alert type
		switch (rule.type) {
			case AlertRule::CPU_USAGE:
				currentValue = systemState_.cpuUsagePercent;
				break;
			case AlertRule::MEMORY_USAGE:
				currentValue = systemState_.memoryUsagePercent;
				break;
			case AlertRule::DISK_USAGE:
				currentValue = systemState_.diskUsagePercent;
				break;
			case AlertRule::NETWORK_USAGE:
				currentValue = systemState_.networkUsagePercent;
				break;
			case AlertRule::PROCESS_COUNT:
				currentValue = static_cast<float>(systemState_.totalProcesses);
				break;
		}
		
		// Check if threshold is exceeded
		shouldTrigger = currentValue > rule.threshold;
		
		if (shouldTrigger) {
			// Check if this alert already exists
			bool alertExists = false;
			for (const auto& alert : systemState_.activeAlerts) {
				if (alert.type == rule.type) {
					alertExists = true;
					break;
				}
			}
			
			// Create new alert if it doesn't exist
			if (!alertExists) {
				SystemAlert alert;
				alert.type = rule.type;
				alert.currentValue = currentValue;
				alert.threshold = rule.threshold;
				alert.timestamp = currentTime;
				alert.severity = (currentValue > rule.threshold * 1.5f) ? 3 : 2; // Critical if 150% of threshold
				
				switch (rule.type) {
					case AlertRule::CPU_USAGE:
						alert.title = "High CPU Usage";
						alert.message = "CPU usage is " + std::to_string((int)currentValue) + "%, exceeding threshold of " + std::to_string((int)rule.threshold) + "%";
						break;
					case AlertRule::MEMORY_USAGE:
						alert.title = "High Memory Usage";
						alert.message = "Memory usage is " + std::to_string((int)currentValue) + "%, exceeding threshold of " + std::to_string((int)rule.threshold) + "%";
						break;
					case AlertRule::DISK_USAGE:
						alert.title = "High Disk Usage";
						alert.message = "Disk usage is " + std::to_string((int)currentValue) + "%, exceeding threshold of " + std::to_string((int)rule.threshold) + "%";
						break;
					case AlertRule::NETWORK_USAGE:
						alert.title = "High Network Usage";
						alert.message = "Network usage is " + std::to_string((int)currentValue) + "%, exceeding threshold of " + std::to_string((int)rule.threshold) + "%";
						break;
					case AlertRule::PROCESS_COUNT:
						alert.title = "High Process Count";
						alert.message = "Process count is " + std::to_string((int)currentValue) + ", exceeding threshold of " + std::to_string((int)rule.threshold);
						break;
				}
				
				systemState_.activeAlerts.push_back(alert);
				rule.triggeredCount++;
				rule.lastTriggeredTime = currentTime;
			}
		} else {
			// Remove alert if condition no longer met
			auto it = std::remove_if(systemState_.activeAlerts.begin(), systemState_.activeAlerts.end(),
				[&rule](const SystemAlert& alert) {
					return alert.type == rule.type;
				});
			systemState_.activeAlerts.erase(it, systemState_.activeAlerts.end());
		}
	}
	
	// Shrink alert vectors to free excess memory
	systemState_.activeAlerts.shrink_to_fit();
	systemState_.alertRules.shrink_to_fit();
	systemState_.totalAlerts = static_cast<uint32_t>(systemState_.activeAlerts.size());
}

void DataCollector::InitializeDefaultAlertRules()
{
	// CPU usage alert
	AlertRule cpuRule;
	cpuRule.type = AlertRule::CPU_USAGE;
	cpuRule.threshold = 80.0f;
	cpuRule.durationSeconds = 30;
	cpuRule.isEnabled = true;
	cpuRule.message = "CPU usage is consistently high";
	systemState_.alertRules.push_back(cpuRule);
	
	// Memory usage alert
	AlertRule memoryRule;
	memoryRule.type = AlertRule::MEMORY_USAGE;
	memoryRule.threshold = 85.0f;
	memoryRule.durationSeconds = 60;
	memoryRule.isEnabled = true;
	memoryRule.message = "Memory usage is critically high";
	systemState_.alertRules.push_back(memoryRule);
	
	// Disk usage alert
	AlertRule diskRule;
	diskRule.type = AlertRule::DISK_USAGE;
	diskRule.threshold = 90.0f;
	diskRule.durationSeconds = 60;
	diskRule.isEnabled = true;
	diskRule.message = "Disk activity is very high";
	systemState_.alertRules.push_back(diskRule);
	
	// Network usage alert
	AlertRule networkRule;
	networkRule.type = AlertRule::NETWORK_USAGE;
	networkRule.threshold = 75.0f;
	networkRule.durationSeconds = 45;
	networkRule.isEnabled = true;
	networkRule.message = "Network usage is unusually high";
	systemState_.alertRules.push_back(networkRule);
}