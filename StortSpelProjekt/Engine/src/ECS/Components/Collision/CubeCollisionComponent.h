#ifndef CUBECOLLISIONCOMPONENT_H
#define CUBECOLLISIONCOMPONENT_H
#include "CollisionComponent.h"

namespace component
{
	class CubeCollisionComponent : public CollisionComponent
	{
	public:
		CubeCollisionComponent(Entity* parent, float mass = 1.0f, float width = 1.0f, float height = 1.0f, float depth = 1.0f);
		~CubeCollisionComponent();
		virtual void CheckCollision(CollisionComponent* other);
	private:
		float m_Rad;
	};
}
#endif