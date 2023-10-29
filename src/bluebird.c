/*
 * Copyright (c) 2023 Lusion2
 *
 * This software is provided 'as is' and is free for anyone to use
 * If you do use it, I ask that you cite me as your source
 * 
 * Bluebird is a project I am making in an attempt to learn more about graphics programming 
 * and the WIN32 API and minimal OpenGL (software rendering)
 * 
 * Currently, this project will be a software renderer but in the future, I may consider looking at
 * implementing more OpenGL functionality instead of my own code
 * 
 * 
 * This is currently the implementation of the Bluebird library
 * 
 * 
 */

#include <windows.h>
#include <gl/GL.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdarg.h>
#include <math.h>

#include "bluebird.h"

#define WIDTH   1280
#define HEIGHT  720

void shader1(bbWindow *bbWin, bbVec2i v);
void shader2(bbWindow *bbWin, bbVec2i v);

int main(void){
    // Make a debug console
    bbCreateDebugConsole();

    // Initialize our window
    bbWindow win = {0};
    if(!bbCreateWindow(&win, "Bluebird", WIDTH, HEIGHT)){
        return -1;
    }

    // bbShaderState state = {.useColorCoords = true};
    // state.colorCoords[0].x = 0.25f; state.colorCoords[0].y = 0.25f; 

    bbSetShaderIndex(&win, 0);
    bbSetShader(&win, shader1);
    bbSetShaderIndex(&win, 1);
    bbSetShader(&win, shader2);
    // bbSetShaderState(&win, state);

    bbTriF tri1 = {
        .v1.x = 0.5f, .v1.y = 0.5f, // top right
        .v2.x = 0.5f, .v2.y = -0.5f,
        .v3.x = -0.5f, .v3.y = -0.5f, 
    };
    bbTriF tri2 = {
        .v1.x = -0.5f, .v1.y =  0.5f, // top left
        .v2.x =  0.5f, .v2.y =  0.5f, // top right
        .v3.x = -0.5f, .v3.y = -0.5f, // bottom left
    };

    while(bbWindowIsOpen(&win)){

        bbUpdateTime(&win);
        bbPollEvents(&win);

        // draw at 30 fps
        if(win.current_t - win.lastFrame_t > 16){ // this says 16 but the avg ms per frame is about 31-32. I don't know why but it works
            bbDEBUGfpsCounter(&win);
            win.lastFrame_t = win.current_t;
            printf("Delta: %i ms\n", win.delta_t);
            bbClear();
            
            bbSetShaderIndex(&win, 0);
            bbTriangleFS(&win, tri1);
            bbSetShaderIndex(&win, 1);
            bbTriangleFS(&win, tri2);

            bbSwapBuffers(&win);
        }
    }
    
    bbTerminate(&win);
    return 0;
}

void shader1(bbWindow *bbWin, bbVec2i v){
    bbPixel p = {.R = 0x40, .B = cos(bbWin->current_t * 0.0001f) * 0xff - 1};
    // bbShaderState state = bbWin->shaderState[bbWin->shaderIndex];
    
    
    bbPutPixel(v.x, v.y, p);
}

void shader2(bbWindow *bbWin, bbVec2i v){
    bbPixel p = {.R = 0x40, .B = sin(bbWin->current_t * 0.0001f) * 0xff - 1};
    // bbShaderState state = bbWin->shaderState[bbWin->shaderIndex];
    
    bbPutPixel(v.x, v.y, p);
}

