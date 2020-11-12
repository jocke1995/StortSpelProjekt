#ifndef ENEMYCOLLISIONCATEGORY_H
#define ENEMYCOLLISIONCATEGORY_H
#include "../CollisionCategory.h"

class EnemyCollisionCategory : public CollisionCategory
{
public:
	EnemyCollisionCategory(Entity* parent);
	virtual ~EnemyCollisionCategory();
	void Collide(CollisionCategory* other) override;
	void Collide(EnemyProjectileCollisionCategory* other) override;
	void Collide(PlayerProjectileCollisionCategory* other) override;
};

#endif
