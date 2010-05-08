#include "stdafx.h"
#include <QMessageBox>
#include <QFileDialog>
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "qscreen.h"
#include "qkeyboardview.h"
#include "Emulator.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    this->setWindowTitle(_T("UKNC Back to Life"));

    // Setup menu
    QMenu* menuFile = ui->menuBar->addMenu(_T("File"));
    QAction* actionFileExit = menuFile->addAction(_T("Exit"));
    QObject::connect(actionFileExit, SIGNAL(triggered()), this, SLOT(close()));
    //QMenu* menuView = ui->menuBar->addMenu(_T("View"));
    QMenu* menuEmulator = ui->menuBar->addMenu(_T("Emulator"));
    QAction* actionEmulatorRun = menuEmulator->addAction(_T("Run"));
    actionEmulatorRun->setCheckable(true);
    menuEmulator->addAction(_T("Reset"), this, SLOT(emulatorReset()));
    QObject::connect(actionEmulatorRun, SIGNAL(changed()), this, SLOT(emulatorRun()));
    QMenu* menuDrives = ui->menuBar->addMenu(_T("Drives"));
    menuDrives->addSeparator();
    menuDrives->addAction(_T("Floppy MZ0:"), this, SLOT(emulatorFloppy0()));
    menuDrives->addAction(_T("Floppy MZ1:"), this, SLOT(emulatorFloppy1()));
    menuDrives->addAction(_T("Floppy MZ2:"), this, SLOT(emulatorFloppy2()));
    menuDrives->addAction(_T("Floppy MZ3:"), this, SLOT(emulatorFloppy3()));
    menuDrives->addSeparator();
    menuDrives->addAction(_T("Cartridge 1"), this, SLOT(emulatorCartridge1()));
    menuDrives->addAction(_T("Hard Drive 1"), this, SLOT(emulatorHardDrive1()));
    menuDrives->addAction(_T("Cartridge 2"), this, SLOT(emulatorCartridge2()));
    menuDrives->addAction(_T("Hard Drive 2"), this, SLOT(emulatorHardDrive2()));
    //QMenu* menuDebug = ui->menuBar->addMenu(_T("Debug"));
    QMenu* menuHelp = ui->menuBar->addMenu(_T("Help"));
    QAction* actionHelpAboutQt = menuHelp->addAction(_T("About Qt"));
    QObject::connect(actionHelpAboutQt, SIGNAL(triggered()), this, SLOT(helpAboutQt()));

    // Setup toolbar
    ui->mainToolBar->setIconSize(QSize(16, 16));
    QAction* actionRun = ui->mainToolBar->addAction(_T("Run"));
    actionRun->setCheckable(true);
    QObject::connect(actionRun, SIGNAL(changed()), this, SLOT(emulatorRun()));
    ui->mainToolBar->addAction(_T("Reset"), this, SLOT(emulatorReset()));
    ui->mainToolBar->addSeparator();
    ui->mainToolBar->addAction(_T("MZ0"), this, SLOT(emulatorFloppy0()));
    ui->mainToolBar->addAction(_T("MZ1"), this, SLOT(emulatorFloppy1()));
    ui->mainToolBar->addAction(_T("MZ2"), this, SLOT(emulatorFloppy2()));
    ui->mainToolBar->addAction(_T("MZ3"), this, SLOT(emulatorFloppy3()));
    ui->mainToolBar->addSeparator();
    ui->mainToolBar->addAction(_T("Cart1"), this, SLOT(emulatorCartridge1()));
    ui->mainToolBar->addAction(_T("Hard1"), this, SLOT(emulatorHardDrive1()));
    ui->mainToolBar->addAction(_T("Cart2"), this, SLOT(emulatorCartridge2()));
    ui->mainToolBar->addAction(_T("Hard2"), this, SLOT(emulatorHardDrive2()));

    // Screen
    m_screen = new QScreen(ui->centralWidget);
    m_screen->move(4, 4);
    m_keyboard = new QKeyboardView(ui->centralWidget);
    m_keyboard->move(4, UKNC_SCREEN_HEIGHT + 8);
}

MainWindow::~MainWindow()
{
    delete ui;
    delete m_screen;
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

void MainWindow::helpAboutQt()
{
    QMessageBox::aboutQt(this, _T("About Qt"));
}

void MainWindow::emulatorFrame()
{
    if (g_okEmulatorRunning)
    {
        if (Emulator_SystemFrame())
        {
            m_screen->repaint();
        }
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
        //Settings_SetCartridgeFilePath(slot, NULL);
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

        //Settings_SetCartridgeFilePath(slot, sFileName);
    }
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
        //Settings_SetFloppyFilePath(slot, NULL);
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

        //Settings_SetFloppyFilePath(slot, sFileName);
    }
}

void MainWindow::emulatorHardDrive1() { emulatorHardDrive(1); }
void MainWindow::emulatorHardDrive2() { emulatorHardDrive(2); }
void MainWindow::emulatorHardDrive(int slot)
{
    if (g_pBoard->IsHardImageAttached(slot))
    {
        g_pBoard->DetachHardImage(slot);
        //Settings_SetHardFilePath(slot, NULL);
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

        //Settings_SetHardFilePath(slot, bufFileName);
    }
}
