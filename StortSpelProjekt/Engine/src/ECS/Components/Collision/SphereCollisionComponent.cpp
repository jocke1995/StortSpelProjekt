#include "stdafx.h"
#include "SphereCollisionComponent.h"
#include "../Renderer/Transform.h"

component::SphereCollisionComponent::SphereCollisionComponent(Entity* parent, Transform* trans, float mass, float rad):
	CollisionComponent(parent, trans, mass),
	m_Rad(rad)
{
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
	DirectX::XMFLOAT3 lineOfAction = {
		(m_pTrans->GetPositionXMFLOAT3().x) - (other->m_pTrans->GetPositionXMFLOAT3().x),
		(m_pTrans->GetPositionXMFLOAT3().y) - (other->m_pTrans->GetPositionXMFLOAT3().y),
		(m_pTrans->GetPositionXMFLOAT3().z) - (other->m_pTrans->GetPositionXMFLOAT3().z)
	};

	float distSquared = lineOfAction.x * lineOfAction.x + lineOfAction.y * lineOfAction.y + lineOfAction.z * lineOfAction.z;

	if (distSquared <= (m_Rad + other->m_Rad) * (m_Rad + other->m_Rad))
	{
		// Collision has occured. Resolve it here.


		DirectX::XMFLOAT3 normLOA;
		DirectX::XMStoreFloat3(&normLOA, DirectX::XMVector3Normalize(DirectX::XMLoadFloat3(&lineOfAction)));

		DirectX::XMFLOAT3 velFirst = m_pTrans->GetMovement();
		DirectX::XMFLOAT3 velSecond = other->m_pTrans->GetMovement();

		// If the spheres are moving away from eachother, don't resolve another collision...
		if (velFirst.x * normLOA.x + velFirst.y * normLOA.y + velFirst.z * normLOA.z < 0.0f &&
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

		m_pTrans->SetActualMovement(postVelFirst.x, postVelFirst.y, postVelFirst.z);
		other->m_pTrans->SetActualMovement(postVelSec.x, postVelSec.y, postVelSec.z);
	}
}
