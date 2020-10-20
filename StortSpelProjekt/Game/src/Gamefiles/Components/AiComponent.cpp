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
	m_pScene = nullptr;
	m_pNavMesh = nullptr;
	m_pQuads = nullptr;
	m_pCurrentQuad = nullptr;
}

component::AiComponent::~AiComponent()
{
	delete[] m_pQuads;
}

void component::AiComponent::SetScene(Scene* scene)
{
	m_pScene = scene;
	m_pNavMesh = scene->GetNavMesh();

	for (NavQuad* quad : m_pNavMesh->GetAllQuads())
	{
		float	right = quad->position.x + (quad->size.x / 2.0),
				left = quad->position.x - (quad->size.x / 2.0),
				forward = quad->position.z + (quad->size.y / 2.0),
				back = quad->position.z - (quad->size.y / 2.0);

		for (float i = back; i < forward; ++i)
		{
			m_pQuads[quad->id].entranceTilesRight.push_back({right, 0.0, i});
			m_pQuads[quad->id].entranceTilesLeft.push_back({left, 0.0, i});
		}
		for (float i = left; i < right; ++i)
		{
			m_pQuads[quad->id].entranceTilesForward.push_back({ i, 0.0, forward });
			m_pQuads[quad->id].entranceTilesBack.push_back({ i, 0.0, back });
		}
		
	}
}

void component::AiComponent::Update(double dt)
{
	if (m_pParent->GetComponent<component::HealthComponent>()->GetHealth() > 0)
	{
		selectTarget();
		//findPathToTarget();

		Transform* targetTrans = m_pTarget->GetComponent<component::TransformComponent>()->GetTransform();
		Transform* parentTrans = m_pParent->GetComponent<component::TransformComponent>()->GetTransform();
		CollisionComponent* cc = m_pParent->GetComponent<component::CollisionComponent>();

		float3 targetPos = m_Path.front();
		float3 pos = parentTrans->GetPositionFloat3();

		float3 direction = { targetPos.x - pos.x, (targetPos.y - pos.y) * static_cast<float>(m_Flags & F_AI_FLAGS::CAN_JUMP), targetPos.z - pos.z };
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
					m_pTarget->GetComponent<component::HealthComponent>()->ChangeHealth(-1);
				}
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

void component::AiComponent::OnLoadScene()
{
}

void component::AiComponent::OnUnloadScene()
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
	m_Path.clear();
	m_OpenList.clear();
	m_Tiles.clear();

	m_StartPos = m_pParent->GetComponent<component::TransformComponent>()->GetTransform()->GetPositionFloat3();
	m_GoalPos = m_pTarget->GetComponent<component::TransformComponent>()->GetTransform()->GetPositionFloat3();

	m_pCurrentQuad = m_pNavMesh->GetQuad(m_StartPos);

	m_Tiles[{m_StartPos.x, m_StartPos.z}].parent = m_StartPos;
	m_Tiles[{m_StartPos.x, m_StartPos.z}].closed = true;
	m_CurrentTile = m_StartPos;

	checkAdjacent();

	bool found = false;
	do
	{
		found = moveToNextTile();
	} while (!m_OpenList.empty() && !found);

	do
	{
		m_Path.push_back(m_CurrentTile);
		m_CurrentTile = m_Tiles[{m_CurrentTile.x, m_CurrentTile.z}].parent;
	} while (m_CurrentTile != m_StartPos);
}

