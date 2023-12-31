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
 * implementing the NVIDIA Cuda API to access the graphics card, but that's a long ways away.
 * 
 * Documentation will be made procedurely as I create this project
 * 
 * 
 * Bluebird functions and data structures will be prefixed with "bb"
 * For example, bbWindow or bbCreateWindow()
 * 
 */

#include <windows.h>
#include <gl/GL.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>

#define WIDTH   1280
#define HEIGHT  720

// int typedefs
typedef uint64_t        u64;
typedef int64_t         i64;
typedef uint32_t        u32;
typedef int32_t         i32;
typedef uint16_t        u16;
typedef int16_t         i16;
typedef uint8_t         u8;
typedef int8_t          i8;


/*
 * Struct to hold pixel data
 * 
 * Holds the red, green, blue, and alpha channels of the pixel
 */
typedef struct bbPixel bbPixel;
struct bbPixel{
    BYTE R, G, B, A;
};

/*
 * 2-Dimensional vector struct
 * 
 * Holds an x and y coordinate as a float in coordinate space (-1.0 to 1.0)
 */
typedef struct bbVec2f bbVec2f;
struct bbVec2f{
    float x, y;
};

/*
 * 2-Dimensional vector struct
 * 
 * Holds an x and y coordinate as an unsigned int in pixel space (0 to WIDTH or 0 to HEIGHT )
 */
typedef struct bbVec2i bbVec2i;
struct bbVec2i{
    u32 x, y;
};

/*
 * Triangle struct
 *
 * Holds the vertex positions of a triangle in coordinate space using bbVec2f
 */
typedef struct bbTrif bbTrif;
struct bbTrif{
    bbVec2f v1, v2, v3;
};

/*
 * This is currently unused but could be made into something later 
*/
typedef struct bbStateInfo bbStateInfo;
struct bbStateInfo{

}; 

/* 
 * Window struct that will hold global window data
 * 
 * Currently, this structure only hold the window handle but 
 * more data will be added as this project continues
 * 
 * To initialize this, look at the documentation for bbCreateWindow
*/
typedef struct _bbWindow bbWindow;
struct _bbWindow{
    /*
     * Window handle that the operating system uses to 
     * identify the window being used
    */
    HWND hwnd;

    /*
     * Holds the current state information of the window
     * Currently has no functionality 
     * // TODO : Give bbStateInfo functionality
     */
    // bbStateInfo pState;

    /*
     * Array of all pixels on the screen and the amount as well as width and height
     */
    bbPixel *PIXELS;
    size_t pixels_size;
    size_t width, height;

    /*
     * Device context. This is used to set up the OpenGL context
     */
    HDC hdc;

    /*
     * Pixel format descriptor which holds pixel format data of a drawing surface
    */
    PIXELFORMATDESCRIPTOR pfd;

    /*
     * OpenGL context for drawing to the screen
     */
    HGLRC glContext;
};

/*
 * Creates a window with a name "WindowName" and a width and height of w and h
 * 
 * To use this function, do the following:
 *      bbWindow win = {0};
 *      bbCreateWindow(&win, *name*, *w*, *h*);
 *    
 * 
 * Create the struct variable first, then call the function
 * 
 * This initializes the bbWin struct if a window is created
*/
bool bbCreateWindow(bbWindow *bbWin, LPCSTR WindowName, u32 w, u32 h);

/*
 * Initializes a bbWindow's pfd. This is only used in bbCreateWindow
 */
PIXELFORMATDESCRIPTOR bbSetPFD(bbWindow *bbWin);

/*
 * Terminates a window and frees any dynamically allocated memory
 */
void bbTerminate(bbWindow *bbWin);

/*
 * Clears the screen
*/
void bbClear(bbWindow *bbWin);

/*
 * Draws pixel, p, to a specified location on the screen, (x, y)
*/
void bbPutPixel(bbWindow *win, u32 x, u32 y, bbPixel p);

