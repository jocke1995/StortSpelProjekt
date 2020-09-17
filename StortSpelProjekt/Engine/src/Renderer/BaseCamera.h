#ifndef BASECAMERA_H
#define BASECAMERA_H

#include "Core.h"
#include "EngineMath.h"

class BaseCamera
{
public:
	BaseCamera(DirectX::XMVECTOR position = { 0.0, 4.0, -10.0, 1.0f }, DirectX::XMVECTOR direction = { 0.0f, -2.0f, 10.0f, 1.0f });
	virtual ~BaseCamera();

	void Update(double dt);

	void SetPosition(float x, float y, float z);
	void SetDirection(float x, float y, float z);

	DirectX::XMFLOAT3 GetPosition() const;
	float3 GetPositionFloat3() const;
	DirectX::XMFLOAT3 GetDirection() const;
	float3 GetDirectionFloat3() const;
	DirectX::XMFLOAT3 GetRightVector() const;
	float3 GetRightVectorFloat3() const;
	const DirectX::XMMATRIX* GetViewMatrix() const;
	const DirectX::XMMATRIX* GetViewMatrixInverse() const;
	virtual const DirectX::XMMATRIX* GetViewProjection() const = 0;
	virtual const DirectX::XMMATRIX* GetViewProjectionTranposed() const = 0;

protected:
	DirectX::XMVECTOR m_RightVector;
	DirectX::XMVECTOR m_EyeVector;
	DirectX::XMVECTOR m_DirectionVector;
	DirectX::XMVECTOR m_UpVector;

	DirectX::XMMATRIX m_ViewMatrix;
	DirectX::XMMATRIX m_ViewMatrixInverse;

	virtual void updateSpecific(double dt = 0.0) = 0;
};

#endif
