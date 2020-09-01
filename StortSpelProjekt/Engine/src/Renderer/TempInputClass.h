#ifndef TEMPINPUTCLASS_H
#define TEMPINPUTCLASS_H

#pragma comment (lib, "dinput8.lib")
#pragma comment (lib, "dxguid.lib")
#include <dinput.h>

class TempInputClass
{
public:
	TempInputClass();
	virtual ~TempInputClass();

    void InitDirectInput(HINSTANCE hInstance, HWND hwnd);
    void DetectInput(
        double dt,
        float *mfb,
        float *mlr,
        float *mud,
        float *camYaw,
        float *camPitch);
private:
    float m_MovementSpeed = 10.0f;
    IDirectInputDevice8* m_pKeyboard;
    IDirectInputDevice8* m_pMouse;
    LPDIRECTINPUT8 m_DirectInput;
    DIMOUSESTATE m_MouseLastState;

   
};

#endif