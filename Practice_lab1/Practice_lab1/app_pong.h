#pragma once 

#include <windows.h>
#include<string>

class app_pong {
public:
	app_pong(HINSTANCE instance);
	~app_pong();
	int run(int show_command);
private:
	static constexpr int MAIN_WIDTH = 200;
	static constexpr int MAIN_HEIGHT = 300;
	static constexpr BYTE main_al = 204;
	static constexpr int ball_size = 15;
	static constexpr int paddle_w = 50;
	static constexpr int paddle_h = 10;

	static std::wstring const s_main_class_name;
	static std::wstring const s_ballClass_name;
	static std::wstring const s_paddleClass_name;
	bool register_main_class();
	bool register_ball_class();
	bool register_paddle_class();
	static LRESULT CALLBACK window_proc_static(
		HWND window, UINT message,
		WPARAM wparam, LPARAM lparam);
	LRESULT window_proc(
		HWND window, UINT message,
		WPARAM wparam, LPARAM lparam);
	HWND create_main_window();
	HWND create_ball_window();
	HWND create_paddle_window();
	void center_window(HWND window);
	void place_init_ch();
	HINSTANCE m_instance;
	HWND m_main;
	HWND m_ball;
	HWND m_paddle;
	HBRUSH m_main_brush;
	HBRUSH m_ball_brush;
	HBRUSH m_paddle_brush;
};