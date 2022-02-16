// dear imgui: standalone example application for DirectX 9
// If you are new to dear imgui, see examples/README.txt and documentation at the top of imgui.cpp.

#pragma comment(linker, "/SUBSYSTEM:windows /ENTRY:mainCRTStartup")
#include "imgui.h"
#include "imgui_internal.h"
#include "imgui_impl_dx9.h"
#include "imgui_impl_win32.h"
#include <d3d9.h>
#define DIRECTINPUT_VERSION 0x0800
#include <dinput.h>
#include <tchar.h>
#include "scm.h"
#include "global.h"
#include <iostream>
#include <sstream>
#include "RW.h"
#include "defs.h"
//#include "windivert.h"
#include "kmclass.h"
#include "tools.h"
#include <Psapi.h>
#include "aobscan.h"
#include "utils.h"
#include "record.h"
HMODULE GetProcessModuleHandle(DWORD pid, TCHAR* moduleName, DWORD64 *rt, DWORD64 *rt2) {	// 根据 PID 、模块名（需要写后缀，如：".dll"），获取模块入口地址。
    MODULEENTRY32 moduleEntry;

    HANDLE handle = NULL;
    handle = ::CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, pid); //  获取进程快照中包含在th32ProcessID中指定的进程的所有的模块。
    if (!handle) {
        CloseHandle(handle);
        return NULL;
    }
    ZeroMemory(&moduleEntry, sizeof(MODULEENTRY32));
    moduleEntry.dwSize = sizeof(MODULEENTRY32);
    if (!Module32First(handle, &moduleEntry)) {
        CloseHandle(handle);
        return NULL;
    }
    do {
        std::wstring a = moduleEntry.szModule;
        std::wstring b = moduleName;
        if (a == b) {

            DWORD64 get64 = (DWORD64)moduleEntry.hModule;
            *rt = get64;
            *rt2 = (DWORD64)moduleEntry.modBaseSize;
     

        }
        else {
            /*	printf("\n  moduleName - > %s %s\n",
                    a.c_str(),b.c_str());*/
        }
    } while (Module32Next(handle, &moduleEntry));
    CloseHandle(handle);
    return 0;
}

namespace memory
{
    static uintptr_t find_signature(DWORD pid, TCHAR* moduleName, const char* pattern_, const char* mask) {
        const auto compare = [](const uint8_t * data, const uint8_t * pattern, const char* mask_) {
            for (; *mask_; ++mask_, ++data, ++pattern)
                if (*mask_ == 'x' && *data != *pattern)
                    return false;

            return (*mask_) == 0;
        };


 /*       MODULEINFO module_info = {};
        GetModuleInformation(GetCurrentProcess(), GetModuleHandleA(module), &module_info, sizeof MODULEINFO);*/

        //auto module_start = uintptr_t(module_info.lpBaseOfDll);
        DWORD64 startAdr; DWORD64 modsize;
        GetProcessModuleHandle( pid, moduleName, &startAdr, &modsize);

        const uint8_t* pattern = reinterpret_cast<const uint8_t*>(pattern_);
        for (size_t i = 0; i < modsize; i++)
            if (compare(reinterpret_cast<uint8_t*>(startAdr + i), pattern, mask))
                return startAdr + i;

        return 0;
    }

}

cDrvCtrl* dcptr=NULL;
static int gamePID = 0;
 
 
char catchname[6];
char keys[100];
extern std::vector<pnode> *pmembox = NULL;
 
 
 
 
 

 
std::vector <std::string> gInfos;
 
 
DWORD64 modbase = 0;
DWORD64 modbase2 = 0;
VOID readbox();
WCHAR testpname[] = L"ntdll.dll";
std::string $NULL = "NULL";
std::string $None = "None";
// Data
static LPDIRECT3D9              g_pD3D = NULL;
static LPDIRECT3DDEVICE9        g_pd3dDevice = NULL;
static D3DPRESENT_PARAMETERS    g_d3dpp = {};

// Forward declarations of helper functions
bool CreateDeviceD3D(HWND hWnd);
void CleanupDeviceD3D();
void ResetDevice();
LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
std::vector<pnode>  membox;
// Main code

