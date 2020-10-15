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

    // Gets
    const DirectX::XMMATRIX* GetViewProjection() const;
    const DirectX::XMMATRIX* GetViewProjectionTranposed() const;
    
    const float GetLeft() const;
    const float GetRight() const;
    const float GetBot() const;
    const float GetTop() const;
    const float GetNearZ() const;
    const float GetFarZ() const;

    // Sets to modify camera projection
    void SetLeft(float left);
    void SetRight(float right);
    void SetBot(float bot);
    void SetTop(float top);
    void SetNearZ(float nearPlaneDistance);
    void SetFarZ(float farPlaneDistance);

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
