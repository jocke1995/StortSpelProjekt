#include "stdafx.h"
#include "Window.h"

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
		if (wParam == VK_TAB)
		{
			tabPressed = true;
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
	m_ScreenWidth = screenWidth;
	m_ScreenHeight = screenHeight;
	m_FullScreen = fullScreen;
	m_WindowName = windowName;
	m_WindowTitle = windowTitle;

	initWindow(hInstance, nCmdShow);
}


Window::~Window()
{

}

void Window::SetWindowTitle(std::wstring newTitle)
{
	SetWindowTextW(m_Hwnd, newTitle.c_str());
}

bool Window::IsFullScreen() const
{
	return m_FullScreen;
}

int Window::GetScreenWidth() const
{
	return m_ScreenWidth;
}

int Window::GetScreenHeight() const
{
	return m_ScreenHeight;
}

const HWND* Window::GetHwnd() const
{
	return &m_Hwnd;
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

bool Window::WasTabPressed()
{
	if (tabPressed == true)
	{
		tabPressed = false;
		return true;
	}
	return false;
}

bool Window::initWindow(HINSTANCE hInstance, int nCmdShow)
{
	if (m_FullScreen)
	{
		HMONITOR hmon = MonitorFromWindow(m_Hwnd, MONITOR_DEFAULTTONEAREST);
		MONITORINFO mi = { sizeof(mi) };
		GetMonitorInfo(hmon, &mi);

		m_ScreenWidth = mi.rcMonitor.right - mi.rcMonitor.left;
		m_ScreenHeight = mi.rcMonitor.bottom - mi.rcMonitor.top;
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
	wc.lpszClassName = m_WindowName;
	wc.hIconSm = LoadIcon(NULL, IDI_APPLICATION);

	if (!RegisterClassEx(&wc))
	{
		MessageBox(NULL, L"Error registering class", L"Error", MB_OK | MB_ICONERROR);
		return false;
	}

	// This structure describes the window
	m_Hwnd = CreateWindowEx(NULL,
		m_WindowName,
		m_WindowTitle,
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, CW_USEDEFAULT,
		m_ScreenWidth, m_ScreenHeight,
		NULL,
		NULL,
		hInstance,
		NULL);

	// If the windowhandle was unsuccesful
	if (!m_Hwnd)
	{
		MessageBox(NULL, L"Error creating window", L"Error", MB_OK | MB_ICONERROR);
		return false;
	}

	// Remove the topbar of the window if we are in fullscreen
	if (m_FullScreen)
	{
		SetWindowLong(m_Hwnd, GWL_STYLE, 0);
	}

	ShowWindow(m_Hwnd, nCmdShow);
	UpdateWindow(m_Hwnd);

	return true;
}
