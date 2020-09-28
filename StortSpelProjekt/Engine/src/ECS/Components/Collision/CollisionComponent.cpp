#include "stdafx.h"
#include "CollisionComponent.h"
#include "../ECS/Entity.h"

component::CollisionComponent::CollisionComponent(Entity* parent , Transform* trans, float mass) : Component(parent), 
m_pTrans(trans), 
m_Mass(mass)
{
}

component::CollisionComponent::~CollisionComponent()
{
	delete m_pBody;
}

btRigidBody* component::CollisionComponent::GetBody()
{
	return m_pBody;
}
