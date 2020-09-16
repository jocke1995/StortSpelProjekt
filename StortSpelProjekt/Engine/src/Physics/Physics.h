#ifndef PHYSICS_H
#define PHYSICS_H

#include<DirectXCollision.h>
#include<iostream>
#include<vector>

class Entity;
class Collision;
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



private:
	Physics();
	double m_timeSinceLastColCheck = 0;
	// How often the collisions are checked
	const double m_CollisionUpdateInterval = 0.2;
	// Vector containing all objects collision will be checked for
	std::vector<Entity*> m_CollisionEntities;

	// Checks collision for all entities in the collison entities vector
	// publishes an event if a collision has happened
	void collisionChecks(double dt);
	// prints which entities that have collieded to the consol
	void printCollisions(Collision* event);


};

#endif
