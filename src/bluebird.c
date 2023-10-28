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

#define BB_PIXEL_SCALE  4.0f

// Currently, there are only 10 available shaders
#define MAX_SHADERS 10

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
typedef struct bbTriF bbTriF;
struct bbTriF{
    bbVec2f v1, v2, v3;
};

/*
 * Holds shader parameters that can be activated and deactivated at the user's command
 *
 * The bbWindow struct will hold an array of bbShaderState for different shaders to be used
 */
typedef struct bbShaderState bbShaderState;
struct bbShaderState{
    /*
     * The colors at each corner of a triangle
     *
     * color[0] will map to tri.v1,
     * color[1] will map to tri.v2,
     * color[2] will map to tri.v3
     *
     * Color coords can be linearly interpolated using shaders
     */
    bbPixel color[3];
    bool useColorCoords;
};

/*
 * This is currently unused but could be made into something later
 */
// typedef struct bbStateInfo bbStateInfo;
// struct bbStateInfo{
// };

/*
 * Enum to determine the draw type
 *
 * Currently, primitives can be drawn filled in or as the wireframe
 */
typedef enum bbDrawType bbDrawType;
enum bbDrawType{
    bbFILL,
    bbLINE
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

    /*
     * The draw type enum to determine whether to draw
     * lines or to fill in primitives
     *
     * This is default initialized to bbFILL
     */
    bbDrawType DrawType;

    // Shader functionality here:
    /*
     * The shaderState for each shader
     *
     * The shaderIndex will hold the current shader state that we want to draw
     * The shader[shaderIndex] will use the shaderState[shaderIndex] to draw
     */
    bbShaderState shaderState[MAX_SHADERS];
    u8 shaderIndex;
    void (*shader[MAX_SHADERS])(bbWindow *win, bbVec2i v);

    // Time stuff
    //* between last frame and this frame
    u16 delta_t;

    //* time of the current frame
    u32 current_t;

    //* time of the last frame
    u32 lastFrame_t;
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
 * Sets the shader at bbWin->shaderIndex
 */
void bbSetShader(bbWindow *bbWin, void (*shader)(bbWindow *win, bbVec2i v));

/*
 * Sets the current shader index
 *
 * The most recent index set will be the one used when calling a shader
 */
void bbSetShaderIndex(bbWindow *bbWin, u8 index);

/*
 * Sets shader state at bbWin->shaderIndex
 */
void bbSetShaderState(bbWindow *bbWin, bbShaderState state);

/*
 * Updates the delta time and currentframe time
 */
void bbUpdateTime(bbWindow *bbWin);

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
void bbClear();

/*
 * Draws pixel, p, to a specified location on the screen, (x, y)
 */
void bbPutPixel(u32 x, u32 y, bbPixel p);

/*
 * Returns a positive number if the point is on the right of a line
 * Returns a negative number if the point is on the left of the line
 * Returns zero if the point is on the line
 * This function is used for triangle rasterization, it can be ignored
 */
float bbEdgeFunc(bbVec2f *a, bbVec2f *b, bbVec2f *c);

/*
 * Draws a triangle with vertices specified in the bbTri and is colored in using a shader callback
 *
 * The shader used will be the shader at bbWin->shaders[bbWin->shaderIndex] and bbWin->shaderState[bbWin->shaderIndex]
 * will determine the parameters used. This has to be set before calling this function
 *
 * The shadercallback is a function defined by the programmer used to color in the triangle
 * Shaders will only be used for color
 *
 * An example shadercallback might look like this:
 *      // TODO : {insert code here}
 */
void bbTriangleFS(bbWindow *bbWin, bbTriF tri);

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

/*
 * Adds the FPS to the window's title
 */
void bbDEBUGfpsCounter(bbWindow *bbWin);

//*
//* This is the window procedure that handles messages sent to the window
//* from the operating system
//*
LRESULT CALLBACK bbWindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

/*
 * Sets the size of the window and 
 */
void bbWindowResize(bbWindow *bbWin, u32 w, u32 h);


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

    // Begin the message loop
    MSG msg = {0};

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

    while(GetMessage(&msg, NULL, 0, 0) > 0){

        bbUpdateTime(&win);

        // draw at 30 fps
        if(win.current_t - win.lastFrame_t > 16){   // this says 16 but the avg ms per frame is about 31-32. do not know why but it works
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

        // handle messages
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    
    bbTerminate(&win);
    return 0;
}

void shader1(bbWindow *bbWin, bbVec2i v){
    bbPixel p = {.R = cos(bbWin->current_t * 0.001f) * 0xff};
    // bbShaderState state = bbWin->shaderState[bbWin->shaderIndex];
    
    
    bbPutPixel(v.x, v.y, p);
}

void shader2(bbWindow *bbWin, bbVec2i v){
    bbPixel p = {0x55, 0x00, 0xff, 0xff};
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

    SetWindowLongPtr(bbWin->hwnd, GWLP_USERDATA, (LONG_PTR)bbWin);

    bbWindowResize(bbWin, w, h);

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
    glPointSize(BB_PIXEL_SCALE);
    glOrtho(0, WIDTH, 0, HEIGHT, -1, 1);

    // show the window using ShowWindow
    ShowWindow(bbWin->hwnd, SW_SHOW);
    return true;
}

LRESULT CALLBACK bbWindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam){
    bbWindow *bbWin = (bbWindow *)GetWindowLongPtr(hwnd, GWLP_USERDATA);
    if(bbWin == NULL || bbWin == (bbWindow *)-1){
        return DefWindowProc(hwnd, uMsg, wParam, lParam);
    }
    
    switch(uMsg){ 
        case WM_SIZE:
            // Need  bbWin struct to fix size issues
            bbWindowResize(bbWin, LOWORD(lParam), HIWORD(lParam));
            return 0;
        case WM_PAINT:
            return 0;
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

void bbCreateDebugConsole(){
    AllocConsole();

    FILE *stream;
    freopen_s(&stream, "CONIN$", "r", stdin);
    freopen_s(&stream, "CONOUT$", "w", stdout);
    freopen_s(&stream, "CONOUT$", "w", stderr);
}