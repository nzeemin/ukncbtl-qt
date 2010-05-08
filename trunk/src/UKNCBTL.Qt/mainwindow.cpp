#include "stdafx.h"
#include <QMessageBox>
#include <QFileDialog>
#include <QAction>
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

    // Assign signals
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
    QObject::connect(ui->actionHelpAboutQt, SIGNAL(triggered()), this, SLOT(helpAboutQt()));

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

void MainWindow::UpdateMenu()
{
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

    UpdateMenu();
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

    UpdateMenu();
}
