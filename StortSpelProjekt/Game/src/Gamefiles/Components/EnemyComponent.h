#ifndef ENEMYCOMPONENT_H
#define ENEMYCOMPONENT_H
#include "../ECS/Components/Component.h"
class EnemyFactory;

namespace component
{
	class EnemyComponent : public Component
	{
	public:
		EnemyComponent(Entity* parent, EnemyFactory* factory);
		virtual ~EnemyComponent();

		//void Update(double dt);

		void OnInitScene();
		void OnUnInitScene();

	private:
		EnemyFactory* m_pFactory;
	};
}
#endif
