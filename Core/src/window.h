#pragma once

// clang-format off
#include <windows.h> /* must include this before GL/gl.h */
#include <GL/gl.h> /* OpenGL header file */
#include <GL/glu.h> /* OpenGL utilities header file */
#include "winapi/gl_loader.h"
// clang-format on

class Window {
public:
    Window();
    ~Window();
    void update();
    void getMessages();

private:
    HWND CreateOpenGLWindow(char* title, int x, int y, int width, int height, BYTE type, DWORD flags);
    static LONG WINAPI Window::WindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
    HDC hDC; // device context
    HGLRC hRC; // opengl context
    HWND hWnd; // window
    MSG msg; // message
};
