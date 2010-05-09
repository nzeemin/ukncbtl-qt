#ifndef QSCREEN_H
#define QSCREEN_H

#include <QWidget>

class QScreen : public QWidget
{
    Q_OBJECT
public:
    QScreen(QWidget *parent = 0);
    ~QScreen();

protected:
    void paintEvent(QPaintEvent *event);
    void keyPressEvent(QKeyEvent *event);
    void keyReleaseEvent(QKeyEvent *event);

private:
    QImage* m_image;

private:
    unsigned char TranslateQtKeyToUkncKey(int qtkey);
};

#endif // QSCREEN_H
