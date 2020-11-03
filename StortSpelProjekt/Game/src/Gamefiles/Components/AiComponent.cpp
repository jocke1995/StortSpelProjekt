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
	m_AttackSpeed = 0.1;
	m_IntervalTimeAccumulator = 0.0;
	m_SpeedTimeAccumulator = 0.0;
	m_MeleeAttackDmg = 10;
	m_pScene = nullptr;
	m_pNavMesh = nullptr;
	m_pQuads = nullptr;
	m_pTriangles = nullptr;
	m_pCurrentQuad = nullptr;
	m_pCurrentTriangle = nullptr;
	m_StartPos = m_pParent->GetComponent<component::TransformComponent>()->GetTransform()->GetPositionFloat3();
	m_GoalPos = m_pTarget->GetComponent<component::TransformComponent>()->GetTransform()->GetPositionFloat3();
	m_pStartQuad = nullptr;
	m_pGoalQuad = nullptr;
	m_pNextQuad = nullptr;
	m_pStartTriangle = nullptr;
	m_pGoalTriangle = nullptr;
	m_pNextTriangle = nullptr;
	m_LastPos = m_StartPos;
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

	for (unsigned int i = 0; i < m_pNavMesh->GetNumTriangles(); ++i)
	{
		delete m_pTriangles[i];
	}
	delete[] m_pTriangles;
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
	m_pTriangles = new PathTriangle*[m_pNavMesh->GetNumTriangles()];
	for (unsigned int i = 0; i < m_pNavMesh->GetNumTriangles(); ++i)
	{
		m_pTriangles[i] = new PathTriangle;
		m_pTriangles[i]->id = i;
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


		if (std::strcmp(m_pNavMesh->GetType().c_str(), "Quads") == 0)
		{
			NavQuad* targetQuad = m_pNavMesh->GetQuad(finalTargetPos);

			if (targetQuad != m_pGoalQuad)
			{
				findPathToTargetQuad();
			}

			if (m_PathFound)
			{
				m_Path = m_NextPath;
				m_LastPos = pos;
				m_PathFound = false;
			}

			m_pNextQuad = m_pNavMesh->GetQuad(m_NextTargetPos);

			if (m_pNavMesh->GetQuad(pos) == m_pNextQuad && !m_Path.empty())
			{
				m_LastPos = pos;
				m_Path.pop_back();
			}
		}
		else if (std::strcmp(m_pNavMesh->GetType().c_str(), "Triangles") == 0)
		{
			NavTriangle* targetTriangle = m_pNavMesh->GetTriangle(finalTargetPos);

			if (targetTriangle != m_pGoalTriangle)
			{
				findPathToTargetTriangle();
			}

			if (m_PathFound)
			{
				m_Path = m_NextPath;
				m_LastPos = pos;
				m_PathFound = false;
			}

			m_pNextTriangle = m_pNavMesh->GetTriangle(m_NextTargetPos);

			if (m_pNavMesh->GetTriangle(pos) == m_pNextTriangle && !m_Path.empty())
			{
				m_LastPos = pos;
				m_Path.pop_back();
			}
		}

		if (!m_Path.empty())
		{
			m_NextTargetPos = m_Path.back();
		}
		else
		{
			m_NextTargetPos = finalTargetPos;
			m_LastPos = pos;
		}

		float3 direction = { m_NextTargetPos.x - m_LastPos.x, (m_NextTargetPos.y - m_LastPos.y) * static_cast<float>(m_Flags & F_AI_FLAGS::CAN_JUMP), m_NextTargetPos.z - m_LastPos.z };

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

			m_IntervalTimeAccumulator += static_cast<float>(dt);
			float playerDistance = (targetTrans->GetPositionFloat3() - parentTrans->GetPositionFloat3()).length();
			if (playerDistance <= m_AttackingDistance)
			{
				// TODO: fix this when meele attack is implemented
				HealthComponent* hc = m_pTarget->GetComponent<component::HealthComponent>();
				if (hc != nullptr)
				{
					m_SpeedTimeAccumulator += static_cast<float>(dt);
					if (m_SpeedTimeAccumulator >= m_AttackSpeed && m_IntervalTimeAccumulator >= m_AttackInterval)
					{
						m_pTarget->GetComponent<component::HealthComponent>()->TakeDamage(-m_MeleeAttackDmg);
						Log::Print("ENEMY ATTACK!\n");
						m_SpeedTimeAccumulator = 0.0;
						m_IntervalTimeAccumulator = 0.0;
					}
				}
			}
			else
			{
				m_SpeedTimeAccumulator = 0.0;
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

void component::AiComponent::SetAttackSpeed(float speed)
{
	m_AttackSpeed = speed;
}

void component::AiComponent::SetMeleeAttackDmg(float dmg)
{
	m_MeleeAttackDmg = dmg;
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

void component::AiComponent::findPathToTargetQuad()
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

	checkAdjacentQuad();

	bool found = false;
	do
	{
		found = moveToNextQuad();
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

		if (!checkIntersectQuad(pointCurrentQuad, pointGoalQuad, topLeft, topRight, bottomLeft, bottomRight) || !m_NextPath.empty() || (parentQuad->position.x != m_pGoalQuad->position.x && parentQuad->position.z != m_pGoalQuad->position.z))
		{
			m_NextPath.push_back(m_pCurrentQuad->position);
		}

		m_pCurrentQuad = parentQuad;
	} while (m_pCurrentQuad != m_pStartQuad);

	m_PathFound = true;
}

void component::AiComponent::findPathToTargetTriangle()
{
	m_NextPath.clear();
	m_StartPos = m_pParent->GetComponent<component::TransformComponent>()->GetTransform()->GetPositionFloat3();
	m_GoalPos = m_pTarget->GetComponent<component::TransformComponent>()->GetTransform()->GetPositionFloat3();

	m_pStartTriangle = m_pNavMesh->GetTriangle(m_StartPos);
	m_pCurrentTriangle = m_pNavMesh->GetTriangle(m_StartPos);
	m_pGoalTriangle = m_pNavMesh->GetTriangle(m_GoalPos);

	if (m_pStartTriangle == m_pGoalTriangle)
	{
		m_PathFound = true;
		return;
	}

	for (unsigned int i = 0; i < m_pNavMesh->GetNumTriangles(); ++i)
	{
		m_pTriangles[i]->g = 0;
		m_pTriangles[i]->f = 0;
		m_pTriangles[i]->closed = false;
		m_pTriangles[i]->parent = nullptr;
	}
	m_OpenList.clear();

	m_pTriangles[m_pStartTriangle->id]->parent = m_pTriangles[m_pStartTriangle->id];
	m_pTriangles[m_pStartTriangle->id]->closed = true;

	checkAdjacentTriangle();

	bool found = false;
	do
	{
		found = moveToNextTriangle();
	} while (!m_OpenList.empty() && !found);

	float2 vertex1, vertex2, vertex3, pointCurrentTriangle, pointGoalTriangle;

	do
	{
		NavTriangle* parentTriangle = m_pNavMesh->GetAllTriangles()[m_pTriangles[m_pCurrentTriangle->id]->parent->id];

		vertex1 = { m_pCurrentTriangle->vertex1.x, m_pCurrentTriangle->vertex1.z };
		vertex2 = { m_pCurrentTriangle->vertex2.x, m_pCurrentTriangle->vertex2.z };
		vertex3 = { m_pCurrentTriangle->vertex3.x, m_pCurrentTriangle->vertex3.z };
		pointCurrentTriangle = { parentTriangle->center.x, parentTriangle->center.z };
		pointGoalTriangle = { m_pGoalTriangle->center.x, m_pGoalTriangle->center.z };

		if (!checkIntersectTriangle(pointCurrentTriangle, pointGoalTriangle, vertex1, vertex2, vertex3) || !m_NextPath.empty())
		{
			m_NextPath.push_back(m_pCurrentTriangle->center);
		}

		m_pCurrentTriangle = parentTriangle;
	} while (m_pCurrentTriangle != m_pStartTriangle);

	m_PathFound = true;
}

void component::AiComponent::checkAdjacentQuad()
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

void component::AiComponent::checkAdjacentTriangle()
{
	for (Connection* connection : m_pCurrentTriangle->connections)
	{
		NavTriangle* tri = connection->GetConnectedTriangle(m_pCurrentTriangle);
		if (!m_pTriangles[tri->id]->closed)
		{
			if (m_pTriangles[tri->id]->g == 0 || m_pTriangles[tri->id]->g > m_pTriangles[m_pCurrentTriangle->id]->g)
			{
				m_OpenList.push_back(tri->id);
				m_pTriangles[tri->id]->parent = m_pTriangles[m_pCurrentTriangle->id];

				m_pTriangles[tri->id]->g = m_pTriangles[m_pCurrentTriangle->id]->g + (tri->center - m_pCurrentTriangle->center).length();
				if (m_pTriangles[tri->id] == m_pTriangles[m_pGoalTriangle->id])
				{
					m_pTriangles[tri->id]->g = m_pTriangles[m_pCurrentTriangle->id]->g + (m_GoalPos - m_pCurrentTriangle->center).length();
				}
				if (m_pTriangles[tri->id] == m_pTriangles[m_pStartTriangle->id])
				{
					m_pTriangles[tri->id]->g = m_pTriangles[m_pCurrentTriangle->id]->g + (m_GoalPos - m_StartPos).length();
				}

				float h = (tri->center - m_GoalPos).length();
				m_pTriangles[tri->id]->f = m_pTriangles[tri->id]->g + h;
			}
		}
	}
}

bool component::AiComponent::moveToNextQuad()
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

		checkAdjacentQuad();

		return m_pCurrentQuad == m_pGoalQuad;
	}

	return false;
}

