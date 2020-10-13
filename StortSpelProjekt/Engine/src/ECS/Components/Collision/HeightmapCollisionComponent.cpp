#include "stdafx.h"
#include "HeightmapCollisionComponent.h"
#include <Bullet/BulletCollision/CollisionShapes/btHeightfieldTerrainShape.h>

component::HeightmapCollisionComponent::HeightmapCollisionComponent(Entity* parent, HeightMapInfo info, double mass, double friction, double restitution): CollisionComponent(parent, mass,friction, restitution)
{
	m_pShape = new btHeightfieldTerrainShape(info.width, info.length, info.data, 0.1, info.minHeight, info.maxHeight, 1, PHY_UCHAR, false);
}

component::HeightmapCollisionComponent::~HeightmapCollisionComponent()
{
	delete m_pShape;
}

double component::HeightmapCollisionComponent::GetDistanceToBottom() const
{
	return 0.0;
}
