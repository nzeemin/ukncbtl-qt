#include "stdafx.h"
#include <QMessageBox>
#include <QFileDialog>
#include <QAction>
#include <QSettings>
#include <QVBoxLayout>
#include <QDockWidget>
#include <QLabel>
#include "main.h"
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "qscreen.h"
#include "qkeyboardview.h"
#include "qconsoleview.h"
#include "qdebugview.h"
#include "qdisasmview.h"
#include "qmemoryview.h"
#include "Emulator.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    this->setWindowTitle(_T("UKNC Back to Life"));

    // Assign signals
    QObject::connect(ui->actionFileScreenshot, SIGNAL(triggered()), this, SLOT(fileScreenshot()));
    QObject::connect(ui->actionFileExit, SIGNAL(triggered()), this, SLOT(close()));
    QObject::connect(ui->actionEmulatorRun, SIGNAL(triggered()), this, SLOT(emulatorRun()));
    QObject::connect(ui->actionEmulatorReset, SIGNAL(triggered()), this, SLOT(emulatorReset()));
    QObject::connect(ui->actionDrivesFloppy0, SIGNAL(triggered()), this, SLOT(emulatorFloppy0()));
    QObject::connect(ui->actionDrivesFloppy1, SIGNAL(triggered()), this, SLOT(emulatorFloppy1()));
    QObject::connect(ui->actionDrivesFloppy2, SIGNAL(triggered()), this, SLOT(emulatorFloppy2()));
    QObject::connect(ui->actionDrivesFloppy3, SIGNAL(triggered()), this, SLOT(emulatorFloppy3()));
    QObject::connect(ui->actionDrivesCartridge1, SIGNAL(triggered()), this, SLOT(emulatorCartridge1()));
    QObject::connect(ui->actionDrivesHard1, SIGNAL(triggered()), this, SLOT(emulatorHardDrive1()));
    QObject::connect(ui->actionDrivesCartridge2, SIGNAL(triggered()), this, SLOT(emulatorCartridge2()));
    QObject::connect(ui->actionDrivesHard2, SIGNAL(triggered()), this, SLOT(emulatorHardDrive2()));
    QObject::connect(ui->actionDebugConsoleView, SIGNAL(triggered()), this, SLOT(debugConsoleView()));
    QObject::connect(ui->actionDebugDebugView, SIGNAL(triggered()), this, SLOT(debugDebugView()));
    QObject::connect(ui->actionDebugDisasmView, SIGNAL(triggered()), this, SLOT(debugDisasmView()));
    QObject::connect(ui->actionDebugMemoryView, SIGNAL(triggered()), this, SLOT(debugMemoryView()));
    QObject::connect(ui->actionDebugStepInto, SIGNAL(triggered()), this, SLOT(debugStepInto()));
    QObject::connect(ui->actionDebugStepOver, SIGNAL(triggered()), this, SLOT(debugStepOver()));
    QObject::connect(ui->actionHelpAbout, SIGNAL(triggered()), this, SLOT(helpAbout()));
    QObject::connect(ui->actionViewRgbScreen, SIGNAL(triggered()), this, SLOT(viewRgbScreen()));
    QObject::connect(ui->actionViewGrayscaleScreen, SIGNAL(triggered()), this, SLOT(viewGrayscaleScreen()));
    QObject::connect(ui->actionViewSizeRegular, SIGNAL(triggered()), this, SLOT(viewSizeRegular()));
    QObject::connect(ui->actionViewSizeDouble, SIGNAL(triggered()), this, SLOT(viewSizeDouble()));
    QObject::connect(ui->actionViewSizeUpscaled, SIGNAL(triggered()), this, SLOT(viewSizeUpscaled()));

    // Screen and keyboard
    m_screen = new QScreen();
    m_keyboard = new QKeyboardView();
    m_console = new QConsoleView();
    m_debug = new QDebugView();
    m_disasm = new QDisasmView();
    m_memory = new QMemoryView();

    QVBoxLayout *vboxlayout = new QVBoxLayout;
    vboxlayout->setMargin(4);
    vboxlayout->setSpacing(4);
    vboxlayout->addWidget(m_screen);
    vboxlayout->addWidget(m_keyboard);
    ui->centralWidget->setLayout(vboxlayout);
    ui->centralWidget->setMaximumHeight(m_screen->maximumHeight() + m_keyboard->maximumHeight());
    int maxwid = m_screen->maximumWidth() > m_keyboard->maximumWidth() ? m_screen->maximumWidth() : m_keyboard->maximumWidth();
    ui->centralWidget->setMaximumWidth(maxwid);

    m_dockDebug = new QDockWidget(_T("Processor"));
    m_dockDebug->setObjectName(_T("dockDebug"));
    m_dockDebug->setWidget(m_debug);
    m_dockDisasm = new QDockWidget(_T("Disassemble"));
    m_dockDisasm->setObjectName(_T("dockDisasm"));
    m_dockDisasm->setWidget(m_disasm);
    m_dockMemory = new QDockWidget(_T("Memory"));
    m_dockMemory->setObjectName(_T("dockMemory"));
    m_dockMemory->setWidget(m_memory);
    m_dockConsole = new QDockWidget(_T("Debug Console"));
    m_dockConsole->setObjectName(_T("dockConsole"));
    m_dockConsole->setWidget(m_console);

    this->setCorner(Qt::BottomRightCorner, Qt::RightDockWidgetArea);
    this->setCorner(Qt::TopRightCorner, Qt::RightDockWidgetArea);
    this->addDockWidget(Qt::RightDockWidgetArea, m_dockDebug, Qt::Vertical);
    this->addDockWidget(Qt::RightDockWidgetArea, m_dockDisasm, Qt::Vertical);
    this->addDockWidget(Qt::RightDockWidgetArea, m_dockMemory, Qt::Vertical);
    this->addDockWidget(Qt::BottomDockWidgetArea, m_dockConsole);

    m_statusLabelInfo = new QLabel();
    m_statusLabelFrames = new QLabel();
    m_statusLabelUptime = new QLabel();
    statusBar()->addWidget(m_statusLabelInfo, 600);
    statusBar()->addPermanentWidget(m_statusLabelFrames, 150);
    statusBar()->addPermanentWidget(m_statusLabelUptime, 150);

    this->setFocusProxy(m_screen);
}