bool component::AiComponent::moveToNextTriangle()
{
	if (!m_OpenList.empty())
	{
		int index = 0;
		float f = m_pTriangles[m_OpenList.back()]->f;
		for (unsigned int i = 0; i < m_OpenList.size(); ++i)
		{
			if ((m_pTriangles[m_OpenList.at(i)]->f < f) && !(m_pTriangles[m_OpenList.at(i)]->closed))
			{
				f = m_pTriangles[m_OpenList.at(i)]->f;
				index = i;
			}
		}

		m_pCurrentTriangle = m_pNavMesh->GetAllTriangles()[m_OpenList.at(index)];
		m_pTriangles[m_OpenList.at(index)]->closed = true;
		m_OpenList.erase(m_OpenList.begin() + index);

		checkAdjacentTriangle();

		return m_pCurrentTriangle == m_pGoalTriangle;
	}

	return false;
}

bool component::AiComponent::checkIntersectQuad(float2 point1, float2 point2, float2 topLeft, float2 topRight, float2 bottomLeft, float2 bottomRight)
{
	if ((lineFunction(topLeft, point1, point2) > 0 && lineFunction(topRight, point1, point2) > 0 && lineFunction(bottomLeft, point1, point2) > 0 && lineFunction(bottomRight, point1, point2) > 0) ||
		(lineFunction(topLeft, point1, point2) < 0 && lineFunction(topRight, point1, point2) < 0 && lineFunction(bottomLeft, point1, point2) < 0 && lineFunction(bottomRight, point1, point2) < 0) ||
		(point1.x > topRight.x && point2.x > topRight.x) || (point1.x < bottomLeft.x && point2.x < bottomLeft.x) || (point1.y > topRight.y && point2.y > topRight.y) || (point1.y < bottomLeft.y && point2.y < bottomLeft.y))
	{
		return false;
	}

	return true;
}

bool component::AiComponent::checkIntersectTriangle(float2 point1, float2 point2, float2 vertex1, float2 vertex2, float2 vertex3)
{
	if (( lineFunction(vertex1, point1, point2) > 0 && lineFunction(vertex2, point1, point2) > 0 && lineFunction(vertex3, point1, point2) > 0 ) ||
		( lineFunction(vertex1, point1, point2) < 0 && lineFunction(vertex2, point1, point2) < 0 && lineFunction(vertex3, point1, point2) < 0 ))
	{
		return false;
	}

	return true;
}

float component::AiComponent::lineFunction(float2 point, float2 linePoint1, float2 linePoint2)
{
	return (linePoint2.y - linePoint1.y) * point.x + (linePoint1.x - linePoint2.x) * point.y + (linePoint2.x * linePoint1.y - linePoint1.x * linePoint2.y);
}
