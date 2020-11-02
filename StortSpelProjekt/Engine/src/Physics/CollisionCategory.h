#ifndef COLLISIONCATEGORY_H
#define COLLISIONCATEGORY_H

class Entity;

class CollisionCategory
{
public:
	CollisionCategory(Entity* parent);
	virtual void Collide(CollisionCategory* other) = 0;
private:
	Entity* m_pParent;
};

#endif