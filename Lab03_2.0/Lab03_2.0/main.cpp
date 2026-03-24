#include <windows.h>
#include "resource.h"
#include <string>

#define IDC_BTN_0 1000
#define IDC_BTN_1 1001
#define IDC_BTN_2 1002
#define IDC_BTN_3 1003
#define IDC_BTN_4 1004
#define IDC_BTN_5 1005
#define IDC_BTN_6 1006
#define IDC_BTN_7 1007
#define IDC_BTN_8 1008
#define IDC_BTN_9 1009
#define IDC_BTN_ADD 1010
#define IDC_BTN_SUB 1011
#define IDC_BTN_MUL 1012
#define IDC_BTN_DIV 1013
#define IDC_BTN_EQ  1014

struct ButtonDef {
    int id;
    LPCWSTR text;
    int col, row;
    int colSpan;
};

const int NUM_BUTTONS = 15;
ButtonDef btnDefs[NUM_BUTTONS] = {
    {IDC_BTN_7, L"7", 0, 0, 1}, {IDC_BTN_8, L"8", 1,0,1}, {IDC_BTN_9, L"9", 2,0,1},{IDC_BTN_DIV, L"/", 3,0,1},
    {IDC_BTN_4, L"4", 0,1,1}, {IDC_BTN_5, L"5", 1,1,1}, {IDC_BTN_6, L"6", 2,1,1}, {IDC_BTN_MUL, L"*", 3,1,1},
    {IDC_BTN_1, L"1", 0,2,1}, {IDC_BTN_2, L"2", 1,2,1}, {IDC_BTN_3, L"3", 2,2,1}, {IDC_BTN_SUB, L"-",3,2,1},
    {IDC_BTN_0, L"0", 0,3,1}, {IDC_BTN_ADD, L"+", 1,3,1}, {IDC_BTN_EQ, L"=", 2,3,2}
};
HWND hButtons[NUM_BUTTONS];

constexpr wchar_t CLASS_NAME[] = L"DevCalcClass";
const int MIN_WIDTH = 320;
const int MIN_HEIGHT = 400;

const wchar_t DISPLAY_CLASS_NAME[] = L"DisplayCtrlClass";
HWND hDisplay = NULL;
HFONT hFontHistory = NULL;
HFONT hFontResult = NULL;

std::wstring currentInput = L"0";
std::wstring historyText = L"";
double previousValue = 0.0;
int currentOperator = 0;
bool isNewInput = true;

