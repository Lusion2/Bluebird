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
    int myInt;
};


//*
//* This is the window procedure that handles messages sent to the window 
//* from the operating system
//*
LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);


// The WINAPI main function, used to begin a program
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
                   PSTR lpCmdLine, int nCmdShow)
{
    // begin by registering the window class
    const wchar_t CLASS_NAME[] = L"Sample Window Class";
    WNDCLASS wc = {0};
    wc.lpfnWndProc      = WindowProc;
    wc.hInstance        = hInstance;
    wc.lpszClassName    = (LPCSTR)CLASS_NAME;
    RegisterClass(&wc);

    // now create the window using CreateWindowEx()
    HWND hwnd = CreateWindowEx(
        0,                              // Optional window styles.
        (LPCSTR)CLASS_NAME,             // Window class
        "Learn to Program Windows",     // Window text
        WS_OVERLAPPEDWINDOW,            // Window style

        // Size and position
        CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,

        NULL,      // Parent window
        NULL,      // Menu
        hInstance, // Instance handle
        NULL       // Additional application data
    );

    if(hwnd == NULL){ return -1; } // if the window wasn't created, close the program

    // show the window using ShowWindow
    ShowWindow(hwnd, nCmdShow);

    // Begin the message loop
    MSG msg = {0};
    while(GetMessage(&msg, NULL, 0, 0) > 0){
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    
    return 6;
}

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam){
    switch(uMsg){
        case WM_SIZE: // Hande window resizing
            {
                int width = LOWORD(lParam);     // Macro to get the low-order word
                int height = HIWORD(lParam);    // Macro to get the high-order word
                // respond to the message
            }
            break;
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
        if(MessageBox(hwnd, "Really quit?", "My application", MB_OKCANCEL) == IDOK){
            DestroyWindow(hwnd);
        }
        // Else, user cancelled, do nothing
        return 0;
        
        case WM_DESTROY:
            PostQuitMessage(0);
            return 0;

        default:
            return DefWindowProc(hwnd, uMsg, wParam, lParam);
    }
}
