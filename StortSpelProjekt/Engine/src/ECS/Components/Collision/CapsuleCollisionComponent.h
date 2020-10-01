#ifndef CAPSULECOLLISIONCOMPONENT_H
#define CAPSULECOLLISIONCOMPONENT_H
#include "CollisionComponent.h"

namespace component
{
	class CapsuleCollisionComponent : public CollisionComponent
	{
	public:
		CapsuleCollisionComponent(Entity* parent, double mass = 1.0, double radius = 1.0f, double height = 2.0, double friction = 1.0, double restitution = 0.5);
		~CapsuleCollisionComponent();
		virtual double GetDistanceToBottom() const;
	private:
	};
}

#endif