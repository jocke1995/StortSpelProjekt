#include "stdafx.h"
#include "Physics.h"

Physics::Physics()
{
	
}

Physics::~Physics()
{
}

bool Physics::CheckOBBCollision(const DirectX::BoundingOrientedBox* obb1, const DirectX::BoundingOrientedBox* obb2) const
{
	return obb1->Intersects(*obb2);
}
