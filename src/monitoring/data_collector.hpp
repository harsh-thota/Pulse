#pragma once
#include "../core/system_state.hpp"
#include "../core/interfaces/i_system_monitor.hpp"
#include <memory>
#include <chrono>

class DataCollector
{
public:
	DataCollector();
	~DataCollector();

	bool Initialize();
	void Update();

	const SystemState& GetSystemState() const { return systemState_; }

private:
	SystemState systemState_;
	std::unique_ptr<ISystemMonitor> systemMonitor_;

	std::chrono::steady_clock::time_point lastUpdate_;

	void UpdateSystemMetrics();
	void UpdateAlerts();
	void InitializeDefaultAlertRules();
};