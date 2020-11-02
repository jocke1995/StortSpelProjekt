#ifndef RANGEENEMYCOMPONENT_H
#define RANGEENEMYCOMPONENT_H
#include "../ECS/Components/Component.h"

class SceneManager;
class Scene;
class Model;
class Audio2DVoiceComponent;

namespace component
{
	class RangeEnemyComponent : public Component
	{
	public:
		RangeEnemyComponent(Entity* parent, SceneManager* sm, Scene* scene, Model* model, float scale = 1, int damage = 1, float velocity = 2.5);
		~RangeEnemyComponent();

		void OnInitScene();
		void OnUnInitScene();
		void Update(double dt);

		void SetVelocity(float velocity);
		float GetVelocity();
		void SetAttackInterval(float interval);
		float GetAttackInterval() const;

		void Attack();

	private:
		int m_Damage;
		float m_Velocity;
		float m_Scale;
		int m_NrOfProjectiles = 0;
		float m_AttackInterval;
		double m_TimeAccumulator;

		bool audioPlay;

		Model* m_pModel = nullptr;
		SceneManager* m_pSceneMan = nullptr;
		Scene* m_pScene = nullptr;

		Audio2DVoiceComponent* m_pVoiceComponent;
	};
}

#endif
