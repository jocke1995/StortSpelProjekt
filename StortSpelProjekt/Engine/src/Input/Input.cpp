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

void Input::RegisterControllers()
{
	if (RawGameController::RawGameControllers().Size() > 0 && m_RawGameControllers.size() < 1)
	{
		for (auto const& rawGameController : RawGameController::RawGameControllers())
		{
			// Test whether the raw game controller is already in m_RawGameControllers; if it isn't, add it.
			concurrency::critical_section::scoped_lock lock{ m_ControllerLock };
			auto it{ std::find(begin(m_RawGameControllers), end(m_RawGameControllers), rawGameController) };

			if (it == end(m_RawGameControllers))
			{
				// This code assumes that you're interested in all raw game controllers.
				m_RawGameControllers.push_back(rawGameController);
			}
		}

		m_pMainController = &m_RawGameControllers.at(0);
		m_ControllerButtonCount = m_pMainController->ButtonCount();
		m_ControllerAxisCount = m_pMainController->AxisCount();
		m_ControllerSwitchCount = m_pMainController->SwitchCount();
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
			Scene* scene = SceneManager::GetInstance().GetActiveScene();
			if (scene->GetName() != "ShopScene")
			{
				EventBus::GetInstance().Publish(&UForUpgrade());
			}
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
		EventBus::GetInstance().Publish(&MouseMovement(static_cast<float>(x), static_cast<float>(y)));
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

void Input::ReadControllerInput()
{
	if (m_RawGameControllers.size() > 0)
	{
		std::array<bool, 20> buttonsArray;
		std::array<double, 10> axisArray;
		std::array<GameControllerSwitchPosition, 5> switchesArray;

		m_pMainController->GetCurrentReading(buttonsArray, switchesArray, axisArray);

		bool justPressedUp = !m_KeyState[SCAN_CODES::W];
		bool justPressedLeft = !m_KeyState[SCAN_CODES::A];
		bool justPressedDown = !m_KeyState[SCAN_CODES::S];
		bool justPressedRight = !m_KeyState[SCAN_CODES::D];
		bool justPressedJump = !m_KeyState[SCAN_CODES::SPACE];
		bool justPressedSprint = !m_KeyState[SCAN_CODES::LEFT_SHIFT];
		bool justPressedDash = !m_KeyState[SCAN_CODES::Q];
		bool justPressedAttack = !m_MouseButtonState[MOUSE_BUTTON::LEFT_DOWN];
		bool justPressedShoot = !m_MouseButtonState[MOUSE_BUTTON::RIGHT_DOWN];
		bool justPressedChoose = !m_KeyState[SCAN_CODES::F];

		// Switch 0 is the directional buttons on a DualShock 4
		switch (switchesArray.at(0))
		{
		case GameControllerSwitchPosition::Up:
			m_KeyState[SCAN_CODES::W] = true;
			m_KeyState[SCAN_CODES::A] = false;
			m_KeyState[SCAN_CODES::S] = false;
			m_KeyState[SCAN_CODES::D] = false;
			break;
		case GameControllerSwitchPosition::Left:
			m_KeyState[SCAN_CODES::W] = false;
			m_KeyState[SCAN_CODES::A] = true;
			m_KeyState[SCAN_CODES::S] = false;
			m_KeyState[SCAN_CODES::D] = false;
			break;
		case GameControllerSwitchPosition::Down:
			m_KeyState[SCAN_CODES::W] = false;
			m_KeyState[SCAN_CODES::A] = false;
			m_KeyState[SCAN_CODES::S] = true;
			m_KeyState[SCAN_CODES::D] = false;
			break;
		case GameControllerSwitchPosition::Right:
			m_KeyState[SCAN_CODES::W] = false;
			m_KeyState[SCAN_CODES::A] = false;
			m_KeyState[SCAN_CODES::S] = false;
			m_KeyState[SCAN_CODES::D] = true;
			break;
		case GameControllerSwitchPosition::UpLeft:
			m_KeyState[SCAN_CODES::W] = true;
			m_KeyState[SCAN_CODES::A] = true;
			m_KeyState[SCAN_CODES::S] = false;
			m_KeyState[SCAN_CODES::D] = false;
			break;
		case GameControllerSwitchPosition::UpRight:
			m_KeyState[SCAN_CODES::W] = true;
			m_KeyState[SCAN_CODES::A] = false;
			m_KeyState[SCAN_CODES::S] = false;
			m_KeyState[SCAN_CODES::D] = true;
			break;
		case GameControllerSwitchPosition::DownLeft:
			m_KeyState[SCAN_CODES::W] = false;
			m_KeyState[SCAN_CODES::A] = true;
			m_KeyState[SCAN_CODES::S] = true;
			m_KeyState[SCAN_CODES::D] = false;
			break;
		case GameControllerSwitchPosition::DownRight:
			m_KeyState[SCAN_CODES::W] = false;
			m_KeyState[SCAN_CODES::A] = false;
			m_KeyState[SCAN_CODES::S] = true;
			m_KeyState[SCAN_CODES::D] = true;
			break;
		default:
			// Axis 0 is horizontal movement of left joystick. Axis 1 is vertical movement of left joystick
			m_KeyState[SCAN_CODES::W] = (axisArray.at(1) < 0.45 && axisArray.at(0) > 0.05 && axisArray.at(0) < 0.95);
			m_KeyState[SCAN_CODES::A] = (axisArray.at(0) < 0.45 && axisArray.at(1) > 0.05 && axisArray.at(1) < 0.95);
			m_KeyState[SCAN_CODES::S] = (axisArray.at(1) > 0.55 && axisArray.at(0) > 0.05 && axisArray.at(0) < 0.95);
			m_KeyState[SCAN_CODES::D] = (axisArray.at(0) > 0.55 && axisArray.at(1) > 0.05 && axisArray.at(1) < 0.95);
		}
		
		// Button 0 is Square
		m_KeyState[SCAN_CODES::F] = buttonsArray.at(0);
		// Button 1 is Cross
		m_KeyState[SCAN_CODES::SPACE] = buttonsArray.at(1);
		// Button 2 is Circle
		// Button 3 is Triangle
		// Button 4 is Left Bumper
		m_MouseButtonState[MOUSE_BUTTON::LEFT_DOWN] = buttonsArray.at(4);
		// Button 5 is Right Bumper
		m_MouseButtonState[MOUSE_BUTTON::RIGHT_DOWN] = buttonsArray.at(5);
		// Button 6 is Left Trigger
		m_KeyState[SCAN_CODES::LEFT_SHIFT] = buttonsArray.at(6);
		// Button 7 is Right Trigger
		m_KeyState[SCAN_CODES::Q] = buttonsArray.at(7);
		// Button 8 is Share
		// Button 9 is Options
		// Button 10 is Left Stick
		// Button 11 is Right Stick
		// Button 12 is PS Button
		// Button 13 is Touchpad Click

		if (justPressedUp && m_KeyState[SCAN_CODES::W])
		{
			EventBus::GetInstance().Publish(&MovementInput(SCAN_CODES::W, true, false));
		}
		else if (!justPressedUp && !m_KeyState[SCAN_CODES::W])
		{
			EventBus::GetInstance().Publish(&MovementInput(SCAN_CODES::W, false, false));
		}
		if (justPressedLeft && m_KeyState[SCAN_CODES::A])
		{
			EventBus::GetInstance().Publish(&MovementInput(SCAN_CODES::A, true, false));
		}
		else if (!justPressedLeft && !m_KeyState[SCAN_CODES::A])
		{
			EventBus::GetInstance().Publish(&MovementInput(SCAN_CODES::A, false, false));
		}
		if (justPressedDown && m_KeyState[SCAN_CODES::S])
		{
			EventBus::GetInstance().Publish(&MovementInput(SCAN_CODES::S, true, false));
		}
		else if (!justPressedDown && !m_KeyState[SCAN_CODES::S])
		{
			EventBus::GetInstance().Publish(&MovementInput(SCAN_CODES::S, false, false));
		}
		if (justPressedRight && m_KeyState[SCAN_CODES::D])
		{
			EventBus::GetInstance().Publish(&MovementInput(SCAN_CODES::D, true, false));
		}
		else if (!justPressedRight && !m_KeyState[SCAN_CODES::D])
		{
			EventBus::GetInstance().Publish(&MovementInput(SCAN_CODES::D, false, false));
		}
		if (justPressedJump && m_KeyState[SCAN_CODES::SPACE])
		{
			EventBus::GetInstance().Publish(&MovementInput(SCAN_CODES::SPACE, true, false));
		}
		else if (!justPressedJump && !m_KeyState[SCAN_CODES::SPACE])
		{
			EventBus::GetInstance().Publish(&MovementInput(SCAN_CODES::SPACE, false, false));
		}
		if (justPressedSprint && m_KeyState[SCAN_CODES::LEFT_SHIFT])
		{
			EventBus::GetInstance().Publish(&ModifierInput(SCAN_CODES::LEFT_SHIFT, true));
		}
		else if (!justPressedSprint && !m_KeyState[SCAN_CODES::LEFT_SHIFT])
		{
			EventBus::GetInstance().Publish(&ModifierInput(SCAN_CODES::LEFT_SHIFT, false));
		}
		if (justPressedDash && m_KeyState[SCAN_CODES::Q])
		{
			EventBus::GetInstance().Publish(&MovementInput(SCAN_CODES::Q, true, false));
		}
		else if (!justPressedDash && !m_KeyState[SCAN_CODES::Q])
		{
			EventBus::GetInstance().Publish(&MovementInput(SCAN_CODES::Q, false, false));
		}
		if (justPressedAttack && m_MouseButtonState[MOUSE_BUTTON::LEFT_DOWN])
		{
			EventBus::GetInstance().Publish(&MouseClick(MOUSE_BUTTON::LEFT_DOWN, true));
		}
		else if (!justPressedAttack && !m_MouseButtonState[MOUSE_BUTTON::LEFT_DOWN])
		{
			EventBus::GetInstance().Publish(&MouseRelease(MOUSE_BUTTON::LEFT_DOWN, false));
		}
		if (justPressedShoot && m_MouseButtonState[MOUSE_BUTTON::RIGHT_DOWN])
		{
			EventBus::GetInstance().Publish(&MouseClick(MOUSE_BUTTON::RIGHT_DOWN, true));
		}
		else if (!justPressedShoot && !m_MouseButtonState[MOUSE_BUTTON::RIGHT_DOWN])
		{
			EventBus::GetInstance().Publish(&MouseRelease(MOUSE_BUTTON::RIGHT_DOWN, false));
		}
		if (justPressedChoose && m_KeyState[SCAN_CODES::F])
		{
			Scene* scene = SceneManager::GetInstance().GetActiveScene();
			if (scene->GetName() == "ShopScene")
			{
				EventBus::GetInstance().Publish(&shopGUIStateChange());
			}
		}

		// Axis 2 is horizontal movement of right joystick. Axis 5 is vertical movement of right joystick
		float moveX = 5 * (axisArray.at(2) - 0.5);
		float moveY = 2.5 * (axisArray.at(5) - 0.5);

		if (std::abs(moveX) > 0.3f || std::abs(moveY) > 0.3f)
		{
			EventBus::GetInstance().Publish(&MouseMovement(moveX, moveY));
		}
	}
}

Input::Input()
{
	m_KeyTimer[SCAN_CODES::W] = m_KeyTimer[SCAN_CODES::A] = m_KeyTimer[SCAN_CODES::S] = m_KeyTimer[SCAN_CODES::D] = m_KeyTimer[SCAN_CODES::SPACE] = std::chrono::system_clock::now();
}
