#include "WheelchairDriver.h"

using namespace WheelchairDriverFactory;

vr::EVRInitError WheelchairDriver::Init(vr::IVRDriverContext *pDriverContext)
{
	// Perform driver context initialisation
	if (vr::EVRInitError init_error = vr::InitServerDriverContext(pDriverContext); init_error != vr::EVRInitError::VRInitError_None) {
		return init_error;
	}

	Log("Activating Wheelchair Driver");

	// Add a couple controllers
	this->AddWheelchairController(std::make_shared<WheelchairController>("Wheelchair_WheelchairController"));

	Log("Wheelchair Driver Loaded Successfully");

	return vr::VRInitError_None;
}

void WheelchairDriver::Cleanup()
{
	if (m_wheelchairController) {
		m_wheelchairController->Cleanup();
	}
}

void WheelchairDriver::RunFrame()
{
	// Collect events
	vr::VREvent_t event;
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
	if (m_wheelchairController != nullptr) {
		m_wheelchairController->Update(m_frameTiming);
	}
}

bool WheelchairDriver::ShouldBlockStandbyMode()
{
	return false;
}

void WheelchairDriver::EnterStandby()
{
}

void WheelchairDriver::LeaveStandby()
{
}

bool WheelchairDriver::AddWheelchairController(std::shared_ptr<WheelchairController> wheelchairController)
{
	vr::ETrackedDeviceClass openvr_device_class = vr::ETrackedDeviceClass::TrackedDeviceClass_Controller;

	bool result = vr::VRServerDriverHost()->TrackedDeviceAdded(wheelchairController->GetSerial().c_str(), openvr_device_class,
	                                                           wheelchairController.get());

	if (result) {
		this->m_wheelchairController = wheelchairController;
	}

	return result;
}

std::shared_ptr<WheelchairController> WheelchairDriver::GetWheelchairController()
{
	return this->m_wheelchairController;
}

std::vector<vr::VREvent_t> WheelchairDriver::GetOpenVREvents()
{
	return this->m_openvrEvents;
}

std::chrono::milliseconds WheelchairDriver::GetLastFrameTime()
{
	return this->m_frameTiming;
}

void WheelchairDriver::Log(std::string message)
{
	std::string message_endl = message + "\n";
	vr::VRDriverLog()->Log(message_endl.c_str());
}

vr::IVRDriverInput *WheelchairDriver::GetInput()
{
	return vr::VRDriverInput();
}

vr::CVRPropertyHelpers *WheelchairDriver::GetProperties()
{
	return vr::VRProperties();
}

vr::IVRServerDriverHost *WheelchairDriver::GetDriverHost()
{
	return vr::VRServerDriverHost();
}
