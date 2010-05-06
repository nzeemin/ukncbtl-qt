#ifndef QSCREEN_H
#define QSCREEN_H

#include <QWidget>

class QScreen : public QWidget
{
    Q_OBJECT
public:
    QScreen(QWidget *parent);
    ~QScreen();

protected:
    void paintEvent(QPaintEvent *event);

private:
    QImage* m_image;
};

#endif // QSCREEN_H
