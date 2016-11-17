#include "Window.h"

#include <windows.h>
#include <windowsx.h>

#pragma comment (lib, "Winmm.lib")

Window::Window(HINSTANCE hInstance)
{
	WNDCLASSEX wc;
	wc.cbSize = sizeof(WNDCLASSEX); //size of windows class
	wc.style = CS_HREDRAW | CS_VREDRAW; //class styles
	wc.lpfnWndProc = WndProc; //window processing function
	wc.cbClsExtra = NULL; //extra bytes after wc structure
	wc.cbWndExtra = NULL; //extra bytes after windows instance
	wc.hInstance = hInstance; //instance to current application
	wc.hIcon = LoadIcon(NULL, IDI_WINLOGO); //title bar Icon
	wc.hCursor = LoadCursor(NULL, IDC_ARROW); //default mouse Icon
	wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 2); //window bg color
	wc.lpszMenuName = NULL; //name of the menu attached to window
	wc.lpszClassName = L"Sample DirectX application Window"; //name of windows class
	wc.hIconSm = LoadIcon(NULL, IDI_WINLOGO); //icon in taskbar

	if (!RegisterClassEx(&wc))
	{
		MessageBox(NULL, L"Error registering class", L"Error", MB_OK | MB_ICONERROR);
	}

	RECT windowRect = { 0, 0, g_WindowWidth, g_WindowHeight };
	AdjustWindowRect(&windowRect, WS_OVERLAPPEDWINDOW, FALSE);

	handle = CreateWindow(windowClassName, windowName, WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT,
								windowRect.right - windowRect.left, windowRect.bottom - windowRect.top, 
								nullptr, nullptr, hInstance, nullptr);

	if (!handle)
	{
		MessageBox(NULL, L"Error creating window", L"Error", MB_OK | MB_ICONERROR);
	}

	ShowWindow(handle, SW_SHOWDEFAULT);
	UpdateWindow(handle);
}

LRESULT CALLBACK Window::WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	PAINTSTRUCT paintStruct;
	HDC hDC;

	switch (message)
	{
	case WM_PAINT:
	{
		hDC = BeginPaint(hwnd, &paintStruct);
		EndPaint(hwnd, &paintStruct);
	}
	break;
	case WM_DESTROY:
	{
		PostQuitMessage(0);
	}
	break;
	default:
		return DefWindowProc(hwnd, message, wParam, lParam);
	}

	return 0;
}