#include "stdafx.h"
#include "SphereCollisionComponent.h"
#include "../Renderer/Transform.h"
component::SphereCollisionComponent::SphereCollisionComponent(Entity* parent, double mass, double friction, double restitution, float rad) :
	CollisionComponent(parent, mass, friction, restitution),
	m_Rad(rad)
{
	m_pShape = new btSphereShape(rad);
}

component::SphereCollisionComponent::~SphereCollisionComponent()
{
}