#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

class QScreen;
class QKeyboardView;
class QConsoleView;
class QDebugView;
class QDisasmView;
class QMemoryView;

namespace Ui {
    class MainWindow;
}

class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    MainWindow(QWidget *parent = 0);
    ~MainWindow();

public:
    void UpdateMenu();
    void UpdateAllViews();

public slots:
    void fileScreenshot();
    void helpAboutQt();
    void emulatorFrame();
    void emulatorRun();
    void emulatorReset();
    void emulatorFloppy0();
    void emulatorFloppy1();
    void emulatorFloppy2();
    void emulatorFloppy3();
    void emulatorCartridge1();
    void emulatorCartridge2();
    void emulatorHardDrive1();
    void emulatorHardDrive2();

protected:
    void changeEvent(QEvent *e);

private:
    Ui::MainWindow *ui;

    QScreen *m_screen;
    QKeyboardView *m_keyboard;
    QConsoleView *m_console;
    QDockWidget* m_dockConsole;
    QDebugView *m_debug;
    QDockWidget* m_dockDebug;
    QDisasmView *m_disasm;
    QDockWidget* m_dockDisasm;
    QMemoryView * m_memory;
    QDockWidget* m_dockMemory;

    void emulatorCartridge(int slot);
    void emulatorHardDrive(int slot);
    void emulatorFloppy(int slot);
};

#endif // MAINWINDOW_H
