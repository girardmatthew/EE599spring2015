#include "lauaudiowidget.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    app.setQuitOnLastWindowClosed(false);
    app.setOrganizationName(QString("Lau Consulting Inc"));
    app.setOrganizationDomain(QString("drhalftone.com"));
    app.setApplicationName(QString("Mosquito"));

    LAUAudioDialog w;
    return w.exec();

    return app.exec();
}
