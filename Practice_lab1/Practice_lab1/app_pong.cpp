#include "app_pong.h"

std::wstring const app_pong::s_main_class_name{ L"PongMainWindow" };
std::wstring const app_pong::s_main_class_name{ L"PongBallWindow" };
std::wstring const app_pong::s_main_class_name{ L"PongPaddleWindow" };

bool app_pong::register_main_class() {
	WNDCLASSEXW desc{};
	if (GetClassInfoExW(m_instance, s_main_class_name.c_str(),
		&desc) != 0) return true;
	desc.cbSize = sizeof(WNDCLASSEXW);
	desc.lpfnWndProc = window_proc_static;
	desc.hInstance = m_instance;
	desc.hCursor = LoadCursorW(nullptr, L"IDC_ARROW");
	desc.lpszClassName = s_main_class_name.c_str();
	desc.hbrBackground = m_main_brush;
	return RegisterClassExW(&desc) != 0;

}

bool app_pong::register_ball_class() {
	WNDCLASSEXW desc{};
	if (GetClassInfoExW(m_instance, s_ballClass_name.c_str(),
		&desc) != 0) return true;
	desc.cbSize = sizeof(WNDCLASSEXW);
	desc.lpfnWndProc = window_proc_static;
	desc.hInstance = m_instance;
	desc.hCursor = LoadCursorW(nullptr, L"IDC_ARROW");
	desc.lpszClassName = s_ballClass_name.c_str();
	desc.hbrBackground = m_ball_brush;
	return RegisterClassExW(&desc) != 0;

}

bool app_pong::register_paddle_class() {
	WNDCLASSEXW desc{};
	if (GetClassInfoExW(m_instance, s_paddleClass_name.c_str(),
		&desc) != 0) return true;
	desc.cbSize = sizeof(WNDCLASSEXW);
	desc.lpfnWndProc = window_proc_static;
	desc.hInstance = m_instance;
	desc.hCursor = LoadCursorW(nullptr, L"IDC_ARROW");
	desc.lpszClassName = s_paddleClass_name.c_str();
	desc.hbrBackground = m_paddle_brush;
	return RegisterClassExW(&desc) != 0;

}

app_pong::app_pong(HINSTANCE instance) :m_instance{ instance }, m_main{}, 
			m_main_brush(CreateSolidBrush(RGB(255, 255, 0))),
			m_ball_brush(CreateSolidBrush(RGB(255,0,0))),
			m_paddle_brush(CreateSolidBrush(GetSysColor(COLOR_ACTIVECAPTION))){
	register_main_class();
	register_ball_class();
	register_paddle_class();
	m_main = create_main_window();
	m_ball = create_ball_window();
	m_paddle = create_paddle_window();

	place_init_ch();
}

app_pong::~app_pong() {
	if (m_main_brush) {
		DeleteObject(m_main_brush);
		m_main_brush = nullptr;
	}
}

HWND app_pong::create_main_window() {
	DWORD style_ex = WS_EX_LAYERED;
	DWORD style = WS_OVERLAPPED | WS_SYSMENU | WS_CAPTION | WS_MINIMIZEBOX;
	RECT r{ 0, 0, MAIN_WIDTH, MAIN_HEIGHT };

	AdjustWindowRectEx(&r, style, false, 0);

	HWND wnd = CreateWindowExW(
		style_ex,
		s_main_class_name.c_str(),
		L"Pong",
		style,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		r.right - r.left,
		r.bottom - r.top,
		nullptr,
		nullptr,
		m_instance,
		this
	);
	if (wnd) {
		center_window(wnd);
	}

	SetLayeredWindowAttributes(wnd, 0, main_al, LWA_ALPHA);

	return wnd;
}

HWND app_pong::create_ball_window() {
	DWORD style_ex = WS_EX_LAYERED;
	DWORD style = WS_OVERLAPPED | WS_SYSMENU | WS_CAPTION | WS_MINIMIZEBOX;
	RECT r{ 0, 0, MAIN_WIDTH, MAIN_HEIGHT };

	AdjustWindowRectEx(&r, style, false, 0);

	HWND wnd = CreateWindowExW(
		style_ex,
		s_ballClass_name.c_str(),
		L"",
		style,
		0,
		0,
		r.right - r.left,
		r.bottom - r.top,
		m_main,
		nullptr,
		m_instance,
		this
	);
	
	HRGN region = CreateEllipticRgn(0, 0, ball_size, ball_size);
	SetWindowRgn(wnd, region, TRUE);


	return wnd;
}

LRESULT CALLBACK app_pong::window_proc_static(
	HWND window,
	UINT message,
	WPARAM wparam,
	LPARAM lparam
) {
	app_pong* app = nullptr;
	if (message == WM_NCCREATE)
	{
		auto p = reinterpret_cast<LPCREATESTRUCTW>(lparam);
		app = static_cast<app_pong*>(p->lpCreateParams);
		SetWindowLongPtrW(window, GWLP_USERDATA,
			reinterpret_cast<LONG_PTR>(app));
	}
	else
	{
		app = reinterpret_cast<app_pong*>(
			GetWindowLongPtrW(window, GWLP_USERDATA));
	}
	if (app != nullptr)
	{
		return app->window_proc(window, message,
			wparam, lparam); 
	}
	return DefWindowProcW(window, message, wparam, lparam);
}

LRESULT app_pong::window_proc(
	HWND window, UINT message,
	WPARAM wparam, LPARAM lparam
) {
	switch (message) {
	case WM_ERASEBKGND: {
		RECT r{};
		GetClientRect(window, &r);
		FillRect(reinterpret_cast<HDC>(wparam), &r, m_main_brush);
		return 1;
	}
	case WM_CLOSE:
		DestroyWindow(window);
		return 0;
	case WM_DESTROY:
		if (window == m_main)
			PostQuitMessage(EXIT_SUCCESS);
		return 0;
	}
	return DefWindowProcW(window, message, wparam, lparam);
}

int app_pong::run(int show_command) {
	ShowWindow(m_main, show_command);
	UpdateWindow(m_main);

	MSG msg{};
	while (true) {
		BOOL res = GetMessageW(&msg, nullptr, 0, 0);
		if (res == -1) {
			return 1;
		}
		if (res == 0) {
			break;
		}

		TranslateMessage(&msg);
		DispatchMessageW(&msg);
	}
}

void app_pong::center_window(HWND window) {
	RECT r;
	GetWindowRect(window, &r);
	int w_width = r.right - r.left;
	int w_height = r.bottom - r.top;

	int screen_w = GetSystemMetrics(SM_CXSCREEN);
	int screen_h = GetSystemMetrics(SM_CYSCREEN);

	int x_centered = (screen_w - w_width) / 2;
	int y_centered = (screen_h - w_height) / 2;
	SetWindowPos(
		window,
		nullptr,
		x_centered,
		y_centered,
		0,
		0,
		SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE
	);
}

