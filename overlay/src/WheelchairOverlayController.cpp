// Adapted code from: https://github.com/ValveSoftware/openvr/tree/master/samples/helloworldoverlay
// Copyright Valve Corporation, All rights reserved.

#include "WheelchairOverlayController.h"
#include "WheelchairOverlayWidget.h"

#include <QOpenGLFramebufferObjectFormat>
#include <QOpenGLPaintDevice>
#include <QPainter>
#include <QtWidgets/QWidget>
#include <QMouseEvent>
#include <QtWidgets/QGraphicsSceneMouseEvent>
#include <QtWidgets/QApplication>
#include <QtWidgets/QGraphicsEllipseItem>
#include <QCursor>

#include <sstream>
#include <iostream>

using namespace vr;

WheelchairOverlayController *sharedWheelchairOverlayController = nullptr;

WheelchairOverlayController *WheelchairOverlayController::SharedInstance()
{
	if (!sharedWheelchairOverlayController) {
		sharedWheelchairOverlayController = new WheelchairOverlayController();
	}
	return sharedWheelchairOverlayController;
}

WheelchairOverlayController::WheelchairOverlayController()
		: BaseClass(), m_vrDriver("No Driver"), m_vrDisplay("No Display"), m_lastHmdError(vr::VRInitError_None),
		  m_compositorError(vr::VRInitError_None), m_overlayError(vr::VRInitError_None),
		  m_overlayHandle(vr::k_ulOverlayHandleInvalid), m_openGLContext(nullptr), m_scene(nullptr), m_fbo(nullptr),
		  m_offscreenSurface(nullptr), m_pumpEventsTimer(nullptr), m_widget(nullptr), m_lastMouseButtons(0)
{
}

QString GetTrackedDeviceString(vr::IVRSystem *pHmd, vr::TrackedDeviceIndex_t unDevice, vr::TrackedDeviceProperty prop)
{
	char buf[128];
	vr::TrackedPropertyError err;
	pHmd->GetStringTrackedDeviceProperty(unDevice, prop, buf, sizeof(buf), &err);
	if (err != vr::TrackedProp_Success) {
		return QString("Error Getting String: ") + pHmd->GetPropErrorNameFromEnum(err);
	} else {
		return buf;
	}
}

bool WheelchairOverlayController::Init()
{
	bool success;

	m_name = "systemoverlay";

	QStringList arguments = qApp->arguments();

	int nNameArg = arguments.indexOf("-name");
	if (nNameArg != -1 && nNameArg + 2 <= arguments.size()) {
		m_name = arguments.at(nNameArg + 1);
	}

	QSurfaceFormat format;
	format.setMajorVersion(4);
	format.setMinorVersion(1);
	format.setProfile(QSurfaceFormat::CompatibilityProfile);

	m_openGLContext = new QOpenGLContext();
	m_openGLContext->setFormat(format);
	success = m_openGLContext->create();

	if (!success) {
		return false;
	}

	// create an offscreen surface to attach the context and FBO to
	m_offscreenSurface = new QOffscreenSurface();
	m_offscreenSurface->create();
	m_openGLContext->makeCurrent(m_offscreenSurface);

	m_scene = new QGraphicsScene();
	connect(m_scene, SIGNAL(changed(const QList<QRectF>&)), this, SLOT(OnSceneChanged(const QList<QRectF>&)));

	// Loading the OpenVR Runtime
	success = ConnectToVRRuntime();

	success = success && vr::VRCompositor() != nullptr;

	if (vr::VROverlay()) {
		std::string sKey = std::string("sample.") + m_name.toStdString();
		vr::VROverlayError overlayError = vr::VROverlay()->CreateDashboardOverlay(sKey.c_str(),
		                                                                          m_name.toStdString().c_str(),
		                                                                          &m_overlayHandle,
		                                                                          &m_overlayThumbnailHandle);
		success = success && overlayError == vr::VROverlayError_None;
	}

	if (success) {
		vr::VROverlay()->SetOverlayWidthInMeters(m_overlayHandle, 1.5f);
		vr::VROverlay()->SetOverlayInputMethod(m_overlayHandle, vr::VROverlayInputMethod_Mouse);

		m_pumpEventsTimer = new QTimer(this);
		connect(m_pumpEventsTimer, SIGNAL(timeout()), this, SLOT(OnTimeoutPumpEvents()));
		m_pumpEventsTimer->setInterval(20);
		m_pumpEventsTimer->start();

	}

	return true;
}

