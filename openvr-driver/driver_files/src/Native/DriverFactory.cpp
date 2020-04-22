#include "DriverFactory.hpp"

#include <Driver/VRDriver.hpp>

#include <thread>

static std::shared_ptr<WheelchairDriver::VRDriver> driver;

void *HmdDriverFactory(const char *interface_name, int *return_code)
{
	if (std::strcmp(interface_name, vr::IServerTrackedDeviceProvider_Version) == 0) {
		if (!driver) {
			// Instantiate concrete impl
			driver = std::make_shared<WheelchairDriver::VRDriver>();
		}
		// We always have at least 1 ref to the shared ptr in "driver" so passing out raw pointer is ok
		return driver.get();
	}

	if (return_code) {
		*return_code = vr::VRInitError_Init_InterfaceNotFound;
	}

	return nullptr;
}

std::shared_ptr<WheelchairDriver::VRDriver> WheelchairDriver::GetDriver()
{
	return driver;
}
