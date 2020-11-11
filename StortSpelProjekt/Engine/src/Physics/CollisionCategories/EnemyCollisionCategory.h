#ifndef ENEMYCOLLISIONCATEGORY_H
#define ENEMYCOLLISIONCATEGORY_H
#include "../CollisionCategory.h"

class EnemyProjectileCollisionCategory;
class EnemyCollisionCategory : public CollisionCategory
{
public:
	EnemyCollisionCategory(Entity* parent);
	virtual ~EnemyCollisionCategory();
	void Collide(CollisionCategory* other) override;
	void Collide(EnemyProjectileCollisionCategory* other) override;
};

#endif
