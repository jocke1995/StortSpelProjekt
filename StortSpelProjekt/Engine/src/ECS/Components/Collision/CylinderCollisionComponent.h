#ifndef CYLINDERCOLLISIONCOMPONENT_H
#define CYLINDERCOLLISIONCOMPONENT_H
#include "CollisionComponent.h"

namespace component
{
	class CylinderCollisionComponent : public CollisionComponent
	{
	public:
		CylinderCollisionComponent(Entity* parent, double mass = 1.0, double radius = 1.0f, double height = 2.0, double friction = 1.0, double restitution = 0.5, bool canFall = true);
		CylinderCollisionComponent(Entity* parent, double mass = 1.0, double width = 1.0f, double height = 2.0, double depth = 1.0, double friction = 1.0, double restitution = 0.5, bool canFall = true);
		~CylinderCollisionComponent();
		virtual double GetDistanceToBottom() const;
	private:
	};
}

#endif