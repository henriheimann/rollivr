#pragma once

#include <chrono>
#include <cmath>

#include <Native/WheelchairDriverFactory.h>

class WheelchairController : public vr::ITrackedDeviceServerDriver
{
public:

	WheelchairController(std::string serial);

	~WheelchairController() = default;

	std::string GetSerial();

	void Update();

	vr::TrackedDeviceIndex_t GetDeviceIndex();

	vr::EVRInitError Activate(uint32_t unObjectId);

	void Deactivate();

	void EnterStandby();

	void *GetComponent(const char *pchComponentNameAndVersion);

	void DebugRequest(const char *pchRequest, char *pchResponseBuffer, uint32_t unResponseBufferSize);

	vr::DriverPose_t GetPose();

	/// <summary>
	/// Makes a default device pose
	/// </summary>
	/// <returns>Default initialised pose</returns>
	static inline vr::DriverPose_t MakeDefaultPose(bool connected = true, bool tracking = true)
	{
		vr::DriverPose_t out_pose = {0};

		out_pose.deviceIsConnected = connected;
		out_pose.poseIsValid = tracking;
		out_pose.result = tracking ? vr::ETrackingResult::TrackingResult_Running_OK
		                           : vr::ETrackingResult::TrackingResult_Running_OutOfRange;
		out_pose.willDriftInYaw = false;
		out_pose.shouldApplyHeadModel = false;
		out_pose.qDriverFromHeadRotation.w = out_pose.qWorldFromDriverRotation.w = out_pose.qRotation.w = 1.0;

		return out_pose;
	}

private:
	vr::TrackedDeviceIndex_t device_index_ = vr::k_unTrackedDeviceIndexInvalid;
	std::string serial_;

	vr::DriverPose_t last_pose_;

	vr::VRInputComponentHandle_t a_button_click_component_ = 0;
};