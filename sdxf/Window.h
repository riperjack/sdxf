#pragma once

#include <windows.h>
#include <windowsx.h>

class Window {
public:
	Window(HINSTANCE hInstance);
	HWND handle;
private:
	static LRESULT CALLBACK Window::WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
	static constexpr wchar_t* windowClassName = L"Sample DirectX application Window";
	static constexpr wchar_t* windowName = L"Sample DirectX application";
	static const LONG g_WindowWidth = 1280;
	static const LONG g_WindowHeight = 720;
};