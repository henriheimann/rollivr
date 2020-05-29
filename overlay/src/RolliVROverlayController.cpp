// Adapted code from: https://github.com/ValveSoftware/openvr/tree/master/samples/helloworldoverlay
// Copyright Valve Corporation, All rights reserved.

#include "RolliVROverlayController.h"
#include "RolliVROverlayWidget.h"

#include <QOpenGLFramebufferObjectFormat>
#include <QOpenGLPaintDevice>
#include <QPainter>
#include <QtWidgets/QWidget>
#include <QMouseEvent>
#include <QtWidgets/QGraphicsSceneMouseEvent>
#include <QtWidgets/QApplication>
#include <QtWidgets/QGraphicsEllipseItem>
#include <QCursor>

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/string_cast.hpp>

#include <sstream>
#include <iostream>

using namespace vr;

RolliVROverlayController *sharedRolliVROverlayController = nullptr;

RolliVROverlayController *RolliVROverlayController::SharedInstance()
{
	if (!sharedRolliVROverlayController) {
		sharedRolliVROverlayController = new RolliVROverlayController();
	}
	return sharedRolliVROverlayController;
}

RolliVROverlayController::RolliVROverlayController()
		: BaseClass(), m_vrDriver("No Driver"), m_vrDisplay("No Display"), m_lastHmdError(vr::VRInitError_None),
		  m_compositorError(vr::VRInitError_None), m_overlayError(vr::VRInitError_None),
		  m_overlayHandle(vr::k_ulOverlayHandleInvalid), m_openGLContext(nullptr), m_scene(nullptr), m_fbo(nullptr),
		  m_offscreenSurface(nullptr), m_pumpEventsTimer(nullptr), m_widget(nullptr), m_lastMouseButtons(0),
		  m_frameTiming(), m_lastFrameTime()
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

bool RolliVROverlayController::Init()
{
	bool success;

	m_name = "RolliVR Controller Overlay";

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
	connect(m_scene, &QGraphicsScene::changed, this, &RolliVROverlayController::OnSceneChanged);

	// Loading the OpenVR Runtime
	success = ConnectToVRRuntime();

	success = success && vr::VRCompositor() != nullptr;

	if (vr::VROverlay()) {
		std::string key = std::string("rollivr.") + m_name.toStdString();
		vr::VROverlayError overlayError = vr::VROverlay()->CreateDashboardOverlay(key.c_str(),
		                                                                          m_name.toStdString().c_str(),
		                                                                          &m_overlayHandle,
		                                                                          &m_overlayThumbnailHandle);
		success = success && overlayError == vr::VROverlayError_None;
	}

	if (success) {
		vr::VROverlay()->SetOverlayWidthInMeters(m_overlayHandle, 1.5f);
		vr::VROverlay()->SetOverlayInputMethod(m_overlayHandle, vr::VROverlayInputMethod_Mouse);

		std::string thumbIconPath = (QCoreApplication::applicationDirPath() + "/thumbicon.png").toStdString();
		vr::VROverlay()->SetOverlayFromFile(m_overlayThumbnailHandle, thumbIconPath.c_str());

		std::chrono::system_clock::time_point now = std::chrono::system_clock::now();
		this->m_lastFrameTime = now;

		m_pumpEventsTimer = new QTimer(this);
		connect(m_pumpEventsTimer, &QTimer::timeout, this, &RolliVROverlayController::OnTimeoutPumpEvents);
		m_pumpEventsTimer->setInterval(10);
		m_pumpEventsTimer->start();
	}

	return true;
}

void RolliVROverlayController::Shutdown()
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

