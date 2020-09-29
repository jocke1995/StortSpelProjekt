#ifndef COLLISIONBODYCOMPONENT_H
#define COLLISIONBODYCOMPONENT_H
#define COR 1.0f
#include "../Component.h"

class Transform;
class btRigidBody;
class btCollisionShape;
class btMotionState;
namespace component
{
	class CollisionComponent : public Component
	{
	public:
		CollisionComponent(Entity* parent, float mass = 1.0f);
		virtual ~CollisionComponent();
		// Checks for collision between two objects. If collision occurs, the transform is adjusted accordingly.
		//virtual void CheckCollision(CollisionComponent* other) = 0;
		btRigidBody* GetBody();

		void Update(double dt);
		void InitScene();
		void SetPosition(double x, double y, double z);
		void SetRotation(double roll, double pitch, double yaw);
		void SetVelVector(double x, double y, double z);
		void SetNormalizedVelVector(double x, double y, double z);
		void SetAngularVelocity(double x, double y, double z);
		void SetFriction(double fric);
		void SetRestitution(double rest);
	protected:
		float m_Mass;
		Transform* m_pTrans;
		btCollisionShape* m_pShape;
		btRigidBody* m_pBody;
		btMotionState* m_pMotionState;
	};
}
#endif