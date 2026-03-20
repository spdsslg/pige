#include <windows.h>
#include "resource.h"


HINSTANCE g_hInstance = NULL;
HWND g_hAboutDlg = NULL;

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
INT_PTR CALLBACK AboutDialogProc(HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam);

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR pCmdLine, int nCmdShow)
{
    g_hInstance = hInstance;
    const wchar_t CLASS_NAME[] = L"MyWindowClass";

    WNDCLASSW wc = { 0 };
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = CLASS_NAME;
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);

    wc.hIcon = (HICON)LoadImageW(hInstance, MAKEINTRESOURCEW(IDI_MYICON), IMAGE_ICON, 0, 0, LR_DEFAULTSIZE);

    RegisterClassW(&wc);

    HMENU hMenu = LoadMenuW(hInstance, MAKEINTRESOURCEW(IDR_MAINMENU));
 
    HWND hwnd = CreateWindowExW(
        0,
        CLASS_NAME,
        L"WinAPI Assignment Window",
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT, 400, 300,
        NULL,
        hMenu, //Attach the loaded menu
        hInstance,
        NULL
    );

    if (hwnd == NULL) return 0;

    ShowWindow(hwnd, nCmdShow);

    HACCEL hAccel = LoadAcceleratorsW(hInstance, MAKEINTRESOURCEW(IDR_ACCELERATOR));

    MSG msg = { 0 };

    while (GetMessageW(&msg, NULL, 0, 0))
    {
        if (g_hAboutDlg == NULL || !IsDialogMessageW(g_hAboutDlg, &msg)) {
            if (!TranslateAcceleratorW(hwnd, hAccel, &msg))
            {
                TranslateMessage(&msg);
                DispatchMessageW(&msg);
            }
        }
    }

    return 0;
}

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg)
    {
    case WM_COMMAND:
    {
        int wmId = LOWORD(wParam);
        switch (wmId)
        {
        case IDM_FILE_CLOSE:
            DestroyWindow(hwnd);
            break;

        case IDM_HELP_ABOUT:
            if (g_hAboutDlg == NULL) {
                g_hAboutDlg = CreateDialogW(g_hInstance, MAKEINTRESOURCEW(IDD_ABOUTBOX), hwnd, AboutDialogProc);
                ShowWindow(g_hAboutDlg, SW_SHOW);
            }
            else {
                ShowWindow(g_hAboutDlg, SW_SHOW);
                SetForegroundWindow(g_hAboutDlg);
            }
            break;
        }
        return 0;
    }

    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;
    }
    return DefWindowProcW(hwnd, uMsg, wParam, lParam);
}

INT_PTR CALLBACK AboutDialogProc(HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg)
    {
    case WM_INITDIALOG:
        return (INT_PTR)TRUE;

    case WM_COMMAND:
        //close the dialog if the user clicks "OK" or the top right 'X'
        if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
        {
            ShowWindow(hwndDlg, SW_HIDE);
            return (INT_PTR)TRUE;
        }
        break;
    case WM_CLOSE:
        ShowWindow(hwndDlg, SW_HIDE);
        return (INT_PTR)TRUE;
    }
    return (INT_PTR)FALSE;
}