#include "WheelchairOverlayWidget.h"
#include "ui_WheelchairOverlayWidget.h"

OverlayWidget::OverlayWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::WheelchairOverlayWidget)
{
    ui->setupUi(this);
}

OverlayWidget::~OverlayWidget()
{
    delete ui;
}

void OverlayWidget::on_pushButton_clicked()
{
    QApplication::quit();
}
