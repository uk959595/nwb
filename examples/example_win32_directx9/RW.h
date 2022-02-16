#pragma once
#include <Windows.h>
#include "scm.h"
typedef struct {
    WCHAR ModuleName[110];
    ULONG PID;
    ULONG64 basex64;
}GetBaseStruct, *PGetBaseStruct;

typedef struct {
    ULONG PID;
    SIZE_T sizes;
    ULONG prot;
    ULONG allocTP;
}AllocSubmit, *PAllocSubmit;

typedef struct {
    ULONG64 address;
    SIZE_T RetSizes;
}AllocRespond, *PAllocRespond;
//
#pragma warning(disable : 4214 4201)
#pragma pack(push, 1)
typedef struct _MMVAD_FLAGS_WIN7_7601 // Size=4
{
    DWORD64 CommitCharge : 51;
    DWORD64 NoChange : 1;
    DWORD64 VadType : 3;
    DWORD64 MemCommit : 1;
    DWORD64 Protection : 5;
    DWORD64 Spare : 2;
    DWORD64 PrivateMemory : 1;
} MMVAD_FLAGS_WIN7_7601, *PMMVAD_FLAGS_WIN7_7601;


typedef struct _MMVAD_FLAGS_WIN10 // Size=4
{
    DWORD32 Lock : 1;
    DWORD32 LockContended : 1;
    DWORD32 DeleteInProgress : 1;
    DWORD32 NoChange : 1;
    DWORD32 VadType : 3;
    DWORD32 Protection : 5;
    DWORD32 PreferredNode : 6;
    DWORD32 PageSize : 2;
    DWORD32 PrivateMemory : 1;
} MMVAD_FLAGS_WIN10, *PMMVAD_FLAGS_WIN10;


#pragma pack(pop)

typedef struct _HVADSubmit
{
    DWORD64 RegionBase;             // Region base address
    SIZE_T RegionSize;				// Region size
    ULONG pid;                  // Target process ID
    DWORD64 FakeStartFormate;
} HVADSubmit, *PHVADSubmit;

typedef struct _HVADRespond
{
    DWORD64 FakeStartFormate64;
    DWORD64 FakeEndFormate64;
    DWORD64 OrgStartFormate64;
    DWORD64 OrgEndFormate64;

    DWORD32 FakeStartFormate32;
    DWORD32 FakeEndFormate32;
    DWORD32 OrgStartFormate32;
    DWORD32 OrgEndFormate32;

    UCHAR FakeStartH;
    UCHAR FakeEndH;
    UCHAR OrgStartH;
    UCHAR OrgEndH;

    MMVAD_FLAGS_WIN7_7601 FakeFlagWIN761;
    MMVAD_FLAGS_WIN10 FakeFlagWIN10;
    MMVAD_FLAGS_WIN7_7601 OrgFlagWIN761;
    MMVAD_FLAGS_WIN10 OrgFlagWIN10;

} HVADRespond, *PHVADRespond;

