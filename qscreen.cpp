#include "stdafx.h"
#include <QtGui>
#include "qscreen.h"
#include "Emulator.h"


//////////////////////////////////////////////////////////////////////
// Colors

// Table for color conversion yrgb (4 bits) -> DWORD (32 bits)
const DWORD ScreenView_StandardRGBColors[16] = {
    0x000000, 0x000080, 0x008000, 0x008080, 0x800000, 0x800080, 0x808000, 0x808080,
    0x000000, 0x0000FF, 0x00FF00, 0x00FFFF, 0xFF0000, 0xFF00FF, 0xFFFF00, 0xFFFFFF,
};
const DWORD ScreenView_StandardGRBColors[16] = {
    0x000000, 0x000080, 0x800000, 0x800080, 0x008000, 0x008080, 0x808000, 0x808080,
    0x000000, 0x0000FF, 0xFF0000, 0xFF00FF, 0x00FF00, 0x00FFFF, 0xFFFF00, 0xFFFFFF,
};
// Table for color conversion, gray (black and white) display
const DWORD ScreenView_GrayColors[16] = {
    0x000000, 0x242424, 0x484848, 0x6C6C6C, 0x909090, 0xB4B4B4, 0xD8D8D8, 0xFFFFFF,
    0x000000, 0x242424, 0x484848, 0x6C6C6C, 0x909090, 0xB4B4B4, 0xD8D8D8, 0xFFFFFF,
};


//////////////////////////////////////////////////////////////////////


// Upscale screen from height 288 to 432
static void UpscaleScreen(void* pImageBits)
{
    int ukncline = 287;
    for (int line = 431; line > 0; line--)
    {
        DWORD* pdest = ((DWORD*)pImageBits) + line * UKNC_SCREEN_WIDTH;
        if (line % 3 == 1)
        {
            BYTE* psrc1 = ((BYTE*)pImageBits) + ukncline * UKNC_SCREEN_WIDTH * 4;
            BYTE* psrc2 = ((BYTE*)pImageBits) + (ukncline + 1) * UKNC_SCREEN_WIDTH * 4;
            BYTE* pdst1 = (BYTE*)pdest;
            for (int i = 0; i < UKNC_SCREEN_WIDTH * 4; i++)
            {
                if (i % 4 == 3)
                    *pdst1 = 0;
                else
                    *pdst1 = (BYTE)((((WORD)*psrc1) + ((WORD)*psrc2)) / 2);
                psrc1++;  psrc2++;  pdst1++;
            }
        }
        else
        {
            DWORD* psrc = ((DWORD*)pImageBits) + ukncline * UKNC_SCREEN_WIDTH;
            memcpy(pdest, psrc, UKNC_SCREEN_WIDTH * 4);
            ukncline--;
        }
    }
}


//////////////////////////////////////////////////////////////////////


QScreen::QScreen(QWidget *parent) :
    QWidget(parent), m_image(0)
{
    setFocusPolicy(Qt::StrongFocus);

    m_mode = RGBScreen;
    m_sizeMode = RegularScreen;

    createDisplay();
}

QScreen::~QScreen()
{
    delete m_image;
}

void QScreen::setMode(ScreenViewMode mode)
{
    m_mode = mode;
}

void QScreen::setSizeMode(ScreenSizeMode mode)
{
    if (m_sizeMode == mode) return;

    m_sizeMode = mode;

    createDisplay();
}

void QScreen::saveScreenshot(QString strFileName)
{
    m_image->save(strFileName, _T("PNG"), -1);
}

void QScreen::createDisplay()
{
    if (m_image != 0)
    {
        delete m_image;
        m_image = 0;
    }

    int cxScreenWidth = UKNC_SCREEN_WIDTH;
    int cyScreenHeight = UKNC_SCREEN_HEIGHT;
    if (m_sizeMode == UpscaledScreen)
        cyScreenHeight = 432;

    m_image = new QImage(cxScreenWidth, cyScreenHeight, QImage::Format_RGB32);

    setMinimumSize(cxScreenWidth, cyScreenHeight);
    setMaximumSize(cxScreenWidth + 100, cyScreenHeight + 20);
}

