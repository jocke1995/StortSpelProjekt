#include "Window.h"

// Problem med precompiled header eller liknande.. Intellisense problemet
#include "Engine/stdafx.h"

// callback function for windows messages
LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
	case WM_KEYDOWN:
		if (wParam == VK_ESCAPE)
		{
			//if (MessageBox(0, L"Are you sure you want to exit?", L"Exit", MB_YESNO | MB_ICONQUESTION) == IDYES)
			//{
				DestroyWindow(hWnd);
			//}
		}
		// Temp to create objects during runtime
		if (wParam == VK_SPACE)
		{
			spacePressed = true;
		}
		

		return 0;

	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	}
	return DefWindowProc(hWnd, msg, wParam, lParam);
}

Window::Window(
	HINSTANCE hInstance,
	int nCmdShow,
	bool fullScreen,
	int screenWidth, int screenHeight,
	LPCTSTR windowName, LPCTSTR windowTitle)
{
	this->screenWidth = screenWidth;
	this->screenHeight = screenHeight;
	this->fullScreen = fullScreen;
	this->windowName = windowName;
	this->windowTitle = windowTitle;

	InitWindow(hInstance, nCmdShow);
}


Window::~Window()
{

}

void Window::SetWindowTitle(std::wstring newTitle)
{
	SetWindowTextW(this->hwnd, newTitle.c_str());
}

bool Window::IsFullScreen() const
{
	return this->fullScreen;
}

int Window::GetScreenWidth() const
{
	return this->screenWidth;
}

int Window::GetScreenHeight() const
{
	return this->screenHeight;
}

const HWND* Window::GetHwnd() const
{
	return &this->hwnd;
}

bool Window::ExitWindow()
{
	bool closeWindow = false;
	MSG msg = { 0 };

	if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
	{

		TranslateMessage(&msg);
		DispatchMessage(&msg);	// Går in i "CALLBACK" funktionen

		if (msg.message == WM_QUIT)
		{
			closeWindow = true;
		}
	}
	return closeWindow;
}

bool Window::WasSpacePressed()
{
	if (spacePressed == true)
	{
		spacePressed = false;
		return true;
	}
	return false;
}

bool Window::InitWindow(HINSTANCE hInstance, int nCmdShow)
{
	if (this->fullScreen)
	{
		HMONITOR hmon = MonitorFromWindow(this->hwnd, MONITOR_DEFAULTTONEAREST);
		MONITORINFO mi = { sizeof(mi) };
		GetMonitorInfo(hmon, &mi);

		this->screenWidth = mi.rcMonitor.right - mi.rcMonitor.left;
		this->screenHeight = mi.rcMonitor.bottom - mi.rcMonitor.top;
	}

	WNDCLASSEX wc;
	wc.cbSize = sizeof(WNDCLASSEX);
	wc.style = CS_HREDRAW | CS_VREDRAW;	// Device Context
	wc.lpfnWndProc = WndProc;	// Callback, Event catcher
	wc.cbClsExtra = NULL;
	wc.cbWndExtra = NULL;
	wc.hInstance = hInstance;
	wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 2);
	wc.lpszMenuName = NULL;
	wc.lpszClassName = this->windowName;
	wc.hIconSm = LoadIcon(NULL, IDI_APPLICATION);

	if (!RegisterClassEx(&wc))
	{
		MessageBox(NULL, L"Error registering class", L"Error", MB_OK | MB_ICONERROR);
		return false;
	}

	// This structure describes the window
	this->hwnd = CreateWindowEx(NULL,
		this->windowName,
		this->windowTitle,
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, CW_USEDEFAULT,
		this->screenWidth, this->screenHeight,
		NULL,
		NULL,
		hInstance,
		NULL);

	// If the windowhandle was unsuccesful
	if (!this->hwnd)
	{
		MessageBox(NULL, L"Error creating window", L"Error", MB_OK | MB_ICONERROR);
		return false;
	}

	// Remove the topbar of the window if we are in fullscreen
	if (this->fullScreen)
	{
		SetWindowLong(this->hwnd, GWL_STYLE, 0);
	}

	ShowWindow(this->hwnd, nCmdShow);
	UpdateWindow(this->hwnd);

	return true;
}
