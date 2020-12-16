#include "stdafx.h"
#include "CollisionCategory.h"
#include "../ECS/Entity.h"
#include "../Events/EventBus.h"
#include "CollisionCategories/PlayerCollisionCategory.h"
#include "CollisionCategories/PlayerProjectileCollisionCategory.h"
#include "CollisionCategories/EnemyCollisionCategory.h"
#include "CollisionCategories/EnemyProjectileCollisionCategory.h"

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

void CollisionCategory::Collide(CollisionCategory* other)
{
    EventBus::GetInstance().Publish(&Collision(m_pParent, other->GetParent()));
}

void CollisionCategory::Collide(PlayerCollisionCategory* other)
{
    EventBus::GetInstance().Publish(&Collision(m_pParent, other->GetParent()));
}

void CollisionCategory::Collide(PlayerProjectileCollisionCategory* other)
{
    EventBus::GetInstance().Publish(&Collision(m_pParent, other->GetParent()));
}

void CollisionCategory::Collide(EnemyCollisionCategory* other)
{
    EventBus::GetInstance().Publish(&Collision(m_pParent, other->GetParent()));
}

void CollisionCategory::Collide(EnemyProjectileCollisionCategory* other)
{
    EventBus::GetInstance().Publish(&Collision(m_pParent, other->GetParent()));
}
