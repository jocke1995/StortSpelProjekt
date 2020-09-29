#include "stdafx.h"
#include "CubeCollisionComponent.h"
#include "../Renderer/Transform.h"
#include <Bullet/BulletCollision/CollisionShapes/btBoxShape.h>

component::CubeCollisionComponent::CubeCollisionComponent(Entity* parent, float mass, float halfWidth, float halfHeight, float halfDepth) : 
	CollisionComponent(parent, mass)
{
	m_pShape = new btBoxShape({ halfWidth,halfHeight, halfDepth });
}

component::CubeCollisionComponent::~CubeCollisionComponent()
{
}

void component::CubeCollisionComponent::CheckCollision(CollisionComponent* other)
{
}
