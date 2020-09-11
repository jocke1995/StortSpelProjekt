#include "stdafx.h"
#include "Physics.h"

Physics::Physics()
{
	
}

Physics::~Physics()
{
}

bool Physics::CheckOBBCollision(DirectX::BoundingOrientedBox obb1, DirectX::BoundingOrientedBox obb2)
{
	return obb1.Intersects(obb2);
}
