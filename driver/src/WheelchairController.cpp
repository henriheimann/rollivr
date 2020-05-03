#include "WheelchairController.h"
#include "WheelchairDriver.h"

#include <Windows.h>
#include <sstream>

#include <serial/serial.h>

using namespace WheelchairDriverFactory;

WheelchairController::WheelchairController(std::string serial) :
		m_serial(serial),
		m_arduinoConnected(false)
{
}

std::string WheelchairController::GetSerial()
{
	return this->m_serial;
}

void WheelchairController::Update()
{
	//GetDriver()->Log("Controller update");

	if (this->m_deviceIndex == vr::k_unTrackedDeviceIndexInvalid)
		return;

	// Check if this device was asked to be identified
	/*auto events = GetDriver()->GetOpenVREvents();
	for (auto event : events) {
		// Note here, event.trackedDeviceIndex does not necissarily equal this->m_deviceIndex, not sure why, but the component handle will match so we can just use that instead
		if (event.trackedDeviceIndex == this->m_deviceIndex) {

		}

		std::ostringstream ss;
		ss << "Event for " << event.trackedDeviceIndex << " type: " << event.eventType;
		GetDriver()->Log(ss.str());
	}*/

	SYSTEMTIME localTime;
	GetLocalTime(&localTime);
	if (localTime.wSecond % 2 == 0) {
		GetDriver()->GetInput()->UpdateScalarComponent(this->m_xComponent, 0, 0);
		GetDriver()->GetInput()->UpdateScalarComponent(this->m_yComponent, 0.5f, 0);
	} else {
		GetDriver()->GetInput()->UpdateScalarComponent(this->m_xComponent, 0, 0);
		GetDriver()->GetInput()->UpdateScalarComponent(this->m_yComponent, -0.5f, 0);
	}

	GetDriver()->GetDriverHost()->TrackedDevicePoseUpdated(this->m_deviceIndex, GetPose(), sizeof(vr::DriverPose_t));
}

vr::TrackedDeviceIndex_t WheelchairController::GetDeviceIndex()
{
	return this->m_deviceIndex;
}

vr::EVRInitError WheelchairController::Activate(uint32_t unObjectId)
{
	this->m_deviceIndex = unObjectId;

	std::ostringstream ss;
	ss << "Activating controller " << this->m_serial << " deviceIndex: " << this->m_deviceIndex ;
	GetDriver()->Log(ss.str());

	// Get the properties handle
	auto props = GetDriver()->GetProperties()->TrackedDeviceToPropertyContainer(this->m_deviceIndex);

	GetDriver()->GetInput()->CreateScalarComponent(props, "/input/joystick/x", &this->m_xComponent, vr::VRScalarType_Absolute, vr::VRScalarUnits_NormalizedTwoSided);
	GetDriver()->GetInput()->CreateScalarComponent(props, "/input/joystick/y", &this->m_yComponent, vr::VRScalarType_Absolute, vr::VRScalarUnits_NormalizedTwoSided);

	// Set some universe ID (Must be 2 or higher)
	GetDriver()->GetProperties()->SetUint64Property(props, vr::Prop_CurrentUniverseId_Uint64, 2);

	// Set up a model "number" (not needed but good to have)
	GetDriver()->GetProperties()->SetStringProperty(props, vr::Prop_ModelNumber_String, "wheelchair_controller");

	// Give SteamVR a hint at what hand this controller is for
	GetDriver()->GetProperties()->SetInt32Property(props, vr::Prop_ControllerRoleHint_Int32,
		                                               vr::ETrackedControllerRole::TrackedControllerRole_Treadmill);
	GetDriver()->GetProperties()->SetBoolProperty(props, vr::Prop_NeverTracked_Bool, true);

	// Set controller profile
	GetDriver()->GetProperties()->SetStringProperty(props, vr::Prop_InputProfilePath_String,
	                                                "{wheelchair}/input/wheelchair_controller_bindings.json");

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

		if (device.hardware_id.find("FTDIBUS\\COMPORT&VID_0403&PID_6001") != std::string::npos) {

			std::ostringstream ss;
			ss << "Found Arduino on " << device.port;
			GetDriver()->Log(ss.str());
			m_arduinoConnected = true;
		}
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
	vr::DriverPose_t pose = {0};
	pose.deviceIsConnected = m_arduinoConnected;
	pose.poseIsValid = true; // Must be true? Otherwise the device is shown as 'Standby'
	//pose.result = vr::ETrackingResult::TrackingResult_Running_OK;
	return pose;
}
