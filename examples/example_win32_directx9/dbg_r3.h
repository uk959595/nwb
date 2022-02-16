#pragma once
#include <Windows.h>

#ifdef needlog
#define  DPrint(...) printf(__VA_ARGS__)
#define  Ot(...) OutputDebugStringA(__VA_ARGS__)
#else
#define DPrint(...);
#define  Ot(...);
#endif

//%c, %lc	ANSI�ַ�	char
//%C, %wc	���ַ�	wchar_t
//%d, %i	ʮ�����з�������	int
//%D	ʮ����__int64	__int64
//%L	ʮ�����Ƶ�LARGE_INTEGER	LARGE_INTEGER
//%s, %ls	NULL��ֹ��ANSI�ַ���	char*
//%S, %ws	NULL��ֹ�Ŀ��ַ���	wchar_t*
//%Z	ANSI_STRING�ַ���	ANSI_STRING
//%wZ	UNICODE_STRING�ַ���	UNICODE_STRING
//%u	ʮ���Ƶ�ULONG	ULONG
//%x	Сд�ַ�ʮ�����Ƶ�ULONG	ULONG
//%X	��д�ַ�ʮ�����Ƶ�ULONG	ULONG
//%p	ָ��Pointer 32 / 64λ
