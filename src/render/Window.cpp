#include "Window.h"
#include "winapi/gl_loader.h"
#include <iostream>
#include <sstream>
#include <utility/Log.h>
#include <windows.h>

namespace ojgl {

class Window::Details {
public:
    Details(Vector2i size)
        : _size(size)
    {
    }
    HWND CreateOpenGLWindow(const char* title, int x, int y, BYTE type, DWORD flags, bool fullScreen);
    static LONG WINAPI WindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

    HDC _hDC = nullptr; // device context
    HGLRC _hRC = nullptr; // opengl context
    HWND _hWnd = nullptr; // window
    MSG _msg { 0 }; // message
    Vector2i _size;
    ojstd::vector<UINT> _keysPressed;
    ojstd::unordered_set<UINT> _keysDown;
    Vector2i _cursorPosition = { 0, 0 };
    bool _leftMouseButtonDown = false;
    bool _close = false;
};

Window::Window(Vector2i size, ojstd::string title, bool fullScreen, bool showCursor)
    : _priv(ojstd::make_unique<Details>(size))
{
    SetProcessDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE);
    ShowCursor(showCursor);

    if (fullScreen) {
        DEVMODE Mode;
        EnumDisplaySettings(NULL, ENUM_CURRENT_SETTINGS, &Mode);
        Mode.dmBitsPerPel = 32;
        Mode.dmPelsWidth = static_cast<DWORD>(size.x);
        Mode.dmPelsHeight = static_cast<DWORD>(size.y);
        Mode.dmSize = sizeof(Mode);
        Mode.dmFields = DM_PELSWIDTH | DM_PELSHEIGHT | DM_BITSPERPEL;
        ChangeDisplaySettings(&Mode, CDS_FULLSCREEN);
    }

    _priv->_hWnd = _priv->CreateOpenGLWindow(title.c_str(), 0, 0, PFD_TYPE_RGBA, 0, fullScreen);
    if (_priv->_hWnd == nullptr) {
        exit(1);
    }

    _priv->_hDC = GetDC(_priv->_hWnd);
    _priv->_hRC = wglCreateContext(_priv->_hDC);
    wglMakeCurrent(_priv->_hDC, _priv->_hRC);
    ShowWindow(_priv->_hWnd, 1);

    Window* pThis = this;
    SetLastError(0);
    if (!SetWindowLongPtr(_priv->_hWnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(pThis))) {
        if (GetLastError() != 0) {
            LOG_ERROR("SetWindowLongPtr failed in Window");
        }
    }

    load_gl_functions();
}

Window::~Window()
{
    wglMakeCurrent(nullptr, nullptr);
    ReleaseDC(_priv->_hWnd, _priv->_hDC);
    wglDeleteContext(_priv->_hRC);
    DestroyWindow(_priv->_hWnd);
}

void Window::getMessages()
{
    while (PeekMessage(&_priv->_msg, nullptr, 0, 0, PM_REMOVE)) {
        TranslateMessage(&_priv->_msg);
        DispatchMessage(&_priv->_msg);
    }
}

ojstd::vector<unsigned int> Window::getPressedKeys()
{
    auto keys = this->_priv->_keysPressed;
    this->_priv->_keysPressed.clear();
    return keys;
}

ojstd::unordered_set<unsigned int> Window::getDownKeys() const
{
    return this->_priv->_keysDown;
}

bool Window::isLeftMouseButtonDown() const
{
    return this->_priv->_leftMouseButtonDown;
}

bool Window::isClosePressed() const
{
    return _priv->_close;
}

Vector2i Window::getCursorPosition() const
{
    return _priv->_cursorPosition;
}

void Window::setTitle(const ojstd::string& newTitle)
{
    SetWindowTextA(_priv->_hWnd, newTitle.c_str());
}

