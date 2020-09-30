#include "stdafx.h"
#include "CapsuleCollisionComponent.h"

component::CapsuleCollisionComponent::CapsuleCollisionComponent(Entity* parent, double mass, double radius, double height, double friction, double restitution)
	:CollisionComponent(parent, mass, friction, restitution)
{
	m_pShape = new btCapsuleShape(radius, height);
}

component::CapsuleCollisionComponent::~CapsuleCollisionComponent()
{
	delete m_pShape;
}
