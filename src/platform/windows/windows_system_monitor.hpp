#pragma once
#include "../../core/interfaces/i_system_monitor.hpp"
#include <Windows.h>
#include <pdh.h>
#include <dxgi1_4.h>
#include <d3d11.h>
#include <wbemidl.h>
#include <comdef.h>
#include <string>
#include <cstdint>
#include <vector>

#pragma comment(lib, "pdh.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "wbemuuid.lib")

struct GPUInfo {
	std::string name;
	uint64_t dedicatedVideoMemory = 0;
	uint64_t dedicatedSystemMemory = 0;
	uint64_t sharedSystemMemory = 0;
	bool isValid = false;
};

class WindowsSystemMonitor : public ISystemMonitor
{
public:
	WindowsSystemMonitor();
	~WindowsSystemMonitor() override;

	void UpdateSystemMetrics() override;

	// CPU metrics
	float GetCPUUsage() const override;
	std::string GetCPUName() const override;
	uint32_t GetCoreCount() const override;

	// Memory metrics
	uint64_t GetTotalMemory() const override;
	uint64_t GetUsedMemory() const override;
	float GetMemoryUsagePercent() const override;

	// GPU metrics
	float GetGPUUsage() const override;
	std::string GetGPUName() const override;
	uint64_t GetGPUMemoryUsed() const override;
	uint64_t GetGPUMemoryTotal() const override;
	
	// Disk I/O metrics
	float GetDiskUsage() const override;
	uint64_t GetDiskReadBytesPerSec() const override;
	uint64_t GetDiskWriteBytesPerSec() const override;
	std::string GetPrimaryDiskName() const override;
	
	// Network metrics
	float GetNetworkUsage() const override;
	uint64_t GetNetworkUploadBytesPerSec() const override;
	uint64_t GetNetworkDownloadBytesPerSec() const override;
	std::string GetPrimaryNetworkInterface() const override;
	NetworkStats GetNetworkStats() const override;
	
	// Process metrics
	std::vector<ProcessInfo> GetProcesses() const override;
	uint32_t GetTotalProcesses() const override;
	uint32_t GetTotalThreads() const override;

	// System info
	std::string GetSystemName() const override;

private:
	// CPU tracking
	float cpuUsage_ = 0.0f;
	std::string cpuName_ = "Unknown CPU";
	uint32_t coreCount_ = 0;
	ULARGE_INTEGER lastCPU_ = {};
	ULARGE_INTEGER lastSysCPU_ = {};
	ULARGE_INTEGER lastUserCPU_ = {};

	// Memory tracking
	uint64_t totalMemory_ = 0;
	uint64_t usedMemory_ = 0;

	// GPU tracking
	float gpuUsage_ = 0.0f;
	std::string gpuName_ = "Unknown GPU";
	uint64_t gpuMemoryUsed_ = 0;
	uint64_t gpuMemoryTotal_ = 0;
	std::vector<GPUInfo> gpuDevices_;
	IDXGIFactory* dxgiFactory_ = nullptr;
	bool gpuInitialized_ = false;

	// Disk tracking
	float diskUsage_ = 0.0f;
	uint64_t diskReadBytesPerSec_ = 0;
	uint64_t diskWriteBytesPerSec_ = 0;
	std::string primaryDiskName_ = "System Drive";

	// Network tracking
	float networkUsage_ = 0.0f;
	uint64_t networkUploadBytesPerSec_ = 0;
	uint64_t networkDownloadBytesPerSec_ = 0;
	std::string primaryNetworkInterface_ = "Ethernet";
	NetworkStats networkStats_;

	// Process tracking
	std::vector<ProcessInfo> processes_;
	uint32_t totalProcesses_ = 0;
	uint32_t totalThreads_ = 0;

	// Performance counters
	PDH_HQUERY pdhQuery_ = nullptr;
	PDH_HCOUNTER diskUsageCounter_ = nullptr;
	PDH_HCOUNTER diskReadCounter_ = nullptr;
	PDH_HCOUNTER diskWriteCounter_ = nullptr;
	PDH_HCOUNTER networkSentCounter_ = nullptr;
	PDH_HCOUNTER networkReceivedCounter_ = nullptr;

	// System info
	std::string systemName_ = "Windows";

	// Private methods
	void InitializeSystemInfo();
	void InitializePerformanceCounters();
	void InitializeGPU();
	void CleanupGPU();
	
	void UpdateCPUUsage();
	void UpdateMemoryInfo();
	void UpdateGPUUsage();
	void UpdateDiskUsage();
	void UpdateNetworkUsage();
	void UpdateProcessData();
	void UpdateNetworkData();
	
	// GPU helper methods
	std::vector<GPUInfo> EnumerateGPUDevices();
	bool InitializeDXGI();
	bool InitializeGPUPerformanceCounters();
};