typedef struct _timeToSleep {
    LARGE_INTEGER delay;
    LARGE_INTEGER longDelay;
    DWORD32 needFakeSysSwitchs;
    DWORD64 minSearchBase;
    DWORD64 maxSearchBase;

}timeToSleep, *PtimeToSleep;
PVOID MyReadFile(const WCHAR* fileName, PULONG fileSize);
VOID IJ(PSHM_CONTEXT secContext, ULONG pid, PVOID dllmem, ULONG dllsize, PVOID hookAddrForIJ);
DWORD64  KGetBasex64(PSHM_CONTEXT context, WCHAR *mdname, SIZE_T sizes);
VOID MyReadx64(PSHM_CONTEXT context, ULONG pid, LPCVOID  addr, LPVOID lpBuffer, DWORD size);
VOID SafeWrite(PSHM_CONTEXT context, ULONG pid, LPCVOID  addr, LPVOID lpBuffer, DWORD size);
VOID MyWritex64(PSHM_CONTEXT context, ULONG pid, LPCVOID  addr, LPVOID lpBuffer, DWORD size);
VOID LockThreadSwitchs(PSHM_CONTEXT context, ULONG *Ptid, SIZE_T size, ULONG num);
DWORD64  KALLOC(PSHM_CONTEXT context, ULONG pid, SIZE_T sizes, ULONG pro, SIZE_T *retsize, ULONG allocTP);
VOID HVAD(PSHM_CONTEXT context, ULONG pid, DWORD64 adr, SIZE_T sizes, DWORD64 FakeStart);
SIZE_T FRE(PSHM_CONTEXT context, ULONG pid, ULONG64 addr, SIZE_T sizes, ULONG tp);
ULONG SVP(PSHM_CONTEXT context, ULONG pid, ULONG64 addr, SIZE_T sizes, ULONG pro);
VOID UM(PSHM_CONTEXT context, ULONG pid, DWORD64 adr);
VOID  SetSleepTime(PSHM_CONTEXT context, int delay, int longdelay, DWORD32 needFakeSysSwitchs, DWORD64 minSearchBase, DWORD64 maxSearchBase);
SIZE_T QuerryMem(PSHM_CONTEXT context, ULONG pid, ULONG64 addr, MEMORY_BASIC_INFORMATION *mbi);
int kReadMem(PSHM_CONTEXT context, DWORD pid, DWORD destPid, PVOID target_addr, PVOID dest_addr, SIZE_T read_size, BOOL direct);
typedef struct {
    ULONG PID;
    ULONG MYPID;
    ULONG64 target;
    ULONG64 MyAddr;
    SIZE_T sizes;
}RWSubmit, *PRWSubmit;

typedef struct {
    BYTE store[500];
}RWRespond, *PRWRespond;

typedef struct {
    ULONG PID;
    ULONG64 address;
    SIZE_T RegionSize;
    ULONG tp;
}FreeSubmit, *PFreeSubmit;

typedef struct {
    SIZE_T RegionSize;
}FreeRespond, *PFreeRespond;

typedef enum _MEMORY_INFORMATION_CLASS {
    MemoryBasicInformation
} MEMORY_INFORMATION_CLASS;

typedef struct {
    ULONG PID;
    ULONG64 address;
    MEMORY_INFORMATION_CLASS  emutype;
}QuerySubmit, *PQuerySubmit;

typedef struct {
    MEMORY_BASIC_INFORMATION mbi;
    SIZE_T query_retsize;
}QueryRespond, *PQueryRespond;
typedef struct {

    DWORD pid;
    DWORD destPid;
    DWORD64 srcAddress;
    DWORD64 destAddress;
    DWORD length;
    DWORD Write;
    CHAR data[256];

}SHM_MEMCMD, *PSHM_MEMCMD;

//
typedef struct {
    ULONG PID;
    ULONG64 address;
    ULONG MyProtect;
    SIZE_T sizes;
}SVPSubmit, *PSVPSubmit;

typedef struct {
    ULONG OldProtect;
}SVPRespond, *PSVPRespond;


typedef struct _lockSwitch {
    ULONG uThreadNum;
    ULONG64 uThreadId[8];
}lockSwitch, *PlockSwitch;

typedef struct {
    ULONG PID;
    ULONG64 address;
}UMSubmit, *PUMSubmit;

typedef struct {
    ULONG OldProtect;
}UMRespond, *PUMRespond;

//below is a str8 rip from  http://www.osronline.com/article.cfm%5earticle=261.htm
//thanks
//1√Î = 1000∫¡√Î
//1∫¡√Î = 1000Œ¢√Î
//1Œ¢√Î = 1000ƒ…√Î
//1ƒ…√Î = 1000∆§√Î
//1s = 1000ms
//1ms = 1000¶Ãs
//1¶Ãs = 1000ns
//1ns = 1000ps
#define ABSOLUTE(wait) (wait)

#define RELATIVE(wait) (-(wait))

#define NANOSECONDS(nanos) \
(((signed __int64)(nanos)) / 100L)

#define MICROSECONDS(micros) \
(((signed __int64)(micros)) * NANOSECONDS(1000L))

#define MILLISECONDS(milli) \
(((signed __int64)(milli)) * MICROSECONDS(1000L))

#define SECONDS(seconds) \
(((signed __int64)(seconds)) * MILLISECONDS(1000L))
