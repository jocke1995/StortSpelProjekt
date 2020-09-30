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
		CollisionComponent(Entity* parent, double mass = 1.0, double friction = 1.0, double restitution = 0.5);
		virtual ~CollisionComponent();
		// Checks for collision between two objects. If collision occurs, the transform is adjusted accordingly.
		//virtual void CheckCollision(CollisionComponent* other) = 0;
		btRigidBody* GetBody();

		void Update(double dt);
		void InitScene();
		void SetPosition(double x, double y, double z);
		void SetRotation(double roll, double pitch, double yaw);
		void SetRotation(double3 axis, double angle);
		void Rotate(double3 axis, double angle);
		void SetVelVector(double x, double y, double z);
		void SetNormalizedVelVector(double x, double y, double z);
		void SetAngularVelocity(double x, double y, double z);
		void SetFriction(double fric);
		void SetRestitution(double rest);
		void SetAngularFactor(double3& factor);
		void SetLinearFactor(double3& factor);

		double3 GetPosition();
		double3 GetRotationEuler();
		double4 GetRotationQuaternion();
		double3 GetLinearVelocity();
		double3 GetAngularVelocity();
		double GetFriction();
		double GetRestitution();
		double3 GetAngularFactor();
		double3 GetLinearFactor();

		double CastRay(double3 castTo);
		double CastRay(double3 direction, double length);
	protected:
		double m_Mass;
		double m_Fric;
		double m_Rest;
		Transform* m_pTrans;
		btCollisionShape* m_pShape;
		btRigidBody* m_pBody;
		btMotionState* m_pMotionState;
	};
}
#endif