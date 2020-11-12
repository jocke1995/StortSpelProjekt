#include "stdafx.h"
#include "Entity.h"

Entity::Entity(std::string entityName)
{
	m_Id = staticID++;
	m_Name = entityName;
}

bool Entity::operator==(const Entity* rhs) const
{
	if (m_Id == rhs->m_Id)
	{
		return true;
	}
	return false;
}

Entity::~Entity()
{
	for (Component* component : m_Components)
	{
		delete component;
	}
}

unsigned int Entity::GetID() const
{
	return m_Id;
}

std::string Entity::GetName() const
{
	return m_Name;
}

void Entity::Update(double dt)
{
	for (Component* component : m_Components)
	{
		component->Update(dt);
	}
}

void Entity::RenderUpdate(double dt)
{
	for (Component* component : m_Components)
	{
		component->RenderUpdate(dt);
	}
}

void Entity::OnInitScene()
{
	// for each component in entity: call their implementation of InitScene(),
	// which calls their specific init function (render, audio, game, physics etc)

	for (int i = 0; i < m_Components.size(); i++)
	{
		m_Components.at(i)->OnInitScene();
	}
}

void Entity::OnUnInitScene()
{
	// for each component in entity: call their implementation of InitScene(),
	// which calls their specific init function (render, audio, game, physics etc)
	for (int i = 0; i < m_Components.size(); i++)
	{
		m_Components.at(i)->OnUnInitScene();
	}
}

std::vector<Component*>* Entity::GetAllComponents()
{
	return &m_Components;
}

void Entity::SetEntityState(bool dynamic)
{
	m_Dynamic = dynamic;
}

bool Entity::IsEntityDynamic() const
{
	return m_Dynamic;
}