void RolliVROverlayController::OnSceneChanged(const QList<QRectF> &)
{
	// Skip rendering if the overlay isn't visible
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

void RolliVROverlayController::OnTimeoutPumpEvents()
{
	if (!vr::VRSystem()) {
		return;
	}

	// Update frame timing
	std::chrono::system_clock::time_point now = std::chrono::system_clock::now();
	this->m_frameTiming = std::chrono::duration_cast<std::chrono::milliseconds>(now - this->m_lastFrameTime);
	this->m_lastFrameTime = now;

	//std::cout << "OnTimeoutPumpEvents: " << m_frameTiming.count() << " FPS: " << (1000.0f / m_frameTiming.count()) << std::endl;
	ProcessUIEvents();
	ProcessBindings();
}

void RolliVROverlayController::ProcessUIEvents()
{
	vr::VREvent_t vrEvent{};
	while (vr::VROverlay()->PollNextOverlayEvent(m_overlayHandle, &vrEvent, sizeof(vrEvent))) {
		switch (vrEvent.eventType) {
			case vr::VREvent_MouseMove: {
				QPointF newMousePoint(vrEvent.data.mouse.x, vrEvent.data.mouse.y);
				QPoint globalPoint = newMousePoint.toPoint();
				QGraphicsSceneMouseEvent mouseEvent(QEvent::GraphicsSceneMouseMove);
				mouseEvent.setWidget(nullptr);
				mouseEvent.setPos(newMousePoint);
				mouseEvent.setScenePos(globalPoint);
				mouseEvent.setScreenPos(globalPoint);
				mouseEvent.setLastPos(m_lastMousePoint);
				mouseEvent.setLastScenePos(m_widget->mapToGlobal(m_lastMousePoint.toPoint()));
				mouseEvent.setLastScreenPos(m_widget->mapToGlobal(m_lastMousePoint.toPoint()));
				mouseEvent.setButtons(m_lastMouseButtons);
				mouseEvent.setButton(Qt::NoButton);
				mouseEvent.setModifiers(0);
				mouseEvent.setAccepted(false);

				m_lastMousePoint = newMousePoint;
				QApplication::sendEvent(m_scene, &mouseEvent);

				OnSceneChanged(QList<QRectF>());
				break;
			}

			case vr::VREvent_MouseButtonDown: {
				Qt::MouseButton button =
						vrEvent.data.mouse.button == vr::VRMouseButton_Right ? Qt::RightButton : Qt::LeftButton;

				m_lastMouseButtons |= button;

				QPoint globalPoint = m_lastMousePoint.toPoint();
				QGraphicsSceneMouseEvent mouseEvent(QEvent::GraphicsSceneMousePress);
				mouseEvent.setWidget(nullptr);
				mouseEvent.setPos(m_lastMousePoint);
				mouseEvent.setButtonDownPos(button, m_lastMousePoint);
				mouseEvent.setButtonDownScenePos(button, globalPoint);
				mouseEvent.setButtonDownScreenPos(button, globalPoint);
				mouseEvent.setScenePos(globalPoint);
				mouseEvent.setScreenPos(globalPoint);
				mouseEvent.setLastPos(m_lastMousePoint);
				mouseEvent.setLastScenePos(globalPoint);
				mouseEvent.setLastScreenPos(globalPoint);
				mouseEvent.setButtons(m_lastMouseButtons);
				mouseEvent.setButton(button);
				mouseEvent.setModifiers(0);
				mouseEvent.setAccepted(false);

				QApplication::sendEvent(m_scene, &mouseEvent);
				break;
			}

			case vr::VREvent_MouseButtonUp: {
				Qt::MouseButton button =
						vrEvent.data.mouse.button == vr::VRMouseButton_Right ? Qt::RightButton : Qt::LeftButton;
				m_lastMouseButtons &= ~button;

				QPoint globalPoint = m_lastMousePoint.toPoint();
				QGraphicsSceneMouseEvent mouseEvent(QEvent::GraphicsSceneMouseRelease);
				mouseEvent.setWidget(nullptr);
				mouseEvent.setPos(m_lastMousePoint);
				mouseEvent.setScenePos(globalPoint);
				mouseEvent.setScreenPos(globalPoint);
				mouseEvent.setLastPos(m_lastMousePoint);
				mouseEvent.setLastScenePos(globalPoint);
				mouseEvent.setLastScreenPos(globalPoint);
				mouseEvent.setButtons(m_lastMouseButtons);
				mouseEvent.setButton(button);
				mouseEvent.setModifiers(0);
				mouseEvent.setAccepted(false);

				QApplication::sendEvent(m_scene, &mouseEvent);
				break;
			}

			case vr::VREvent_OverlayShown: {
				m_widget->repaint();
				break;
			}

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
					break;
				}
			}
		}
	}
}

