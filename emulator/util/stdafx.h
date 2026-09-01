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
# define MAKEWORD(a, b)      ((uint16_t)(((uint8_t)(((uint32_t)(a)) & 0xff)) | ((uint16_t)((uint8_t)(((uint32_t)(b)) & 0xff))) << 8))
#endif  // MAKEWORD

#ifndef HANDLE
typedef void *HANDLE;
#endif  // HANDLE

#ifndef DECLARE_HANDLE
# define DECLARE_HANDLE(name) struct name##__ { int unused; }; typedef struct name##__ *name
#endif  // DECLARE_HANDLE

#ifndef INVALID_HANDLE_VALUE
# define INVALID_HANDLE_VALUE ((HANDLE)(int32_t)-1)
#endif  // INVALID_HANDLE_VALUE

//////////////////////////////////////////////////////////////////////
// DebugPrint

void DebugPrint(const char* message);
void DebugPrintFormat(const char* pszFormat, ...);
void DebugLog(const char* message);
void DebugLogFormat(const char* pszFormat, ...);

void PrintOctalValue(char* buffer, uint16_t value);
