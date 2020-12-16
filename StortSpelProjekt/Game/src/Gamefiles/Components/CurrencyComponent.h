#ifndef CURRENCY_COMPONENT_H
#define CURRENCY_COMPONENT_H

#include "../ECS/Components/Component.h"

class Entity;
struct Death;

namespace component
{
	class CurrencyComponent : public Component
	{
	public:
		CurrencyComponent(Entity* parent, int balance = 0);
		~CurrencyComponent();

		void OnInitScene();
		void OnUnInitScene();


		// set the total amount of currency available
		void SetBalance(int newBalance);
		// Used when adding or subtracting currency.
		// E.g. buying or getting currency as reward
		void ChangeBalance(Death* evnt);
		void ChangeBalance(int change);
		int GetBalace() const;
	private:
		int m_Balance;
		bool m_ComponentActive;
	};
}

#endif // !CURRENCY_COMPONENT_H