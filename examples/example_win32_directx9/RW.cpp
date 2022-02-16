#pragma warning(disable:4996)
#include "windows.h"
#include <iostream>
#include <string>
#include "RW.h"

#include "global.h"
#include "stdio.h"

DWORD64  KGetBasex64(PSHM_CONTEXT context, WCHAR *mdname, SIZE_T sizes)
{
    DWORD64 ret;
    if (context->uInit != TRUE)
        return 0;
    PGetBaseStruct  rwdata = (PGetBaseStruct)(&context->dataSubmit[0]);
    ZeroMemory(rwdata, sizeof(GetBaseStruct));

    rwdata->PID = 0; 
    wcscpy_s(rwdata->ModuleName, sizes, mdname);
    context->cmd = CMD_GETBASE;
    dcptr->kSendCmd(context);
    ret = rwdata->basex64;
    //printf("KGetBasex64 %llx \n", rwdata->basex64);

    dcptr->kCleanUp(context);
    return  ret;
}
VOID  SetSleepTime(PSHM_CONTEXT context, int delay, int longdelay, DWORD32 needFakeSysSwitchs, DWORD64 minSearchBase, DWORD64 maxSearchBase)
{
    if (context->uInit != TRUE)
        return;
    PtimeToSleep  allocdata = (PtimeToSleep)(&context->dataSubmit[0]);

    ZeroMemory(allocdata, sizeof(AllocSubmit));


    allocdata->delay.QuadPart = (LONGLONG)RELATIVE(NANOSECONDS(delay));
    allocdata->longDelay.QuadPart = (LONGLONG)RELATIVE(MILLISECONDS(longdelay));
    allocdata->needFakeSysSwitchs = needFakeSysSwitchs;
    allocdata->minSearchBase = minSearchBase;
    allocdata->maxSearchBase = maxSearchBase;

    context->cmd = CMD_SETTIME;
    dcptr->kSendCmd(context);

    dcptr->kCleanUp(context);

}
VOID MyReadx64(PSHM_CONTEXT context, ULONG pid, LPCVOID  addr, LPVOID lpBuffer, DWORD size) {
    if (context->uInit != TRUE)
        return;
    PRWSubmit  rwsubmit = (PRWSubmit)(&context->dataSubmit[0]);
    PRWRespond  rwresp = (PRWRespond)(&context->dataReturn[0]);

    ZeroMemory(rwsubmit, sizeof(GetBaseStruct));
    ZeroMemory(rwresp, sizeof(RWRespond));

    rwsubmit->PID = pid;
    rwsubmit->MYPID = GetCurrentProcessId();
    rwsubmit->target = (ULONG64)addr;
    rwsubmit->sizes = size;
    rwsubmit->MyAddr = (ULONG64)lpBuffer;
    context->cmd = CMD_READ;
    dcptr->kSendCmd(context);

    memcpy_s(lpBuffer, size, &(rwresp->store[0]), size);
    //DPrint("KGetBasex64 %llx \n", rwdata->basex64);

    dcptr->kCleanUp(context);
}


VOID MyWritex64(PSHM_CONTEXT context, ULONG pid, LPCVOID  addr, LPVOID lpBuffer, DWORD size) {
    if (context->uInit != TRUE)
        return;
    PRWSubmit  rwsubmit = (PRWSubmit)(&context->dataSubmit[0]);
    PRWRespond  rwresp = (PRWRespond)(&context->dataReturn[0]);

    ZeroMemory(rwsubmit, sizeof(GetBaseStruct));
    ZeroMemory(rwresp, sizeof(RWRespond));


    memcpy_s(&(rwresp->store[0]), size, lpBuffer, size);
    rwsubmit->PID = pid;
    rwsubmit->MYPID = GetCurrentProcessId();
    rwsubmit->target = (ULONG64)addr;
    rwsubmit->sizes = size;
    rwsubmit->MyAddr = (ULONG64)lpBuffer;
    context->cmd = CMD_WRITE;
    dcptr->kSendCmd(context);


    //DPrint("KGetBasex64 %llx \n", rwdata->basex64);

    dcptr->kCleanUp(context);
}


