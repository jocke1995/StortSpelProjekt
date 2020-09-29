#include "stdafx.h"
#include "CollisionComponent.h"
#include "../ECS/Entity.h"
#include "../Renderer/Transform.h"
component::CollisionComponent::CollisionComponent(Entity* parent , Transform* trans, float mass) : Component(parent), 
m_pTrans(trans), 
m_Mass(mass)
{
}

component::CollisionComponent::~CollisionComponent()
{
	delete m_pBody;
}

void component::CollisionComponent::SetMovement(float x, float y, float z)
{
	m_pBody->setLinearVelocity({ x,y,z });
}

btRigidBody* component::CollisionComponent::GetBody()
{
	return m_pBody;
}

void component::CollisionComponent::Update(double dt)
{
	btTransform trans;
	m_pBody->getMotionState()->getWorldTransform(trans);
	double mat[16];
	float x = trans.getOrigin().x();
	float y = trans.getOrigin().y();
	float z = trans.getOrigin().z();
	m_pTrans->SetPosition(x, y, z);

	double angles[3];
	trans.getRotation().getEulerZYX(angles[2], angles[1], angles[0]);
	m_pTrans->SetRotationX(angles[1]);
	m_pTrans->SetRotationY(angles[1]);
	m_pTrans->SetRotationZ(angles[2]);
}
