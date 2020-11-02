#ifndef DEFAULTCOLLISIONCATEGORY_H
#define DEFAULTCOLLISIONCATEGORY_H
#include "CollisionCategory.h"
class DefaultCollisionCategory : public CollisionCategory
{
public:
	DefaultCollisionCategory(Entity* parent);
	~DefaultCollisionCategory();
	void Collide(CollisionCategory* other) override;
};

#endif