#define UNICODE
#define _UNICODE
#include <windows.h>

constexpr int TILE_SIZE = 60;
constexpr int TILE_GAP = 10;
constexpr int BOARD_DIM = 4;
constexpr int BOARD_MARGIN = 10;

constexpr COLORREF CLR_WINDOW_BG = RGB(250, 247, 238);
constexpr COLORREF CLR_TILE_BG = RGB(250, 192, 174);

static HBRUSH g_windowBrush = nullptr;
static HBRUSH g_tileBrush = nullptr;

HWND g_tiles[BOARD_DIM * BOARD_DIM]{};

int g_windowCount = 0;

// --- Stage 3 globals ---
static HWND g_hwndMain = nullptr;
static HWND g_hwndSecond = nullptr;
static bool g_syncing = false;

int ClientWidth()
{
    return BOARD_MARGIN + BOARD_DIM * TILE_SIZE + (BOARD_DIM - 1) * TILE_GAP + BOARD_MARGIN;
}
int ClientHeight() { return ClientWidth(); }

void CreateTiles(HWND hwnd)
{
    for (int r = 0; r < BOARD_DIM; ++r)
    {
        for (int c = 0; c < BOARD_DIM; ++c)
        {
            const int x = BOARD_MARGIN + c * (TILE_SIZE + TILE_GAP);
            const int y = BOARD_MARGIN + r * (TILE_SIZE + TILE_GAP);
            const int idx = r * BOARD_DIM + c;

            g_tiles[idx] = CreateWindowExW(
                0, L"STATIC", L"",
                WS_CHILD | WS_VISIBLE | SS_CENTER | SS_CENTERIMAGE,
                x, y, TILE_SIZE, TILE_SIZE,
                hwnd, nullptr,
                (HINSTANCE)GetWindowLongPtrW(hwnd, GWLP_HINSTANCE),
                nullptr
            );
        }
    }
}

void SetWindowIconsFromIco(HWND hwnd, const wchar_t* icoPath)
{
    HICON hBig = (HICON)LoadImageW(nullptr, icoPath, IMAGE_ICON, 32, 32, LR_LOADFROMFILE);
    HICON hSm = (HICON)LoadImageW(nullptr, icoPath, IMAGE_ICON, 16, 16, LR_LOADFROMFILE);
    if (hBig) SendMessageW(hwnd, WM_SETICON, ICON_BIG, (LPARAM)hBig);
    if (hSm)  SendMessageW(hwnd, WM_SETICON, ICON_SMALL, (LPARAM)hSm);
}

// --- Stage 3 helper: mirror movement ---
static void MirrorOtherWindow(HWND movedHwnd)
{
    if (g_syncing) return;
    if (!g_hwndMain || !g_hwndSecond) return;

    HWND other = (movedHwnd == g_hwndMain) ? g_hwndSecond : g_hwndMain;
    if (!other) return;

    RECT rMoved{};
    if (!GetWindowRect(movedHwnd, &rMoved)) return;

    const int w = rMoved.right - rMoved.left;
    const int h = rMoved.bottom - rMoved.top;

    // Center of moved window (in screen coords)
    const int cx = rMoved.left + w / 2;
    const int cy = rMoved.top + h / 2;

    // Screen center
    const int screenW = GetSystemMetrics(SM_CXSCREEN);
    const int screenH = GetSystemMetrics(SM_CYSCREEN);
    const int scx = screenW / 2;
    const int scy = screenH / 2;

    // Mirror around screen center: C2 = 2S - C1
    const int otherCx = 2 * scx - cx;
    const int otherCy = 2 * scy - cy;

    // Convert center back to top-left
    const int otherX = otherCx - w / 2;
    const int otherY = otherCy - h / 2;

    g_syncing = true;
    SetWindowPos(other, nullptr, otherX, otherY, 0, 0,
        SWP_NOZORDER | SWP_NOSIZE | SWP_NOACTIVATE);
    g_syncing = false;
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch (msg)
    {
    case WM_CREATE:
    {
        ++g_windowCount;

        if (!g_windowBrush) g_windowBrush = CreateSolidBrush(CLR_WINDOW_BG);
        if (!g_tileBrush)   g_tileBrush = CreateSolidBrush(CLR_TILE_BG);

        CreateTiles(hwnd);

        return 0;
    }

    case WM_CTLCOLORSTATIC:
    {
        HDC hdc = (HDC)wParam;
        SetBkColor(hdc, CLR_TILE_BG);
        SetTextColor(hdc, RGB(0, 0, 0));
        return (LRESULT)g_tileBrush;
    }

    case WM_ERASEBKGND:
    {
        HDC hdc = (HDC)wParam;
        RECT rc;
        GetClientRect(hwnd, &rc);
        FillRect(hdc, &rc, g_windowBrush);
        return 1;
    }

    // --- Stage 3: sync moves ---
    case WM_WINDOWPOSCHANGED:
    {
        // lParam points to WINDOWPOS, but easiest is: if position changed, mirror
        const WINDOWPOS* wp = reinterpret_cast<const WINDOWPOS*>(lParam);
        if (wp && (wp->flags & SWP_NOMOVE) == 0)
        {
            // Only mirror moves of our two main windows, not child STATICs
            if (hwnd == g_hwndMain || hwnd == g_hwndSecond)
                MirrorOtherWindow(hwnd);
        }
        break; // still call DefWindowProc afterwards
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
    wc.hbrBackground = nullptr;

    if (!RegisterClassExW(&wc))
        return 0;

    DWORD mainStyle = WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX;
    DWORD exStyle = 0;

    RECT rMain{ 0, 0, ClientWidth(), ClientHeight() };
    AdjustWindowRectEx(&rMain, mainStyle, FALSE, exStyle);
    const int winW = rMain.right - rMain.left;
    const int winH = rMain.bottom - rMain.top;

    g_hwndMain = CreateWindowExW(
        exStyle,
        CLASS_NAME,
        L"2048",
        mainStyle,
        CW_USEDEFAULT, CW_USEDEFAULT,
        winW, winH,
        nullptr, nullptr,
        hInstance,
        nullptr
    );
    if (!g_hwndMain) return 0;
    SetWindowIconsFromIco(g_hwndMain, L"2048_icon.ico");

    // Popup style: caption only (no icon, no close button)
    DWORD popupStyle = WS_OVERLAPPED | WS_CAPTION;

    // Important: recalc size for popupStyle too (caption thickness differs slightly)
    RECT rPopup{ 0, 0, ClientWidth(), ClientHeight() };
    AdjustWindowRectEx(&rPopup, popupStyle, FALSE, 0);
    const int popupW = rPopup.right - rPopup.left;
    const int popupH = rPopup.bottom - rPopup.top;

    g_hwndSecond = CreateWindowExW(
        0,
        CLASS_NAME,
        L"2048",
        popupStyle,
        100, 100,
        popupW, popupH,
        g_hwndMain,     // owner => no taskbar entry
        nullptr,
        hInstance,
        nullptr
    );

    ShowWindow(g_hwndMain, nCmdShow);
    UpdateWindow(g_hwndMain);

    if (g_hwndSecond)
    {
        // Show without activating so focus stays on main
        ShowWindow(g_hwndSecond, SW_SHOWNA);
        UpdateWindow(g_hwndSecond);
    }

    MSG msg{};
    while (GetMessageW(&msg, nullptr, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessageW(&msg);
    }
    return (int)msg.wParam;
}