#include "stdafx.h"
#include "PlayerProjectileCollisionCategory.h"
#include "PlayerCollisionCategory.h"
#include "../Events/EventBus.h"
#include "EnemyCollisionCategory.h"

PlayerProjectileCollisionCategory::PlayerProjectileCollisionCategory(Entity* parent) : CollisionCategory(parent)
{
}

PlayerProjectileCollisionCategory::~PlayerProjectileCollisionCategory()
{
}

void PlayerProjectileCollisionCategory::Collide(CollisionCategory* other)
{
	other->Collide(this);
}

void PlayerProjectileCollisionCategory::Collide(PlayerCollisionCategory* other)
{
	Log::Print("A projectile Collided with player, no collisionevent sent!\n");
	//EventBus::GetInstance().Publish(&Collision(m_pParent, other->GetParent()));
}

void PlayerProjectileCollisionCategory::Collide(EnemyCollisionCategory* other)
{
	other->Collide(this);
}
