// Adapted code from: https://github.com/ValveSoftware/openvr/tree/master/samples/helloworldoverlay
// Copyright Valve Corporation, All rights reserved.

#pragma once

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

	void OnLeft();
	void OnRight();
	void OnReset();

protected:

private:
	bool ConnectToVRRuntime();
	void DisconnectFromVRRuntime();

	vr::TrackedDevicePose_t m_trackedDevicePose[vr::k_unMaxTrackedDeviceCount];
	QString m_vrDriver;
	QString m_vrDisplay;
	QString m_name;

	vr::HmdError m_lastHmdError;

private:
	vr::HmdError m_compositorError;
	vr::HmdError m_overlayError;
	vr::VROverlayHandle_t m_overlayHandle;
    vr::VROverlayHandle_t m_overlayThumbnailHandle;

	QOpenGLContext *m_openGLContext;
	QGraphicsScene *m_scene;
	QOpenGLFramebufferObject *m_fbo;
	QOffscreenSurface *m_offscreenSurface;

	QTimer *m_pumpEventsTimer;

	WheelchairOverlayWidget *m_widget;

	QPointF m_lastMousePoint;
	Qt::MouseButtons m_lastMouseButtons;
};
