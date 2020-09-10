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
	if (oBB1.Intersects(oBB2))
	{
		return true;
	}
	else
	{
		return false;
	}
}
