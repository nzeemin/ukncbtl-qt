#include "stdafx.h"
#include <QtGui>
#include "qscreen.h"
#include "Emulator.h"


//////////////////////////////////////////////////////////////////////
// Colors

// Table for color conversion yrgb (4 bits) -> DWORD (32 bits)
const quint32 ScreenView_StandardRGBColors[16] =
{
    0x000000, 0x000080, 0x008000, 0x008080, 0x800000, 0x800080, 0x808000, 0x808080,
    0x000000, 0x0000FF, 0x00FF00, 0x00FFFF, 0xFF0000, 0xFF00FF, 0xFFFF00, 0xFFFFFF,
};
const quint32 ScreenView_StandardGRBColors[16] =
{
    0x000000, 0x000080, 0x800000, 0x800080, 0x008000, 0x008080, 0x808000, 0x808080,
    0x000000, 0x0000FF, 0xFF0000, 0xFF00FF, 0x00FF00, 0x00FFFF, 0xFFFF00, 0xFFFFFF,
};
// Table for color conversion, gray (black and white) display
const quint32 ScreenView_GrayColors[16] =
{
    0x000000, 0x242424, 0x484848, 0x6C6C6C, 0x909090, 0xB4B4B4, 0xD8D8D8, 0xFFFFFF,
    0x000000, 0x242424, 0x484848, 0x6C6C6C, 0x909090, 0xB4B4B4, 0xD8D8D8, 0xFFFFFF,
};


//////////////////////////////////////////////////////////////////////

#define AVERAGERGB(a, b)  ( (((a) & 0xfefefeffUL) + ((b) & 0xfefefeffUL)) >> 1 )

// Upscale screen from height 288 to 432
static void UpscaleScreen(void* pImageBits)
{
    quint32* pbits = static_cast<quint32*>(pImageBits);
    quint8* pbits8 = static_cast<quint8*>(pImageBits);
    int ukncline = 287;
    for (int line = 431; line > 0; line--)
    {
        quint32* pdest = pbits + line * UKNC_SCREEN_WIDTH;
        if (line % 3 == 1)
        {
            quint8* psrc1 = pbits8 + ukncline * UKNC_SCREEN_WIDTH * 4;
            quint8* psrc2 = pbits8 + (ukncline + 1) * UKNC_SCREEN_WIDTH * 4;
            quint8* pdst1 = reinterpret_cast<quint8*>(pdest);
            for (int i = 0; i < UKNC_SCREEN_WIDTH * 4; i++)
            {
                if (i % 4 == 3)
                    *pdst1 = 0;
                else
                    *pdst1 = (quint8)((((quint16) *psrc1) + ((quint16) *psrc2)) / 2);
                psrc1++;  psrc2++;  pdst1++;
            }
        }
        else
        {
            quint32* psrc = pbits + ukncline * UKNC_SCREEN_WIDTH;
            memcpy(pdest, psrc, UKNC_SCREEN_WIDTH * 4);
            ukncline--;
        }
    }
}

// Upscale screen from height 288 to 576 with "interlaced" effect
static void UpscaleScreen2(void* pImageBits)
{
    quint32* pbits = static_cast<quint32*>(pImageBits);
    for (int ukncline = 287; ukncline >= 0; ukncline--)
    {
        quint32* psrc = pbits + ukncline * UKNC_SCREEN_WIDTH;
        quint32* pdest = pbits + (ukncline * 2) * UKNC_SCREEN_WIDTH;
        memcpy(pdest, psrc, UKNC_SCREEN_WIDTH * 4);

        pdest += UKNC_SCREEN_WIDTH;
        memset(pdest, 0, UKNC_SCREEN_WIDTH * 4);
    }
}

// Upscale screen from height 288 to 576
static void UpscaleScreen2d(void* pImageBits)
{
    quint32* pbits = static_cast<quint32*>(pImageBits);
    for (int ukncline = 287; ukncline >= 0; ukncline--)
    {
        quint32* psrc = pbits + ukncline * UKNC_SCREEN_WIDTH;
        quint32* pdest = pbits + (ukncline * 2) * UKNC_SCREEN_WIDTH;
        memcpy(pdest, psrc, UKNC_SCREEN_WIDTH * 4);

        pdest += UKNC_SCREEN_WIDTH;
        memcpy(pdest, psrc, UKNC_SCREEN_WIDTH * 4);
    }
}

