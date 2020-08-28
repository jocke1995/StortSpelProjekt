#ifndef COMPONENT_H
#define COMPONENT_H

class Entity;

class Component
{
public:
	Component(Entity* parent);
	virtual ~Component();

	virtual void Update(double dt) = 0;
	Entity* const GetParent() const;

protected:
	Entity* parent = nullptr;
};

#endif
