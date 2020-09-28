#include "stdafx.h"
#include "Window.h"
#include "..\Input\Input.h"

#include "../ImGUI/imgui.h"
#include "../ImGUI/imgui_impl_win32.h"
#include "../ImGUI/imgui_impl_dx12.h"

// Forward declare message handler from imgui_impl_win32.cpp
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

// callback function for windows messages
LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	if (ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam))
		return true;

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

	case WM_INPUT:
		UINT dwSize;

		GetRawInputData((HRAWINPUT)lParam, RID_INPUT, NULL, &dwSize, sizeof(RAWINPUTHEADER));
		LPBYTE lpb = new BYTE[dwSize];
		if (lpb == NULL)
		{
			return 0;
		}

		if (GetRawInputData((HRAWINPUT)lParam, RID_INPUT, lpb, &dwSize, sizeof(RAWINPUTHEADER)) != dwSize)
		{
			OutputDebugString(TEXT("GetRawInputData does not return correct size !\n"));
		}

		RAWINPUT* raw = reinterpret_cast<RAWINPUT*>(lpb);

		if (raw->header.dwType == RIM_TYPEKEYBOARD)
		{
			auto inputData = raw->data.keyboard;

			int modifier = (inputData.Flags / 2 + 1) * 0x100;
			SCAN_CODES key = static_cast<SCAN_CODES>(inputData.MakeCode + modifier);

			if (DEVELOPERMODE_DEVINTERFACE == true)
			{
				if (key == SCAN_CODES::LEFT_SHIFT && !Input::GetInstance().GetKeyState(SCAN_CODES::LEFT_SHIFT) && !(inputData.Flags % 2))
				{
					ShowCursor(true);
				}
				else if (key == SCAN_CODES::LEFT_SHIFT && (inputData.Flags % 2))
				{
					ShowCursor(false);
				}
			}

			Input::GetInstance().SetKeyState(key, !(inputData.Flags % 2));
		}
		else if (raw->header.dwType == RIM_TYPEMOUSE)
		{
			if (DEVELOPERMODE_DEVINTERFACE == false || !Input::GetInstance().GetKeyState(SCAN_CODES::LEFT_SHIFT))
			{
				auto inputData = raw->data.mouse;
				MOUSE_BUTTON button = static_cast<MOUSE_BUTTON>(inputData.usButtonFlags);

				switch (button)
				{
					case MOUSE_BUTTON::WHEEL:
						Input::GetInstance().SetMouseScroll(inputData.usButtonData);
						break;
					case MOUSE_BUTTON::LEFT_DOWN:
					case MOUSE_BUTTON::MIDDLE_DOWN:
					case MOUSE_BUTTON::RIGHT_DOWN:
						Input::GetInstance().SetMouseButtonState(button, true);
						break;
					case MOUSE_BUTTON::LEFT_UP:
					case MOUSE_BUTTON::MIDDLE_UP:
					case MOUSE_BUTTON::RIGHT_UP:
						button = static_cast<MOUSE_BUTTON>(static_cast<int>(button) / 2);
						Input::GetInstance().SetMouseButtonState(button, false);
						break;
					default:
						break;
				}

				Input::GetInstance().SetMouseMovement(inputData.lLastX, inputData.lLastY);

				SetCursorPos(500, 400);
			}
		}

		delete[] lpb;

		return 0;
	}
	return DefWindowProc(hWnd, msg, wParam, lParam);
}

Window::Window(
	HINSTANCE hInstance,
	int nCmdShow,
	bool windowedFullScreen,
	int screenWidth, int screenHeight,
	LPCTSTR windowName, LPCTSTR windowTitle)
{
	m_ScreenWidth = screenWidth;
	m_ScreenHeight = screenHeight;
	m_WindowedFullScreen = windowedFullScreen;
	m_WindowName = windowName;
	m_WindowTitle = windowTitle;

	initWindow(hInstance, nCmdShow);

	m_ShutDown = false;
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
	return m_WindowedFullScreen;
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
	bool closeWindow = m_ShutDown;
	MSG msg = { 0 };

	while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
	{

		TranslateMessage(&msg);
		DispatchMessage(&msg);	// Går in i "CALLBACK" funktionen

		if (msg.message == WM_QUIT)
		{
			m_ShutDown = true;
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
	if (m_WindowedFullScreen)
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
	SetWindowLong(m_Hwnd, GWL_STYLE, 0);

	ShowWindow(m_Hwnd, nCmdShow);
	ShowCursor(false);
	UpdateWindow(m_Hwnd);

	return true;
}
