#pragma once

// clang-format off
#include <windows.h>
// clang-format on
namespace ojgl {

class Window {
public:
    Window(unsigned window, unsigned height, bool fullScreen);
    ~Window();
    void getMessages();

private:
    HWND CreateOpenGLWindow(char* title, int x, int y, int width, int height, BYTE type, DWORD flags, bool fullScreen);
    HWND CreateFullscreenWindow(HWND, HINSTANCE);
    static LONG WINAPI Window::WindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

    HDC _hDC; // device context
    HGLRC _hRC; // opengl context
    HWND _hWnd; // window
    MSG _msg; // message
    unsigned _width;
    unsigned _height;
};
} //namespace ojgl