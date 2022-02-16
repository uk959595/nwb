#pragma once

#include <Windows.h>
#include <vector>
#include "scm.h"
#include <string>
////share

 

////share

extern cDrvCtrl* dcptr;                 
extern bool runread;
               
extern char catchname[6];

extern HWND TopType;
extern HWND hijackHwnd;
extern char dbdname[150];
extern UINT ScreenCenterX;
extern UINT ScreenCenterY;
extern char enddate[100];
extern int remaintimes;

extern UINT toexit;

 
extern HANDLE process;
extern DWORD64 exebase;
extern DWORD64 startread;
extern DWORD64 startoft;
extern DWORD64 maxoft;
extern DWORD64 nameoft;
 

 
 

typedef struct _node {
    DWORD64 start;
    char box[496];
    char str[62*300];
    bool readname;
    bool showfloatA;
    bool showfloatB;
    bool show32A;
    bool show32B;
    bool show64;
    bool showPtr;
} node,*pnode;

extern std::vector<pnode> *pmembox;
extern std::vector <std::string> gInfos;

extern char keys[100];

extern char modulenameA[200];
extern wchar_t modulenameW[200];
extern DWORD64 modbase;
 
extern char pathRecordFile[1000];
typedef struct _actnode {
    DWORD32 sleeptime;//ms
    DWORD32 nameid;//"kdown"1  "kup"2   "mipdown"3 "mmove"4
    DWORD32 code;
    DWORD32 x;
    DWORD32 y;
}actnode, * pactnode;
//200,1,'C',123,543

extern std::vector <actnode>* pRecordPathBox;

extern bool loopPlayflag;
extern bool Recordflag ;
extern bool Playflag ;
extern int check_catchX;
extern int check_catchY;
extern float mouseCheckOftY ;
extern float mouseCheckOftX ;
 
 
