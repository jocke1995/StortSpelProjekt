#include "stdafx.h"
#include "Physics.h"


Physics::Physics()
{
	m_Obb1 =  DirectX::BoundingOrientedBox(DirectX::XMFLOAT3(0.f, 0.f, 0.f), DirectX::XMFLOAT3(1.f, 1.f, 1.f), DirectX::XMFLOAT4(0.f, 0.f, 0.f, 1.f));
	m_Obb2 =  DirectX::BoundingOrientedBox(DirectX::XMFLOAT3(0.f, 2.f, 0.f), DirectX::XMFLOAT3(1.f, 1.f, 1.f), DirectX::XMFLOAT4(0.f, 0.f, 0.f, 1.f));
}

Physics::~Physics()
{
}

int Physics::CreateOBBFloat(float middleX, float middleY, float middleZ, float extentX, float extentY, float extentZ, float quatA, float quatB, float quatC, float quatD)
{
	m_OBBVec.push_back(DirectX::BoundingOrientedBox(DirectX::XMFLOAT3(middleX, middleY, middleZ), DirectX::XMFLOAT3(extentX, extentY, extentZ), DirectX::XMFLOAT4(quatA, quatB, quatC, quatD)));
	return m_OBBVec.size() - 1;
}

int Physics::CreateOBB(DirectX::XMFLOAT3 middleOfOBB, DirectX::XMFLOAT3 xyzRadius, DirectX::XMFLOAT4 quaternion)
{

	m_OBBVec.push_back(DirectX::BoundingOrientedBox(middleOfOBB, xyzRadius, quaternion));
	return m_OBBVec.size() - 1;
}

void Physics::m_TestStuff()
{
	m_Obb1.Center = DirectX::XMFLOAT3(0, 0, 0);
	m_Obb1.Extents = DirectX::XMFLOAT3(0.5, 0.5, 0.5);
	m_Obb1.Orientation = DirectX::XMFLOAT4(0, 0, 0, 1);

	if (m_Obb1.Intersects(m_Obb2))
	{
		Log::Print("Collision!\n");
	}
	else
	{
		Log::Print("No Collision!\n");
	}


}