// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

#include <cstdint>
#include <cassert>
#include <cstdlib>
#include <cstdio>
#include <cstring>

#ifdef _MSC_VER
#include <windows.h>
#endif

#ifndef CALLBACK
# define CALLBACK
#endif  // CALLBACK

#ifndef TCHAR
typedef char TCHAR;
#define _tfopen     fopen
#define _tcscpy     strcpy
#define _tstat      _stat
#define _tcsrchr    strrchr
#define _tcsicmp    strcasecmp
#define _tcscmp     strcmp
#define _tcslen     strlen
#define _sntprintf  _snprintf
#define _snprintf  snprintf
#endif  // TCHAR

#ifdef _MSC_VER
#define strncasecmp _strnicmp
#define strcasecmp _stricmp
#endif

#ifndef _T
# define _T(str) (str)
#endif  // _T

#ifndef LPTSTR
typedef TCHAR* LPTSTR;
#endif  // LCTSTR

#ifndef LPCTSTR
typedef const TCHAR* LPCTSTR;
#endif  // LPCTSTR

#ifndef ASSERT
# define ASSERT(expr) assert(expr)
#endif  // ASSERT

#ifndef MAKEWORD
#define MAKEWORD(a, b)      ((uint16_t)(((uint8_t)(((uint32_t)(a)) & 0xff)) | ((uint16_t)((uint8_t)(((uint32_t)(b)) & 0xff))) << 8))
#endif  // MAKEWORD

//////////////////////////////////////////////////////////////////////
// DebugPrint

void DebugPrint(const char* message);
void DebugPrintFormat(const char* pszFormat, ...);
void DebugLog(const char* message);
void DebugLogFormat(const char* pszFormat, ...);

void PrintOctalValue(char* buffer, uint16_t value);