int kReadMem(PSHM_CONTEXT context, DWORD pid, DWORD destPid, PVOID target_addr, PVOID dest_addr, SIZE_T read_size, BOOL direct)
{

    if (context->uInit != TRUE)
        return FALSE;

    int ret = FALSE;

    PSHM_MEMCMD memCmd = (PSHM_MEMCMD)context->dataSubmit;

    memCmd->srcAddress = (DWORD64)target_addr;
    memCmd->length = read_size;
    memCmd->pid = pid;

    if (direct)
    {
        context->cmd = CMD_READ;
        memCmd->destPid = destPid;
        memCmd->destAddress = (DWORD64)dest_addr;


    }
    else
    {
        context->cmd = CMD_READ;
    }


    // send then Wait for return

    if (dcptr->kSendCmd(context))
    {
        if (!direct)
            memcpy_s(dest_addr, read_size, context->dataReturn, read_size);
        ret = TRUE;

    }

    dcptr->kCleanUp(context);

    return ret;
}



VOID LockThreadSwitchs(PSHM_CONTEXT context, ULONG *Ptid, SIZE_T size, ULONG num) {
    if (context->uInit != TRUE)
        return;
    PlockSwitch  lcsubmit = (PlockSwitch)(&context->dataSubmit[0]);


    ZeroMemory(lcsubmit, sizeof(lockSwitch));



    memcpy_s(lcsubmit->uThreadId, size, Ptid, size);
    lcsubmit->uThreadNum = num;

    context->cmd = CMD_LOCKSWITCH;
    dcptr->kSendCmd(context);


    //DPrint("KGetBasex64 %llx \n", rwdata->basex64);

    dcptr->kCleanUp(context);
}

#pragma pack(push,1)


//
// x64 payload
//
typedef struct _INJECT_PROCESSID_PAYLOAD_X64 {
    UCHAR	saveReg[25];
    UCHAR	subStack[4];
    UCHAR	restoneHook[32]; // mov rcx,xxxx mov rdi,xxxx mov rsi,xxx rep movs byte
    UCHAR	invokeMemLoad[15]; // mov rcx,xxxxx call xxxx
    // UCHAR	eraseDll[24]; // mov rdi,xxxx xor eax,eax mov rcx,xxxxx rep stosb
    UCHAR	addStack[4];
    UCHAR	restoneReg[27];
    UCHAR	jmpOld[14]; //jmp qword [0]

    UCHAR	oldData[14];//

}INJECT_PROCESSID_PAYLOAD_X64, *PINJECT_PROCESSID_PAYLOAD_X64;

#pragma pack(pop)

VOID SafeWrite(PSHM_CONTEXT context, ULONG pid, LPCVOID  addr, LPVOID lpBuffer, DWORD size) {
    float allsize = (float)size;
    float maxsize = 500.0f;

    DWORD32 allcount = floor(allsize / maxsize);
    DWORD32 otherbytes = ((int)allsize) % ((int)maxsize);
    DWORD32 $index = 0;
    DWORD64 adrTo = (DWORD64)addr;
    DWORD64 adrFrom = (DWORD64)lpBuffer;
    for ($index; $index < allcount; $index++) {
        adrTo = (DWORD64)addr + $index * 500;
        adrFrom = (DWORD64)lpBuffer + $index * 500;
        MyWritex64(context, pid, (LPCVOID)adrTo, (LPVOID)adrFrom, 500);

    }

    if (otherbytes != 0) {
        adrTo = (DWORD64)addr + $index * 500;
        adrFrom = (DWORD64)lpBuffer + $index * 500;
        MyWritex64(context, pid, (LPCVOID)adrTo, (LPVOID)adrFrom, otherbytes);
    }
}

