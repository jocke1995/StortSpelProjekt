#include "Component.h"

Component::Component(Entity* parent)
{
	this->parent = parent;
}

Component::~Component()
{
}