HWND Window::Details::CreateOpenGLWindow(const char* title, int x, int y, BYTE type, DWORD flags, bool fullScreen)
{
    int pf;
    HDC hDC;
    HWND hWnd;
    WNDCLASS wc;
    PIXELFORMATDESCRIPTOR pfd;
    static HINSTANCE hInstance = nullptr;
    ojstd::string lpszClassName = "OpenGL";

    /* only register the window class once - use hInstance as a flag. */
    if (!hInstance) {
        hInstance = GetModuleHandle(NULL);
        wc.style = CS_OWNDC;
        wc.lpfnWndProc = (WNDPROC)WindowProc;
        wc.cbClsExtra = 0;
        wc.cbWndExtra = 0;
        wc.hInstance = hInstance;
        wc.hIcon = LoadIcon(NULL, IDI_WINLOGO);
        wc.hCursor = LoadCursor(NULL, IDC_ARROW);
        wc.hbrBackground = NULL;
        wc.lpszMenuName = NULL;
        wc.lpszClassName = lpszClassName.c_str();

        if (!RegisterClass(&wc)) {
            MessageBox(NULL, "RegisterClass() failed:  Cannot register window class.", "Error", MB_OK);
            return NULL;
        }
    }

    hWnd = CreateWindow(lpszClassName.c_str(), title, WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_OVERLAPPED | WS_MINIMIZEBOX | WS_SYSMENU,
        x, y, this->_size.x, this->_size.y, NULL, NULL, hInstance, NULL);

    if (fullScreen) {
        HMONITOR hmon = MonitorFromWindow(hWnd, MONITOR_DEFAULTTONEAREST);
        MONITORINFO mi = { sizeof(mi) };
        if (!GetMonitorInfo(hmon, &mi)) {
            LOG_INFO("Can not retrieve monitor info.");
        }
        hWnd = CreateWindow(lpszClassName.c_str(), title, WS_POPUP | WS_VISIBLE, mi.rcMonitor.left, mi.rcMonitor.top,
            mi.rcMonitor.right - mi.rcMonitor.left, mi.rcMonitor.bottom - mi.rcMonitor.top, hWnd, nullptr, hInstance, nullptr);
    }

    if (hWnd == NULL) {
        MessageBox(NULL, "CreateWindow() failed:  Cannot create a window.",
            "Error", MB_OK);
        return NULL;
    }

    hDC = GetDC(hWnd);

    /* there is no guarantee that the contents of the stack that become
			the pfd are zeroed, therefore _make sure_ to clear these bits. */
    memset(&pfd, 0, sizeof(pfd));
    pfd.nSize = sizeof(pfd);
    pfd.nVersion = 1;
    pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | flags;
    pfd.iPixelType = type;
    pfd.cColorBits = 32;

    pf = ChoosePixelFormat(hDC, &pfd);
    if (pf == 0) {
        MessageBox(NULL, "ChoosePixelFormat() failed:  "
                         "Cannot find a suitable pixel format.",
            "Error", MB_OK);
        return 0;
    }

    if (SetPixelFormat(hDC, pf, &pfd) == FALSE) {
        MessageBox(NULL, "SetPixelFormat() failed:  "
                         "Cannot set format specified.",
            "Error", MB_OK);
        return 0;
    }

    DescribePixelFormat(hDC, pf, sizeof(PIXELFORMATDESCRIPTOR), &pfd);

    ReleaseDC(hWnd, hDC);

    return hWnd;
}

LONG WINAPI Window::Details::WindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    Window* pThis = reinterpret_cast<Window*>(GetWindowLongPtr(hWnd, GWLP_USERDATA));

    if (pThis) {
        // Get the mouse position
        POINT p;
        GetCursorPos(&p);
        ScreenToClient(hWnd, &p);
        pThis->_priv->_cursorPosition = { p.x, p.y };
    }

    static PAINTSTRUCT ps;
    switch (uMsg) {
    case WM_PAINT:
        BeginPaint(hWnd, &ps);
        EndPaint(hWnd, &ps);
        return 0;

    case WM_SIZE:
        // glViewport(0, 0, LOWORD(lParam), HIWORD(lParam));
        PostMessage(hWnd, WM_PAINT, 0, 0);
        return 0;
    case WM_LBUTTONDOWN:
        if (pThis)
            pThis->_priv->_leftMouseButtonDown = true;
        return 0;
    case WM_LBUTTONUP:
        if (pThis)
            pThis->_priv->_leftMouseButtonDown = false;
        return 0;
    case WM_CHAR:
        switch (wParam) {
        case 27: /* ESC key */
            PostQuitMessage(0);
            break;
        }
        return 0;
    case WM_KEYUP:
        if (pThis) {
            UINT key = static_cast<UINT>(wParam);
            pThis->_priv->_keysPressed.push_back(key);
            pThis->_priv->_keysDown.erase(key);
        }
        return 0;
    case WM_KEYDOWN:
        if (pThis) {
            UINT key = static_cast<UINT>(wParam);
            pThis->_priv->_keysDown.insert(key);
        }
        return 0;
    case WM_CLOSE:
        if (pThis) {
            pThis->_priv->_close = true;
        }
        return 0;
    }

    return DefWindowProc(hWnd, uMsg, wParam, lParam);
}

Vector2i Window::size() const
{
    return _priv->_size;
}

void* Window::hdcBackend() const
{
    return _priv->_hDC;
}

} // namespace ojgl
