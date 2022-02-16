#include "record.h"
#include <string>
#include "utils.h"
#include "global.h"
#include <vector>
#include <fstream>  
#include <iostream>
#include "kmclass.h"



DWORD dwStart;
char pathRecordFile[1000] = "C:\\c.txt";
 bool Recordflag = false;
 bool Playflag = false;
 int check_catchX = 0;
 int check_catchY = 0;
 bool loopPlayflag =false;
std::vector <actnode>* pRecordPathBox = NULL;

std::vector<std::string> split(const std::string& str, const  std::string& pattern)
{
    std::vector<std::string> ret;
    if (pattern.empty()) return ret;
    size_t start = 0, index = str.find_first_of(pattern, 0);
    while (index != str.npos)
    {
        if (start != index)
            ret.push_back(str.substr(start, index - start));
        start = index + 1;
        index = str.find_first_of(pattern, start);
    }
    if (!str.substr(start).empty())
        ret.push_back(str.substr(start));
    return ret;
}


VOID importPath() {

    if (strnlen_s(pathRecordFile, sizeof(pathRecordFile)) < 5) {
        return;
    }
    pRecordPathBox->clear();
    std::vector<actnode>().swap(*pRecordPathBox);
    std::ifstream in(pathRecordFile);
    std::string line;
    std::string all = "";

    if (in) // 有该文件  
    {
        while (std::getline(in, line)) // line中不包括每行的换行符  
        {
            std::vector<std::string> s = split(line, std::string(","));
            actnode k = {
                atoi(s[0].c_str()),
                atoi(s[1].c_str()),
                atoi(s[2].c_str()),
                atoi(s[3].c_str()),
                atoi(s[4].c_str()) 
            };
            pRecordPathBox->push_back(k);

        }
    }
}
 
VOID exportPath() {
    if (strnlen_s(pathRecordFile, sizeof(pathRecordFile)) < 5) {
        return;
    }
    std::string outfilestr = "";
    for (UINT k = 0; k < pRecordPathBox->size(); k++) {

        char ss[100];
        sprintf_s(ss, "%d,%d,%d,%d,%d\n",
            (*pRecordPathBox)[k].sleeptime, (*pRecordPathBox)[k].nameid,
            (*pRecordPathBox)[k].code, (*pRecordPathBox)[k].x,
            (*pRecordPathBox)[k].y
        );
        outfilestr += std::string(ss);
    }
    std::ofstream outfile(pathRecordFile);
    outfile << (char*)outfilestr.c_str();
}


void startRecord() {
 
    pRecordPathBox->clear();
    std::vector<actnode>().swap(*pRecordPathBox);

    dwStart = GetTickCount(); //取windows启动到现在的流逝时间(毫秒)
    MyHook::Instance().InstallHook();
}
void endRecord() {
    MyHook::Instance().UninstallHook();
}
static bool actisrun = false;
DWORD  actStep() {
    do {
        for (DWORD64 k = 0; k < pRecordPathBox->size(); k++) {
            if (actisrun == false) {
                return 1;
            }
            // info(vformat(u8"休息 %d ms \n", (*pRecordPathBox)[k].sleeptime));
            if ((*pRecordPathBox)[k].code == 45 || (*pRecordPathBox)[k].code == 36) {
                continue;
            }
            Sleep((*pRecordPathBox)[k].sleeptime);
            //"kdown"1  "kup"2   "mipdown"3 "mmove"4
            if ((*pRecordPathBox)[k].nameid == 1) {
                KeyDown_noirp(dcptr->secContext, MapVirtualKey((*pRecordPathBox)[k].code, 0));
                //  info(vformat(u8"按下 %c  \n", (*pRecordPathBox)[k].code));
            }
            else if ((*pRecordPathBox)[k].nameid == 2) {
                KeyUp_noirp(dcptr->secContext, MapVirtualKey((*pRecordPathBox)[k].code, 0));
                //  info(vformat(u8"弹起 %c  \n", (*pRecordPathBox)[k].code));
            }
            else if ((*pRecordPathBox)[k].nameid == 3) {
                Mouse_noirp(dcptr->secContext, (*pRecordPathBox)[k].code);
                //  info(vformat(u8"鼠标键 %llx  \n", (*pRecordPathBox)[k].code));
            }
            else if ((*pRecordPathBox)[k].nameid == 4) {
                DWORD x = MulDiv((*pRecordPathBox)[k].x, 65535, GetSystemMetrics(SM_CXFULLSCREEN));
                DWORD y = MulDiv((*pRecordPathBox)[k].y, 65535, GetSystemMetrics(SM_CYFULLSCREEN));
                x = x - round(x * mouseCheckOftX);
                y = y - round(y * mouseCheckOftY);
                MouseMove_noirp(dcptr->secContext, MOUSEEVENTF_ABSOLUTE | MOUSEEVENTF_MOVE, x, y);
                //info(vformat(u8"鼠标移动 %d %d  \n", (*pRecordPathBox)[k].x,(*pRecordPathBox)[k].y));
            }
        }
    } while (loopPlayflag);
 

    Playflag = false;
    actisrun = false;
    Beep(566, 266);
    Beep(566, 266);
    return 1;
}
void startPlay() {
    actisrun = true;
    CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)actStep, NULL, 0, NULL);
}
void endPlay() {
    actisrun = false;
}
void MyHook::InstallHook() {
 
    
    if (!(mousehook = SetWindowsHookEx(WH_MOUSE_LL, MyMouseCallback, NULL, 0))) {
 
        info(vformat(u8"鼠标记录错误 \n" ));
    }
    else {
        info(vformat(u8"鼠标记录成功 \n" ));
    }
 
    if (!(keyboardhook = SetWindowsHookEx(WH_KEYBOARD_LL, MyKeyBoardCallback, NULL, 0)))
    {
        info(vformat(u8"键盘记录错误: %d \n", GetLastError()));
    }
    else {
        info(vformat(u8"键盘记录成功: %d \n", GetLastError()));
    }
}




