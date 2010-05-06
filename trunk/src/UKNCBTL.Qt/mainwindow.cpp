#include "stdafx.h"
#include <QMessageBox>
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "qscreen.h"
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
    QObject::connect(actionEmulatorRun, SIGNAL(changed()), this, SLOT(emulatorRun()));
    //QMenu* menuDrives = ui->menuBar->addMenu(_T("Drives"));
    //QMenu* menuDebug = ui->menuBar->addMenu(_T("Debug"));
    QMenu* menuHelp = ui->menuBar->addMenu(_T("Help"));
    QAction* actionHelpAboutQt = menuHelp->addAction(_T("About Qt"));
    QObject::connect(actionHelpAboutQt, SIGNAL(triggered()), this, SLOT(helpAboutQt()));

    // Setup toolbar
    ui->mainToolBar->setIconSize(QSize(16, 16));
    QAction* actionRun = ui->mainToolBar->addAction(_T("Run"));
    actionRun->setCheckable(true);
    QObject::connect(actionRun, SIGNAL(changed()), this, SLOT(emulatorRun()));
    QAction* actionReset = ui->mainToolBar->addAction(_T("Reset"));
    QObject::connect(actionReset, SIGNAL(triggered()), this, SLOT(emulatorReset()));
    ui->mainToolBar->addSeparator();

    // Screen
    m_screen = new QScreen(ui->centralWidget);
}

MainWindow::~MainWindow()
{
    delete ui;
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