// Upscale screen width 640->960, height 288->576 with "interlaced" effect
static void UpscaleScreen3(void* pImageBits)
{
    quint32* pbits = static_cast<quint32*>(pImageBits);
    for (int ukncline = 287; ukncline >= 0; ukncline--)
    {
        quint32* psrc = pbits + ukncline * UKNC_SCREEN_WIDTH;
        psrc += UKNC_SCREEN_WIDTH - 1;
        quint32* pdest = pbits + (ukncline * 2) * 960;
        pdest += 960 - 1;
        for (int i = 0; i < UKNC_SCREEN_WIDTH / 2; i++)
        {
            quint32 c1 = *psrc;  psrc--;
            quint32 c2 = *psrc;  psrc--;
            quint32 c12 =
                (((c1 & 0xff) + (c2 & 0xff)) >> 1) |
                ((((c1 & 0xff00) + (c2 & 0xff00)) >> 1) & 0xff00) |
                ((((c1 & 0xff0000) + (c2 & 0xff0000)) >> 1) & 0xff0000);
            *pdest = c1;  pdest--;
            *pdest = c12; pdest--;
            *pdest = c2;  pdest--;
        }

        pdest += 960;
        memset(pdest, 0, 960 * 4);
    }
}

// Upscale screen width 640->960, height 288->720
static void UpscaleScreen4(void* pImageBits)
{
    quint32* pbits = static_cast<quint32*>(pImageBits);
    for (int ukncline = 0; ukncline < 288; ukncline += 2)
    {
        quint32* psrc1 = pbits + (286 - ukncline) * UKNC_SCREEN_WIDTH;
        quint32* psrc2 = psrc1 + UKNC_SCREEN_WIDTH;
        quint32* pdest0 = pbits + (286 - ukncline) / 2 * 5 * 960;
        quint32* pdest1 = pdest0 + 960;
        quint32* pdest2 = pdest1 + 960;
        quint32* pdest3 = pdest2 + 960;
        quint32* pdest4 = pdest3 + 960;
        for (int i = 0; i < UKNC_SCREEN_WIDTH / 2; i++)
        {
            quint32 c1a = *(psrc1++);  quint32 c1b = *(psrc1++);
            quint32 c2a = *(psrc2++);  quint32 c2b = *(psrc2++);
            quint32 c1 = AVERAGERGB(c1a, c1b);
            quint32 c2 = AVERAGERGB(c2a, c2b);
            quint32 ca = AVERAGERGB(c1a, c2a);
            quint32 cb = AVERAGERGB(c1b, c2b);
            quint32 c  = AVERAGERGB(ca,  cb);
            (*pdest0++) = c1a;  (*pdest0++) = c1;  (*pdest0++) = c1b;
            (*pdest1++) = c1a;  (*pdest1++) = c1;  (*pdest1++) = c1b;
            (*pdest2++) = ca;   (*pdest2++) = c;   (*pdest2++) = cb;
            (*pdest3++) = c2a;  (*pdest3++) = c2;  (*pdest3++) = c2b;
            (*pdest4++) = c2a;  (*pdest4++) = c2;  (*pdest4++) = c2b;
        }
    }
}

// Upscale screen width 640->1120 (x1.75), height 288->864 (x3) with "interlaced" effect
static void UpscaleScreen175(void* pImageBits)
{
    quint32* pbits = static_cast<quint32*>(pImageBits);
    for (int ukncline = 287; ukncline >= 0; ukncline--)
    {
        quint32* psrc = pbits + ukncline * UKNC_SCREEN_WIDTH;
        quint32* pdest1 = pbits + (ukncline * 3) * 1120;
        quint32* pdest2 = pdest1 + 1120;
        quint32* pdest3 = pdest2 + 1120;
        for (int i = 0; i < UKNC_SCREEN_WIDTH / 4; i++)
        {
            DWORD c1 = *(psrc++);
            DWORD c2 = *(psrc++);
            DWORD c3 = *(psrc++);
            DWORD c4 = *(psrc++);

            *(pdest1++) = *(pdest2++) = c1;
            *(pdest1++) = *(pdest2++) = AVERAGERGB(c1, c2);
            *(pdest1++) = *(pdest2++) = c2;
            *(pdest1++) = *(pdest2++) = AVERAGERGB(c2, c3);
            *(pdest1++) = *(pdest2++) = c3;
            *(pdest1++) = *(pdest2++) = AVERAGERGB(c3, c4);
            *(pdest1++) = *(pdest2++) = c4;
        }

        memset(pdest3, 0, 1120 * 4);
    }
}

// Upscale screen width 640->1280, height 288->864 with "interlaced" effect
static void UpscaleScreen5(void* pImageBits)
{
    quint32* pbits = static_cast<quint32*>(pImageBits);
    for (int ukncline = 287; ukncline >= 0; ukncline--)
    {
        quint32* psrc = pbits + ukncline * UKNC_SCREEN_WIDTH;
        quint32* pdest = pbits + (ukncline * 3) * 1280;
        psrc += UKNC_SCREEN_WIDTH - 1;
        pdest += 1280 - 1;
        quint32* pdest2 = pdest + 1280;
        quint32* pdest3 = pdest2 + 1280;
        for (int i = 0; i < UKNC_SCREEN_WIDTH; i++)
        {
            quint32 color = *psrc;  psrc--;
            *pdest = color;  pdest--;
            *pdest = color;  pdest--;
            *pdest2 = color;  pdest2--;
            *pdest2 = color;  pdest2--;
            *pdest3 = 0;  pdest3--;
            *pdest3 = 0;  pdest3--;
        }
    }
}


