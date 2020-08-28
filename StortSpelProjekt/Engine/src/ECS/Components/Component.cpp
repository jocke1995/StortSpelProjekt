#include "Component.h"

Component::Component(Entity* parent)
{
	this->parent = parent;
}

Component::~Component()
{
}

Entity* const Component::GetParent() const
{
	return this->parent;
}
