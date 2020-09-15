#include "Input.h"
#include "..\Events\EventBus.h"

Input& Input::GetInstance()
{
	static Input instance;

	return instance;
}

void Input::RegisterDevices(const HWND* hWnd)
{
	static RAWINPUTDEVICE m_Rid[2];

	// Register mouse
	m_Rid[0].usUsagePage = 0x01;
	m_Rid[0].usUsage = 0x02;
	m_Rid[0].dwFlags = 0;
	m_Rid[0].hwndTarget = *hWnd;

	// Register keyboard
	m_Rid[1].usUsagePage = 0x01;
	m_Rid[1].usUsage = 0x06;
	m_Rid[1].dwFlags = 0;
	m_Rid[1].hwndTarget = *hWnd;

	if (RegisterRawInputDevices(m_Rid, 2, sizeof(m_Rid[0])) == FALSE)
	{
		Log::Print("Device registration error: %f\n", GetLastError());
	}
	else
	{
		Log::Print("Input devices registered!\n");
	}
}

void Input::SetKeyState(SCAN_CODES key, bool pressed)
{
	bool justPressed = !m_KeyState[key];
	m_KeyState[key] = pressed;
	if ((key == SCAN_CODES::W || key == SCAN_CODES::A || key == SCAN_CODES::S || key == SCAN_CODES::D || key == SCAN_CODES::Q || key == SCAN_CODES::E) && !pressed)
	{
		EventBus::GetInstance().Publish(&MovementInput(key, pressed));
	}
	else if (key == SCAN_CODES::LEFT_CTRL && !pressed)
	{
		EventBus::GetInstance().Publish(&ModifierInput(key, pressed));
	}
	else if ((key == SCAN_CODES::W || key == SCAN_CODES::A || key == SCAN_CODES::S || key == SCAN_CODES::D || key == SCAN_CODES::Q || key == SCAN_CODES::E) && justPressed)
	{
		EventBus::GetInstance().Publish(&MovementInput(key, justPressed));
	}
	else if (key == SCAN_CODES::LEFT_CTRL && justPressed)
	{
		EventBus::GetInstance().Publish(&ModifierInput(key, justPressed));
	}
}

void Input::SetMouseButtonState(MOUSE_BUTTON button, bool pressed)
{
	m_MouseButtonState[button] = pressed;
	EventBus::GetInstance().Publish(&MouseClick(button, pressed));
}

void Input::SetMouseScroll(SHORT scroll)
{
	int mouseScroll = static_cast<int>(scroll > 0) * 2 - 1;
	EventBus::GetInstance().Publish(&MouseScroll(mouseScroll));
}

void Input::SetMouseMovement(int x, int y)
{
	EventBus::GetInstance().Publish(&MouseMovement(x, y));
}

bool Input::GetKeyState(SCAN_CODES key)
{
	return m_KeyState[key];
}

bool Input::GetMouseButtonState(MOUSE_BUTTON button)
{
	return m_MouseButtonState[button];
}

Input::Input()
{
}