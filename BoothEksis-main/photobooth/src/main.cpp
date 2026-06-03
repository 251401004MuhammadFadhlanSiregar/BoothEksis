#include <QApplication>
#include <QScreen>
#include "mainwindow.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    app.setApplicationName("Space Adventure Photobooth");
    app.setApplicationDisplayName("Bootheksis – Space Adventure");
    app.setStyle("Fusion");

    MainWindow w;
    // Center on screen
    QScreen *screen = QGuiApplication::primaryScreen();
    QRect sg = screen->availableGeometry();
    w.setFixedSize(820, 900);
    w.move(sg.center() - w.rect().center());
    w.show();

    return app.exec();
}