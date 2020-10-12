#include "stdafx.h"
#include "HeightmapCollisionComponent.h"
#include <Bullet/BulletCollision/CollisionShapes/btHeightfieldTerrainShape.h>

component::HeightmapCollisionComponent::HeightmapCollisionComponent(Entity* parent, HeightMapInfo info, double mass, double friction, double restitution): CollisionComponent(parent, mass,friction, restitution)
{
	m_pShape = new btHeightfieldTerrainShape(info.width, info.length, info.data, 1.0f, info.minHeight, info.maxHeight, 1, PHY_FLOAT, false);
	btVector3 scaling = { 1.0,10.0,1.0 };
	((btHeightfieldTerrainShape*)m_pShape)->setLocalScaling(scaling);
}

component::HeightmapCollisionComponent::~HeightmapCollisionComponent()
{
	delete m_pShape;
}

double component::HeightmapCollisionComponent::GetDistanceToBottom() const
{
	return 0.0;
}
