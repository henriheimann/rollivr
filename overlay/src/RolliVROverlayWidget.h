#pragma once

#include <QtWidgets/QWidget>

namespace Ui {
class RolliVROverlayWidget;
}

class RolliVROverlayWidget : public QWidget
{
    Q_OBJECT

public:
    explicit RolliVROverlayWidget(QWidget *parent = nullptr);
    ~RolliVROverlayWidget() override;

    float GetHeightOffset() const;
    float GetTurnSpeed() const;
    float GetMovementSpeed() const;

signals:
	void ConfigurationChanged();
	void Start();
	void Stop();
	void Reset();

private slots:
	void on_startStopPushButton_clicked();
	void on_resetPushButton_clicked();

	void on_decrementHeightOffsetPushButton_clicked();
	void on_incrementHeightOffsetPushButton_clicked();
	void on_decrementTurnSpeedPushButton_clicked();
	void on_incrementTurnSpeedPushButton_clicked();
	void on_decrementMovementSpeedPushButton_clicked();
	void on_incrementMovementSpeedPushButton_clicked();

	void updateHeightOffsetLabel();
	void updateTurnSpeedLabel();
	void updateMovementSpeedLabel();

private:
    Ui::RolliVROverlayWidget *m_ui;

    bool m_started;
    int m_currentHeightOffset;
	int m_currentTurnSpeed;
	int m_currentMovementSpeed;
};
