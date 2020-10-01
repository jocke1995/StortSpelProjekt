#ifndef SPHERECOLLISIONCOMPONENT
#define SPHERECOLLISIONCOMPONENT

#include "CollisionComponent.h"

namespace component
{
	class SphereCollisionComponent : public CollisionComponent
	{
	public:
		SphereCollisionComponent(Entity* parent, double mass = 1.0, double radius = 1.0, double friction = 1.0, double restitution = 0.5);
		~SphereCollisionComponent();
		double GetDistanceToBottom() const;
	private:
	};
}
#endif