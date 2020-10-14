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
	/* 
		The CollisionComponent is a base class for all collision detection and collision response. It should not be used on its 
		own as it requires a collisionshape for usage. When initalized it will be thrown into the world held by the physics 
		sub-engine.
	*/
	class CollisionComponent : public Component
	{
	public:
		// if Mass is zero, the object is static (It will not move because of collisions).
		CollisionComponent(Entity* parent, double mass = 1.0, double friction = 1.0, double restitution = 0.5, bool canFall = true);
		virtual ~CollisionComponent();
		
		void Update(double dt);

		// Initializes the component and gives it to the physics sub-engine. Needs to be called before any methods are used.
		void OnInitScene();
		void OnUnInitScene();
		
		// Sets the position of the object. This affects the transform from a transformcomponent as well.
		void SetPosition(double x, double y, double z);
		// Sets the rotation of the object using three angles (x,y,z). This affects the transform from a transformcomponent as well.
		void SetRotation(double roll, double pitch, double yaw);
		// Sets the rotation of the object using a quaternion. This affects the transform from a transformcomponent as well.
		void SetRotation(double3 axis, double angle);
		// Rotates the object by the given quaternion.
		void Rotate(double3 axis, double angle);
		// Sets a velocity for the object. This velocity is not constant and may dissapear due to friction.
		void SetVelVector(double x, double y, double z);
		// Sets an angular velocity for the object, rotating it along the given axis. This velocity is not constant and may dissapear due to friction.
		void SetAngularVelocity(double x, double y, double z);
		// Sets the friction of the object.
		void SetFriction(double fric);
		// Sets the Restitution of the object (bounciness).
		void SetRestitution(double rest);
		// Sets the angular factor, how much the object may rotate in the world. 0 will stop it from rotating in that axis.
		void SetAngularFactor(double3& factor);
		// Sets the linear factor, how much the object may move in the world. 0 will stop it from moving in that axis.
		void SetLinearFactor(double3& factor);


		btRigidBody* GetBody() const;
		double3 GetPosition() const;
		double3 GetRotationEuler() const;
		double4 GetRotationQuaternion() const;
		double3 GetLinearVelocity() const;
		double3 GetAngularVelocity() const;
		double GetFriction() const;
		double GetRestitution() const;
		double3 GetAngularFactor() const;
		double3 GetLinearFactor() const;

		// Casts a ray from the object to a given position, returning the minimal distance to another object. returns -1 if nothing is hit.
		double CastRay(double3 castTo) const;
		// Casts a ray from the object in the given direction and length, returning the minimal distance to another object. returns -1 if nothing is hit.
		double CastRay(double3 direction, double length) const;

		virtual double GetDistanceToBottom() const = 0;
	protected:
		bool m_CanFall;
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