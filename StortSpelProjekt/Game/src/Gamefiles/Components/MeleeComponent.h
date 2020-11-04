#ifndef MELEECOMPONENT_H
#define MELEECOMPONENT_H

// STL
#include <vector>

// Base level engine
#include "Core.h"
#include "EngineMath.h"

// Components
#include "../ECS/Components/Component.h"


// Sub-engines
#include "../Renderer/Transform.h"
#include "../Renderer/Mesh.h"
#include "../Physics/Physics.h"


class Entity;
class Audio2DVoiceComponent;
class Audio3DEmitterComponent;

#define MELEE_RANGE 12

namespace component
{
	class MeleeComponent : public Component
	{
	public:
		MeleeComponent(Entity* parent);

		virtual ~MeleeComponent();

		void OnInitScene();
		void OnUnInitScene();

		void Update(double dt);
		void Attack();
		
		void setAttackInterval(float interval);
		void SetDamage(int damage);
		void ChangeDamage(int change);

		void createCornersHitbox();
		void createDrawnHitbox(component::BoundingBoxComponent* bbc);

	private:

		std::vector<Vertex> m_BoundingBoxVerticesLocal;
		std::vector<unsigned int> m_BoundingBoxIndicesLocal;

		bool m_Attacking;
		bool m_Cooldown;
		bool audioPlay;
		float m_AttackInterval;
		float m_TimeSinceLastAttackCheck;
		int m_Damage;

		float3 m_HalfSize;

		Transform* m_pMeleeTransform;
		Transform m_MeleeTransformModified;

		Audio2DVoiceComponent* m_pVoiceComponent;

		Mesh* m_pMesh;

		DirectX::XMFLOAT3 m_Corners[8];
		component::BoundingBoxComponent* m_pBbc;

		DirectX::BoundingOrientedBox m_Hitbox;
		DirectX::BoundingOrientedBox m_TempHitbox;

		void CheckCollision();

	};
}
#endif
