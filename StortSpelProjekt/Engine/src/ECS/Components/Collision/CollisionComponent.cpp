#include "stdafx.h"
#include "CollisionComponent.h"
#include "../ECS/Entity.h"
#include "../Renderer/Transform.h"
#include "../Physics/Physics.h"
component::CollisionComponent::CollisionComponent(Entity* parent, double mass, double friction, double restitution) : Component(parent),
m_pTrans(nullptr),
m_pBody(nullptr),
m_pMotionState(nullptr),
m_pShape(nullptr),
m_Mass(mass),
m_Fric(friction),
m_Rest(restitution)
{
}

component::CollisionComponent::~CollisionComponent()
{
	delete m_pBody;
	delete m_pMotionState;
}

btRigidBody* component::CollisionComponent::GetBody()
{
	return m_pBody;
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

	info.m_restitution = m_Rest;
	info.m_friction = m_Fric;

	m_pBody = new btRigidBody(info);
	m_pBody->setLinearVelocity({ m_pTrans->GetMovement().x, m_pTrans->GetMovement().y, m_pTrans->GetMovement().z });
	m_pBody->setActivationState(DISABLE_DEACTIVATION);
	Physics::GetInstance().AddCollisionComponent(this);
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

void component::CollisionComponent::SetAngularFactor(double3& factor)
{
	m_pBody->setAngularFactor({ factor.x, factor.y, factor.z });
}

void component::CollisionComponent::SetLinearFactor(double3& factor)
{
	m_pBody->setLinearFactor({ factor.x, factor.y, factor.z });
}

double3 component::CollisionComponent::GetPosition()
{
	btVector3& pos = m_pBody->getWorldTransform().getOrigin();
	return { pos.x(), pos.y(), pos.z()};
}

double3 component::CollisionComponent::GetRotationEuler()
{
	btQuaternion& rot = m_pBody->getWorldTransform().getRotation();

	double roll, pitch, yaw;

	rot.getEulerZYX(roll, pitch, yaw);

	return { pitch, yaw, roll };
}

double4 component::CollisionComponent::GetRotationQuaternion()
{
	btQuaternion& rot = m_pBody->getWorldTransform().getRotation();
	return {rot.x(), rot.y(), rot.z(), rot.getAngle()};
}

double3 component::CollisionComponent::GetLinearVelocity()
{
	btVector3 vel = m_pBody->getLinearVelocity();
	return { vel.x(), vel.y(), vel.z() };
}

double3 component::CollisionComponent::GetAngularVelocity()
{
	btVector3 vel = m_pBody->getAngularVelocity();
	return { vel.x(), vel.y(), vel.z() };
}

double component::CollisionComponent::GetFriction()
{
	return m_pBody->getFriction();
}

double component::CollisionComponent::GetRestitution()
{
	return m_pBody->getRestitution();
}

double3 component::CollisionComponent::GetAngularFactor()
{
	btVector3 factor = m_pBody->getAngularFactor();
	return {factor.x(), factor.y(), factor.z()};
}

double3 component::CollisionComponent::GetLinearFactor()
{
	btVector3 factor = m_pBody->getLinearFactor();
	return { factor.x(), factor.y(), factor.z() };
}

double component::CollisionComponent::CastRay(double3 castTo)
{
	btVector3 btFrom = m_pBody->getWorldTransform().getOrigin();
	btVector3 btTo(castTo.x, castTo.y, castTo.z);
	btCollisionWorld::ClosestRayResultCallback res(btFrom, btTo);

	Physics::GetInstance().GetWorld()->rayTest(btFrom, btTo, res); // m_btWorld is btDiscreteDynamicsWorld

	if (res.hasHit()) {
		return (res.m_hitPointWorld - btFrom).length();
	}
	return -1;
}

double component::CollisionComponent::CastRay(double3 direction, double length)
{
	btVector3 btFrom = m_pBody->getWorldTransform().getOrigin();
	btVector3 btTo(direction.x, direction.y, direction.z);

	btTo = btTo.normalize() * length;

	btCollisionWorld::ClosestRayResultCallback res(btFrom, btTo);

	Physics::GetInstance().GetWorld()->rayTest(btFrom, btTo, res); // m_btWorld is btDiscreteDynamicsWorld

	if (res.hasHit()) {
		return (res.m_hitPointWorld - btFrom).length();
	}
	return -1;
}

