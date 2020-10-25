#include "AiComponent.h"
#include "../ECS/Entity.h"
#include "../ECS/Scene.h"
#include "../Renderer/Transform.h"
#include "../Misc/EngineRand.h"
#include "Components/HealthComponent.h"
#include "Misc/NavMesh.h"

component::AiComponent::AiComponent(Entity* parent, Entity* target, unsigned int flags, float detectionRadius, float attackingDistance) : Component(parent)
{
	m_pTarget = target;
	m_Targets.push_back(target);
	m_DetectionRadius = detectionRadius;
	m_AttackingDistance = attackingDistance;
	m_Flags = flags;
	m_AttackInterval = 0.5;
	m_TimeAccumulator = 0.0;
	m_pScene = nullptr;
	m_pNavMesh = nullptr;
	m_pQuads = nullptr;
	m_pCurrentQuad = nullptr;
	m_StartPos = m_pParent->GetComponent<component::TransformComponent>()->GetTransform()->GetPositionFloat3();
	m_GoalPos = m_pTarget->GetComponent<component::TransformComponent>()->GetTransform()->GetPositionFloat3();
	m_pStartQuad = nullptr;
	m_pGoalQuad = nullptr;
	m_NextTargetPos = m_StartPos;
	m_PathFound = false;
}

component::AiComponent::~AiComponent()
{
	for (unsigned int i = 0; i < m_pNavMesh->GetNumQuads(); ++i)
	{
		delete m_pQuads[i];
	}
	delete[] m_pQuads;
}

void component::AiComponent::SetScene(Scene* scene)
{
	m_pScene = scene;
	m_pNavMesh = scene->GetNavMesh();
	m_pQuads = new PathQuad*[m_pNavMesh->GetNumQuads()];
	for (unsigned int i = 0; i < m_pNavMesh->GetNumQuads(); ++i)
	{
		m_pQuads[i] = new PathQuad;
		m_pQuads[i]->id = i;
	}
}

void component::AiComponent::Update(double dt)
{
	if (m_pParent->GetComponent<component::HealthComponent>()->GetHealth() > 0)
	{
		selectTarget();
		Transform* targetTrans = m_pTarget->GetComponent<component::TransformComponent>()->GetTransform();
		Transform* parentTrans = m_pParent->GetComponent<component::TransformComponent>()->GetTransform();
		CollisionComponent* cc = m_pParent->GetComponent<component::CollisionComponent>();

		float3 finalTargetPos = targetTrans->GetPositionFloat3();
		float3 pos = parentTrans->GetPositionFloat3();
		NavQuad* targetQuad = m_pNavMesh->GetQuad(finalTargetPos);

		if (targetQuad != m_pGoalQuad)
		{
			findPathToTarget();
		}

		if (m_PathFound)
		{
			m_Path = m_NextPath;
			m_PathFound = false;
		}

		m_pNextQuad = m_pNavMesh->GetQuad(m_NextTargetPos);

		if (m_pNavMesh->GetQuad(pos) == m_pNextQuad && !m_Path.empty())
		{
			m_Path.pop_back();
		}

		if (!m_Path.empty())
		{
			m_NextTargetPos = m_Path.back();
		}
		else
		{
			m_NextTargetPos = finalTargetPos;
		}

		float3 direction = { m_NextTargetPos.x - pos.x, (m_NextTargetPos.y - pos.y) * static_cast<float>(m_Flags & F_AI_FLAGS::CAN_JUMP), m_NextTargetPos.z - pos.z };
		if (!(m_Flags & F_AI_FLAGS::CAN_ROLL))
		{
			double angle = std::atan2(direction.x, direction.z);
			cc->SetRotation({ 0.0, 1.0, 0.0 }, angle);
		}
		float distance = sqrt(direction.x * direction.x + direction.y * direction.y + direction.z * direction.z);

		if (cc->CastRay({ 0.0, -1.0, 0.0 }, cc->GetDistanceToBottom() + 0.5) != -1)
		{
			double vel;
			if (distance <= m_DetectionRadius && distance >= (m_AttackingDistance - 0.5f))
			{
				vel = parentTrans->GetVelocity() * 3.0;
				cc->SetVelVector(vel * direction.x / distance, vel * 2 * direction.y / distance, vel * direction.z / distance);
			}
			else
			{
				vel = parentTrans->GetVelocity();
				float randX = -1.0f + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (1.0f - (-1.0f))));
				float randZ = -1.0f + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (1.0f - (-1.0f))));
				double moveX = min(max(cc->GetLinearVelocity().x + vel * randX, -5.0f * vel), 5.0f * vel);
				double moveZ = min(max(cc->GetLinearVelocity().z + vel * randZ, -5.0f * vel), 5.0f * vel);

				if (!(m_Flags & F_AI_FLAGS::CAN_ROLL))
				{
					double angle = std::atan2(moveX, moveZ);
					cc->SetRotation({ 0.0, 1.0, 0.0 }, angle);
				}
				cc->SetVelVector(min(max(cc->GetLinearVelocity().x + vel * randX, -5.0f * vel), 5.0f * vel), 0.0f, min(max(cc->GetLinearVelocity().z + vel * randZ, -5.0f * vel), 5.0f * vel));
			}

			if (distance <= m_AttackingDistance)
			{
				// TODO: fix this when meele attack is implemented
				HealthComponent* hc = m_pTarget->GetComponent<component::HealthComponent>();
				if (hc != nullptr)
				{
					m_TimeAccumulator += static_cast<float>(dt);
					if (m_TimeAccumulator > m_AttackInterval)
					{
						m_pTarget->GetComponent<component::HealthComponent>()->TakeDamage(-10);
						Log::Print("ENEMY ATTACK!\n");
						m_TimeAccumulator = 0.0;
					}
				}
			}
			else
			{
				// "reset" accumulator if out of attack range
				m_TimeAccumulator = 0.0;
			}
		}
	}
	else
	{
		m_pParent->GetComponent<component::TransformComponent>()->GetTransform()->SetMovement(0, 0, 0);
	}

}

