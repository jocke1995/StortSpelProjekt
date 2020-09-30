#ifndef MELEECOMPONENT_H
#define MELEECOMPONENT_H

#include "Core.h"
#include "EngineMath.h"
#include <vector>
#include "../Renderer/Transform.h"
#include "../Renderer/Mesh.h"
#include "../ECS/Components/Component.h"
#include "../Physics/Physics.h"

class Entity;

namespace component
{
	class MeleeComponent : public Component
	{
	public:
		MeleeComponent(Entity* parent);

		virtual ~MeleeComponent();

		//void Init();

		//void RenderUpdate(double dt);

		void Update(double dt);
		void Attack(bool attack);
		
		void setAttackIntervall(float intervall);

		void createCornersHitbox();
		void createDrawnHitbox(component::BoundingBoxComponent* bbc);

	private:

		std::vector<Vertex> m_BoundingBoxVerticesLocal;
		std::vector<unsigned int> m_BoundingBoxIndicesLocal;

		bool m_Attacking;
		bool m_Cooldown;
		float m_AttackIntervall;
		float m_TimeSinceLastAttackCheck;

		//std::vector<Entity*> collideList;

		Transform* m_pMeleeTransform;
		Transform m_MeleeTransformTwo;

		Mesh* m_pMesh;

		DirectX::XMFLOAT3 m_Corners[8];
		component::BoundingBoxComponent* m_pBbc;

		DirectX::BoundingOrientedBox m_Hitbox;
		DirectX::BoundingOrientedBox m_TempHitbox;

		void CheckCollision();

	};
}
#endif
