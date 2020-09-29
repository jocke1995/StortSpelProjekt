#ifndef SPHERECOLLISIONCOMPONENT
#define SPHERECOLLISIONCOMPONENT

#include "CollisionComponent.h"

namespace component
{
	class SphereCollisionComponent : public CollisionComponent
	{
	public:
		SphereCollisionComponent(Entity* parent, double mass = 1.0, double friction = 1.0, double restitution = 0.5, float rad = 1.0f);
		~SphereCollisionComponent();
	private:
		float m_Rad;
	};
}
#endif