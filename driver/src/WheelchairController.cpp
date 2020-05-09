#include "WheelchairController.h"
#include "WheelchairDriver.h"

#include <Windows.h>
#include <sstream>
#include <utility>

#include <serial/serial.h>

using namespace WheelchairDriverFactory;

WheelchairController::WheelchairController(std::string serial) :
		m_serial(std::move(serial)),
		m_serialPortInterface({"FTDIBUS\\COMPORT&VID_0403&PID_6001"})
{
}

std::string WheelchairController::GetSerial()
{
	return this->m_serial;
}

void WheelchairController::Update(std::chrono::milliseconds frameTiming)
{
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

	m_serialPortInterface.Update(frameTiming);

	if (m_serialPortInterface.IsLineAvailable()) {
		std::string line = m_serialPortInterface.GetMostRecentLine();

		size_t separatorIndex = line.find(',');
		if (separatorIndex != std::string::npos) {
			try {
				m_inputY = std::stof(line.substr(0, separatorIndex));
				m_inputX = std::stof(line.substr(separatorIndex + 1));
			} catch (std::exception &) {

			}
		}
	}

	GetDriver()->GetInput()->UpdateScalarComponent(this->m_xComponent, m_inputX, 0);
	GetDriver()->GetInput()->UpdateScalarComponent(this->m_yComponent, m_inputY, 0);

	GetDriver()->GetDriverHost()->TrackedDevicePoseUpdated(this->m_deviceIndex, GetPose(), sizeof(vr::DriverPose_t));
}

void WheelchairController::Cleanup()
{
	m_serialPortInterface.Cleanup();
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

	GetDriver()->GetProperties()->SetBoolProperty(props, vr::Prop_Identifiable_Bool, false);

	// Set controller profile
	GetDriver()->GetProperties()->SetStringProperty(props, vr::Prop_InputProfilePath_String,
	                                                "{wheelchair}/input/wheelchair_controller_bindings.json");

	std::string controller_ready_file = "{wheelchair}/icons/controller_ready.png";
	std::string controller_not_ready_file = "{wheelchair}/icons/controller_not_ready.png";

	GetDriver()->GetProperties()->SetStringProperty(props, vr::Prop_NamedIconPathDeviceOff_String,
	                                                controller_not_ready_file.c_str());
	GetDriver()->GetProperties()->SetStringProperty(props, vr::Prop_NamedIconPathDeviceSearching_String,
	                                                controller_not_ready_file.c_str());
	GetDriver()->GetProperties()->SetStringProperty(props, vr::Prop_NamedIconPathDeviceSearchingAlert_String,
	                                                controller_not_ready_file.c_str());
	GetDriver()->GetProperties()->SetStringProperty(props, vr::Prop_NamedIconPathDeviceReady_String,
	                                                controller_ready_file.c_str());
	GetDriver()->GetProperties()->SetStringProperty(props, vr::Prop_NamedIconPathDeviceReadyAlert_String,
	                                                controller_not_ready_file.c_str());
	GetDriver()->GetProperties()->SetStringProperty(props, vr::Prop_NamedIconPathDeviceNotReady_String,
	                                                controller_not_ready_file.c_str());
	GetDriver()->GetProperties()->SetStringProperty(props, vr::Prop_NamedIconPathDeviceStandby_String,
	                                                controller_not_ready_file.c_str());
	GetDriver()->GetProperties()->SetStringProperty(props, vr::Prop_NamedIconPathDeviceAlertLow_String,
	                                                controller_not_ready_file.c_str());
	GetDriver()->GetProperties()->SetStringProperty(props, vr::Prop_NamedIconPathDeviceStandbyAlert_String,
	                                                controller_not_ready_file.c_str());

	return vr::EVRInitError::VRInitError_None;
}

void WheelchairController::Deactivate()
{
	m_deviceIndex = vr::k_unTrackedDeviceIndexInvalid;

	m_serialPortInterface.Cleanup();
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
	pose.deviceIsConnected = m_serialPortInterface.IsConnected();
	pose.poseIsValid = true; // Must be true? Otherwise the device is shown as 'Standby'
	//pose.result = vr::ETrackingResult::TrackingResult_Running_OK;
	return pose;
}
