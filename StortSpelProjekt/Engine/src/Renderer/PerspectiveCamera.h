#ifndef PERSPECTIVECAMERA_H
#define PERSPECTIVECAMERA_H

#include "TempInputClass.h"

#include "BaseCamera.h"

class PerspectiveCamera : public BaseCamera
{
public:
    // Temp constructor for input (USED ONCE ATM)
    PerspectiveCamera(HINSTANCE hInstance, HWND hwnd);

    // main constructor
    PerspectiveCamera(XMVECTOR position, XMVECTOR lookAt,
        double fov = 45.0f,
        double aspectRatio = 16.0f / 9.0f,
        double zNear = 0.1f,
        double zFar = 1000.0f);

	virtual ~PerspectiveCamera();

    const XMMATRIX* GetViewProjection() const;
    const XMMATRIX* GetViewProjectionTranposed() const;

private:
    float moveLeftRight = 0.0f;
    float moveForwardBackward = 0.0f;
    float moveUpDown = 0.0f;
    float camYaw = 0.0f;
    float camPitch = 0.0f;

    XMMATRIX projMatrix;
    XMMATRIX viewProjMatrix;
    XMMATRIX viewProjTranposedMatrix;

    double fov = 0.0f;
    double aspectRatio = 0.0f;
    double zNear = 0.0f;
    double zFar = 0.0f;

    void Init(
        double fov = 45.0f,
        double aspectRatio = 16.0f / 9.0f,
        double zNear = 0.1f,
        double zFar = 1000.0f);

    void UpdateSpecific(double dt);
    void UpdateCameraMovement();

    //  ------------------------ TEMPORARY IN THIS CLASS ------------------------
    bool tempHasInputObject = false;
    TempInputClass* tempInputClass = nullptr;
};

#endif