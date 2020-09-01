#ifndef ORTHOGRAPHICCAMERA_H
#define ORTHOGRAPHICCAMERA_H

#include "BaseCamera.h"

class OrthographicCamera : public BaseCamera
{
public:
    OrthographicCamera(
        DirectX::XMVECTOR position,
        DirectX::XMVECTOR lookAt,
        float left = -30.0f,
        float right = 30.0f,
        float bot = -30.0f,
        float top = 30.0f,
        float nearZ = 0.01f,
        float farZ = 1000.0f);
    virtual ~OrthographicCamera();

    const DirectX::XMMATRIX* GetViewProjection() const;
    const DirectX::XMMATRIX* GetViewProjectionTranposed() const;

private:
    float left  = 0;
    float right = 0;
    float bot   = 0;
    float top   = 0;
    float nearZ = 0;
    float farZ  = 0;

    DirectX::XMMATRIX projMatrix;

    DirectX::XMMATRIX viewProjMatrix;
    DirectX::XMMATRIX viewProjTranposedMatrix;

    void UpdateSpecific(double dt);
};

#endif
