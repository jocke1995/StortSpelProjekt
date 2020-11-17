#include "HealthComponent.h"
#include "../Events/EventBus.h"
#include "../Events/Events.h"
#include "ECS/Entity.h"
#include "UpgradeComponents/UpgradeComponent.h"
#include "Player.h"
#include "PoisonDebuff.h"
#include "Misc/AssetLoader.h"
#include "Particles/ParticleSystem.h"
#include "Renderer/Texture/Texture2DGUI.h"

component::PoisonDebuff::PoisonDebuff(Entity* parent, int damagePerTick, int ticks, double tickDuration, float percentageSlow) : Component(parent)
{
	ParticleEffectSettings settings = {};
	settings.particleCount = 50;
	settings.startValues.lifetime = 0.5;
	settings.spawnInterval = settings.startValues.lifetime / settings.particleCount;
	settings.startValues.acceleration = { 0, -2, 0 };

	// Need to fix EngineRand.rand() for negative values
	RandomParameter3 randParam1 = { -2, 2, 1, 4, -2, 2 };

	settings.randPosition = { -1, 1, -1, 1, -1, 1 };
	settings.randVelocity = randParam1;
	settings.randSize = { 0.2, 1 };
	settings.randRotationSpeed = { 0, 3 };

	Texture2DGUI* particleTexture = static_cast<Texture2DGUI*>(AssetLoader::Get()->LoadTexture2D(L"../Vendor/Resources/Textures/Particles/poison_particle.png"));
	parent->AddComponent<component::ParticleEmitterComponent>(particleTexture, &settings, true);
	parent->GetComponent<component::ParticleEmitterComponent>()->OnInitScene();
	Reset(damagePerTick, ticks, tickDuration, percentageSlow);
}

component::PoisonDebuff::~PoisonDebuff()
{
}

void component::PoisonDebuff::OnInitScene()
{
}

void component::PoisonDebuff::OnUnInitScene()
{
}

void component::PoisonDebuff::Reset(int damagePerTick, int ticks, double tickDuration, float percentageSlow)
{
	m_Damage = damagePerTick;
	m_Ticks = ticks;
	m_TickInterval = tickDuration;
	Transform* transform = m_pParent->GetComponent<component::TransformComponent>()->GetTransform();
	m_Slow = transform->GetVelocity() * percentageSlow;
	transform->SetVelocity(transform->GetVelocity() - m_Slow);
	m_TickTimer = 0.0;
	m_pParent->GetComponent<component::ParticleEmitterComponent>()->Play();
}

void component::PoisonDebuff::Update(double dt)
{
	if (m_Ticks > 0)
	{
		m_TickTimer += dt;
		if (m_TickTimer >= m_TickInterval)
		{
			tick();
		}
	}
}

void component::PoisonDebuff::tick()
{
	m_TickTimer -= m_TickInterval;
	m_Ticks--;
	m_pParent->GetComponent<component::HealthComponent>()->TakeDamage(m_Damage);
	if (m_Ticks == 0)
	{
		Transform* transform = m_pParent->GetComponent<component::TransformComponent>()->GetTransform();
		transform->SetVelocity(transform->GetVelocity() + m_Slow);
		m_pParent->GetComponent<component::ParticleEmitterComponent>()->Stop();
	}
}
