#define UNICODE
#define _UNICODE
#include <windows.h>
#include <array>

static constexpr int TILE_SIZE = 60;
static constexpr int TILE_GAP = 10;   // gap between tiles
static constexpr int BOARD_DIM = 4;
static constexpr int BOARD_MARGIN = 10;  // around the grid inside the window client area


// Colors from spec
static constexpr COLORREF CLR_WINDOW_BG = RGB(250, 247, 238);
static constexpr COLORREF CLR_TILE_BG = RGB(250, 192, 174);

static HBRUSH g_windowBrush = nullptr;
static HBRUSH g_tileBrush = nullptr;

HWND g_tiles[BOARD_DIM*BOARD_DIM]{};

static int g_windowCount = 0;

static int ClientWidth()
{
    // Margin + 4 tiles + 3 gaps + margin
    return BOARD_MARGIN + BOARD_DIM * TILE_SIZE + (BOARD_DIM - 1) * TILE_GAP + BOARD_MARGIN;
}

static int ClientHeight()
{
    return ClientWidth(); // square board
}

static void CreateTiles(HWND hwnd)
{
    // Create 16 STATIC controls, positioned in a 4x4 grid
    for (int r = 0; r < BOARD_DIM; ++r)
    {
        for (int c = 0; c < BOARD_DIM; ++c)
        {
            const int x = BOARD_MARGIN + c * (TILE_SIZE + TILE_GAP);
            const int y = BOARD_MARGIN + r * (TILE_SIZE + TILE_GAP);

            const int idx = r * BOARD_DIM + c;

            g_tiles[idx] = CreateWindowExW(
                0,
                L"STATIC",
                L"", // later we can put numbers here
                WS_CHILD | WS_VISIBLE | SS_CENTER | SS_CENTERIMAGE,
                x, y, TILE_SIZE, TILE_SIZE,
                hwnd,
                nullptr,
                (HINSTANCE)GetWindowLongPtrW(hwnd, GWLP_HINSTANCE),
                nullptr
            );
        }
    }
}

static void SetWindowIconsFromIco(HWND hwnd, const wchar_t* icoPath)
{
    // Load big + small icons from file (your 2048_icon.ico)
    HICON hBig = (HICON)LoadImageW(nullptr, icoPath, IMAGE_ICON, 32, 32, LR_LOADFROMFILE);
    HICON hSm = (HICON)LoadImageW(nullptr, icoPath, IMAGE_ICON, 16, 16, LR_LOADFROMFILE);

    if (hBig) SendMessageW(hwnd, WM_SETICON, ICON_BIG, (LPARAM)hBig);
    if (hSm)  SendMessageW(hwnd, WM_SETICON, ICON_SMALL, (LPARAM)hSm);

    // Note: we don't DestroyIcon here yet; Windows typically owns icons after WM_SETICON.
    // We'll keep this simple for Stage 1.
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch (msg)
    {
    case WM_CREATE:
    {
        g_windowCount++;
        // Create brushes once window is created
        if (!g_windowBrush) g_windowBrush = CreateSolidBrush(CLR_WINDOW_BG);
        if (!g_tileBrush)   g_tileBrush = CreateSolidBrush(CLR_TILE_BG);

        CreateTiles(hwnd);

        // Try to set icon from file in the same folder as exe:
        SetWindowIconsFromIco(hwnd, L"2048_icon.ico");
        return 0;
    }

    case WM_CTLCOLORSTATIC:
    {
        // This message lets us color STATIC controls without custom painting.
        HDC hdc = (HDC)wParam;
        SetBkColor(hdc, CLR_TILE_BG);

        // If you later show text numbers, black is fine for now:
        SetTextColor(hdc, RGB(0, 0, 0));

        // Return the brush used to paint the background of STATIC controls
        return (LRESULT)g_tileBrush;
    }

    case WM_ERASEBKGND:
    {
        // Paint the background with our window brush to avoid default gray
        HDC hdc = (HDC)wParam;
        RECT rc;
        GetClientRect(hwnd, &rc);
        FillRect(hdc, &rc, g_windowBrush);
        return 1; // we erased it
    }

    case WM_DESTROY:
    {
        --g_windowCount;

        if (g_windowCount == 0)
        {
            if (g_tileBrush) { DeleteObject(g_tileBrush);   g_tileBrush = nullptr; }
            if (g_windowBrush) { DeleteObject(g_windowBrush); g_windowBrush = nullptr; }
            PostQuitMessage(0);
        }
        return 0;
    }
    }
    return DefWindowProcW(hwnd, msg, wParam, lParam);
}

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE, PWSTR, int nCmdShow)
{
    const wchar_t* CLASS_NAME = L"WinAPI_2048_Lab";

    WNDCLASSEXW wc{};
    wc.cbSize = sizeof(wc);
    wc.lpfnWndProc = WndProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = CLASS_NAME;
    wc.hCursor = LoadCursorW(nullptr, IDC_ARROW);

    // Background brush is handled by WM_ERASEBKGND, so we can leave this null.
    // But it’s also ok to set it; we keep it null to avoid double painting.
    wc.hbrBackground = nullptr;

    if (!RegisterClassExW(&wc))
        return 0;

    // We want a fixed-size window: no resizing, no maximize.
    DWORD style = WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX;
    DWORD exStyle = 0;

    RECT r{ 0, 0, ClientWidth(), ClientHeight() };
    AdjustWindowRectEx(&r, style, FALSE, exStyle);

    const int winW = r.right - r.left;
    const int winH = r.bottom - r.top;

    HWND hwndMain = CreateWindowExW(
        exStyle,
        CLASS_NAME,
        L"2048",
        style,
        CW_USEDEFAULT, CW_USEDEFAULT,
        winW, winH,
        nullptr, nullptr,
        hInstance,
        nullptr
    );

    if (!hwndMain) return 0;

    // Create the second window as an OWNED window of the first.
    // Owned windows typically do NOT show in the taskbar.
    HWND hwndSecond = CreateWindowExW(
        0,              // if needed later: WS_EX_TOOLWINDOW
        CLASS_NAME,
        L"2048",
        WS_OVERLAPPED | WS_CAPTION ,
        100, 100,       // you can choose a position; we'll refine later
        winW, winH,
        hwndMain,       // <-- OWNER (important! not parent-child, but "owner")
        nullptr,
        hInstance,
        nullptr
    );

    ShowWindow(hwndMain, nCmdShow);
    UpdateWindow(hwndMain);

    if (hwndSecond)
    {
        ShowWindow(hwndSecond, nCmdShow);
        UpdateWindow(hwndSecond);
    }


    MSG msg{};
    while (GetMessageW(&msg, nullptr, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessageW(&msg);
    }
    return (int)msg.wParam;
}