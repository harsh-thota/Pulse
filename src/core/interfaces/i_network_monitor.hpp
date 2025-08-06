#pragma once
#include "../system_state.hpp"

class INetworkMonitor
{
//public:
	virtual ~INetworkMonitor() = default;

	virtual void UpdateNetworkStats() = 0;
	virtual uint64_t GetUploadBytesPerSec() const = 0;
	virtual uint64_t GetDownloadBytesPerSec() const = 0;
	virtual NetworkStats GetNetworkStats() const = 0;
};