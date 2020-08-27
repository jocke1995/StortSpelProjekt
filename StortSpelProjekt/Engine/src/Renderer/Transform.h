#ifndef TRANSFORM_H
#define TRANSFORM_H

// Intellisense bug
#include "../Headers/stdafx.h"

using namespace DirectX;


class Transform
{
public:
	Transform();
	~Transform();

	void SetPosition(float x, float y, float z);
	void SetPosition(XMFLOAT3 pos);
	
	void RotateX(float radians);
	void RotateY(float radians);
	void RotateZ(float radians);

	void SetScale(float scale);
	void SetScale(float x, float y, float z);
	void SetScale(XMFLOAT3 scale);
	void IncreaseScaleByPercent(float scale);

	void UpdateWorldMatrix();

	XMMATRIX* GetWorldMatrix();
	XMMATRIX* GetWorldMatrixTransposed();

	XMFLOAT3 GetPositionXMFLOAT3() const;
	float3 GetPositionFloat3() const;

private:
	XMMATRIX worldMat;
	XMMATRIX worldMatTransposed;

	XMFLOAT3 position;
	XMFLOAT3 scale;

	XMMATRIX rotXMat;
	XMMATRIX rotYMat;
	XMMATRIX rotZMat;
	XMMATRIX rotationMat;
};

#endif