MainWindow::~MainWindow()
{
    delete ui;
    delete m_screen;
    delete m_keyboard;
    delete m_console;
    delete m_debug;
    delete m_disasm;
    delete m_memory;
    delete m_dockConsole;
    delete m_dockDebug;
    delete m_dockDisasm;
    delete m_dockMemory;
}

void MainWindow::changeEvent(QEvent *e)
{
    QMainWindow::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        ui->retranslateUi(this);
        break;
    default:
        break;
    }
}

void MainWindow::closeEvent(QCloseEvent *)
{
    Global_getSettings()->setValue("MainWindow/Geometry", saveGeometry());
    Global_getSettings()->setValue("MainWindow/WindowState", saveState());

    Global_getSettings()->setValue("MainWindow/ConsoleView", m_dockConsole->isVisible());
    Global_getSettings()->setValue("MainWindow/DebugView", m_dockDebug->isVisible());
    Global_getSettings()->setValue("MainWindow/DisasmView", m_dockDisasm->isVisible());
    Global_getSettings()->setValue("MainWindow/MemoryView", m_dockMemory->isVisible());
}

void MainWindow::restoreSettings()
{
    restoreGeometry(Global_getSettings()->value("MainWindow/Geometry").toByteArray());
    restoreState(Global_getSettings()->value("MainWindow/WindowState").toByteArray());

    m_dockConsole->setVisible(Global_getSettings()->value("MainWindow/ConsoleView", false).toBool());
    m_dockDebug->setVisible(Global_getSettings()->value("MainWindow/DebugView", false).toBool());
    m_dockDisasm->setVisible(Global_getSettings()->value("MainWindow/DisasmView", false).toBool());
    m_dockMemory->setVisible(Global_getSettings()->value("MainWindow/MemoryView", false).toBool());
}

