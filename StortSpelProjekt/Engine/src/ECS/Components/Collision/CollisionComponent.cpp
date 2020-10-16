#include "stdafx.h"
#include "CollisionComponent.h"
#include "../ECS/Entity.h"
#include "../Renderer/Transform.h"
#include "../Physics/Physics.h"
component::CollisionComponent::CollisionComponent(Entity* parent, double mass, double friction, double restitution, bool canFall) : Component(parent),
m_pTrans(nullptr),
m_pBody(nullptr),
m_pMotionState(nullptr),
m_pShape(nullptr),
m_Mass(mass),
m_Fric(friction),
m_Rest(restitution),
m_CanFall(canFall)
{
}

component::CollisionComponent::~CollisionComponent()
{
	delete m_pBody;
	delete m_pMotionState;
}

void component::CollisionComponent::Update(double dt)
{
	
	btTransform trans = m_pBody->getWorldTransform();
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

void component::CollisionComponent::OnInitScene()
{
	if (m_pBody != nullptr)
	{
		delete m_pBody;
		m_pBody = nullptr;
	}

	if (m_pMotionState != nullptr)
	{
		delete m_pMotionState;
		m_pMotionState = nullptr;
	}

	// If no transform is given, this component is useless!
	if (!m_pParent->HasComponent<component::TransformComponent>())
	{
		Log::PrintSeverity(Log::Severity::CRITICAL, "No Transform provided for collisioncomponent in entity %s\n", m_pParent->GetName().c_str());
		return;
	}

	// Initiate the body with a given shape. This shape should be defined in an inherited class.
	m_pTrans = m_pParent->GetComponent<component::TransformComponent>()->GetTransform();

	// Set scaling for the shape
	m_pShape->setLocalScaling({ m_pTrans->GetScale().x, m_pTrans->GetScale().y, m_pTrans->GetScale().z });

	btTransform btTrans;
	btTrans.setIdentity();
	btTrans.setOrigin({ m_pTrans->GetPositionFloat3().x, m_pTrans->GetPositionFloat3().y, m_pTrans->GetPositionFloat3().z });

	// Set rotation
	float4 dxQuat = m_pTrans->GetRotation();
	btQuaternion quat(dxQuat.x, dxQuat.y, dxQuat.z, dxQuat.w);
	btTrans.setRotation(quat);

	btVector3 inertia = { 0.0f,0.0f,0.0f };
	m_pShape->calculateLocalInertia(m_Mass, inertia);
	m_pMotionState = new btDefaultMotionState(btTrans);

	btRigidBody::btRigidBodyConstructionInfo info(m_Mass, m_pMotionState, m_pShape, inertia);

	info.m_restitution = m_Rest;
	info.m_friction = m_Fric;

	m_pBody = new btRigidBody(info);
	m_pBody->setLinearVelocity({ m_pTrans->GetMovement().x, m_pTrans->GetMovement().y, m_pTrans->GetMovement().z });

	// Will be removed in the future!
	// If it isn't called, objects may be deactivated as Bullet deduces they wont be colliding with anything, but if user then changes position bullet does not react.
	m_pBody->setActivationState(DISABLE_DEACTIVATION);

	// Add the collisioncomponent to the physics sub-engine.
	Physics::GetInstance().AddCollisionComponent(this);
	if (!m_CanFall)
	{
		m_pBody->setAngularFactor({ 0.0, 1.0, 0.0 });
	}
}

void component::CollisionComponent::OnUnInitScene()
{
	Physics::GetInstance().RemoveCollisionComponent(this);
}


void component::CollisionComponent::SetPosition(double x, double y, double z)
{
	btTransform trans;
	trans = m_pBody->getWorldTransform();
	trans.setOrigin({ x,y,z });
	m_pBody->setWorldTransform(trans);
}

void component::CollisionComponent::SetRotation(double pitch, double yaw, double roll)
{
	btTransform trans = m_pBody->getWorldTransform();
	btQuaternion quat = trans.getRotation();

	quat.setEulerZYX(roll, yaw, pitch);
	trans.setRotation(quat);
	m_pBody->setWorldTransform(trans);
}

void component::CollisionComponent::SetRotation(double3 axis, double angle)
{
	btTransform trans = m_pBody->getWorldTransform();

	btQuaternion rotQuat;
	rotQuat.setRotation({ axis.x, axis.y, axis.z }, angle);

	trans.setRotation(rotQuat);
	m_pBody->setWorldTransform(trans);
}

void component::CollisionComponent::Rotate(double3 axis, double angle)
{
	btTransform trans = m_pBody->getWorldTransform();
	btQuaternion quat = trans.getRotation();

	btQuaternion rotQuat;
	rotQuat.setRotation({ axis.x, axis.y, axis.z }, angle);

	quat = quat * rotQuat;
	trans.setRotation(quat);
	m_pBody->setWorldTransform(trans);

	double roll;
	double pitch;
	double yaw;

	quat.getEulerZYX(roll, pitch, yaw);
	m_pTrans->SetRotationX(yaw);
	m_pTrans->SetRotationY(pitch);
	m_pTrans->SetRotationZ(roll);
}

void component::CollisionComponent::SetVelVector(double x, double y, double z)
{
	m_pBody->setLinearVelocity({ x, y, z });
}

void component::CollisionComponent::SetAngularVelocity(double x, double y, double z)
{
	m_pBody->setAngularVelocity({ x,y,z });
}

void component::CollisionComponent::SetFriction(double fric)
{
	m_pBody->setFriction(fric);
}

void component::CollisionComponent::SetRestitution(double rest)
{
	m_pBody->setRestitution(rest);
}

void component::CollisionComponent::SetAngularFactor(const double3& factor)
{
	m_pBody->setAngularFactor({ factor.x, factor.y, factor.z });
}

void component::CollisionComponent::SetLinearFactor(double3& factor)
{
	m_pBody->setLinearFactor({ factor.x, factor.y, factor.z });
}

btRigidBody* component::CollisionComponent::GetBody() const
{
	return m_pBody;
}

double3 component::CollisionComponent::GetPosition() const
{
	btVector3& pos = m_pBody->getWorldTransform().getOrigin();
	return { pos.x(), pos.y(), pos.z()};
}

double3 component::CollisionComponent::GetRotationEuler() const
{
	btQuaternion& rot = m_pBody->getWorldTransform().getRotation();

	double roll, pitch, yaw;

	rot.getEulerZYX(roll, pitch, yaw);

	return { pitch, yaw, roll };
}

double4 component::CollisionComponent::GetRotationQuaternion() const
{
	btQuaternion& rot = m_pBody->getWorldTransform().getRotation();
	return {rot.x(), rot.y(), rot.z(), rot.getAngle()};
}

double3 component::CollisionComponent::GetLinearVelocity() const
{
	btVector3 vel = m_pBody->getLinearVelocity();
	return { vel.x(), vel.y(), vel.z() };
}

double3 component::CollisionComponent::GetAngularVelocity() const
{
	btVector3 vel = m_pBody->getAngularVelocity();
	return { vel.x(), vel.y(), vel.z() };
}

double component::CollisionComponent::GetFriction() const
{
	return m_pBody->getFriction();
}

double component::CollisionComponent::GetRestitution() const
{
	return m_pBody->getRestitution();
}

double3 component::CollisionComponent::GetAngularFactor() const
{
	btVector3 factor = m_pBody->getAngularFactor();
	return {factor.x(), factor.y(), factor.z()};
}

double3 component::CollisionComponent::GetLinearFactor() const
{
	btVector3 factor = m_pBody->getLinearFactor();
	return { factor.x(), factor.y(), factor.z() };
}

double component::CollisionComponent::CastRay(double3 castTo) const
{
	// The ray does not collide with the object itself (tested on cube).
	// Probably the ray only collides with frontface of any triangle of objects.
	btVector3 btFrom = m_pBody->getWorldTransform().getOrigin();
	btVector3 btTo(castTo.x, castTo.y, castTo.z);
	btCollisionWorld::ClosestRayResultCallback res(btFrom, btTo);

	Physics::GetInstance().GetWorld()->rayTest(btFrom, btTo, res);

	if (res.hasHit()) {
		return (res.m_hitPointWorld - btFrom).length();
	}
	return -1;
}

double component::CollisionComponent::CastRay(double3 direction, double length) const
{
	// The ray does not collide with the object itself (tested on cube).
	// Probably the ray only collides with frontface of any triangle of objects.

	btVector3 btFrom = m_pBody->getWorldTransform().getOrigin();
	btVector3 btTo(direction.x, direction.y, direction.z);

	btTo = btFrom + btTo.normalize() * length;

	btCollisionWorld::ClosestRayResultCallback res(btFrom, btTo);

	Physics::GetInstance().GetWorld()->rayTest(btFrom, btTo, res);

	if (res.hasHit()) {
		return (res.m_hitPointWorld - btFrom).length();
	}
	return -1;
}