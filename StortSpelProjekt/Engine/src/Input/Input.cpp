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

	EventBus::GetInstance().Subscribe(this, &Input::onReset);
}

void Input::RegisterControllers()
{
	if (RawGameController::RawGameControllers().Size() > 0)
	{
		if (m_RawGameControllers.size() < 1)
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
			Log::Print("%d, %d\n", m_pMainController->HardwareVendorId(), m_pMainController->HardwareProductId());
		}
		if (m_pMainController->HardwareVendorId() == 1356)	// Dualshock controller
		{
			m_pControllerType = CONTROLLER_TYPE::DUALSHOCK;
		}
		else if (m_pMainController->HardwareVendorId() == 1118)	// XBOX controller
		{
			m_pControllerType = CONTROLLER_TYPE::XBOX;
		}
		else
		{
			m_pControllerType = CONTROLLER_TYPE::NONE;
		}
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
	if (key == SCAN_CODES::W || key == SCAN_CODES::A || key == SCAN_CODES::S || key == SCAN_CODES::D || key == SCAN_CODES::Q || key == SCAN_CODES::E || key == SCAN_CODES::LEFT_SHIFT || key == SCAN_CODES::RIGHT_SHIFT || key == SCAN_CODES::SPACE)
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
	else if (key == SCAN_CODES::F)
	{
		if (justPressed && !m_IsPaused)
		{
			// Check if we are in the ShopScene
			Scene* scene = SceneManager::GetInstance().GetActiveScene();
			if (scene->GetName() == "ShopScene")
			{
				EventBus::GetInstance().Publish(&shopGUIStateChange());
			}
		}
	}
	else if (key == SCAN_CODES::ESCAPE && justPressed)
	{
		Scene* scene = SceneManager::GetInstance().GetActiveScene();
		if ((scene->GetName() == "ShopScene" && !Player::GetInstance().GetShop()->IsShop2DGUIDisplaying()) || scene->GetName() == "GameScene")
		{
			m_IsPaused = !m_IsPaused;
			EventBus::GetInstance().Publish(&PauseGame(m_IsPaused));
			ShowCursor(m_IsPaused);
		}
		else if (scene->GetName() == "ShopScene")
		{
			EventBus::GetInstance().Publish(&shopGUIStateChange());
			m_IsPaused = !m_IsPaused;
			EventBus::GetInstance().Publish(&PauseGame(m_IsPaused));
			ShowCursor(m_IsPaused);
		}
	}
	else if (DEVELOPERMODE_FREECAM && key == SCAN_CODES::LEFT_CTRL && justPressed && !m_IsPaused)
	{
		EventBus::GetInstance().Publish(&ModifierInput(key, pressed));
	}
}

