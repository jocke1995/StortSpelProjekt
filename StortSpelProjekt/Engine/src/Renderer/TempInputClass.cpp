#include "stdafx.h"
#include "TempInputClass.h"

TempInputClass::TempInputClass()
{
}

TempInputClass::~TempInputClass()
{
	m_pKeyboard->Unacquire();
	m_pMouse->Unacquire();

	m_DirectInput->Release();
}

void TempInputClass::InitDirectInput(HINSTANCE hInstance, HWND hwnd)
{
	// Init DirectInput
	HRESULT hr = DirectInput8Create(hInstance,
		DIRECTINPUT_VERSION,
		IID_IDirectInput8,
		(void**)&m_DirectInput,
		NULL);
	if (FAILED(hr))
		Log::PrintSeverity(Log::Severity::CRITICAL, "Failed to Create Direct Input\n");

	// Create Device for Keyboard
	hr = m_DirectInput->CreateDevice(GUID_SysKeyboard,
		&m_pKeyboard,
		NULL);
	if (FAILED(hr))
		Log::PrintSeverity(Log::Severity::CRITICAL, "Failed to Create device for keyboard\n");

	// Create Device for Mouse
	hr = m_DirectInput->CreateDevice(GUID_SysMouse,
		&m_pMouse,
		NULL);
	if (FAILED(hr))
		Log::PrintSeverity(Log::Severity::CRITICAL, "Failed to Create device for mouse\n");

	// Init Keyboard
	hr = m_pKeyboard->SetDataFormat(&c_dfDIKeyboard);
	if (FAILED(hr))
		Log::PrintSeverity(Log::Severity::CRITICAL, "Failed to SetDataFormat for keyboard\n");
	hr = m_pKeyboard->SetCooperativeLevel(hwnd, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE);
	if (FAILED(hr))
		Log::PrintSeverity(Log::Severity::CRITICAL, "Failed to SetCooperativeLevel for keyboard\n");

	// Init Mouse
	hr = m_pMouse->SetDataFormat(&c_dfDIMouse);
	if (FAILED(hr))
		Log::PrintSeverity(Log::Severity::CRITICAL, "Failed to SetDataFormat for mouse\n");
	hr = m_pMouse->SetCooperativeLevel(hwnd, DISCL_EXCLUSIVE | DISCL_NOWINKEY | DISCL_FOREGROUND);
	if (FAILED(hr))
		Log::PrintSeverity(Log::Severity::CRITICAL, "Failed to SetCooperativeLevel for mouse\n");

	m_MouseLastState.lX = 0;
	m_MouseLastState.lY = 0;
}

void TempInputClass::DetectInput(
	double dt,
	float* mfb,
	float* mlr,
	float* mud,
	float* camYaw,
	float* camPitch)
{
	DIMOUSESTATE mouseCurrState;

	// Array of possible keys to be pressed
	unsigned char keyboardState[256];

	m_pKeyboard->Acquire();
	m_pMouse->Acquire();	// Takes control over the mouse

	m_pMouse->GetDeviceState(sizeof(DIMOUSESTATE), &mouseCurrState);
	m_pKeyboard->GetDeviceState(sizeof(keyboardState), (LPVOID)&keyboardState);

	if (keyboardState[DIK_W] & 0x80)
		*mfb += m_MovementSpeed * dt;

	if (keyboardState[DIK_S] & 0x80)
		*mfb -= m_MovementSpeed * dt;

	if (keyboardState[DIK_A] & 0x80)
		*mlr += m_MovementSpeed * dt;

	if (keyboardState[DIK_D] & 0x80)
		*mlr -= m_MovementSpeed * dt;

	if (keyboardState[DIK_R] & 0x80)
		*mud += m_MovementSpeed * dt;

	if (keyboardState[DIK_F] & 0x80)
		*mud -= m_MovementSpeed * dt;

	if ((mouseCurrState.lX != m_MouseLastState.lX) || (mouseCurrState.lY != m_MouseLastState.lY))
	{
		*camYaw += m_MouseLastState.lX * 0.001f;
		*camPitch += mouseCurrState.lY * 0.001f;

		// Lock at 90 degrees pitch
		if (*camPitch > 0.89)
		{
			*camPitch = 0.89;
		}
		else if (*camPitch < -0.89)
		{
			*camPitch = -0.89;
		}

		m_MouseLastState = mouseCurrState;
	}
}
