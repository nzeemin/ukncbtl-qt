#ifndef QEMULATORSCREEN_H
#define QEMULATORSCREEN_H

#include <QWidget>
#include "main.h"

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
    QImage getScreenshot();
    bool getScreenText(uint8_t* buffer);
    void setMode(ScreenViewMode mode);
    ScreenViewMode mode() const { return m_mode; }
    void setSizeMode(ScreenSizeMode mode);
    ScreenSizeMode sizeMode() const { return m_sizeMode; }

protected:
    void createDisplay();
    void paintEvent(QPaintEvent *event) override;
    void contextMenuEvent(QContextMenuEvent *event) override;
    void keyPressEvent(QKeyEvent *event) override;
    void keyReleaseEvent(QKeyEvent *event) override;

private:
    void* m_bits;
    QImage* m_image;
    ScreenViewMode m_mode;
    ScreenSizeMode m_sizeMode;

private:
    unsigned char TranslateQtKeyToUkncKey(int qtkey);
};

#endif // QEMULATORSCREEN_H
