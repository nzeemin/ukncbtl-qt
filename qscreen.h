#ifndef QEMULATORSCREEN_H
#define QEMULATORSCREEN_H

#include <QWidget>

enum ScreenViewMode
{
    RGBScreen = 1,
    GrayScreen = 2,
    GRBScreen = 3
};

enum ScreenSizeMode
{
    RegularScreen = 1,
    DoubleInterlacedScreen = 2,
    DoubleScreen = 3,
    UpscaledScreen = 4,
    UpscaledScreen3 = 5,
    UpscaledScreen4 = 6,
    UpscaledScreen175 = 7,
    UpscaledScreen5 = 8
};

class QEmulatorScreen : public QWidget
{
    Q_OBJECT
public:
    QEmulatorScreen(QWidget *parent = nullptr);
    ~QEmulatorScreen();

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

#endif // QEMULATORSCREEN_H
