#pragma once
#include <Windows.h>

#ifdef needlog
#define  DPrint(...) printf(__VA_ARGS__)
#define  Ot(...) OutputDebugStringA(__VA_ARGS__)
#else
#define DPrint(...);
#define  Ot(...);
#endif

//%c, %lc	ANSI字符	char
//%C, %wc	宽字符	wchar_t
//%d, %i	十进制有符号整数	int
//%D	十进制__int64	__int64
//%L	十六进制的LARGE_INTEGER	LARGE_INTEGER
//%s, %ls	NULL终止的ANSI字符串	char*
//%S, %ws	NULL终止的宽字符串	wchar_t*
//%Z	ANSI_STRING字符串	ANSI_STRING
//%wZ	UNICODE_STRING字符串	UNICODE_STRING
//%u	十进制的ULONG	ULONG
//%x	小写字符十六进制的ULONG	ULONG
//%X	大写字符十六进制的ULONG	ULONG
//%p	指针Pointer 32 / 64位
