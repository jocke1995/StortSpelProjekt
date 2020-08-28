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

    float movementSpeed = 10.0f;

    IDirectInputDevice8* keyboard;
    IDirectInputDevice8* mouse;
    DIMOUSESTATE mouseLastState;
    LPDIRECTINPUT8 DirectInput;

   
};

#endif