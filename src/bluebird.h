/*
 * Copyright (c) 2023 Lusion2
 *
 * This software is provided 'as is' and is free for anyone to use
 * If you do use it, I ask that you cite me as your source
 *
 * Bluebird is a project I am making in an attempt to learn more about graphics programming
 * using the WIN32 API and minimal OpenGL (software rendering)
 *
 * Currently, this project will be a software renderer but in the future, I may consider looking at
 * implementing more OpenGL functionality instead of my own code
 *
 *
 */

#ifndef __BLUEBIRD_H__
#define __BLUEBIRD_H__

#include <windows.h>
#include <gl/GL.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdarg.h>
#include <math.h>

// Currently, there are only 10 available shaders that the window can hold
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

// Null int to specify if a parameter has been left empty but is not of pointer type
#define BB_NULL -1

// default pixel scale, if the pixel scale parameter is not set, this will be used
#define BB_DEFAULT_PIXEL_SCALE 4.0f

/*
 * bbPixel
 *
 * Holds the red, green, blue, and alpha channels of a pixel
 */
typedef struct bbPixel bbPixel;
struct bbPixel{
    BYTE R, G, B, A;
};

/*
 * bbVec2f
 *
 * Holds an x and y coordinate as a float in coordinate space (-1.0 to 1.0)
 */
typedef struct bbVec2f bbVec2f;
struct bbVec2f{
    float x, y;
};

/*
 * bbVec2i
 *
 * Holds an x and y coordinate as an unsigned int in pixel space (0 to WIDTH or 0 to HEIGHT )
 */
typedef struct bbVec2i bbVec2i;
struct bbVec2i{
    u32 x, y;
};

/*
 * bbTriF
 *
 * Holds the vertex positions of a triangle in coordinate space using bbVec2f
 */
typedef struct bbTriF bbTriF;
struct bbTriF{
    bbVec2f v1, v2, v3;
};

/*
 * bbShaderState
 *
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
 * bbDrawType
 *
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
 * bbWindow
 *
 * Window struct that will hold global window data
 *
 * Currently, this structure only hold the window handle but
 * more data will be added as this project continues
 *
 * To initialize this, look at the documentation for bbCreateWindow
 */
typedef struct bbWindow bbWindow;
struct bbWindow{
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
     * Pixel information including the width, height, and pixel scale
     */
    float pixel_scale;
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

    //* Time stuff
    u16 delta_t;
    u32 current_t;
    u32 lastFrame_t;

    //* true if the window is open, false if not
    bool isOpen;
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
 * Sets the clear color
 *
 * The alpha channel can be ignored since the background will always be opaque
 */
void bbClearColorP(bbPixel p);
void bbClearColorB(BYTE r, BYTE g, BYTE b);

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
 * Resets the size of the window
 * This is only used in bbWindowProc
 */
void bbWindowResize(bbWindow *bbWin, u32 w, u32 h);

/*
 * Changes the pixel scale
 *
 * If this function is not called, the pixel scale will be set 
 * to BB_DEFAULT_PIXEL_SCALE
 */
void bbSetPixelScale(bbWindow *bbWin, float pScale);

/*
 * Checks if the window is still open
 *
 * This function should be called as the condition of the while loop
 * that updates the program
 */
bool bbWindowIsOpen(bbWindow *bbWin);

/*
 * Checks the WIN32 message queue and updates properly
 *
 * This NEEDS to be called within the update loop
 */
void bbPollEvents(bbWindow *bbWin);

#endif // __BLUEBIRD_H__