void WheelchairOverlayController::Shutdown()
{
	DisconnectFromVRRuntime();

	delete m_scene;
	delete m_fbo;
	delete m_offscreenSurface;

	if (m_openGLContext) {
//		m_openGLContext->destroy();
		delete m_openGLContext;
		m_openGLContext = nullptr;
	}
}

void WheelchairOverlayController::OnSceneChanged(const QList<QRectF> &)
{
	// skip rendering if the overlay isn't visible
	if ((m_overlayHandle == k_ulOverlayHandleInvalid) || !vr::VROverlay() ||
	    (!vr::VROverlay()->IsOverlayVisible(m_overlayHandle) &&
	     !vr::VROverlay()->IsOverlayVisible(m_overlayThumbnailHandle))) {
		return;
	}

	m_openGLContext->makeCurrent(m_offscreenSurface);
	m_fbo->bind();

	QOpenGLPaintDevice device(m_fbo->size());
	QPainter painter(&device);

	m_scene->render(&painter);

	m_fbo->release();

	GLuint unTexture = m_fbo->texture();
	if (unTexture != 0) {
		vr::Texture_t texture = {(void *)(uintptr_t)unTexture, vr::TextureType_OpenGL, vr::ColorSpace_Auto};
		vr::VROverlay()->SetOverlayTexture(m_overlayHandle, &texture);
	}
}

