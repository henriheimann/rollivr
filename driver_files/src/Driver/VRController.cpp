#include "VRController.hpp"
#include "VRDriver.hpp"

#include <Windows.h>
#include <sstream>

WheelchairDriver::VRController::VRController(std::string serial) :
		serial_(serial)
{
}

std::string WheelchairDriver::VRController::GetSerial()
{
	return this->serial_;
}

void WheelchairDriver::VRController::Update()
{
	GetDriver()->Log("Controller update");

	if (this->device_index_ == vr::k_unTrackedDeviceIndexInvalid)
		return;

	// Check if this device was asked to be identified
	auto events = GetDriver()->GetOpenVREvents();
	for (auto event : events) {
		// Note here, event.trackedDeviceIndex does not necissarily equal this->device_index_, not sure why, but the component handle will match so we can just use that instead
		//if (event.trackedDeviceIndex == this->device_index_) {
		//if (event.eventType == vr::EVREventType::VREvent_Input_HapticVibration) {
		//	if (event.data.hapticVibration.componentHandle == this->haptic_component_) {
		//		this->did_vibrate_ = true;
		//	}
		//}
		//}
	}

	// Setup pose for this frame
	auto pose = VRController::MakeDefaultPose();

	SYSTEMTIME localTime;
	GetLocalTime(&localTime);
	bool pressed = (localTime.wSecond % 2) == 0;

	std::ostringstream ss;
	ss << "Button pressed: " << pressed;
	GetDriver()->Log(ss.str());

	if (localTime.wSecond % 2 == 0) {
		GetDriver()->GetInput()->UpdateBooleanComponent(this->a_button_click_component_, true, 0);
	} else {
		GetDriver()->GetInput()->UpdateBooleanComponent(this->a_button_click_component_, false, 0);
	}

	// Post pose
	GetDriver()->GetDriverHost()->TrackedDevicePoseUpdated(this->device_index_, pose, sizeof(vr::DriverPose_t));
	this->last_pose_ = pose;
}

vr::TrackedDeviceIndex_t WheelchairDriver::VRController::GetDeviceIndex()
{
	return this->device_index_;
}

vr::EVRInitError WheelchairDriver::VRController::Activate(uint32_t unObjectId)
{
	this->device_index_ = unObjectId;

	GetDriver()->Log("Activating controller " + this->serial_);

	// Get the properties handle
	auto props = GetDriver()->GetProperties()->TrackedDeviceToPropertyContainer(this->device_index_);

	GetDriver()->GetInput()->CreateBooleanComponent(props, "/input/a/click", &this->a_button_click_component_);

	// Set some universe ID (Must be 2 or higher)
	GetDriver()->GetProperties()->SetUint64Property(props, vr::Prop_CurrentUniverseId_Uint64, 2);

	// Set up a model "number" (not needed but good to have)
	GetDriver()->GetProperties()->SetStringProperty(props, vr::Prop_ModelNumber_String, "wheelchair_controller");

	// Set up a render model path
	GetDriver()->GetProperties()->SetStringProperty(props, vr::Prop_RenderModelName_String,
	                                                "{wheelchair}wheelchair_controller");

	// Give SteamVR a hint at what hand this controller is for
	GetDriver()->GetProperties()->SetInt32Property(props, vr::Prop_ControllerRoleHint_Int32,
		                                               vr::ETrackedControllerRole::TrackedControllerRole_Treadmill);

	// Set controller profile
	GetDriver()->GetProperties()->SetStringProperty(props, vr::Prop_InputProfilePath_String,
	                                                "{wheelchair}/input/wheelchair_controller_bindings.json");

	// Change the icon depending on which handedness this controller is using (ANY uses right)
	std::string controller_ready_file = "{wheelchair}/icons/controller_ready.png";
	std::string controller_not_ready_file = "{wheelchair}/icons/controller_not_ready_.png";

	GetDriver()->GetProperties()->SetStringProperty(props, vr::Prop_NamedIconPathDeviceReady_String,
	                                                controller_ready_file.c_str());

	GetDriver()->GetProperties()->SetStringProperty(props, vr::Prop_NamedIconPathDeviceOff_String,
	                                                controller_not_ready_file.c_str());
	GetDriver()->GetProperties()->SetStringProperty(props, vr::Prop_NamedIconPathDeviceSearching_String,
	                                                controller_not_ready_file.c_str());
	GetDriver()->GetProperties()->SetStringProperty(props, vr::Prop_NamedIconPathDeviceSearchingAlert_String,
	                                                controller_not_ready_file.c_str());
	GetDriver()->GetProperties()->SetStringProperty(props, vr::Prop_NamedIconPathDeviceReadyAlert_String,
	                                                controller_not_ready_file.c_str());
	GetDriver()->GetProperties()->SetStringProperty(props, vr::Prop_NamedIconPathDeviceNotReady_String,
	                                                controller_not_ready_file.c_str());
	GetDriver()->GetProperties()->SetStringProperty(props, vr::Prop_NamedIconPathDeviceStandby_String,
	                                                controller_not_ready_file.c_str());
	GetDriver()->GetProperties()->SetStringProperty(props, vr::Prop_NamedIconPathDeviceAlertLow_String,
	                                                controller_not_ready_file.c_str());

	return vr::EVRInitError::VRInitError_None;
}

void WheelchairDriver::VRController::Deactivate()
{
	this->device_index_ = vr::k_unTrackedDeviceIndexInvalid;
}

void WheelchairDriver::VRController::EnterStandby()
{
}

void *WheelchairDriver::VRController::GetComponent(const char *pchComponentNameAndVersion)
{
	return nullptr;
}

void WheelchairDriver::VRController::DebugRequest(
		const char *pchRequest, char *pchResponseBuffer, uint32_t unResponseBufferSize)
{
	if (unResponseBufferSize >= 1) {
		pchResponseBuffer[0] = 0;
	}
}

vr::DriverPose_t WheelchairDriver::VRController::GetPose()
{
	return last_pose_;
}
