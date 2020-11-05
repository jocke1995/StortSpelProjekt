#include "stdafx.h"
#include "Physics.h"

#include"../ECS/Entity.h"
#include "../Events/EventBus.h"
#include "../ECS/Components/Collision/CollisionComponent.h"

Physics::Physics() : m_CollisionUpdateInterval(0.01)
{
	m_pCollisionConfig = new btDefaultCollisionConfiguration();
	m_pDispatcher = new btCollisionDispatcher(m_pCollisionConfig);
	m_pBroadphase = new btDbvtBroadphase();
	m_pSolver = new btSequentialImpulseConstraintSolver();
	m_pWorld = new btDiscreteDynamicsWorld(m_pDispatcher,m_pBroadphase,m_pSolver, m_pCollisionConfig);

	m_TimeSinceLastColCheck = 0;
}

Physics& Physics::GetInstance()
{
	static Physics instance;
	return instance;
}

Physics::~Physics()
{
}

void Physics::deletePhysics()
{
	for (int i = 0; i < m_CollisionComponents.size(); i++)
	{
		m_pWorld->removeCollisionObject(m_CollisionComponents[i]->GetBody());
	}

	delete m_pDispatcher;
	delete m_pBroadphase;
	delete m_pCollisionConfig;
	delete m_pSolver;
	delete m_pWorld;
}

void Physics::Update(double dt)
{
	collisionChecks(dt);
}

bool Physics::CheckOBBCollision(const DirectX::BoundingOrientedBox* obb1, const DirectX::BoundingOrientedBox* obb2) const
{
	return obb1->Intersects(*obb2);
}

std::vector<Entity*> Physics::SpecificCollisionCheck(const DirectX::BoundingOrientedBox* obb1)
{
	std::vector<Entity*> entityHitList;
	for (unsigned int i = 0; i < m_CollisionEntities.size(); i++)
	{
		if (obb1 != m_CollisionEntities.at(i)->GetComponent<component::BoundingBoxComponent>()->GetOBB())
		{
			if (CheckOBBCollision(obb1, m_CollisionEntities.at(i)->GetComponent<component::BoundingBoxComponent>()->GetOBB()))
			{
				entityHitList.push_back(m_CollisionEntities.at(i));
			}
		}
	}
	return entityHitList;
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

void Physics::RemoveCollisionEntity(Entity* ent)
{
	for (int i = 0; i < m_CollisionEntities.size(); i++)
	{
		if (m_CollisionEntities[i] == ent)
		{
			m_CollisionEntities.erase(m_CollisionEntities.begin() + i);
			break;
		}
	}
}

void Physics::AddCollisionComponent(component::CollisionComponent* comp)
{
	m_CollisionComponents.push_back(comp);
	m_pWorld->addRigidBody(comp->GetBody());
}

void Physics::RemoveCollisionComponent(component::CollisionComponent* comp)
{
	for (unsigned int i = 0; i < m_CollisionComponents.size(); i++)
	{
		if (m_CollisionComponents.at(i) == comp)
		{
			m_CollisionComponents.erase(m_CollisionComponents.begin() + i);
		}
	}
	m_pWorld->removeRigidBody(comp->GetBody());
}

void Physics::SetCollisionEntities(const std::vector<Entity*>* collisionEntities)
{
	m_CollisionEntities = *collisionEntities;
}

const std::vector<Entity*>* Physics::GetCollisionEntities() const
{
	return &m_CollisionEntities;
}

void Physics::OnResetScene()
{
	removeAllCollisionComponents();
	removeAllCollisionEntities();
}

const btDynamicsWorld* Physics::GetWorld()
{
	return m_pWorld;
}

void Physics::removeAllCollisionComponents()
{
	unsigned int size = m_CollisionComponents.size();
	for (unsigned int i = 0; i < size; i++)
	{
		component::CollisionComponent* colComp = m_CollisionComponents.at(i);
		m_pWorld->removeRigidBody(colComp->GetBody());
	}

	m_CollisionComponents.clear();
}

void Physics::removeAllCollisionEntities()
{
	m_CollisionEntities.clear();
}

void Physics::collisionChecks(double dt)
{
	m_TimeSinceLastColCheck += dt;

	if (m_TimeSinceLastColCheck > m_CollisionUpdateInterval)
	{
		// if there is 0 or only 1 object in our vector then we don't have to check collision
		if (m_CollisionEntities.size() > 1)
		{
			component::BoundingBoxComponent* first;
			component::BoundingBoxComponent* second;
			for (int i = 0; i < m_CollisionEntities.size(); i++)
			{
				first = m_CollisionEntities.at(i)->GetComponent<component::BoundingBoxComponent>();
				for (int j = i + 1; j < m_CollisionEntities.size(); j++)
				{
					second = m_CollisionEntities.at(j)->GetComponent<component::BoundingBoxComponent>();
					if (CheckOBBCollision(
						first->GetOBB(),
						second->GetOBB()))
					{
						first->Collide(*second);
						//EventBus::GetInstance().Publish(&Collision(m_CollisionEntities.at(i), m_CollisionEntities.at(j)));
					}
				}
			}
		}
		m_TimeSinceLastColCheck = 0;
		m_pWorld->stepSimulation(dt);
	}
}

