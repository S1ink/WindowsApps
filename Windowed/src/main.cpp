#include "additions.h"

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR pCmdLine, int nCmdShow) {
	wnd::MainWindow window;

	if (!window.Create(L"Windowed Program v.1.1.3", WS_OVERLAPPEDWINDOW)) {
		return 0;
	}

	ShowWindow(window.Window(), nCmdShow);

	MSG msg = {};
	while (GetMessage(&msg, NULL, 0, 0)) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	return 0;
}