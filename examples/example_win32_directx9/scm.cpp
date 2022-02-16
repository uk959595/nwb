#pragma once
#include "global.h"
#include "scm.h"
 

//#include "VMProtectSDK.h"
//#define LnkName_static "aaa112346xxxxxxxxxx"
//#define SysFileName_static "DriverCore.sys"

//#define $MapName  "Global\\asd3dqd"   //Global\\SharedMem         jj332
CHAR $MapName[200];
HANDLE hfm;

BOOL cDrvCtrl::GetSvcHandle(PCHAR pServiceName)
{
    m_pServiceName = pServiceName;
    m_hSCManager = OpenSCManagerA(NULL, NULL, SC_MANAGER_ALL_ACCESS);
    if (NULL == m_hSCManager)
    {
        m_dwLastError = GetLastError();
        return FALSE;
    }
    m_hService = OpenServiceA(m_hSCManager, m_pServiceName, SERVICE_ALL_ACCESS);
    if (NULL == m_hService)
    {
        CloseServiceHandle(m_hSCManager);
        return FALSE;
    }
    else
    {
        return TRUE;
    }
}

BOOL cDrvCtrl::Install(PCHAR pSysPath, PCHAR pServiceName, PCHAR pDisplayName)
{
    /*DPrint("redy to install %s\n", pSysPath);
    DPrint("pServiceName %s\n", pServiceName);
    DPrint("pDisplayName %s\n", pDisplayName);*/
    m_pSysPath = pSysPath;
    m_pServiceName = pServiceName;
    m_pDisplayName = pDisplayName;
    m_hSCManager = OpenSCManagerA(NULL, NULL, SC_MANAGER_ALL_ACCESS);
    if (NULL == m_hSCManager)
    {
        m_dwLastError = GetLastError();
        return FALSE;
    }
    m_hService = CreateServiceA(m_hSCManager, m_pServiceName, m_pDisplayName,
        SERVICE_ALL_ACCESS, SERVICE_KERNEL_DRIVER, SERVICE_DEMAND_START, SERVICE_ERROR_NORMAL,
        m_pSysPath, NULL, NULL, NULL, NULL, NULL);
    if (NULL == m_hService)
    {
        m_dwLastError = GetLastError();
        if (ERROR_SERVICE_EXISTS == m_dwLastError)
        {
            m_hService = OpenServiceA(m_hSCManager, m_pServiceName, SERVICE_ALL_ACCESS);
            if (NULL == m_hService)
            {
                CloseServiceHandle(m_hSCManager);
                return FALSE;
            }
        }
        else
        {
            CloseServiceHandle(m_hSCManager);
            return FALSE;
        }
    }
    return TRUE;
}


BOOL cDrvCtrl::Start()
{
    if (!StartServiceA(m_hService, NULL, NULL))
    {
        if (GetLastError() == 1056) {
            //DPrint("1056 \n");
            return TRUE;
        }
        else {
            m_dwLastError = GetLastError();
            //DPrint("Start_noirp %d\n", GetLastError());
            return FALSE;
        }

    }

    return TRUE;
}
BOOL cDrvCtrl::Stop()
{
    SERVICE_STATUS ss;
    GetSvcHandle(m_pServiceName);
    if (!ControlService(m_hService, SERVICE_CONTROL_STOP, &ss))
    {
        m_dwLastError = GetLastError();
        return FALSE;
    }
    return TRUE;

}

BOOL cDrvCtrl::Remove()
{
    GetSvcHandle(m_pServiceName);
    if (!DeleteService(m_hService))
    {
        m_dwLastError = GetLastError();
        return FALSE;
    }
    return TRUE;
}




BOOL InitDriver() {
    /*VMProtectBeginUltra("InitDriver");

    int nSize = VMProtectGetCurrentHWID(NULL, 0);
    char *hwid = new char[nSize + 1];
    VMProtectGetCurrentHWID(hwid, nSize);
    hwid[nSize] = 0;
    char catchname[6];
    for (int k = 0; k < sizeof(catchname);k++) {
        catchname[k] = hwid[k];
    }
    catchname[5] = 0;
    delete(hwid);*/
    strcpy_s($MapName, sizeof($MapName), "Global\\");
    strcat_s($MapName, sizeof($MapName), catchname);
  

#ifdef install
    //安装并启动驱动
    //非irp控制下，只要不卸载 就没问题
    if (!dcptr->Install(szSysFile, szSvcLnkName, szSvcLnkName)) {
        //DPrint("Install %x\n", dcptr->m_dwLastError);

        return FALSE;
    }
    if (!dcptr->Start()) {
        //DPrint("Start %x\n", dcptr->m_dwLastError);

        return FALSE;
    }
    //OutputDebugString("!!!!!!!!!!!!client go!!!!!!!!!!!!!!!!\n");
#endif
    //hfm = CreateFileMapping(INVALID_HANDLE_VALUE, 0, PAGE_READWRITE, 0, 0x1000, $MapName);
    
    hfm = OpenFileMappingA(FILE_MAP_WRITE, FALSE, $MapName);
  
    if (hfm)
    {

        dcptr->secContext = (PSHM_CONTEXT)MapViewOfFile(hfm, FILE_MAP_ALL_ACCESS, 0, 0, 0x1000);
        // attempt to open coms with kernel

        if (dcptr->secContext)
        {

            dcptr->secContext->uInit = TRUE;

            // attempt to talk with kernel

            if (dcptr->secContext->kInit == SHM_MAGIC)
            {
                //DPrint("init dcptr %lx  m_hService %lx \n", dcptr, dcptr->m_hService);
                // success talking with kernel
                //防止以前的残留信息还存在
                RtlSecureZeroMemory(dcptr->secContext->dataReturn, 512);
                RtlSecureZeroMemory(dcptr->secContext->dataSubmit, 512);

                return TRUE;
            }
            else {
                
            }

        }
        else {
           
        }
    }
    else {

       
    }
    /*VMProtectEnd();*/

    return FALSE;
}




BOOL cDrvCtrl::kSendCmd(PSHM_CONTEXT c) {

    c->uReady = TRUE;
    c->cmdReady = CMD_READY;


    do {
        //DPrint("c->cmdReady %x \n", c->cmdReady);
        Sleep(0);
    } while (c->cmdReady != CMD_DONE);


    c->uReady = FALSE;
    return TRUE;
}

void cDrvCtrl::kCleanUp(PSHM_CONTEXT c)
{
    //clean up
    c->cmd = FALSE;
    c->cmdReady = FALSE;
    RtlSecureZeroMemory(c->dataReturn, 512);
    RtlSecureZeroMemory(c->dataSubmit, 512);
}
void cDrvCtrl::StopShareMemThread(PSHM_CONTEXT context) {
    //目前重启才会消除，只移除用户层的句柄
    //不停止也不卸载
    //context->cmd = CMD_EXIT;
    //context->cmdReady = CMD_READY;
    UnmapViewOfFile(dcptr->secContext);
    CloseHandle(hfm);

    //停止并卸载驱动
#ifdef install
    if (!dcptr->Stop()) {
        DPrint("init dcptr %lx  m_hService %lx \n", dcptr, dcptr->m_hService);
        DPrint("Stop err%x \n", dcptr->m_dwLastError);
        getchar();
    }

    if (!dcptr->Remove()) {
        DPrint("Remove err%x\n", dcptr->m_dwLastError);
        getchar();
    }
#endif
    //OutputDebugString("ByeBye!");
}