//////////////////////////////////////////////////////////////////////


QEmulatorScreen::QEmulatorScreen(QWidget *parent) :
    QWidget(parent), m_image(nullptr)
{
    setFocusPolicy(Qt::StrongFocus);

    m_image = nullptr;
    m_mode = RGBScreen;
    m_sizeMode = RegularScreen;

    createDisplay();
}

QEmulatorScreen::~QEmulatorScreen()
{
    delete m_image;
}

void QEmulatorScreen::setMode(ScreenViewMode mode)
{
    m_mode = mode;
}

void QEmulatorScreen::setSizeMode(ScreenSizeMode mode)
{
    if (m_sizeMode == mode) return;

    m_sizeMode = mode;

    createDisplay();
}

void QEmulatorScreen::saveScreenshot(QString strFileName)
{
    m_image->save(strFileName, "PNG", -1);
}

void QEmulatorScreen::createDisplay()
{
    if (m_image != nullptr)
    {
        delete m_image;
        m_image = nullptr;
    }

    int cxScreenWidth = UKNC_SCREEN_WIDTH;
    int cyScreenHeight = UKNC_SCREEN_HEIGHT;
    if (m_sizeMode == DoubleInterlacedScreen || m_sizeMode == DoubleScreen)
        cyScreenHeight = UKNC_SCREEN_HEIGHT * 2;
    else if (m_sizeMode == UpscaledScreen)
        cyScreenHeight = 432;
    else if (m_sizeMode == UpscaledScreen3)
    {
        cxScreenWidth = 960;
        cyScreenHeight = UKNC_SCREEN_HEIGHT * 2;
    }
    else if (m_sizeMode == UpscaledScreen4)
    {
        cxScreenWidth = 960;
        cyScreenHeight = 720;
    }
    else if (m_sizeMode == UpscaledScreen175)
    {
        cxScreenWidth = 1120;
        cyScreenHeight = UKNC_SCREEN_HEIGHT * 3;
    }
    else if (m_sizeMode == UpscaledScreen5)
    {
        cxScreenWidth = UKNC_SCREEN_WIDTH * 2;
        cyScreenHeight = UKNC_SCREEN_HEIGHT * 3;
    }

    m_image = new QImage(cxScreenWidth, cyScreenHeight, QImage::Format_RGB32);

    setMinimumSize(cxScreenWidth, cyScreenHeight);
    setMaximumSize(cxScreenWidth + 100, cyScreenHeight + 20);
}

void QEmulatorScreen::paintEvent(QPaintEvent * /*event*/)
{
    const quint32* colors;
    switch (m_mode)
    {
    case RGBScreen:
        colors = ScreenView_StandardRGBColors; break;
    case GrayScreen:
        colors = ScreenView_GrayColors; break;
    default:
        colors = ScreenView_StandardGRBColors; break;
    }

    Emulator_PrepareScreenRGB32(m_image->bits(), colors);
    if (m_sizeMode == DoubleInterlacedScreen)
        UpscaleScreen2(m_image->bits());
    else if (m_sizeMode == DoubleScreen)
        UpscaleScreen2d(m_image->bits());
    else if (m_sizeMode == UpscaledScreen)
        UpscaleScreen(m_image->bits());
    else if (m_sizeMode == UpscaledScreen3)
        UpscaleScreen3(m_image->bits());
    else if (m_sizeMode == UpscaledScreen4)
        UpscaleScreen4(m_image->bits());
    else if (m_sizeMode == UpscaledScreen175)
        UpscaleScreen175(m_image->bits());
    else if (m_sizeMode == UpscaledScreen5)
        UpscaleScreen5(m_image->bits());

    QPainter painter(this);
    painter.drawImage(0, 0, *m_image);
}

void QEmulatorScreen::keyPressEvent(QKeyEvent *event)
{
    if (! g_okEmulatorRunning) return;
    if (event->isAutoRepeat()) return;

    unsigned char ukncscan = TranslateQtKeyToUkncKey(event->key());
    if (ukncscan == 0) return;

    Emulator_KeyEvent(ukncscan, true);
    event->accept();
}

void QEmulatorScreen::keyReleaseEvent(QKeyEvent *event)
{
    unsigned char ukncscan = TranslateQtKeyToUkncKey(event->key());
    if (ukncscan == 0) return;

    Emulator_KeyEvent(ukncscan, false);
    event->accept();
}

const unsigned char arrQtkey2UkncscanLat[256] =    // ËÀÒ
{
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
const unsigned char arrQtkey2UkncscanRus[256] =    // ÐÓÑ
{
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


unsigned char QEmulatorScreen::TranslateQtKeyToUkncKey(int qtkey)
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
