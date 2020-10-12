#ifndef AICOMPONENT_H
#define AICOMPONENT_H
#include "../ECS/Components/Component.h"
#include "EngineMath.h"
#include <vector>
#include <string>

class EngineRand;

namespace component
{
	class AiComponent : public Component
	{
	public:
		// Default Settings
		AiComponent(Entity* parent, Entity* target, bool canJump = false, float detectionRadius = 25.0f, float attackingDistance = 3.5f);
		virtual ~AiComponent();

		void Update(double dt);
		void RenderUpdate(double dt);

		void AddTarget(Entity* target);
		void RemoveTarget(std::string name);

	private:
		Entity* m_pTarget;
		std::vector<Entity*> m_Targets;
		float m_DetectionRadius;
		float m_AttackingDistance;
		bool m_CanJump;

		void selectTarget();
	};
}

#endif