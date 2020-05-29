#include "RolliVROverlayWidget.h"
#include "RolliVROverlayController.h"

#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
    QApplication application(argc, argv);

    RolliVROverlayWidget *overlayWidget = new RolliVROverlayWidget;

    RolliVROverlayController::SharedInstance()->Init();
    RolliVROverlayController::SharedInstance()->SetWidget(overlayWidget);

    return application.exec();
}
