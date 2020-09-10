#ifndef ORTHOGRAPHICCAMERA_H
#define ORTHOGRAPHICCAMERA_H

#include "BaseCamera.h"

class OrthographicCamera : public BaseCamera
{
public:
    OrthographicCamera(
        DirectX::XMVECTOR position,
        DirectX::XMVECTOR direction,
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
    float m_Left  = 0;
    float m_Right = 0;
    float m_Bot   = 0;
    float m_Top   = 0;
    float m_NearZ = 0;
    float m_FarZ  = 0;

    DirectX::XMMATRIX m_ProjMatrix;
    DirectX::XMMATRIX m_ViewProjMatrix;
    DirectX::XMMATRIX m_ViewProjTranposedMatrix;

    void updateSpecific(double dt);
};

#endif
