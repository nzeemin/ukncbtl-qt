#include "stdafx.h"
#include <QtGui>
#include "qkeyboardview.h"
#include "Emulator.h"


// Keyboard key mapping to bitmap
const quint16 m_arrKeyboardKeys[] =
{
    /*   x1,y1    w,h    UKNCscan  */
    18, 15,  42, 27,    0010, // K1
    62, 15,  42, 27,    0011, // K2
    106, 15,  42, 27,    0012, // K3
    151, 15,  42, 27,    0014, // K4
    195, 15,  42, 27,    0015, // K5
    343, 15,  42, 27,    0172, // POM
    387, 15,  42, 27,    0152, // UST
    431, 15,  42, 27,    0151, // ISP
    506, 15,  42, 27,    0171, // SBROS (RESET)
    551, 15,  42, 27,    0004, // STOP

    18, 56,  28, 27,    0006, // AR2
    47, 56,  28, 27,    0007, // ; +
    77, 56,  27, 27,    0030, // 1
    106, 56,  28, 27,    0031, // 2
    136, 56,  27, 27,    0032, // 3
    165, 56,  28, 27,    0013, // 4
    195, 56,  27, 27,    0034, // 5
    224, 56,  28, 27,    0035, // 6
    254, 56,  27, 27,    0016, // 7
    283, 56,  28, 27,    0017, // 8
    313, 56,  27, 27,    0177, // 9
    342, 56,  28, 27,    0176, // 0
    372, 56,  27, 27,    0175, // - =
    401, 56,  28, 27,    0173, // / ?
    431, 56,  42, 27,    0132, // Backspace

    18, 86,  42, 27,    0026, // TAB
    62, 86,  27, 27,    0027, // É J
    91, 86,  28, 27,    0050, // Ö C
    121, 86,  27, 27,    0051, // Ó U
    150, 86,  28, 27,    0052, // Ê K
    180, 86,  27, 27,    0033, // Å E
    210, 86,  28, 27,    0054, // Í N
    239, 86,  27, 27,    0055, // Ã G
    269, 86,  27, 27,    0036, // Ø [
    298, 86,  28, 27,    0037, // Ù ]
    328, 86,  27, 27,    0157, // Ç Z
    357, 86,  28, 27,    0156, // Õ H
    387, 86,  27, 27,    0155, // Ú
    416, 86,  28, 27,    0174, // : *

    18, 115, 49, 27,    0046, // UPR
    69, 115, 28, 27,    0047, // Ô F
    99, 115, 27, 27,    0070, // Û Y
    128, 115, 28, 27,    0071, // Â W
    158, 115, 27, 27,    0072, // À A
    187, 115, 28, 27,    0053, // Ï P
    217, 115, 27, 27,    0074, // Ð R
    246, 115, 28, 27,    0075, // Î O
    276, 115, 27, 27,    0056, // Ë L
    305, 115, 28, 27,    0057, // Ä D
    335, 115, 27, 27,    0137, // Æ V
    364, 115, 28, 27,    0136, // Ý Backslash
    394, 115, 35, 27,    0135, // . >
    431, 115, 16, 27,    0153, // ENTER - left part
    446, 86,  27, 56,    0153, // ENTER - right part

    18, 145, 34, 27,    0106, // ALF
    55, 145, 27, 27,    0066, // GRAF
    84, 145, 27, 27,    0067, // ß Q
    114, 145, 27, 27,    0110, // × ^
    143, 145, 27, 27,    0111, // Ñ S
    173, 145, 27, 27,    0112, // Ì
    202, 145, 27, 27,    0073, // È I
    232, 145, 27, 27,    0114, // Ò
    261, 145, 27, 27,    0115, // Ü X
    291, 145, 27, 27,    0076, // Á B
    320, 145, 28, 27,    0077, // Þ @
    350, 145, 34, 27,    0117, // , <

    18, 174, 56, 27,    0105, // Left Shift
    77, 174, 34, 27,    0107, // FIKS
    114, 174, 211, 27,    0113, // Space bar
    328, 174, 56, 27,    0105, // Right Shift

    387, 145, 27, 56,    0116, // Left
    416, 145, 28, 27,    0154, // Up
    416, 174, 28, 27,    0134, // Down
    446, 145, 27, 56,    0133, // Right

    506, 56,  28, 27,    0131, // + NumPad
    536, 56,  27, 27,    0025, // - NumPad
    565, 56,  28, 27,    0005, // , NumPad
    506, 86,  28, 27,    0125, // 7 NumPad
    536, 86,  27, 27,    0145, // 8 NumPad
    565, 86,  28, 27,    0165, // 9 NumPad
    506, 115, 28, 27,    0130, // 4 NumPad
    536, 115, 27, 27,    0150, // 5 NumPad
    565, 115, 28, 27,    0170, // 6 NumPad
    506, 145, 28, 27,    0127, // 1 NumPad
    536, 145, 27, 27,    0147, // 2 NumPad
    565, 145, 28, 27,    0167, // 3 NumPad
    506, 174, 28, 27,    0126, // 0 NumPad
    536, 174, 27, 27,    0146, // . NumPad
    565, 174, 28, 27,    0166, // ENTER NumPad
};
const int m_nKeyboardKeysCount = sizeof(m_arrKeyboardKeys) / sizeof(unsigned short) / 5;

