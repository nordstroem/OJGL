#pragma once

// clang-format off
#include <windows.h>
#include "../utility/OJstd.h"
// clang-format on
namespace ojgl {

class Window {
public:
    Window(unsigned width, unsigned height, bool fullScreen);
    ~Window();
    void getMessages();
    ojstd::vector<UINT> getPressedKeys();
    static constexpr int KEY_LEFT = 37;
    static constexpr int KEY_UP = 38;
    static constexpr int KEY_RIGHT = 39;
    static constexpr int KEY_DOWN = 40;
    static constexpr int KEY_ESCAPE = 27;
    static constexpr int KEY_SPACE = 32;
    static constexpr int KEY_R = 82;
    static constexpr int KEY_F1 = 112;

private:
    HWND CreateOpenGLWindow(const char* title, int x, int y, BYTE type, DWORD flags, bool fullScreen);
    HWND CreateFullscreenWindow(HWND hwnd, HINSTANCE hInstance);
    static LONG WINAPI WindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

    HDC _hDC; // device context
    HGLRC _hRC; // opengl context
    HWND _hWnd; // window
    MSG _msg; // message
    unsigned _width;
    unsigned _height;
    ojstd::vector<UINT> _keys;
    bool _closed;
};
} //namespace ojgl