void MainWindow::UpdateMenu()
{
    ui->actionEmulatorRun->setChecked(g_okEmulatorRunning);
    ui->actionViewRgbScreen->setChecked(m_screen->mode() == RGBScreen);
    ui->actionViewGrayscaleScreen->setChecked(m_screen->mode() == GrayScreen);
    ui->actionViewSizeRegular->setChecked(m_screen->sizeMode() == RegularScreen);
    ui->actionViewSizeUpscaled->setChecked(m_screen->sizeMode() == UpscaledScreen);
    ui->actionViewSizeDouble->setChecked(m_screen->sizeMode() == DoubleScreen);

    ui->actionDrivesFloppy0->setIcon(QIcon(
            g_pBoard->IsFloppyImageAttached(0) ? _T(":/images/iconFloppy.png") : _T(":/images/iconFloppySlot.png") ));
    ui->actionDrivesFloppy1->setIcon(QIcon(
            g_pBoard->IsFloppyImageAttached(1) ? _T(":/images/iconFloppy.png") : _T(":/images/iconFloppySlot.png") ));
    ui->actionDrivesFloppy2->setIcon(QIcon(
            g_pBoard->IsFloppyImageAttached(2) ? _T(":/images/iconFloppy.png") : _T(":/images/iconFloppySlot.png") ));
    ui->actionDrivesFloppy3->setIcon(QIcon(
            g_pBoard->IsFloppyImageAttached(3) ? _T(":/images/iconFloppy.png") : _T(":/images/iconFloppySlot.png") ));

    ui->actionDrivesCartridge1->setIcon(QIcon(
            g_pBoard->IsROMCartridgeLoaded(1) ? _T(":/images/iconCartridge.png") : _T(":/images/iconCartridgeSlot.png") ));
    ui->actionDrivesCartridge2->setIcon(QIcon(
            g_pBoard->IsROMCartridgeLoaded(2) ? _T(":/images/iconCartridge.png") : _T(":/images/iconCartridgeSlot.png") ));

    ui->actionDrivesHard1->setIcon(QIcon(
            g_pBoard->IsHardImageAttached(1) ? _T(":/images/iconHdd.png") : _T(":/images/iconHddSlot.png") ));
    ui->actionDrivesHard2->setIcon(QIcon(
            g_pBoard->IsHardImageAttached(2) ? _T(":/images/iconHdd.png") : _T(":/images/iconHddSlot.png") ));

    ui->actionDebugConsoleView->setChecked(m_console->isVisible());
    ui->actionDebugDebugView->setChecked(m_dockDebug->isVisible());
    ui->actionDebugDisasmView->setChecked(m_dockDisasm->isVisible());
    ui->actionDebugMemoryView->setChecked(m_dockMemory->isVisible());
}

void MainWindow::UpdateAllViews()
{
    Emulator_OnUpdate();

    if (m_debug != NULL)
        m_debug->updateData();
    if (m_disasm != NULL)
        m_disasm->updateData();
    if (m_memory != NULL)
        m_memory->updateData();

    m_screen->repaint();
    if (m_debug != NULL)
        m_debug->repaint();
    if (m_disasm != NULL)
        m_disasm->repaint();
    if (m_memory != NULL)
        m_memory->repaint();

    UpdateMenu();
}

void MainWindow::setCurrentProc(bool okProc)
{
    if (m_debug != NULL)
        m_debug->setCurrentProc(okProc);
    if (m_disasm != NULL)
        m_disasm->setCurrentProc(okProc);
    if (m_console != NULL)
        m_console->setCurrentProc(okProc);
}

