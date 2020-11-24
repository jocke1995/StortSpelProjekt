#include "Input.h"
#include "..\Events\EventBus.h"
#include "..\Misc\Timer.h"

// Needed for shopEvent
#include "../ECS/SceneManager.h"
#include "../../Game/src/Gamefiles/Player.h"
#include "../../Game/src/Gamefiles/Shop.h"

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
	bool doubleTap = false;
	if (justPressed)
	{
		std::chrono::system_clock::time_point timeLast = m_KeyTimer[key];
		m_KeyTimer[key] = std::chrono::system_clock::now();
		std::chrono::duration<double> elapsed_time = m_KeyTimer[key] - timeLast;
		auto time = elapsed_time.count();

		if (time < 0.2)
		{
			doubleTap = true;
		}
	}

	m_KeyState[key] = pressed;
	if (key == SCAN_CODES::W || key == SCAN_CODES::A || key == SCAN_CODES::S || key == SCAN_CODES::D || key == SCAN_CODES::Q || key == SCAN_CODES::E || key == SCAN_CODES::SPACE)
	{
		// Disable movement when in Shop2D-GUI state
		if (Player::GetInstance().GetShop()->IsShop2DGUIDisplaying() == false)
		{
			// Publish movement events
			if (justPressed)
			{
				EventBus::GetInstance().Publish(&MovementInput(key, justPressed, doubleTap));
			}
			else if (!pressed)
			{
				EventBus::GetInstance().Publish(&MovementInput(key, pressed, doubleTap));
			}
		}
	}
	else if (key == SCAN_CODES::LEFT_CTRL || key == SCAN_CODES::LEFT_SHIFT ||key == SCAN_CODES::TAB)
	{
		if (justPressed)
		{
			EventBus::GetInstance().Publish(&ModifierInput(key, justPressed));
		}
		else if (!pressed)
		{
			EventBus::GetInstance().Publish(&ModifierInput(key, pressed));
		}
	}
	else if (key == SCAN_CODES::U)
	{
		if (justPressed)
		{
			EventBus::GetInstance().Publish(&UForUpgrade());
		}
	}
	else if (key == SCAN_CODES::F)
	{
		if (justPressed)
		{
			// Check if we are in the ShopScene
			Scene* scene = SceneManager::GetInstance().GetActiveScene();
			if (scene->GetName() == "ShopScene")
			{
				EventBus::GetInstance().Publish(&UForUpgrade());
				EventBus::GetInstance().Publish(&shopGUIStateChange());
			}
		}
	}
}

void Input::SetMouseButtonState(MOUSE_BUTTON button, bool pressed)
{
	m_MouseButtonState[button] = pressed;
	switch (pressed) {
	case true:
		EventBus::GetInstance().Publish(&MouseClick(button, pressed));
		break;
	case false:
		EventBus::GetInstance().Publish(&MouseRelease(button, pressed));
		break;
	}
}

void Input::SetMouseScroll(SHORT scroll)
{
	int mouseScroll = static_cast<int>(scroll > 0) * 2 - 1;
	EventBus::GetInstance().Publish(&MouseScroll(mouseScroll));
}

void Input::SetMouseMovement(int x, int y)
{
	// Disable movement when in Shop2D-GUI state
	if (Player::GetInstance().GetShop()->IsShop2DGUIDisplaying() == false)
	{
		EventBus::GetInstance().Publish(&MouseMovement(x, y));
	}
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
	m_KeyTimer[SCAN_CODES::W] = m_KeyTimer[SCAN_CODES::A] = m_KeyTimer[SCAN_CODES::S] = m_KeyTimer[SCAN_CODES::D] = m_KeyTimer[SCAN_CODES::SPACE] = std::chrono::system_clock::now();
}
