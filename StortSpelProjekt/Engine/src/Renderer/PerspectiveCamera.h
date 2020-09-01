#ifndef PERSPECTIVECAMERA_H
#define PERSPECTIVECAMERA_H

#include "BaseCamera.h"

class TempInputClass;

class PerspectiveCamera : public BaseCamera
{
public:
    // Temp constructor for input (USED ONCE ATM)
    PerspectiveCamera(HINSTANCE hInstance, HWND hwnd);

    // main constructor
    PerspectiveCamera(DirectX::XMVECTOR position, DirectX::XMVECTOR lookAt,
        double fov = 45.0f,
        double aspectRatio = 16.0f / 9.0f,
        double zNear = 0.1f,
        double zFar = 1000.0f);

	virtual ~PerspectiveCamera();

    const DirectX::XMMATRIX* GetViewProjection() const;
    const DirectX::XMMATRIX* GetViewProjectionTranposed() const;

private:
    float m_MoveLeftRight = 0.0f;
    float m_MoveForwardBackward = 0.0f;
    float m_MoveUpDown = 0.0f;
    float m_CamYaw = 0.0f;
    float m_CamPitch = 0.0f;

    DirectX::XMMATRIX m_ProjMatrix;
    DirectX::XMMATRIX m_ViewProjMatrix;
    DirectX::XMMATRIX m_ViewProjTranposedMatrix;

    double m_Fov = 0.0f;
    double m_AspectRatio = 0.0f;
    double m_ZNear = 0.0f;
    double m_ZFar = 0.0f;

    void init(
        double fov = 45.0f,
        double aspectRatio = 16.0f / 9.0f,
        double zNear = 0.1f,
        double zFar = 1000.0f);

    void updateSpecific(double dt);
    void updateCameraMovement();



    //  ------------------------ TEMPORARY IN THIS CLASS ------------------------
    bool m_TempHasInputObject = false;
    TempInputClass* m_pTempInputClass = nullptr;
};

#endif