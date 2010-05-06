#include "stdafx.h"
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
    QMenu* menuView = ui->menuBar->addMenu(_T("View"));
    QMenu* menuEmulator = ui->menuBar->addMenu(_T("Emulator"));
    QAction* actionEmulatorRun = menuEmulator->addAction(_T("Run"));
    actionEmulatorRun->setCheckable(true);
    QObject::connect(actionEmulatorRun, SIGNAL(changed()), this, SLOT(emulatorRun()));
    QMenu* menuDrives = ui->menuBar->addMenu(_T("Drives"));
    QMenu* menuDebug = ui->menuBar->addMenu(_T("Debug"));
    QMenu* menuHelp = ui->menuBar->addMenu(_T("Help"));

    // Setup toolbar
    ui->mainToolBar->setIconSize(QSize(16, 16));
    QAction* actionRun = ui->mainToolBar->addAction(_T("Run"));
    actionRun->setCheckable(true);
    QObject::connect(actionRun, SIGNAL(changed()), this, SLOT(emulatorRun()));
    QAction* actionReset = ui->mainToolBar->addAction(_T("Reset"));
    ui->mainToolBar->addSeparator();

    // Screen
    screen = new QScreen(ui->centralWidget);
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

void MainWindow::emulatorFrame()
{
    if (g_okEmulatorRunning)
    {
        Emulator_SystemFrame();
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
