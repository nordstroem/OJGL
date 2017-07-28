#pragma once

// clang-format off
#include <windows.h> /* must include this before GL/gl.h */

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
