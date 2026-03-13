//#include "app_2048.h"
//
//std::wstring const app_2048::s_class_name{ L"2048 Window" };
//
//bool app_2048::register_class(){
//	WNDCLASSEXW desc{};
//	if (GetClassInfoExW(m_instance, s_class_name.c_str(), &desc) != 0) return true;
//	desc = {
//		.cbSize = sizeof(WNDCLASSEXW),
//		.lpfnWndProc = window_proc_static,
//		.hInstance = m_instance,
//		.hCursor = LoadCursorW(nullptr, IDC_ARROW),
//		.lpszClassName = s_class_name.c_str()
//	};
//	return RegisterClassExW(&desc)!=0;
//}
//
//HWND app_2048::create_window() {
//	return CreateWindowExW(
//		0,
//		s_class_name.c_str(),
//		L"2048",
//		WS_OVERLAPPED | WS_SYSMENU | WS_CAPTION | WS_BORDER | WS_MINIMIZEBOX,
//		CW_USEDEFAULT, 0,
//		CW_USEDEFAULT, 0,
//		nullptr,
//		nullptr,
//		m_instance,
//		this
//	);
//}
//
