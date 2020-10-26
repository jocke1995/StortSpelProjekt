#include "EnemyComponent.h"
#include "EnemyFactory.h"
component::EnemyComponent::EnemyComponent(Entity* parent, EnemyFactory* factory): Component(parent), m_pFactory(factory)
{
}

component::EnemyComponent::~EnemyComponent()
{
	m_pFactory->RemoveEnemyFromList(m_pParent);
}

void component::EnemyComponent::OnInitScene()
{
	m_pFactory->AddEnemyToList(m_pParent);
}

void component::EnemyComponent::OnUnInitScene()
{
	m_pFactory->RemoveEnemyFromList(m_pParent);
}
