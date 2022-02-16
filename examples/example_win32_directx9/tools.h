#pragma once
#include <Windows.h>
#include <TlHelp32.h>
bool GetProcessIDByName(wchar_t* pname, int *pid);
void autoSay();