LRESULT CALLBACK WndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK DisplayWndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE, PWSTR, int nCmdShow) {
    WNDCLASSEXW desc = { 0 };
    desc.cbSize = sizeof(WNDCLASSEXW);
    desc.lpfnWndProc = WndProc;
    desc.hInstance = hInstance;
    desc.lpszClassName = CLASS_NAME;
    desc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    desc.hCursor = LoadCursor(nullptr, IDC_ARROW);

    desc.hIcon = LoadIcon(hInstance, MAKEINTRESOURCEW(IDI_MYICON));
    desc.hIconSm = LoadIcon(hInstance, MAKEINTRESOURCEW(IDI_MYICON));
    desc.lpszMenuName = MAKEINTRESOURCE(IDR_MAINMENU);

    RegisterClassExW(&desc);


    WNDCLASSEXW dwc = { 0 };
    dwc.cbSize = sizeof(tagWNDCLASSEXW);
    dwc.style = CS_HREDRAW | CS_VREDRAW;
    dwc.lpfnWndProc = DisplayWndProc;
    dwc.hInstance = hInstance;
    dwc.hCursor = LoadCursor(NULL, IDC_ARROW);
    dwc.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
    dwc.lpszClassName = DISPLAY_CLASS_NAME;

    RegisterClassExW(&dwc);

    HWND hwnd = CreateWindowEx(
        0,
        CLASS_NAME,
        L"DevCalculator",
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        MIN_WIDTH,
        MIN_HEIGHT,
        nullptr,
        nullptr,
        hInstance,
        nullptr
    );
    if (hwnd == NULL) return 0;

    ShowWindow(hwnd, nCmdShow);
    UpdateWindow(hwnd);

    HACCEL hAccel = LoadAcceleratorsW(hInstance, MAKEINTRESOURCEW(IDA_MAINACCEL));
    if (hAccel == NULL) {
        MessageBoxW(hwnd, L"Failed to load accelerator", L"Debug", MB_OK | MB_ICONERROR);
    }

    MSG msg = { 0 };
    while (GetMessage(&msg, nullptr, 0, 0))
    {
        if (!TranslateAcceleratorW(hwnd, hAccel, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessageW(&msg);
        }
    }

    return (int)msg.wParam;
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    switch (uMsg) {
    case WM_CREATE:
    {
        hDisplay = CreateWindowExW(
            0, DISPLAY_CLASS_NAME, NULL,
            WS_CHILD | WS_VISIBLE, //child of the main window
            0, 0, 0, 0,            
            hwnd, NULL, (HINSTANCE)GetWindowLongPtr(hwnd, GWLP_HINSTANCE), NULL
        );

        //buttons
        HFONT hDefaultFont = (HFONT)GetStockObject(DEFAULT_GUI_FONT);
        for (int i = 0; i < NUM_BUTTONS; i++) {
            hButtons[i] = CreateWindowExW(
                0, L"Button",
                btnDefs[i].text,
                WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
                0,
                0,
                0,
                0,
                hwnd,
                (HMENU)(UINT_PTR)btnDefs[i].id,
                (HINSTANCE)GetWindowLongPtr(hwnd, GWLP_HINSTANCE),
                nullptr
            );
            SendMessageW(hButtons[i], WM_SETFONT, (WPARAM)hDefaultFont, MAKELPARAM(FALSE, 0));
        }

        return 0;
    }

    case WM_SIZE:
    {
        int width = LOWORD(lParam);
        int height = HIWORD(lParam);

        int padding = 5;
        int displayHeight = (height / 4) - padding;

        //button grid
        int startY = padding + displayHeight + padding;
        int gridHeight = height - startY - padding;
        int gridWidth = width - (padding * 2);

        int btnWidth = (gridWidth - (padding * 3)) / 4;
        int btnHeight = (gridHeight - (padding * 3)) / 4;

        HDWP hdwp = BeginDeferWindowPos(1 + NUM_BUTTONS);//movement of display and 15 buttons

        if (hDisplay) {
            hdwp = DeferWindowPos(hdwp, hDisplay, NULL, padding, padding,
                gridWidth, displayHeight, SWP_NOZORDER);
        }

        for (int i = 0; i < NUM_BUTTONS; i++) {
            if (hButtons[i]) {
                int x = padding + (btnDefs[i].col * (btnWidth + padding));
                int y = startY + (btnDefs[i].row * (btnHeight + padding));

                int w = (btnWidth * btnDefs[i].colSpan) + (padding * (btnDefs[i].colSpan - 1));//equals sign absorbs one padding
                hdwp = DeferWindowPos(hdwp, hButtons[i], NULL, x, y, w, btnHeight, SWP_NOZORDER);
            }
        }

        EndDeferWindowPos(hdwp);

        return 0;
    }

    case WM_GETMINMAXINFO: {
        MINMAXINFO* pMinMax = (MINMAXINFO*)lParam;
        pMinMax->ptMinTrackSize.x = MIN_WIDTH;
        pMinMax->ptMinTrackSize.y = MIN_HEIGHT;
        return 0;
    }
    case WM_CHAR:
    {
        wchar_t ch = (wchar_t)wParam;
        if (ch >= L'0' && ch <= L'9') {
            SendMessage(hwnd, WM_COMMAND, IDC_BTN_0 + (ch - L'0'), 0);
        }
        else if (ch == L'+') SendMessage(hwnd, WM_COMMAND, IDC_BTN_ADD, 0);
        else if (ch == L'-') SendMessage(hwnd, WM_COMMAND, IDC_BTN_SUB, 0);
        else if (ch == L'*') SendMessage(hwnd, WM_COMMAND, IDC_BTN_MUL, 0);
        else if (ch == L'/') SendMessage(hwnd, WM_COMMAND, IDC_BTN_DIV, 0);
        else if (ch == L'=' || ch == VK_RETURN) SendMessage(hwnd, WM_COMMAND, IDC_BTN_EQ, 0);
        else if (ch == VK_BACK) {
            if (!isNewInput && currentInput.length() > 0) {
                currentInput.pop_back();
                if (currentInput.empty()) {
                    currentInput = L"0";
                    isNewInput = true;
                }
                InvalidateRect(hDisplay, NULL, TRUE);
            }
        }
        return 0;
    }

    case WM_COMMAND:
    {
        int wmId = LOWORD(wParam);
        SetFocus(hwnd);
        if (wmId == IDM_MODE_PROGRAMMER) {
            MessageBoxW(hwnd, L"Not implemented yet", L"DevCalculator", MB_OK | MB_ICONINFORMATION);
        }
        else if (wmId == IDM_EDIT_CLEAR) { //clear
            currentInput = L"0";
            historyText = L"";
            previousValue = 0.0;
            currentOperator = 0;
            isNewInput = true;
            InvalidateRect(hDisplay, NULL, TRUE);
        }
        else if (wmId >= IDC_BTN_0 && wmId <= IDC_BTN_9) {
            int digit = wmId - IDC_BTN_0;
            if (isNewInput || currentInput == L"0") {
                currentInput = std::to_wstring(digit);
                isNewInput = false;
            }
            else {
                if (currentInput != L"0") currentInput += std::to_wstring(digit);
            }
            InvalidateRect(hDisplay, NULL, TRUE);
        }
        else if (wmId >= IDC_BTN_ADD && wmId <= IDC_BTN_DIV) {
            previousValue = std::stod(currentInput); //converts text to double
            currentOperator = wmId;
            isNewInput = true;

            wchar_t opChar = L' ';
            if (wmId == IDC_BTN_ADD) opChar = L'+';
            if (wmId == IDC_BTN_SUB) opChar = L'-';
            if (wmId == IDC_BTN_MUL) opChar = L'*';
            if (wmId == IDC_BTN_DIV) opChar = L'/';

            historyText = currentInput + L" " + opChar;
            InvalidateRect(hDisplay, NULL, TRUE);
        }
        else if (wmId == IDC_BTN_EQ) {
            if (currentOperator != 0) {
                double currentValue = std::stod(currentInput);
                double result = 0.0;

                if (currentOperator == IDC_BTN_ADD) result = previousValue + currentValue;
                if (currentOperator == IDC_BTN_SUB) result = previousValue - currentValue;
                if (currentOperator == IDC_BTN_MUL) result = previousValue * currentValue;
                if (currentOperator == IDC_BTN_DIV && currentValue != 0) result = previousValue / currentValue;

                //removes trailing zeros
                wchar_t buf[64];
                swprintf(buf, 64, L"%g", result);

                historyText += L" " + currentInput + L" =";
                currentInput = buf;
                currentOperator = 0;
                isNewInput = true;
                InvalidateRect(hDisplay, NULL, TRUE);
            }
        }
 
        return 0;
    }
    case WM_DESTROY: {
        PostQuitMessage(0);
        return 0;
    }
    }

    return DefWindowProcW(hwnd, uMsg, wParam, lParam);
}

LRESULT CALLBACK DisplayWndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    switch (uMsg) {
        case WM_SIZE:
        {
            int height = HIWORD(lParam);
            if (hFontHistory) DeleteObject(hFontHistory);
            if (hFontResult) DeleteObject(hFontResult);

            int histFontSize = height / 3;
            int resultFontSize = height / 2;

            hFontHistory = CreateFontW(histFontSize, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE,
                DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
                CLEARTYPE_QUALITY, DEFAULT_PITCH | FF_SWISS, L"Segoe UI");

            hFontResult = CreateFontW(resultFontSize, 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE,
                DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
                CLEARTYPE_QUALITY, DEFAULT_PITCH | FF_SWISS, L"Segoe UI");
            return 0;
        }
        case WM_PAINT:
        {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hwnd, &ps);

            RECT rect;
            GetClientRect(hwnd, &rect);

            RECT rectHist = rect;
            rectHist.bottom = rect.bottom * 4 / 10;
            rectHist.right -= 5;//right padding for text

            RECT rectResult = rect;
            rectResult.top = rectHist.bottom;
            rectResult.right -= 5;

            SetBkMode(hdc, TRANSPARENT);
            //drawing history
            SetTextColor(hdc, RGB(100, 100, 100));//dark gray
            SelectObject(hdc, hFontHistory);
            DrawTextW(hdc, historyText.c_str(), -1, &rectHist, DT_RIGHT | DT_BOTTOM | DT_SINGLELINE);
            
            //drawing result
            SetTextColor(hdc, RGB(0, 0, 0));
            SelectObject(hdc, hFontResult);
            DrawTextW(hdc, currentInput.c_str(), -1, &rectResult, DT_RIGHT | DT_BOTTOM | DT_SINGLELINE);

            EndPaint(hwnd, &ps);
            return 0;
        }
        case WM_DESTROY:
        {
            if (hFontHistory) DeleteObject(hFontHistory);
            if (hFontResult) DeleteObject(hFontResult);
            return 0;
        }
    }

    return DefWindowProcW(hwnd, uMsg, wParam, lParam);
}