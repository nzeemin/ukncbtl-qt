#include "stdafx.h"
#include <QtGui/QApplication>
#include <QtCore/QThread>
#include <QtCore/QTimer>
#include <QSettings>
#include "main.h"
#include "mainwindow.h"
#include "Emulator.h"


void RestoreSettings();


QApplication *g_Application;
MainWindow *g_MainWindow;
QSettings *g_Settings;


int main(int argc, char *argv[])
{
    QApplication application(argc, argv);
    g_Application = &application;

    QSettings settings(QSettings::IniFormat, QSettings::UserScope, _T("Back to Life"), _T("UKNCBTL"));
    g_Settings = &settings;

    MainWindow w;
    g_MainWindow = &w;
    w.show();

    if (!InitEmulator()) return 255;

    RestoreSettings();
    w.UpdateMenu();

    QTimer timerFrame;
    QObject::connect(&timerFrame, SIGNAL(timeout()), &w, SLOT(emulatorFrame()), Qt::AutoConnection);
    timerFrame.start(20);

    int result = application.exec();

    DoneEmulator();

    settings.sync();

    return result;
}

QApplication* Global_getApplication()
{
    return g_Application;
}
MainWindow* Global_getMainWindow()
{
    return g_MainWindow;
}
QSettings* Global_getSettings()
{
    return g_Settings;
}

void Global_UpdateAllViews()
{
    Global_getMainWindow()->UpdateAllViews();
}
void Global_UpdateMenu()
{
    Global_getMainWindow()->UpdateMenu();
}

void RestoreSettings()
{
    // Reattach floppy images
    for (int slot = 0; slot < 4; slot++)
    {
        QString path = Settings_GetFloppyFilePath(slot);
        if (path.length() > 0)
        {
            if (! g_pBoard->AttachFloppyImage(slot, qPrintable(path)))
                Settings_SetFloppyFilePath(slot, NULL);
        }
    }

    // Reattach cartridge images
    for (int slot = 0; slot < 2; slot++)
    {
        QString path = Settings_GetCartridgeFilePath(slot);
        if (path.length() > 0)
        {
            Emulator_LoadROMCartridge(slot, qPrintable(path));
            //TODO: If failed to load Then
            //    Settings_SetCartridgeFilePath(slot, NULL);

            QString hardpath = Settings_GetHardFilePath(slot);
            if (hardpath.length() > 0)
            {
                g_pBoard->AttachHardImage(slot, qPrintable(hardpath));
            }
        }
    }
}
