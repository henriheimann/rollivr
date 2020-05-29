#pragma once

#include <chrono>
#include <memory>
#include <variant>
#include <vector>

#include <openvr_driver.h>

#include "RolliVRController.h"

class RolliVRDriver : protected vr::IServerTrackedDeviceProvider
{
public:
	bool AddRolliVRController(std::shared_ptr<RolliVRController> rolliVRController);

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

	~RolliVRDriver() = default;

	inline const char *const *GetInterfaceVersions() override
	{
		return vr::k_InterfaceVersions;
	};

	std::string GetResourcePath(const std::string &name) const;

private:
	std::shared_ptr<RolliVRController> m_rolliVRController;
	std::vector<vr::VREvent_t> m_openvrEvents;
	std::chrono::milliseconds m_frameTiming = std::chrono::milliseconds(16);
	std::chrono::system_clock::time_point m_lastFrameTime = std::chrono::system_clock::now();

};