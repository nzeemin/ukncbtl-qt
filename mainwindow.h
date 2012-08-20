#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

class QScreen;
class QKeyboardView;
class QConsoleView;
class QDebugView;
class QDisasmView;
class QMemoryView;
class QLabel;

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
    void setCurrentProc(bool okProc);
    void restoreSettings();
    void showUptime(int uptimeMillisec);
    void showFps(double framesPerSecond);

public slots:
    void saveScreenshot();
    void saveScreenshot(const QString&);
    void helpAbout();
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
    void debugConsoleView();
    void debugDebugView();
    void debugDisasmView();
    void debugMemoryView();
    void debugStepInto();
    void debugStepOver();
    void viewRgbScreen();
    void viewGrayscaleScreen();
    void viewSizeRegular();
    void viewSizeUpscaled();
    void viewSizeDouble();
    void viewSizeUpscaled3();
    void viewSizeUpscaled4();
    void soundEnabled();
    void scriptRun();

protected:
    void changeEvent(QEvent *e);
    void closeEvent(QCloseEvent *);

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

    QLabel* m_statusLabelInfo;
    QLabel* m_statusLabelFrames;
    QLabel* m_statusLabelUptime;

    void emulatorCartridge(int slot);
    void emulatorHardDrive(int slot);
    void emulatorFloppy(int slot);
};

#endif // MAINWINDOW_H
