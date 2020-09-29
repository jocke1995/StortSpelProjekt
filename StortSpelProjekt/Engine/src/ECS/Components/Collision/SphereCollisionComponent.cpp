#include "stdafx.h"
#include "SphereCollisionComponent.h"
#include "../Renderer/Transform.h"
component::SphereCollisionComponent::SphereCollisionComponent(Entity* parent, Transform* trans, float mass, float rad) :
	CollisionComponent(parent, trans, mass),
	m_Rad(rad)
{
	btTransform btTrans;
	btTrans.setIdentity();
	btTrans.setOrigin({ trans->GetPositionFloat3().x, trans->GetPositionFloat3().y, trans->GetPositionFloat3().z });
	m_pSphere = new btSphereShape(rad);
	btVector3 inertia = { 0.0f,0.0f,0.0f };
	m_pSphere->calculateLocalInertia(mass, inertia);
	m_pMotionState = new btDefaultMotionState(btTrans);
	btRigidBody::btRigidBodyConstructionInfo info(mass, m_pMotionState, m_pSphere);
	m_pBody = new btRigidBody(info);
}

component::SphereCollisionComponent::~SphereCollisionComponent()
{
	delete m_pSphere;
	delete m_pMotionState;
}

void component::SphereCollisionComponent::CheckCollision(CollisionComponent* other)
{
	SphereCollisionComponent* scc = dynamic_cast<SphereCollisionComponent*>(other);

	if (scc)
	{
		CheckCollisionSphere(scc);
	}
}

void component::SphereCollisionComponent::CheckCollisionSphere(SphereCollisionComponent* other)
{
	/*
	DirectX::XMFLOAT3 distVec = {
		(m_pTrans->GetPositionXMFLOAT3().x) - (other->m_pTrans->GetPositionXMFLOAT3().x),
		(m_pTrans->GetPositionXMFLOAT3().y) - (other->m_pTrans->GetPositionXMFLOAT3().y),
		(m_pTrans->GetPositionXMFLOAT3().z) - (other->m_pTrans->GetPositionXMFLOAT3().z)
	};

	float distSquared = distVec.x * distVec.x + distVec.y * distVec.y + distVec.z * distVec.z;
	float minColDistSquared = (m_Rad + other->m_Rad) * (m_Rad + other->m_Rad);
	if (distSquared <= minColDistSquared)
	{
		// Collision has occured. Resolve it here.


		DirectX::XMFLOAT3 normLOA;
		DirectX::XMStoreFloat3(&normLOA, DirectX::XMVector3Normalize(DirectX::XMLoadFloat3(&distVec)));

		DirectX::XMFLOAT3 velFirst = m_pTrans->GetMovement();
		DirectX::XMFLOAT3 velSecond = other->m_pTrans->GetMovement();

		// If the spheres are moving away from eachother, don't resolve another collision...
		if (-velFirst.x * normLOA.x - velFirst.y * normLOA.y - velFirst.z * normLOA.z < 0.0f &&
			velSecond.x * normLOA.x + velSecond.y * normLOA.y + velSecond.z * normLOA.z < 0.0f)
		{
			return;
		}

		// Calculate the velocity along the line of action.
		float v1p = velFirst.x * normLOA.x + velFirst.y * normLOA.y + velFirst.z * normLOA.z;
		float v2p = velSecond.x * normLOA.x + velSecond.y * normLOA.y + velSecond.z * normLOA.z;

		// Calculate the post collision velocities along the line of action
		float u1p = v1p * (m_Mass - COR * other->m_Mass) / (m_Mass + other->m_Mass) + v2p * (1.0f + COR) * other->m_Mass / (m_Mass + other->m_Mass);
		float u2p = v2p * (other->m_Mass - COR * m_Mass) / (other->m_Mass + m_Mass) + v1p * (1.0f + COR) * m_Mass / (other->m_Mass + m_Mass);

		// Apply post collision velocities and assign to transform.
		DirectX::XMFLOAT3 postVelFirst = 
		{ 
			velFirst.x + (u1p - v1p) * (normLOA.x),
			velFirst.y + (u1p - v1p) * (normLOA.y), 
			velFirst.z + (u1p - v1p) * (normLOA.z)
		};

		DirectX::XMFLOAT3 postVelSec =
		{ 
			velSecond.x + (u2p - v2p) * (normLOA.x),
			velSecond.y + (u2p - v2p) * (normLOA.y),
			velSecond.z + (u2p - v2p) * (normLOA.z)
		};

		// Calculate how far the spheres need to move to not be colliding with eachother

		// Calculate length of each velocity vector
		float firstVel = sqrtf(postVelFirst.x * postVelFirst.x + postVelFirst.y * postVelFirst.y + postVelFirst.z * postVelFirst.z);
		float secVel = sqrtf(postVelFirst.x * postVelFirst.x + postVelFirst.y * postVelFirst.y + postVelFirst.z * postVelFirst.z);
		float actualDiff = sqrtf(minColDistSquared) - sqrtf(distSquared);

		float velTot = firstVel + secVel;
		float firstToMove = firstVel * actualDiff / velTot;
		float secToMove = secVel * actualDiff / velTot;

		m_pTrans->SetPosition
		(
			m_pTrans->GetPositionXMFLOAT3().x + postVelFirst.x * firstToMove, 
			m_pTrans->GetPositionXMFLOAT3().y + postVelFirst.y * firstToMove, 
			m_pTrans->GetPositionXMFLOAT3().z + postVelFirst.z * firstToMove
		);

		other->m_pTrans->SetPosition
		(
			other->m_pTrans->GetPositionXMFLOAT3().x + postVelSec.x * secToMove,
			other->m_pTrans->GetPositionXMFLOAT3().y + postVelSec.y * secToMove,
			other->m_pTrans->GetPositionXMFLOAT3().z + postVelSec.z * secToMove
		);


		m_pTrans->SetActualMovement(postVelFirst.x, postVelFirst.y, postVelFirst.z);
		other->m_pTrans->SetActualMovement(postVelSec.x, postVelSec.y, postVelSec.z);
	}*/
}