void QScreen::paintEvent(QPaintEvent * /*event*/)
{
    const DWORD* colors;
    switch (m_mode)
    {
        case RGBScreen:   colors = ScreenView_StandardRGBColors; break;
        case GrayScreen:  colors = ScreenView_GrayColors; break;
        case GRBScreen:   colors = ScreenView_StandardGRBColors; break;
        default:          colors = ScreenView_StandardRGBColors; break;
    }

    Emulator_PrepareScreenRGB32(m_image->bits(), colors);
    if (m_sizeMode == UpscaledScreen)
        UpscaleScreen(m_image->bits());

    QPainter painter(this);
    painter.drawImage(0, 0, *m_image);
}

void QScreen::keyPressEvent(QKeyEvent *event)
{
    if (! g_okEmulatorRunning) return;
    if (event->isAutoRepeat()) return;

    unsigned char ukncscan = TranslateQtKeyToUkncKey(event->key());
    if (ukncscan == 0) return;

    Emulator_KeyEvent(ukncscan, TRUE);
    event->accept();
}

void QScreen::keyReleaseEvent(QKeyEvent *event)
{
    unsigned char ukncscan = TranslateQtKeyToUkncKey(event->key());
    if (ukncscan == 0) return;

    Emulator_KeyEvent(ukncscan, FALSE);
    event->accept();
}

const unsigned char arrQtkey2UkncscanLat[256] = {  // ËÀÒ
/*       0     1     2     3     4     5     6     7     8     9     a     b     c     d     e     f  */
/*0*/    0000, 0000, 0000, 0000, 0000, 0000, 0000, 0000, 0000, 0000, 0000, 0000, 0000, 0000, 0000, 0000,
/*1*/    0000, 0000, 0000, 0000, 0000, 0000, 0000, 0000, 0000, 0000, 0000, 0000, 0000, 0000, 0000, 0000,
/*2*/    0113, 0004, 0151, 0172, 0000, 0116, 0154, 0133, 0134, 0000, 0000, 0000, 0000, 0000, 0000, 0000,
/*3*/    0176, 0030, 0031, 0032, 0013, 0034, 0035, 0016, 0017, 0177, 0000, 0000, 0000, 0000, 0000, 0000,
/*4*/    0000, 0072, 0076, 0050, 0057, 0033, 0047, 0055, 0156, 0073, 0027, 0052, 0056, 0112, 0054, 0075,
/*5*/    0053, 0067, 0074, 0111, 0114, 0051, 0137, 0071, 0115, 0070, 0157, 0000, 0000, 0000, 0000, 0000,
/*6*/    0000, 0000, 0000, 0000, 0000, 0000, 0000, 0000, 0000, 0000, 0000, 0000, 0000, 0000, 0000, 0000,
/*7*/    0000, 0000, 0000, 0000, 0000, 0000, 0000, 0000, 0000, 0000, 0000, 0000, 0000, 0000, 0000, 0000,
/*8*/    0000, 0000, 0000, 0000, 0000, 0000, 0000, 0000, 0000, 0000, 0000, 0000, 0000, 0000, 0000, 0000,
/*9*/    0000, 0000, 0000, 0000, 0000, 0000, 0000, 0000, 0000, 0000, 0000, 0000, 0000, 0000, 0000, 0000,
/*a*/    0000, 0000, 0000, 0000, 0000, 0000, 0000, 0000, 0000, 0000, 0000, 0000, 0000, 0000, 0000, 0000,
/*b*/    0000, 0000, 0000, 0000, 0000, 0000, 0000, 0000, 0000, 0000, 0000, 0000, 0000, 0000, 0000, 0000,
/*c*/    0000, 0000, 0000, 0000, 0000, 0000, 0000, 0000, 0000, 0000, 0000, 0000, 0000, 0000, 0000, 0000,
/*d*/    0000, 0000, 0000, 0000, 0000, 0000, 0000, 0000, 0000, 0000, 0000, 0000, 0000, 0000, 0000, 0000,
/*e*/    0000, 0000, 0000, 0000, 0000, 0000, 0000, 0000, 0000, 0000, 0000, 0000, 0000, 0000, 0000, 0000,
/*f*/    0000, 0000, 0000, 0000, 0000, 0000, 0000, 0000, 0000, 0000, 0000, 0000, 0000, 0000, 0000, 0000,
};
const unsigned char arrQtkey2UkncscanRus[256] = {  // ÐÓÑ
/*       0     1     2     3     4     5     6     7     8     9     a     b     c     d     e     f  */
/*0*/    0000, 0000, 0000, 0000, 0000, 0000, 0000, 0000, 0000, 0000, 0000, 0000, 0000, 0000, 0000, 0000,
/*1*/    0000, 0000, 0000, 0000, 0000, 0000, 0000, 0000, 0000, 0000, 0000, 0000, 0000, 0000, 0000, 0000,
/*2*/    0113, 0004, 0151, 0172, 0000, 0116, 0154, 0133, 0134, 0000, 0000, 0000, 0000, 0171, 0152, 0000,
/*3*/    0176, 0030, 0031, 0032, 0013, 0034, 0035, 0016, 0017, 0177, 0000, 0000, 0000, 0000, 0000, 0000,
/*4*/    0000, 0047, 0073, 0111, 0071, 0051, 0072, 0053, 0074, 0036, 0075, 0056, 0057, 0115, 0114, 0037,
/*5*/    0157, 0027, 0052, 0070, 0033, 0055, 0112, 0050, 0110, 0054, 0067, 0000, 0000, 0000, 0000, 0000,
/*6*/    0000, 0000, 0000, 0000, 0000, 0000, 0000, 0000, 0000, 0000, 0000, 0000, 0000, 0000, 0000, 0000,
/*7*/    0000, 0000, 0000, 0000, 0000, 0000, 0000, 0000, 0000, 0000, 0000, 0000, 0000, 0000, 0000, 0000,
/*8*/    0000, 0000, 0000, 0000, 0000, 0000, 0000, 0000, 0000, 0000, 0000, 0000, 0000, 0000, 0000, 0000,
/*9*/    0000, 0000, 0000, 0000, 0000, 0000, 0000, 0000, 0000, 0000, 0000, 0000, 0000, 0000, 0000, 0000,
/*a*/    0000, 0000, 0000, 0000, 0000, 0000, 0000, 0000, 0000, 0000, 0000, 0000, 0000, 0000, 0000, 0000,
/*b*/    0000, 0000, 0000, 0000, 0000, 0000, 0000, 0000, 0000, 0000, 0000, 0000, 0000, 0000, 0000, 0000,
/*c*/    0000, 0000, 0000, 0000, 0000, 0000, 0000, 0000, 0000, 0000, 0000, 0000, 0000, 0000, 0000, 0000,
/*d*/    0000, 0000, 0000, 0000, 0000, 0000, 0000, 0000, 0000, 0000, 0000, 0000, 0000, 0000, 0000, 0000,
/*e*/    0000, 0000, 0000, 0000, 0000, 0000, 0000, 0000, 0000, 0000, 0000, 0000, 0000, 0000, 0000, 0000,
/*f*/    0000, 0000, 0000, 0000, 0000, 0000, 0000, 0000, 0000, 0000, 0000, 0000, 0000, 0000, 0000, 0000,
};


