#include "WheelchairController.h"
#include "WheelchairDriver.h"

#include <Windows.h>
#include <sstream>

#include <serial/serial.h>

using namespace WheelchairDriverFactory;

WheelchairController::WheelchairController(std::string serial) :
		m_serial(serial)
{
}

std::string WheelchairController::GetSerial()
{
	return this->m_serial;
}

void WheelchairController::Update()
{
	GetDriver()->Log("Controller update");

	if (this->m_deviceIndex == vr::k_unTrackedDeviceIndexInvalid)
		return;

	// Check if this device was asked to be identified
	auto events = GetDriver()->GetOpenVREvents();
	for (auto event : events) {
		// Note here, event.trackedDeviceIndex does not necissarily equal this->m_deviceIndex, not sure why, but the component handle will match so we can just use that instead
		//if (event.trackedDeviceIndex == this->m_deviceIndex) {
		//if (event.eventType == vr::EVREventType::VREvent_Input_HapticVibration) {
		//	if (event.data.hapticVibration.componentHandle == this->haptic_component_) {
		//		this->did_vibrate_ = true;
		//	}
		//}
		//}
	}



	// Setup pose for this frame
	auto pose = WheelchairController::MakeDefaultPose();

	SYSTEMTIME localTime;
	GetLocalTime(&localTime);
	bool pressed = (localTime.wSecond % 2) == 0;

	if (localTime.wSecond % 2 == 0) {
		GetDriver()->GetInput()->UpdateBooleanComponent(this->m_aButtonClickComponent, true, 0);
	} else {
		GetDriver()->GetInput()->UpdateBooleanComponent(this->m_aButtonClickComponent, false, 0);
	}

	// Post pose
	GetDriver()->GetDriverHost()->TrackedDevicePoseUpdated(this->m_deviceIndex, pose, sizeof(vr::DriverPose_t));
	this->m_lastPose = pose;
}

vr::TrackedDeviceIndex_t WheelchairController::GetDeviceIndex()
{
	return this->m_deviceIndex;
}

vr::EVRInitError WheelchairController::Activate(uint32_t unObjectId)
{
	this->m_deviceIndex = unObjectId;

	GetDriver()->Log("Activating controller " + this->m_serial);

	// Get the properties handle
	auto props = GetDriver()->GetProperties()->TrackedDeviceToPropertyContainer(this->m_deviceIndex);

	GetDriver()->GetInput()->CreateBooleanComponent(props, "/input/a/click", &this->m_aButtonClickComponent);

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

	std::vector<serial::PortInfo> devices_found = serial::list_ports();
	std::vector<serial::PortInfo>::iterator iter = devices_found.begin();

	GetDriver()->Log("Serial Ports:");
	while(iter != devices_found.end()) {
		serial::PortInfo device = *iter++;

		std::ostringstream ss;
		ss << device.port.c_str() << device.description.c_str() << device.hardware_id.c_str();
		GetDriver()->Log(ss.str());
	}

	return vr::EVRInitError::VRInitError_None;
}

void WheelchairController::Deactivate()
{
	this->m_deviceIndex = vr::k_unTrackedDeviceIndexInvalid;
}

void WheelchairController::EnterStandby()
{
}

void *WheelchairController::GetComponent(const char *pchComponentNameAndVersion)
{
	return nullptr;
}

void WheelchairController::DebugRequest(
		const char *pchRequest, char *pchResponseBuffer, uint32_t unResponseBufferSize)
{
	if (unResponseBufferSize >= 1) {
		pchResponseBuffer[0] = 0;
	}
}

vr::DriverPose_t WheelchairController::GetPose()
{
	return m_lastPose;
}
