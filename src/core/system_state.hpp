#pragma once
#include "ring_buffer.hpp"
#include <string>
#include <vector>
#include <cstdint>

struct ProcessInfo;
struct NetworkStats;

struct SystemState
{
	std::string systemName = "Unknown";
	std::string cpuName = "Unknown CPU";
	uint32_t coreCount = 0;

	uint64_t lastUpdateTime = 0;

	float cpuUsagePercent = 0.0f;
	RingBuffer<float, 300> cpuHistory;

	uint64_t totalRAMBytes = 0;
	uint64_t usedRAMBytes = 0;
	float memoryUsagePercent = 0.0f;
	RingBuffer<float, 300> memoryHistory;

	float gpuUsagePercent = 0.0f;
	RingBuffer<float, 300> gpuHistory;

	uint64_t uploadBytesPerSec = 0;
	uint64_t downloadBytesPerSec = 0;
	RingBuffer<uint64_t, 300> uploadHistory;
	RingBuffer<uint64_t, 300> downloadHistory;

	std::vector<ProcessInfo> processes;

	//NetworkStats netStats;

	uint64_t pulseMemoryUse = 0;
};

struct ProcessInfo
{
	uint32_t pid = 0;
	std::string name;
	uint64_t memoryUsage = 0;
	float cpuUsagePercent = 0.0f;

	ProcessInfo() = default;
	ProcessInfo(uint32_t p, const std::string& n, uint64_t mem, float cpu) : pid(p), name(n), memoryUsage(mem), cpuUsagePercent(cpu) {}
};

struct NetworkStats
{
	uint64_t totalBytesRecieved = 0;
	uint64_t totalBytesSent = 0;
	uint32_t activeConnections = 0;
	std::string primaryInterface = "Unknown";
};