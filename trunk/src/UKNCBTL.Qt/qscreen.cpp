#include "stdafx.h"
#include <QtGui>
#include "qscreen.h"
#include "Emulator.h"

QScreen::QScreen(QWidget *parent) :
    QWidget(parent)
{
    setMinimumSize(UKNC_SCREEN_WIDTH, UKNC_SCREEN_HEIGHT);
    m_image = new QImage(UKNC_SCREEN_WIDTH, UKNC_SCREEN_HEIGHT, QImage::Format_RGB32);
}

QScreen::~QScreen()
{
    delete m_image;
}

void QScreen::paintEvent(QPaintEvent *event)
{
    Emulator_PrepareScreenRGB32(m_image->bits());

    QPainter painter(this);
    painter.drawImage(0, 0, *m_image);
}
