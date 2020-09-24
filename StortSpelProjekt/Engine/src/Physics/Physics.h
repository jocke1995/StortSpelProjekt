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
class Physics
{
public:
	static Physics& GetInstance();
	~Physics();

	void Update(double dt);
	// returns true if there is an intersection between the OBBs
	bool CheckOBBCollision(const DirectX::BoundingOrientedBox* obb1, const DirectX::BoundingOrientedBox* obb2) const;

	// Add an entity with collision enabled to the collision entities vector
	void AddCollisionEntity(Entity* ent);

	void AddCollisionComponent(component::CollisionComponent* comp);
	void RemoveCollisionComponent(component::CollisionComponent* comp);

private:
	Physics();
	double m_timeSinceLastColCheck = 0;
	// How often the collisions are checked
	const double m_CollisionUpdateInterval = 0.1;
	// Vector containing all objects collision will be checked for
	std::vector<Entity*> m_CollisionEntities;
	std::vector<component::CollisionComponent*> m_CollisionComponents;

	// Checks collision for all entities in the collison entities vector
	// publishes an event if a collision has happened
	void collisionChecks(double dt);
	// Checks collision for all added collision components
	void collisionComponentChecks();
	// prints which entities that have collieded to the consol
	void printCollisions(Collision* event);
};

#endif
