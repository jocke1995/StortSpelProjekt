#ifndef SPHERECOLLISIONCOMPONENT
#define SPHERECOLLISIONCOMPONENT

#include "CollisionComponent.h"

namespace component
{
	class SphereCollisionComponent : public CollisionComponent
	{
	public:
		SphereCollisionComponent(Entity* parent, float mass = 1.0f, float rad = 1.0f);
		~SphereCollisionComponent();
		virtual void CheckCollision(CollisionComponent* other);
	private:
		float m_Rad;
		void CheckCollisionSphere(SphereCollisionComponent* other);
	};
}
#endif