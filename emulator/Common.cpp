// Common.cpp

#include "stdafx.h"
#include <QClipboard>
#include <QMessageBox>
#include <QGuiApplication>
#include <QFont>
#include <QPainter>
#include <QCoreApplication>
#include "main.h"
#include "mainwindow.h"


//////////////////////////////////////////////////////////////////////


bool AssertFailedLine(const char * lpszFileName, int nLine)
{
    char buffer[360];
    _snprintf(buffer, 360,
            QT_TRANSLATE_NOOP("Common",
                    "ASSERTION FAILED\n\nFile: %s\nLine: %d\n\n"
                    "Press Abort to stop the program, Retry to break to the debugger, or Ignore to continue execution."),
            lpszFileName, nLine);
    int result = QMessageBox::question(nullptr, QT_TRANSLATE_NOOP("Common", "UKNC Back to Life"),
            buffer, QMessageBox::Abort, QMessageBox::Retry, QMessageBox::Ignore);
    switch (result)
    {
    case QMessageBox::Retry:
        return true;
    case QMessageBox::Ignore:
        return false;
    case QMessageBox::Abort:
        QCoreApplication::exit(255);
    }
    return false;
}

void AlertInfo(const QString &sMessage)
{
    QMessageBox::information(nullptr, QT_TRANSLATE_NOOP("Common", "UKNC Back to Life"),
            sMessage, QMessageBox::Ok);
}

void AlertWarning(const QString &sMessage)
{
    QMessageBox::warning(nullptr, QT_TRANSLATE_NOOP("Common", "UKNC Back to Life"),
            sMessage, QMessageBox::Ok);
}

bool AlertOkCancel(const QString &sMessage)
{
    int result = QMessageBox::question(nullptr, QT_TRANSLATE_NOOP("Common", "UKNC Back to Life"),
            sMessage, QMessageBox::Ok, QMessageBox::Cancel);
    return (result == QMessageBox::Ok);
}


//////////////////////////////////////////////////////////////////////
// DebugPrint and DebugLog

#if !defined(PRODUCT)

void DebugPrint(const char* message)
{
    MainWindow* mainWindow = Global_getMainWindow();
    if (mainWindow != nullptr)
        mainWindow->consolePrint(message);
}

void DebugPrintFormat(const char* pszFormat, ...)
{
    char buffer[512];

    va_list ptr;
    va_start(ptr, pszFormat);
    vsnprintf(buffer, 512, pszFormat, ptr);
    va_end(ptr);

    DebugPrint(buffer);
}

const char* TRACELOG_FILE_NAME = "trace.log";
const char* TRACELOG_NEWLINE = "\r\n";

FILE* Common_LogFile = nullptr;

void DebugLog(const char* message)
{
    if (Common_LogFile == nullptr)
    {
        Common_LogFile = ::fopen(TRACELOG_FILE_NAME, "a+b");
        //TODO: Check if Common_LogFile == nullptr
    }

    ::fseek(Common_LogFile, 0, SEEK_END);

    size_t dwLength = strlen(message) * sizeof(char);
    ::fwrite(message, 1, dwLength, Common_LogFile);
}

void DebugLogFormat(const char* pszFormat, ...)
{
    char buffer[512];

    va_list ptr;
    va_start(ptr, pszFormat);
    vsnprintf(buffer, 512, pszFormat, ptr);
    va_end(ptr);

    DebugLog(buffer);
}


#endif // !defined(PRODUCT)


//////////////////////////////////////////////////////////////////////


static QFont* g_MonospacedFont = nullptr;

QFont Common_GetMonospacedFont()
{
    if (g_MonospacedFont != nullptr)
        return *g_MonospacedFont;

#ifdef __APPLE__
    const char* fontName = "Monaco";
    int fontSize = 10;
#else
    const char* fontName = "Lucida Console";
    int fontSize = 9;
#endif
    g_MonospacedFont = new QFont(fontName, fontSize, QFont::Normal, false);
    g_MonospacedFont->setFixedPitch(true);
    g_MonospacedFont->setStyleHint(QFont::TypeWriter);
    g_MonospacedFont->setHintingPreference(QFont::PreferNoHinting);

    return *g_MonospacedFont;
}

void Common_Cleanup()
{
    if (g_MonospacedFont != nullptr)
        delete g_MonospacedFont;
}

QColor Common_GetColorShifted(const QPalette& palette, QRgb rgb)
{
    QColor color(qRed(rgb), qGreen(rgb), qBlue(rgb));
    // For dark theme, invert the color
    if (palette.windowText().color().lightness() > palette.window().color().lightness())
        return QColor::fromHsl(color.hslHue(), color.hslSaturation(), 255 - color.lightness(), color.alpha());
    else
        return color;
}


// Print octal 16-bit value to buffer
// buffer size at least 7 characters
void PrintOctalValue(char* buffer, quint16 value)
{
    for (int p = 0; p < 6; p++)
    {
        int digit = value & 7;
        buffer[5 - p] = '0' + digit;
        value = (value >> 3);
    }
    buffer[6] = 0;
}
// Print hex 16-bit value to buffer
// buffer size at least 5 characters
void PrintHexValue(char* buffer, quint16 value)
{
    for (int p = 0; p < 4; p++)
    {
        int digit = value & 15;
        buffer[3 - p] = (digit < 10) ? '0' + (char)digit : 'a' + (char)(digit - 10);
        value = (value >> 4);
    }
    buffer[4] = 0;
}
// Print binary 16-bit value to buffer
// buffer size at least 17 characters
void PrintBinaryValue(char * buffer, quint16 value)
{
    for (int b = 0; b < 16; b++)
    {
        int bit = (value >> b) & 1;
        buffer[15 - b] = bit ? '1' : '0';
    }
    buffer[16] = 0;
}

