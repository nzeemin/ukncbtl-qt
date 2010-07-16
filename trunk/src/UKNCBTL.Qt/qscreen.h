#ifndef QSCREEN_H
#define QSCREEN_H

#include <QWidget>

enum ScreenViewMode {
    RGBScreen = 1,
    GrayScreen = 2,
    GRBScreen = 3,
};

class QScreen : public QWidget
{
    Q_OBJECT
public:
    QScreen(QWidget *parent = 0);
    ~QScreen();

public:
    void saveScreenshot(QString strFileName);
    void setMode(ScreenViewMode mode);
    ScreenViewMode mode() const { return m_mode; }

protected:
    void paintEvent(QPaintEvent *event);
    void keyPressEvent(QKeyEvent *event);
    void keyReleaseEvent(QKeyEvent *event);

private:
    QImage* m_image;
    ScreenViewMode m_mode;

private:
    unsigned char TranslateQtKeyToUkncKey(int qtkey);
};

#endif // QSCREEN_H
