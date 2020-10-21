#ifndef TELEPORT_COMPONENT_H
#define TELEPORT_COMPONENT_H

#include "../../Engine/src/ECS/Components/Component.h"

class Entity;
struct Collision;

namespace component
{
	class TeleportComponent : public Component
	{
	public:
		TeleportComponent(Entity* parent, Entity* player);
		~TeleportComponent();

		void OnInitScene();
		void OnUnInitScene();

		void OnCollision(Collision* collisionEvent);

		bool ChangeSceneThisFrame();
	private:
		bool m_ChangeSceneThisFrame = false;
		Entity* m_pPlayerInstance = nullptr;
	};
}

#endif
