#pragma once
#include <vector>
#include "../system_state.hpp"

class IProcessMonitor
{
//public:s
	virtual ~IProcessMonitor() = default;

	virtual void UpdateProcesses() = 0;
	virtual	std::vector<ProcessInfo> GetProcesses() const = 0;
	virtual size_t GetProcessCount() const = 0;
};