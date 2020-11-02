#ifndef PLAYERCOLLISIONCATEGORY_H
#define PLAYERCOLLISIONCATEGORY_H
#include "Physics/CollisionCategory.h"
class PlayerProjectileCollisionCategory;
class PlayerCollisionCategory : public CollisionCategory
{
public:
	PlayerCollisionCategory(Entity* parent);
	virtual ~PlayerCollisionCategory();
	void Collide(CollisionCategory* other) override;
	void Collide(PlayerProjectileCollisionCategory* other) override;
};

#endif