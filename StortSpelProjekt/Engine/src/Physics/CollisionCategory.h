#ifndef COLLISIONCATEGORY_H
#define COLLISIONCATEGORY_H
class Entity;

class PlayerCollisionCategory;
class PlayerProjectileCollisionCategory;
class EnemyCollisionCategory;
class EnemyProjectileCollisionCategory;

class CollisionCategory
{
public:
	CollisionCategory(Entity* parent);
	virtual ~CollisionCategory();
	Entity* GetParent() const;

	// The first collision function is only used to forward one of the categories to the other or to default call the collision event.
	virtual void Collide(CollisionCategory* other);

	// The baseclass should as default call the collision event if two objects are colliding with each other, but this is not a rule
	// so if any category is supposed to act different from other categories as default it should.
	// When a new collisioncategory is added it needs to have a collisionfunction declared in this baseclass.

	virtual void Collide(PlayerCollisionCategory* other);
	virtual void Collide(PlayerProjectileCollisionCategory* other);
	virtual void Collide(EnemyCollisionCategory* other);
	virtual void Collide(EnemyProjectileCollisionCategory* other);
protected:
	Entity* m_pParent;
};

#endif