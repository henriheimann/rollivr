#pragma once

#include <chrono>
#include <memory>
#include <variant>
#include <vector>

#include <openvr_driver.h>

#include "WheelchairController.h"

class WheelchairDriver : protected vr::IServerTrackedDeviceProvider
{
public:

	typedef std::variant<std::monostate, std::string, int, float, bool> SettingsValue;

	bool AddWheelchairController(std::shared_ptr<WheelchairController> wheelchairController);

	std::shared_ptr<WheelchairController> GetWheelchairController();

	std::vector<vr::VREvent_t> GetOpenVREvents();

	std::chrono::milliseconds GetLastFrameTime();

	SettingsValue GetSettingsValue(std::string key);

	void Log(std::string message);

	vr::IVRDriverInput *GetInput();

	vr::CVRPropertyHelpers *GetProperties();

	vr::IVRServerDriverHost *GetDriverHost();

	vr::EVRInitError Init(vr::IVRDriverContext *pDriverContext) override;

	void Cleanup() override;

	void RunFrame() override;

	bool ShouldBlockStandbyMode() override;

	void EnterStandby() override;

	void LeaveStandby() override;

	~WheelchairDriver() = default;

	inline const char *const *GetInterfaceVersions() override
	{
		return vr::k_InterfaceVersions;
	};

private:
	std::shared_ptr<WheelchairController> wheelchairController_;
	std::vector<vr::VREvent_t> openvr_events_;
	std::chrono::milliseconds frame_timing_ = std::chrono::milliseconds(16);
	std::chrono::system_clock::time_point last_frame_time_ = std::chrono::system_clock::now();
	std::string settings_key_ = "driver_wheelchair";

};