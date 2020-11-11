#ifndef RANGE_COMPONENT_H
#define RANGE_COMPONENT_H

#include "../ECS/Components/Component.h"


class SceneManager;
class Scene;
class Model;
class Audio2DVoiceComponent;
struct MouseClick;

namespace component
{
	class RangeComponent : public Component
	{
	public:
		RangeComponent(Entity* parent, SceneManager* sm, Scene* scene, Model* model, float scale = 1, int damage = 1, float velocity = 2.5);
		~RangeComponent();

		void OnInitScene();
		void OnUnInitScene();
		void Update(double dt);

		void SetVelocity(float velocity);
		float GetVelocity();
		void SetAttackInterval(float interval);
		void SetDamage(int damage);
		void ChangeDamage(int change);
		float GetAttackInterval() const;

		void Attack();
		
	private:
		int m_Damage;
		float m_Velocity;
		float m_Scale;
		int m_NrOfProjectiles;
		float m_AttackInterval;
		double m_TimeAccumulator;

		bool m_AudioPlay;

		Model* m_pModel = nullptr;
		SceneManager* m_pSceneMan = nullptr;
		Scene* m_pScene = nullptr;

		Audio2DVoiceComponent* m_pVoiceComponent;

		
	};
}


#endif