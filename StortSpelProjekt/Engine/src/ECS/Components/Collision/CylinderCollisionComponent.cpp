#include "stdafx.h"
#include "CylinderCollisionComponent.h"

component::CylinderCollisionComponent::CylinderCollisionComponent(Entity* parent, double mass, double radius, double height, double friction, double restitution, bool canFall) :
	CollisionComponent(parent,mass,friction,restitution,canFall)
{
	m_pShape = new btCylinderShape({radius,height,radius});
}

component::CylinderCollisionComponent::~CylinderCollisionComponent()
{
	delete m_pShape;
}

double component::CylinderCollisionComponent::GetDistanceToBottom() const
{
	return static_cast<btCylinderShape*>(m_pShape)->getHalfExtentsWithMargin().y();
}