void Input::SetMouseButtonState(MOUSE_BUTTON button, bool pressed)
{
	m_MouseButtonState[button] = pressed;
	switch (pressed) {
	case true:
		EventBus::GetInstance().Publish(&MouseClick(button, pressed));
		if (SceneManager::GetInstance().GetActiveScene()->GetName() == "MainMenuScene")
		{
			m_pControllerType = CONTROLLER_TYPE::NONE;
		}
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
	if (Player::GetInstance().GetShop()->IsShop2DGUIDisplaying() == false && !m_IsPaused)
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

bool Input::IsPaused()
{
	return m_IsPaused;
}

void Input::ReadControllerInput(double dt)
{
	if (m_pControllerType == CONTROLLER_TYPE::DUALSHOCK)
	{
		Scene* scene = SceneManager::GetInstance().GetActiveScene();
		bool useMouse = m_IsPaused || scene->GetName() == "MainMenuScene" || scene->GetName() == "gameOverScene" || scene->GetName() == "OptionScene" || (scene->GetName() == "ShopScene" && Player::GetInstance().GetShop()->IsShop2DGUIDisplaying());
		std::array<bool, 20> buttonsArray;
		std::array<double, 10> axisArray;
		std::array<GameControllerSwitchPosition, 5> switchesArray;

		m_pMainController->GetCurrentReading(buttonsArray, switchesArray, axisArray);

		bool justPressedUp = !m_KeyState[SCAN_CODES::W];
		bool justPressedLeft = !m_KeyState[SCAN_CODES::A];
		bool justPressedDown = !m_KeyState[SCAN_CODES::S];
		bool justPressedRight = !m_KeyState[SCAN_CODES::D];
		bool justPressedJump = !m_KeyState[SCAN_CODES::SPACE];
		bool justPressedDashLeft = !m_KeyState[SCAN_CODES::LEFT_SHIFT];
		bool justPressedDashRight = !m_KeyState[SCAN_CODES::RIGHT_SHIFT];
		bool justPressedAttack = !m_MouseButtonState[MOUSE_BUTTON::LEFT_DOWN];
		bool justPressedShoot = !m_MouseButtonState[MOUSE_BUTTON::RIGHT_DOWN];
		bool justPressedChoose = !m_KeyState[SCAN_CODES::F];
		bool justPressedBack = !m_KeyState[SCAN_CODES::U];
		bool justPressedPause = !m_KeyState[SCAN_CODES::ESCAPE];

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
		m_KeyState[SCAN_CODES::U] = buttonsArray.at(2);
		// Button 3 is Triangle
		// Button 4 is Left Bumper
		m_MouseButtonState[MOUSE_BUTTON::LEFT_DOWN] = buttonsArray.at(4);
		// Button 5 is Right Bumper
		m_MouseButtonState[MOUSE_BUTTON::RIGHT_DOWN] = buttonsArray.at(5);
		// Button 6 is Left Trigger
		m_KeyState[SCAN_CODES::LEFT_SHIFT] = buttonsArray.at(6);
		// Button 7 is Right Trigger
		m_KeyState[SCAN_CODES::RIGHT_SHIFT] = buttonsArray.at(7);
		// Button 8 is Share
		// Button 9 is Options
		m_KeyState[SCAN_CODES::ESCAPE] = buttonsArray.at(9);
		// Button 10 is Left Stick
		// Button 11 is Right Stick
		// Button 12 is PS Button
		// Button 13 is Touchpad Click

		bool shopIsClosed = (scene->GetName() == "ShopScene" && !Player::GetInstance().GetShop()->IsShop2DGUIDisplaying()) || scene->GetName() == "GameScene";

		if (justPressedUp && m_KeyState[SCAN_CODES::W] && shopIsClosed)
		{
			EventBus::GetInstance().Publish(&MovementInput(SCAN_CODES::W, true, false));
		}
		else if (!justPressedUp && !m_KeyState[SCAN_CODES::W] && shopIsClosed)
		{
			EventBus::GetInstance().Publish(&MovementInput(SCAN_CODES::W, false, false));
		}
		if (justPressedLeft && m_KeyState[SCAN_CODES::A] && shopIsClosed)
		{
			EventBus::GetInstance().Publish(&MovementInput(SCAN_CODES::A, true, false));
		}
		else if (!justPressedLeft && !m_KeyState[SCAN_CODES::A] && shopIsClosed)
		{
			EventBus::GetInstance().Publish(&MovementInput(SCAN_CODES::A, false, false));
		}
		if (justPressedDown && m_KeyState[SCAN_CODES::S] && shopIsClosed)
		{
			EventBus::GetInstance().Publish(&MovementInput(SCAN_CODES::S, true, false));
		}
		else if (!justPressedDown && !m_KeyState[SCAN_CODES::S] && shopIsClosed)
		{
			EventBus::GetInstance().Publish(&MovementInput(SCAN_CODES::S, false, false));
		}
		if (justPressedRight && m_KeyState[SCAN_CODES::D] && shopIsClosed)
		{
			EventBus::GetInstance().Publish(&MovementInput(SCAN_CODES::D, true, false));
		}
		else if (!justPressedRight && !m_KeyState[SCAN_CODES::D] && shopIsClosed)
		{
			EventBus::GetInstance().Publish(&MovementInput(SCAN_CODES::D, false, false));
		}
		if (justPressedJump && m_KeyState[SCAN_CODES::SPACE] && shopIsClosed)
		{
			EventBus::GetInstance().Publish(&MovementInput(SCAN_CODES::SPACE, true, false));
		}
		else if (!justPressedJump && !m_KeyState[SCAN_CODES::SPACE] && shopIsClosed)
		{
			EventBus::GetInstance().Publish(&MovementInput(SCAN_CODES::SPACE, false, false));
		}
		if (justPressedDashLeft && m_KeyState[SCAN_CODES::LEFT_SHIFT] && shopIsClosed)
		{
			EventBus::GetInstance().Publish(&MovementInput(SCAN_CODES::LEFT_SHIFT, true, false));
		}
		else if (!justPressedDashLeft && !m_KeyState[SCAN_CODES::LEFT_SHIFT] && shopIsClosed)
		{
			EventBus::GetInstance().Publish(&MovementInput(SCAN_CODES::LEFT_SHIFT, false, false));
		}
		if (justPressedDashRight && m_KeyState[SCAN_CODES::RIGHT_SHIFT] && shopIsClosed)
		{
			EventBus::GetInstance().Publish(&MovementInput(SCAN_CODES::RIGHT_SHIFT, true, false));
		}
		else if (!justPressedDashRight && !m_KeyState[SCAN_CODES::RIGHT_SHIFT] && shopIsClosed)
		{
			EventBus::GetInstance().Publish(&MovementInput(SCAN_CODES::RIGHT_SHIFT, false, false));
		}
		if ((justPressedAttack && m_MouseButtonState[MOUSE_BUTTON::LEFT_DOWN] && !m_IsPaused && shopIsClosed) || (justPressedChoose && m_KeyState[SCAN_CODES::F] && useMouse))
		{
			EventBus::GetInstance().Publish(&MouseClick(MOUSE_BUTTON::LEFT_DOWN, true));
		}
		else if ((!justPressedAttack && !m_MouseButtonState[MOUSE_BUTTON::LEFT_DOWN] && !m_IsPaused && shopIsClosed) || (!justPressedChoose && !m_KeyState[SCAN_CODES::F] && useMouse))
		{
			EventBus::GetInstance().Publish(&MouseRelease(MOUSE_BUTTON::LEFT_DOWN, false));
		}
		if (justPressedShoot && m_MouseButtonState[MOUSE_BUTTON::RIGHT_DOWN] && shopIsClosed)
		{
			EventBus::GetInstance().Publish(&MouseClick(MOUSE_BUTTON::RIGHT_DOWN, true));
		}
		else if (!justPressedShoot && !m_MouseButtonState[MOUSE_BUTTON::RIGHT_DOWN] && shopIsClosed)
		{
			EventBus::GetInstance().Publish(&MouseRelease(MOUSE_BUTTON::RIGHT_DOWN, false));
		}
		if (justPressedChoose && m_KeyState[SCAN_CODES::F])
		{
			Scene* scene = SceneManager::GetInstance().GetActiveScene();
			if (scene->GetName() == "ShopScene" && shopIsClosed && !m_IsPaused)
			{
				EventBus::GetInstance().Publish(&shopGUIStateChange());
			}
		}
		if (justPressedBack && m_KeyState[SCAN_CODES::U])
		{
			if (scene->GetName() == "ShopScene" && !shopIsClosed)
			{
				EventBus::GetInstance().Publish(&shopGUIStateChange());
			}
			else if (m_IsPaused)
			{
				m_IsPaused = !m_IsPaused;
				EventBus::GetInstance().Publish(&PauseGame(m_IsPaused));
				ShowCursor(m_IsPaused);
			}
		}
		if (justPressedPause && m_KeyState[SCAN_CODES::ESCAPE])
		{
			if (shopIsClosed)
			{
				m_IsPaused = !m_IsPaused;
				EventBus::GetInstance().Publish(&PauseGame(m_IsPaused));
				ShowCursor(m_IsPaused);
			}
			else if (scene->GetName() == "ShopScene")
			{
				EventBus::GetInstance().Publish(&shopGUIStateChange());
				m_IsPaused = !m_IsPaused;
				EventBus::GetInstance().Publish(&PauseGame(m_IsPaused));
				ShowCursor(m_IsPaused);
			}
		}

		// Axis 2 is horizontal movement of right joystick. Axis 5 is vertical movement of right joystick
		float3 move = { 5 * (axisArray.at(2) - 0.5), 2.5 * (axisArray.at(5) - 0.5), 0.0f };

		if (std::abs(move.x) > 0.3f || std::abs(move.y) > 0.3f)
		{
			EventBus::GetInstance().Publish(&MouseMovement(move.x, move.y));
		}

		if (useMouse)
		{
			move.x = (axisArray.at(0) - 0.5);
			move.y = (axisArray.at(1) - 0.5);

			if (std::abs(move.x) > 0.2f || std::abs(move.y) > 0.2f)
			{
				POINT cursorPos;
				GetCursorPos(&cursorPos);

				cursorPos.x += move.x * 1000.0 * dt;
				cursorPos.y += move.y * 1000.0 * dt;

				SetCursorPos(cursorPos.x, cursorPos.y);
			}
		}
	}
	else if (m_pControllerType == CONTROLLER_TYPE::XBOX)
	{
		Scene* scene = SceneManager::GetInstance().GetActiveScene();
		bool useMouse = m_IsPaused || scene->GetName() == "MainMenuScene" || scene->GetName() == "gameOverScene" || scene->GetName() == "OptionScene" || (scene->GetName() == "ShopScene" && Player::GetInstance().GetShop()->IsShop2DGUIDisplaying());
		std::array<bool, 20> buttonsArray;
		std::array<double, 10> axisArray;
		std::array<GameControllerSwitchPosition, 5> switchesArray;

		m_pMainController->GetCurrentReading(buttonsArray, switchesArray, axisArray);

		bool justPressedUp = !m_KeyState[SCAN_CODES::W];
		bool justPressedLeft = !m_KeyState[SCAN_CODES::A];
		bool justPressedDown = !m_KeyState[SCAN_CODES::S];
		bool justPressedRight = !m_KeyState[SCAN_CODES::D];
		bool justPressedJump = !m_KeyState[SCAN_CODES::SPACE];
		bool justPressedDashLeft = !m_KeyState[SCAN_CODES::LEFT_SHIFT];
		bool justPressedDashRight = !m_KeyState[SCAN_CODES::RIGHT_SHIFT];
		bool justPressedAttack = !m_MouseButtonState[MOUSE_BUTTON::LEFT_DOWN];
		bool justPressedShoot = !m_MouseButtonState[MOUSE_BUTTON::RIGHT_DOWN];
		bool justPressedChoose = !m_KeyState[SCAN_CODES::F];
		bool justPressedBack = !m_KeyState[SCAN_CODES::U];
		bool justPressedPause = !m_KeyState[SCAN_CODES::ESCAPE];

		
		// Axis 0 is horizontal movement of left joystick. Axis 1 is vertical movement of left joystick
		m_KeyState[SCAN_CODES::W] = (axisArray.at(0) > 0.65 && axisArray.at(1) > 0.02 && axisArray.at(1) < 0.98);
		m_KeyState[SCAN_CODES::A] = (axisArray.at(1) < 0.35 && axisArray.at(0) > 0.02 && axisArray.at(0) < 0.98);
		m_KeyState[SCAN_CODES::S] = (axisArray.at(0) < 0.35 && axisArray.at(1) > 0.02 && axisArray.at(1) < 0.98);
		m_KeyState[SCAN_CODES::D] = (axisArray.at(1) > 0.65 && axisArray.at(0) > 0.02 && axisArray.at(0) < 0.98);

		m_KeyState[SCAN_CODES::F] = buttonsArray.at(2);	// X
		// Button 1 is Cross
		m_KeyState[SCAN_CODES::SPACE] = buttonsArray.at(0);	// A
		// Button 2 is Circle
		m_KeyState[SCAN_CODES::U] = buttonsArray.at(1);	// B
		// Button 3 is Triangle
		// Button 4 is Left Bumper
		m_MouseButtonState[MOUSE_BUTTON::LEFT_DOWN] = buttonsArray.at(4);
		// Button 5 is Right Bumper
		m_MouseButtonState[MOUSE_BUTTON::RIGHT_DOWN] = buttonsArray.at(5);
		// Button 6 is Left Trigger
		m_KeyState[SCAN_CODES::LEFT_SHIFT] = axisArray.at(4) > 0.35f;
		// Button 7 is Right Trigger
		m_KeyState[SCAN_CODES::RIGHT_SHIFT] = axisArray.at(5) > 0.35f;
		// Button 8 is Share
		// Button 9 is Options
		m_KeyState[SCAN_CODES::ESCAPE] = buttonsArray.at(7);	// Should be 6 or 7 (Start or select)
		// Button 10 is Left Stick
		// Button 11 is Right Stick
		// Button 12 is PS Button
		// Button 13 is Touchpad Click

		bool shopIsClosed = (scene->GetName() == "ShopScene" && !Player::GetInstance().GetShop()->IsShop2DGUIDisplaying()) || scene->GetName() == "GameScene";

		if (justPressedUp && m_KeyState[SCAN_CODES::W] && shopIsClosed)
		{
			EventBus::GetInstance().Publish(&MovementInput(SCAN_CODES::W, true, false));
		}
		else if (!justPressedUp && !m_KeyState[SCAN_CODES::W] && shopIsClosed)
		{
			EventBus::GetInstance().Publish(&MovementInput(SCAN_CODES::W, false, false));
		}
		if (justPressedLeft && m_KeyState[SCAN_CODES::A] && shopIsClosed)
		{
			EventBus::GetInstance().Publish(&MovementInput(SCAN_CODES::A, true, false));
		}
		else if (!justPressedLeft && !m_KeyState[SCAN_CODES::A] && shopIsClosed)
		{
			EventBus::GetInstance().Publish(&MovementInput(SCAN_CODES::A, false, false));
		}
		if (justPressedDown && m_KeyState[SCAN_CODES::S] && shopIsClosed)
		{
			EventBus::GetInstance().Publish(&MovementInput(SCAN_CODES::S, true, false));
		}
		else if (!justPressedDown && !m_KeyState[SCAN_CODES::S] && shopIsClosed)
		{
			EventBus::GetInstance().Publish(&MovementInput(SCAN_CODES::S, false, false));
		}
		if (justPressedRight && m_KeyState[SCAN_CODES::D] && shopIsClosed)
		{
			EventBus::GetInstance().Publish(&MovementInput(SCAN_CODES::D, true, false));
		}
		else if (!justPressedRight && !m_KeyState[SCAN_CODES::D] && shopIsClosed)
		{
			EventBus::GetInstance().Publish(&MovementInput(SCAN_CODES::D, false, false));
		}
		if (justPressedJump && m_KeyState[SCAN_CODES::SPACE] && shopIsClosed)
		{
			EventBus::GetInstance().Publish(&MovementInput(SCAN_CODES::SPACE, true, false));
		}
		else if (!justPressedJump && !m_KeyState[SCAN_CODES::SPACE] && shopIsClosed)
		{
			EventBus::GetInstance().Publish(&MovementInput(SCAN_CODES::SPACE, false, false));
		}
		if (justPressedDashLeft && m_KeyState[SCAN_CODES::LEFT_SHIFT] && shopIsClosed)
		{
			EventBus::GetInstance().Publish(&MovementInput(SCAN_CODES::LEFT_SHIFT, true, false));
		}
		else if (!justPressedDashLeft && !m_KeyState[SCAN_CODES::LEFT_SHIFT] && shopIsClosed)
		{
			EventBus::GetInstance().Publish(&MovementInput(SCAN_CODES::LEFT_SHIFT, false, false));
		}
		if (justPressedDashRight && m_KeyState[SCAN_CODES::RIGHT_SHIFT] && shopIsClosed)
		{
			EventBus::GetInstance().Publish(&MovementInput(SCAN_CODES::RIGHT_SHIFT, true, false));
		}
		else if (!justPressedDashRight && !m_KeyState[SCAN_CODES::RIGHT_SHIFT] && shopIsClosed)
		{
			EventBus::GetInstance().Publish(&MovementInput(SCAN_CODES::RIGHT_SHIFT, false, false));
		}
		if ((justPressedAttack && m_MouseButtonState[MOUSE_BUTTON::LEFT_DOWN] && !m_IsPaused && shopIsClosed) || (justPressedChoose && m_KeyState[SCAN_CODES::F] && useMouse))
		{
			EventBus::GetInstance().Publish(&MouseClick(MOUSE_BUTTON::LEFT_DOWN, true));
		}
		else if ((!justPressedAttack && !m_MouseButtonState[MOUSE_BUTTON::LEFT_DOWN] && !m_IsPaused && shopIsClosed) || (!justPressedChoose && !m_KeyState[SCAN_CODES::F] && useMouse))
		{
			EventBus::GetInstance().Publish(&MouseRelease(MOUSE_BUTTON::LEFT_DOWN, false));
		}
		if (justPressedShoot && m_MouseButtonState[MOUSE_BUTTON::RIGHT_DOWN] && shopIsClosed)
		{
			EventBus::GetInstance().Publish(&MouseClick(MOUSE_BUTTON::RIGHT_DOWN, true));
		}
		else if (!justPressedShoot && !m_MouseButtonState[MOUSE_BUTTON::RIGHT_DOWN] && shopIsClosed)
		{
			EventBus::GetInstance().Publish(&MouseRelease(MOUSE_BUTTON::RIGHT_DOWN, false));
		}
		if (justPressedChoose && m_KeyState[SCAN_CODES::F])
		{
			Scene* scene = SceneManager::GetInstance().GetActiveScene();
			if (scene->GetName() == "ShopScene" && shopIsClosed && !m_IsPaused)
			{
				EventBus::GetInstance().Publish(&shopGUIStateChange());
			}
		}
		if (justPressedBack && m_KeyState[SCAN_CODES::U])
		{
			if (scene->GetName() == "ShopScene" && !shopIsClosed)
			{
				EventBus::GetInstance().Publish(&shopGUIStateChange());
			}
			else if (m_IsPaused)
			{
				m_IsPaused = !m_IsPaused;
				EventBus::GetInstance().Publish(&PauseGame(m_IsPaused));
				ShowCursor(m_IsPaused);
			}
		}
		if (justPressedPause && m_KeyState[SCAN_CODES::ESCAPE])
		{
			if (shopIsClosed)
			{
				m_IsPaused = !m_IsPaused;
				EventBus::GetInstance().Publish(&PauseGame(m_IsPaused));
				ShowCursor(m_IsPaused);
			}
			else if (scene->GetName() == "ShopScene")
			{
				EventBus::GetInstance().Publish(&shopGUIStateChange());
				m_IsPaused = !m_IsPaused;
				EventBus::GetInstance().Publish(&PauseGame(m_IsPaused));
				ShowCursor(m_IsPaused);
			}
		}

		// Axis 2 is horizontal movement of right joystick. Axis 5 is vertical movement of right joystick
		float3 move = { 5 * (axisArray.at(3) - 0.5), -2.5 * (axisArray.at(2) - 0.5), 0.0f };

		if (std::abs(move.x) > 0.5f || std::abs(move.y) > 0.5f)
		{
			EventBus::GetInstance().Publish(&MouseMovement(move.x, move.y));
		}

		if (useMouse)
		{
			if (axisArray.at(0) > EPSILON || axisArray.at(1) > EPSILON)
			{
				move.x = (axisArray.at(1) - 0.5);
				move.y = -(axisArray.at(0) - 0.5);

				if (std::abs(move.x) > 0.2f || std::abs(move.y) > 0.2f)
				{
					POINT cursorPos;
					GetCursorPos(&cursorPos);

					cursorPos.x += move.x * 1000.0 * dt;
					cursorPos.y += move.y * 1000.0 * dt;

					SetCursorPos(cursorPos.x, cursorPos.y);
				}
			}
		}
	}
}

Input::Input()
{
	m_KeyTimer[SCAN_CODES::W] = m_KeyTimer[SCAN_CODES::A] = m_KeyTimer[SCAN_CODES::S] = m_KeyTimer[SCAN_CODES::D] = m_KeyTimer[SCAN_CODES::SPACE] = std::chrono::system_clock::now();
	m_IsPaused = false;
}

void Input::onReset(ResetGame* evnt)
{
	if (m_IsPaused)
	{
		m_IsPaused = !m_IsPaused;
		EventBus::GetInstance().Publish(&PauseGame(m_IsPaused));
	}
}
