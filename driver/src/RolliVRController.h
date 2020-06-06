#pragma once

#include <chrono>
#include <cmath>

#include <RolliVRDriverFactory.h>
#include "SerialPortInterface.h"

class RolliVRController : public vr::ITrackedDeviceServerDriver
{
public:

	/**
	 * Constructs a new RolliVRController with the given serial forwarded to OpenVR and connections allowed with the given hardware IDs.
	 * @param serial The serial of this controller.
	 * @param acceptedHardwareIds The map of accepted hardware Ids and their baud rates.
	 */
	RolliVRController(std::string serial, SerialPortInterface::AcceptedHardwareIds acceptedHardwareIds);

	~RolliVRController() = default;

	std::string GetSerial();

	void Update(std::chrono::milliseconds frameTiming);

	void Cleanup();

	vr::EVRInitError Activate(uint32_t unObjectId) override;

	void Deactivate() override;

	void EnterStandby() override;

	void *GetComponent(const char *pchComponentNameAndVersion) override;

	void DebugRequest(const char *pchRequest, char *pchResponseBuffer, uint32_t unResponseBufferSize) override;

	vr::DriverPose_t GetPose() override;

private:

	/**
	 * Parses a single line obtained that was read from the serial port interface and stores the resulting
	 * input values in m_inputX and m_inputY.
	 * @param line The line to parse.
	 */
	void ParseSerialPortLine(const std::string &line);

	vr::TrackedDeviceIndex_t m_deviceIndex = vr::k_unTrackedDeviceIndexInvalid;
	std::string m_serial;

	SerialPortInterface m_serialPortInterface;

	float m_inputX = 0;
	float m_inputY = 0;

	vr::VRInputComponentHandle_t m_xComponent = 0;
	vr::VRInputComponentHandle_t m_yComponent = 0;
};