#ifndef QSCREEN_H
#define QSCREEN_H

#include <QWidget>

enum ScreenViewMode {
    RGBScreen = 1,
    GrayScreen = 2,
    GRBScreen = 3,
};

enum ScreenSizeMode {
    RegularScreen = 1,
    DoubleScreen = 2,
    UpscaledScreen = 2,
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
    void setSizeMode(ScreenSizeMode mode);
    ScreenSizeMode sizeMode() const { return m_sizeMode; }

protected:
    void createDisplay();
    void paintEvent(QPaintEvent *event);
    void keyPressEvent(QKeyEvent *event);
    void keyReleaseEvent(QKeyEvent *event);

private:
    QImage* m_image;
    ScreenViewMode m_mode;
    ScreenSizeMode m_sizeMode;

private:
    unsigned char TranslateQtKeyToUkncKey(int qtkey);
};

#endif // QSCREEN_H
