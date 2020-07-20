#pragma once

#include <string>
#include <chrono>

namespace RolliVROverlay
{
	constexpr const char *applicationKey = "rollivr.overlay";
	constexpr const char *qtSettingsApplication = "RolliVR";
	constexpr const char *qtSettingsOrganization = "HRW";

	constexpr const int targetFps = 100;
	constexpr std::chrono::milliseconds targetFrameTiming(1000 / targetFps);

	/**
	 * Installs the OpenVR manifest for this application.
	 * The VR context must be initialised when calling this function.
	 */
	void installManifest(bool cleanInstall = false);

	/**
	 * Removes the OpenVR manifest for this application.
	 * The VR context must be initialised when calling this function.
	 */
	void removeManifest();

	/**
	 * Checks if there are any command line arguments and executes the corresponding action.
	 * If there are any command line arguments, this function does not return and quits the application after execution.
	 */
	void checkForCommandLineArguments(int argc, char *argv[]);
}