QKeyboardView::QKeyboardView(QWidget *parent) :
    QWidget(parent)
{
    setMinimumSize(UKNC_SCREEN_WIDTH, 200);
    setMaximumSize(UKNC_SCREEN_WIDTH + 60, 210 + 20);
}

void QKeyboardView::paintEvent(QPaintEvent *)
{
    QPainter painter(this);

    QImage imageKeyboard(":/images/keyboard.png");

    // Center image
    int cxBitmap = (int) imageKeyboard.width();
    int cyBitmap = (int) imageKeyboard.height();
    m_nImageLeft = (this->width() - cxBitmap) / 2;
    m_nImageTop = (this->height() - cyBitmap) / 2;

    painter.drawImage(QPoint(m_nImageLeft, m_nImageTop), imageKeyboard);

    //// Show key mappings
    //for (int i = 0; i < m_nKeyboardKeysCount; i++)
    //{
    //    QRect rcKey;
    //    rcKey.setLeft(m_nImageLeft + m_arrKeyboardKeys[i * 5]);
    //    rcKey.setTop(m_nImageTop + m_arrKeyboardKeys[i * 5 + 1]);
    //    rcKey.setRight(rcKey.left() + m_arrKeyboardKeys[i * 5 + 2]);
    //    rcKey.setBottom(rcKey.top() + m_arrKeyboardKeys[i * 5 + 3]);

    //    painter.drawRect(rcKey);
    //}
}

void QKeyboardView::mousePressEvent(QMouseEvent *event)
{
    quint8 keyscan = GetKeyByPoint(event->x(), event->y());
    if (keyscan == 0) return;

    Emulator_KeyEvent(keyscan, true);
}

void QKeyboardView::mouseReleaseEvent(QMouseEvent *event)
{
    unsigned char keyscan = GetKeyByPoint(event->x(), event->y());
    if (keyscan == 0) return;

    Emulator_KeyEvent(keyscan, false);
}

quint8 QKeyboardView::GetKeyByPoint(int x, int y)
{
    for (int i = 0; i < m_nKeyboardKeysCount; i++)
    {
        QRect rcKey;
        rcKey.setLeft(m_nImageLeft + m_arrKeyboardKeys[i * 5]);
        rcKey.setTop(m_nImageTop + m_arrKeyboardKeys[i * 5 + 1]);
        rcKey.setRight(rcKey.left() + m_arrKeyboardKeys[i * 5 + 2]);
        rcKey.setBottom(rcKey.top() + m_arrKeyboardKeys[i * 5 + 3]);

        if (x >= rcKey.left() && x < rcKey.right() && y >= rcKey.top() && y < rcKey.bottom())
        {
            return (quint8) m_arrKeyboardKeys[i * 5 + 4];
        }
    }

    return 0;
}
