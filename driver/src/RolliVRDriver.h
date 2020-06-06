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
	RolliVRDriver() = default;

	~RolliVRDriver() = default;

	vr::EVRInitError Init(vr::IVRDriverContext *driverContext) override;

	void Cleanup() override;

	void RunFrame() override;

	bool ShouldBlockStandbyMode() override;

	void EnterStandby() override;

	void LeaveStandby() override;

	inline const char *const *GetInterfaceVersions() override
	{
		return vr::k_InterfaceVersions;
	};

	std::vector<vr::VREvent_t> GetOpenVREvents();

	static vr::IVRDriverInput *GetInput();

	static vr::CVRPropertyHelpers *GetProperties();

	static vr::IVRServerDriverHost *GetDriverHost();

	template<typename...Args>
	void Log(const std::string& format, Args...args)
	{
		size_t size = snprintf(nullptr, 0, format.c_str(), args...) + 1;
		if (size <= 0) {
			throw std::runtime_error("Error during formatting.");
		}

		std::unique_ptr<char[]> buf(new char[size]);
		snprintf(buf.get(), size, format.c_str(), args...);

		std::string message = std::string(buf.get(), buf.get() + size - 1);
		message += "\n";

		vr::VRDriverLog()->Log(message.c_str());
	}

	/**
	 * Returns the absolute file path for a file with the given name relative to the root directory of the driver.
	 * @param name The name or path of the file.
	 * @return The absolute file path.
	 */
	static std::string GetResourcePath(const std::string &name);

private:

	/**
	 * Loads a map of accepted hardware Ids mapped to their baud rate from a configuration file.
	 * @return The map of accepted hardware ids and baud rates.
	 */
	SerialPortInterface::AcceptedHardwareIds loadAcceptedHardwareIds();

	std::shared_ptr<RolliVRController> m_rolliVRController;
	std::vector<vr::VREvent_t> m_openvrEvents;
	std::chrono::milliseconds m_frameTiming = std::chrono::milliseconds(16);
	std::chrono::system_clock::time_point m_lastFrameTime = std::chrono::system_clock::now();

};