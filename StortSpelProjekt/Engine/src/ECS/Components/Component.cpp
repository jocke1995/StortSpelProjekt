#include "stdafx.h"
#include "Component.h"

Component::Component(Entity* parent)
{
	m_pParent = parent;
}

Component::~Component()
{
}

Entity* const Component::GetParent() const
{
	return m_pParent;
}