typedef struct _bkn {
    DWORD64 bk_startread;
    int bk_page;
    char bk_str_startread[200];
    
}bkn,*pbkn;

std::vector<pbkn>  bkbox;

static bool drinit = false;
 
static int oneDiv = 6;
//0 0 0 0 4 0
static int Div1 = 10;
static int Div1_back = 0;
static int Div2 = 460;
static int Div3 = 460;
static bool runscript = false;
 
static bool betop = true;
 
 

int randomInt(int m, int n) { 

    return m;// rand() % (n - m + 1) + m;

}

VOID ToTap(CHAR i, int wt) {
    KeyDown_noirp(dcptr->secContext, MapVirtualKey(i, 0));
    Sleep(wt);
    KeyUp_noirp(dcptr->secContext, MapVirtualKey(i, 0));
}
DWORD32 trynum = 0;


DWORD WINAPI saybye(LPVOID param) {
    MessageBoxA(NULL, "et", "et", NULL);
}
static bool mouseCheckMode = false;
static bool recordAndPlay = false;
float mouseCheckOftY = 0.0763f;
float mouseCheckOftX = 0.0f;
static std::vector <actnode> RecordPathBox;
int main(int, char**)
{
    pRecordPathBox = &RecordPathBox;
    wchar_t szCurrentPath[MAX_PATH];
    GetCurrentDirectoryW(MAX_PATH, szCurrentPath);
    pmembox = &membox;
    // Create application window
    WNDCLASSEX wc = { sizeof(WNDCLASSEX), CS_CLASSDC, WndProc, 0L, 0L, GetModuleHandle(NULL), NULL, NULL, NULL, NULL, szCurrentPath, NULL };
    ::RegisterClassEx(&wc);
    HWND hwnd = ::CreateWindow(wc.lpszClassName, szCurrentPath, WS_OVERLAPPEDWINDOW, 100, 100, 600, 600, NULL, NULL, wc.hInstance, NULL);
 
    // Initialize Direct3D
    if (!CreateDeviceD3D(hwnd))
    {
        CleanupDeviceD3D();
        ::UnregisterClass(wc.lpszClassName, wc.hInstance);
        return 1;
    }

    // Show the window
    ::ShowWindow(hwnd, SW_SHOWDEFAULT);
    ::UpdateWindow(hwnd);

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    //io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    //io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();
    //ImGui::StyleColorsClassic();

    // Setup Platform/Renderer bindings
    ImGui_ImplWin32_Init(hwnd);
    ImGui_ImplDX9_Init(g_pd3dDevice);

    // Load Fonts
    // - If no fonts are loaded, dear imgui will use the default font. You can also load multiple fonts and use ImGui::PushFont()/PopFont() to select them.
    // - AddFontFromFileTTF() will return the ImFont* so you can store it if you need to select the font among multiple.
    // - If the file cannot be loaded, the function will return NULL. Please handle those errors in your application (e.g. use an assertion, or display an error and quit).
    // - The fonts will be rasterized at a given size (w/ oversampling) and stored into a texture when calling ImFontAtlas::Build()/GetTexDataAsXXXX(), which ImGui_ImplXXXX_NewFrame below will call.
    // - Read 'misc/fonts/README.txt' for more instructions and details.
    // - Remember that in C/C++ if you want to include a backslash \ in a string literal you need to write a double backslash \\ !
    //io.Fonts->AddFontDefault();
    //io.Fonts->AddFontFromFileTTF("../../misc/fonts/Roboto-Medium.ttf", 16.0f);
    //io.Fonts->AddFontFromFileTTF("../../misc/fonts/Cousine-Regular.ttf", 15.0f);
    //io.Fonts->AddFontFromFileTTF("../../misc/fonts/DroidSans.ttf", 16.0f);
    //io.Fonts->AddFontFromFileTTF("../../misc/fonts/ProggyTiny.ttf", 10.0f);
    //ImFont* font = io.Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\ArialUni.ttf", 18.0f, NULL, io.Fonts->GetGlyphRangesJapanese());
    //IM_ASSERT(font != NULL);
    ImFont* font = io.Fonts->AddFontFromFileTTF("C:\\Windows\\Fonts\\simhei.ttf", 20.0f, NULL, io.Fonts->GetGlyphRangesChineseFull());
    // Our state
    bool show_demo_window = true;
    bool show_another_window = false;
    ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
    static cDrvCtrl dc;
    dcptr = &dc;
    // Main loop
    MSG msg;
    ZeroMemory(&msg, sizeof(msg));
    while (msg.message != WM_QUIT)
    {
        // Poll and handle messages (inputs, window resize, etc.)
        // You can read the io.WantCaptureMouse, io.WantCaptureKeyboard flags to tell if dear imgui wants to use your inputs.
        // - When io.WantCaptureMouse is true, do not dispatch mouse input data to your main application.
        // - When io.WantCaptureKeyboard is true, do not dispatch keyboard input data to your main application.
        // Generally you may always pass all inputs to dear imgui, and hide them from your application based on those two flags.
        if (::PeekMessage(&msg, NULL, 0U, 0U, PM_REMOVE))
        {
            ::TranslateMessage(&msg);
            ::DispatchMessage(&msg);
            continue;
        }

        // Start the Dear ImGui frame
        ImGui_ImplDX9_NewFrame();
        ImGui_ImplWin32_NewFrame();
        ImGui::NewFrame();
        bool first = true;
        if (first ) {
            ImGui::SetNextWindowSize(ImVec2(600, 600));
            ImGui::SetNextWindowPos(ImVec2(0, 0));
            first = true;
        }

        //gamePID = 0;
        //GetProcessIDByName(L"ffxiv_dx11.exe", &gamePID);
        //if (gamePID == 0) {
        //    playerPos = 0;
        //}
        //else {
        //    if (playerPos == 0) {
        //        DWORD64 a; DWORD64 a2;
        //        GetProcessModuleHandle(gamePID, L"ffxiv_dx11.exe",&a,&a2);
        //     /*   playerPos = memory::find_signature(gamePID,L"ffxiv_dx11.exe" ,
        //            "\x75\x69\x2F\x69\x63\x6F\x6E\x2F", "xxxxxxxxxxxxxxxx"); 4D 5A 90 00 03 00 00 00 75692F69636F6E2F*/
        //        printf("gamePID = %llx  \n", gamePID);
        //        std::vector <DWORD64> vResultContainer =  AobScan::FindSigX64("75692F69636F6E2F", gamePID, 0, 0x7fffffffffff);
        //        for (auto it = vResultContainer.begin(); it != vResultContainer.end(); it++)
        //        {
	       //         printf("find ->>   %llx  \n", it);

        //        }
        //       // printf("playerPos %llx\n", playerPos);
        //        getchar();
        //    }
        //}
        

        // 1. Show the big demo window (Most of the sample code is in ImGui::ShowDemoWindow()! You can browse its code to learn more about Dear ImGui!).

    
        static bool gameLog = true;
  
        {
        
 
            static int dd = 0;
           //info(vformat(u8"消息 %d",dd++));
            ImGui::Begin(" ");                          // Create a window called "Hello, world!" and append into it.
          
            /*       ImGui::TextColored(ImVec4(1.0f, 0.655f, 0.0f, 1.0f), u8"按键持续 %d  Div1 %d  纠正次数 %d  Div1_back  %d",
                       oneDiv, Div1, trynum, Div1_back);*/
            if (!drinit) {
                ImGui::InputText("keys", &keys[0], IM_ARRAYSIZE(keys)); ImGui::SameLine();
            }

            if (drinit == false && ImGui::Button(u8"激活")) {
                for (int k = 0; k < 6; k++) {
                    catchname[k] = keys[k];
                }

                catchname[5] = '\0';

                if (!InitDriver()) {

                    drinit = false;

                }
                else {
                    SetSleepTime(dcptr->secContext, 10, 10, 0, 0, 0x7fffffffffff);
                    drinit = true;


                }
            }
            if (drinit) {
                // ImGui::TextColored(ImVec4(1.0f, 0.0f, 1.0f, 1.0f), "init!");
            }
            else {
                ImGui::Text(u8"暂未激活");
            }
            ImGui::Text(u8" %.3f ms/frame (%.1f FPS)  ", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);

            {
                //鼠标校验 开始
       
                static POINT check_mouse;
              
          
                ImGui::Checkbox(u8"消息日志", &gameLog);ImGui::SameLine();
                ImGui::Checkbox(u8"鼠标位置校验", &mouseCheckMode); ImGui::SameLine();
                ImGui::Checkbox(u8"记录与播放", &recordAndPlay);
                if (mouseCheckMode) {
                    if (ImGui::CollapsingHeader(u8"鼠标位置校验 ")) {
                        GetCursorPos(&check_mouse);
                        ImGui::TextColored(ImVec4(1.0f, 0.655f, 0.0f, 1.0f), u8"当前位置 %d  %d",
                            check_mouse.x, check_mouse.y);
                        ImGui::InputFloat(u8"水平修正参数", &mouseCheckOftX, 0.0f, 0.1f, "%.3f");
                        ImGui::InputFloat(u8"垂直修正参数", &mouseCheckOftY, 0.0f, 0.1f, "%.3f");
                        if (GetAsyncKeyState(VK_INSERT) & 1) {
                            check_catchX = check_mouse.x;
                            check_catchY = check_mouse.y;
                        }
                        if (GetAsyncKeyState(VK_DELETE) & 1) {
                            /*int x = round(float(catchX) *  65535.0f / float(GetSystemMetrics(SM_CXFULLSCREEN)));
                            int y = round(float(catchY) * 65535.0f / float(GetSystemMetrics(SM_CYFULLSCREEN)));*/

                            DWORD x = MulDiv(check_catchX, 65535, GetSystemMetrics(SM_CXFULLSCREEN));
                            DWORD y = MulDiv(check_catchY, 65535, GetSystemMetrics(SM_CYFULLSCREEN));
                            x = x - round(x * mouseCheckOftX);
                            y = y - round(y * mouseCheckOftY);
                            MouseMove_noirp(dcptr->secContext, MOUSEEVENTF_ABSOLUTE | MOUSEEVENTF_MOVE, x, y);

                        }
                        if (GetAsyncKeyState(VK_HOME) & 1) {
                            Mouse_noirp(dcptr->secContext, MOUSE_RIGHT_BUTTON_DOWN);
                            Sleep(30);
                            Mouse_noirp(dcptr->secContext, MOUSE_RIGHT_BUTTON_UP);
                        }
                        if (GetAsyncKeyState(VK_END) & 1) {
                            Mouse_noirp(dcptr->secContext, MOUSE_LEFT_BUTTON_DOWN);
                            Sleep(30);
                            Mouse_noirp(dcptr->secContext, MOUSE_LEFT_BUTTON_UP);
                        }
                        if (GetAsyncKeyState(VK_PRIOR) & 1) {
                            MouseMove_noirp(dcptr->secContext, MOUSE_MOVE_RELATIVE, 100, 100);
                        }
                        if (GetAsyncKeyState(VK_NEXT) & 1) {
                            MouseMove_noirp(dcptr->secContext, MOUSE_MOVE_RELATIVE, 0, 0);
                        }
                    }
                    
                }
                //鼠标校验 结束

                //记录模式 开始
            
            
                
                if (recordAndPlay) {
                    if (ImGui::CollapsingHeader(u8"记录与播放 ")) {
                        ImGui::Text(u8"勾选以后按Ctrl + INS 录制 / 停止  Ctrl + HOME播放");

                        ImGui::Checkbox(u8"循环播放", &loopPlayflag);
                        ImGui::SameLine();
                        ImGui::Text(u8"当前步骤次数 %lld", pRecordPathBox->size());
                        if (Recordflag) {
                            ImGui::SameLine();
                            ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), u8"录制中");
                        }
                        if (Playflag) {
                            ImGui::SameLine();
                            ImGui::TextColored(ImVec4(0.0f, 1.0f, 0.0f, 1.0f), u8"播放中");
                        }
                        ImGui::InputText(u8"录制文件路径", pathRecordFile, IM_ARRAYSIZE(pathRecordFile));
                        if (ImGui::Button(u8"导入")) {
                            Beep(666, 366);
                            importPath();

                        }ImGui::SameLine();
                        if (ImGui::Button(u8"导出")) {
                            Beep(666, 366);
                            exportPath();
                        }
                    }
                    
                }
             
                if (
                    (GetAsyncKeyState(VK_INSERT) & 1) &&
                    (GetAsyncKeyState(VK_CONTROL) & 1) &&
                    recordAndPlay &&
                    Playflag == false
                    ) {
                    if (Recordflag == false) {
                        Beep(333, 566);
                        Recordflag = true;
                        startRecord();
                    }
                    else {
                        Recordflag = false;
                        Beep(566, 266);
                        Beep(566, 266);
                        endRecord();
                    }
                }
                if (
                    (GetAsyncKeyState(VK_HOME) & 1) &&
                    (GetAsyncKeyState(VK_CONTROL) & 1) &&
                    recordAndPlay &&
                    Recordflag == false
                    ) {
                    if (Playflag == false) {
                        Beep(333, 566);
                        Playflag = true;
                        startPlay();
                    }
                    else {
                        Playflag = false;
                        Beep(566, 266);
                        Beep(566, 266);
                        endPlay();
                    }
                }
                //记录模式 结束


            }

           
            //ImGui::Checkbox(u8"置顶", &betop);







            







            ImGui::End();
        }
        if (gameLog) {
            ImGuiWindowFlags  window_flags = 0;
            window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoNav | ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoInputs;
            ImGui::SetNextWindowSize(ImVec2(2560, 1440));
            ImGui::SetNextWindowPos(ImVec2(0, 0));
            ImGui::Begin("log window", NULL, window_flags);
            for (size_t i = 0;i < gInfos.size();i++) {
                ImGui::TextColored(ImVec4(1.0f, 0.655f, 0.0f, 1.0f), gInfos[i].c_str());
            }
            ImGui::End();
        }
        {
            ImGuiWindowFlags  window_flags = 0;
            window_flags|= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoNav  | ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoInputs;
  
            RECT r;
            HWND gamewindow = FindWindowA("CryENGINE", NULL);
            if (gamewindow!=NULL) {
                GetWindowRect(gamewindow, &r);
                ImGuiContext& g = *GImGui;
                ImGui::SetNextWindowSize(ImVec2(r.right - r.left, r.bottom - r.top));
                ImGui::SetNextWindowPos(ImVec2(0, 0));
                ImGui::Begin("game window", NULL, window_flags);
                ImDrawList* $DrawList = ImGui::GetWindowDrawList();

                $DrawList->AddCircle(ImVec2(32, 32), (int)52, IM_COL32(224, 88, 47, 255), 12, 1.0f);
                //$DrawList->AddText(g.Font, g.FontSize , ImVec2(200, 200), IM_COL32(255, 0, 0, 255), u8"时间到了,按END退出...");
               /* ImGui::TextColored(ImVec4(1.0f, 0.655f, 0.0f, 1.0f), u8"进入游戏");
                ImGui::TextColored(ImVec4(1.0f, 0.655f, 0.0f, 1.0f), u8"进入游戏");*/


                ImGui::End();
            }
        
        }
      

        // Rendering
        ImGui::EndFrame();
        g_pd3dDevice->SetRenderState(D3DRS_ZENABLE, false);
        g_pd3dDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, false);
        g_pd3dDevice->SetRenderState(D3DRS_SCISSORTESTENABLE, false);
        D3DCOLOR clear_col_dx = D3DCOLOR_RGBA((int)(clear_color.x*255.0f), (int)(clear_color.y*255.0f), (int)(clear_color.z*255.0f), (int)(clear_color.w*255.0f));
        g_pd3dDevice->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, clear_col_dx, 1.0f, 0);
        if (g_pd3dDevice->BeginScene() >= 0)
        { 
            ImGui::Render();
            ImGui_ImplDX9_RenderDrawData(ImGui::GetDrawData());
            g_pd3dDevice->EndScene();
        }
        HRESULT result = g_pd3dDevice->Present(NULL, NULL, NULL, NULL);

        // Handle loss of D3D9 device
        if (result == D3DERR_DEVICELOST && g_pd3dDevice->TestCooperativeLevel() == D3DERR_DEVICENOTRESET)
            ResetDevice();

 
        if (runscript == true) {
       
           
            /*if (mouse) {
                Sleep(randomInt(Div1, Div1 + 12));
                Mouse_noirp(dcptr->secContext, MOUSE_LEFT_BUTTON_DOWN);
                Sleep(randomInt(oneDiv, oneDiv + 10));
                Mouse_noirp(dcptr->secContext, MOUSE_LEFT_BUTTON_UP);
                 
                Sleep(randomInt(Div1, Div1 + 12));

                Mouse_noirp(dcptr->secContext, MOUSE_RIGHT_BUTTON_DOWN);
                Sleep(randomInt(oneDiv, oneDiv + 10));
                Mouse_noirp(dcptr->secContext, MOUSE_RIGHT_BUTTON_UP);
            }
            else {
                ToTap('O', randomInt(oneDiv, oneDiv + 3));
                Sleep(randomInt(Div3, Div3 + 3));
                ToTap('O', randomInt(oneDiv, oneDiv + 3));
                Sleep(randomInt(Div1, Div1 + 3));
                ToTap('O', randomInt(oneDiv, oneDiv + 3));
                Sleep(randomInt(Div1, Div1 + 3));
                ToTap('O', randomInt(oneDiv, oneDiv + 3));
                Sleep(randomInt(Div1, Div1 + 3));
                ToTap('P', randomInt(oneDiv, oneDiv + 3));
                Sleep(randomInt(Div1, Div2 + 3));
                ToTap('O', randomInt(oneDiv, oneDiv + 3));
                Sleep(randomInt(Div1, Div1 + 3));
            }*/

        }
        if (betop==true) {

        }
   
 
 
       
       /* if (GetAsyncKeyState(VK_DELETE) & 1) {
            mouse = !mouse;
            if (mouse) {
                KeyDown_noirp(dcptr->secContext, MapVirtualKey('X', 0));
            }
            else {

                KeyUp_noirp(dcptr->secContext, MapVirtualKey('X', 0));
            }

        }*/
        
    }

    ImGui_ImplDX9_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();

    CleanupDeviceD3D();
    ::DestroyWindow(hwnd);
    ::UnregisterClass(wc.lpszClassName, wc.hInstance);

    return 0;
}

