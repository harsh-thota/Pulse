#include "windows_system_monitor.hpp"
#include <iostream>
#include <sstream>
#include <iomanip>

WindowsSystemMonitor::WindowsSystemMonitor()
{
    cpuUsage_ = 0.0f;
    totalMemory_ = 0;
    usedMemory_ = 0;
    coreCount_ = 0;
    systemName_ = "Windows";
    cpuName_ = "Unknown CPU";

    lastCPU_.QuadPart = 0;
    lastSysCPU_.QuadPart = 0;
    lastUserCPU_.QuadPart = 0;

	InitializeSystemInfo();
}

void WindowsSystemMonitor::UpdateSystemMetrics()
{
	UpdateCPUUsage();
	UpdateMemoryInfo();
}

float WindowsSystemMonitor::GetCPUUsage() const
{
	return cpuUsage_;
}

uint64_t WindowsSystemMonitor::GetTotalMemory() const
{
	return totalMemory_;
}

uint64_t WindowsSystemMonitor::GetUsedMemory() const
{
	return usedMemory_;
}

float WindowsSystemMonitor::GetMemoryUsagePercent() const
{
	if (totalMemory_ == 0) return 0.0f;
	return (static_cast<float>(usedMemory_) / static_cast<float>(totalMemory_)) * 100.0f;
}

std::string WindowsSystemMonitor::GetSystemName() const
{
	return systemName_;
}

std::string WindowsSystemMonitor::GetCPUName() const
{
	return cpuName_;
}

uint32_t WindowsSystemMonitor::GetCoreCount() const
{
	return coreCount_;
}

void WindowsSystemMonitor::InitializeSystemInfo()
{
	SYSTEM_INFO sysInfo;
	GetSystemInfo(&sysInfo);
	coreCount_ = sysInfo.dwNumberOfProcessors;

	HKEY hKey;
	LONG result = RegOpenKeyExA(HKEY_LOCAL_MACHINE, "HARDWARE\\DESCRIPTION\\System\\CentralProcessor\\0", 0, KEY_READ, &hKey);

	if (result == ERROR_SUCCESS)
	{
		char processorName[256];
		DWORD bufferSize = sizeof(processorName);
		result = RegQueryValueExA(hKey, "ProcessorNameString", NULL, NULL, reinterpret_cast<BYTE*>(processorName), &bufferSize);

		if (result == ERROR_SUCCESS)
		{
			cpuName_ = std::string(processorName);
			size_t start = cpuName_.find_first_not_of(" \t");
			size_t end = cpuName_.find_last_not_of(" \t");
			if (start != std::string::npos && end != std::string::npos)
			{
				cpuName_ = cpuName_.substr(start, end - start + 1);
			}
		}
		RegCloseKey(hKey);
	}

	UpdateMemoryInfo();

	FILETIME idleTime, kernelTime, userTime;
	GetSystemTimes(&idleTime, &kernelTime, &userTime);

	lastCPU_.LowPart = idleTime.dwLowDateTime;
	lastCPU_.HighPart = idleTime.dwHighDateTime;

	lastSysCPU_.LowPart = kernelTime.dwLowDateTime;
	lastSysCPU_.HighPart = kernelTime.dwHighDateTime;

	lastUserCPU_.LowPart = userTime.dwLowDateTime;
	lastUserCPU_.HighPart = userTime.dwHighDateTime;

	//std::cout << "Sys Initialized: " << coreCount_ << "cores, " << cpuName_ << "\n";
}

void WindowsSystemMonitor::UpdateCPUUsage()
{
	FILETIME idleTime, kernelTime, userTime;
	if (!GetSystemTimes(&idleTime, &kernelTime, &userTime))
	{
		cpuUsage_ = 0.0f;
		return;
	}

	ULARGE_INTEGER nowCPU, nowSysCPU, nowUserCPU;

	nowCPU.LowPart = idleTime.dwLowDateTime;
	nowCPU.HighPart = idleTime.dwHighDateTime;

	nowSysCPU.LowPart = kernelTime.dwLowDateTime;
	nowSysCPU.HighPart = kernelTime.dwHighDateTime;

	nowUserCPU.LowPart = userTime.dwLowDateTime;
	nowUserCPU.HighPart = userTime.dwHighDateTime;

	uint64_t cpuDelta = nowCPU.QuadPart - lastCPU_.QuadPart;
	uint64_t sysDelta = nowSysCPU.QuadPart - lastSysCPU_.QuadPart;
	uint64_t userDelta = nowUserCPU.QuadPart - lastUserCPU_.QuadPart;

	uint64_t totalDelta = sysDelta + userDelta;

	if (totalDelta > 0)
	{
		uint64_t activeDelta = totalDelta - cpuDelta;
		cpuUsage_ = (static_cast<float>(activeDelta) / static_cast<float>(totalDelta)) * 100.0f;

		if (cpuUsage_ < 0.0f) cpuUsage_ = 0.0f;
		if (cpuUsage_ > 1000.0f) cpuUsage_ = 100.0f;
	}
	else
	{
		cpuUsage_ = 0.0f;
	}

	lastCPU_ = nowCPU;
	lastSysCPU_ = nowSysCPU;
	lastUserCPU_ = nowUserCPU;
}

void WindowsSystemMonitor::UpdateMemoryInfo()
{
	MEMORYSTATUSEX memInfo;
	memInfo.dwLength = sizeof(MEMORYSTATUSEX);

	if (GlobalMemoryStatusEx(&memInfo))
	{
		totalMemory_ = memInfo.ullTotalPhys;
		usedMemory_ = totalMemory_ - memInfo.ullAvailPhys;
	}
	else
	{
		totalMemory_ = 0;
		usedMemory_ = 0;
	}
}