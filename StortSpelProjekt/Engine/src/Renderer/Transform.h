#ifndef TRANSFORM_H
#define TRANSFORM_H

#include "EngineMath.h"

#define BASE_VEL 30.0

class Transform
{
public:
	Transform(bool invertDirection = false);
	virtual ~Transform();

	void SetPosition(float x, float y, float z);
	void SetPosition(DirectX::XMFLOAT3 pos);
	// Sets the movement direction. This will later be normalized to the velocity of the transform.
	void SetMovement(float x, float y, float z);
	// Sets the movement direction. This will later be normalized to the velocity of the transform.
	void SetMovement(DirectX::XMFLOAT3 mov);
	void UpdateMovement(float x, float y, float z);
	// Moves the object in the direction of the current movement multiplied by the set speed.
	void Move(float dt);
	// Moves the object in the direction of the current movement multiplied by the set speed.
	void MoveRender(float dt);

	// Moves the object in the direction of the current movement, but at the set speed. (Moves the object a maximum distance of the current speed * dt)
	void NormalizedMove(float dt);
	// Moves the object in the direction of the current movement, but at the set speed. (Moves the object a maximum distance of the current speed * dt)
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
	DirectX::XMFLOAT3 GetRenderPositionXMFLOAT3() const;
	float3 GetRenderPositionFloat3() const;
	DirectX::XMFLOAT3 GetScale() const;
	// gets a quaternion representation of the rotation matrix
	float4 GetRotation();
	// gets the rotation of the transform in all axisis
	DirectX::XMMATRIX GetRotMatrix() const;

	DirectX::XMFLOAT3 GetMovement() const;

	/// <summary>
	/// Gets the forward vector of the transform as an XMFLOAT3
	/// </summary>
	/// <returns>XMFLOAT3 containing the forward vector</returns>
	DirectX::XMFLOAT3 GetForwardXMFLOAT3() const;
	/// <summary>
	/// Gets the forward vector of the transform as an float3
	/// </summary>
	/// <returns>float3 containing the forward vector</returns>
	float3 GetForwardFloat3() const;
	/// <summary>
	/// Gets the right vector of the transform as an XMFLOAT3
	/// </summary>
	/// <returns>XMFLOAT3 containing the right vector</returns>
	DirectX::XMFLOAT3 GetRightXMFLOAT3() const;
	/// <summary>
	/// Gets the right vector of the transform as an float3
	/// </summary>
	/// <returns>float3 containing the right vector</returns>
	float3 GetRightFloat3() const;
	/// <summary>
	/// Gets the up vector of the transform as an XMFLOAT3
	/// </summary>
	/// <returns>XMFLOAT3 containing the up vector</returns>
	DirectX::XMFLOAT3 GetUpXMFLOAT3() const;
	/// <summary>
	/// Gets the up vector of the transform as an float3
	/// </summary>
	/// <returns>float3 containing the up vector</returns>
	float3 GetUpFloat3() const;

	float GetVelocity() const;
	void SetVelocity(float vel);

	int GetInvDir();

	// Sets the movement. Also sets the velocity to the length of the given vector.
	void SetActualMovement(float x, float y, float z);
	// Sets the movement. Also sets the velocity to the length of the given vector.
	void SetActualMovement(DirectX::XMFLOAT3 mov);

	void UpdateActualMovement(float x, float y, float z);

private:
	DirectX::XMMATRIX m_WorldMat;
	DirectX::XMMATRIX m_WorldMatTransposed;

	DirectX::XMFLOAT3 m_Position;
	DirectX::XMFLOAT3 m_OldPosition;
	DirectX::XMFLOAT3 m_RenderPosition;
	DirectX::XMFLOAT3 m_Movement;
	DirectX::XMFLOAT3 m_Scale;

	DirectX::XMMATRIX m_RotXMat;
	DirectX::XMMATRIX m_RotYMat;
	DirectX::XMMATRIX m_RotZMat;
	DirectX::XMMATRIX m_RotationMat;

	float m_Velocity;

	double m_TimeBetweenFrame;
	double m_UpdateRate;

	int m_InvDir;
};

#endif