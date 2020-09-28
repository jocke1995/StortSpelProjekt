#ifndef WINDOW_H
#define WINDOW_H
#include <Windows.h>
#include <string>
// Temp
static bool spacePressed = false;
static bool tabPressed = false;

class Window
{
public:
	// Varying screensize constructor
	Window( // Default values
		HINSTANCE hInstance,
		int nCmdShow,
		bool windowedFullScreen = false,
		int screenWidth = 800,
		int screenHeight = 600,
		LPCTSTR windowName = L"windowName",
		LPCTSTR windowTitle = L"windowTitle");

	~Window();

	void SetWindowTitle(std::wstring newTitle);

	bool IsFullScreen() const;
	int GetScreenWidth() const;
	int GetScreenHeight() const;
	const HWND* GetHwnd() const;

	bool ExitWindow();

	// Temp
	bool WasSpacePressed();
	bool WasTabPressed();

private:
	int m_ScreenWidth;
	int m_ScreenHeight;
	bool m_WindowedFullScreen;
	LPCTSTR m_WindowName;
	LPCTSTR m_WindowTitle;

	HWND m_Hwnd = nullptr;
	bool m_ShutDown;

	bool initWindow(HINSTANCE hInstance, int nCmdShow);
};

#endif