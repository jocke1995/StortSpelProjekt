#include "stdafx.h"
#include "EnemyCollisionCategory.h"
#include "EnemyProjectileCollisionCategory.h"
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
	Log::Print("Enemy collided with enemy projectile!\n");
}