void RolliVROverlayController::ProcessBindings()
{
	vr::VRActiveActionSet_t actionSet = {0};
	actionSet.ulActionSet = m_actionSetMain;
	vr::VRInput()->UpdateActionState(&actionSet, sizeof(actionSet), 1);

	vr::InputAnalogActionData_t analogData{};
	if (vr::VRInput()->GetAnalogActionData(m_actionMovementAndRotationInput, &analogData, sizeof(analogData),
	                                       vr::k_ulInvalidInputValueHandle) == vr::VRInputError_None && analogData.bActive) {
		m_lastInputX = analogData.x;
		m_lastInputY = analogData.y;
	} else {
		m_lastInputX = 0.0f;
		m_lastInputY = 0.0f;
	}

	UpdateZeroPose();
}

void RolliVROverlayController::ResetZeroPose()
{
	m_currentTranslation = glm::vec3{};
	m_currentRotation = 0;

	UpdateZeroPose();
}

void RolliVROverlayController::UpdateZeroPose()
{
	float heightOffset = (m_widget == nullptr) ? 0.0f : m_widget->GetHeightOffset();
	float xOffset = (m_widget == nullptr) ? 0.0f : m_widget->GetXOffset();
	float yOffset = (m_widget == nullptr) ? 0.0f : m_widget->GetYOffset();
	float rotationOffset = (m_widget == nullptr) ? 0.0f : m_widget->GetRotationOffset();
	float turnSpeed = (m_widget == nullptr) ? 1.0f : m_widget->GetTurnSpeed();
	float movementSpeed = (m_widget == nullptr) ? 1.0f : m_widget->GetMovementSpeed();

	float delta = (m_frameTiming.count() / 1000.0f);

	// Don't process input if we are at <10 FPS.
	if (delta > 0.1f || delta < -0.1f) {
		delta = 0.0;
	}

	m_currentRotation += m_lastInputX * delta * turnSpeed;

	// Move forward
	glm::vec3 forward = glm::vec4(0, 0, 1, 0) * glm::rotate(glm::mat4(1.0f), m_currentRotation, glm::vec3(0, 1, 0));
	m_currentTranslation += forward * m_lastInputY * delta * movementSpeed;

	glm::mat4 currentZeroPose = glm::translate(glm::mat4(1.0f), glm::vec3(-xOffset, -heightOffset, -yOffset));
	currentZeroPose = glm::rotate(currentZeroPose, -rotationOffset, glm::vec3(0, 1, 0));
	currentZeroPose = glm::rotate(currentZeroPose, m_currentRotation, glm::vec3(0, 1, 0));
	currentZeroPose = glm::translate(currentZeroPose, m_currentTranslation);

	// Transpose to row major order and store in HmdMatrix34
	glm::mat4 transposedNewZeroPose = glm::transpose(currentZeroPose);

	// Copy over to SteamVR zero pose
	vr::HmdMatrix34_t vrNewZeroPose{};
	memcpy(&vrNewZeroPose.m[0][0], glm::value_ptr(transposedNewZeroPose), sizeof(vrNewZeroPose.m));

	vr::VRChaperoneSetup()->SetWorkingStandingZeroPoseToRawTrackingPose(&vrNewZeroPose);
	vr::VRChaperoneSetup()->CommitWorkingCopy(EChaperoneConfigFile_Live);
}

void RolliVROverlayController::SetWidget(RolliVROverlayWidget *widget)
{
	if (m_widget != nullptr) {
		disconnect(m_widget, &RolliVROverlayWidget::Start, this, &RolliVROverlayController::OnStart);
		disconnect(m_widget, &RolliVROverlayWidget::Stop, this, &RolliVROverlayController::OnStop);
		disconnect(m_widget, &RolliVROverlayWidget::ConfigurationChanged, this, &RolliVROverlayController::OnConfigurationChanged);
	}

	if (m_scene) {
		widget->move(0, 0);
		m_scene->addWidget(widget);
	}
	m_widget = widget;

	connect(widget, &RolliVROverlayWidget::Start, this, &RolliVROverlayController::OnStart);
	connect(widget, &RolliVROverlayWidget::Stop, this, &RolliVROverlayController::OnStop);
	connect(widget, &RolliVROverlayWidget::ConfigurationChanged, this, &RolliVROverlayController::OnConfigurationChanged);

	m_fbo = new QOpenGLFramebufferObject(widget->width(), widget->height(), GL_TEXTURE_2D);

	if (vr::VROverlay()) {
		vr::HmdVector2_t vecWindowSize = {
				(float)widget->width(),
				(float)widget->height()
		};
		vr::VROverlay()->SetOverlayMouseScale(m_overlayHandle, &vecWindowSize);
	}
}

