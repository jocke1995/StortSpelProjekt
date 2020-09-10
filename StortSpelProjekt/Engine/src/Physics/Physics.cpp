#include "stdafx.h"
#include "Physics.h"


Physics::Physics()
{
	
}

Physics::~Physics()
{
}

bool Physics::checkOBBCollision(DirectX::BoundingOrientedBox oBB1, DirectX::BoundingOrientedBox oBB2)
{
	return oBB1.Intersects(oBB2);
}
