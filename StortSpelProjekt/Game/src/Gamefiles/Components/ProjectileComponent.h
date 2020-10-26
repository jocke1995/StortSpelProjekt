#ifndef PROJECTILE_COMPONENT_H
#define PROJECTILE_COMPONENT_H

#include "../../Engine/src/ECS/Components/Component.h"

struct Collision;
class Audio3DEmitterComponent;

namespace component 
{
	class ProjectileComponent : public Component
	{
	public:
		ProjectileComponent(Entity* parent, int damage);
		~ProjectileComponent();
		void Update(float dt);

		void OnInitScene();
		void OnUnInitScene();

		int GetDamage() const;

	private:
		int m_Damage;

		void hit(Collision* event);
	};
}


#endif // !PROJECTILE_COMPONENT_H
