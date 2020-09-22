#ifndef ACCELRATIONCOMPONENT_H
#define ACCELRATIONCOMPONENT_H
#include "../ECS/Components/Component.h"
#include "EngineMath.h"
class Entity;

namespace component 
{
	class AccelrationComponent : public Component
	{
	public:
		AccelrationComponent(Entity* parent, double gravity = 0);
		virtual ~AccelrationComponent();

		void Update(double dt);

		DirectX::XMFLOAT3 GetDirection() const;
		float GetAccSpeed() const;

		void SetAccelrationDirection(DirectX::XMFLOAT3 dir);
		void SetAccelrationSpeed(float speed);
	private:
		DirectX::XMFLOAT3 m_AccDir;
		float m_AccSpeed;
		double m_Gravity;
	};
}

#endif