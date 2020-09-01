#ifndef TRANSFORM_H
#define TRANSFORM_H

#include "EngineMath.h"

class Transform
{
public:
	Transform();
	virtual ~Transform();

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
	DirectX::XMMATRIX m_WorldMat;
	DirectX::XMMATRIX m_WorldMatTransposed;

	DirectX::XMFLOAT3 m_Position;
	DirectX::XMFLOAT3 m_Scale;

	DirectX::XMMATRIX m_RotXMat;
	DirectX::XMMATRIX m_RotYMat;
	DirectX::XMMATRIX m_RotZMat;
	DirectX::XMMATRIX m_RotationMat;
};

#endif