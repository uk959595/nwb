#pragma once
#include <Windows.h>
#include <iostream>
#include "global.h"
 
class MyHook {
public:
    //single ton
    static MyHook& Instance() {
        static MyHook myHook;
        return myHook;
    }
    HHOOK mousehook; // handle to the hook	
    HHOOK keyboardhook;
    void InstallHook(); // function to install our hook
    void UninstallHook(); // function to uninstall our hook

    


};
LRESULT WINAPI MyMouseCallback(int nCode, WPARAM wParam, LPARAM lParam); //callback declaration
LRESULT WINAPI MyKeyBoardCallback(int nCode, WPARAM wParam, LPARAM lParam);
void startRecord();
void endRecord();
VOID importPath();
VOID exportPath();
void startPlay();
void endPlay();
