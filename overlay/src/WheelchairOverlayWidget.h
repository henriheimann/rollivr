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

signals:
	void Reset();

private slots:
    void on_quitOverlayPushButton_clicked();
	void on_resetPushButton_clicked();

private:
    Ui::WheelchairOverlayWidget *m_ui;
};
