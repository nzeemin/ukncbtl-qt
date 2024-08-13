// Common.h

#pragma once

#include <QtGlobal>
#include <QColor>

class QFont;
class QPainter;
class QPalette;
class QString;


//////////////////////////////////////////////////////////////////////
// Defines for compilation under MinGW and GCC

#ifndef _TCHAR_DEFINED
typedef char TCHAR;
#define _tfopen     fopen
#define _tcscpy     strcpy
#define _tstat      _stat
#define _tcsrchr    strrchr
#define _tcsicmp    _stricmp
#define _tcscmp     strcmp
#define _tcslen     strlen
#define _sntprintf  _snprintf
#define _T(x)       x
#endif

#ifdef __GNUC__
//#define _stat       stat
#define _stricmp    strcasecmp
#define _snprintf   snprintf
#endif

typedef char * LPTSTR;
typedef const char * LPCTSTR;

#define MAKELONG(a, b)      ((qint16)(((quint16)(((quint32)(a)) & 0xffff)) | ((quint32)((quint16)(((quint32)(b)) & 0xffff))) << 16))
#define MAKEWORD(a, b)      ((quint16)(((quint8)(((quint32)(a)) & 0xff)) | ((quint16)((quint8)(((quint32)(b)) & 0xff))) << 8))

#ifdef __GNUC__
#define CALLBACK
#else
#define CALLBACK __stdcall
#endif

typedef void *HANDLE;
#define INVALID_HANDLE_VALUE ((HANDLE)(qint32)-1)
#define DECLARE_HANDLE(name) struct name##__ { int unused; }; typedef struct name##__ *name


//////////////////////////////////////////////////////////////////////
// Assertions checking - MFC-like ASSERT macro

#ifdef _DEBUG

bool AssertFailedLine(const char * lpszFileName, int nLine);
#define ASSERT(f)          (void) ((f) || !AssertFailedLine(__FILE__, __LINE__) || (__debugbreak(), 0))
#define VERIFY(f)          ASSERT(f)

#else   // _DEBUG

#define ASSERT(f)          ((void)0)
#define VERIFY(f)          ((void)f)

#endif // !_DEBUG


//////////////////////////////////////////////////////////////////////


void AlertInfo(const QString &sMessage);
void AlertWarning(const QString &sMessage);
bool AlertOkCancel(const QString &sMessage);


//////////////////////////////////////////////////////////////////////
// DebugPrint

void DebugPrint(const char* message);
void DebugPrintFormat(const char* pszFormat, ...);
void DebugLog(const char* message);
void DebugLogFormat(const char* pszFormat, ...);


//////////////////////////////////////////////////////////////////////


// Processor register names
const LPCTSTR REGISTER_NAME[] = { "R0", "R1", "R2", "R3", "R4", "R5", "SP", "PC" };

const int UKNC_SCREEN_WIDTH = 640;
const int UKNC_SCREEN_HEIGHT = 288;

#define COLOR_VALUECHANGED  qRgb(128,   0,   0)
#define COLOR_PREVIOUS      qRgb(  0,   0, 212)
#define COLOR_MEMORYROM     qRgb(  0,   0, 128)
#define COLOR_MEMORYIO      qRgb(  0, 128,   0)
#define COLOR_MEMORYNA      qRgb(128, 128, 128)
#define COLOR_SUBTITLE      qRgb(  0,  64,   0)
#define COLOR_VALUE         qRgb(128, 128, 128)
#define COLOR_VALUEROM      qRgb(128, 128, 192)
#define COLOR_JUMP          qRgb( 80, 192, 224)
#define COLOR_JUMPYES       qRgb( 80, 240,  80)
#define COLOR_JUMPNO        qRgb(180, 180, 180)
#define COLOR_JUMPHINT      qRgb( 40, 128, 160)
#define COLOR_HINT          qRgb( 40,  40, 160)
#define COLOR_BREAKPOINT    qRgb(228,  64,  64)

QFont Common_GetMonospacedFont();
QColor Common_GetColorShifted(const QPalette& palette, QRgb rgb);
void Common_Cleanup();
void PrintOctalValue(char* buffer, quint16 value);
void PrintHexValue(char* buffer, quint16 value);
void PrintBinaryValue(char* buffer, quint16 value);
void DrawOctalValue(QPainter &painter, int x, int y, quint16 value);
void DrawHexValue(QPainter &painter, int x, int y, quint16 value);
void DrawBinaryValue(QPainter &painter, int x, int y, quint16 value);
bool ParseOctalValue(const char* text, quint16* pValue);
bool ParseOctalValue(const QString &text, quint16* pValue);
bool ParseHexValue(const char* text, quint16* pValue);

void CopyTextToClipboard(const char* text);
void CopyWordOctalToClipboard(uint16_t value);
void CopyWordHexToClipboard(uint16_t value);
void CopyWordBinaryToClipboard(uint16_t value);

ushort Translate_KOI8R(quint8 ch);


//////////////////////////////////////////////////////////////////////
