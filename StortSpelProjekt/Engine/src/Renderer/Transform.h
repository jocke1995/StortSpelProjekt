#ifndef TRANSFORM_H
#define TRANSFORM_H

#include "EngineMath.h"

class Transform
{
public:
	Transform();
	~Transform();

	void SetPosition(float x, float y, float z);
	void SetPosition(DirectX::XMFLOAT3 pos);
	
	void RotateX(float radians);
	void RotateY(float radians);
	void RotateZ(float radians);

	void SetScale(float scale);
	void SetScale(float x, float y, float z);
	void IncreaseScaleByPercent(float scale);

	void UpdateWorldMatrix();

	DirectX::XMMATRIX* GetWorldMatrix();
	DirectX::XMMATRIX* GetWorldMatrixTransposed();

	DirectX::XMFLOAT3 GetPositionXMFLOAT3() const;
	float3 GetPositionFloat3() const;

private:
	DirectX::XMMATRIX worldMat;
	DirectX::XMMATRIX worldMatTransposed;

	DirectX::XMFLOAT3 position;
	DirectX::XMFLOAT3 scale;

	DirectX::XMMATRIX rotXMat;
	DirectX::XMMATRIX rotYMat;
	DirectX::XMMATRIX rotZMat;
	DirectX::XMMATRIX rotationMat;
};

#endif