#ifndef BASECAMERA_H
#define BASECAMERA_H

#include "Core.h"
#include <DirectXMath.h>

class BaseCamera
{
public:
	BaseCamera(DirectX::XMVECTOR position = { 0.0, 3.0, -5.0, 1.0f }, DirectX::XMVECTOR lookAt = { 0.0f, 0.0f, 1.0f, 1.0f });
	virtual ~BaseCamera();

	void Update(double dt);

	void SetPosition(float x, float y, float z);
	void SetLookAt(float x, float y, float z);

	DirectX::XMFLOAT3 GetPosition() const;
	float3 GetPositionFloat3() const;

	DirectX::XMFLOAT3 GetLookAt() const;
	float3 GetLookAtFloat3() const;

	const DirectX::XMMATRIX* GetViewMatrix() const;
	const DirectX::XMMATRIX* GetViewMatrixInverse() const;
	virtual const DirectX::XMMATRIX* GetViewProjection() const = 0;
	virtual const DirectX::XMMATRIX* GetViewProjectionTranposed() const = 0;

protected:
	DirectX::XMVECTOR rightVector;
	DirectX::XMVECTOR eyeVector;
	DirectX::XMVECTOR atVector;
	DirectX::XMVECTOR upVector;

	DirectX::XMMATRIX viewMatrix;
	DirectX::XMMATRIX viewMatrixInverse;

	virtual void UpdateSpecific(double dt = 0.0) = 0;
};

#endif
