#include "VRDriver.hpp"

vr::EVRInitError WheelchairDriver::VRDriver::Init(vr::IVRDriverContext *pDriverContext)
{
	// Perform driver context initialisation
	if (vr::EVRInitError init_error = vr::InitServerDriverContext(pDriverContext); init_error != vr::EVRInitError::VRInitError_None) {
		return init_error;
	}

	Log("Activating Wheelchair Driver");

	// Add a couple controllers
	this->AddWheelchairController(std::make_shared<VRController>("Wheelchair_WheelchairController"));

	Log("Wheelchair Driver Loaded Successfully");

	return vr::VRInitError_None;
}

void WheelchairDriver::VRDriver::Cleanup()
{
}

void WheelchairDriver::VRDriver::RunFrame()
{
	// Collect events
	vr::VREvent_t event;
	std::vector<vr::VREvent_t> events;
	while (vr::VRServerDriverHost()->PollNextEvent(&event, sizeof(event))) {
		events.push_back(event);
	}
	this->openvr_events_ = events;

	// Update frame timing
	std::chrono::system_clock::time_point now = std::chrono::system_clock::now();
	this->frame_timing_ = std::chrono::duration_cast<std::chrono::milliseconds>(now - this->last_frame_time_);
	this->last_frame_time_ = now;

	// Update devices
	if (wheelchairController_ != nullptr) {
		wheelchairController_->Update();
	}
}

bool WheelchairDriver::VRDriver::ShouldBlockStandbyMode()
{
	return false;
}

void WheelchairDriver::VRDriver::EnterStandby()
{
}

void WheelchairDriver::VRDriver::LeaveStandby()
{
}

bool WheelchairDriver::VRDriver::AddWheelchairController(std::shared_ptr<VRController> wheelchairController)
{
	vr::ETrackedDeviceClass openvr_device_class = vr::ETrackedDeviceClass::TrackedDeviceClass_Controller;

	bool result = vr::VRServerDriverHost()->TrackedDeviceAdded(wheelchairController->GetSerial().c_str(), openvr_device_class,
	                                                           wheelchairController.get());

	if (result) {
		this->wheelchairController_ = wheelchairController;
	}

	return result;
}

std::shared_ptr<WheelchairDriver::VRController> WheelchairDriver::VRDriver::GetWheelchairController()
{
	return this->wheelchairController_;
}

std::vector<vr::VREvent_t> WheelchairDriver::VRDriver::GetOpenVREvents()
{
	return this->openvr_events_;
}

std::chrono::milliseconds WheelchairDriver::VRDriver::GetLastFrameTime()
{
	return this->frame_timing_;
}

WheelchairDriver::VRDriver::SettingsValue WheelchairDriver::VRDriver::GetSettingsValue(std::string key)
{
	vr::EVRSettingsError err = vr::EVRSettingsError::VRSettingsError_None;
	int int_value = vr::VRSettings()->GetInt32(settings_key_.c_str(), key.c_str(), &err);
	if (err == vr::EVRSettingsError::VRSettingsError_None) {
		return int_value;
	}
	err = vr::EVRSettingsError::VRSettingsError_None;
	float float_value = vr::VRSettings()->GetFloat(settings_key_.c_str(), key.c_str(), &err);
	if (err == vr::EVRSettingsError::VRSettingsError_None) {
		return float_value;
	}
	err = vr::EVRSettingsError::VRSettingsError_None;
	bool bool_value = vr::VRSettings()->GetBool(settings_key_.c_str(), key.c_str(), &err);
	if (err == vr::EVRSettingsError::VRSettingsError_None) {
		return bool_value;
	}
	std::string str_value;
	str_value.reserve(1024);
	vr::VRSettings()->GetString(settings_key_.c_str(), key.c_str(), str_value.data(), 1024, &err);
	if (err == vr::EVRSettingsError::VRSettingsError_None) {
		return str_value;
	}
	err = vr::EVRSettingsError::VRSettingsError_None;

	return SettingsValue();
}

void WheelchairDriver::VRDriver::Log(std::string message)
{
	std::string message_endl = message + "\n";
	vr::VRDriverLog()->Log(message_endl.c_str());
}

vr::IVRDriverInput *WheelchairDriver::VRDriver::GetInput()
{
	return vr::VRDriverInput();
}

vr::CVRPropertyHelpers *WheelchairDriver::VRDriver::GetProperties()
{
	return vr::VRProperties();
}

vr::IVRServerDriverHost *WheelchairDriver::VRDriver::GetDriverHost()
{
	return vr::VRServerDriverHost();
}
