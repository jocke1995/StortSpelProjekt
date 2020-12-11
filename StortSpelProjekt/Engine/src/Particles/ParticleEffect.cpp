#include "stdafx.h"
#include "ParticleEffect.h"

#include "../Misc/AssetLoader.h"

#include "../Renderer/Renderer.h"
#include "../Renderer/DescriptorHeap.h"

#include "../Renderer/GPUMemory/Resource.h"
#include "../Renderer/GPUMemory/ShaderResourceView.h"
#include "../Renderer/GPUMemory/UnorderedAccessView.h"

#include "../Renderer/Texture/Texture2DGUI.h"
#include "../Renderer/Model.h"
#include "../Renderer/Mesh.h"

#include "../ECS/Components/ParticleEmitterComponent.h"
#include "../ECS/Components/TransformComponent.h"
#include "../ECS/Entity.h"

#include <algorithm>

EngineRand ParticleEffect::rand = {5};

ParticleEffect::ParticleEffect(Entity* parent, ParticleEffectSettings* settings)
{
	m_pEntity = parent;
	m_Settings = *settings;
	m_Settings.startValues.color.normalize();

	m_pTexture = settings->texture;

	if (m_pTexture == nullptr)
	{
		// Set default texture
		AssetLoader* al = AssetLoader::Get();
		m_pTexture = static_cast<Texture2DGUI*>(al->LoadTexture2D(L"../Vendor/Resources/Textures/2DGUI/stefanHuvud.png"));

		Log::PrintSeverity(Log::Severity::WARNING, "ParticleEffect::Texture was nullptr\n");
	}

	init();
}

ParticleEffect::~ParticleEffect()
{
	// Might be temporary, might not
	Renderer::GetInstance().waitForGPU();

	delete m_pUploadResource;
	delete m_pDefaultResource;
	delete m_pSRV;
}

void ParticleEffect::Update(double dt)
{
	m_TimeSinceSpawn += dt;

	// If should particle spawn
	while (isTimeToSpawnParticles() && m_IsSpawning)
	{
		bool spawned = spawnParticle();

		if (!spawned)
		{
			m_TimeSinceSpawn = 0;
			break;
		}
	}

	// Update all particles
	for (Particle& particle : m_Particles)
	{
		// Only update if alive
		if (particle.IsAlive())
		{
			particle.Update(dt, m_SizeChangePerFrame, m_ColorChangePerFrame);
		}
	}
}

void ParticleEffect::SetIsSpawning(bool value)
{
	m_IsSpawning = value;
}

void ParticleEffect::Clear()
{
	m_ParticleIndex = 0;

	for (Particle& particle : m_Particles)
	{
		particle.m_Attributes.lifetime = -1;
	}
}

Texture2DGUI* ParticleEffect::GetTexture() const
{
	return m_pTexture;
}

bool ParticleEffect::isTimeToSpawnParticles() const
{
	return m_TimeSinceSpawn >= m_Settings.spawnInterval;
}

bool ParticleEffect::spawnParticle()
{
	// m_ParticleIndex is always at the oldest particle first
	Particle& particle = m_Particles.at(m_ParticleIndex);

	// If particle is alive/can't spawn particle
	if (particle.IsAlive())
	{
		return false;
	}

	if (!m_Settings.isLooping)
	{
		if (m_ParticleIndex + 1 == m_Particles.size())
		{
			SetIsSpawning(false);
		}
	}

	// Update ParticleIndex
	m_ParticleIndex = ++m_ParticleIndex % m_Settings.maxParticleCount;

	// "Spawn"
	m_TimeSinceSpawn -= m_Settings.spawnInterval;
	initParticle(particle);

	return true;
}

void ParticleEffect::init()
{
	Renderer& renderer = Renderer::GetInstance();

	DescriptorHeap* descriptorHeap = renderer.getCBVSRVUAVdHeap();

	// Only send position (float3) + size (float) to gpu
	size_t entrySize = sizeof(PARTICLE_DATA);
	unsigned long long resourceByteSize = entrySize * m_Settings.maxParticleCount;

	// used to format a debug string
	std::wstring a = L"ParticleEffect_";
	static unsigned int particleCounter = 0;
	std::wstring b = std::to_wstring(particleCounter++);
	std::wstring c = L"_UPLOAD";
	std::wstring d = L"_DEFAULT";

	// Resources
	m_pUploadResource = new Resource(renderer.m_pDevice5, resourceByteSize, RESOURCE_TYPE::UPLOAD, a + b + c);
	m_pDefaultResource = new Resource(renderer.m_pDevice5, resourceByteSize, RESOURCE_TYPE::DEFAULT, a + b + d);

	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.Format = DXGI_FORMAT_UNKNOWN;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_BUFFER;
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.Buffer.FirstElement = 0;
	srvDesc.Buffer.NumElements = m_Settings.maxParticleCount;
	srvDesc.Buffer.StructureByteStride = entrySize;
	srvDesc.Buffer.Flags = D3D12_BUFFER_SRV_FLAG_NONE;

	m_pSRV = new ShaderResourceView(renderer.m_pDevice5, descriptorHeap, &srvDesc, m_pDefaultResource);

	m_Particles.resize(m_Settings.maxParticleCount);
	m_ParticlesData.resize(m_Settings.maxParticleCount);
}