void component::AiComponent::checkAdjacent()
{
	for (Connection* connection : m_pCurrentQuad->connections)
	{
		NavQuad* quad = connection->GetConnectedQuad(m_pCurrentQuad);

		if (quad->position.z > m_pCurrentQuad->position.z)
		{
			for (float3& tile : m_pQuads[quad->id].entranceTilesBack)
			{
				if (!m_Tiles[{tile.x, tile.z}].closed)
				{
					if (m_Tiles[{tile.x, tile.z}].g == 0 || m_Tiles[{tile.x, tile.z}].g > m_Tiles[{m_CurrentTile.x, m_CurrentTile.z}].g)
					{
						m_OpenList.push_back(tile);
						m_Tiles[{tile.x, tile.z}].parent = m_CurrentTile;

						m_Tiles[{tile.x, tile.z}].g = m_Tiles[{m_CurrentTile.x, m_CurrentTile.z}].g + (tile - m_CurrentTile).length();

						float h = (tile - m_GoalPos).length();
						m_Tiles[{tile.x, tile.z}].f = m_Tiles[{tile.x, tile.z}].g + h;
					}
				}
			}
		}
		else if (quad->position.z < m_pCurrentQuad->position.z)
		{
			for (float3& tile : m_pQuads[quad->id].entranceTilesForward)
			{
				if (!m_Tiles[{tile.x, tile.z}].closed)
				{
					if (m_Tiles[{tile.x, tile.z}].g == 0 || m_Tiles[{tile.x, tile.z}].g > m_Tiles[{m_CurrentTile.x, m_CurrentTile.z}].g)
					{
						m_OpenList.push_back(tile);
						m_Tiles[{tile.x, tile.z}].parent = m_CurrentTile;

						m_Tiles[{tile.x, tile.z}].g = m_Tiles[{m_CurrentTile.x, m_CurrentTile.z}].g + (tile - m_CurrentTile).length();

						float h = (tile - m_GoalPos).length();
						m_Tiles[{tile.x, tile.z}].f = m_Tiles[{tile.x, tile.z}].g + h;
					}
				}
			}
		}
		else if (quad->position.x > m_pCurrentQuad->position.x)
		{
			for (float3& tile : m_pQuads[quad->id].entranceTilesLeft)
			{
				if (!m_Tiles[{tile.x, tile.z}].closed)
				{
					if (m_Tiles[{tile.x, tile.z}].g == 0 || m_Tiles[{tile.x, tile.z}].g > m_Tiles[{m_CurrentTile.x, m_CurrentTile.z}].g)
					{
						m_OpenList.push_back(tile);
						m_Tiles[{tile.x, tile.z}].parent = m_CurrentTile;

						m_Tiles[{tile.x, tile.z}].g = m_Tiles[{m_CurrentTile.x, m_CurrentTile.z}].g + (tile - m_CurrentTile).length();

						float h = (tile - m_GoalPos).length();
						m_Tiles[{tile.x, tile.z}].f = m_Tiles[{tile.x, tile.z}].g + h;
					}
				}
			}
		}
		else if (quad->position.x < m_pCurrentQuad->position.x)
		{
			for (float3& tile : m_pQuads[quad->id].entranceTilesRight)
			{
				if (!m_Tiles[{tile.x, tile.z}].closed)
				{
					if (m_Tiles[{tile.x, tile.z}].g == 0 || m_Tiles[{tile.x, tile.z}].g > m_Tiles[{m_CurrentTile.x, m_CurrentTile.z}].g)
					{
						m_OpenList.push_back(tile);
						m_Tiles[{tile.x, tile.z}].parent = m_CurrentTile;

						m_Tiles[{tile.x, tile.z}].g = m_Tiles[{m_CurrentTile.x, m_CurrentTile.z}].g + (tile - m_CurrentTile).length();

						float h = (tile - m_GoalPos).length();
						m_Tiles[{tile.x, tile.z}].f = m_Tiles[{tile.x, tile.z}].g + h;
					}
				}
			}
		}
	}
}

bool component::AiComponent::moveToNextTile()
{
	if (!m_OpenList.empty())
	{
		int index = 0;
		float f = m_Tiles[{m_OpenList.back().x, m_OpenList.back().z}].f;
		for (unsigned int i = 0; i < m_OpenList.size(); ++i)
		{
			if ((m_Tiles[{m_OpenList.at(i).x, m_OpenList.at(i).z}].f < f) && !(m_Tiles[{m_OpenList.at(i).x, m_OpenList.at(i).z}].closed))
			{
				f = m_Tiles[{m_OpenList.at(i).x, m_OpenList.at(i).z}].f;
				index = i;
			}
		}

		m_CurrentTile = m_OpenList.at(index);
		m_Tiles[{m_CurrentTile.x, m_CurrentTile.z}].closed = true;
		m_OpenList.erase(m_OpenList.begin() + index);

		m_pCurrentQuad = m_pNavMesh->GetQuad(m_CurrentTile);

		checkAdjacent();

		return m_pCurrentQuad == m_pNavMesh->GetQuad(m_GoalPos);
	}

	return false;
}
