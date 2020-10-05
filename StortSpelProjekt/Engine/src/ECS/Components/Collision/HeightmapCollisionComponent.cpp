#include "stdafx.h"
#include "HeightmapCollisionComponent.h"
#include <Bullet/BulletCollision/CollisionShapes/btHeightfieldTerrainShape.h>

component::HeightmapCollisionComponent::HeightmapCollisionComponent(Entity* parent, HeightMapInfo info, double mass, double friction, double restitution): CollisionComponent(parent, mass,friction, restitution)
{
	m_pShape = new btHeightfieldTerrainShape(info.width, info.length, info.data, info.maxHeight, 1, info.datatype,false);
}

component::HeightmapCollisionComponent::~HeightmapCollisionComponent()
{
	delete m_pShape;
}

double component::HeightmapCollisionComponent::GetDistanceToBottom()
{
	return 0.0;
}
