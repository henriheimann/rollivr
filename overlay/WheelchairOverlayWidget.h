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

signals:
	void Test();

private slots:
    void on_quitOverlayPushButton_clicked();
	void on_testPushButton_clicked();

private:
    Ui::WheelchairOverlayWidget *m_ui;
};
