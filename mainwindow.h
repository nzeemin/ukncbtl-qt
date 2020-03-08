#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

class QEmulatorScreen;
class QKeyboardView;
class QConsoleView;
class QDebugView;
class QDisasmView;
class QMemoryView;
class QLabel;

namespace Ui
{
    class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

public:
    void UpdateMenu();
    void UpdateAllViews();
    void setCurrentProc(bool okProc);
    void restoreSettings();
    void showUptime(int uptimeMillisec);
    void showFps(double framesPerSecond);

public:
    void saveStateImage(const QString& filename);
    void loadStateImage(const QString& filename);
    void saveScreenshot(const QString& filename);
    bool attachFloppy(int slot, const QString& filename);
    void detachFloppy(int slot);
    bool attachCartridge(int slot, const QString& filename);
    void detachCartridge(int slot);
    bool attachHardDrive(int slot, const QString& filename);
    void detachHardDrive(int slot);

public slots:
    void saveStateImage();
    void loadStateImage();
    void saveScreenshot();
    void saveScreenshotAs();
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
    void viewKeyboard();
    void viewRgbScreen();
    void viewGrbScreen();
    void viewGrayscaleScreen();
    void viewSizeRegular();
    void viewSizeUpscaled();
    void viewSizeDoubleInterlaced();
    void viewSizeDouble();
    void viewSizeUpscaled3();
    void viewSizeUpscaled175();
    void viewSizeUpscaled5();
    void soundEnabled();
    void scriptRun();
    void consolePrint(const QString&);

protected:
    void changeEvent(QEvent *e);
    void closeEvent(QCloseEvent *);

private:
    Ui::MainWindow *ui;

    QEmulatorScreen *m_screen;
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
