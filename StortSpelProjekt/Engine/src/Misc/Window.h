#ifndef WINDOW_H
#define WINDOW_H
#include <Windows.h>
#include <string>

// Temp
static bool spacePressed = false;
static bool tabPressed = false;

struct ShutDown;
struct CursorShow;

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

	void SetScreenWidth(int width);
	void SetScreenHeight(int height);

	bool ExitWindow();

	void MouseInClipspace(float* x, float* y) const;

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
	void closeWindow(ShutDown* evnt);
	void setShowCursor(CursorShow* evnt);
};

#endif