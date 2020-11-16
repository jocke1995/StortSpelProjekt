#ifndef ENEMYPROJECTILECOLLISIONCATEGORY_H
#define ENEMYPROJECTILECOLLISIONCATEGORY_H
#include "../CollisionCategory.h"

class EnemyProjectileCollisionCategory : public CollisionCategory
{
public:
	EnemyProjectileCollisionCategory(Entity* parent);
	virtual ~EnemyProjectileCollisionCategory();
	void Collide(CollisionCategory* other) override;
	void Collide(EnemyCollisionCategory* other) override;
};

#endif
