#ifndef AICOMPONENT_H
#define AICOMPONENT_H
#include "../ECS/Components/Component.h"
#include "EngineMath.h"

class EngineRand;

namespace component
{
	class AiComponent : public Component
	{
	public:
		// Default Settings
		AiComponent(Entity* parent, Entity* target, float detectionRadius = 25.0f, float attackingDistance = 3.5f);
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
	};
}

#endif