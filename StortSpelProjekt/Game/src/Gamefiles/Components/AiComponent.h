#ifndef AICOMPONENT_H
#define AICOMPONENT_H
#include "../ECS/Components/Component.h"
#include "EngineMath.h"
#include "Core.h"

class EngineRand;

enum F_AI_FLAGS
{
	CAN_JUMP = BIT(1),
	CAN_ROLL = BIT(2),
};

namespace component
{
	class AiComponent : public Component
	{
	public:
		// Default Settings
		AiComponent(Entity* parent, Entity* target, unsigned int flags = 0, float detectionRadius = 25.0f, float attackingDistance = 3.5f);
		virtual ~AiComponent();

		void Update(double dt);
		void RenderUpdate(double dt);

		void OnInitScene();
		void OnLoadScene();
		void OnUnloadScene();

	private:
		Entity* m_pTarget;
		float m_DetectionRadius;
		float m_AttackingDistance;
		unsigned int m_Flags;
	};
}

#endif