void DrawOctalValue(QPainter &painter, int x, int y, quint16 value)
{
    char buffer[7];
    PrintOctalValue(buffer, value);
    painter.drawText(x, y, buffer);
}
void DrawHexValue(QPainter &painter, int x, int y, quint16 value)
{
    char buffer[7];
    PrintHexValue(buffer, value);
    painter.drawText(x, y, buffer);
}
void DrawBinaryValue(QPainter &painter, int x, int y, quint16 value)
{
    char buffer[17];
    PrintBinaryValue(buffer, value);
    painter.drawText(x, y, buffer);
}

// Parse 16-bit octal value from text
bool ParseOctalValue(const char* text, quint16* pValue)
{
    quint16 value = 0;
    char* pChar = (char*) text;
    for (int p = 0; ; p++)
    {
        if (p > 6) return false;
        char ch = *pChar;  pChar++;
        if (ch == 0) break;
        if (ch < '0' || ch > '7') return false;
        value = (value << 3);
        int digit = ch - '0';
        value += digit;
    }
    *pValue = value;
    return true;
}

// Parse 16-bit octal value from text
bool ParseOctalValue(const QString &text, quint16* pValue)
{
    quint16 value = 0;
    for (int p = 0; p < text.length(); p++)
    {
        if (p > 6) return false;
        char ch = text.at(p).toLatin1();
        if (ch == 0) break;
        if (ch < '0' || ch > '7') return false;
        value = (value << 3);
        int digit = ch - '0';
        value += digit;
    }
    *pValue = value;
    return true;
}

// Parse 16-bit hex value from text
bool ParseHexValue(const char* text, quint16* pValue)
{
    quint16 value = 0;
    char* pChar = (char*) text;
    for (int p = 0; ; p++)
    {
        if (p > 4) return false;
        char ch = *pChar;  pChar++;
        if (ch == 0) break;
        if (ch >= '0' && ch <= '9')
        {
            value = (value << 4);
            int digit = ch - '0';
            value += digit;
        }
        else if (ch >= 'a' && ch <= 'f')
        {
            value = (value << 4);
            int digit = ch - 'a' + 10;
            value += digit;
        }
        else if (ch >= 'A' && ch <= 'F')
        {
            value = (value << 4);
            int digit = ch - 'A' + 10;
            value += digit;
        }
        else
            return false;
    }
    *pValue = value;
    return true;
}

void CopyTextToClipboard(const char* text)
{
    QClipboard *clipboard = QGuiApplication::clipboard();
    clipboard->clear();
    clipboard->setText(text);
}
void CopyWordOctalToClipboard(uint16_t value)
{
    char buffer[7];
    PrintOctalValue(buffer, value);
    CopyTextToClipboard(buffer);
}
void CopyWordHexToClipboard(uint16_t value)
{
    char bufferHex[5];
    PrintHexValue(bufferHex, value);
    CopyTextToClipboard(bufferHex);
}
void CopyWordBinaryToClipboard(uint16_t value)
{
    char bufferBin[17];
    PrintBinaryValue(bufferBin, value);
    CopyTextToClipboard(bufferBin);
}


// UKNC KOI8-R (Russian) to Unicode conversion table
const ushort KOI8R_CODES[] =
{
    0x2191, 0x2193, 0x2190, 0x2192, 0x25B2, 0x25BC, 0x25C4, 0x25BA, 0x2555, 0x255C, 0x255B, 0x2559, 0x2552, 0x2553, 0x2558, 0x2556,
    0x00D7, 0x00F7, 0x2265, 0x2264, 0x21B2, 0x00B1, 0x2248, 0x2320, 0x2321, 0x00B0, 0x2260, 0x2591, 0x2591, 0x2592, 0x2593, 0x2588,
    0x2562, 0x2534, 0x255F, 0x2510, 0x2551, 0x251C, 0x2514, 0x2560, 0x2554, 0x256A, 0x250C, 0x252C, 0x2569, 0x2568, 0x2524, 0x256C,
    0x253C, 0x255E, 0x256B, 0x2565, 0x2567, 0x2500, 0x255A, 0x2502, 0x2564, 0x2518, 0x2550, 0x2563, 0x255D, 0x2566, 0x2561, 0x2557,
    0x044e, 0x0430, 0x0431, 0x0446, 0x0434, 0x0435, 0x0444, 0x0433, 0x0445, 0x0438, 0x0439, 0x043a, 0x043b, 0x043c, 0x043d, 0x043e,
    0x043f, 0x044f, 0x0440, 0x0441, 0x0442, 0x0443, 0x0436, 0x0432, 0x044c, 0x044b, 0x0437, 0x0448, 0x044d, 0x0449, 0x0447, 0x044a,
    0x042e, 0x0410, 0x0411, 0x0426, 0x0414, 0x0415, 0x0424, 0x0413, 0x0425, 0x0418, 0x0419, 0x041a, 0x041b, 0x041c, 0x041d, 0x041e,
    0x041f, 0x042f, 0x0420, 0x0421, 0x0422, 0x0423, 0x0416, 0x0412, 0x042c, 0x042b, 0x0417, 0x0428, 0x042d, 0x0429, 0x0427, 0x042a
};
// Translate one KOI8-R character to Unicode character
ushort Translate_KOI8R(quint8 ch)
{
    if (ch < 128) return (char) ch;
    return KOI8R_CODES[ch - 128];
}


//////////////////////////////////////////////////////////////////////