void component::AiComponent::RenderUpdate(double dt)
{
}

void component::AiComponent::OnInitScene()
{
}

void component::AiComponent::OnUnInitScene()
{
}

void component::AiComponent::AddTarget(Entity* target)
{
	m_Targets.push_back(target);
}

void component::AiComponent::RemoveTarget(std::string name)
{
	for (unsigned int i = 0; i < m_Targets.size(); i++)
	{
		if (m_Targets.at(i)->GetName() == name)
		{
			m_Targets.erase(m_Targets.begin() + i);
		}
	}
}

Entity* component::AiComponent::GetTarget()
{
	return m_pTarget;
}

void component::AiComponent::SetAttackInterval(float interval)
{
	m_AttackInterval = interval;
}

void component::AiComponent::selectTarget()
{
	float distance = (m_pParent->GetComponent<component::TransformComponent>()->GetTransform()->GetPositionFloat3() - m_Targets.at(0)->GetComponent<component::TransformComponent>()->GetTransform()->GetPositionFloat3()).length();
	int index = 0;
	for (unsigned int i = 1; i < m_Targets.size(); i++)
	{
		float newDistance = (m_pParent->GetComponent<component::TransformComponent>()->GetTransform()->GetPositionFloat3() - m_Targets.at(i)->GetComponent<component::TransformComponent>()->GetTransform()->GetPositionFloat3()).length();
		if (newDistance < distance)
		{
			index = i;
			distance = newDistance;
		}
	}
	m_pTarget = m_Targets.at(index);
}

void component::AiComponent::findPathToTarget()
{

	m_NextPath.clear();
	m_StartPos = m_pParent->GetComponent<component::TransformComponent>()->GetTransform()->GetPositionFloat3();
	m_GoalPos = m_pTarget->GetComponent<component::TransformComponent>()->GetTransform()->GetPositionFloat3();

	m_pStartQuad = m_pNavMesh->GetQuad(m_StartPos);
	m_pCurrentQuad = m_pNavMesh->GetQuad(m_StartPos);
	m_pGoalQuad = m_pNavMesh->GetQuad(m_GoalPos);

	if (m_pStartQuad == m_pGoalQuad)
	{
		m_PathFound = true;
		return;
	}

	for (unsigned int i = 0; i < m_pNavMesh->GetNumQuads(); ++i)
	{
		m_pQuads[i]->g = 0;
		m_pQuads[i]->f = 0;
		m_pQuads[i]->closed = false;
		m_pQuads[i]->parent = nullptr;
	}
	m_OpenList.clear();

	m_pQuads[m_pStartQuad->id]->parent = m_pQuads[m_pStartQuad->id];
	m_pQuads[m_pStartQuad->id]->closed = true;

	checkAdjacent();

	bool found = false;
	do
	{
		found = moveToNextTile();
	} while (!m_OpenList.empty() && !found);

	float2 topLeft, topRight, bottomLeft, bottomRight, pointCurrentQuad, pointGoalQuad;

	do
	{
		NavQuad* parentQuad = m_pNavMesh->GetAllQuads()[m_pQuads[m_pCurrentQuad->id]->parent->id];

		topLeft = { m_pCurrentQuad->position.x - (m_pCurrentQuad->size.x / 2.0f), m_pCurrentQuad->position.z + (m_pCurrentQuad->size.y / 2.0f) };
		topRight = { m_pCurrentQuad->position.x + (m_pCurrentQuad->size.x / 2.0f), m_pCurrentQuad->position.z + (m_pCurrentQuad->size.y / 2.0f) };
		bottomLeft = { m_pCurrentQuad->position.x - (m_pCurrentQuad->size.x / 2.0f), m_pCurrentQuad->position.z - (m_pCurrentQuad->size.y / 2.0f) };
		bottomRight = { m_pCurrentQuad->position.x + (m_pCurrentQuad->size.x / 2.0f), m_pCurrentQuad->position.z - (m_pCurrentQuad->size.y / 2.0f) };
		pointCurrentQuad = { parentQuad->position.x, parentQuad->position.z };
		pointGoalQuad = { m_pGoalQuad->position.x, m_pGoalQuad->position.z };

		if (!checkIntersect(pointCurrentQuad, pointGoalQuad, topLeft, topRight, bottomLeft, bottomRight) || !m_NextPath.empty() || (parentQuad->position.x != m_pGoalQuad->position.x && parentQuad->position.z != m_pGoalQuad->position.z))
		{
			m_NextPath.push_back(m_pCurrentQuad->position);
		}

		m_pCurrentQuad = parentQuad;
	} while (m_pCurrentQuad != m_pStartQuad);

	m_PathFound = true;
}

