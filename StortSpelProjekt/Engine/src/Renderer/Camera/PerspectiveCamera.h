#ifndef PERSPECTIVECAMERA_H
#define PERSPECTIVECAMERA_H

#include "BaseCamera.h"

class PerspectiveCamera : public BaseCamera
{
public:
    // main constructor
    PerspectiveCamera(DirectX::XMVECTOR position, DirectX::XMVECTOR direction,
        double fov = 45.0f,
        double aspectRatio = 16.0f / 9.0f,
        double zNear = 0.1f,
        double zFar = 1000.0f);

	virtual ~PerspectiveCamera();

    const DirectX::XMMATRIX* GetViewProjection() const;
    const DirectX::XMMATRIX* GetViewProjectionTranposed() const;


    // Used by freelook Camera (when pressing ctrl in the scene, the camera detaches from the player)
    void UpdateMovement(float x, float y, float z);
    void SetMovement(float x, float y, float z);

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

    void updateProjectionMatrix();
    void updateSpecific(double dt);
    void updateCameraMovement(double dt);
};

#endif