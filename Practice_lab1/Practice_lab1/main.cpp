#include <windows.h>
#include "app_pong.h"

int WINAPI wWinMain(HINSTANCE instance, HINSTANCE, LPWSTR, int show_command) {
	app_pong app(instance);
	return app.run(show_command);
}