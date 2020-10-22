#ifndef TELEPORT_COMPONENT_H
#define TELEPORT_COMPONENT_H

#include "../../Engine/src/ECS/Components/Component.h"

#include <string>

class SceneManager;
class Scene;
class Entity;
struct Collision;

namespace component
{
	class TeleportComponent : public Component
	{
	public:
		TeleportComponent(Entity* parent, Entity* player, std::string newSceneName);
		~TeleportComponent();

		void OnInitScene();
		void OnUnInitScene();

		void OnCollision(Collision* collisionEvent);

	private:
		Entity* m_pPlayerInstance = nullptr;

		std::string m_NewSceneName = "";
	};
}

#endif
