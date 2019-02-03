#include "Popup.h"
#include "utility/OJstd.h"
#include <utility/Log.h>
#include <windows.h>

#ifndef UNICODE
#define UNICODE
#endif

using namespace ojgl;

namespace {

struct CallbackData {
    int reslistindex = 0;
    bool full = false;
};

constexpr int PLAY = 1;
constexpr int QUIT = 2;
constexpr int FULL = 3;
constexpr int RESOLUTION = 4;

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    CallbackData* dataptr = reinterpret_cast<CallbackData*>(GetWindowLongPtr(hwnd, GWLP_USERDATA));

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
            dataptr->reslistindex = index;
        }

        if (LOWORD(wParam) == FULL) {
            if (IsDlgButtonChecked(hwnd, FULL)) {
                LOG_INFO("Fullscreen unchecked");
                CheckDlgButton(hwnd, FULL, BST_UNCHECKED);
                dataptr->full = false;
            } else {
                LOG_INFO("Fullscreen checked");
                CheckDlgButton(hwnd, FULL, BST_CHECKED);
                dataptr->full = true;
            }
        }
        break;
    }
    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}
}

popup::Data popup::show()
{
    static HINSTANCE hInstance = nullptr;

    hInstance = GetModuleHandle(NULL);

    // Register the window class.
    ojstd::string CLASS_NAME = "Sample Window Class";

    WNDCLASS wc = {};

    wc.lpfnWndProc = WindowProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = CLASS_NAME.c_str();

    RegisterClass(&wc);

    // Create the window.

    HWND hwnd = CreateWindowEx(0, CLASS_NAME.c_str(), "OJ",
        WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
        NULL, NULL, hInstance, NULL);

    if (hwnd == NULL) {
        LOG_ERROR("hwnd == NULL");
        exit(0);
    }

    CallbackData callbackData;
    if (!SetWindowLongPtr(hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(&callbackData))) {
        if (GetLastError() != 0) {
            LOG_ERROR("SetWindowLongPtr failed in popup");
        }
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
    popup::Data data;
    data.full = callbackData.full;
    data.width = resolutions[callbackData.reslistindex].first;
    data.height = resolutions[callbackData.reslistindex].second;

    return data;
}
