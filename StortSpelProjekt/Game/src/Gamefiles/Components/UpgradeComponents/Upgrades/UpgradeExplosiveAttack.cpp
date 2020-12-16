#include "UpgradeExplosiveAttack.h"
#include "EngineMath.h"
#include "ECS/Entity.h"
#include "Player.h"
#include "Components/ProjectileComponent.h"
#include "EnemyFactory.h"
#include "Misc/AssetLoader.h"
#include "Renderer/Texture/Texture2DGUI.h"
#include "ECS/SceneManager.h"
#include "Renderer/Transform.h"
#include "ECS/Components/TemporaryLifeComponent.h"
#include "ECS/Components/TransformComponent.h"

UpgradeExplosiveAttack::UpgradeExplosiveAttack(Entity* parent)
	:Upgrade(parent)
{
	// name the upgrade!
	SetName("UpgradeExplosiveAttack");
	// set the upgrade type/types!
	SetType(F_UpgradeType::RANGE);	// goes on the projectiles
	// set the price of this upgrade
	m_Price = 300;
	m_StartingPrice = m_Price;
	// We don't want the explosve damage given to enemies around the target to be more damage than the target itself.
	// So max level at lvl 10.
	m_MaxLevel = 10;

	// percentage of damage done to steal as life
	m_Radius = 10.0;
	m_PercentageDamage = 0.5;

	m_ImageName = "Explosion.png";
}

UpgradeExplosiveAttack::~UpgradeExplosiveAttack()
{
}

void UpgradeExplosiveAttack::IncreaseLevel()
{
	m_Level++;
	m_Radius = 10 + (4 * (m_Level - 1));
	m_PercentageDamage = 0.5 + (0.05 * m_Level);
	m_Price = m_StartingPrice * (m_Level + 1);
}

void UpgradeExplosiveAttack::OnRangedHit(Entity* target, Entity* projectile)
{
	// get damage from projectile
	int damage = m_pParentEntity->GetComponent<component::ProjectileComponent>()->GetDamage() * m_PercentageDamage;

	std::vector<Entity*>* enemies = EnemyFactory::GetInstance().GetAllEnemies();
	float3 hitTarget = projectile->GetComponent<component::TransformComponent>()->GetTransform()->GetPositionFloat3();
	for (int i = 0; i < enemies->size(); i++)
	{
		if (enemies->at(i) != target)
		{
			if (m_Radius > (hitTarget - enemies->at(i)->GetComponent<component::TransformComponent>()->GetTransform()->GetPositionFloat3()).length())
			{
				enemies->at(i)->GetComponent<component::HealthComponent>()->TakeDamage(damage);
			}
		}
	}

	ParticleEffectSettings settings = {};
	settings.maxParticleCount = 200;
	settings.spawnInterval = 0.001;
	settings.isLooping = true;

	// Start values
	settings.startValues.position = { 0, 0, 0 };
	settings.startValues.acceleration = { 0, 0, 0 };
	settings.startValues.lifetime = 1.0;
	settings.startValues.size = 0.4f * m_Radius / 10.0f;

	// End values
	settings.endValues.size = 0;
	settings.endValues.color.a = 1;

	// Randomize values
	settings.randPosition = { 0,0,0,0,0,0 };
	settings.randVelocity = { -m_Radius/sqrtf(3), m_Radius/sqrtf(3), -m_Radius / sqrtf(3), m_Radius / sqrtf(3), -m_Radius / sqrtf(3), m_Radius / sqrtf(3) };
	settings.randSize = { -0.2f * m_Radius / 10.0f, 0 };
	settings.randRotation = { 0, 2 * PI };
	settings.randRotationSpeed = { -PI / 2, PI / 2 };

	settings.texture = static_cast<Texture2DGUI*>(AssetLoader::Get()->LoadTexture2D(L"../Vendor/Resources/Textures/Particles/Explosion.png"));


	Entity*  particleEntity = SceneManager::GetInstance().GetActiveScene()->AddEntity("explosionParticle" + std::to_string(m_ParticleCounter++));
	component::TransformComponent* transform = particleEntity->AddComponent<component::TransformComponent>();
	float3 position = projectile->GetComponent<component::TransformComponent>()->GetTransform()->GetPositionFloat3();
	transform->GetTransform()->SetPosition(position.x, position.y, position.z);
	particleEntity->AddComponent<component::ParticleEmitterComponent>(&settings, true);
	particleEntity->GetComponent<component::ParticleEmitterComponent>()->OnInitScene();
	particleEntity->AddComponent<component::TemporaryLifeComponent>(1.0);

	settings.texture = static_cast<Texture2DGUI*>(AssetLoader::Get()->LoadTexture2D(L"../Vendor/Resources/Textures/Particles/player_magic.png"));
	particleEntity = SceneManager::GetInstance().GetActiveScene()->AddEntity("explosionMagicParticle" + std::to_string(m_ParticleCounter++));
	transform = particleEntity->AddComponent<component::TransformComponent>();
	transform->GetTransform()->SetPosition(position.x, position.y, position.z);
	particleEntity->AddComponent<component::ParticleEmitterComponent>(&settings, true);
	particleEntity->GetComponent<component::ParticleEmitterComponent>()->OnInitScene();
	particleEntity->AddComponent<component::TemporaryLifeComponent>(1.0);
}

void UpgradeExplosiveAttack::ApplyBoughtUpgrade()
{
}

std::string UpgradeExplosiveAttack::GetDescription(unsigned int level)
{
	return "Explosive Projectile: Projectile cause pieces of shrapnel to explode out from enemies causing " + std::to_string(static_cast<int>((0.4 + (0.1 * level))*100)) + "\% projectile damage over " + std::to_string(10 + (4 * (level - 1))) + " units radius";
}
