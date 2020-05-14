// Adapted code from: https://github.com/ValveSoftware/openvr/tree/master/samples/helloworldoverlay
// Copyright Valve Corporation, All rights reserved.

#pragma once

#include <chrono>

#define GLM_FORCE_SWIZZLE
#include <glm/glm.hpp>

#include "openvr.h"

#include <QtCore/QtCore>
// because of incompatibilities with QtOpenGL and GLEW we need to cherry pick includes
#include <QtGui/QVector2D>
#include <QtGui/QMatrix4x4>
#include <QtCore/QVector>
#include <QtGui/QVector2D>
#include <QtGui/QVector3D>
#include <QtGui/QOpenGLContext>
#include <QtGui/QOpenGLFramebufferObject>
#include <QtWidgets/QGraphicsScene>
#include <QtGui/QOffscreenSurface>

class WheelchairOverlayWidget;

class WheelchairOverlayController : public QObject
{
	Q_OBJECT
	typedef QObject BaseClass;

public:
    static WheelchairOverlayController *SharedInstance();

public:
    WheelchairOverlayController();
    ~WheelchairOverlayController() override = default;

	bool Init();
	void Shutdown();
	void EnableRestart();

	bool BHMDAvailable();
	vr::HmdError GetLastHmdError();

	QString GetVRDriverString();
	QString GetVRDisplayString();
	QString GetName() { return m_name; }

	void SetWidget(WheelchairOverlayWidget *widget);

public slots:
	void OnSceneChanged(const QList<QRectF>&);
	void OnTimeoutPumpEvents();

	void OnReset();
	void OnConfigurationChanged();

protected:

private:
	bool ConnectToVRRuntime();
	void DisconnectFromVRRuntime();

	void ProcessUIEvents();
	void ProcessBindings();

	void ResetZeroPose();
	void UpdateZeroPose();

	vr::TrackedDevicePose_t m_trackedDevicePose[vr::k_unMaxTrackedDeviceCount]{};
	QString m_vrDriver;
	QString m_vrDisplay;
	QString m_name;

	vr::HmdError m_lastHmdError;

private:
	vr::HmdError m_compositorError;
	vr::HmdError m_overlayError;
	vr::VROverlayHandle_t m_overlayHandle;
    vr::VROverlayHandle_t m_overlayThumbnailHandle{};

	vr::VRActionHandle_t m_actionMovementAndRotationInput = vr::k_ulInvalidActionHandle;
	vr::VRActionSetHandle_t m_actionSetMain = vr::k_ulInvalidActionSetHandle;

	QOpenGLContext *m_openGLContext;
	QGraphicsScene *m_scene;
	QOpenGLFramebufferObject *m_fbo;
	QOffscreenSurface *m_offscreenSurface;

	QTimer *m_pumpEventsTimer;
	std::chrono::milliseconds m_frameTiming;
	std::chrono::system_clock::time_point m_lastFrameTime;

	WheelchairOverlayWidget *m_widget;

	QPointF m_lastMousePoint;
	Qt::MouseButtons m_lastMouseButtons;

	glm::mat4 initialZeroPose{};

	glm::vec3 m_currentTranslation{};
	float m_currentRotation{};

	float m_lastInputX{};
	float m_lastInputY{};
};
