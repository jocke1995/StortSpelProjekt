#ifndef COMPONENT_H
#define COMPONENT_H

#include "../../Engine/stdafx.h"
class Entity;

class Component
{
public:
	Component(Entity* parent);
	virtual ~Component();

	virtual void Update(double dt) = 0;

protected:
	Entity* parent = nullptr;
};

#endif
