#include "stdafx.h"
#include "CollisionComponent.h"
#include "../ECS/Entity.h"
#include "../Renderer/Transform.h"
#include "../Physics/Physics.h"
component::CollisionComponent::CollisionComponent(Entity* parent, float mass) : Component(parent), 
m_pTrans(nullptr),
m_Mass(mass)
{
}

component::CollisionComponent::~CollisionComponent()
{
	delete m_pBody;
	delete m_pMotionState;
	delete m_pShape;
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

void component::CollisionComponent::InitScene()
{
#ifdef _DEBUG
	if (!m_pParent->HasComponent<component::TransformComponent>())
	{
		Log::PrintSeverity(Log::Severity::CRITICAL, "No Transform provided for collisioncomponent in entity %s\n", m_pParent->GetName().c_str());
		return;
	}
#endif

	m_pTrans = m_pParent->GetComponent<component::TransformComponent>()->GetTransform();

	btTransform btTrans;
	btTrans.setIdentity();
	btTrans.setOrigin({ m_pTrans->GetPositionFloat3().x, m_pTrans->GetPositionFloat3().y, m_pTrans->GetPositionFloat3().z });
	btVector3 inertia = { 0.0f,0.0f,0.0f };
	m_pShape->calculateLocalInertia(m_Mass, inertia);
	m_pMotionState = new btDefaultMotionState(btTrans);
	btRigidBody::btRigidBodyConstructionInfo info(m_Mass, m_pMotionState, m_pShape, inertia);

	info.m_restitution = 0.5;
	info.m_friction = 1.0;

	m_pBody = new btRigidBody(info);
	m_pBody->setLinearVelocity({ m_pTrans->GetMovement().x, m_pTrans->GetMovement().y, m_pTrans->GetMovement().z });
	Physics::GetInstance().AddCollisionComponent(this);
}

void component::CollisionComponent::SetPosition(double x, double y, double z)
{
	m_pBody->getWorldTransform().setOrigin({ x, y, z });
}

void component::CollisionComponent::SetRotation(double roll, double pitch, double yaw)
{
	btQuaternion quat = m_pBody->getWorldTransform().getRotation();

	quat.setEulerZYX(roll, pitch, yaw);

	m_pBody->getWorldTransform().setRotation(quat);
}

void component::CollisionComponent::SetVelVector(double x, double y, double z)
{
	m_pBody->setLinearVelocity({ x, y, z });
}

void component::CollisionComponent::SetNormalizedVelVector(double x, double y, double z)
{
	double length = sqrt(x * x + y * y + z * z);

	if (length > 1.0)
	{
		x = x / length;
		y = y / length;
		z = z / length;
	}

	m_pBody->setLinearVelocity({ x, y, z });
}
