#include "stdafx.h"
#include "CollisionCategory.h"
#include "../ECS/Entity.h"

CollisionCategory::CollisionCategory(Entity* parent) : m_pParent(parent)
{
}

CollisionCategory::~CollisionCategory()
{
}

Entity* CollisionCategory::GetParent() const
{
    return m_pParent;
}

void CollisionCategory::Collide(PlayerCollisionCategory* other)
{
}

void CollisionCategory::Collide(PlayerProjectileCollisionCategory* other)
{
}