void WheelchairOverlayController::OnTimeoutPumpEvents()
{
	if (!vr::VRSystem()) {
		return;
	}

	vr::VRActiveActionSet_t actionSet = { 0 };
	actionSet.ulActionSet = m_actionSetMain;
	vr::VRInput()->UpdateActionState(&actionSet, sizeof(actionSet), 1);

	vr::InputAnalogActionData_t analogData;
	if (vr::VRInput()->GetAnalogActionData(m_actionMovementAndRotationInput, &analogData, sizeof(analogData), vr::k_ulInvalidInputValueHandle) == vr::VRInputError_None && analogData.bActive) {
		std::ostringstream ss;
		ss << "x: " << analogData.x << " y: " << analogData.y << " update time: " << analogData.fUpdateTime;
		m_widget->SetTestLabel(ss.str());
		std::cout << ss.str() << std::endl;
	}

	vr::VREvent_t vrEvent;
	while (vr::VROverlay()->PollNextOverlayEvent(m_overlayHandle, &vrEvent, sizeof(vrEvent))) {
		switch (vrEvent.eventType) {
			case vr::VREvent_MouseMove: {
				QPointF ptNewMouse(vrEvent.data.mouse.x, vrEvent.data.mouse.y);
				QPoint ptGlobal = ptNewMouse.toPoint();
				QGraphicsSceneMouseEvent mouseEvent(QEvent::GraphicsSceneMouseMove);
				mouseEvent.setWidget(nullptr);
				mouseEvent.setPos(ptNewMouse);
				mouseEvent.setScenePos(ptGlobal);
				mouseEvent.setScreenPos(ptGlobal);
				mouseEvent.setLastPos(m_lastMousePoint);
				mouseEvent.setLastScenePos(m_widget->mapToGlobal(m_lastMousePoint.toPoint()));
				mouseEvent.setLastScreenPos(m_widget->mapToGlobal(m_lastMousePoint.toPoint()));
				mouseEvent.setButtons(m_lastMouseButtons);
				mouseEvent.setButton(Qt::NoButton);
				mouseEvent.setModifiers(0);
				mouseEvent.setAccepted(false);

				m_lastMousePoint = ptNewMouse;
				QApplication::sendEvent(m_scene, &mouseEvent);

				OnSceneChanged(QList<QRectF>());
			}
				break;

			case vr::VREvent_MouseButtonDown: {
				Qt::MouseButton button =
						vrEvent.data.mouse.button == vr::VRMouseButton_Right ? Qt::RightButton : Qt::LeftButton;

				m_lastMouseButtons |= button;

				QPoint ptGlobal = m_lastMousePoint.toPoint();
				QGraphicsSceneMouseEvent mouseEvent(QEvent::GraphicsSceneMousePress);
				mouseEvent.setWidget(nullptr);
				mouseEvent.setPos(m_lastMousePoint);
				mouseEvent.setButtonDownPos(button, m_lastMousePoint);
				mouseEvent.setButtonDownScenePos(button, ptGlobal);
				mouseEvent.setButtonDownScreenPos(button, ptGlobal);
				mouseEvent.setScenePos(ptGlobal);
				mouseEvent.setScreenPos(ptGlobal);
				mouseEvent.setLastPos(m_lastMousePoint);
				mouseEvent.setLastScenePos(ptGlobal);
				mouseEvent.setLastScreenPos(ptGlobal);
				mouseEvent.setButtons(m_lastMouseButtons);
				mouseEvent.setButton(button);
				mouseEvent.setModifiers(0);
				mouseEvent.setAccepted(false);

				QApplication::sendEvent(m_scene, &mouseEvent);
			}
				break;

			case vr::VREvent_MouseButtonUp: {
				Qt::MouseButton button =
						vrEvent.data.mouse.button == vr::VRMouseButton_Right ? Qt::RightButton : Qt::LeftButton;
				m_lastMouseButtons &= ~button;

				QPoint ptGlobal = m_lastMousePoint.toPoint();
				QGraphicsSceneMouseEvent mouseEvent(QEvent::GraphicsSceneMouseRelease);
				mouseEvent.setWidget(nullptr);
				mouseEvent.setPos(m_lastMousePoint);
				mouseEvent.setScenePos(ptGlobal);
				mouseEvent.setScreenPos(ptGlobal);
				mouseEvent.setLastPos(m_lastMousePoint);
				mouseEvent.setLastScenePos(ptGlobal);
				mouseEvent.setLastScreenPos(ptGlobal);
				mouseEvent.setButtons(m_lastMouseButtons);
				mouseEvent.setButton(button);
				mouseEvent.setModifiers(0);
				mouseEvent.setAccepted(false);

				QApplication::sendEvent(m_scene, &mouseEvent);
			}
				break;

			case vr::VREvent_OverlayShown: {
				m_widget->repaint();
			}
				break;

			case vr::VREvent_Quit:
				QApplication::exit();
				break;
		}
	}

	if (m_overlayThumbnailHandle != vr::k_ulOverlayHandleInvalid) {
		while (vr::VROverlay()->PollNextOverlayEvent(m_overlayThumbnailHandle, &vrEvent, sizeof(vrEvent))) {
			switch (vrEvent.eventType) {
				case vr::VREvent_OverlayShown: {
					m_widget->repaint();
				}
					break;
			}
		}
	}

}

void WheelchairOverlayController::SetWidget(WheelchairOverlayWidget *widget)
{
	if (m_widget != nullptr) {
		disconnect(m_widget, &WheelchairOverlayWidget::Left, this, &WheelchairOverlayController::OnLeft);
		disconnect(m_widget, &WheelchairOverlayWidget::Right, this, &WheelchairOverlayController::OnRight);
		disconnect(m_widget, &WheelchairOverlayWidget::Reset, this, &WheelchairOverlayController::OnReset);
	}

	if (m_scene) {
		widget->move(0, 0);
		m_scene->addWidget(widget);
	}
	m_widget = widget;

	connect(widget, &WheelchairOverlayWidget::Left, this, &WheelchairOverlayController::OnLeft);
	connect(widget, &WheelchairOverlayWidget::Right, this, &WheelchairOverlayController::OnRight);
	connect(widget, &WheelchairOverlayWidget::Reset, this, &WheelchairOverlayController::OnReset);

	m_fbo = new QOpenGLFramebufferObject(widget->width(), widget->height(), GL_TEXTURE_2D);

	if (vr::VROverlay()) {
		vr::HmdVector2_t vecWindowSize = {
				(float)widget->width(),
				(float)widget->height()
		};
		vr::VROverlay()->SetOverlayMouseScale(m_overlayHandle, &vecWindowSize);
	}

}