/*
 * Draws a triangle with vertices specified in the bbTri and is colored in using a shader callback
 * 
 * The 
 */
// void bbTriangleS(bbWindow *win, );

/*
 * Draws the bbWindow PIXELS array to the screen and swaps the buffers
 * 
 * This should be called at the end of each update
 */
void bbSwapBuffers(bbWindow *win);

/*
 * Creates a debug console for printf and other printing commands to work with
 */
void bbCreateDebugConsole();

//*
//* This is the window procedure that handles messages sent to the window
//* from the operating system
//*
LRESULT CALLBACK bbWindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

int main(void){
    // Make a debug console
    bbCreateDebugConsole();

    // Initialize our window
    bbWindow win = {0};
    if(!bbCreateWindow(&win, "Bluebird", WIDTH, HEIGHT)){
        return -1;
    }

    // Begin the message loop
    MSG msg = {0};

    BYTE i = 0;

    while(GetMessage(&msg, NULL, 0, 0) > 0){
        bbClear(&win);
        bbPixel p = {.R = i, .G = i, .B = i};
        bbPutPixel(&win, WIDTH / 2, HEIGHT / 2, p);
        TranslateMessage(&msg);
        DispatchMessage(&msg);
        bbSwapBuffers(&win);
        i++;
    }
    
    bbTerminate(&win);
    return 0;
}

LRESULT CALLBACK bbWindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam){
    bbStateInfo *pState = NULL;
    if(uMsg == WM_CREATE){
        CREATESTRUCT *pCreate = (CREATESTRUCT *)lParam;
        pState = (bbStateInfo *)pCreate->lpCreateParams;
        SetWindowLongPtr(hwnd, GWLP_USERDATA, (LONG_PTR)pState);
    }
    else{
        LONG_PTR ptr = GetWindowLongPtr(hwnd, GWLP_USERDATA);
        pState = (bbStateInfo*)ptr;
    }
    
    switch(uMsg){
        // case WM_PAINT:
        // {
        //     bbPaint(&win);
        // }
        // return 0;

            
        case WM_CLOSE:
            DestroyWindow(hwnd);
            return 0;
        
        case WM_DESTROY:
            PostQuitMessage(0);
            return 0;

        default:
            return DefWindowProc(hwnd, uMsg, wParam, lParam);
    }
    return TRUE;
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

    // Initialize our pixels to draw
    bbWin->pixels_size  = w * h;
    bbWin->width        = w;
    bbWin->height       = h;
    bbWin->PIXELS       = (bbPixel*)malloc(bbWin->pixels_size * sizeof(bbPixel));

    // Set the default clear color
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

    // show the window using ShowWindow
    ShowWindow(bbWin->hwnd, SW_SHOW);
    return true;
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

void bbTerminate(bbWindow *bbWin){
    if(bbWin->PIXELS != NULL){ free(bbWin->PIXELS); }
}

void bbClear(bbWindow *bbWin){
    glClear(GL_COLOR_BUFFER_BIT);
    for(size_t i = 0; i < bbWin->pixels_size; i++){
        bbPixel p = {0};
        bbWin->PIXELS[i] = p;
    }
    glFlush();
}

void bbPutPixel(bbWindow *bbWin, u32 x, u32 y, bbPixel p){
    if(x > bbWin->width || y > bbWin->height){ return; }
    
    bbWin->PIXELS[y*bbWin->width + x] = p;
}

void bbSwapBuffers(bbWindow *bbWin){
    glDrawPixels(bbWin->width, bbWin->height, GL_RGBA, GL_UNSIGNED_BYTE, bbWin->PIXELS);
    SwapBuffers(bbWin->hdc);
}

void bbCreateDebugConsole(){
    AllocConsole();

    FILE *stream;
    freopen_s(&stream, "CONIN$", "r", stdin);
    freopen_s(&stream, "CONOUT$", "w", stdout);
    freopen_s(&stream, "CONOUT$", "w", stderr);
}