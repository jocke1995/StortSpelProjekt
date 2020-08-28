#ifndef BASECAMERA_H
#define BASECAMERA_H

#include <DirectXMath.h>
#include "Core.h"

using namespace DirectX;

enum class CAMERA_TYPE
{
	PERSPECTIVE,
	ORTHOGRAPHIC,
	NUM_CAMERA_TYPES,
	UNDEFINED
};

class BaseCamera
{
public:
	BaseCamera(XMVECTOR position = { 0.0, 3.0, -5.0, 1.0f }, XMVECTOR lookAt = { 0.0f, 0.0f, 1.0f, 1.0f });
	virtual ~BaseCamera();

	void Update(double dt);

	void SetPosition(float x, float y, float z);
	void SetLookAt(float x, float y, float z);

	XMFLOAT3 GetPosition() const;
	float3 GetPositionFloat3() const;

	XMFLOAT3 GetLookAt() const;
	float3 GetLookAtFloat3() const;

	const XMMATRIX* GetViewMatrix() const;
	const XMMATRIX* GetViewMatrixInverse() const;
	virtual const XMMATRIX* GetViewProjection() const = 0;
	virtual const XMMATRIX* GetViewProjectionTranposed() const = 0;

protected:
	XMVECTOR rightVector;
	XMVECTOR eyeVector;
	XMVECTOR atVector;
	XMVECTOR upVector;

	XMMATRIX viewMatrix;
	XMMATRIX viewMatrixInverse;

	virtual void UpdateSpecific(double dt = 0.0) = 0;
};

#endif