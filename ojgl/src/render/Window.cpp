#include "Window.h"
#include <utility/Log.h>
#include <windows.h>

namespace ojgl {

class Window::Details {
public:
    Details(unsigned width, unsigned height)
        : _width(width)
        , _height(height)
    {
    }
    HWND CreateOpenGLWindow(const char* title, int x, int y, BYTE type, DWORD flags, bool fullScreen);
    static LONG WINAPI WindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

    HDC _hDC; // device context
    HGLRC _hRC; // opengl context
    HWND _hWnd; // window
    MSG _msg; // message
    unsigned _width;
    unsigned _height;
    ojstd::vector<UINT> _keys;
    bool _close = false;
};

#ifndef UNICODE
#define UNICODE
#endif

#include <windows.h>

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

constexpr int PLAY = 1;
constexpr int QUIT = 2;
constexpr int FULL = 3;
constexpr int RESOLUTION = 4;

int WINAPI wWinMain(HINSTANCE hInstance)
{
    // Register the window class.
    ojstd::string CLASS_NAME = "Sample Window Class";

    WNDCLASS wc = {};

    wc.lpfnWndProc = WindowProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = CLASS_NAME.c_str();

    RegisterClass(&wc);

    // Create the window.

    HWND hwnd = CreateWindowEx(0, CLASS_NAME.c_str(), "Learn to Program Windows",
        WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
        NULL, NULL, hInstance, NULL);

    if (hwnd == NULL) {
        return 0;
    }

    DEVMODE dm = { 0 };
    dm.dmSize = sizeof(dm);
    ojstd::vector<ojstd::Pair<int, int>> resolutions;
    ojstd::unordered_set<ojstd::Pair<int, int>> checkedResolutions;

    for (int iModeNum = 0; EnumDisplaySettings(NULL, iModeNum, &dm) != 0; iModeNum++) {
        ojstd::Pair<int, int> res(dm.dmPelsWidth, dm.dmPelsHeight);
        if (checkedResolutions.find(res) == checkedResolutions.end()) {
            resolutions.push_back(res);
            checkedResolutions.insert(res);
        }
        LOG_INFO("Mode #" << iModeNum << " = " << dm.dmPelsWidth << "x" << dm.dmPelsHeight << " dmBitsPerPel " << dm.dmBitsPerPel << " dmDisplayFrequency " << dm.dmDisplayFrequency);
    }

    for (int i = 0; i < resolutions.size(); i++) {
        LOG_INFO("Res " << resolutions[i].first << "x" << resolutions[i].second);
    }

    //HWND checkbox = CreateWindowEx(NULL, CLASS_NAME.c_str(), "Checkbox", BS_CHECKBOX, 50, 50, 50, 50, hwnd, NULL, NULL, NULL);

    CreateWindow(TEXT("button"), TEXT("Play"),
        WS_VISIBLE | WS_CHILD,
        20, 50, 80, 25,
        hwnd, (HMENU)PLAY, NULL, NULL);

    CreateWindow(TEXT("button"), TEXT("Quit"),
        WS_VISIBLE | WS_CHILD,
        120, 50, 80, 25,
        hwnd, (HMENU)QUIT, NULL, NULL);

    CreateWindow(TEXT("BUTTON"), "Fullscreen", BS_CHECKBOX | WS_VISIBLE | WS_CHILD, 250, 50, 150, 150, hwnd, (HMENU)FULL, NULL, NULL);

    HWND hWndComboBox = CreateWindow("ComboBox", TEXT(""),
        CBS_DROPDOWN | CBS_HASSTRINGS | WS_CHILD | WS_OVERLAPPED | WS_VISIBLE | WS_VSCROLL,
        400, 50, 200, 200, hwnd, (HMENU)RESOLUTION, NULL, // HINST_THISCOMPONENT
        NULL);

    for (int i = 0; i < resolutions.size(); ++i) {
        ojstd::string res;
        res.append(ojstd::to_string(resolutions[i].first));
        res.append("x");
        res.append(ojstd::to_string(resolutions[i].second));
        // Add string to combobox.
        SendMessage(hWndComboBox, (UINT)CB_ADDSTRING, (WPARAM)0, (LPARAM)res.c_str());
    }

    // Send the CB_SETCURSEL message to display an initial item
    //  in the selection field
    SendMessage(hWndComboBox, CB_SETCURSEL, (WPARAM)2, (LPARAM)0);

    ShowWindow(hwnd, 1);

    // Run the message loop.

    MSG msg = {};
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return 0;
}

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg) {
    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;
    case WM_CREATE: {

        break;
    }
    case WM_PAINT: {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hwnd, &ps);

        FillRect(hdc, &ps.rcPaint, (HBRUSH)(COLOR_WINDOW + 1));
        EndPaint(hwnd, &ps);
        break;
    }
    case WM_CLOSE:
        DestroyWindow(hwnd);
        //PostQuitMessage(0);
        return 0;
    case WM_COMMAND:
        if (HIWORD(wParam) == BN_CLICKED && LOWORD(wParam) == PLAY) {
            LOG_INFO("Play button clicked");
            DestroyWindow(hwnd);
        }

        if (HIWORD(wParam) == CBN_SELCHANGE && LOWORD(wParam) == RESOLUTION) {
            int index = SendMessage((HWND)lParam, (UINT)CB_GETCURSEL, (WPARAM)0, (LPARAM)0);
            LOG_INFO("Selected list item " << index);
        }

        if (LOWORD(wParam) == FULL) {
            if (IsDlgButtonChecked(hwnd, FULL)) {
                LOG_INFO("Fullscreen unchecked");
                CheckDlgButton(hwnd, FULL, BST_UNCHECKED);
            } else {
                LOG_INFO("Fullscreen checked");
                CheckDlgButton(hwnd, FULL, BST_CHECKED);
            }
        }
        break;
    }
    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

Window::Window()
{
    static HINSTANCE hInstance = nullptr;

    hInstance = GetModuleHandle(NULL);

    wWinMain(hInstance);
}

Window::Window(unsigned width, unsigned height, ojstd::string title, bool fullScreen, bool showCursor)
    : _priv(ojstd::make_shared<Details>(width, height))
{
    ShowCursor(showCursor);

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
    auto keys = this->_priv->_keys;
    this->_priv->_keys.clear();
    return keys;
}

bool Window::isClosePressed() const
{
    return _priv->_close;
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

    hWnd = CreateWindow(lpszClassName.c_str(), title, WS_OVERLAPPEDWINDOW | WS_CLIPSIBLINGS | WS_CLIPCHILDREN,
        x, y, this->_width, this->_height, NULL, NULL, hInstance, NULL);

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

    case WM_CHAR:
        switch (wParam) {
        case 27: /* ESC key */
            PostQuitMessage(0);
            break;
        }
        return 0;
    case WM_KEYUP:
        if (pThis) {
            pThis->_priv->_keys.push_back(wParam);
        }
        return 0;
    case WM_CLOSE:
        pThis->_priv->_close = true;
        return 0;
    }

    return DefWindowProc(hWnd, uMsg, wParam, lParam);
}
} // namespace ojgl
