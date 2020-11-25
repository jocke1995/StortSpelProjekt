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

	// percentage of damage done to steal as life
	m_Radius = 3.0;
	m_PercentageDamage = 0.5;

	m_ImageName = "Explosion.png";
}

UpgradeExplosiveAttack::~UpgradeExplosiveAttack()
{
}

void UpgradeExplosiveAttack::IncreaseLevel()
{
	m_Level++;
	m_Radius = (5 * m_Level);
	m_PercentageDamage = 0.4 + (0.1 * m_Level);
	m_Price += m_StartingPrice;
}

void UpgradeExplosiveAttack::OnRangedHit(Entity* target)
{
	//Should only affect enemies. Only enemies should have AiComponent and therefore should be a good test
	if (target->HasComponent<component::AiComponent>())
	{
		// get damage from projectile
		int damage = m_pParentEntity->GetComponent<component::ProjectileComponent>()->GetDamage() * m_PercentageDamage;

		std::vector<Entity*>* enemies = EnemyFactory::GetInstance().GetAllEnemies();
		float3 hitTarget = target->GetComponent<component::TransformComponent>()->GetTransform()->GetPositionFloat3();
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
		settings.maxParticleCount = 20;
		settings.startValues.lifetime = 1.0;
		settings.spawnInterval = 0.001;
		settings.startValues.acceleration = { 0, 0, 0 };
		settings.isLooping = false;

		// Need to fix EngineRand.rand() for negative values
		RandomParameter3 randParam1 = { -m_Radius, m_Radius, -m_Radius, m_Radius, -m_Radius, m_Radius };

		settings.randPosition = { -1, 1, -1, 1, -1, 1 };
		settings.randVelocity = randParam1;
		settings.randSize = { 0.4, 0.8 };
		settings.randRotationSpeed = { 0, 3 };

		settings.texture = static_cast<Texture2DGUI*>(AssetLoader::Get()->LoadTexture2D(L"../Vendor/Resources/Textures/Particles/shrapnel_particle.png"));
		int particleCounter = 0;
		while (SceneManager::GetInstance().GetActiveScene()->EntityExists("explosionParticle" + std::to_string(particleCounter)))
		{
			particleCounter++;
		}
		Entity* particleEntity = SceneManager::GetInstance().GetActiveScene()->AddEntity("explosionParticle" + std::to_string(particleCounter));
		component::TransformComponent* transform = particleEntity->AddComponent<component::TransformComponent>();
		float3 position = target->GetComponent<component::TransformComponent>()->GetTransform()->GetPositionFloat3();
		transform->GetTransform()->SetPosition(position.x, position.y, position.z);
		particleEntity->AddComponent<component::ParticleEmitterComponent>(&settings, true);
		particleEntity->GetComponent<component::ParticleEmitterComponent>()->OnInitScene();
		particleEntity->AddComponent<component::TemporaryLifeComponent>(1.0);
	}
}

void UpgradeExplosiveAttack::ApplyBoughtUpgrade()
{
}

std::string UpgradeExplosiveAttack::GetDescription(unsigned int level)
{
	return "Explosive Projectile: Projectile cause pieces of shrapnel to explode out from enemies causing " + std::to_string(static_cast<int>((0.4 + (0.1 * level))*100)) + "\% projectile damage over " + std::to_string(5 * level) + " units radius";
}
