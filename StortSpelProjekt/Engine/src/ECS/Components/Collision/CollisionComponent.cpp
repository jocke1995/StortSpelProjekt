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

void component::CollisionComponent::SetRotation(double roll, double pitch, double yaw)
{
	btTransform trans;
	m_pBody->getMotionState()->getWorldTransform(trans);
	btQuaternion quat;
	quat.setEulerZYX(roll, pitch, yaw);
	trans.setRotation(quat);
	m_pBody->getMotionState()->setWorldTransform(trans);

	m_pBody->getMotionState()->getWorldTransform(trans);
	yaw = 0;
	trans.getRotation().getEulerZYX(roll, pitch, yaw);
}

btRigidBody* component::CollisionComponent::GetBody()
{
	return m_pBody;
}

void component::CollisionComponent::Update(double dt)
{
	btTransform trans;
	m_pBody->getMotionState()->getWorldTransform(trans);
	float x = trans.getOrigin().x();
	float y = trans.getOrigin().y();
	float z = trans.getOrigin().z();
	m_pTrans->SetPosition(x, y, z);

	double roll;
	double pitch;
	double yaw;

	trans.getRotation().getEulerZYX(roll, pitch, yaw);
	m_pTrans->SetRotationX(yaw);
	m_pTrans->SetRotationY(pitch);
	m_pTrans->SetRotationZ(roll);
}
