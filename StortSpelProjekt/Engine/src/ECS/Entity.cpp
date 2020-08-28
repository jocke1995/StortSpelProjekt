#include "Entity.h"

Entity::Entity(std::string entityName)
{
	this->id = staticID++;
	this->name = entityName;

	this->referenceCount = 1;
}

bool Entity::operator==(const Entity* rhs) const
{
	if (this->id == rhs->id)
	{
		return true;
	}
	return false;
}

Entity::~Entity()
{
	for (Component* component : this->components)
	{
		delete component;
	}
}

unsigned int Entity::GetID() const
{
	return this->id;
}

std::string Entity::GetName() const
{
	return this->name;
}

unsigned int Entity::GetRefCount() const
{
	return this->referenceCount;
}

void Entity::IncrementRefCount()
{
	this->referenceCount++;
}

void Entity::DecrementRefCount()
{
	this->referenceCount--;
}

void Entity::Update(double dt)
{
	for (Component* component : this->components)
	{
		component->Update(dt);
	}
}
