#ifndef ACCELERATIONCOMPONENT_H
#define ACCELERATIONCOMPONENT_H
#include "../ECS/Components/Component.h"
#include "EngineMath.h"
class Entity;

namespace component 
{
	class AccelerationComponent : public Component
	{
	public:
		AccelerationComponent(Entity* parent, double gravity = 0);
		virtual ~AccelerationComponent();

		void Update(double dt);

		void OnInitScene();
		void OnUnInitScene();

		DirectX::XMFLOAT3 GetDirection() const;
		float GetAccSpeed() const;

		void SetAccelerationDirection(DirectX::XMFLOAT3 dir);
		void SetAccelerationDirection(float3 dir);
		void SetAccelerationDirection(float x, float y, float z);
		void SetAccelerationSpeed(float speed);
	private:
		DirectX::XMFLOAT3 m_AccDir;
		float m_AccSpeed;
		double m_Gravity;
	};
}

#endif