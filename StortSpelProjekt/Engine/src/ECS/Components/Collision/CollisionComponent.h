#ifndef COLLISIONBODYCOMPONENT_H
#define COLLISIONBODYCOMPONENT_H
#define COR 1.0f
#include "../Component.h"

class Transform;
class btRigidBody;
namespace component
{
	class CollisionComponent : public Component
	{
	public:
		CollisionComponent(Entity* parent, Transform* trans, float mass = 1.0f);
		virtual ~CollisionComponent();
		// Checks for collision between two objects. If collision occurs, the transform is adjusted accordingly.
		//virtual void CheckCollision(CollisionComponent* other) = 0;
		void SetMovement(float x, float y, float z);
		btRigidBody* GetBody();

		void Update(double dt);
	protected:
		float m_Mass;
		Transform* m_pTrans;
		btRigidBody* m_pBody;
	};
}
#endif