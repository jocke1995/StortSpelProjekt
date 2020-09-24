#ifndef COLLISIONBODYCOMPONENT_H
#define COLLISIONBODYCOMPONENT_H
#define COR 0.8f
#include "../Component.h"

class Transform;
namespace component
{
	class CollisionComponent : public Component
	{
	public:
		CollisionComponent(Entity* parent, Transform* trans, float mass = 1.0f);
		// Checks for collision between two objects. If collision occurs, the transform is adjusted accordingly.
		virtual void CheckCollision(CollisionComponent* other) = 0;
	protected:
		float m_Mass;
		Transform* m_pTrans;
	};
}
#endif