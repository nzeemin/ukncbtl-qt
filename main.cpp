#include "stdafx.h"
#include <QtGui/QApplication>
#include <QtCore/QThread>
#include <QtCore/QTimer>
#include "mainwindow.h"
#include "Emulator.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.show();

    if (!InitEmulator()) return 255;

    QTimer timerFrame;
    QObject::connect(&timerFrame, SIGNAL(timeout()), &w, SLOT(emulatorFrame()), Qt::AutoConnection);
    timerFrame.start(20);

    int result = a.exec();

    DoneEmulator();

    return result;
}