bool bbCreateWindow(bbWindow *bbWin, LPCSTR WindowName, u32 w, u32 h){
    // Setting this to NULL in case it has a garbage value if bbTerminate() 
    // is called before mallocing the pixels
    // bbWin->PIXELS = NULL;
    
    // register the window class
    const wchar_t CLASS_NAME[] = L"Bluebird Window Class";
    WNDCLASS wc = {0};
    wc.lpfnWndProc = bbWindowProc;
    wc.hInstance = GetModuleHandle(NULL);
    wc.lpszClassName = (LPCSTR)CLASS_NAME;
    RegisterClass(&wc);

    // create the window using CreateWindowEx()
    HWND hwnd = CreateWindowEx(
        0,                   // Optional window styles.
        (LPCSTR)CLASS_NAME,  // Window class
        WindowName,          // Window text
        WS_OVERLAPPEDWINDOW, // Window style

        // Size and position
        CW_USEDEFAULT, CW_USEDEFAULT, w, h,

        NULL,                  // Parent window
        NULL,                  // Menu
        NULL,                  // Instance handle
        bbWin                  // Additional application data
    );

    // return false if the window was not created
    if(!hwnd){ return false; }

    // initialize the bbWin strct
    bbWin->hwnd         = hwnd;
    bbWin->hdc          = GetDC(bbWin->hwnd);
    if(!bbWin->hdc){
        MessageBox(bbWin->hwnd, "issue creating hdc", "Error", 1);
        exit(1);
    }
    // pixel format descriptor
    bbWin->pfd          = bbSetPFD(bbWin);
    int pixelFormat     = ChoosePixelFormat(bbWin->hdc, &bbWin->pfd);
    if(pixelFormat == 0){
        MessageBox(bbWin->hwnd, "issue with pixelFormat", "Error", 1);
        exit(1);
    }
    // set the pixel format
    if(!SetPixelFormat(bbWin->hdc, pixelFormat, &bbWin->pfd)){
        MessageBox(bbWin->hwnd, "issue setting pixelFormat", "Error", 1);
        exit(1);
    }
    // create the OpenGL context
    bbWin->glContext    = wglCreateContext(bbWin->hdc);
    if(!bbWin->glContext){
        MessageBox(bbWin->hwnd, "issue creating OpenGL context", "Error", 1);
        bbTerminate(bbWin);
        exit(1);
    }
    if(!wglMakeCurrent(bbWin->hdc, bbWin->glContext)){
        MessageBox(bbWin->hwnd, "issue creating OpenGL context", "Error", 1);
        bbTerminate(bbWin);
        exit(1);
    }

    // Add this window to the user data so we can access it within bbWindowProc
    SetWindowLongPtr(bbWin->hwnd, GWLP_USERDATA, (LONG_PTR)bbWin);

    // Initialize the size of the window
    bbWindowResize(bbWin, w, h);
    bbWin->pixel_scale = BB_DEFAULT_PIXEL_SCALE;

    // Default the draw type to fill
    bbWin->DrawType     = bbFILL;

    // Default the shader index to 0
    bbWin->shaderIndex  = 0;

    // Initialize time
    bbWin->delta_t        = 0;
    bbWin->current_t      = 0;
    bbWin->lastFrame_t    = 0;

    // OpenGL initialization
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glPointSize(bbWin->pixel_scale);
    glOrtho(0, WIDTH, 0, HEIGHT, -1, 1);

    // show the window using ShowWindow
    ShowWindow(bbWin->hwnd, SW_SHOW);
    bbWin->isOpen = true;
    return true;
}

LRESULT CALLBACK bbWindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam){
    // Get a pointer to the window so we can access it without using it as a parameter 
    bbWindow *bbWin = (bbWindow *)GetWindowLongPtr(hwnd, GWLP_USERDATA);
    if(bbWin == NULL || bbWin == (bbWindow *)-1){
        return DefWindowProc(hwnd, uMsg, wParam, lParam);
    }
    
    switch(uMsg){ 
        case WM_SIZE:
            bbWindowResize(bbWin, LOWORD(lParam), HIWORD(lParam));
            return 0;
        case WM_PAINT:
            return 0;
        case WM_CLOSE:
            DestroyWindow(hwnd);
            bbWin->isOpen = false;
            return 0;
        
        case WM_DESTROY:
            PostQuitMessage(0);
            return 0;

        default:
            return DefWindowProc(hwnd, uMsg, wParam, lParam);
    }
    return TRUE;
}

void bbWindowResize(bbWindow *bbWin, u32 w, u32 h){
    printf("Width: %i, Height: %i\n", w, h);
    bbWin->width = w;
    bbWin->height = h;
    bbWin->pixels_size = w * h;
}

PIXELFORMATDESCRIPTOR bbSetPFD(bbWindow *bbWin){
    PIXELFORMATDESCRIPTOR pfd = // pfd Tells Windows How We Want Things To Be
        {
            sizeof(PIXELFORMATDESCRIPTOR),  // Size Of This Pixel Format Descriptor
            1,                              // Version Number
            PFD_DRAW_TO_WINDOW |            // Format Must Support Window
            PFD_SUPPORT_OPENGL |            // Format Must Support OpenGL
            PFD_DOUBLEBUFFER,               // Must Support Double Buffering
            PFD_TYPE_RGBA,                  // Request An RGBA Format
            32,                             // Select Our Color Depth
            0, 0, 0, 0, 0, 0,               // Color Bits Ignored
            0,                              // No Alpha Buffer
            0,                              // Shift Bit Ignored
            0,                              // No Accumulation Buffer
            0, 0, 0, 0,                     // Accumulation Bits Ignored
            16,                             // 16Bit Z-Buffer (Depth Buffer)
            0,                              // No Stencil Buffer
            0,                              // No Auxiliary Buffer
            PFD_MAIN_PLANE,                 // Main Drawing Layer
            0,                              // Reserved
            0, 0, 0                         // Layer Masks Ignored
        };
        bbWin->pfd = pfd;
        return pfd;
}

