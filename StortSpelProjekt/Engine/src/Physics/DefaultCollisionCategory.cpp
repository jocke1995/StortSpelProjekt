#include "stdafx.h"
#include "DefaultCollisionCategory.h"
#include "../Events/EventBus.h"
DefaultCollisionCategory::DefaultCollisionCategory(Entity* parent): CollisionCategory(parent)
{
}

DefaultCollisionCategory::~DefaultCollisionCategory()
{
}

void DefaultCollisionCategory::Collide(CollisionCategory* other)
{
	EventBus::GetInstance().Publish(&Collision(m_pParent, other->GetParent()));
}
