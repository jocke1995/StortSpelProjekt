#ifndef MELEECOMPONENT_H
#define MELEECOMPONENT_H

#include "Core.h"
#include "EngineMath.h"
#include <vector>
#include "../Renderer/Transform.h"
#include "../Renderer/Mesh.h"
#include "../ECS/Components/Component.h"

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

		void AddEntityRock(Entity* ent);
		void CheckCollision();

	private:

		std::vector<Vertex> m_BoundingBoxVerticesLocal;
		std::vector<unsigned int> m_BoundingBoxIndicesLocal;

		bool m_Attacking;
		bool m_Cooldown;
		float m_attackIntervall;
		float m_timeSinceLastAttackCheck;

		Entity* collide;

		Transform* m_pMeleeTransform;
		Transform m_MeleeTransformTwo;

		Mesh* m_pMesh;

		DirectX::BoundingOrientedBox Hitbox;
		DirectX::BoundingOrientedBox tempHitbox;

	};
}
#endif
