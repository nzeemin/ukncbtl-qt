#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "qscreen.h"

namespace Ui {
    class MainWindow;
}

class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    MainWindow(QWidget *parent = 0);
    ~MainWindow();

public slots:
    void emulatorFrame();
    void emulatorRun();

protected:
    void changeEvent(QEvent *e);

private:
    Ui::MainWindow *ui;
    QScreen *screen;
};

#endif // MAINWINDOW_H
