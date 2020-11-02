#ifndef COLLISIONCATEGORY_H
#define COLLISIONCATEGORY_H
class Entity;

class PlayerCollisionCategory;
class PlayerProjectileCollisionCategory;

class CollisionCategory
{
public:
	CollisionCategory(Entity* parent);
	virtual ~CollisionCategory();
	Entity* GetParent() const;

	// The first collision function is only used to forward one of the categories to the other.
	virtual void Collide(CollisionCategory* other) = 0;

	// The baseclass should as default do nothing when two categories are colliding with each other.
	// When a new collisioncategory is added it should have a collisionfunction declared in this baseclass.

	virtual void Collide(PlayerCollisionCategory* other);
	virtual void Collide(PlayerProjectileCollisionCategory* other);
protected:
	Entity* m_pParent;
};

#endif
