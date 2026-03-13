#define UNICODE
#define _UNICODE

#include<windows.h>

constexpr wchar_t class_name[] = L"PoorMansSnakeWindowClass";
constexpr wchar_t window_title[] = L"poor man's snake";

constexpr int width = 800;
constexpr int height = 600;
constexpr COLORREF bg_color = RGB(240, 230, 120);
constexpr int player_size = 100;
constexpr COLORREF player_color = RGB(200, 80, 90);
constexpr UINT_PTR t_id = 1;
constexpr UINT timer_interval = 20;
constexpr int move_step = 3;

HWND glob_player = nullptr;
HBRUSH glob_backgroundBrush = nullptr;
HBRUSH glob_playerBrush = nullptr;

int playerX = 0;
int playerY = 0;

int dx = -move_step;
int dy = 0;

void PlayerMove() {
	int nextX = playerX + dx;
	int nextY = playerY + dy;

	if (nextX<0 || nextX>width-player_size) {
		dx = -dx;
		nextX = playerX + dx;
	}
	if (nextY<0 || nextY>height - player_size) {
		dy = -dy;
		nextY = playerY + dy;
	}

	playerX = playerX + dx;
	playerY = playerY + dy;

	SetWindowPos(
		glob_player,
		nullptr,
		playerX,
		playerY,
		0,
		0,
		SWP_NOZORDER | SWP_NOSIZE
	);
}

void SetDirection(int local_dx, int local_dy) {
	dx = local_dx;
	dy = local_dy;
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
	switch (msg) {
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	case WM_CTLCOLORSTATIC:{
		HDC hdcStatic = reinterpret_cast<HDC>(wParam);
		HWND hStatic = reinterpret_cast<HWND>(lParam);
		if (hStatic == glob_player) {
			SetBkColor(hdcStatic, player_color);
			return reinterpret_cast<LRESULT>(glob_player);
		}
		break;
	}
	case WM_TIMER:
		if (wParam == t_id) {
			PlayerMove();
			return 0;
		}
		break;

	case WM_KEYDOWN:
		switch (wParam) {
		case VK_LEFT:
		case 'A':
			SetDirection(-move_step, 0);
			return 0;

		case VK_RIGHT:
		case 'D':
			SetDirection(move_step, 0);
			return 0;

		case VK_UP:
		case'W':
			SetDirection(0, -move_step);
			return 0;

		case VK_DOWN:
		case 'S':
			SetDirection(0, move_step);
			return 0;
		}
		break;
	}
	return DefWindowProc(hwnd, msg, wParam, lParam);
}

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE, PWSTR, int show_command) {
	glob_backgroundBrush = CreateSolidBrush(bg_color);
	glob_playerBrush = CreateSolidBrush(player_color);

	WNDCLASS wc = {};
	wc.lpfnWndProc = WndProc;
	wc.hInstance = hInstance;
	wc.lpszClassName = class_name;
	wc.hbrBackground = glob_backgroundBrush;
	wc.hCursor = LoadCursor(nullptr, IDC_ARROW);

	RegisterClass(&wc);

	DWORD style = WS_OVERLAPPED | WS_CAPTION | WS_MAXIMIZEBOX | WS_SYSMENU;
	RECT r = { 0,0,width, height };
	AdjustWindowRect(&r, style, FALSE);

	int wndWidth = r.right - r.left;
	int wndHeight = r.bottom - r.top;

	HWND hwnd = CreateWindowEx(
		0,
		class_name,
		window_title,
		style | WS_CLIPCHILDREN,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		wndWidth,
		wndHeight,
		nullptr,
		nullptr,
		hInstance,
		nullptr
	);

	playerX = (width - player_size) / 2;
	playerY = (height - player_size) / 2;

	glob_player = CreateWindowEx(
		0,
		L"STATIC",
		nullptr,
		WS_CHILD | WS_VISIBLE,
		playerX,
		playerY,
		player_size,
		player_size,
		hwnd,
		nullptr,
		hInstance,
		nullptr
	);

	SetTimer(hwnd, t_id, timer_interval, nullptr);

	ShowWindow(hwnd, show_command);
	UpdateWindow(hwnd);

	MSG msg = {};
	while (GetMessage(&msg, nullptr, 0, 0)) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	DeleteObject(glob_backgroundBrush);
	DeleteObject(glob_playerBrush);
	return static_cast<int>(msg.wParam);
}

