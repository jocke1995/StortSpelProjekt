#include "AiComponent.h"
#include "../ECS/Entity.h"
#include "../Renderer/Transform.h"
#include "../Misc/EngineRand.h"
#include "Components/HealthComponent.h"

component::AiComponent::AiComponent(Entity* parent, Entity* target, float detectionRadius, float attackingDistance) : Component(parent)
{
	m_pTarget = target;
	m_DetectionRadius = detectionRadius;
	m_AttackingDistance = attackingDistance;
}

component::AiComponent::~AiComponent()
{
}

void component::AiComponent::Update(double dt)
{
	Transform* targetTrans = m_pTarget->GetComponent<component::TransformComponent>()->GetTransform();
	Transform* parentTrans = m_pParent->GetComponent<component::TransformComponent>()->GetTransform();
	CollisionComponent* cc = m_pParent->GetComponent<component::CollisionComponent>();

	float3 targetPos = targetTrans->GetPositionFloat3();
	float3 pos = parentTrans->GetPositionFloat3();

	float3 direction = { targetPos.x - pos.x, targetPos.y - pos.y, targetPos.z - pos.z };

	float distance = sqrt(direction.x * direction.x + direction.y * direction.y + direction.z * direction.z);

	if (cc->CastRay({ 0.0, -1.0, 0.0 }, cc->GetDistanceToBottom() + 0.1) != -1)
	{
		double vel;
		if (distance <= m_DetectionRadius && distance >= (m_AttackingDistance - 0.5f))
		{
			parentTrans->SetVelocity(7.5);
			parentTrans->SetMovement(direction.x / distance, direction.y / distance, direction.z / distance);
			// for demoscene! 
			GetParent()->GetComponent<component::Audio3DEmitterComponent>()->Play(L"Bruh");
			vel = 7.5;
			parentTrans->SetVelocity(vel);
			cc->SetVelVector(vel * direction.x / distance, vel * 2 * direction.y / distance, vel * direction.z / distance);
		}
		else
		{
			vel = 2.5;
			parentTrans->SetVelocity(vel);
			float randX = -1.0f + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (1.0f - (-1.0f))));
			float randZ = -1.0f + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (1.0f - (-1.0f))));
			parentTrans->UpdateMovement(randX, 0.0f, randZ);
			parentTrans->SetMovement(min(max(parentTrans->GetMovement().x, -5.0f), 5.0f), 0.0f, min(max(parentTrans->GetMovement().z, -5.0f), 5.0f));

			// for demoscene! 
			GetParent()->GetComponent<component::Audio3DEmitterComponent>()->Stop(L"Bruh");
			cc->SetVelVector(min(max(cc->GetLinearVelocity().x + vel * randX, -5.0f * vel), 5.0f * vel), 0.0f, min(max(cc->GetLinearVelocity().z + vel * randZ, -5.0f * vel), 5.0f * vel));
		}
	}

	if (distance <= m_AttackingDistance)
	{
		Log::Print("%s attacking player!\n", m_pParent->GetName().c_str());
		// TODO: fix this when meele attack is implemented
		m_pTarget->GetComponent<component::HealthComponent>()->ChangeHealth(-1);
		// for demoscene!
		GetParent()->GetComponent<component::Audio2DVoiceComponent>()->Play(L"attack");
	}
}

void component::AiComponent::RenderUpdate(double dt)
{
}
