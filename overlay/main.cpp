#include "WheelchairOverlayWidget.h"
#include "WheelchairOverlayController.h"

#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    OverlayWidget *pOverlayWidget = new OverlayWidget;

    WheelchairOverlayController::SharedInstance()->Init();
    WheelchairOverlayController::SharedInstance()->SetWidget(pOverlayWidget );

    return a.exec();
}
