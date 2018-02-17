// Common.h

#pragma once

#include <QtGlobal>

class QFont;
class QPainter;
class QString;


//////////////////////////////////////////////////////////////////////
// Defines for compilation under MinGW and GCC

#ifndef _TCHAR_DEFINED
typedef char TCHAR;
#define _tfopen     fopen
#define _tfsopen    _fsopen
#define _tcscpy     strcpy
#define _tstat      _stat
#define _tcsrchr    strrchr
#define _tcsicmp    _stricmp
#define _tcslen     strlen
#define _sntprintf  _snprintf
#define _T(x)       x
#endif

#ifdef __GNUC__
//#define _stat       stat
#define _stricmp    strcasecmp
#define _snprintf   snprintf
#endif

typedef const char * LPCTSTR;

#define MAKELONG(a, b)      ((qint16)(((quint16)(((quint32)(a)) & 0xffff)) | ((quint32)((quint16)(((quint32)(b)) & 0xffff))) << 16))
#define MAKEWORD(a, b)      ((quint16)(((quint8)(((quint32)(a)) & 0xff)) | ((quint16)((quint8)(((quint32)(b)) & 0xff))) << 8))
//#define LOWORD(l)           ((quint16)(((quint32)(l)) & 0xffff))
//#define HIWORD(l)           ((quint16)((((quint32)(l)) >> 16) & 0xffff))
//#define LOBYTE(w)           ((quint8)(((quint32)(w)) & 0xff))
//#define HIBYTE(w)           ((quint8)((((quint32)(w)) >> 8) & 0xff))

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


void AlertWarning(const QString &sMessage);
bool AlertOkCancel(const QString &sMessage);


//////////////////////////////////////////////////////////////////////
// DebugPrint

#if !defined(PRODUCT)

void DebugPrint(const char* message);
void DebugPrintFormat(const char* pszFormat, ...);
void DebugLog(const char* message);
void DebugLogFormat(const char* pszFormat, ...);

#endif // !defined(PRODUCT)


//////////////////////////////////////////////////////////////////////


// Processor register names
const LPCTSTR REGISTER_NAME[] = { "R0", "R1", "R2", "R3", "R4", "R5", "SP", "PC" };

const int UKNC_SCREEN_WIDTH = 640;
const int UKNC_SCREEN_HEIGHT = 288;

QFont Common_GetMonospacedFont();
void Common_Cleanup();
void PrintOctalValue(char* buffer, quint16 value);
void PrintBinaryValue(char* buffer, quint16 value);
void DrawOctalValue(QPainter &painter, int x, int y, quint16 value);
void DrawHexValue(QPainter &painter, int x, int y, quint16 value);
void DrawBinaryValue(QPainter &painter, int x, int y, quint16 value);
bool ParseOctalValue(const char* text, quint16* pValue);
bool ParseOctalValue(const QString &text, quint16* pValue);
ushort Translate_KOI8R(quint8 ch);


//////////////////////////////////////////////////////////////////////
