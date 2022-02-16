/*============================
Drvier Control Class (SCM way)
============================*/
#pragma once
#include <stdio.h>
#include <Windows.h>
#include <string.h>

#pragma comment(lib,"advapi32.lib")




BOOL InitDriver();


#define CMD_LogIn 0x111
#define CMD_EXIT 0x333
#define CMD_Protect 0x606

#define SHM_MAGIC 0x45

#define CMD_MEMREAD 0x1000
#define CMD_MEMREAD_DIRECT 0x1001

#define CMD_MOUSE 0x1002
#define CMD_KEYBOARD 0x1003

#define CMD_MEMWRITE 0x2000
#define CMD_GET_IMGBASE 0x3000

#define CMD_READY 0x1
#define CMD_FALSE 0x0
#define CMD_DONE 0x2
#define CMD_GETBASE 0x3
#define CMD_READ 0x4
#define CMD_WRITE 0x5
#define CMD_KBD 0x6
#define CMD_LOCKSWITCH 0x7
#define CMD_SETTIME 0x8
#define CMD_ALLOC 0x9
#define CMD_FREE 0x10
#define CMD_QUERY 0x11
#define CMD_SVP 0x12
#define CMD_HVAD 0x13
#define CMD_UM 0x14
#define CMD_UNHVAD 0x15
typedef struct _ss {

    CHAR kInit;
    CHAR uInit;
    CHAR kReady;
    CHAR uReady;
    CHAR kBusy;
    CHAR uBusy;
    CHAR cmdReady;

    DWORD cmd;
    CHAR dataSubmit[512];
    CHAR dataReturn[512];

}SHM_CONTEXT, *PSHM_CONTEXT;



typedef struct _pd {

    DWORD64 ptid;
    CHAR ptpath1[200];
    CHAR ptpath2[200];

}ProtectData, *PProtectData;

class cDrvCtrl
{
public:
    cDrvCtrl()
    {
        m_pSysPath = NULL;
        m_pServiceName = NULL;
        m_pDisplayName = NULL;
        m_hSCManager = NULL;
        m_hService = NULL;

    }
    ~cDrvCtrl()
    {


        StopShareMemThread(secContext);
#ifdef onlyuse
        return;
#endif

        CloseServiceHandle(m_hService);
        CloseServiceHandle(m_hSCManager);
    }
public:
    DWORD m_dwLastError;
    PCHAR m_pSysPath;
    PCHAR m_pServiceName;
    PCHAR m_pDisplayName;

    SC_HANDLE m_hSCManager;
    SC_HANDLE m_hService;
    PSHM_CONTEXT secContext;
public:
    BOOL Install(PCHAR pSysPath, PCHAR pServiceName, PCHAR pDisplayName);
    BOOL Start();
    BOOL Stop();
    BOOL Remove();
    BOOL kSendCmd(PSHM_CONTEXT c);
    VOID StopShareMemThread(PSHM_CONTEXT c);
    void kCleanUp(PSHM_CONTEXT c);

private:
    BOOL GetSvcHandle(PCHAR pServiceName);

protected:
    //null
};





