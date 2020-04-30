#pragma once

#include <cstdlib>
#include <memory>

#include <openvr_driver.h>

extern "C" __declspec(dllexport) void *HmdDriverFactory(const char *interface_name, int *return_code);

class WheelchairDriver;

namespace WheelchairDriverFactory
{
	std::shared_ptr<WheelchairDriver> GetDriver();
}