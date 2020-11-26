#include "ProjectileComponent.h"
#include "Engine.h"
#include "HealthComponent.h"
#include "UpgradeComponents/UpgradeComponent.h"

#include "Events/Events.h"
#include "../ECS/Entity.h"

#include "../ECS/Components/Audio3DEmitterComponent.h"
#include "../ECS/Components/TemporaryLifeComponent.h"

unsigned int component::ProjectileComponent::m_EffectCounter = 0;

component::ProjectileComponent::ProjectileComponent(Entity* parent, int damage, float ttl) : Component(parent)
{
	m_TimeToLive = ttl;
	m_CurrentDuration = 0.0f;
	m_Damage = damage;

	EventBus::GetInstance().Subscribe(this, &ProjectileComponent::hit);
}

component::ProjectileComponent::~ProjectileComponent()
{
}

void component::ProjectileComponent::Update(double dt)
{
	m_CurrentDuration += dt;

	if (m_CurrentDuration >= m_TimeToLive)
	{
		EventBus::GetInstance().Publish(&RemoveMe(m_pParent));
	}
}

void component::ProjectileComponent::OnInitScene()
{
}

void component::ProjectileComponent::OnUnInitScene()
{
	EventBus::GetInstance().Unsubscribe(this, &ProjectileComponent::hit);
}

int component::ProjectileComponent::GetDamage() const
{
	return m_Damage;
}

int component::ProjectileComponent::GetTimeToLive() const
{
	return m_TimeToLive;
}

void component::ProjectileComponent::hit(Collision* event)
{
	// if we are the one that collided then make 
	// the other object lose health (if it has health)
	if (event->ent1 == m_pParent || event->ent2 == m_pParent)
	{
		Entity* other = nullptr;
		if (event->ent1 == m_pParent)
		{
			other = event->ent2;
		}
		else
		{
			other = event->ent1;
		}

		if (other->HasComponent<component::HealthComponent>())
		{
			createProjectileParticleEffectAt(m_pParent);
			other->GetComponent<component::HealthComponent>()->ChangeHealth(-m_Damage);
			if (other->GetName().find("enemy") != std::string::npos && other->GetComponent<component::Audio3DEmitterComponent>())
			{
				other->GetComponent<component::Audio3DEmitterComponent>()->UpdateEmitter(L"Bruh");
				other->GetComponent<component::Audio3DEmitterComponent>()->Play(L"Bruh");
				EventBus::GetInstance().Publish(&RemoveMe(m_pParent));
			}
		}
		else if (m_pParent->HasComponent<CollisionComponent>())
		{
			if (m_pParent->GetComponent<component::CollisionComponent>()->GetRestitution() <= EPSILON)
			{
				EventBus::GetInstance().Publish(&RemoveMe(m_pParent));
			}
		}
		// Call on upgrade on hit functions
		if (m_pParent->HasComponent<component::UpgradeComponent>())
		{
			m_pParent->GetComponent<component::UpgradeComponent>()->OnHit(other);
			m_pParent->GetComponent<component::UpgradeComponent>()->OnRangedHit(other, m_pParent);
		}
	}
}

void component::ProjectileComponent::createProjectileParticleEffectAt(Entity* target) const
{
	float duration = 0.2;

	// Create test particleEffect
	ParticleEffectSettings settings = {};
	settings.maxParticleCount = 1;
	settings.startValues.lifetime = duration;
	settings.spawnInterval = 0.00000001;
	settings.startValues.acceleration = { 0, 0, 0 };
	settings.startValues.size = 3.5;
	settings.isLooping = false;

	// Need to fix EngineRand.rand() for negative values

	settings.randPosition = { 0, 0, 0, 0, 0, 0 };
	settings.randVelocity = { 0, 0, 0, 0, 0, 0 };
	settings.randSize = { 0, 0 };
	settings.randRotation = { 0, 2*PI };
	settings.randRotationSpeed = { 0.0, 0.2 };

	AssetLoader* al = AssetLoader::Get();
	Texture2DGUI* particleTexture = static_cast<Texture2DGUI*>(al->LoadTexture2D(L"../Vendor/Resources/Textures/Particles/projParticle.png"));
	settings.texture = particleTexture;

	SceneManager& sm = SceneManager::GetInstance();
	
	Entity* entity = sm.GetActiveScene()->AddEntity("projectileHitEffect_" + std::to_string(m_EffectCounter++));;

	component::TransformComponent* tc = entity->AddComponent<component::TransformComponent>();
	DirectX::XMFLOAT3 targetPos = target->GetComponent<component::TransformComponent>()->GetTransform()->GetPositionXMFLOAT3();
	tc->GetTransform()->SetPosition(targetPos);

	entity->AddComponent<component::ParticleEmitterComponent>(&settings, true);
	entity->AddComponent<component::TemporaryLifeComponent>(duration);

	sm.AddEntity(entity, sm.GetActiveScene());
	
}
