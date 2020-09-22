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
	// Sets the movement direction. This will later be normalized to the velocity of the transform.
	void SetMovement(float x, float y, float z);
	// Sets the movement direction. This will later be normalized to the velocity of the transform.
	void SetMovement(DirectX::XMFLOAT3 mov);
	void UpdateMovement(float x, float y, float z);
	void Move(float dt);
	void MoveRender(float dt);

	void NormalizedMove(float dt);
	void NormalizedMoveRender(float dt);
	
	void SetRotationX(float radians);
	void SetRotationY(float radians);
	void SetRotationZ(float radians);

	void SetScale(float scale);
	void SetScale(float x, float y, float z);
	void IncreaseScaleByPercent(float scale);

	void UpdateWorldMatrix();

	DirectX::XMMATRIX* GetWorldMatrix();
	DirectX::XMMATRIX* GetWorldMatrixTransposed();

	DirectX::XMFLOAT3 GetPositionXMFLOAT3() const;
	float3 GetPositionFloat3() const;
	DirectX::XMFLOAT3 GetScale() const;
	// gets the rotation of the transform in all axisis
	DirectX::XMMATRIX GetRotMatrix() const;

	DirectX::XMFLOAT3 GetMovement() const;

	float GetVelocity() const;
	void SetVelocity(float vel);

	// Sets the movement. Also sets the velocity to the length of the given vector.
	void SetActualMovement(float x, float y, float z);
	// Sets the movement. Also sets the velocity to the length of the given vector.
	void SetActualMovement(DirectX::XMFLOAT3 mov);

	void UpdateActualMovement(float x, float y, float z);

private:
	DirectX::XMMATRIX m_WorldMat;
	DirectX::XMMATRIX m_WorldMatTransposed;

	DirectX::XMFLOAT3 m_Position;
	DirectX::XMFLOAT3 m_RenderPosition;
	DirectX::XMFLOAT3 m_Movement;
	DirectX::XMFLOAT3 m_Scale;

	DirectX::XMMATRIX m_RotXMat;
	DirectX::XMMATRIX m_RotYMat;
	DirectX::XMMATRIX m_RotZMat;
	DirectX::XMMATRIX m_RotationMat;

	float m_Velocity;
};

#endif