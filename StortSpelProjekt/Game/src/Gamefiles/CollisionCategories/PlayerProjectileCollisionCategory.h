#ifndef PLAYERPROJECTILECOLLISIONCATEGORY_H
#define PLAYERPROJECTILECOLLISIONCATEGORY_H
#include "Physics/CollisionCategory.h"

class PlayerCollisionCategory;

class PlayerProjectileCollisionCategory : public CollisionCategory 
{
public:
	PlayerProjectileCollisionCategory(Entity* parent);
	virtual ~PlayerProjectileCollisionCategory();
	void Collide(CollisionCategory* other) override;
	void Collide(PlayerCollisionCategory* other) override;
};

#endif