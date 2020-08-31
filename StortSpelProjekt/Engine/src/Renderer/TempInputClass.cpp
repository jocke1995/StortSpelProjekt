#include "stdafx.h"
#include "TempInputClass.h"

TempInputClass::TempInputClass()
{
}

TempInputClass::~TempInputClass()
{
	this->keyboard->Unacquire();
	this->mouse->Unacquire();

	this->DirectInput->Release();
}

void TempInputClass::InitDirectInput(HINSTANCE hInstance, HWND hwnd)
{
	// Init DirectInput
	HRESULT hr = DirectInput8Create(hInstance,
		DIRECTINPUT_VERSION,
		IID_IDirectInput8,
		(void**)&DirectInput,
		NULL);
	if (FAILED(hr))
		Log::PrintSeverity(Log::Severity::CRITICAL, "Failed to Create Direct Input\n");

	// Create Device for Keyboard
	hr = DirectInput->CreateDevice(GUID_SysKeyboard,
		&this->keyboard,
		NULL);
	if (FAILED(hr))
		Log::PrintSeverity(Log::Severity::CRITICAL, "Failed to Create device for keyboard\n");

	// Create Device for Mouse
	hr = DirectInput->CreateDevice(GUID_SysMouse,
		&this->mouse,
		NULL);
	if (FAILED(hr))
		Log::PrintSeverity(Log::Severity::CRITICAL, "Failed to Create device for mouse\n");

	// Init Keyboard
	hr = this->keyboard->SetDataFormat(&c_dfDIKeyboard);
	if (FAILED(hr))
		Log::PrintSeverity(Log::Severity::CRITICAL, "Failed to SetDataFormat for keyboard\n");
	hr = this->keyboard->SetCooperativeLevel(hwnd, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE);
	if (FAILED(hr))
		Log::PrintSeverity(Log::Severity::CRITICAL, "Failed to SetCooperativeLevel for keyboard\n");

	// Init Mouse
	hr = this->mouse->SetDataFormat(&c_dfDIMouse);
	if (FAILED(hr))
		Log::PrintSeverity(Log::Severity::CRITICAL, "Failed to SetDataFormat for mouse\n");
	hr = this->mouse->SetCooperativeLevel(hwnd, DISCL_EXCLUSIVE | DISCL_NOWINKEY | DISCL_FOREGROUND);
	if (FAILED(hr))
		Log::PrintSeverity(Log::Severity::CRITICAL, "Failed to SetCooperativeLevel for mouse\n");

	mouseLastState.lX = 0;
	mouseLastState.lY = 0;
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

	this->keyboard->Acquire();
	this->mouse->Acquire();	// Takes control over the mouse

	this->mouse->GetDeviceState(sizeof(DIMOUSESTATE), &mouseCurrState);
	this->keyboard->GetDeviceState(sizeof(keyboardState), (LPVOID)&keyboardState);

	if (keyboardState[DIK_W] & 0x80)
		*mfb += this->movementSpeed * dt;

	if (keyboardState[DIK_S] & 0x80)
		*mfb -= this->movementSpeed * dt;

	if (keyboardState[DIK_A] & 0x80)
		*mlr += this->movementSpeed * dt;

	if (keyboardState[DIK_D] & 0x80)
		*mlr -= this->movementSpeed * dt;

	if (keyboardState[DIK_R] & 0x80)
		*mud += this->movementSpeed * dt;

	if (keyboardState[DIK_F] & 0x80)
		*mud -= this->movementSpeed * dt;

	if ((mouseCurrState.lX != mouseLastState.lX) || (mouseCurrState.lY != mouseLastState.lY))
	{
		*camYaw += mouseLastState.lX * 0.001f;
		*camPitch += mouseCurrState.lY * 0.001f;

		mouseLastState = mouseCurrState;
	}
}
