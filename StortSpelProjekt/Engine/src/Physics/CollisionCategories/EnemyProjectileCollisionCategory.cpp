#include "stdafx.h"
#include "EnemyProjectileCollisionCategory.h"
#include "EnemyCollisionCategory.h"
#include "../Events/EventBus.h"

EnemyProjectileCollisionCategory::EnemyProjectileCollisionCategory(Entity* parent) : CollisionCategory(parent)
{
}

EnemyProjectileCollisionCategory::~EnemyProjectileCollisionCategory()
{
}

void EnemyProjectileCollisionCategory::Collide(CollisionCategory* other)
{
	other->Collide(this);
}

void EnemyProjectileCollisionCategory::Collide(EnemyCollisionCategory* other)
{
	Log::Print("A projectile Collided with ENEMY\n");
}