void bbSetShader(bbWindow *bbWin, void (*shader)(bbWindow *win, bbVec2i v)){
    bbWin->shader[bbWin->shaderIndex] = shader;
}

void bbSetShaderIndex(bbWindow *bbWin, u8 index){
    if(index >= MAX_SHADERS) { return; }
    bbWin->shaderIndex = index;
}

void bbSetShaderState(bbWindow *bbWin, bbShaderState state){
    bbWin->shaderState[bbWin->shaderIndex] = state;
}

void bbSetPixelScale(bbWindow *bbWin, float pScale){
    bbWin->pixel_scale = pScale;
}

void bbUpdateTime(bbWindow *bbWin){
    DWORD t = GetTickCount();       // Time in milliseconds since system started
    bbWin->current_t = t;           // Set the current time to t
    bbWin->delta_t = bbWin->current_t - bbWin->lastFrame_t; // update delta
}

void bbDEBUGfpsCounter(bbWindow *bbWin){
    char str[64];
    float fps = 1000.0f / (bbWin->delta_t);     // 1000 / deltatime is fps
    sprintf(str, "Bluebird - FPS: %.3f", fps);

    SetWindowTextA(bbWin->hwnd, str);
}

void bbTerminate(bbWindow *bbWin){
    TerminateProcess(bbWin->hwnd, 0);
}

void bbClear(){
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glFlush();
}

void bbClearColorP(bbPixel p){
    glClearColor(p.R/0xFF, p.G/0xFF, p.B/0xFF, 0xFF);
}

void bbClearColorB(BYTE r, BYTE g, BYTE b){
    glClearColor(r/0xFF, g/0xFF, b/0xFF, 0xFF);
}

void bbPutPixel(u32 x, u32 y, bbPixel p){
    glBegin(GL_POINTS);
    glColor4ub(p.R, p.G, p.B, p.A);
    glVertex2i(x, y);
    glEnd();
}

float bbEdgeFunc(bbVec2f *a, bbVec2f *b, bbVec2f *c){
    return ((c->x - a->x) * (b->y - a->y) - (c->y - a->y) * (b->x - a->x));
}

void bbTriangleFS(bbWindow *bbWin, bbTriF tri){
    if(bbWin->DrawType == bbFILL){
        tri.v1.x = (((tri.v1.x + 1.0f) * 0.5f) * bbWin->width); tri.v1.y = (((tri.v1.y + 1.0f) * 0.5f) * bbWin->height);
        tri.v2.x = (((tri.v2.x + 1.0f) * 0.5f) * bbWin->width); tri.v2.y = (((tri.v2.y + 1.0f) * 0.5f) * bbWin->height);
        tri.v3.x = (((tri.v3.x + 1.0f) * 0.5f) * bbWin->width); tri.v3.y = (((tri.v3.y + 1.0f) * 0.5f) * bbWin->height);
        for(u32 y = 0; y < bbWin->height; y++){
            for(u32 x = 0; x < bbWin->width; x++){
                bbVec2f p = {.x = x, .y = y};
                float w0 = bbEdgeFunc(&tri.v2, &tri.v3, &p);
                float w1 = bbEdgeFunc(&tri.v3, &tri.v1, &p);
                float w2 = bbEdgeFunc(&tri.v1, &tri.v2, &p);
                if(w0 >= 0 && w1 >= 0 && w2 >= 0){
                    bbVec2i v = {x, y};
                    bbWin->shader[bbWin->shaderIndex](bbWin, v);
                }
            }
        }
    }
}

void bbSwapBuffers(bbWindow *bbWin){
    SwapBuffers(bbWin->hdc);
}

bool bbWindowIsOpen(bbWindow *bbWin){
    return bbWin->isOpen;
}

void bbPollEvents(bbWindow *bbWin){
    MSG msg = {0};
    if(GetMessage(&msg, NULL, 0, 0) > 0){
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    else{
        bbWin->isOpen = false;
    }
}

void bbCreateDebugConsole(){
    AllocConsole();

    FILE *stream;
    freopen_s(&stream, "CONIN$", "r", stdin);
    freopen_s(&stream, "CONOUT$", "w", stdout);
    freopen_s(&stream, "CONOUT$", "w", stderr);
}