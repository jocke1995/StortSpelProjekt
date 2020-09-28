#ifndef CUBECOLLISIONCOMPONENT_H
#define CUBECOLLISIONCOMPONENT_H
#include "CollisionComponent.h"

class btBoxShape;
class btMotionState;

namespace component
{
	class CubeCollisionComponent : public CollisionComponent
	{
	public:
		CubeCollisionComponent(Entity* parent, Transform* trans, float mass = 1.0f, float width = 1.0f, float height = 1.0f, float depth = 1.0f);
		~CubeCollisionComponent();
		virtual void CheckCollision(CollisionComponent* other);
		void Update(double dt);
	private:
		float m_Rad;
		btBoxShape* m_pBox;
		btMotionState* m_pMotionState;
	};
}
#endif