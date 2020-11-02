#include "PlayerCollisionCategory.h"
#include "PlayerProjectileCollisionCategory.h"
#include "Physics/CollisionCategory.h"
#include "Headers/Core.h"
#include "Events/EventBus.h"

PlayerCollisionCategory::PlayerCollisionCategory(Entity* parent) : CollisionCategory(parent)
{
}

PlayerCollisionCategory::~PlayerCollisionCategory()
{
}

void PlayerCollisionCategory::Collide(CollisionCategory* other)
{
	other->Collide(this);
}

void PlayerCollisionCategory::Collide(PlayerProjectileCollisionCategory* other)
{
	Log::Print("A player collided with projectile, no collision event sent!\n");
	//EventBus::GetInstance().Publish(&Collision(m_pParent, other->GetParent()));
}
