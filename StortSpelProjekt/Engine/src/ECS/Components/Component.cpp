#include "stdafx.h"
#include "Component.h"

Component::Component(Entity* parent)
{
	m_pParent = parent;
}

Component::~Component()
{
}

void Component::Update(double dt)
{
}

void Component::RenderUpdate(double dt)
{
}

Entity* const Component::GetParent() const
{
	return m_pParent;
}