bool WheelchairOverlayController::ConnectToVRRuntime()
{
	m_lastHmdError = vr::VRInitError_None;
	vr::IVRSystem *vrSystem = vr::VR_Init(&m_lastHmdError, vr::VRApplication_Overlay);

	if (m_lastHmdError != vr::VRInitError_None) {
		m_vrDriver = "No Driver";
		m_vrDisplay = "No Display";
		return false;
	}

	m_vrDriver = GetTrackedDeviceString(vrSystem, vr::k_unTrackedDeviceIndex_Hmd, vr::Prop_TrackingSystemName_String);
	m_vrDisplay = GetTrackedDeviceString(vrSystem, vr::k_unTrackedDeviceIndex_Hmd, vr::Prop_SerialNumber_String);

	std::string actionManifestPath = (QCoreApplication::applicationDirPath() + "/wheelchairoverlay_vractions.json").toStdString();
	vr::VRInput()->SetActionManifestPath(actionManifestPath.c_str());
	vr::VRInput()->GetActionHandle("/actions/main/in/movementAndRotation", &m_actionMovementAndRotationInput);
	vr::VRInput()->GetActionSetHandle("/actions/main", &m_actionSetMain);

	return true;
}

void WheelchairOverlayController::DisconnectFromVRRuntime()
{
	vr::VR_Shutdown();
}

QString WheelchairOverlayController::GetVRDriverString()
{
	return m_vrDriver;
}

QString WheelchairOverlayController::GetVRDisplayString()
{
	return m_vrDisplay;
}

bool WheelchairOverlayController::BHMDAvailable()
{
	return vr::VRSystem() != nullptr;
}

vr::HmdError WheelchairOverlayController::GetLastHmdError()
{
	return m_lastHmdError;
}

void WheelchairOverlayController::OnLeft()
{
	vr::HmdMatrix34_t standingZeroPose;

	vr::VRChaperoneSetup()->GetWorkingStandingZeroPoseToRawTrackingPose(&standingZeroPose);
	standingZeroPose.m[0][3] += 0.1f;
	vr::VRChaperoneSetup()->SetWorkingStandingZeroPoseToRawTrackingPose(&standingZeroPose);
	vr::VRChaperoneSetup()->CommitWorkingCopy(EChaperoneConfigFile_Live);
}

void WheelchairOverlayController::OnRight()
{
	vr::HmdMatrix34_t standingZeroPose;

	vr::VRChaperoneSetup()->GetWorkingStandingZeroPoseToRawTrackingPose(&standingZeroPose);
	standingZeroPose.m[0][3] -= 0.1f;
	vr::VRChaperoneSetup()->SetWorkingStandingZeroPoseToRawTrackingPose(&standingZeroPose);
	vr::VRChaperoneSetup()->CommitWorkingCopy(EChaperoneConfigFile_Live);
}

void WheelchairOverlayController::OnReset()
{
	vr::HmdMatrix34_t standingZeroPose;

	vr::VRChaperoneSetup()->GetWorkingStandingZeroPoseToRawTrackingPose(&standingZeroPose);

	std::cout << standingZeroPose.m[0][0] << ", " << standingZeroPose.m[0][1] << ", " << standingZeroPose.m[0][2] << ", " << standingZeroPose.m[0][3] << std::endl;
	std::cout << standingZeroPose.m[1][0] << ", " << standingZeroPose.m[1][1] << ", " << standingZeroPose.m[1][2] << ", " << standingZeroPose.m[1][3] << std::endl;
	std::cout << standingZeroPose.m[2][0] << ", " << standingZeroPose.m[2][1] << ", " << standingZeroPose.m[1][2] << ", " << standingZeroPose.m[2][3] << std::endl;

	vr::VRChaperoneSetup()->ReloadFromDisk(EChaperoneConfigFile_Live);
}

void WheelchairOverlayController::EnableRestart()
{

}


