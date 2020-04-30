#ifndef OVERLAYWIDGET_H
#define OVERLAYWIDGET_H

#include <QtWidgets/QWidget>

namespace Ui {
class WheelchairOverlayWidget;
}

class OverlayWidget : public QWidget
{
    Q_OBJECT

public:
    explicit OverlayWidget(QWidget *parent = 0);
    ~OverlayWidget();

private slots:
    void on_pushButton_clicked();

private:
    Ui::WheelchairOverlayWidget *ui;
};

#endif // OVERLAYWIDGET_H
