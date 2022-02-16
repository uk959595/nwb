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
//���ݽ�������ȡPID
bool GetProcessIDByName(wchar_t* pname,int *pid)
{
    bool findit = false;
    

    HANDLE SnapShot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    SHFILEINFO shSmall;
    BOOL Status = FALSE;
    PROCESSENTRY32 ProcessInfo;//����������Ϣ����
 ;
    if (SnapShot == NULL)
    {
        return -1;
    }


    ProcessInfo.dwSize = sizeof(ProcessInfo);//����ProcessInfo�Ĵ�С
    //����ϵͳ�е�һ�����̵���Ϣ
    Status = Process32First(SnapShot, &ProcessInfo);

    while (Status)
    {
      
        //��ȡ�����ļ���Ϣ
        SHGetFileInfo(ProcessInfo.szExeFile, 0, &shSmall,
            sizeof(shSmall), SHGFI_ICON | SHGFI_SMALLICON);
        //���б�ؼ������ӳ������
        std::wstring a = ProcessInfo.szExeFile;
        std::wstring b = pname;
        if (isContain(a, b)==true)
        {
            *pid = ProcessInfo.th32ProcessID;
            findit = true;
            break;
        }
        //��ȡ��һ�����̵���Ϣ
        Status = Process32Next(SnapShot, &ProcessInfo);

    }
    return findit;
}


VOID ToTap2(CHAR i, int wt) {
    KeyDown_noirp(dcptr->secContext, MapVirtualKey(i, 0));
    Sleep(wt);
    KeyUp_noirp(dcptr->secContext, MapVirtualKey(i, 0));
}
 
