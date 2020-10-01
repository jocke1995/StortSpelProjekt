#include "stdafx.h"
#include "Physics.h"

#include"../ECS/Entity.h"
#include "../Events/EventBus.h"
#include "../ECS/Components/Collision/CollisionComponent.h"

Physics::Physics()
{
	m_pCollisionConfig = new btDefaultCollisionConfiguration();
	m_pDispatcher = new btCollisionDispatcher(m_pCollisionConfig);
	m_pBroadphase = new btDbvtBroadphase();
	m_pSolver = new btSequentialImpulseConstraintSolver();
	m_pWorld = new btDiscreteDynamicsWorld(m_pDispatcher,m_pBroadphase,m_pSolver, m_pCollisionConfig);
	m_pWorld->setGravity({ 0.0, -50, 0.0 });
}

Physics& Physics::GetInstance()
{
	static Physics instance;
	return instance;
}

Physics::~Physics()
{
}

void Physics::DestroyPhysics()
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

void Physics::AddCollisionComponent(component::CollisionComponent* comp)
{
	m_CollisionComponents.push_back(comp);
	m_pWorld->addRigidBody(comp->GetBody());
}

void Physics::RemoveCollisionComponent(component::CollisionComponent* comp)
{
	for (int i = 0; i < m_CollisionComponents.size(); i++)
	{
		if (m_CollisionComponents.at(i) == comp)
			m_CollisionComponents.erase(m_CollisionComponents.begin() + i);
	}
	//m_pWorld->removeRigidBody(comp->GetBody());
}

const btDynamicsWorld* Physics::GetWorld()
{
	return m_pWorld;
}

void Physics::collisionChecks(double dt)
{
	m_timeSinceLastColCheck += dt;

	if (m_timeSinceLastColCheck > m_CollisionUpdateInterval)
	{
		collisionComponentChecks();
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
		m_pWorld->stepSimulation(dt);
	}
}

void Physics::collisionComponentChecks()
{
	// This is done in collisionChecks which is called before. Saved for future reference.
	//m_timeSinceLastColCheck += dt;

	for (int i = 0; i < m_CollisionComponents.size(); i++)
	{
		for (int j = i + 1; j < m_CollisionComponents.size(); j++)
		{
			//m_CollisionComponents[i]->CheckCollision(m_CollisionComponents[j]);
		}
	}
}