void ParticleEffect::initParticle(Particle& particle)
{
	// Setup interpolation values for byLifetime
	m_SizeChangePerFrame = (m_Settings.startValues.size - m_Settings.endValues.size) / m_Settings.startValues.lifetime;
	m_ColorChangePerFrame = (m_Settings.startValues.color - m_Settings.endValues.color) / m_Settings.startValues.lifetime;

	// Set start values
	particle.initValues(&m_Settings.startValues);

	// Add the random parameters
	particle.m_Attributes.position += randomizeParameter(m_Settings.randPosition);
	particle.m_Attributes.velocity += randomizeParameter(m_Settings.randVelocity);
	particle.m_Attributes.size += randomizeParameter(m_Settings.randSize);
	particle.m_Attributes.rotation += randomizeParameter(m_Settings.randRotation);
	particle.m_Attributes.rotationSpeed += randomizeParameter(m_Settings.randRotationSpeed);
	particle.m_Attributes.lifetime += randomizeParameter(m_Settings.randLifetime);

	// Add Entity transform position to start position.
	component::TransformComponent* tc = m_pEntity->GetComponent<component::TransformComponent>();

	if (tc != nullptr)
	{
		particle.m_Attributes.position = particle.m_Attributes.position + tc->GetTransform()->GetRenderPositionFloat3();
	}
}

float3 ParticleEffect::randomizeParameter(RandomParameter3& param)
{
	float3 randFloat = { 0,0,0 };
	if (!(param.x.intervalLower == 0 && param.x.intervalUpper == 0))
	{
		randFloat.x = randomizeFloat(param.x.intervalLower, param.x.intervalUpper);
	}

	if (!(param.y.intervalLower == 0 && param.y.intervalUpper == 0))
	{
		randFloat.y = randomizeFloat(param.y.intervalLower, param.y.intervalUpper);
	}

	if (param.z.intervalLower != 0 && param.z.intervalUpper != 0)
	{
		randFloat.z = randomizeFloat(param.z.intervalLower, param.z.intervalUpper);
	}

	return randFloat;
}

float ParticleEffect::randomizeParameter(RandomParameter& param)
{
	float randFloat = 0;
	if (!(param.intervalLower == 0 && param.intervalUpper == 0))
	{
		randFloat = randomizeFloat(param.intervalLower, param.intervalUpper);
	}

	return randFloat;
}

float ParticleEffect::randomizeFloat(float lower, float upper) const
{
	// rand.Randf can't handle same numbers
	if (lower == upper)
	{
		return lower;
	}
	const float increaseRandomness = 100;
	float r = rand.Randf(floor(lower * increaseRandomness), ceil(upper * increaseRandomness)) / increaseRandomness;
	return r;
}

void ParticleEffect::updateResourceData(float3 cameraPos)
{
	PARTICLE_DATA tempData;

	unsigned int index = 0;
	for (Particle& p : m_Particles)
	{
#if !PARTICLESYSTEM_RENDER_DEAD_PARTICLES
		if (!p.IsAlive())
		{
			p.m_Attributes.size = 0;
		}
#endif // PARTICLESYSTEM_RENDER_DEAD_PARTICLES

		// Don't know if this should be here, but do not render sizes that are < 0, normal will just be flipped if rendered.
		if (p.m_Attributes.size < 0)
		{
			p.m_Attributes.size = 0;
		}

		tempData = {
			p.m_Attributes.position.x, p.m_Attributes.position.y, p.m_Attributes.position.z, p.m_Attributes.size,
			p.m_Attributes.color.r, p.m_Attributes.color.g, p.m_Attributes.color.b, p.m_Attributes.color.a,
			p.m_Attributes.rotation
		};
		
		m_ParticlesData[index++] = tempData;
	}

	// Sort z from camera for blend
	std::sort(m_ParticlesData.begin(), m_ParticlesData.end(),
		[&](const PARTICLE_DATA& i, const PARTICLE_DATA& j) -> bool 
	{
		float distFromCamI = (i.position - cameraPos).length();
		float distFromCamJ = (j.position - cameraPos).length();

		return distFromCamI > distFromCamJ;
	});
}