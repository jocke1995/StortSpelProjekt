#pragma once

#include "EngineMath.h"
#include<DirectXCollision.h>

class Physics
{
public:
	Physics();
	~Physics();

	// Create an Oriented bounding box with floats as input. Returns the placenemt of this newly created OBB in the OBB vector.
	// The first 3 are the location of the center of said box in the world. 
	// The next 3 are the size of xzy from center of box to edge. 
	// Last 4 are the quaternions used for rotation
	int CreateOBBFloat(float middleX, float middleY, float middleZ, float extentX, float extentY, float extentZ, float quatA, float quatB, float quatC, float quatD);

	// Create an Oriented bounding box with XMFLOATS as input. Returns the placenemt of this newly created OBB in the OBB vector.
	int CreateOBB(DirectX::XMFLOAT3 middleOfOBB, DirectX::XMFLOAT3 xyzRadius, DirectX::XMFLOAT4 quaternion);

private:
	DirectX::XMFLOAT3 m_Center = DirectX::XMFLOAT3((0.1f), (0.0f) ,(0.0f)); // ska bort bara här för jag ska ha exempel på hur man skriver
	DirectX::BoundingOrientedBox m_Obb1;	// ska bort bara här för test
	DirectX::BoundingOrientedBox m_Obb2;	// ska bort bara här för test
	std::vector<DirectX::BoundingOrientedBox> m_OBBVec;

	void m_TestStuff();
	//DirectX::BoundingOrientedBox Intersects(const DirectX::BoundingOrientedBox& obb) const;
	//void m_TestStuff();
	//Intersects(const BoundingOrientedBox& sh) const;
};



