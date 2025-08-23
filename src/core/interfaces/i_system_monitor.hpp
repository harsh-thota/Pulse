// ISystemMonitor: Defines the interface for collecting system metrics.
// Implementations should talk to OS APIs and fill in ProcessInfo, NetworkStats, etc.
#pragma once
#include <string>
#include <vector>
#include "../system_state.hpp"

class ISystemMonitor
{
public:
	virtual ~ISystemMonitor() = default;

	virtual void UpdateSystemMetrics() = 0;
	
	// CPU metrics
	virtual float GetCPUUsage() const = 0;
	virtual std::string GetCPUName() const = 0;
	virtual uint32_t GetCoreCount() const = 0;
	
	// Memory metrics
	virtual uint64_t GetTotalMemory() const = 0;
	virtual uint64_t GetUsedMemory() const = 0;
	virtual float GetMemoryUsagePercent() const = 0;
	
	// GPU metrics
	virtual float GetGPUUsage() const = 0;
	virtual std::string GetGPUName() const = 0;
	virtual uint64_t GetGPUMemoryUsed() const = 0;
	virtual uint64_t GetGPUMemoryTotal() const = 0;
	
	// Disk I/O metrics
	virtual float GetDiskUsage() const = 0;
	virtual uint64_t GetDiskReadBytesPerSec() const = 0;
	virtual uint64_t GetDiskWriteBytesPerSec() const = 0;
	virtual std::string GetPrimaryDiskName() const = 0;
	
	// Network metrics
	virtual float GetNetworkUsage() const = 0;
	virtual uint64_t GetNetworkUploadBytesPerSec() const = 0;
	virtual uint64_t GetNetworkDownloadBytesPerSec() const = 0;
	virtual std::string GetPrimaryNetworkInterface() const = 0;
	virtual NetworkStats GetNetworkStats() const = 0;
	
	// Process metrics
	virtual std::vector<ProcessInfo> GetProcesses() const = 0;
	virtual uint32_t GetTotalProcesses() const = 0;
	virtual uint32_t GetTotalThreads() const = 0;
	
	// System info
	virtual std::string GetSystemName() const = 0;
};