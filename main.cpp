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

void ParseCommandLine(int argc, char *argv[]);

#ifdef Q_OS_WIN
#define OPTIONCHAR '/'
#define OPTIONSTR "/"
#else
#define OPTIONCHAR '-'
#define OPTIONSTR "-"
#endif

const char CommandLineHelp[] =
    "Command line options:\n"
    OPTIONSTR "h " OPTIONSTR "help    Show command line options\n"
    OPTIONSTR "boot    Auto-start the emulation, select boot from disk\n"
    OPTIONSTR "bootN    Auto-start the emulation, select boot menu item N=1..7\n"
    OPTIONSTR "autostart " OPTIONSTR "autostarton    Start emulation on window open\n"
    OPTIONSTR "noautostart " OPTIONSTR "autostartoff    Do not start emulation on window open\n"
    OPTIONSTR "sound " OPTIONSTR "soundon    Turn sound on\n"
    OPTIONSTR "nosound " OPTIONSTR "soundoff    Turn sound off\n"
    OPTIONSTR "diskN:filePath    Attach disk image, N=0..3\n"
    OPTIONSTR "cartN:filePath    Attach cartridge image, N=1..2\n"
    OPTIONSTR "hardN:filePath    Attach hard disk image, N=1..2\n";


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

    ParseCommandLine(argc, argv);  // Override settings by command-line option if needed

    Emulator_SetSound(Settings_GetSound());
    Emulator_SetSoundAY(Settings_GetSoundAY());

    if (!Emulator_Init())
        return 255;

    w.restoreSettings();

    w.show();

    RestoreSettings();
    w.updateMenu();
    w.updateAllViews();

    if (Option_ShowHelp)
    {
        AlertInfo(CommandLineHelp);
    }

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
    Global_getMainWindow()->updateAllViews();
}
void Global_UpdateMenu()
{
    Global_getMainWindow()->updateMenu();
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

void ParseCommandLine(int argc, char *argv[])
{
    char** it = argv;
    char** itend = argv + argc;
    while (it != itend)
    {
        const char* param = *it;
        if (param[0] == OPTIONCHAR)
        {
            QString option = QString::fromLocal8Bit(param + 1);
            if (option == "help" || option == "h")
            {
                Option_ShowHelp = true;
            }
            else if (option.startsWith("boot"))
            {
                Option_AutoBoot = 1;
                if (option.length() > 4 && option[4] >= '1' && option[4] <= '7')
                {
                    Option_AutoBoot = option[4].toLatin1() - '0';
                }
            }
            else if (option == "autostart" || option == "autostarton")
            {
                Settings_SetAutostart(true);
            }
            else if (option == "noautostart" || option == "autostartoff")
            {
                Settings_SetAutostart(false);
            }
            else if (option == "sound" || option == "soundon")
            {
                Settings_SetSound(true);
            }
            else if (option == "soundoff" || option == "nosound")
            {
                Settings_SetSound(false);
            }
            else if (option.startsWith("disk") && option.length() > 6 && // "/diskN:filePath", N=0..3
                    option[4] >= '0' && option[4] <= '3' && option[5] == ':')
            {
                int slot = option[4].toLatin1() - '0';
                Settings_SetFloppyFilePath(slot, option.mid(6));
            }
            else if (option.startsWith("cart") && option.length() > 6 && // "/cartN:filePath", N=1..2
                    option[4] >= '1' && option[4] <= '2' && option[5] == ':')
            {
                int slot = option[4].toLatin1() - '0';
                Settings_SetCartridgeFilePath(slot, option.mid(6));
            }
            else if (option.startsWith("hard") && option.length() > 6 && // "/hardN:filePath", N=1..2
                    option[4] >= '1' && option[4] <= '2' && option[5] == ':')
            {
                int slot = option[4].toLatin1() - '0';
                Settings_SetHardFilePath(slot, option.mid(6));
            }
            //TODO
        }

        ++it;
    }
}
