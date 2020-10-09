#ifndef RANGE_COMPONENT_H
#define RANGE_COMPONENT_H

#include "../ECS/Components/Component.h"
#include "Engine.h"
#include <vector>

class SceneManager;
class Scene;
class Model;
struct MouseClick;
namespace component
{
	class RangeComponent : public Component
	{
	public:
		RangeComponent(Entity* parent, SceneManager* sm, Scene* scene, Model* model, float scale = 1, int damage = 1, float velocity = 2.5);
		~RangeComponent();
		void Attack();

		std::vector<Entity*> GetProjectileList();
		void ClearProjectileList();

	private:
		int m_Damage;
		float m_Velocity;
		float m_Scale;
		int m_NrOfProjectiles = 0;

		std::vector<Entity*> m_ProjectileList;

		Model* m_pModel = nullptr;
		SceneManager* m_pSceneMan = nullptr;
		Scene* m_pScene = nullptr;
	};
}


#endif