// Helper functions

bool CreateDeviceD3D(HWND hWnd)
{
    if ((g_pD3D = Direct3DCreate9(D3D_SDK_VERSION)) == NULL)
        return false;

    // Create the D3DDevice
    ZeroMemory(&g_d3dpp, sizeof(g_d3dpp));
    g_d3dpp.Windowed = TRUE;
    g_d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
    g_d3dpp.BackBufferFormat = D3DFMT_UNKNOWN;
    g_d3dpp.EnableAutoDepthStencil = TRUE;
    g_d3dpp.AutoDepthStencilFormat = D3DFMT_D16;
    g_d3dpp.PresentationInterval = D3DPRESENT_INTERVAL_ONE;           // Present with vsync
    //g_d3dpp.PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;   // Present without vsync, maximum unthrottled framerate
    if (g_pD3D->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hWnd, D3DCREATE_HARDWARE_VERTEXPROCESSING, &g_d3dpp, &g_pd3dDevice) < 0)
        return false;

    return true;
}

void CleanupDeviceD3D()
{
    if (g_pd3dDevice) { g_pd3dDevice->Release(); g_pd3dDevice = NULL; }
    if (g_pD3D) { g_pD3D->Release(); g_pD3D = NULL; }
}

void ResetDevice()
{
    ImGui_ImplDX9_InvalidateDeviceObjects();
    HRESULT hr = g_pd3dDevice->Reset(&g_d3dpp);
    if (hr == D3DERR_INVALIDCALL)
        IM_ASSERT(0);
    ImGui_ImplDX9_CreateDeviceObjects();
}

// Win32 message handler
extern LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    if (ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam))
        return true;

    switch (msg)
    {
    case WM_SIZE:
        if (g_pd3dDevice != NULL && wParam != SIZE_MINIMIZED)
        {
            g_d3dpp.BackBufferWidth = LOWORD(lParam);
            g_d3dpp.BackBufferHeight = HIWORD(lParam);
            ResetDevice(); 
        }
        return 0;
    case WM_SYSCOMMAND:
        if ((wParam & 0xfff0) == SC_KEYMENU) // Disable ALT application menu
            return 0;
        break;
    case WM_DESTROY:
        ::PostQuitMessage(0);
        return 0;
    }
    return ::DefWindowProc(hWnd, msg, wParam, lParam);
}


 
