#pragma once
#include "ring_buffer.hpp"
#include <string>
#include <vector>
#include <cstdint>

struct ProcessInfo
{
	uint32_t pid = 0;
	std::string name;
	uint64_t memoryUsage = 0;
	float cpuUsagePercent = 0.0f;
	std::string status = "Running";
	uint32_t threadCount = 0;
	std::string filePath;
	
	ProcessInfo() = default;
	ProcessInfo(uint32_t p, const std::string& n, uint64_t mem, float cpu) 
		: pid(p), name(n), memoryUsage(mem), cpuUsagePercent(cpu) {}
};

struct NetworkInterface
{
	std::string name;
	std::string type;
	bool isConnected = false;
	uint64_t bytesReceived = 0;
	uint64_t bytesSent = 0;
	uint64_t packetsReceived = 0;
	uint64_t packetsSent = 0;
	std::string ipAddress;
	std::string macAddress;
	uint32_t speed = 0; // In Mbps
};

struct NetworkStats
{
	uint64_t totalBytesReceived = 0;
	uint64_t totalBytesSent = 0;
	uint32_t activeConnections = 0;
	std::string primaryInterface = "Unknown";
	std::vector<NetworkInterface> interfaces;
};

struct AlertRule
{
	enum Type { CPU_USAGE, MEMORY_USAGE, DISK_USAGE, NETWORK_USAGE, PROCESS_COUNT };
	
	Type type;
	float threshold = 80.0f;
	uint32_t durationSeconds = 60; // How long the condition must persist
	bool isEnabled = true;
	std::string message;
	uint32_t triggeredCount = 0;
	uint64_t lastTriggeredTime = 0;
};

struct SystemAlert
{
	AlertRule::Type type;
	std::string title;
	std::string message;
	float currentValue = 0.0f;
	float threshold = 0.0f;
	uint64_t timestamp = 0;
	bool isActive = true;
	uint32_t severity = 1; // 1=Info, 2=Warning, 3=Critical
};

struct SystemState
{
	// Basic system info
	std::string systemName = "Unknown";
	std::string cpuName = "Unknown CPU";
	uint32_t coreCount = 0;
	uint64_t lastUpdateTime = 0;

	// CPU metrics
	float cpuUsagePercent = 0.0f;
	RingBuffer<float, 300> cpuHistory;

	// Memory metrics
	uint64_t totalRAMBytes = 0;
	uint64_t usedRAMBytes = 0;
	float memoryUsagePercent = 0.0f;
	RingBuffer<float, 300> memoryHistory;

	// GPU metrics
	float gpuUsagePercent = 0.0f;
	RingBuffer<float, 300> gpuHistory;
	std::string gpuName = "Unknown GPU";
	uint64_t gpuMemoryUsed = 0;
	uint64_t gpuMemoryTotal = 0;

	// Disk I/O metrics
	float diskUsagePercent = 0.0f;
	uint64_t diskReadBytesPerSec = 0;
	uint64_t diskWriteBytesPerSec = 0;
	RingBuffer<float, 300> diskHistory;
	std::string primaryDiskName = "System Drive";

	// Network metrics
	float networkUsagePercent = 0.0f;
	uint64_t uploadBytesPerSec = 0;
	uint64_t downloadBytesPerSec = 0;
	RingBuffer<float, 300> networkHistory;
	RingBuffer<uint64_t, 300> uploadHistory;
	RingBuffer<uint64_t, 300> downloadHistory;
	std::string primaryNetworkInterface = "Ethernet";

	// Process information
	std::vector<ProcessInfo> processes;
	uint32_t totalProcesses = 0;
	uint32_t totalThreads = 0;

	// Network details
	NetworkStats networkStats;

	// Alert system
	std::vector<AlertRule> alertRules;
	std::vector<SystemAlert> activeAlerts;
	uint32_t totalAlerts = 0;

	// Application metrics
	uint64_t pulseMemoryUse = 0;
	float pulseFrameRate = 60.0f;
};