void component::AiComponent::checkAdjacent()
{
	for (Connection* connection : m_pCurrentQuad->connections)
	{
		NavQuad* quad = connection->GetConnectedQuad(m_pCurrentQuad);
		if (!m_pQuads[quad->id]->closed)
		{
			if (m_pQuads[quad->id]->g == 0 || m_pQuads[quad->id]->g > m_pQuads[m_pCurrentQuad->id]->g)
			{
				m_OpenList.push_back(quad->id);
				m_pQuads[quad->id]->parent = m_pQuads[m_pCurrentQuad->id];

				m_pQuads[quad->id]->g = m_pQuads[m_pCurrentQuad->id]->g + (quad->position - m_pCurrentQuad->position).length();

				float h = abs(quad->position.x - m_GoalPos.x) + abs(quad->position.z - m_GoalPos.z);
				m_pQuads[quad->id]->f = m_pQuads[quad->id]->g + h;
			}
		}
	}
}

bool component::AiComponent::moveToNextTile()
{
	if (!m_OpenList.empty())
	{
		int index = 0;
		float f = m_pQuads[m_OpenList.back()]->f;
		for (unsigned int i = 0; i < m_OpenList.size(); ++i)
		{
			if ((m_pQuads[m_OpenList.at(i)]->f < f) && !(m_pQuads[m_OpenList.at(i)]->closed))
			{
				f = m_pQuads[m_OpenList.at(i)]->f;
				index = i;
			}
		}

		m_pCurrentQuad = m_pNavMesh->GetAllQuads()[m_OpenList.at(index)];
		m_pQuads[m_OpenList.at(index)]->closed = true;
		m_OpenList.erase(m_OpenList.begin() + index);

		checkAdjacent();

		return m_pCurrentQuad == m_pGoalQuad;
	}

	return false;
}

bool component::AiComponent::checkIntersect(float2 point1, float2 point2, float2 topLeft, float2 topRight, float2 bottomLeft, float2 bottomRight)
{
	if ((lineFunction(topLeft, point1, point2) > 0 && lineFunction(topRight, point1, point2) > 0 && lineFunction(bottomLeft, point1, point2) > 0 && lineFunction(bottomRight, point1, point2) > 0) ||
		(lineFunction(topLeft, point1, point2) < 0 && lineFunction(topRight, point1, point2) < 0 && lineFunction(bottomLeft, point1, point2) < 0 && lineFunction(bottomRight, point1, point2) < 0) ||
		(point1.x > topRight.x && point2.x > topRight.x) || (point1.x < bottomLeft.x && point2.x < bottomLeft.x) || (point1.y > topRight.y && point2.y > topRight.y) || (point1.y < bottomLeft.y && point2.y < bottomLeft.y))
	{
		return false;
	}

	return true;
}

float component::AiComponent::lineFunction(float2 point, float2 linePoint1, float2 linePoint2)
{
	return (linePoint2.y - linePoint1.y) * point.x + (linePoint1.x - linePoint2.x) * point.y + (linePoint2.x * linePoint1.y - linePoint1.x * linePoint2.y);
}
