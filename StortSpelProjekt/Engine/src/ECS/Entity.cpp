#include "stdafx.h"
#include "Entity.h"

Entity::Entity(std::string entityName)
{
	m_Id = staticID++;
	m_Name = entityName;

	m_ReferenceCount = 1;
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

unsigned int Entity::GetRefCount() const
{
	return m_ReferenceCount;
}

void Entity::IncrementRefCount()
{
	m_ReferenceCount++;
}

void Entity::DecrementRefCount()
{
	m_ReferenceCount--;
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

std::vector<Component*>* Entity::GetAllComponents()
{
	return &m_Components;
}
