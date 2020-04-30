#include "WheelchairOverlayWidget.h"
#include "WheelchairOverlayController.h"

#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
    QApplication application(argc, argv);
    WheelchairOverlayWidget *overlayWidget = new WheelchairOverlayWidget;

    WheelchairOverlayController::SharedInstance()->Init();
    WheelchairOverlayController::SharedInstance()->SetWidget(overlayWidget);

    return application.exec();
}
