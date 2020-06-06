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

	SerialPortInterface::AcceptedHardwareIds acceptedHardwareIds;
	std::string comportsConfigPath = GetResourcePath("\\resources\\config\\comports.txt");

	std::ifstream in(comportsConfigPath);
	if (in) {
		std::string line;
		while (std::getline(in, line)) {
			if (!line.empty()) {
				size_t separatorIndex = line.find(';');

				if (separatorIndex != std::string::npos) {
					Log("Unable to parse comport config line.");
					continue;
				}

				try {
					std::string hardwareId = line.substr(0, separatorIndex);
					uint32_t baudRate = std::stoul(line.substr(separatorIndex + 1));
					acceptedHardwareIds.insert_or_assign(hardwareId, baudRate);

				} catch (std::exception &) {
					Log("Unable to parse comport config line.");
				}
			}
		}
	}

	AddRolliVRController(std::make_shared<RolliVRController>("RolliVR_RolliVRController", acceptedHardwareIds));

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

bool RolliVRDriver::AddRolliVRController(std::shared_ptr<RolliVRController> rolliVRController)
{
	vr::ETrackedDeviceClass openvr_device_class = vr::ETrackedDeviceClass::TrackedDeviceClass_Controller;

	bool result = vr::VRServerDriverHost()->TrackedDeviceAdded(rolliVRController->GetSerial().c_str(), openvr_device_class,
	                                                           rolliVRController.get());

	if (result) {
		this->m_rolliVRController = rolliVRController;
	}

	return result;
}

void RolliVRDriver::Log(std::string message)
{
	std::string messageEndl = message + "\n";
	vr::VRDriverLog()->Log(messageEndl.c_str());
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

EXTERN_C IMAGE_DOS_HEADER __ImageBase;

std::string RolliVRDriver::GetResourcePath(const std::string &name) const
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
