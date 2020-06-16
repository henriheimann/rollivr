#include "main.h"
#include "RolliVROverlayWidget.h"
#include "RolliVROverlayController.h"

#include <QtWidgets/QApplication>
#include <iostream>

void RolliVROverlay::installManifest(bool cleanInstall)
{
	// Adapted code from: https://github.com/matzman666/OpenVR-AdvancedSettings/

	auto manifestQPath = QDir::cleanPath(QDir(QCoreApplication::applicationDirPath()).absoluteFilePath("manifest.vrmanifest"));
	if (QFile::exists(manifestQPath)) {
		bool alreadyInstalled = false;
		if (vr::VRApplications()->IsApplicationInstalled(RolliVROverlay::applicationKey)) {
			if (cleanInstall) {
				char buffer[1024];
				auto appError = vr::VRApplicationError_None;
				vr::VRApplications()->GetApplicationPropertyString(RolliVROverlay::applicationKey, vr::VRApplicationProperty_WorkingDirectory_String, buffer, sizeof(buffer), &appError);
				if (appError == vr::VRApplicationError_None) {
					auto oldManifestQPath = QDir::cleanPath(QDir(buffer).absoluteFilePath("manifest.vrmanifest"));
					if (oldManifestQPath.compare(manifestQPath, Qt::CaseInsensitive) != 0) {
						vr::VRApplications()->RemoveApplicationManifest(QDir::toNativeSeparators(oldManifestQPath).toStdString().c_str());
					} else {
						alreadyInstalled = true;
					}
				}
			} else {
				alreadyInstalled = true;
			}
		}
		auto appError = vr::VRApplications()->AddApplicationManifest(QDir::toNativeSeparators(manifestQPath).toStdString().c_str());
		if (appError != vr::VRApplicationError_None) {
			throw std::runtime_error(std::string("Could not add application manifest: ") + std::string(vr::VRApplications()->GetApplicationsErrorNameFromEnum(appError)));
		} else if (!alreadyInstalled || cleanInstall) {
			appError = vr::VRApplications()->SetApplicationAutoLaunch(RolliVROverlay::applicationKey, true);
			if (appError != vr::VRApplicationError_None) {
				throw std::runtime_error(std::string("Could not set auto start: ") + std::string(vr::VRApplications()->GetApplicationsErrorNameFromEnum(appError)));
			}
		}
	} else {
		throw std::runtime_error(std::string("Could not find application manifest: ") + manifestQPath.toStdString());
	}
}

void RolliVROverlay::removeManifest()
{
	// Adapted code from: https://github.com/matzman666/OpenVR-AdvancedSettings/

	auto manifestQPath = QDir::cleanPath(QDir(QCoreApplication::applicationDirPath()).absoluteFilePath("manifest.vrmanifest"));
	if (QFile::exists(manifestQPath)) {
		if (vr::VRApplications()->IsApplicationInstalled(RolliVROverlay::applicationKey)) {
			vr::VRApplications()->RemoveApplicationManifest(QDir::toNativeSeparators(manifestQPath).toStdString().c_str());
		}
	} else {
		throw std::runtime_error(std::string("Could not find application manifest: ") + manifestQPath.toStdString());
	}
}

void RolliVROverlay::checkForCommandLineArguments(int argc, char *argv[])
{
	if (argc > 1) {
		QCoreApplication coreApp(argc, argv);

		auto exitCode = EXIT_SUCCESS;
		auto initError = vr::VRInitError_None;
		vr::VR_Init(&initError, vr::VRApplication_Utility);

		if (initError != vr::VRInitError_None) {
			std::cerr << "Failed to initialize OpenVR: " << std::string(vr::VR_GetVRInitErrorAsEnglishDescription(initError)) << std::endl;
			exit(EXIT_FAILURE);
		}

		if (std::string(argv[1]) == "-installmanifest") {
			try {
				RolliVROverlay::installManifest(true);
			} catch (std::exception& e) {
				exitCode = EXIT_FAILURE;
				std::cerr << "Failed to install Manifest: " << e.what() << std::endl;
			}
		} else if (std::string(argv[1]) == "-removemanifest") {
			try {
				RolliVROverlay::removeManifest();
			} catch (std::exception& e) {
				exitCode = EXIT_FAILURE;
				std::cerr << "Failed to remove Manifest: " << e.what() << std::endl;
			}
		} else {
			exitCode = EXIT_FAILURE;
			std::cerr << "Unknown argument: " << argv[1] << std::endl;
		}

		vr::VR_Shutdown();
		exit(exitCode);
	}
}

int main(int argc, char *argv[])
{
	RolliVROverlay::checkForCommandLineArguments(argc, argv);

    QApplication application(argc, argv);
    auto *overlayWidget = new RolliVROverlayWidget;
    RolliVROverlayController::SharedInstance()->Init();
    RolliVROverlayController::SharedInstance()->SetWidget(overlayWidget);
    return QApplication::exec();
}
