#define UNICODE
#define _UNICODE
#include <windows.h>

constexpr wchar_t CLASS_NAME[] = L"MainWindowClass";
constexpr wchar_t WINDOW_TITLE[] = L"My WinAPI App";

constexpr int C_WIDTH = 800;
constexpr int C_HEIGHT = 600;

HWND main_window = nullptr;
HBRUSH bg_brush = nullptr;

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch (msg)
    {
    case WM_KEYDOWN:
        //
        return 0;

    case WM_TIMER:
        //
        return 0;

    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;
    }

    return DefWindowProc(hwnd, msg, wParam, lParam);
}

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE, PWSTR, int nCmdShow)
{
    bg_brush = CreateSolidBrush(RGB(240, 240, 240));

    WNDCLASS desc = {};
    desc.lpfnWndProc = WndProc;
    desc.hInstance = hInstance;
    desc.lpszClassName = CLASS_NAME;
    desc.hbrBackground = bg_brush;
    desc.hCursor = LoadCursor(nullptr, IDC_ARROW);

    RegisterClass(&desc);

    DWORD style = WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX;

    RECT r = { 0, 0, C_WIDTH, C_HEIGHT };
    AdjustWindowRect(&r, style, FALSE);

    HWND hwnd = CreateWindowEx(
        0,
        CLASS_NAME,
        WINDOW_TITLE,
        style,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        r.right - r.left,
        r.bottom - r.top,
        nullptr,
        nullptr,
        hInstance,
        nullptr
    );

    if (!hwnd)
    {
        DeleteObject(bg_brush);
        return 0;
    }

    main_window = hwnd;

    ShowWindow(hwnd, nCmdShow);
    UpdateWindow(hwnd);

    MSG msg = {};
    while (GetMessage(&msg, nullptr, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    DeleteObject(bg_brush);
    return static_cast<int>(msg.wParam);
}