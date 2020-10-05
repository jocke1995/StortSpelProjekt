#ifndef CURRENCY_COMPONENT_H
#define CURRENCY_COMPONENT_H

#include "../Engine/src/ECS/Components/Component.h"

namespace component
{
	class CurrencyComponent : public Component
	{
	public:
		CurrencyComponent(Entity* parent, int balance = 0);
		~CurrencyComponent();

		// set the total amount of currency available
		void SetBalance(int newBalance);
		// Used when adding or subtracting currency.
		// E.g. buying or getting currency as reward
		void ChangeBalance(int change);
		int GetBalace() const;
	private:
		int m_balance;
	};
}

#endif // !CURRENCY_COMPONENT_H