/*
 * Copyright (c) 2023 Lusion2
 *
 * This software is provided 'as is' and is free for anyone to use
 * If you do use it, I ask that you cite me as your source
 * 
 * Bluebird is a project I am making in an attempt to learn more about graphics programming 
 * and the WIN32 API. This project is definitely not the next OpenGL (especially since it's a 
 * software renderer) but it will serve as a great learning opportunity
 * 
 * Currently, this project will be a software renderer but in the future, I may consider looking at
 * implementing the NVIDIA Cuda API to access the graphics card, but that's a long ways away.
 * 
 * Documentation will be made procedurely as I create this project
 * 
 */


#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>


// int typedefs
typedef uint64_t        u64;
typedef int64_t         i64;
typedef uint32_t        u32;
typedef int32_t         i32;
typedef uint16_t        u16;
typedef int16_t         i16;
typedef uint8_t         u8;
typedef int8_t          i8;
typedef uint8_t         BYTE;

// window struct that will hold global window data
typedef struct _bbWindow bbWindow;
struct _bbWindow{
    HWND hwnd;
};

typedef struct StateInfo{
    int myInt;
} StateInfo;

//*
//*
//*
bool bbCreateWindow(bbWindow *bbWin, LPCSTR lpWindowName, int nWidth, int nHeight);

//*
//* This is the window procedure that handles messages sent to the window
//* from the operating system
//*
LRESULT CALLBACK bbWindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

// The WINAPI main function, used to begin a program
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
                   PSTR lpCmdLine, int nCmdShow)
{
    bbWindow win = {0};
    bbCreateWindow(&win, "Bluebird", 1280, 720);

    // show the window using ShowWindow
    ShowWindow(win.hwnd, nCmdShow);

    // Begin the message loop
    MSG msg = {0};
    while(GetMessage(&msg, NULL, 0, 0) > 0){
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    
    return 0;
}

LRESULT CALLBACK bbWindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam){
    switch(uMsg){
        case WM_PAINT:
        {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hwnd, &ps);

            // All painting occurs here, between BeginPaint and EndPaint.

            FillRect(hdc, &ps.rcPaint, (HBRUSH) (COLOR_WINDOW+1));

            EndPaint(hwnd, &ps);
        }
        return 0;

        case WM_CLOSE:
            DestroyWindow(hwnd);
            // Else, user cancelled, do nothing
            return 0;
        
        case WM_DESTROY:
            PostQuitMessage(0);
            return 0;

        default:
            return DefWindowProc(hwnd, uMsg, wParam, lParam);
    }
}


bool bbCreateWindow(bbWindow *bbWin, LPCSTR lpWindowName, int width, int height){
    // register the window class
    const wchar_t CLASS_NAME[] = L"Sample Window Class";
    WNDCLASS wc = {0};
    wc.lpfnWndProc = bbWindowProc;
    wc.hInstance = GetModuleHandle(NULL);
    wc.lpszClassName = (LPCSTR)CLASS_NAME;
    RegisterClass(&wc);

    StateInfo pState = {0};

    // create the window using CreateWindowEx()
    HWND hwnd = CreateWindowEx(
        0,                   // Optional window styles.
        (LPCSTR)CLASS_NAME,  // Window class
        lpWindowName,        // Window text
        WS_OVERLAPPEDWINDOW, // Window style

        // Size and position
        CW_USEDEFAULT, CW_USEDEFAULT, width, height,

        NULL,                  // Parent window
        NULL,                  // Menu
        GetModuleHandle(NULL), // Instance handle
        bbWin                  // Additional application data
    );

    bbWin->hwnd = hwnd;

    return (hwnd ? TRUE : FALSE);
}