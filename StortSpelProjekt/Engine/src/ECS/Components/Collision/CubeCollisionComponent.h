#ifndef CUBECOLLISIONCOMPONENT_H
#define CUBECOLLISIONCOMPONENT_H
#include "CollisionComponent.h"

namespace component
{
	class CubeCollisionComponent : public CollisionComponent
	{
	public:
		CubeCollisionComponent(Entity* parent, double mass = 1.0, double halfWidth = 1.0, double halfHeight = 1.0, double halfDepth = 1.0, double friction = 1.0, double restitution = 0.5, bool canFall = true);
		~CubeCollisionComponent();
		virtual double GetDistanceToBottom() const;
	private:
	};
}
#endif