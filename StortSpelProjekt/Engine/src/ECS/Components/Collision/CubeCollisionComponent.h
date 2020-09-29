#ifndef CUBECOLLISIONCOMPONENT_H
#define CUBECOLLISIONCOMPONENT_H
#include "CollisionComponent.h"

namespace component
{
	class CubeCollisionComponent : public CollisionComponent
	{
	public:
		CubeCollisionComponent(Entity* parent, double mass = 1.0, double friction = 1.0, double restitution = 0.5, float width = 1.0f, float height = 1.0f, float depth = 1.0f);
		~CubeCollisionComponent();
	private:
		float m_Rad;
	};
}
#endif