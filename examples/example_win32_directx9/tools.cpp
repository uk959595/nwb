#include "tools.h"
#include <string>
#include "global.h"
#include "kmclass.h"
bool isContain(std::wstring str1, std::wstring str2) {

    if (str1.find(str2) != std::wstring::npos) {
        return true;
    }
    else {
        return false;
    }
}
//根据进程名获取PID
bool GetProcessIDByName(wchar_t* pname,int *pid)
{
    bool findit = false;
    

    HANDLE SnapShot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    SHFILEINFO shSmall;
    BOOL Status = FALSE;
    PROCESSENTRY32 ProcessInfo;//声明进程信息变量
 ;
    if (SnapShot == NULL)
    {
        return -1;
    }


    ProcessInfo.dwSize = sizeof(ProcessInfo);//设置ProcessInfo的大小
    //返回系统中第一个进程的信息
    Status = Process32First(SnapShot, &ProcessInfo);

    while (Status)
    {
      
        //获取进程文件信息
        SHGetFileInfo(ProcessInfo.szExeFile, 0, &shSmall,
            sizeof(shSmall), SHGFI_ICON | SHGFI_SMALLICON);
        //在列表控件中添加映像名称
        std::wstring a = ProcessInfo.szExeFile;
        std::wstring b = pname;
        if (isContain(a, b)==true)
        {
            *pid = ProcessInfo.th32ProcessID;
            findit = true;
            break;
        }
        //获取下一个进程的信息
        Status = Process32Next(SnapShot, &ProcessInfo);

    }
    return findit;
}


VOID ToTap2(CHAR i, int wt) {
    KeyDown_noirp(dcptr->secContext, MapVirtualKey(i, 0));
    Sleep(wt);
    KeyUp_noirp(dcptr->secContext, MapVirtualKey(i, 0));
}
 
