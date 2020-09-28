#ifndef SPHERECOLLISIONCOMPONENT
#define SPHERECOLLISIONCOMPONENT

#include "CollisionComponent.h"

class btSphereShape;
class btMotionState;

namespace component
{
	class SphereCollisionComponent : public CollisionComponent
	{
	public:
		SphereCollisionComponent(Entity* parent, Transform* trans, float mass = 1.0f, float rad = 1.0f);
		~SphereCollisionComponent();
		virtual void CheckCollision(CollisionComponent* other);
		void Update(double dt);
	private:
		float m_Rad;
		btSphereShape* m_pSphere;
		btMotionState* m_pMotionState;
		void CheckCollisionSphere(SphereCollisionComponent* other);
	};
}
#endif