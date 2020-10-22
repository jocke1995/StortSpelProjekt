#ifndef PHYSICS_H
#define PHYSICS_H

#include<DirectXCollision.h>
#include<iostream>
#include<vector>

class Entity;
class Collision;
namespace component 
{
	class CollisionComponent;
}

// Bullet physics forward declaration.
class btDynamicsWorld;
class btDispatcher;
class btCollisionConfiguration;
class btDbvtBroadphase;
class btConstraintSolver;
class btRigidBody;

class Physics
{
public:
	static Physics& GetInstance();
	~Physics();
	// Needs to be called before any collisioncomponents are destroyed (before scenemanager kills them...)
	void DestroyPhysics();

	void Update(double dt);
	// returns true if there is an intersection between the OBBs
	bool CheckOBBCollision(const DirectX::BoundingOrientedBox* obb1, const DirectX::BoundingOrientedBox* obb2) const;
	std::vector<Entity*> SpecificCollisionCheck(const DirectX::BoundingOrientedBox* obb1);

	// Add an entity with collision enabled to the collision entities vector
	void AddCollisionEntity(Entity* ent);

	void AddCollisionComponent(component::CollisionComponent* comp);
	void RemoveCollisionComponent(component::CollisionComponent* comp);

	void SetCollisionEntities(const std::vector<Entity*>* collisionEntities);
	const std::vector<Entity*>* GetCollisionEntities() const;
	
	void OnResetScene();

	const btDynamicsWorld* GetWorld();
private:
	Physics();
	double m_TimeSinceLastColCheck;
	// How often the collisions are checked
	const double m_CollisionUpdateInterval;
	// Vector containing all objects collision will be checked for
	std::vector<Entity*> m_CollisionEntities;
	std::vector<component::CollisionComponent*> m_CollisionComponents;

	// bullet physics variables dynamicly allocated as their classes are abstract.
	btDynamicsWorld* m_pWorld;
	btDispatcher* m_pDispatcher;
	btCollisionConfiguration* m_pCollisionConfig;
	btDbvtBroadphase* m_pBroadphase;
	btConstraintSolver* m_pSolver;

	void removeAllCollisionComponents();
	void removeAllCollisionEntities();

	// Checks collision for all entities in the collison entities vector
	// publishes an event if a collision has happened
	void collisionChecks(double dt);
};

#endif
