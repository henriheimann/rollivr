#pragma once

#include <QtWidgets/QWidget>

namespace Ui {
class WheelchairOverlayWidget;
}

class WheelchairOverlayWidget : public QWidget
{
    Q_OBJECT

public:
    explicit WheelchairOverlayWidget(QWidget *parent = nullptr);
    ~WheelchairOverlayWidget() override;

    float GetHeightOffset() const;
    float GetXOffset() const;
    float GetYOffset() const;
    float GetRotationOffset() const;
    float GetTurnSpeed() const;
    float GetMovementSpeed() const;

signals:
	void Reset();
	void ConfigurationChanged();

private slots:
    void on_quitOverlayPushButton_clicked();
	void on_resetFormPushButton_clicked();
	void on_resetPushButton_clicked();

	void on_decrementHeightOffsetPushButton_clicked();
	void on_incrementHeightOffsetPushButton_clicked();
	void on_decrementXOffsetPushButton_clicked();
	void on_incrementXOffsetPushButton_clicked();
	void on_decrementYOffsetPushButton_clicked();
	void on_incrementYOffsetPushButton_clicked();
	void on_decrementRotationOffsetPushButton_clicked();
	void on_incrementRotationOffsetPushButton_clicked();
	void on_decrementTurnSpeedPushButton_clicked();
	void on_incrementTurnSpeedPushButton_clicked();
	void on_decrementMovementSpeedPushButton_clicked();
	void on_incrementMovementSpeedPushButton_clicked();

	void updateHeightOffsetLabel();
	void updateXOffsetLabel();
	void updateYOffsetLabel();
	void updateRotationOffsetLabel();
	void updateTurnSpeedLabel();
	void updateMovementSpeedLabel();

private:
    Ui::WheelchairOverlayWidget *m_ui;

    float m_currentHeightOffset;
    float m_currentXOffset;
    float m_currentYOffset;
    float m_currentRotationOffset;
    float m_currentTurnSpeed;
    float m_currentMovementSpeed;
};
