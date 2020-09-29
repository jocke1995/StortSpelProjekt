#include "stdafx.h"
#include "CubeCollisionComponent.h"
#include "../Renderer/Transform.h"
#include <Bullet/BulletCollision/CollisionShapes/btBoxShape.h>

component::CubeCollisionComponent::CubeCollisionComponent(Entity* parent, Transform* trans, float mass, float halfWidth, float halfHeight, float halfDepth) : 
	CollisionComponent(parent, trans, mass)
{
	btTransform btTrans;
	btTrans.setIdentity();
	btTrans.setOrigin({ trans->GetPositionFloat3().x, trans->GetPositionFloat3().y, trans->GetPositionFloat3().z });
	m_pBox = new btBoxShape({ halfWidth,halfHeight, halfDepth });
	btVector3 inertia = { 0.0f,0.0f,0.0f };
	m_pBox->calculateLocalInertia(mass, inertia);
	m_pMotionState = new btDefaultMotionState(btTrans);
	btRigidBody::btRigidBodyConstructionInfo info(mass, m_pMotionState, m_pBox, inertia);

	info.m_restitution = 0.5;
	info.m_friction = 1.0;

	m_pBody = new btRigidBody(info);
}

component::CubeCollisionComponent::~CubeCollisionComponent()
{
	delete m_pBox;
	delete m_pMotionState;
}

void component::CubeCollisionComponent::CheckCollision(CollisionComponent* other)
{
}
