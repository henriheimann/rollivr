#include "WheelchairOverlayWidget.h"
#include "ui_WheelchairOverlayWidget.h"

WheelchairOverlayWidget::WheelchairOverlayWidget(QWidget *parent) :
		QWidget(parent),
		m_ui(new Ui::WheelchairOverlayWidget)
{
    m_ui->setupUi(this);
}

WheelchairOverlayWidget::~WheelchairOverlayWidget()
{
    delete m_ui;
}

void WheelchairOverlayWidget::on_quitOverlayPushButton_clicked()
{
    QApplication::quit();
}

void WheelchairOverlayWidget::on_resetPushButton_clicked()
{
	emit Reset();
}

float WheelchairOverlayWidget::GetHeightOffset() const
{
	return (float)m_ui->heightAdjustmentSlider->value() / 100.0f;
}
