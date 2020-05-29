#include "RolliVRDriverFactory.h"

#include <RolliVRDriver.h>

#include <thread>

static std::shared_ptr<RolliVRDriver> driver;

void *HmdDriverFactory(const char *interface_name, int *return_code)
{
	if (std::strcmp(interface_name, vr::IServerTrackedDeviceProvider_Version) == 0) {
		if (!driver) {
			// Instantiate concrete impl
			driver = std::make_shared<RolliVRDriver>();
		}
		// We always have at least 1 ref to the shared ptr in "driver" so passing out raw pointer is ok
		return driver.get();
	}

	if (return_code) {
		*return_code = vr::VRInitError_Init_InterfaceNotFound;
	}

	return nullptr;
}

namespace RolliVRDriverFactory
{
	std::shared_ptr<RolliVRDriver> GetDriver()
	{
		return driver;
	}
}
