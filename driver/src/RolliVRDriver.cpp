#include "RolliVRDriver.h"

#include <Windows.h>
#include <fstream>

using namespace RolliVRDriverFactory;

vr::EVRInitError RolliVRDriver::Init(vr::IVRDriverContext *driverContext)
{
	// Perform driver context initialisation
	if (vr::EVRInitError initError = vr::InitServerDriverContext(driverContext); initError != vr::EVRInitError::VRInitError_None) {
		return initError;
	}

	SerialPortInterface::AcceptedHardwareIds acceptedHardwareIds = loadAcceptedHardwareIds();

	// Create the controller
	std::shared_ptr<RolliVRController> rolliVRController = std::make_shared<RolliVRController>("RolliVR_RolliVRController",
			acceptedHardwareIds);

	// Try to add the controller as tracked device to OpenVR
	bool result = vr::VRServerDriverHost()->TrackedDeviceAdded(rolliVRController->GetSerial().c_str(),
			vr::ETrackedDeviceClass::TrackedDeviceClass_Controller, rolliVRController.get());

	if (!result) {
		return vr::VRInitError_Driver_Failed;
	}

	this->m_rolliVRController = rolliVRController;
	Log("RolliVR Driver Loaded Successfully");
	return vr::VRInitError_None;
}

void RolliVRDriver::Cleanup()
{
	if (m_rolliVRController) {
		m_rolliVRController->Cleanup();
	}
}

void RolliVRDriver::RunFrame()
{
	// Collect events
	vr::VREvent_t event{};
	std::vector<vr::VREvent_t> events;
	while (vr::VRServerDriverHost()->PollNextEvent(&event, sizeof(event))) {
		events.push_back(event);
	}
	this->m_openvrEvents = events;

	// Update frame timing
	std::chrono::system_clock::time_point now = std::chrono::system_clock::now();
	this->m_frameTiming = std::chrono::duration_cast<std::chrono::milliseconds>(now - this->m_lastFrameTime);
	this->m_lastFrameTime = now;

	// Update devices
	if (m_rolliVRController != nullptr) {
		m_rolliVRController->Update(m_frameTiming);
	}
}

bool RolliVRDriver::ShouldBlockStandbyMode()
{
	return false;
}

void RolliVRDriver::EnterStandby()
{
}

void RolliVRDriver::LeaveStandby()
{
}

std::vector<vr::VREvent_t> RolliVRDriver::GetOpenVREvents()
{
	return m_openvrEvents;
}

vr::IVRDriverInput *RolliVRDriver::GetInput()
{
	return vr::VRDriverInput();
}

vr::CVRPropertyHelpers *RolliVRDriver::GetProperties()
{
	return vr::VRProperties();
}

vr::IVRServerDriverHost *RolliVRDriver::GetDriverHost()
{
	return vr::VRServerDriverHost();
}

#pragma clang diagnostic push
#pragma ide diagnostic ignored "bugprone-reserved-identifier"

EXTERN_C IMAGE_DOS_HEADER __ImageBase;

std::string RolliVRDriver::GetResourcePath(const std::string &name)
{
	char dll_path[MAX_PATH] = {0};
	GetModuleFileNameA((HINSTANCE)&__ImageBase, dll_path, MAX_PATH);

	std::string file_path(dll_path);

	file_path = file_path.substr(0, file_path.find_last_of("\\/"));
	file_path = file_path.substr(0, file_path.find_last_of("\\/"));
	file_path = file_path.substr(0, file_path.find_last_of("\\/"));
	file_path += name;

	return file_path;
}

#pragma clang diagnostic pop

SerialPortInterface::AcceptedHardwareIds RolliVRDriver::loadAcceptedHardwareIds()
{
	SerialPortInterface::AcceptedHardwareIds acceptedHardwareIds;
	std::string comportsConfigPath = GetResourcePath(R"(\resources\config\comports.txt)");

	std::ifstream in(comportsConfigPath);
	if (in) {
		std::string line;
		while (std::getline(in, line)) {
			if (!line.empty()) {
				size_t separatorIndex = line.find(';');

				if (separatorIndex == std::string::npos) {
					Log("Unable to parse comport config line, separator not found: %s", line.c_str());
					continue;
				}

				try {
					std::string hardwareId = line.substr(0, separatorIndex);
					uint32_t baudRate = std::stoul(line.substr(separatorIndex + 1));
					acceptedHardwareIds.insert_or_assign(hardwareId, baudRate);

				} catch (std::exception &e) {
					Log("Unable to parse comport config line, exception %s during parsing: %s", e.what(), line.c_str());
				}
			}
		}
	}

	return acceptedHardwareIds;
}