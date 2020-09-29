#include "stdafx.h"
#include "SphereCollisionComponent.h"
#include "../Renderer/Transform.h"
component::SphereCollisionComponent::SphereCollisionComponent(Entity* parent, float mass, float rad) :
	CollisionComponent(parent, mass),
	m_Rad(rad)
{
	m_pShape = new btSphereShape(rad);
}

component::SphereCollisionComponent::~SphereCollisionComponent()
{
}