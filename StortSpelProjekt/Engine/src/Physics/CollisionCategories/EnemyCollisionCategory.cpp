#include "stdafx.h"
#include "EnemyCollisionCategory.h"
#include "EnemyProjectileCollisionCategory.h"
#include "PlayerProjectileCollisionCategory.h"
#include "../Events/EventBus.h"

EnemyCollisionCategory::EnemyCollisionCategory(Entity* parent) : CollisionCategory(parent)
{
}

EnemyCollisionCategory::~EnemyCollisionCategory()
{
}

void EnemyCollisionCategory::Collide(CollisionCategory* other)
{
	other->Collide(this);
}

void EnemyCollisionCategory::Collide(EnemyProjectileCollisionCategory* other)
{
	// Do nothing
}

void EnemyCollisionCategory::Collide(PlayerProjectileCollisionCategory* other)
{
	EventBus::GetInstance().Publish(&Collision(m_pParent, other->GetParent()));
}
