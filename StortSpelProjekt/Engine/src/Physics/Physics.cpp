#include "stdafx.h"
#include "Physics.h"

#include"../ECS/Entity.h"
#include "../Events/EventBus.h"

Physics::Physics()
{
	// makes it so printCollisions() prints when a collision event happens
	EventBus::GetInstance().Subscribe(this, &Physics::printCollisions);
}

Physics& Physics::GetInstance()
{
	static Physics instance;
	return instance;
}

Physics::~Physics()
{
}

void Physics::Update(double dt)
{
	collisionChecks(dt);
}

bool Physics::CheckOBBCollision(const DirectX::BoundingOrientedBox* obb1, const DirectX::BoundingOrientedBox* obb2) const
{
	return obb1->Intersects(*obb2);
}

void Physics::AddCollisionEntity(Entity *ent)
{
	if (ent->GetComponent<component::BoundingBoxComponent>()->GetFlagOBB() & F_OBBFlags::COLLISION)
	{
		m_CollisionEntities.push_back(ent);
	}
	else
	{
		Log::PrintSeverity(Log::Severity::WARNING, 
			"Trying to add an object \"%s\" without collision flag to m_CollisionEntities in \"Physics::AddCollisionEntity\"", 
			ent->GetName().c_str());
	}
}

void Physics::collisionChecks(double dt)
{
	m_timeSinceLastColCheck += dt;

	if (m_timeSinceLastColCheck > m_CollisionUpdateInterval)
	{
		// if there is 0 or only 1 object in our vector then we don't have to check collision
		if (m_CollisionEntities.size() > 1)
		{
			for (int i = 0; i < m_CollisionEntities.size(); i++)
			{
				for (int j = i + 1; j < m_CollisionEntities.size(); j++)
				{
					if (CheckOBBCollision(
						m_CollisionEntities.at(i)->GetComponent<component::BoundingBoxComponent>()->GetOBB(),
						m_CollisionEntities.at(j)->GetComponent<component::BoundingBoxComponent>()->GetOBB()))
					{
						EventBus::GetInstance().Publish(&Collision(m_CollisionEntities.at(i), m_CollisionEntities.at(j)));
					}
				}
			}
		}
		m_timeSinceLastColCheck = 0;
	}
}

void Physics::printCollisions(Collision* event)
{
	Log::Print("%s collided with %s\n", event->ent1->GetName().c_str(), event->ent2->GetName().c_str());
}