void MyHook::UninstallHook() {
    /*
    uninstall our hook using the hook handle
    */
    if (0!=UnhookWindowsHookEx(mousehook)) {
        info(vformat(u8"鼠标记录停止【成功】\n"));
    }
    else {
        info(vformat(u8"鼠标记录停止【失败】%d\n", GetLastError()));
    } 
    
    if (0 != UnhookWindowsHookEx(keyboardhook)) {
        info(vformat(u8"键盘记录停止【成功】\n"));
    }
    else {
        info(vformat(u8"键盘记录停止【失败】%d\n", GetLastError()));
    }
}

LRESULT WINAPI MyMouseCallback(int nCode, WPARAM wParam, LPARAM lParam) {
    MSLLHOOKSTRUCT* pMouseStruct = (MSLLHOOKSTRUCT*)lParam; // WH_MOUSE_LL struct
 
    actnode k;
    if (nCode == 0) { // we have information in wParam/lParam ? If yes, let's check it:
        
        bool needpush = false;
        switch (wParam) {

            case WM_LBUTTONUP: {
               // printf_s("LEFT CLICK UP\n");
                needpush = true;
                k.nameid = 3;
                k.code = MOUSE_LEFT_BUTTON_UP;
            }break;
            case WM_LBUTTONDOWN: {
                //printf_s("LEFT CLICK DOWN\n");
                needpush = true;
                k.nameid = 3;
                k.code = MOUSE_LEFT_BUTTON_DOWN;
            }break;
            case WM_RBUTTONUP: {
               // printf_s("RIGHT CLICK UP\n");
                needpush = true;
                k.nameid = 3;
                k.code = MOUSE_RIGHT_BUTTON_UP;
            }break;
            case WM_RBUTTONDOWN: {
                //printf_s("RIGHT CLICK DOWN\n");
                needpush = true;
                k.nameid = 3;
                k.code = MOUSE_RIGHT_BUTTON_DOWN;
            }break;
            case WM_MBUTTONDOWN: {
                //printf_s("WM_M BUTTON DOWN\n");
                needpush = true;
                k.nameid = 3;
                k.code = MOUSE_MIDDLE_BUTTON_DOWN;
            }break;
            case WM_MBUTTONUP: {
                //printf_s("WM_M BUTTON UP\n");
                needpush = true;
                k.nameid = 3;
                k.code = MOUSE_MIDDLE_BUTTON_UP;
            }break;


            case WM_MOUSEMOVE: {
                //printf_s("WM_MOUSEMOVE\n");
                needpush = true;
                k.nameid = 4;
                POINT check_mouse;
                GetCursorPos(&check_mouse);
                //printf_s("catch: x = %i | y = %i \n", check_mouse.x, check_mouse.y);
                k.code = MOUSEEVENTF_ABSOLUTE | MOUSEEVENTF_MOVE;
                k.x = check_mouse.x;
                k.y = check_mouse.y;
            }break;
        }
        if (needpush) { // Mouse struct contain information?			
         
            DWORD dwUsed = GetTickCount() - dwStart; //计算该函数所消耗的时间
            dwStart = GetTickCount();
            k.sleeptime = dwUsed;
            pRecordPathBox->push_back(k);

        }

    }
 
    return CallNextHookEx(MyHook::Instance().mousehook, nCode, wParam, lParam);
}

LRESULT WINAPI MyKeyBoardCallback(int nCode, WPARAM wParam, LPARAM lParam)
{
    KBDLLHOOKSTRUCT* pKeyStruct = (KBDLLHOOKSTRUCT*)lParam;
 
    if (nCode == 0)
    {
        actnode k;
        if (pKeyStruct)
        {

            k.code = pKeyStruct->vkCode;
            k.x = 0;
            k.y = 0;
        }
        //"kdown"1  "kup"2  "mupdown"3 "mmove"4  
        switch (wParam)
        {
        case WM_KEYDOWN: {
           // info(vformat("Sys Key down\n"));
            k.nameid = 1;
            DWORD dwUsed = GetTickCount() - dwStart; //计算该函数所消耗的时间
            dwStart = GetTickCount();
           // info(vformat("time %d\n", dwUsed));
            //info(vformat("Virtual Key Code: %d %d\n", pKeyStruct->vkCode, 'O'));
            k.sleeptime = dwUsed;
            pRecordPathBox->push_back(k);
        }break;
        case WM_KEYUP: {
          //  info(vformat("Sys Key up\n"));
            k.nameid = 2;
            DWORD dwUsed = GetTickCount() - dwStart; //计算该函数所消耗的时间
            dwStart = GetTickCount();
            //info(vformat("time %d\n", dwUsed));
           // info(vformat("Virtual Key Code: %d %d\n", pKeyStruct->vkCode, 'O'));
            k.sleeptime = dwUsed;
            pRecordPathBox->push_back(k);
        }break;
        case WM_SYSKEYDOWN: {
            info(vformat("Not Sys Key\n")
            );

        }break;
        }
         

    }
    return CallNextHookEx(MyHook::Instance().keyboardhook, nCode, wParam, lParam);
}