unsigned char QScreen::TranslateQtKeyToUkncKey(int qtkey)
{
    switch (qtkey)
    {
    case Qt::Key_Down:      return 0134;
    case Qt::Key_Up:        return 0154;
    case Qt::Key_Left:      return 0116;
    case Qt::Key_Right:     return 0133;
    case Qt::Key_Enter:     return 0166;
    case Qt::Key_Return:    return 0166;
    case Qt::Key_Tab:       return 0026;
    case Qt::Key_Shift:     return 0105;  // HP key
    case Qt::Key_Space:     return 0113;
    case Qt::Key_Backspace: return 0132;  // ZB (BACKSPACE) key
    case Qt::Key_Control:   return 0046;  // SU (UPR) key
    case Qt::Key_F1:        return 0010;  // K1 / K6
    case Qt::Key_F2:        return 0011;  // K1 / K6
    case Qt::Key_F3:        return 0012;  // K1 / K6
    case Qt::Key_F4:        return 0014;  // K1 / K6
    case Qt::Key_F5:        return 0015;  // K1 / K6
    case Qt::Key_F7:        return 0152;  // UST key
    case Qt::Key_F8:        return 0151;  // ISP key
    }

    if (qtkey >= 32 && qtkey <= 255)
    {
        unsigned short ukncRegister = g_pBoard->GetKeyboardRegister();
        const unsigned char * pTable = ((ukncRegister & KEYB_LAT) != 0) ? arrQtkey2UkncscanLat : arrQtkey2UkncscanRus;
        return pTable[qtkey];
    }

    return 0;
}
