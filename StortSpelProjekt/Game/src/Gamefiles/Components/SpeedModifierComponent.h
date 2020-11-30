#ifndef SPEEDMODIFIERCOMPONENT_H
#define SPEEDMODIFIERCOMPONENT_H
#include "../ECS/Components/Component.h"
#include "EngineMath.h"
class Entity;

//This class manages all slows and speedup that the entity experiance.
//It only contains slows at the moment and any speedup can be added when needed.
namespace component 
{
	class SpeedModifier : public Component
	{
	public:
		SpeedModifier(Entity* parent);
		virtual ~SpeedModifier();

		void Update(double dt);

		void SetTemporayModifier(float tempMod);
		void UpdateBaseVelocity();

		void OnInitScene();
		void OnUnInitScene();

	private:
		float m_BaseVelocity;
		float m_TemporarySpeedMod;
	};
}

#endif