bool RolliVROverlayController::ConnectToVRRuntime()
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

	std::string actionManifestPath = (QCoreApplication::applicationDirPath() + "/rollivr_overlay_vractions.json").toStdString();
	vr::VRInput()->SetActionManifestPath(actionManifestPath.c_str());
	vr::VRInput()->GetActionHandle("/actions/main/in/movementAndRotation", &m_actionMovementAndRotationInput);
	vr::VRInput()->GetActionSetHandle("/actions/main", &m_actionSetMain);

	vr::VRSettings()->SetBool(vr::k_pch_CollisionBounds_Section, vr::k_pch_CollisionBounds_CenterMarkerOn_Bool, true);

	// Reset Chaperone
	vr::VRChaperoneSetup()->ReloadFromDisk(EChaperoneConfigFile_Live);

	vr::HmdMatrix34_t vrZeroPose{};
	vr::VRChaperoneSetup()->GetWorkingStandingZeroPoseToRawTrackingPose(&vrZeroPose);

	// Copy from HmdMatrix34 and transpose to column major order
	memcpy(glm::value_ptr(initialZeroPose), &vrZeroPose.m[0][0], sizeof(vrZeroPose.m));
	initialZeroPose = glm::transpose(initialZeroPose);

	ResetZeroPose();

	return true;
}

void RolliVROverlayController::DisconnectFromVRRuntime()
{
	vr::VR_Shutdown();
}

QString RolliVROverlayController::GetVRDriverString()
{
	return m_vrDriver;
}

QString RolliVROverlayController::GetVRDisplayString()
{
	return m_vrDisplay;
}

bool RolliVROverlayController::BHMDAvailable()
{
	return vr::VRSystem() != nullptr;
}

vr::HmdError RolliVROverlayController::GetLastHmdError()
{
	return m_lastHmdError;
}

void RolliVROverlayController::OnStart()
{
	ResetZeroPose();

	TrackedDevicePose_t hmdPose{0};
	vr::VRSystem()->GetDeviceToAbsoluteTrackingPose(TrackingUniverseStanding, 0.0f, &hmdPose, 1);

	glm::mat4 headsetMatrix(1.0f);
	memcpy(glm::value_ptr(headsetMatrix), &hmdPose.mDeviceToAbsoluteTracking.m[0][0], sizeof(hmdPose.mDeviceToAbsoluteTracking.m));
	headsetMatrix = glm::transpose(headsetMatrix);

	float heightOffset = m_widget->GetHeightOffset();
	float xOffset = m_widget->GetXOffset();
	float yOffset = m_widget->GetYOffset();
	float rotationOffset = m_widget->GetRotationOffset();

	glm::mat4 currentZeroPose =  glm::translate(glm::mat4(1.0f), glm::vec3(-xOffset, -heightOffset, -yOffset));
	currentZeroPose = glm::rotate(currentZeroPose, -rotationOffset, glm::vec3(0, 1, 0));

	glm::vec2 forward = glm::normalize((currentZeroPose * (headsetMatrix * glm::vec4(0, 0, 1, 0))).xz());
	glm::vec2 offset = (currentZeroPose * -(headsetMatrix * glm::vec4(0, 0, 0, 1))).xz();

	float rotation = std::atan2((float)forward.y, (float)forward.x) - M_PI / 2.0f;
	float x = offset.x;
	float y = offset.y;

	m_widget->MoveOffsets(x, y, rotation);
	UpdateZeroPose();
}

void RolliVROverlayController::OnStop()
{
	ResetZeroPose();
}

void RolliVROverlayController::OnConfigurationChanged()
{
	UpdateZeroPose();
}
