#ifndef RANGEENEMYCOMPONENT_H
#define RANGEENEMYCOMPONENT_H
#include "../ECS/Components/Component.h"

class SceneManager;
class Scene;
class Model;
class Audio2DVoiceComponent;
class Texture2DGUI;

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

		void Attack(float3 direction);

	private:
		int m_Damage;
		float m_Velocity;
		float m_Scale;
		int m_NrOfProjectiles;
		float m_AttackInterval;
		double m_TimeAccumulator;
		std::string m_ParentName;

		bool m_AudioPlay;

		Model* m_pModel = nullptr;
		Texture2DGUI* m_pParticleTexture = nullptr;
		SceneManager* m_pSceneMan = nullptr;
		Scene* m_pScene = nullptr;

		Audio2DVoiceComponent* m_pVoiceComponent;
	};
}

#endif
