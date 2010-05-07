#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

class QScreen;
class QKeyboardView;

namespace Ui {
    class MainWindow;
}

class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    MainWindow(QWidget *parent = 0);
    ~MainWindow();

public slots:
    void helpAboutQt();
    void emulatorFrame();
    void emulatorRun();
    void emulatorReset();
    void emulatorCartridge1();
    void emulatorCartridge2();

protected:
    void changeEvent(QEvent *e);

private:
    Ui::MainWindow *ui;
    QScreen *m_screen;
    QKeyboardView *m_keyboard;

    void emulatorCartridge(int slot);
};

#endif // MAINWINDOW_H
