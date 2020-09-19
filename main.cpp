#include "stdafx.h"
#include <QApplication>
#include <QtCore/QThread>
#include <QtCore/QTimer>
#include <QSettings>
#include <QTranslator>
#include "main.h"
#include "mainwindow.h"
#include "Emulator.h"


void RestoreSettings();


static QApplication *g_Application;
static MainWindow *g_MainWindow;
static QSettings *g_Settings;

#if !defined(QT_NO_DEBUG)
extern void UnitTests_ExecuteAll();  // Defined in UnitTests.cpp
#endif

int main(int argc, char *argv[])
{
#if !defined(QT_NO_DEBUG)
    if (argc > 1 && _stricmp(argv[1], "-test") == 0)
    {
        UnitTests_ExecuteAll();
        return 0;
    }
#endif

    QApplication application(argc, argv);
    g_Application = &application;

//    QTranslator translator;
//    translator.load("ukncbtl_en.tr");
//    application.installTranslator(&translator);

    QSettings settings(QSettings::IniFormat, QSettings::UserScope, "Back to Life", "UKNCBTL");
    g_Settings = &settings;

    MainWindow w;
    g_MainWindow = &w;

    Emulator_SetSound(Settings_GetSound());

    if (!Emulator_Init())
        return 255;

    w.restoreSettings();

    w.show();

    RestoreSettings();
    w.UpdateMenu();
    w.UpdateAllViews();

    QTimer timerFrame;
    QObject::connect(&timerFrame, SIGNAL(timeout()), &w, SLOT(emulatorFrame()), Qt::AutoConnection);
    timerFrame.start(32);

    int result = application.exec();

    Emulator_Done();

    settings.sync();

    Common_Cleanup();

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
void Global_RedrawDebugView()
{
    Global_getMainWindow()->redrawDebugView();
}
void Global_RedrawDisasmView()
{
    Global_getMainWindow()->redrawDisasmView();
}
void Global_SetCurrentProc(bool okProc)
{
    Global_getMainWindow()->setCurrentProc(okProc);
}

void Global_showUptime(int uptimeMillisec)
{
    Global_getMainWindow()->showUptime(uptimeMillisec);
}
void Global_showFps(double framesPerSecond)
{
    Global_getMainWindow()->showFps(framesPerSecond);
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
                Settings_SetFloppyFilePath(slot, nullptr);
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
            //    Settings_SetCartridgeFilePath(slot, nullptr);

            QString hardpath = Settings_GetHardFilePath(slot);
            if (hardpath.length() > 0)
            {
                g_pBoard->AttachHardImage(slot, qPrintable(hardpath));
            }
        }
    }
}
