
#include "stdafx.h"
#include "Window.h"
#include "Option.h"

#include "../Input/Input.h"
#include "../ImGUI/imgui.h"
#include "../ImGUI/imgui_impl_win32.h"
#include "../ImGUI/imgui_impl_dx12.h"
#include "../Events/EventBus.h"

// Forward declare message handler from imgui_impl_win32.cpp
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

struct WindowChange;

// callback function for windows messages
LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	static bool programRunning = true;

	if (ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam))
	{
		return true;
	}

	switch (msg)
	{
	case WM_SYSKEYDOWN: // alt+enter
		// Removed since it causes crashes.
		//if (wParam == VK_RETURN && (lParam & 0x60000000) == 0x20000000)
		//{
		//	EventBus::GetInstance().Publish(&WindowChange());
		//}
		return 0;
	case WM_ACTIVATEAPP: // alt+tab, windows key and more
		if (!wParam && programRunning
			&& (std::atoi(Option::GetInstance().GetVariable("i_windowMode").c_str()) == static_cast<int>(WINDOW_MODE::FULLSCREEN)))
		{
			EventBus::GetInstance().Publish(&WindowChange());
		}
		return 0;

	case WM_KEYDOWN:
		// Removed since we don't want people to randomly press escape and shut the program down.
		// Todo: implement menu where you can exit.
		//if (wParam == VK_ESCAPE)
		//{
		//	//if (MessageBox(0, L"Are you sure you want to exit?", L"Exit", MB_YESNO | MB_ICONQUESTION) == IDYES)
		//	//{
		//	programRunning = false;
		//	DestroyWindow(hWnd);
		//	//}
		//}
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

	// On close and alt+f4, alt+f4 don't work apparently, is coded in WM_KEYDOWN
	case WM_CLOSE:
		programRunning = false;
		DestroyWindow(hWnd);
		return 0;
		
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;

	case WM_INPUT:
		UINT dwSize;

		GetRawInputData((HRAWINPUT)lParam, RID_INPUT, NULL, &dwSize, sizeof(RAWINPUTHEADER));
		LPBYTE lpb = new BYTE[dwSize];

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

			if ( key == SCAN_CODES::END || (key == SCAN_CODES::F4 && Input::GetInstance().GetKeyState(SCAN_CODES::ALT)))
			{
				// Check if not pressed up (case where HOLD f4 -> HOLD alt -> RELEASE f4)
				if (inputData.Flags != RI_KEY_BREAK)
				{
					// UnSet alt
					Input::GetInstance().SetKeyState(SCAN_CODES::ALT, false);

					// Quit
					programRunning = false;
					DestroyWindow(hWnd);
				}
			}

			if (key == SCAN_CODES::TAB && Input::GetInstance().GetKeyState(SCAN_CODES::TAB))
			{
				if (DEVELOPERMODE_GAMEINTERFACE && !IsGuiHidden())
				{
					// Hide
					HideGui(true);
					ShowCursor(false);
				}
				else if (DEVELOPERMODE_GAMEINTERFACE)
				{
					// Show
					HideGui(false);
					ShowCursor(true);
				}

				if (DEVELOPERMODE_DEVINTERFACE && !IsImguiHidden())
				{
					// Hide
					HideImgui(true);
					ShowCursor(false);
				}
				else if (DEVELOPERMODE_DEVINTERFACE)
				{
					// Show
					HideImgui(false);
					ShowCursor(true);
				}
			}

			Input::GetInstance().SetKeyState(key, !(inputData.Flags % 2));
		}
		else if (raw->header.dwType == RIM_TYPEMOUSE)
		{
			auto inputData = raw->data.mouse;
			MOUSE_BUTTON button = static_cast<MOUSE_BUTTON>(inputData.usButtonFlags);

			switch (button)
			{
				case MOUSE_BUTTON::WHEEL:
					Input::GetInstance().SetMouseScroll(inputData.usButtonData);
					break;
				case MOUSE_BUTTON::LEFT_DOWN:
					Input::GetInstance().SetMouseButtonState(button, true);
					break;
				case MOUSE_BUTTON::MIDDLE_DOWN:
				case MOUSE_BUTTON::RIGHT_DOWN:
					Input::GetInstance().SetMouseButtonState(button, true);
					break;
				case MOUSE_BUTTON::LEFT_UP:
					button = static_cast<MOUSE_BUTTON>(static_cast<int>(button) / 2);
					Input::GetInstance().SetMouseButtonState(button, false);
					break;
				case MOUSE_BUTTON::MIDDLE_UP:
				case MOUSE_BUTTON::RIGHT_UP:
					button = static_cast<MOUSE_BUTTON>(static_cast<int>(button) / 2);
					Input::GetInstance().SetMouseButtonState(button, false);
					break;
				default:
					break;
			}

			Input::GetInstance().SetMouseMovement(inputData.lLastX, inputData.lLastY);
			RECT win;
			GetWindowRect(hWnd, &win);
			ClipCursor(&win);
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

	EventBus::GetInstance().Subscribe(this, &Window::closeWindow);
	EventBus::GetInstance().Subscribe(this, &Window::setShowCursor);
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

void Window::SetScreenWidth(int width)
{
	m_ScreenWidth = width;
}

void Window::SetScreenHeight(int height)
{
	m_ScreenHeight = height;
}

bool Window::ExitWindow()
{
	bool closeWindow = m_ShutDown;
	MSG msg = { 0 };

	while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
	{

		TranslateMessage(&msg);
		DispatchMessage(&msg);

		if (msg.message == WM_QUIT)
		{
			m_ShutDown = true;
		}
	}
	return closeWindow;
}

void Window::MouseInClipspace(float* x, float* y) const
{
	// Get the mouse position from your screenspace
	POINT p;
	GetCursorPos(&p);
	
	// Transform the position from your screenspace to the clientspace (space of the window)
	ScreenToClient(m_Hwnd, &p);

	// Transform the clientspace to the DirectX coordinates (0, 0) = (-1, 1)
	*x = (static_cast<float>(p.x) / (m_ScreenWidth / 2)) - 1;
	*y = -((static_cast<float>(p.y) / (m_ScreenHeight / 2)) - 1);
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
	HMONITOR hmon = MonitorFromWindow(m_Hwnd, MONITOR_DEFAULTTONEAREST);
	MONITORINFO mi = { sizeof(mi) };
	GetMonitorInfo(hmon, &mi);

	int width = mi.rcMonitor.right - mi.rcMonitor.left;
	int height = mi.rcMonitor.bottom - mi.rcMonitor.top;

	if (m_WindowedFullScreen || (width < m_ScreenWidth || height < m_ScreenHeight))
	{
		m_ScreenWidth = width;
		m_ScreenHeight = height;
	}

	WNDCLASSEX wc;
	wc.cbSize = sizeof(WNDCLASSEX);
	wc.style = CS_HREDRAW | CS_VREDRAW ;	// Device Context
	wc.lpfnWndProc = WndProc;	// Callback, Event catcher
	wc.cbClsExtra = NULL;
	wc.cbWndExtra = NULL;
	wc.hInstance = hInstance;
	wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 2);
	wc.lpszMenuName = L"WindowSomethingMenuName";
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
		WS_OVERLAPPED,
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
	UpdateWindow(m_Hwnd);

	return true;
}

void Window::closeWindow(ShutDown* evnt)
{
	m_ShutDown = true;
}

void Window::setShowCursor(CursorShow* evnt)
{
	ShowCursor(evnt->m_Show);
}

bool IsImguiHidden()
{
	return g_HideImgui;
}

bool IsGuiHidden()
{
	return g_HideGui;
}

void HideImgui(bool hide)
{
	g_HideImgui = hide;
}

void HideGui(bool hide)
{
	g_HideGui = hide;
}
