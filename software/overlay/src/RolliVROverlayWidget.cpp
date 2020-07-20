#include <QtCore/qmath.h>
#include "RolliVROverlayWidget.h"
#include "ui_RolliVROverlayWidget.h"
#include "main.h"

RolliVROverlayWidget::RolliVROverlayWidget(QWidget *parent) :
		QWidget(parent),
		m_ui(new Ui::RolliVROverlayWidget),
		m_settings(RolliVROverlay::qtSettingsOrganization, RolliVROverlay::qtSettingsApplication)
{
    m_ui->setupUi(this);
	m_ui->hiddenWhileNotRunningWidget->setVisible(false);

	m_currentHeightOffset = m_settings.value("height_offset", 0).toInt();
	m_currentTurnSpeed = m_settings.value("turn_speed", 100).toInt();
	m_currentMovementSpeed = m_settings.value("movement_speed", 100).toInt();
	m_started = false;

	updateHeightOffsetLabel();
	updateTurnSpeedLabel();
	updateMovementSpeedLabel();
	emit ConfigurationChanged();
}

RolliVROverlayWidget::~RolliVROverlayWidget()
{
    delete m_ui;
}

void RolliVROverlayWidget::on_resetPushButton_clicked()
{
	emit Reset();
}

void RolliVROverlayWidget::on_startStopPushButton_clicked()
{
	if (m_started) {
		emit Stop();
		m_ui->startInfoLabel->setVisible(true);
		m_ui->startStopPushButton->setText("START FORCED MOVEMENT");
		m_ui->hiddenWhileNotRunningWidget->setVisible(false);
	} else {
		emit Start();
		m_ui->startInfoLabel->setVisible(false);
		m_ui->startStopPushButton->setText("STOP FORCED MOVEMENT");
		m_ui->hiddenWhileNotRunningWidget->setVisible(true);
	}

	m_started = !m_started;
}

void RolliVROverlayWidget::updateHeightOffsetLabel()
{
	m_ui->heightOffsetLabel->setText(QString().setNum((float)m_currentHeightOffset / 100.0f, 'f', 2) + QString("m"));
}

void RolliVROverlayWidget::updateTurnSpeedLabel()
{
	m_ui->turnSpeedLabel->setText(QString().setNum(m_currentTurnSpeed) + QString("%"));
}

void RolliVROverlayWidget::updateMovementSpeedLabel()
{
	m_ui->movementSpeedLabel->setText(QString().setNum(m_currentMovementSpeed) + QString("%"));
}

void RolliVROverlayWidget::on_decrementHeightOffsetPushButton_clicked()
{
	m_currentHeightOffset = qMax(m_currentHeightOffset - 5, -100);
	m_settings.setValue("height_offset", m_currentHeightOffset);
	updateHeightOffsetLabel();
	emit ConfigurationChanged();
}

void RolliVROverlayWidget::on_incrementHeightOffsetPushButton_clicked()
{
	m_currentHeightOffset = qMin(m_currentHeightOffset + 5, 100);
	m_settings.setValue("height_offset", m_currentHeightOffset);
	updateHeightOffsetLabel();
	emit ConfigurationChanged();
}

void RolliVROverlayWidget::on_decrementTurnSpeedPushButton_clicked()
{
	m_currentTurnSpeed = qMax(m_currentTurnSpeed - 5, 0);
	m_settings.setValue("turn_speed", m_currentTurnSpeed);
	updateTurnSpeedLabel();
	emit ConfigurationChanged();
}

void RolliVROverlayWidget::on_incrementTurnSpeedPushButton_clicked()
{
	m_currentTurnSpeed = qMin(m_currentTurnSpeed + 5, 200);
	m_settings.setValue("turn_speed", m_currentTurnSpeed);
	updateTurnSpeedLabel();
	emit ConfigurationChanged();
}

void RolliVROverlayWidget::on_decrementMovementSpeedPushButton_clicked()
{
	m_currentMovementSpeed = qMax(m_currentMovementSpeed - 5, 0);
	m_settings.setValue("movement_speed", m_currentMovementSpeed);
	updateMovementSpeedLabel();
	emit ConfigurationChanged();
}

void RolliVROverlayWidget::on_incrementMovementSpeedPushButton_clicked()
{
	m_currentMovementSpeed = qMin(m_currentMovementSpeed + 5, 200);
	m_settings.setValue("movement_speed", m_currentMovementSpeed);
	updateMovementSpeedLabel();
	emit ConfigurationChanged();
}

float RolliVROverlayWidget::GetHeightOffset() const
{
	return (float)m_currentHeightOffset / 100.0f;
}

float RolliVROverlayWidget::GetTurnSpeed() const
{
	return (float)m_currentTurnSpeed / 100.0f;
}

float RolliVROverlayWidget::GetMovementSpeed() const
{
	return (float)m_currentMovementSpeed / 100.0f;
}