void MainWindow::showUptime(int uptimeMillisec)
{
    int seconds = (int) (uptimeMillisec % 60);
    int minutes = (int) (uptimeMillisec / 60 % 60);
    int hours   = (int) (uptimeMillisec / 3600 % 60);

    TCHAR buffer[20];
    _sntprintf(buffer, 20, _T("Uptime: %02d:%02d:%02d"), hours, minutes, seconds);

    m_statusLabelUptime->setText(buffer);
}
void MainWindow::showFps(double framesPerSecond)
{
    if (framesPerSecond <= 0)
    {
        m_statusLabelFrames->setText(_T(""));
    }
    else
    {
        double speed = framesPerSecond / 25.0 * 100.0;
        TCHAR buffer[16];
        _sntprintf(buffer, 16, _T("%03.f%%"), speed);
        m_statusLabelFrames->setText(buffer);
    }
}

void MainWindow::fileScreenshot()
{
    QFileDialog dlg;
    dlg.setAcceptMode(QFileDialog::AcceptSave);
    dlg.setNameFilter(_T("PNG images (*.png)"));
    if (dlg.exec() == QDialog::Rejected)
        return;

    QString strFileName = dlg.selectedFiles().at(0);

    m_screen->saveScreenshot(strFileName);
}

void MainWindow::helpAbout()
{
    QMessageBox::about(this, _T("About"), _T(
        "QtUkncBtl Version 1.0\n"
        "Copyright (C) 2007-2011\n\n"
        "http://www.felixl.com/Uknc\r\nhttp://code.google.com/p/ukncbtl/\n\n"
        "Authors:\r\nNikita Zeemin (nzeemin@gmail.com)\nFelix Lazarev (felix.lazarev@gmail.com)\nAlexey Kisly\n\n"
        "Special thanks to:\nArseny Gordin\n\n"
        "Build date:\t" __DATE__ " " __TIME__ "\n"
        "Qt version:\t" QT_VERSION_STR));
}

void MainWindow::viewRgbScreen()
{
    m_screen->setMode(RGBScreen);
    UpdateMenu();
}
void MainWindow::viewGrayscaleScreen()
{
    m_screen->setMode(GrayScreen);
    UpdateMenu();
}

void MainWindow::viewSizeRegular()
{
    m_screen->setSizeMode(RegularScreen);
    UpdateMenu();

    //Update centralWidget size
    ui->centralWidget->setMaximumHeight(m_screen->maximumHeight() + m_keyboard->maximumHeight());
}
void MainWindow::viewSizeUpscaled()
{
    m_screen->setSizeMode(UpscaledScreen);
    UpdateMenu();

    //Update centralWidget size
    ui->centralWidget->setMaximumHeight(m_screen->maximumHeight() + m_keyboard->maximumHeight());
}
void MainWindow::viewSizeDouble()
{
    m_screen->setSizeMode(DoubleScreen);
    UpdateMenu();

    //Update centralWidget size
    ui->centralWidget->setMaximumHeight(m_screen->maximumHeight() + m_keyboard->maximumHeight());
}

void MainWindow::emulatorFrame()
{
    if (!g_okEmulatorRunning)
        return;

    if (Emulator_IsBreakpoint())
        Emulator_Stop();
    else if (Emulator_SystemFrame())
    {
        m_screen->repaint();
    }
}

void MainWindow::emulatorRun()
{
    if (g_okEmulatorRunning)
    {
        this->setWindowTitle(_T("UKNC Back to Life"));
        Emulator_Stop();
    }
    else
    {
        this->setWindowTitle(_T("UKNC Back to Life [run]"));
        Emulator_Start();
    }
}

void MainWindow::emulatorReset()
{
    Emulator_Reset();

    m_screen->repaint();
}

void MainWindow::emulatorCartridge1() { emulatorCartridge(1); }
void MainWindow::emulatorCartridge2() { emulatorCartridge(2); }
void MainWindow::emulatorCartridge(int slot)
{
    if (g_pBoard->IsROMCartridgeLoaded(slot))
    {
        g_pBoard->UnloadROMCartridge(slot);
        Settings_SetCartridgeFilePath(slot, NULL);
    }
    else
    {
        QFileDialog dlg;
        dlg.setNameFilter(_T("UKNC ROM cartridge images (*.bin)"));
        if (dlg.exec() == QDialog::Rejected)
            return;

        QString strFileName = dlg.selectedFiles().at(0);
        LPCTSTR sFileName = qPrintable(strFileName);

        Emulator_LoadROMCartridge(slot, sFileName);

        Settings_SetCartridgeFilePath(slot, sFileName);
    }

    UpdateMenu();
}

void MainWindow::emulatorFloppy0() { emulatorFloppy(0); }
void MainWindow::emulatorFloppy1() { emulatorFloppy(1); }
void MainWindow::emulatorFloppy2() { emulatorFloppy(2); }
void MainWindow::emulatorFloppy3() { emulatorFloppy(3); }
void MainWindow::emulatorFloppy(int slot)
{
    if (g_pBoard->IsFloppyImageAttached(slot))
    {
        g_pBoard->DetachFloppyImage(slot);
        Settings_SetFloppyFilePath(slot, NULL);
    }
    else
    {
        QFileDialog dlg;
        dlg.setNameFilter(_T("UKNC floppy images (*.dsk *.rtd)"));
        if (dlg.exec() == QDialog::Rejected)
            return;

        QString strFileName = dlg.selectedFiles().at(0);
        LPCTSTR sFileName = qPrintable(strFileName);

        if (! g_pBoard->AttachFloppyImage(slot, sFileName))
        {
            AlertWarning(_T("Failed to attach floppy image."));
            return;
        }

        Settings_SetFloppyFilePath(slot, sFileName);
    }

    UpdateMenu();
}

void MainWindow::emulatorHardDrive1() { emulatorHardDrive(1); }
void MainWindow::emulatorHardDrive2() { emulatorHardDrive(2); }
void MainWindow::emulatorHardDrive(int slot)
{
    if (g_pBoard->IsHardImageAttached(slot))
    {
        g_pBoard->DetachHardImage(slot);
        Settings_SetHardFilePath(slot, NULL);
    }
    else
    {
        // Check if cartridge (HDD ROM image) already selected
        BOOL okCartLoaded = g_pBoard->IsROMCartridgeLoaded(slot);
        if (!okCartLoaded)
        {
            AlertWarning(_T("Please select HDD ROM image as cartridge first."));
            return;
        }

        // Select HDD disk image
        QFileDialog dlg;
        dlg.setNameFilter(_T("UKNC HDD images (*.img)"));
        if (dlg.exec() == QDialog::Rejected)
            return;

        QString strFileName = dlg.selectedFiles().at(0);
        LPCTSTR sFileName = qPrintable(strFileName);

        // Attach HDD disk image
        g_pBoard->AttachHardImage(slot, sFileName);

        Settings_SetHardFilePath(slot, sFileName);
    }

    UpdateMenu();
}

void MainWindow::debugConsoleView()
{
    BOOL okShow = !m_dockConsole->isVisible();
    m_dockConsole->setVisible(okShow);
    m_dockDebug->setVisible(okShow);
    m_dockDisasm->setVisible(okShow);
    m_dockMemory->setVisible(okShow);

    if (!okShow)
    {
        this->adjustSize();
    }

    UpdateMenu();
}
void MainWindow::debugDebugView()
{
    m_dockDebug->setVisible(!m_dockDebug->isVisible());
    UpdateMenu();
}
void MainWindow::debugDisasmView()
{
    m_dockDisasm->setVisible(!m_dockDisasm->isVisible());
    UpdateMenu();
}
void MainWindow::debugMemoryView()
{
    m_dockMemory->setVisible(!m_dockMemory->isVisible());
    UpdateMenu();
}

void MainWindow::debugStepInto()
{
    if (!g_okEmulatorRunning)
        m_console->execConsoleCommand(_T("s"));
}
void MainWindow::debugStepOver()
{
    if (!g_okEmulatorRunning)
        m_console->execConsoleCommand(_T("so"));
}
